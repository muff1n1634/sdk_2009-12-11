#include <revolution/cx/CXUncompression.h>

/*******************************************************************************
 * headers
 */

#include <decomp.h> // unk_t
#include <macros.h>

#include <revolution/types.h>

#include <revolution/cx.h>
#include "CXInternal.h"

#if 0
#include <revolution/os/OSAssert.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * types
 */

struct BitReader
{
	byte_t			const *at_0x00;	// size 0x04, offset 0x00
	unk4_t			at_0x04;		// size 0x04, offset 0x04
	unk4_t unsigned	at_0x08;		// size 0x04, offset 0x08
	unk4_t			at_0x0c;		// size 0x04, offset 0x0c
}; // size 0x10?

struct RCInfo
{
	unk4_t unsigned	*at_0x00;	// size 0x04, offset 0x00
	unk4_t unsigned	*at_0x04;	// size 0x04, offset 0x04
	unk4_t unsigned	at_0x08;	// size 0x04, offset 0x08
	unk1_t unsigned	at_0x0c;	// size 0x04, offset 0x0c
}; // size 0x10?

struct RCState
{
	unk4_t			at_0x00;	// size 0x04, offset 0x00
	unk4_t unsigned	at_0x04;	// size 0x04, offset 0x04
	unk4_t			at_0x08;	// size 0x04, offset 0x08
	unk1_t			at_0x0c;	// size 0x04, offset 0x0c
	byte_t pad0_[3]; // alignment?
	unk4_t			at_0x10;	// size 0x04, offset 0x10
}; // size 0x14?

/*******************************************************************************
 * local function declarations
 */

static int CXGetCompressionType(void const *compressed);

static unk_t CXiHuffImportTree(u16 *tree, byte_t const *stream, u8);

// only reasonable way i see an init function come after a read function
static inline void BitReader_Init(struct BitReader *bitReader,
                                  byte_t const *stream);
static byte_t BitReader_Read(struct BitReader *bitReader);

static inline void RCInitInfo_(struct RCInfo *rcInfo, unk1_t unsigned,
                               unk4_t unsigned *);
static inline void RCInitState_(struct RCState *rcState);
static void RCAddCount_(struct RCInfo *rcInfo, u16);
static u16 RCSearch_(struct RCInfo *rcInfo, unk_t, unk_t unsigned, unk_t);
static u16 RCGetData_(byte_t const *stream, struct RCInfo *rcInfo,
                        struct RCState *rcState, unk_t *);

/*******************************************************************************
 * functions
 */

u32 CXGetUncompressedSize(void const *compressed)
{
	u32 size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, compressed, 0)) >> 8;

	if (!size)
		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, compressed, 4));

	return size;
}

void CXUncompressAny(void const *compressed, void *uncompressed)
{
	switch (CXGetCompressionType(compressed))
	{
	case CX_COMPRESSION_TYPE_RUN_LENGTH:
		CXUncompressRL(compressed, uncompressed);
		break;

	case CX_COMPRESSION_TYPE_LEMPEL_ZIV:
		CXUncompressLZ(compressed, uncompressed);
		break;

	case CX_COMPRESSION_TYPE_HUFFMAN:
		CXUncompressHuffman(compressed, uncompressed);
		break;

	case CX_COMPRESSION_TYPE_FILTER_DIFF:
		CXUnfilterDiff(compressed, uncompressed);
		break;

	default:
		OSAssertMessage_Line(81, false, "Unknown compressed format");
	}
}

void CXUncompressRL(void const *compressed, void *uncompressed)
{
	byte_t const *src = compressed;
	byte_t *dst = uncompressed;

	u32 size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;
	src += sizeof(byte4_t);

	if (!size)
	{
		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0));
		src += sizeof(byte4_t);
	}

	while (size)
	{
		byte_t byte = *src++;

		u32 runLength = byte & 0x7f;

		if (!(byte & 0x80))
		{
			runLength += 1;

			if (runLength > size)
				runLength = size;

			size -= runLength;

			do
				*dst++ = *src++;
			while (--runLength);
		}
		else
		{
			runLength += 3;

			if (runLength > size)
				runLength = size;

			size -= runLength;

			byte_t byte = *src++;

			do
				*dst++ = byte;
			while (--runLength);
		}
	}
}

void CXUncompressLZ(void const *compressed, void *uncompressed)
{
	byte_t const *src = compressed;
	byte_t *dst = uncompressed;

	u32 size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;
	BOOL stat = BOOLIFY_TERNARY(IN_BUFFER_AT(byte1_t, src, 0) & 0x0f);
	src += sizeof(byte4_t);

	if (!size)
	{
		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0));
		src += sizeof(byte4_t);
	}

	while (size)
	{
		u32 i;
		byte4_t flags = *src++;

		for (i = 0; i < 8; ++i)
		{
			if (!(flags & 0x80))
			{
				*dst++ = *src++;
				size--;
			}
			else
			{
				s32 length = *src >> 4;

				if (!stat)
				{
					length += 3;
				}
				else
				{
					if (length == 0x01)
					{
						length = (*src++ & 0x0f) << 12;
						length |= *src++ << 4;
						length |= *src >> 4;
						length += 0x111;
					}
					else if (length == 0x00)
					{
						length = (*src++ & 0x0f) << 4;
						length |= *src >> 4;
						length += 0x11;
					}
					else
					{
						length += 0x01;
					}
				}

				s32 offset = (*src++ & 0x0f) << 8;
				offset = (offset | *src++) + 1;

				if (length > size)
					length = size;

				size -= length;

				// clang-format off
				do
				{
#if defined(__MWERKS__)
					// ERRATUM: Unsequenced modification and access to dst
					/* Here, the compiler has decided to evaluate the left hand
					 * side dst first, then the right hand side dst, then the
					 * side effect of the post-increment on the left hand side
					 * dst.
					 */
					*dst++ = dst[-offset];
#else
					// equivalent defined version
					*dst = dst[-offset];
					++dst;
#endif
				} while (--length > 0);
				// clang-format on
			}

			if (!size)
				break;

			flags <<= 1;
		}
	}
}

void CXUncompressHuffman(void const *compressed, void *uncompressed)
{
	byte_t const *src = compressed;
	byte_t *dst = uncompressed;

	s32 size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;

	byte_t const *base = size ? src + 4 : src + 8;
	byte_t const *basep1 = base + 1;

	unk_t a = *src & 0x0f;
	unk_t unsigned b = 0;
	unk_t unsigned c = 0;
	unk_t d = (a & 0x07) + 4;

	if (!size)
		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 4));

	src = base + ((*base + 1) << 1);
	base = basep1;

	while (size > 0)
	{
		int i = 32;

		byte4_t f;
#if defined(__MWERKS__)
		// NOTE: assignment to lvalue cast is a CW extension
		f = CXiConvertEndian32_(*((byte4_t *)(src))++);
#else
		f = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0));
		src += sizeof(byte4_t);
#endif

		while (--i >= 0)
		{
			unk_t g = f >> 31;
			unk_t h = *base;
			h <<= g;

			// ok
			base = (byte_t const *)((((u32)(base) >> 1) << 1)
			                        + (((*base & 0x3f) + 1) << 1) + g);

			if (h & 0x80)
			{
				b >>= a;
				b |= *base << (32 - a);
				base = basep1;

				if (++c == d)
				{
#if defined(__MWERKS__)
					// NOTE: assignment to lvalue cast is a CW extension
					*((byte4_t *)(dst))++ = CXiConvertEndian32_(b);
#else
					*(byte4_t *)dst = CXiConvertEndian32_(b);
					dst += sizeof(byte4_t);
#endif
					size -= sizeof(byte4_t);
					c = 0;
				}
			}

			if (size <= 0)
				break;

			f <<= 1;
		}
	}
}

static unk_t CXiHuffImportTree(u16 *tree, byte_t const *stream, u8 param_3)
{
	unk_t a;
	unk_t b;
	unk_t unsigned c;
	unk_t unsigned d;
	unk_t e;
	unk_t unsigned f;
	unk_t unsigned g;

	a = 1;
	c = 0;

	d = 0;
	e = (1 << param_3) - 1;

	f = 0;
	g = (1 << param_3) << 1;

	if (param_3 > 8)
	{
		b = CXiConvertEndian16_(IN_BUFFER_AT(byte2_t, stream, 0));

		stream += sizeof(byte2_t);
		f += sizeof(byte2_t);
	}
	else
	{
		b = IN_BUFFER_AT(byte1_t, stream, 0);

		stream += sizeof(byte1_t);
		f += sizeof(byte1_t);
	}

	b = (b + 1) << 2;

	while (f < b)
	{
		while (d < param_3)
		{
			c <<= 8;
			c |= *stream++;
			++f;
			d += 8;
		}

		if (a < g)
			tree[a++] = e & (c >> (d - param_3));

		d -= param_3;
	}

	(void)stream;

	return b;
}

static void BitReader_Init(struct BitReader *bitReader, byte_t const *stream)
{
	bitReader->at_0x00	= stream;
	bitReader->at_0x04	= 0;
	bitReader->at_0x08	= 0;
	bitReader->at_0x0c	= 0;
}

static byte_t BitReader_Read(struct BitReader *bitReader)
{
	if (!bitReader->at_0x0c)
	{
		bitReader->at_0x08 = bitReader->at_0x00[bitReader->at_0x04++];
		bitReader->at_0x0c = 8;
	}

	byte_t a = (bitReader->at_0x08 >> (bitReader->at_0x0c - 1)) & 0x01;
	--bitReader->at_0x0c;

	return a;
}

void CXUncompressLH(void const *compressed, byte_t *uncompressed, u16 *param_3)
{
	u32 size;
	unk_t a = 0;
	byte_t const *src = compressed;
	u16 *b = param_3;
	u16 *c = param_3 + 0x400;

	// size
	size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;
	src += sizeof(byte4_t);

	if (!size)
	{
		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0));
		src += sizeof(byte4_t);
	}

	src += CXiHuffImportTree(b, src, 9);
	src += CXiHuffImportTree(c, src, 5);

	struct BitReader bitReader;
	BitReader_Init(&bitReader, src);

	while (a < size)
	{
		u16 d;
		u16 *e = b + 1;

		while (true)
		{
			byte_t f = BitReader_Read(&bitReader);
			unk_t g = (((*e & 0x7f) + 1) << 1) + f;

			if (*e & (0x100 >> f))
			{
				e = ROUND_DOWN_PTR(e, 4);
				d = e[g];
				break;
			}
			else
			{
				e = ROUND_DOWN_PTR(e, 4);
				e += g;
			}
		}

		if (d < 0x100)
		{
			uncompressed[a++] = d;
			continue;
		}

		u16 h;
		u16 j = (d & 0xff) + 3;
		u16 *k = c + 1;

		while (true)
		{
			byte_t l = BitReader_Read(&bitReader);
			u32 m = (((*k & 0x07) + 1) << 1) + l;

			if (*k & (0x10 >> l))
			{
				k = ROUND_DOWN_PTR(k, 4);
				d = k[m];
				break;
			}
			else
			{
				k = ROUND_DOWN_PTR(k, 4);
				k += m;
			}
		}

		h = d;
		d = 0;

		if (h)
		{
			d = 1;

			while (--h)
			{
				d <<= 1;
				d |= BitReader_Read(&bitReader);
			}
		}

#if !defined(NDEBUG) // TODO: What
		d = d + 1;
#else
		++d;
#endif

		if (a + j > size)
			j = size - a;

		while (j--)
		{
			uncompressed[a] = uncompressed[a - d];
			++a;
		}
	}
}

static void RCInitInfo_(struct RCInfo *rcInfo, unk1_t unsigned param_2,
                        unk4_t unsigned *param_3)
{
	u32 i;
	unk_t a = 1 << param_2;

	rcInfo->at_0x0c = param_2;
	rcInfo->at_0x00 = param_3;
	rcInfo->at_0x04 = param_3 + a;

	for (i = 0; i < a; ++i)
	{
		rcInfo->at_0x00[i] = 1;
		rcInfo->at_0x04[i] = i;
	}

	rcInfo->at_0x08 = a;
}

static void RCInitState_(struct RCState *rcState)
{
	rcState->at_0x00 = 0;
	rcState->at_0x04 = 0x80000000;
	rcState->at_0x08 = 0;
	rcState->at_0x0c = 0;
	rcState->at_0x10 = 0;
}

static void RCAddCount_(struct RCInfo *rcInfo, u16 param_2)
{
	u32 i;
	unk_t unsigned a = 1 << rcInfo->at_0x0c;

	++rcInfo->at_0x00[param_2];
	++rcInfo->at_0x08;

	for (i = param_2 + 1; i < a; ++i)
		++rcInfo->at_0x04[i];

	if (rcInfo->at_0x08 < 0x10000)
		return;

	if (*rcInfo->at_0x00 > 1)
		*rcInfo->at_0x00 >>= 1;

	*rcInfo->at_0x04 = 0;
	rcInfo->at_0x08 = *rcInfo->at_0x00;

	for (i = 1; i < a; ++i)
	{
		if (rcInfo->at_0x00[i] > 1)
			rcInfo->at_0x00[i] >>= 1;

		rcInfo->at_0x04[i] = rcInfo->at_0x04[i - 1] + rcInfo->at_0x00[i - 1];
		rcInfo->at_0x08 += rcInfo->at_0x00[i];
	}
}

static u16 RCSearch_(struct RCInfo *rcInfo, unk_t param_2,
                     unk_t unsigned param_3, unk_t param_4)
{
	unk_t a = 1 << rcInfo->at_0x0c;
	unk_t b = param_2 - param_4;
	unk_t unsigned c = param_3 / rcInfo->at_0x08;
	unk_t unsigned d = b / c;
	unk_t unsigned e = 0;
	unk_t f = a - 1;

	unk_t g;
	while (e < f)
	{
		g = (e + f) >> 1;

		if (rcInfo->at_0x04[g] > d)
			f = g;
		else
			e = g + 1;
	}

	g = e;
	while (rcInfo->at_0x04[g] > d)
		--g;

	return g;
}

static u16 RCGetData_(byte_t const *stream, struct RCInfo *rcInfo,
                      struct RCState *rcState, unk_t *param_4)
{
	u16 a = RCSearch_(rcInfo, rcState->at_0x08, rcState->at_0x04, rcState->at_0x00);
	unk_t b = 0;

	// arbitrary block
	{
		unk_t c = rcState->at_0x04 / rcInfo->at_0x08;

		rcState->at_0x00 += c * rcInfo->at_0x04[a];
		rcState->at_0x04 = c * rcInfo->at_0x00[a];

		RCAddCount_(rcInfo, a);

		while (rcState->at_0x04 < 0x1000000)
		{
			rcState->at_0x08 <<= 8;
			rcState->at_0x08 += stream[b++];
			rcState->at_0x04 <<= 8;
			rcState->at_0x00 <<= 8;
		}
	}

	*param_4 = b;
	return a;
}

void CXUncompressLRC(void const *compressed, byte_t *uncompressed,
                     unk_t unsigned *param_3)
{
	byte_t const *src = compressed;
	unk_t unsigned a = 0;
	u32 size = 0;

	struct RCInfo info1;
	RCInitInfo_(&info1, 9, param_3);

	struct RCInfo info2;
#if !defined(NDEBUG) // What
	RCInitInfo_(&info2, 12, param_3 + 0x400);
#else
	RCInitInfo_(&info2, 12, param_3 += 0x400);
#endif

	struct RCState state;
	RCInitState_(&state);

	// size
	size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;
	src += sizeof(byte4_t);

	if (!size)
	{
		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0));
		src += sizeof(byte4_t);
	}

	// What
	state.at_0x08 = src[0] << 24 | src[1] << 16 | src[2] << 8 | src[3];
	src += sizeof(byte_t) * 4;

	while (a < size)
	{
		unk_t b;

		// why is recast necessary?
		u16 c = (u16)RCGetData_(src, &info1, &state, &b);
		src += b;

		if (c < 0x100)
		{
			uncompressed[a++] = c;
			continue;
		}

		u16 d = (c & 0xff) + 3;

		c = RCGetData_(src, &info2, &state, &b) + 1;
		src += b;

		if (a + d > size)
			break;

		if (a < c)
			return;

		while (d--)
		{
			uncompressed[a] = uncompressed[a - c];
			++a;
		}
	}
}

void CXUnfilterDiff(void const *compressed, void *uncompressed)
{
	byte_t const *src = compressed;
	byte_t *dst = uncompressed;

	u32 stat = IN_BUFFER_AT(byte1_t, src, 0) & 0x0f;
	s32 size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;
	u32 sum = 0;

	src += sizeof(u32);

	if (stat != 0x01)
	{
		do
		{
			byte1_t num = *src++;

			size -= sizeof(byte1_t);
			sum += num;

			*dst++ = sum;
		} while (size > 0);
	}
	else
	{
		do
		{
			byte2_t num = CXiConvertEndian16_(*(byte2_t *)src);
			src += sizeof(byte2_t);

			size -= sizeof(byte2_t);
			sum += num;

			*(byte2_t *)dst = CXiConvertEndian16_(sum);
			dst += sizeof(byte2_t);
		} while (size > 0);
	}
}

CXCompressionHeader CXGetCompressionHeader(void const *compressed)
{
	CXCompressionHeader header;
	header.type = (IN_BUFFER_AT(byte1_t, compressed, 0) & 0xf0) >> 4;
	header.stat = (IN_BUFFER_AT(byte1_t, compressed, 0) & 0x0f);
	header.size =
		CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, compressed, 0)) >> 8;

	if (!header.size)
	{
		header.size =
			CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, compressed, 4));
	}

	return header;
}
