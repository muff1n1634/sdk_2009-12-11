#include <revolution/cx/CXSecureUncompression.h>

/*******************************************************************************
 * headers
 */

#include <decomp.h> // unk_t
#include <macros.h>

#include <revolution/types.h>

#include <revolution/cx.h>
#include "CXInternal.h"

#include <revolution/cx/CXUncompression.h>

/*******************************************************************************
 * types
 */

struct BitReader
{
	byte_t			const *at_0x00;	// size 0x04, offset 0x00
	unk4_t			at_0x04;		// size 0x04, offset 0x04
	unk4_t unsigned	at_0x08;		// size 0x04, offset 0x08
	unk4_t			at_0x0c;		// size 0x04, offset 0x0c
	unk4_t			at_0x10;		// size 0x04, offset 0x0c
}; // size 0x14?

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

static unk_t CXiHuffImportTree(u16 *tree, byte_t const *stream, u8,
                               unk_t unsigned);

// only reasonable way i see an init function come after a read function
static inline void BitReader_Init(struct BitReader *bitReader,
                                  byte_t const *stream, unk_t);
// why is it signed char now
static signed char BitReader_Read(struct BitReader *bitReader);

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

CXSecureResult CXSecureUncompressAny(void const *compressed, u32 length,
                                     void *uncompressed)
{
	switch (CXGetCompressionType(compressed))
	{
	case CX_COMPRESSION_TYPE_RUN_LENGTH:
		return CXSecureUncompressRL(compressed, length, uncompressed);
		break;

	case CX_COMPRESSION_TYPE_LEMPEL_ZIV:
		return CXSecureUncompressLZ(compressed, length, uncompressed);
		break;

	case CX_COMPRESSION_TYPE_HUFFMAN:
		return CXSecureUncompressHuffman(compressed, length, uncompressed);
		break;

	case CX_COMPRESSION_TYPE_FILTER_DIFF:
		return CXSecureUnfilterDiff(compressed, length, uncompressed);
		break;

	default:
		return CXSECURE_EBADTYPE;
	}
}

CXSecureResult CXSecureUncompressRL(void const *compressed, u32 length,
                                    void *uncompressed)
{
	byte_t const *src = compressed;
	byte_t *dst = uncompressed;

	byte1_t secstat = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0));

	u32 size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;
	s32 remainingLength = length;

	if ((secstat & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_TYPE_RUN_LENGTH)
		return CXSECURE_EBADTYPE;

	if ((secstat & 0x0f) != 0)
		return CXSECURE_EBADTYPE;

	if (length <= 4)
		return CXSECURE_E2SMALL;

	src += sizeof(byte4_t);
	remainingLength -= sizeof(byte4_t);

	if (!size)
	{
		if (remainingLength < 4)
			return CXSECURE_E2SMALL;

		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0));
		src += sizeof(byte4_t);
		remainingLength -= sizeof(byte4_t);
	}

	while (size)
	{
		byte_t byte = *src++;

		s32 runLength = byte & 0x7f;

		--remainingLength;
		if (remainingLength < 0)
			return CXSECURE_E2SMALL;

		if (!(byte & 0x80))
		{
			runLength += 1;

			if (runLength > size)
				return CXSECURE_EBADSIZE;

			remainingLength -= runLength;
			if (remainingLength < 0)
				return CXSECURE_E2SMALL;

			size -= runLength;

			do
				*dst++ = *src++;
			while (--runLength > 0);
		}
		else
		{
			runLength += 3;

			if (runLength > size)
				return CXSECURE_EBADSIZE;

			size -= runLength;
			byte_t byte = *src++;

			remainingLength -= 1;
			if (remainingLength < 0)
				return CXSECURE_E2SMALL;

			do
				*dst++ = byte;
			while (--runLength > 0);
		}
	}

	if (remainingLength > 0x20)
		return CXSECURE_E2BIG;

	(void)src;
	(void)src;

	return CXSECURE_ESUCCESS;
}

CXSecureResult CXSecureUncompressLZ(void const *compressed, u32 length,
                                    void *uncompressed)
{
	byte_t const *src = compressed;
	byte_t *dst = uncompressed;

	u8 secstat = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) & 0xff;
	u32 size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;
	s32 remainingLength = length;

	BOOL stat = BOOLIFY_TERNARY(IN_BUFFER_AT(byte1_t, src, 0) & 0x0f);

	if ((secstat & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_TYPE_LEMPEL_ZIV)
		return CXSECURE_EBADTYPE;

	if ((secstat & 0x0f) != 0 && (secstat & 0x0f) != 1)
		return CXSECURE_EBADTYPE;

	if (length <= 4)
		return CXSECURE_E2SMALL;

	src += sizeof(byte4_t);
	remainingLength -= sizeof(byte4_t);

	if (!size)
	{
		if (remainingLength < 4)
			return CXSECURE_E2SMALL;

		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0));
		src += sizeof(byte4_t);
		remainingLength -= sizeof(byte4_t);
	}

	while (size)
	{
		u32 i;
		byte4_t flags = *src++;

			--remainingLength;
			if (remainingLength < 0)
				return CXSECURE_E2SMALL;
		for (i = 0; i < 8; ++i)
		{

			if (!(flags & 0x80))
			{
				*dst++ = *src++;

				--remainingLength;
				if (remainingLength < 0)
					return CXSECURE_E2SMALL;

				--size;
			}
			else
			{
				s32 length2 = *src >> 4;

				if (!stat)
				{
					length2 += 3;
				}
				else
				{
					if (length2 == 0x01)
					{
						length2 = (*src++ & 0x0f) << 12;
						length2 |= *src++ << 4;
						length2 |= *src >> 4;
						length2 += 0x111;

						remainingLength -= 2;
					}
					else if (length2 == 0x00)
					{
						length2 = (*src++ & 0x0f) << 4;
						length2 |= *src >> 4;
						length2 += 0x11;

						remainingLength -= 1;
					}
					else
					{
						length2 += 0x01;
					}
				}

				s32 offset = (*src++ & 0x0f) << 8;
				offset = (offset | *src++) + 1;

				remainingLength -= 2;
				if (remainingLength < 0)
					return CXSECURE_E2SMALL;

				if (length2 > size)
					return CXSECURE_EBADSIZE;

				if ((u32)&dst[-offset] < (u32)uncompressed)
					return CXSECURE_EBADSIZE;


				size -= length2;

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
				} while (--length2 > 0);
				// clang-format on
			}

			if (!size)
				break;

			flags <<= 1;
		}
	}

	if (remainingLength > 0x20)
		return CXSECURE_E2BIG;

	return CXSECURE_ESUCCESS;
}

_Bool CXiVerifyHuffmanTable_(void const *param_1, u8 param_2)
{
	// What would this be for
	static byte_t const FLAGS_ARRAY_NUM[4] ATTR_UNUSED = {0, 0, 0, 0x40};

	byte_t const *a = param_1;
	byte_t const *b = a + 1;
	unk_t unsigned c = *a;
	byte_t const *d = (byte_t *)param_1 + ((c + 1) << 1);

	byte_t e[sizeof(byte_t) * 0x40];
	for (u32 i = 0; i < ARRAY_LENGTH(e); ++i)
		e[i] = 0;

	if (param_2 == 4 && c >= 16)
		return false;

	unk_t unsigned f = 1;

	for (a = b; a < d; ++f, (void)++a)
	{
		if (e[f / 8] & (1 << (f % 8)))
			continue;

		unk_t g = ((*a & 0x3f) + 1) << 1;
		unk_t h = ((u32)a >> 1 << 1) + g;

		if (*a == 0x00 && f >= c << 1)
			continue;

		if ((u32)h >= (u32)d)
			return false;

		if (*a & 0x80)
		{
			unk_t unsigned j = (f & ~1) + g;
			e[j / 8] |= (byte_t)(1 << (j % 8));
		}

		if (*a & 0x40)
		{
			unk_t unsigned k = (f & ~1) + g + 1;
			e[k / 8] |= (byte_t)(1 << (k % 8));
		}
	}

	return true;
}

CXSecureResult CXSecureUncompressHuffman(void const *compressed, u32 length,
                                         void *uncompressed)
{
	byte_t const *src = compressed;
	byte_t *dst = uncompressed;

	u8 secstat =
		CXiConvertEndian32_(IN_BUFFER_AT(byte4_t const, src, 0)) & 0xff;
	s32 size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;

	byte_t const *base = size ? src + 4 : src + 8;
	byte_t const *basep1 = base + 1;

	u32 stat = IN_BUFFER_AT(byte1_t, src, 0) & 0x0f;
	unk_t unsigned b = 0;
	unk_t unsigned c = 0;
	unk_t d = (stat & 7) + 4;
	s32 remainingLength = length;
	unk_t unsigned e = (*base + 1) << 1;

	if ((secstat & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_TYPE_HUFFMAN)
		return CXSECURE_EBADTYPE;

	if (stat != 4 && stat != 8)
		return CXSECURE_EBADTYPE;

	if (!size)
	{
		if (length < e + 8)
			return CXSECURE_E2SMALL;

		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 4));
	}
	else
	{
		if (length < e + 4)
			return CXSECURE_E2SMALL;
	}

	if (!CXiVerifyHuffmanTable_(base, stat))
		return CXSECURE_EBADTABLE;

	src = base + e;
	remainingLength -= (u32)src - (u32)compressed;

	if (remainingLength < 0)
		return CXSECURE_E2SMALL;

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

		remainingLength -= sizeof(byte4_t);
		if (remainingLength < 0)
			return CXSECURE_E2SMALL;

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
				b >>= stat;
				b |= *base << (32 - stat);
				base = basep1;

				++c;

				if (size <= c * stat >> 3)
				{
					b >>= stat * (d - c);
					c = d;
				}

				if (c == d)
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

	if (remainingLength > 0x20)
		return CXSECURE_E2BIG;

	return CXSECURE_ESUCCESS;
}

static unk_t CXiHuffImportTree(u16 *tree, byte_t const *param_2, u8 param_3,
                               unk_t unsigned param_4)
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
		b = CXiConvertEndian16_(IN_BUFFER_AT(byte2_t, param_2, 0));

		param_2 += sizeof(byte2_t);
		f += sizeof(byte2_t);
	}
	else
	{
		b = IN_BUFFER_AT(byte1_t, param_2, 0);

		param_2 += sizeof(byte1_t);
		f += sizeof(byte1_t);
	}

	b = (b + 1) << 2;

	if (param_4 < b)
		return b;

	while (f < b)
	{
		while (d < param_3)
		{
			c <<= 8;
			c |= *param_2++;
			++f;
			d += 8;
		}

		if (a < g)
			tree[a++] = e & (c >> (d - param_3));

		d -= param_3;
	}

	(void)param_2;

	return b;
}

CXSecureResult CXSecureUnfilterDiff(void const *compressed, u32 length,
                                    void *uncompressed)
{
	byte_t const *src = compressed;
	byte_t *dst = uncompressed;

	u32 stat = IN_BUFFER_AT(byte1_t, src, 0) & 0x0f;
	u8 stat2 = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) & 0xff;
	s32 size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;
	u32 sum = 0;

	s32 remainingLength = length;

	if ((stat2 & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_TYPE_FILTER_DIFF)
		return CXSECURE_EBADTYPE;

	if (stat != 0 && stat != 1)
		return CXSECURE_EBADTYPE;

	if (length <= 4)
		return CXSECURE_E2SMALL;

	src += sizeof(byte4_t);
	remainingLength -= sizeof(byte4_t);

	if (stat != 0x01)
	{
		do
		{
			byte1_t num = *src++;

			remainingLength -= sizeof(byte1_t);
			if (remainingLength < 0)
				return CXSECURE_E2SMALL;

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

			remainingLength -= sizeof(byte2_t);
			if (remainingLength < 0)
				return CXSECURE_E2SMALL;

			size -= sizeof(byte2_t);

			sum += num;

			*(byte2_t *)dst = CXiConvertEndian16_(sum);
			dst += sizeof(byte2_t);
		} while (size > 0);
	}

	if (remainingLength > 0x20)
		return CXSECURE_E2BIG;

	return CXSECURE_ESUCCESS;
}

static void BitReader_Init(struct BitReader *bitReader, byte_t const *param_2,
                           unk_t param_3)
{
	bitReader->at_0x00 = param_2;
	bitReader->at_0x04 = 0;
	bitReader->at_0x08 = 0;
	bitReader->at_0x0c = 0;
	bitReader->at_0x10 = param_3;
}

static signed char BitReader_Read(struct BitReader *bitReader)
{
	if (!bitReader->at_0x0c)
	{
		if ((u32)bitReader->at_0x04 > bitReader->at_0x10)
			return CXSECURE_EBADTYPE;

		bitReader->at_0x08 = bitReader->at_0x00[bitReader->at_0x04++];
		bitReader->at_0x0c = 8;
	}

	signed char a = (bitReader->at_0x08 >> (bitReader->at_0x0c - 1)) & 0x01;
	--bitReader->at_0x0c;

	return a;
}

_Bool CXiLHVerifyTable(void const *param_1, u8 param_2)
{
	u16 const *a = param_1;
	u16 const *b = a + 1;
	unk_t unsigned c = *a;
	u16 const *d = (u16 *)param_1 + c;
	u16 e = (1 << (param_2 - 2)) - 1;
	u16 f = 1 << (param_2 - 1);
	u16 g = 1 << (param_2 - 2);

	byte_t h[sizeof(u16) * 0x40];
	for (u32 i = 0; i < ARRAY_LENGTH(h); ++i)
		h[i] = 0;

	if (c > 1 << (param_2 + 1))
		return false;

	unk_t unsigned j = 1;
	for (a = b; a < d; ++j, (void)++a)
	{
		if (h[j / 8] & (1 << (j % 8)))
			continue;

		unk_t k = ((*a & e) + 1) << 1;
		unk_t l = ((u32)a & ~3) + (k << 1);

		if (*a == 0x00 && j >= c - 4)
			continue;

		if (l >= (u32)d)
			return false;

		if (*a & f)
		{
			unk_t unsigned m = (j & ~1) + k;
			h[m / 8] |= (byte_t)(1 << (m % 8));
		}

		if (*a & g)
		{
			unk_t unsigned n = (j & ~1) + k + 1;
			h[n / 8] |= (byte_t)(1 << (n % 8));
		}

	}

	return true;
}

CXSecureResult CXSecureUncompressLH(void const *compressed, u32 length,
                                    byte_t *uncompressed, u16 *param_4)
{
	u32 size;
	unk_t unsigned a = 0;
	byte_t const *src = compressed;

	if ((IN_BUFFER_AT(byte1_t, compressed, 0) & CX_COMPRESSION_TYPE_MASK)
	    != CX_COMPRESSION_TYPE_LH)
	{
		return CXSECURE_EBADTYPE;
	}

	if (length <= 4)
		return CXSECURE_E2SMALL;

	u16 *b = param_4;
	u16 *c = param_4 + 0x400;
	u16 *n ATTR_UNUSED = param_4 + 0x440;

	// size
	size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0)) >> 8;
	src += sizeof(byte4_t);

	if (!size)
	{
		size = CXiConvertEndian32_(IN_BUFFER_AT(byte4_t, src, 0));
		src += sizeof(byte4_t);

		if (length < 8)
			return CXSECURE_E2SMALL;
	}

	src += CXiHuffImportTree(b, src, 9, length - ((u32)src - (u32)compressed));

	if ((u32)src > (u32)compressed + length)
		return CXSECURE_E2SMALL;

	if (!CXiLHVerifyTable(b, 9))
		return CXSECURE_EBADTABLE;

	src += CXiHuffImportTree(c, src, 5, length - ((u32)src - (u32)compressed));

	if ((u32)src > (u32)compressed + length)
		return CXSECURE_E2SMALL;

	if (!CXiLHVerifyTable(c, 5))
		return CXSECURE_EBADTABLE;

	struct BitReader bitReader;
	BitReader_Init(&bitReader, src, length - ((u32)src - (u32)compressed));

	while (a < size)
	{
		u16 d;
		u16 *e = b + 1;

		while (true)
		{
			signed char f = BitReader_Read(&bitReader);
			unk_t g = (((*e & 0x7f) + 1) << 1) + f;

			if (f < 0)
				return CXSECURE_E2SMALL;

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
			signed char l = BitReader_Read(&bitReader);
			u32 m = (((*k & 0x07) + 1) << 1) + l;

			if (l < 0)
				return CXSECURE_E2SMALL;

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

		// Wait why does it just match like this now
		d = d + 1;

		if (a < d)
			return CXSECURE_EBADSIZE;

		if (a + j > size)
			return CXSECURE_EBADSIZE;

		while (j--)
		{
			uncompressed[a] = uncompressed[a - d];
			++a;
		}
	}

	if ((u32)bitReader.at_0x10 - bitReader.at_0x04 > 0x20)
		return CXSECURE_E2BIG;

	(void)a;
	(void)compressed;

	return CXSECURE_ESUCCESS;
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

CXSecureResult CXSecureUncompressLRC(void const *compressed, u32 length,
                                     byte_t *uncompressed,
                                     unk_t unsigned *param_3)
{
	byte_t const *src = compressed;
	unk_t unsigned a = 0;
	u32 size = 0;

	if ((IN_BUFFER_AT(byte1_t, compressed, 0) & CX_COMPRESSION_TYPE_MASK)
	    != CX_COMPRESSION_TYPE_LRC)
	{
		return CXSECURE_EBADTYPE;
	}

	if (length <= 4)
		return CXSECURE_E2SMALL;

	struct RCInfo info1;
	RCInitInfo_(&info1, 9, param_3);

	struct RCInfo info2;
#if !defined(NDEBUG) // TODO (What)
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

		if (length < 8)
			return CXSECURE_E2SMALL;
	}


	if (length - ((u32)src - (u32)compressed) < 4)
		return CXSECURE_E2SMALL;

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
			return CXSECURE_EBADSIZE;

		if (a < c)
			return CXSECURE_EBADSIZE;

		if ((u32)src - (u32)compressed > length)
			return CXSECURE_E2SMALL;

		while (d--)
		{
			uncompressed[a] = uncompressed[a - c];
			++a;
		}
	}

	(void)compressed;

	return CXSECURE_ESUCCESS;
}
