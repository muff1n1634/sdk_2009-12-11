#include <revolution/cx/CXStreamingUncompression.h>

/*******************************************************************************
 * headers
 */

#include <decomp.h>
#include <macros.h> // ROUND_DOWN_PTR

#include <revolution/types.h>

#include <revolution/cx.h>
#include "CXInternal.h"

/*******************************************************************************
 * types
 */

struct BitReader
{
	byte_t			const *at_0x00;	// size 0x04, offset 0x00
	unk4_t			at_0x04;		// size 0x04, offset 0x04
	unk4_t unsigned	at_0x08;		// size 0x04, offset 0x08
	unk4_t unsigned	at_0x0c;		// size 0x04, offset 0x0c
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
	unk1_t unsigned	at_0x0c;	// size 0x04, offset 0x0c
	byte_t pad0_[3]; // alignment?
	unk4_t			at_0x10;	// size 0x04, offset 0x10
}; // size 0x14?

/*******************************************************************************
 * local function declarations
 */

static inline unk_t CXiReadHeader(u8 *, unk4_t signed *, byte_t const *, unk_t,
                                  unk_t);

static inline unk_t signed BitReader_Read(struct BitReader *bitReader,
                                          unk1_t unsigned);
static inline unk8_t BitReader_ReadEx(struct BitReader *bitReader,
                                      unk1_t unsigned);

static inline byte_t *GetNextNode(byte_t *, unk_t);

static inline void RCInitInfo_(struct RCInfo *rcInfo, unk1_t unsigned);
static inline void RCInitState_(struct RCState *rcState);
static void RCAddCount_(struct RCInfo *rcInfo, u16);
static u16 RCSearch_(struct RCInfo *rcInfo, unk_t, unk_t unsigned, unk_t);
static u16 RCGetData_(byte_t const *stream, struct RCInfo *rcInfo,
                      struct RCState *rcState, unk_t, unk_t *);

static inline void LRCIntro_(CXUncompContextLRC *context, struct RCInfo *info1,
                             struct RCInfo *info2, struct RCState *state);
static inline void LRCFin_(CXUncompContextLRC *context, struct RCInfo *info1,
                           struct RCInfo *info2, struct RCState *state);

/*******************************************************************************
 * functions
 */

void CXInitUncompContextRL(CXUncompContextRL *context, byte_t *param_2)
{
	context->at_0x00 = param_2;
	context->at_0x04 = 0;
	context->at_0x0e = 0;
	context->at_0x0c = 0;
	context->at_0x0f = 8;
	context->at_0x08 = 0;
}

void CXInitUncompContextLZ(CXUncompContextLZ *context, byte_t *param_2)
{
	context->at_0x00 = param_2;
	context->at_0x04 = 0;
	context->at_0x11 = 0;
	context->at_0x12 = 0;
	context->at_0x0c = 0;
	context->at_0x10 = 3;
	context->at_0x13 = 8;
	context->at_0x14 = 0;
	context->at_0x08 = 0;
}

void CXInitUncompContextHuffman(CXUncompContextHuffman *context,
                                unk4_t *param_2)
{
	context->at_0x00 = param_2;
	context->at_0x04 = 0;
	context->at_0x1c = 0;
	context->at_0x18 = -1;
	context->at_0x0c = context->at_0x20;
	context->at_0x14 = 0;
	context->at_0x1b = 0;
	context->at_0x10 = 0;
	context->at_0x1a = 0;
	context->at_0x1d = 8;
	context->at_0x08 = 0;
}

CXStreamingResult CXReadUncompRL(CXUncompContextRL *context,
                                 void const *compressed, u32 size)
{
	byte_t const *src = compressed;

	if (context->at_0x0f)
	{
		if (context->at_0x0f == 8)
		{
			if ((*src & CX_COMPRESSION_TYPE_MASK)
			    != CX_COMPRESSION_TYPE_RUN_LENGTH)
			{
				return CXSTREAM_EBADTYPE;
			}

			if ((*src & 0x0f) != 0)
				return CXSTREAM_EBADTYPE;
		}

		unk_t a = CXiReadHeader(&context->at_0x0f, &context->at_0x04, src,
		                        size, context->at_0x08);

		src += a;
		size -= a;

		if (!size)
			return !context->at_0x0f ? context->at_0x04 : CXSTREAM_EBADTYPE;
	}

	while (context->at_0x04 > 0)
	{
		if (!(context->at_0x0e & 0x80))
		{
			while (context->at_0x0c)
			{
				*context->at_0x00++ = *src++;

				--context->at_0x0c;
				--context->at_0x04;
				--size;

				if (!size)
					return context->at_0x04;
			}
		}
		else if (context->at_0x0c)
		{
			byte_t b = *src++;
			--size;

			while (context->at_0x0c)
			{
				*context->at_0x00++ = b;

				--context->at_0x0c;
				--context->at_0x04;
			}

			if (!size)
				return context->at_0x04;
		}

		if (!context->at_0x04)
			break;

		context->at_0x0e = *src++;
		--size;
		context->at_0x0c = context->at_0x0e & 0x7f;

		if (context->at_0x0e & 0x80)
			context->at_0x0c += 3;
		else
			context->at_0x0c += 1;

		if (context->at_0x0c > context->at_0x04)
		{
			if (!context->at_0x08)
				return CXSTREAM_EBADSIZE;

			context->at_0x0c = context->at_0x04;
		}

		if (size)
			continue;

		return context->at_0x04;
	}

	if (!context->at_0x08 && size > 0x20)
		return CXSTREAM_E2BIG;

	return CXSTREAM_ESUCCESS;
}

static unk_t CXiReadHeader(u8 *param_1, unk4_t signed *param_2,
                           byte_t const *param_3, unk_t param_4,
                           unk_t param_5)
{
	unk_t a = 0;

	while (*param_1)
	{
		--*param_1;

		if (*param_1 <= 3)
			*param_2 |= *param_3 << ((3 - *param_1) << 3);
		else if (*param_1 <= 6)
			*param_2 |= *param_3 << ((6 - *param_1) << 3);

		++param_3;
		++a;

		if (*param_1 == 4 && *param_2 > 0)
			*param_1 = 0;

		--param_4;
		if (param_4 == 0 && *param_1 != 0)
			return a;
	}

	if (param_5 > 0 && param_5 < *param_2)
		*param_2 = param_5;

	return a;
}

CXStreamingResult CXReadUncompLZ(CXUncompContextLZ *context,
                                 void const *compressed, u32 size)
{
	byte_t const *src = compressed;

	if (context->at_0x13)
	{
		if (context->at_0x13 == 8)
		{
			if ((*src & CX_COMPRESSION_TYPE_MASK)
			    != CX_COMPRESSION_TYPE_LEMPEL_ZIV)
			{
				return CXSTREAM_EBADTYPE;
			}

			context->at_0x14 = *src & 0x0f;

			if (context->at_0x14 != 0 && context->at_0x14 != 1)
				return CXSTREAM_EBADTYPE;
		}

		unk_t a = CXiReadHeader(&context->at_0x13, &context->at_0x04, src, size,
		                        context->at_0x08);

		src += a;
		size -= a;

		if (!size)
			return !context->at_0x13 ? context->at_0x04 : CXSTREAM_EBADTYPE;
	}

	while (context->at_0x04 > 0)
	{
		while (context->at_0x12)
		{
			if (!size)
				return context->at_0x04;

			if (!(context->at_0x11 & 0x80))
			{
				*context->at_0x00++ = *src++;
				--context->at_0x04;

				--size;

				goto there; // TODO: how to get rid of
			}

			while (context->at_0x10)
			{
				--context->at_0x10;

				if (!context->at_0x14)
				{
					context->at_0x0c = *src++;
					context->at_0x0c += 0x30;
					context->at_0x10 = 0;
				}
				else
				{
					switch (context->at_0x10)
					{
					case 2:
						context->at_0x0c = *src++;

						if (context->at_0x0c >> 4 == 1)
						{
							context->at_0x0c = (context->at_0x0c & 0x0f) << 16;
							context->at_0x0c += 0x1110;
						}
						else if (context->at_0x0c >> 4 == 0)
						{
							context->at_0x0c = (context->at_0x0c & 0x0f) << 8;
							context->at_0x0c += 0x110;
							context->at_0x10 = 1;
						}
						else
						{
							context->at_0x0c += 0x10;
							context->at_0x10 = 0;
						}

						break;

					case 1:
						context->at_0x0c += *src++ << 8;
						break;

					case 0:
						context->at_0x0c += *src++;
						break;
					}
				}

				--size;
				if (!size)
					return context->at_0x04;
			}

			unk_t long a = (context->at_0x0c & 0x0f) << 8;
			context->at_0x0c >>= 4;

			a = (a | *src++) + 1;
			--size;
			context->at_0x10 = 3;

			if (context->at_0x0c > context->at_0x04)
			{
				if (!context->at_0x08)
					return CXSTREAM_EBADSIZE;

				context->at_0x0c = context->at_0x04;
			}

			while (context->at_0x0c > 0)
			{
				*context->at_0x00 = context->at_0x00[-a];
				++context->at_0x00;
				--context->at_0x04;
				--context->at_0x0c;
			}

			there:
			if (!context->at_0x04)
				goto out;

			context->at_0x11 <<= 1;
			--context->at_0x12;
		}

		if (!size)
			return context->at_0x04;

		context->at_0x11 = *src++;
		context->at_0x12 = 8;
		--size;
	}

out:
	if (!context->at_0x08 && size > 0x20)
		return CXSTREAM_E2BIG;

	return CXSTREAM_ESUCCESS;
}

CXStreamingResult CXReadUncompHuffman(CXUncompContextHuffman *context,
                                      void const *compressed, u32 size)
{
	byte_t const *src = compressed;

	if (context->at_0x1d)
	{
		if (context->at_0x1d == 8)
		{
			context->at_0x1c = *src & 0x0f;

			if ((*src & CX_COMPRESSION_TYPE_MASK)
			    != CX_COMPRESSION_TYPE_HUFFMAN)
			{
				return CXSTREAM_EBADTYPE;
			}

			if (context->at_0x1c != 4 && context->at_0x1c != 8)
				return CXSTREAM_EBADTYPE;
		}

		unk_t a = CXiReadHeader(&context->at_0x1d, &context->at_0x04, src, size,
		                        context->at_0x08);

		src += a;
		size -= a;

		if (!size)
			return !context->at_0x1d ? context->at_0x04 : CXSTREAM_EBADTYPE;
	}

	if (context->at_0x18 < 0)
	{
		context->at_0x18 = ((*src + 1) << 1) - 1;
		*context->at_0x0c++ = *src++;
		--size;
	}

	while (context->at_0x18 > 0)
	{
		if (!size)
			return context->at_0x04;

		*context->at_0x0c++ = *src++;
		--context->at_0x18;
		--size;

		if (context->at_0x18)
			continue;

		context->at_0x0c = context->at_0x20 + 1;

		if (!CXiVerifyHuffmanTable_(context->at_0x20, context->at_0x1c))
			return CXSTREAM_EBADTABLE;
	}

	while (context->at_0x04 > 0)
	{
		while (context->at_0x1a < 0x20)
		{
			if (!size)
				return context->at_0x04;

			context->at_0x10 |= *src++ << context->at_0x1a;

			--size;
			context->at_0x1a += 8;
		}

		while (context->at_0x1a)
		{
			unk_t b = context->at_0x10 >> 31;
			unk_t c = (*(byte_t volatile *)context->at_0x0c << b) & 0x80; // ?

			context->at_0x0c = GetNextNode(context->at_0x0c, b);
			context->at_0x10 <<= 1;
			--context->at_0x1a;

			if (!c)
				continue;

			context->at_0x14 >>= context->at_0x1c;
			context->at_0x14 |= *context->at_0x0c << (32 - context->at_0x1c);
			context->at_0x0c = context->at_0x20 + 1;
			context->at_0x1b += context->at_0x1c;

			if (context->at_0x04 <= context->at_0x1b / 8)
			{
				context->at_0x14 >>= 32 - context->at_0x1b;
				context->at_0x1b = 32;
			}

			if (context->at_0x1b == 32)
			{
				*context->at_0x00 = CXiConvertEndian32_(context->at_0x14);
				++context->at_0x00;
				context->at_0x04 -= 4;
				context->at_0x1b = 0;

				if (context->at_0x04 <= 0)
					goto out;
			}

			(void)b;
		}
	}

out:
	if (!context->at_0x08 && size > 0x20)
		return CXSTREAM_E2BIG;

	return CXSTREAM_ESUCCESS;
}

static byte_t *GetNextNode(byte_t *param_1, unk_t param_2)
{
	return (byte_t *)ROUND_DOWN_PTR(param_1, 2) + (((*param_1 & 0x3f) + 1) << 1)
	     + param_2;
}

void CXInitUncompContextLH(CXUncompContextLH *context, byte_t *param_2)
{
	context->at_0x000 = param_2;
	context->at_0x004 = -1;
	context->at_0x88c = context->at_0x00c + 1;
	context->at_0x890 = -1;
	context->at_0x894 = -1;
	context->at_0x8a7 = 8;
	context->at_0x8a4 = 0;
	context->at_0x89c = 0;
	context->at_0x8a0 = 0;
	context->at_0x8a6 = -1;
	context->at_0x008 = 0;
}

CXStreamingResult CXReadUncompLH(CXUncompContextLH *context,
                                 void const *compressed, u32 size)
{
	byte_t const *src = compressed;

	struct BitReader bitReader;
	bitReader.at_0x00 = src;
	bitReader.at_0x04 = size;
	bitReader.at_0x08 = context->at_0x89c;
	bitReader.at_0x0c = context->at_0x8a0;

	while (context->at_0x8a7)
	{
		unk8_t signed a = BitReader_ReadEx(&bitReader, 32);

		if (a < 0)
			goto end;

		context->at_0x8a7 -= 4;

		if (context->at_0x8a7 == 4)
		{
			byte4_t stat = CXiConvertEndian32_(a);

			if ((stat & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_TYPE_LH)
				return CXSTREAM_EBADTYPE;

			context->at_0x004 = stat >> 8;

			if (!context->at_0x004)
			{
				context->at_0x8a7 = 4;
				context->at_0x004 = -1;
			}
			else
			{
				context->at_0x8a7 = 0;
			}
		}
		else
		{
			context->at_0x004 = CXiConvertEndian32_(a);
		}

		if (context->at_0x8a7)
			continue;

		if (context->at_0x008 <= 0)
			continue;

		if (context->at_0x008 >= context->at_0x004)
			continue;

		context->at_0x004 = context->at_0x008;
	}

	unk_t b;

	if (context->at_0x890 < 0)
	{
		b = BitReader_Read(&bitReader, 16);

		if (b < 0)
			goto end;

		context->at_0x890 = (CXiConvertEndian16_(b) + 1) << 2 << 3;
		context->at_0x898 = 1;
		context->at_0x890 -= 16;
	}

	while (context->at_0x890 >= 9)
	{
		b = BitReader_Read(&bitReader, 9);

		if (b < 0)
			goto end;

		context->at_0x00c[context->at_0x898++] = b;
		context->at_0x890 -= 9;
	}

	while (context->at_0x890 > 0)
	{
		b = BitReader_Read(&bitReader, context->at_0x890);

		if (b < 0)
			goto end;

		context->at_0x890 = 0;
	}

	if (!CXiLHVerifyTable(context->at_0x00c, 9))
		return CXSTREAM_EBADTABLE;

	if (context->at_0x894 < 0)
	{
		b = BitReader_Read(&bitReader, 8);

		if (b < 0)
			goto end;

		context->at_0x894 = ((b & 0xffff) + 1) << 2 << 3;
		context->at_0x898 = 1;
		context->at_0x894 -= 8;
	}

	while (context->at_0x894 >= 5)
	{
		b = BitReader_Read(&bitReader, 5);

		if (b < 0)
			goto end;

		context->at_0x80c[context->at_0x898++] = b;
		context->at_0x894 -= 5;
	}

	while (context->at_0x894 > 0)
	{
		b = BitReader_Read(&bitReader, context->at_0x894);

		if (b < 0)
			goto end;

		context->at_0x894 = 0;
	}

	if (!CXiLHVerifyTable(context->at_0x80c, 5))
		return CXSTREAM_EBADTABLE;

	u16 *c = context->at_0x88c;
	unk2_t unsigned d = context->at_0x8a4;

	while (context->at_0x004 > 0)
	{
		if (!d)
		{
			while (true)
			{
				b = BitReader_Read(&bitReader, 1);

				if (b < 0)
				{
					context->at_0x88c = c;
					context->at_0x8a4 = d;
					goto end;
				}

				unk1_t unsigned e = b & 1;
				unk4_t f = (((*c & 0x7f) + 1) << 1) + e;

				if (*c & (0x100 >> e))
				{
					c = ROUND_DOWN_PTR(c, 4);
					d = c[f];
					c = context->at_0x80c + 1;

					break;
				}
				else
				{
					c = ROUND_DOWN_PTR(c, 4);
					c += f;
				}
			}
		}

		if (d < 0x100)
		{
			*context->at_0x000++ = d;
			--context->at_0x004;

			c = context->at_0x00c + 1;
			d = 0;
			continue;
		}

		unk2_t unsigned g;
		unk2_t unsigned h = (d & 0xff) + 3;

		if (context->at_0x8a6 < 0)
		{
			while (true)
			{
				b = BitReader_Read(&bitReader, 1);

				if (b < 0)
				{
					context->at_0x88c = c;
					context->at_0x8a4 = d;
					goto end;
				}

				unk1_t unsigned l = b & 1;
				unk4_t m = (((*c & 0x07) + 1) << 1) + l;

				if (*c & (0x10 >> l))
				{
					c = ROUND_DOWN_PTR(c, 4);
					context->at_0x8a6 = c[m];

					break;
				}
				else
				{
					c = ROUND_DOWN_PTR(c, 4);
					c += m;
				}
			}
		}

		if (context->at_0x8a6 <= 1)
		{
			b = context->at_0x8a6;
		}
		else
		{
			b = BitReader_Read(&bitReader, context->at_0x8a6 - 1);

			if (b < 0)
			{
				context->at_0x88c = c;
				context->at_0x8a4 = d;
				goto end;
			}
		}

		if (context->at_0x8a6 >= 2)
			b |= 1 << (context->at_0x8a6 - 1);

		context->at_0x8a6 = -1;

		g = b + 1;

		if (context->at_0x004 < h)
		{
			if (!context->at_0x008)
				return CXSTREAM_EBADSIZE;

			h = context->at_0x004;
		}

		context->at_0x004 -= h;

		while (h--)
		{
			*context->at_0x000 = *(context->at_0x000 - g);
			++context->at_0x000;
		}

		d = 0;
		c = context->at_0x00c + 1;
	}

end:
	context->at_0x89c = bitReader.at_0x08;
	context->at_0x8a0 = bitReader.at_0x0c;

	if (!context->at_0x004 && !context->at_0x008 && bitReader.at_0x0c > 0x20)
		return CXSTREAM_E2BIG;

	return context->at_0x004;
}

static unk_t signed BitReader_Read(struct BitReader *bitReader,
                                   unk1_t unsigned param_2)
{
	while (bitReader->at_0x0c < param_2)
	{
		if (!bitReader->at_0x04)
			return -1;

		bitReader->at_0x08 <<= 8;
		bitReader->at_0x08 += *bitReader->at_0x00;
		++bitReader->at_0x00;
		--bitReader->at_0x04;
		bitReader->at_0x0c += 8;
	}

	unk_t signed a = ((1 << param_2) - 1)
	               & (bitReader->at_0x08 >> (bitReader->at_0x0c - param_2));

	bitReader->at_0x0c -= param_2;

	return a;
}

static unk8_t BitReader_ReadEx(struct BitReader *bitReader,
                                      unk1_t unsigned param_2)
{
	unk1_t unsigned a = 0;

	while (bitReader->at_0x0c < param_2)
	{
		if (!bitReader->at_0x04)
			return -1;

		if (bitReader->at_0x0c > 24)
			a = bitReader->at_0x08 >> 24;

		bitReader->at_0x08 <<= 8;
		bitReader->at_0x08 += *bitReader->at_0x00;
		++bitReader->at_0x00;
		--bitReader->at_0x04;
		bitReader->at_0x0c += 8;
	}

	unk8_t b = bitReader->at_0x08;
	b |= (unk8_t)a << 32;
	b = (b >> (bitReader->at_0x0c - param_2)) & ((1 << param_2) - 1);

	bitReader->at_0x0c -= param_2;

	return b;
}

static void RCInitInfo_(struct RCInfo *rcInfo, unk1_t unsigned param_2)
{
	u32 i;
	unk_t a = 1 << param_2;

	rcInfo->at_0x0c = param_2;

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
                      struct RCState *rcState, unk_t param_4, unk_t *param_5)
{
	u16 a =
		RCSearch_(rcInfo, rcState->at_0x08, rcState->at_0x04, rcState->at_0x00);
	unk_t b = 0;

	// arbitrary block
	{
		unk_t c = rcState->at_0x04 / rcInfo->at_0x08;

		rcState->at_0x00 += c * rcInfo->at_0x04[a];
		rcState->at_0x04 = c * rcInfo->at_0x00[a];

		RCAddCount_(rcInfo, a);

		while (rcState->at_0x04 < 0x1000000)
		{
			if (!param_4)
			{
				b = b < 0 ? b - 1 : -1;
			}
			else
			{
				rcState->at_0x08 <<= 8;
				rcState->at_0x08 += stream[b++];

				--param_4;
			}

			rcState->at_0x04 <<= 8;
			rcState->at_0x00 <<= 8;
		}
	}

	*param_5 = b;
	return a;
}

static void LRCIntro_(CXUncompContextLRC *context, struct RCInfo *info1,
                      struct RCInfo *info2, struct RCState *state)
{
	info1->at_0x00 = &context->at_0x000c;
	info1->at_0x04 = &context->at_0x080c;
	info1->at_0x08 = context->at_0x900c;
	info1->at_0x0c = 9;

	info2->at_0x00 = &context->at_0x100c;
	info2->at_0x04 = &context->at_0x500c;
	info2->at_0x08 = context->at_0x9010;
	info2->at_0x0c = 12;

	state->at_0x00 = context->at_0x901c;
	state->at_0x04 = context->at_0x9014;
	state->at_0x08 = context->at_0x9018;
	state->at_0x0c = context->at_0x9024;
	state->at_0x10 = context->at_0x9020;
}

static void LRCFin_(CXUncompContextLRC *context, struct RCInfo *info1,
                    struct RCInfo *info2, struct RCState *state)
{
	context->at_0x900c = info1->at_0x08;
	context->at_0x9010 = info2->at_0x08;
	context->at_0x901c = state->at_0x00;
	context->at_0x9014 = state->at_0x04;
	context->at_0x9018 = state->at_0x08;
	context->at_0x9024 = (u8)state->at_0x0c;
	context->at_0x9020 = state->at_0x10;
}

void CXInitUncompContextLRC(CXUncompContextLRC *context,
                            unk1_t unsigned *param_2)
{
	struct RCInfo info1;
	struct RCInfo info2;
	struct RCState state;

	LRCIntro_(context, &info1, &info2, &state);

	context->at_0x0000 = param_2;
	context->at_0x0004 = 0;
	context->at_0x9028 = 8;
	context->at_0x9026 = 0;
	context->at_0x0008 = 0;
	context->at_0x9025 = 4;

	RCInitInfo_(&info1, 9);
	RCInitInfo_(&info2, 12);
	RCInitState_(&state);

	LRCFin_(context, &info1, &info2, &state);
}

CXStreamingResult CXReadUncompLRC(CXUncompContextLRC *context,
                                  void const *compressed, u32 size)
{
	byte_t const *src = compressed;

	struct RCInfo rcInfo1;
	struct RCInfo rcInfo2;
	struct RCState rcState;

	LRCIntro_(context, &rcInfo1, &rcInfo2, &rcState);

	if (context->at_0x9028)
	{
		if (context->at_0x9028 == 8)
		{
			if ((*src & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_TYPE_LRC)
				return CXSTREAM_EBADTYPE;

			if ((*src & 0x0f) != 0)
				return CXSTREAM_EBADTYPE;
		}

		unk_t a = CXiReadHeader(&context->at_0x9028, &context->at_0x0004, src,
		                          size, context->at_0x0008);

		src += a;
		size -= a;

		if (!size)
			return !context->at_0x9028 ? context->at_0x0004 : CXSTREAM_EBADTYPE;
	}

	while (context->at_0x9025)
	{
		if (!size)
			goto end;

		rcState.at_0x08 <<= 8;
		rcState.at_0x08 += *src;

		++src;
		--size;
		--context->at_0x9025;
	}

	while (context->at_0x0004 > 0)
	{
		if (!context->at_0x9026)
		{
			unk_t b;
			unk2_t unsigned c =
				RCGetData_(src, &rcInfo1, &rcState, size, &b);

			if (c < 0x100)
			{
				*context->at_0x0000++ = c;
				--context->at_0x0004;
			}
			else
			{
				context->at_0x9026 = (c & 0xff) + 3;
			}

			if (b < 0)
			{
				context->at_0x9025 = -b;
				break;
			}

			src += b;
			size -= b;

		}

		if (context->at_0x9026)
		{
			unk_t d;
			unk2_t unsigned e =
				RCGetData_(src, &rcInfo2, &rcState, size, &d) + 1;

			if (context->at_0x9026 > context->at_0x0004)
			{
				if (!context->at_0x0008)
					return CXSTREAM_EBADSIZE;

				context->at_0x9026 = context->at_0x0004;
			}

			while (context->at_0x9026)
			{
				*context->at_0x0000 = context->at_0x0000[-e];
				++context->at_0x0000;
				--context->at_0x0004;
				--context->at_0x9026;
			}

			if (d < 0)
			{
				context->at_0x9025 = -d;
				break;
			}

			src += d;
			size -= d;
		}
	}

end:
	LRCFin_(context, &rcInfo1, &rcInfo2, &rcState);

	if (!context->at_0x0004 && !context->at_0x0008 && size > 0x20)
		return CXSTREAM_E2BIG;

	return context->at_0x0004;
}
