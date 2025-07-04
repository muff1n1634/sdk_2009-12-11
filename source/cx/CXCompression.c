#include <revolution/cx/CXCompression.h>

/*******************************************************************************
 * headers
 */

#include <decomp.h>
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
 * macros
 */

#undef NULL
#define NULL	((void *)0)

/*******************************************************************************
 * types
 */

struct LZTable
{
	unk2_t unsigned	at_0x00;	// size 0x02, offset 0x00
	unk2_t unsigned	at_0x02;	// size 0x02, offset 0x02
	unk2_t signed	*at_0x04;	// size 0x04, offset 0x04
	unk2_t signed	*at_0x08;	// size 0x04, offset 0x08
	unk2_t signed	*at_0x0c;	// size 0x04, offset 0x0c
}; // size 0x10

struct at0
{
	unk4_t unsigned	at_0x00;	// size 0x04, offset 0x00
	unk2_t unsigned	at_0x04;	// size 0x02, offset 0x04
	unk2_t signed	at_0x06;	// size 0x02, offset 0x06
	unk2_t signed	at_0x08;	// size 0x02, offset 0x08
	unk2_t signed	at_0x0a;	// size 0x02, offset 0x0a
	unk2_t unsigned	at_0x0c;	// size 0x02, offset 0x0c
	unk2_t unsigned	at_0x0e;	// size 0x02, offset 0x0e
	unk4_t			at_0x10;	// size 0x04, offset 0x10
	unk1_t unsigned	at_0x14;	// size 0x01, offset 0x14
	/* 1 byte padding */
	u16				at_0x16;	// size 0x02, offset 0x16
}; // size 0x18

struct at8
{
	unk1_t unsigned	at_0x00;	// size 0x01, offset 0x00
	unk1_t unsigned	at_0x01;	// size 0x01, offset 0x01
	unk2_t unsigned	at_0x02;	// size 0x02, offset 0x02
	unk2_t unsigned	at_0x04;	// size 0x02, offset 0x04
}; // size 0x06

struct HuffTable
{
	struct at0	*at_0x00;	// size 0x04, offset 0x00
	byte_t		*at_0x04;	// size 0x04, offset 0x04
	struct at8	*at_0x08;	// size 0x04, offset 0x08
	u8			at_0x0c;	// size 0x01, offset 0x0c
	/* 3 bytes padding */
}; // size 0x10

/*******************************************************************************
 * local function declarations
 */

static unk_t SearchLZ(struct LZTable const *table, byte_t const *data, u32,
                      unk2_t unsigned *, unk4_t);
static void LZInitTable(struct LZTable *table, unk2_t *work);
static void SlideByte(struct LZTable *table, byte_t const *data);
static void LZSlide(struct LZTable *table, byte_t const *data, u32 length);

static void HuffInitTable(struct HuffTable *, void *work, u16);

static void HuffCountData(struct at0 *, byte_t const *data, u32 size,
                          u8 huffBitSize);
static u16 HuffConstructTree(struct at0 *, unk_t unsigned);
static void HuffAddParentDepthToTable(struct at0 *, u16, u16);
static void HuffAddCodeToTable(struct at0 *, u16, unk_t);
static u8 HuffAddCountHWordToTable(struct at0 *, u16);
static void HuffMakeHuffTree(struct HuffTable *, unk2_t unsigned);
static void HuffMakeSubsetHuffTree(struct HuffTable *, u16, u8);
static u8 HuffRemainingNodeCanSetOffset(struct HuffTable *, u8);
static void HuffSetOneNodeOffset(struct HuffTable *, u16, u8);
static u32 HuffConvertData(struct at0 *, byte_t const *srcp, byte_t *dstp,
                           u32 size, u32 maxLength, u8 huffBitSize);

/*******************************************************************************
 * functions
 */

u32 CXCompressLZImpl(byte_t const *srcp, u32 size, byte_t *dstp,
                     void *work, BOOL param_5)
{
	u32 length;
	u32 a;
	byte_t b;
	u16 c;
	byte_t *d;

	u8 i;

	u32 f;
	unk4_t long g; // or unsigned

	g = param_5 ? 0x10110 : 0x12;

	OSAssert_Line(111, ((u32)srcp & 0x1) == 0);
	OSAssert_Line(112, work != NULL);
	OSAssert_Line(113, size > 4);

	if (size < 0x1000000)
	{
		*(byte4_t *)dstp =
			CXiConvertEndian32_(size << 8 | CX_COMPRESSION_TYPE_LEMPEL_ZIV
		                        | BOOLIFY_TERNARY(param_5));

		dstp += sizeof(byte4_t);

		length = sizeof(byte4_t);
	}
	else
	{
		*(byte4_t *)dstp = CXiConvertEndian32_(
			CX_COMPRESSION_TYPE_LEMPEL_ZIV | BOOLIFY_TERNARY(param_5));
		dstp += sizeof(byte4_t);

		*(byte4_t *)dstp = CXiConvertEndian32_(size);
		dstp += sizeof(byte4_t);

		length = sizeof(byte4_t) + sizeof(byte4_t);
	}

	f = size;

	struct LZTable table;
	LZInitTable(&table, work);

	while (size)
	{
		b = 0;
		d = dstp++;
		++length;

		for (i = 0; i < 8; ++i)
		{
			b <<= 1;

			if (!size)
				continue;

			a = SearchLZ(&table, srcp, size, &c, g);
			if (a)
			{
				b |= 1;

				if (length + 2 >= f)
					return 0;

				u32 h;

				if (param_5)
				{
					if (a >= 0x111)
					{
						h = a - 0x111;

						*dstp++ = h >> 12 | 0x10;
						*dstp++ = h >> 4;
						length += 2;
					}
					else if (a >= 0x11)
					{
						h = a - 0x11;

						*dstp++ = h >> 4;
						length += 1;
					}
					else
					{
						h = a - 0x1;
					}
				}
				else
				{
					h = a - 3;
				}

				*dstp++ = (h << 4) | (c - 1) >> 8;
				*dstp++ = c - 1;
				length += 2;

				LZSlide(&table, srcp, a);

				srcp += a;
				size -= a;
			}
			else
			{
				if (length + 1 >= f)
					return 0;

				LZSlide(&table, srcp, 1);
				*dstp++ = *srcp++;

				--size;
				++length;
			}
		}

		*d = b;
	}

	for (i = 0; (length + i) % 4 != 0; ++i)
		*dstp++ = 0;

	(void)size;

	return length;
}

static unk_t SearchLZ(struct LZTable const *table, byte_t const *data,
                      u32 param_3, unk2_t unsigned *param_4, unk4_t param_5)
{
	byte_t const *a;
	byte_t const *b;
	byte_t const *c;
	unk2_t unsigned d;
	unk2_t unsigned e;
	unk_t unsigned f;
	unk2_t signed *g;
	unk2_t unsigned h;
	unk_t l;
	unk_t unsigned m;

	m = 2;
	g = table->at_0x04;
	e = table->at_0x00;
	h = table->at_0x02;

	if (param_3 < 3)
		return 0;

	l = table->at_0x08[*data];

	while (l != -1)
	{
		if (l < e)
			b = data - e + l;
		else
			b = data - h - e + l;

		if (b[1] != data[1] || b[2] != data[2])
		{
			l = g[l];
			continue;
		}

		if (data - b < 2)
			break;

		f = 3;
		c = b + 3;
		a = data + 3;

		while (a - data < param_3 && *a == *c)
		{
			++a;
			++c;
			++f;

			if (f == param_5)
				break;
		}

		if (f > m)
		{
			m = f;
			d = data - b;

			if (m == param_5 || m == param_3)
				break;
		}

		l = g[l];
	}

	if (m < 3)
		return 0;

	*param_4 = d;
	return m;
}

static void LZInitTable(struct LZTable *table, unk2_t *work)
{
	table->at_0x04 = work;
	table->at_0x08 = work + 0x1000;
	table->at_0x0c = work + 0x1100;

	u16 i;
	for (i = 0; i < 0x100; ++i)
	{
		table->at_0x08[i] = -1;
		table->at_0x0c[i] = -1;
	}

	table->at_0x00 = 0;
	table->at_0x02 = 0;
}

static void SlideByte(struct LZTable *table, byte_t const *data)
{
	unk1_t unsigned a;
	unk2_t unsigned b;
	unk2_t c;
	unk2_t *d;
	unk2_t *e;
	unk2_t *f;

	a = *data;
	d = table->at_0x08;
	e = table->at_0x04;
	f = table->at_0x0c;

	unk2_t unsigned const g = table->at_0x00;
	unk2_t unsigned const h = table->at_0x02;

	if (h == 0x1000)
	{
		unk1_t unsigned i = data[-0x1000];

		if ((d[i] = e[d[i]]) == -1)
			f[i] = -1;

		b = g;
	}
	else
	{
		b = h;
	}

	c = f[a];

	if (c == -1)
		d[a] = b;
	else
		e[c] = b;

	f[a] = b;
	e[b] = -1;

	if (h == 0x1000)
		table->at_0x00 = (g + 1) % 0x1000;
	else
		++table->at_0x02;
}

static void LZSlide(struct LZTable *table, byte_t const *data, u32 length)
{
	int i;
	for (i = 0; i < length; ++i)
		SlideByte(table, data++);
}

u32 CXCompressRL(byte_t const *srcp, u32 size, byte_t *dstp)
{
	unk_t unsigned a;
	unk_t unsigned b;
	unk_t c;
	unk1_t unsigned d;
	unk1_t unsigned e;
	unk1_t unsigned f;
	byte_t const *g;

	OSAssert_Line(428, srcp != NULL);
	OSAssert_Line(429, dstp != NULL);
	OSAssert_Line(430, size > 4);

	if (size < 0x1000000)
	{
		IN_BUFFER_AT(byte4_t, dstp, 0) =
			CXiConvertEndian32_(size << 8 | CX_COMPRESSION_TYPE_RUN_LENGTH);

		b = sizeof(byte4_t);
	}
	else
	{
		IN_BUFFER_AT(byte4_t, dstp, 0) =
			CXiConvertEndian32_(CX_COMPRESSION_TYPE_RUN_LENGTH);
		IN_BUFFER_AT(byte4_t, dstp, 4) = CXiConvertEndian32_(size);

		b = sizeof(byte4_t) + sizeof(byte4_t);
	}

	c = 0;
	f = 0;
	d = 0;

	while (c < size)
	{
		g = srcp + c;

		for (a = 0; a < 128; ++a)
		{
			if (c + f >= size)
			{
				f = size - c;
				break;
			}

			if (c + f + 2 < size && g[a] == g[a + 1] && g[a] == g[a + 2])
			{
				d = 1;
				break;
			}

			++f;
		}

		if (f)
		{
			if (b + f + 1 >= size)
				return 0;

			dstp[b++] = f - 1;

			for (a = 0; a < f; ++a)
				dstp[b++] = srcp[c++];

			f = 0;
		}

		if (d)
		{
			e = 3;

			// What
			for (a = 0 + 3; a < 127 + 3; ++a)
			{
				if (c + e >= size)
				{
					e = size - c;
					break;
				}

				if (srcp[c] != srcp[c + e])
					break;

				++e;
			}

			if (b + 2 >= size)
				return 0;

			dstp[b++] = (e - 3) | 0x80;
			dstp[b++] = srcp[c];

			c += e;
			d = 0;
		}
	}

	for (a = 0; (b + a) % 4 != 0; ++a)
		dstp[b + a] = 0;

	(void)c;

	return b;
}

u32 CXCompressHuffman(byte_t const *srcp, u32 size, byte_t *dstp,
                      u8 huffBitSize, void *work)
{
	int i;

	u16 a;
	unk2_t unsigned b = 1 << huffBitSize;

	OSAssert_Line(617, srcp != NULL);
	OSAssert_Line(618, dstp != NULL);
	OSAssert_Line(619, huffBitSize == 4 || huffBitSize == 8);
	OSAssert_Line(620, work != NULL);
	OSAssert_Line(621, ((u32)work & 0x3) == 0);
	OSAssert_Line(622, size > 4);

	struct HuffTable table;

	HuffInitTable(&table, work, b);
	HuffCountData(table.at_0x00, srcp, size, huffBitSize);
	a = HuffConstructTree(table.at_0x00, b);
	HuffMakeHuffTree(&table, a);

	*table.at_0x04 = --table.at_0x0c;

	u32 headerLength;
	if (size < 0x1000000)
	{
		IN_BUFFER_AT(byte4_t, dstp, 0) = CXiConvertEndian32_(
			size << 8 | CX_COMPRESSION_TYPE_HUFFMAN | huffBitSize);

		headerLength = sizeof(byte4_t);
	}
	else
	{
		IN_BUFFER_AT(byte4_t, dstp, 0) =
			CXiConvertEndian32_(CX_COMPRESSION_TYPE_HUFFMAN | huffBitSize);
		IN_BUFFER_AT(byte4_t, dstp, 4) = CXiConvertEndian32_(size);

		headerLength = sizeof(byte4_t) + sizeof(byte4_t);
	}

	u32 length = headerLength;

	if (length + ((table.at_0x0c + 1) << 1) >= size)
		return 0;

	for (i = 0; i < (u16)((table.at_0x0c + 1) << 1); ++i)
	{
#if !defined(NDEBUG) // TODO
		dstp[length++] = table.at_0x04[i];
#else
		dstp[length++] = ((u32)table.at_0x04)[(u8 *)i];
#endif
	}

	while (length % 4 != 0)
	{
		if (length % 2 != 0)
		{
			++table.at_0x0c;
			++dstp[headerLength];
		}

		dstp[length++] = 0;
	}

	{ // random block
		unk_t c = HuffConvertData(table.at_0x00, srcp, dstp + length, size,
		                          size - length, huffBitSize);
		if (!c)
			return 0;

		length += c;
	}

	return length;
}

static void HuffInitTable(struct HuffTable *param_1, void *work, u16 param_3)
{
	u32 i;

	// TODO: clean up types here
	param_1->at_0x00 = (struct at0 *)((u32)work + 0);
	param_1->at_0x04 = (byte_t *)((u32)work + 0x3000);
	param_1->at_0x08 = (struct at8 *)((u32)param_1->at_0x04 + 0x200);
	param_1->at_0x0c = 1;

	{ // random block
		struct at0 *a = param_1->at_0x00;
		struct at0 initial = {.at_0x08 = -1, .at_0x0a = -1};

		for (i = 0; i < param_3 << 1; ++i)
		{
			a[i] = initial;
			a[i].at_0x04 = i;
		}
	}

	{ // another random block
		struct at8 initial = {.at_0x00 = 1, .at_0x01 = 1};

		byte_t *b = param_1->at_0x04;
		struct at8 *c = param_1->at_0x08;

		for (i = 0; i < 0x100; ++i)
		{
			b[i * 2 + 0] = 0;
			b[i * 2 + 1] = 0;

			c[i] = initial;
		}
	}
}

static void HuffCountData(struct at0 *param_1, byte_t const *data, u32 size,
                          u8 huffBitSize)
{
	u32 i;

	byte_t a;

	if (huffBitSize == 8)
	{
		for (i = 0; i < size; ++i)
			++param_1[data[i]].at_0x00;
	}
	else // (huffBitSize == 4)
	{
		for (i = 0; i < size; ++i)
		{
			a = (data[i] & 0xf0) >> 4;
			++param_1[a].at_0x00;

			a = (data[i] & 0x0f) >> 0;
			++param_1[a].at_0x00;
		}
	}
}

static u16 HuffConstructTree(struct at0 *param_1, unk_t unsigned param_2)
{
	int i;

	unk_t signed a;
	unk_t signed b;
	u16 c = param_2;

	a = -1;
	b = -1;

	u16 d;
	u16 e ATTR_UNUSED; // ?

	while (true)
	{
		for (i = 0; i < c; ++i)
		{
			if (param_1[i].at_0x00 && !param_1[i].at_0x06)
			{
				if (a < 0)
					a = i;
				else if (param_1[i].at_0x00 < param_1[a].at_0x00)
					a = i;
			}
		}

		for (i = 0; i < c; ++i)
		{
			if (param_1[i].at_0x00 && !param_1[i].at_0x06 && i != a)
			{
				if (b < 0)
					b = i;
				else if (param_1[i].at_0x00 < param_1[b].at_0x00)
					b = i;
			}
		}

		if (b < 0)
		{
			if (c == param_2)
			{
				param_1[c].at_0x00 = param_1[a].at_0x00;
				param_1[c].at_0x08 = a;
				param_1[c].at_0x0a = a;
				param_1[c].at_0x0e = 1;

				param_1[a].at_0x06 = c;
				param_1[a].at_0x14 = 0;
				param_1[a].at_0x0c = 1;
			}
			else
			{
				--c;
			}

			d = c;
			e = (((d - param_2) + 1) << 1) + 1;

			break;
		}

		param_1[c].at_0x00 = param_1[a].at_0x00 + param_1[b].at_0x00;
		param_1[c].at_0x08 = a;
		param_1[c].at_0x0a = b;

		if (param_1[a].at_0x0e > param_1[b].at_0x0e)
			param_1[c].at_0x0e = param_1[a].at_0x0e + 1;
		else
			param_1[c].at_0x0e = param_1[b].at_0x0e + 1;

		param_1[a].at_0x06 = param_1[b].at_0x06 = c;
		param_1[a].at_0x14 = 0;
		param_1[b].at_0x14 = 1;

		HuffAddParentDepthToTable(param_1, a, b);

		++c;
		a = b = -1;
	}

	HuffAddCodeToTable(param_1, d, 0);
	HuffAddCountHWordToTable(param_1, d);

	return d;
}

static void HuffAddParentDepthToTable(struct at0 *param_1, u16 param_2,
                                      u16 param_3)
{
	++param_1[param_2].at_0x0c;
	++param_1[param_3].at_0x0c;

	if (param_1[param_2].at_0x0e)
	{
		HuffAddParentDepthToTable(param_1, param_1[param_2].at_0x08,
		                          param_1[param_2].at_0x0a);
	}

	if (param_1[param_3].at_0x0e)
	{
		HuffAddParentDepthToTable(param_1, param_1[param_3].at_0x08,
		                          param_1[param_3].at_0x0a);
	}
}

static void HuffAddCodeToTable(struct at0 *param_1, u16 param_2, unk_t param_3)
{
	param_1[param_2].at_0x10 = param_3 << 1 | param_1[param_2].at_0x14;

	if (param_1[param_2].at_0x0e)
	{
		HuffAddCodeToTable(param_1, param_1[param_2].at_0x08,
		                   param_1[param_2].at_0x10);
		HuffAddCodeToTable(param_1, param_1[param_2].at_0x0a,
		                   param_1[param_2].at_0x10);
	}
}

static u8 HuffAddCountHWordToTable(struct at0 *param_1, u16 param_2)
{
	u8 a;
	u8 b;

	switch (param_1[param_2].at_0x0e)
	{
	case 0:
		return 0;

	case 1:
		a = b = 0;
		break;

	default:
		a = HuffAddCountHWordToTable(param_1, param_1[param_2].at_0x08);
		b = HuffAddCountHWordToTable(param_1, param_1[param_2].at_0x0a);
		break;
	}

	param_1[param_2].at_0x16 = a + b + 1;

	return a + b + 1;
}

static void HuffMakeHuffTree(struct HuffTable *param_1, unk2_t unsigned param_2)
{
	s16 i;

	s16 a;
	s16 b;

	unk2_t c;
	unk2_t d;
	s16 e;
	s16 f;
	u16 g;
	unk1_t h ATTR_UNUSED; // ?

	u8 l;

	param_1->at_0x0c = 1;
	d = 0;
	param_1->at_0x08->at_0x00 = 0;
	param_1->at_0x08->at_0x04 = param_2;

loop:
	while (true)
	{
		u8 m = 0;

		for (i = 0; i < param_1->at_0x0c; ++i)
		{
			if (param_1->at_0x08[i].at_0x00)
				++m;

			if (param_1->at_0x08[i].at_0x01)
				++m;
		}

		c = -1;
		b = -1;
		h = 0;
		l = 0;

		for (i = 0; i < param_1->at_0x0c; ++i)
		{
			e = (u8)(param_1->at_0x0c - i);

			if (param_1->at_0x08[i].at_0x00)
			{
				a = param_1->at_0x00[param_1->at_0x08[i].at_0x02].at_0x16;

				if (a + m <= 64 && HuffRemainingNodeCanSetOffset(param_1, a))
				{
					if (a > c)
					{
						b = i;
						f = 0;
					}
					else if (a == c && e > d)
					{
						b = i;
						f = 0;
					}
				}
			}

			if (param_1->at_0x08[i].at_0x01)
			{
				a = param_1->at_0x00[param_1->at_0x08[i].at_0x04].at_0x16;

				if (a + m <= 64 && HuffRemainingNodeCanSetOffset(param_1, a))
				{
					if (a > c)
					{
						b = i;
						f = 1;
					}
					else if (a == c && e > d)
					{
						b = i;
						f = 1;
					}
				}
			}
		}

		if (b >= 0)
			HuffMakeSubsetHuffTree(param_1, b & 0xff, f);
		else
			break;

		(void)m;
	}

	for (i = 0; i < param_1->at_0x0c; ++i)
	{
		g = 0;
		l = 0;

		if (param_1->at_0x08[i].at_0x00)
			g = param_1->at_0x00[param_1->at_0x08[i].at_0x02].at_0x16;

		if (param_1->at_0x08[i].at_0x01
		    && param_1->at_0x00[param_1->at_0x08[i].at_0x04].at_0x16 > g)
		{
			l = 1;
		}

		if (g || l)
		{
			HuffSetOneNodeOffset(param_1, i & 0xff, l);
			goto loop;
		}
	}

	(void)b;
}

static void HuffMakeSubsetHuffTree(struct HuffTable *param_1, u16 param_2,
                                   u8 param_3)
{
	u8 i = param_1->at_0x0c;

	HuffSetOneNodeOffset(param_1, param_2, param_3);

	if (param_3)
		param_1->at_0x08[param_2].at_0x01 = 0;
	else
		param_1->at_0x08[param_2].at_0x00 = 0;

	for (; i < param_1->at_0x0c; ++i)
	{
		if (param_1->at_0x08[i].at_0x00)
		{
			HuffSetOneNodeOffset(param_1, i, 0);
			param_1->at_0x08[i].at_0x00 = 0;
		}

		if (param_1->at_0x08[i].at_0x01)
		{
			HuffSetOneNodeOffset(param_1, i, 1);
			param_1->at_0x08[i].at_0x01 = 0;
		}
	}
}

static u8 HuffRemainingNodeCanSetOffset(struct HuffTable *param_1, u8 param_2)
{
	u8 i;

	s16 a = 64 - param_2;

	for (i = 0; i < param_1->at_0x0c; ++i)
	{
		if (param_1->at_0x08[i].at_0x00)
		{
			if (param_1->at_0x0c - i <= a)
				--a;
			else
				return false;
		}

		if (param_1->at_0x08[i].at_0x01)
		{
			if (param_1->at_0x0c - i <= a)
				--a;
			else
				return false;
		}
	}

	return true;
}

static void HuffSetOneNodeOffset(struct HuffTable *param_1, u16 param_2,
                                 u8 param_3)
{
	u16 a;
	byte_t b = 0;

	struct at0	*table0 = param_1->at_0x00;
	byte_t		*table4 = param_1->at_0x04;
	struct at8	*table8 = param_1->at_0x08;
	u8			tablec = param_1->at_0x0c;

	if (param_3 != 0)
	{
		a = table8[param_2].at_0x04;
		table8[param_2].at_0x01 = 0;
	}
	else // if (param_3 == 0)
	{
		a = table8[param_2].at_0x02;
		table8[param_2].at_0x00 = 0;
	}

	if (!table0[table0[a].at_0x08].at_0x0e)
	{
		b |= 0x80;

		table4[tablec * 2 + 0] = table0[a].at_0x08;
		table8[tablec].at_0x02 = (u8)table0[a].at_0x08;
		table8[tablec].at_0x00 = 0;
	}
	else
	{
		table8[tablec].at_0x02 = table0[a].at_0x08;
	}

	if (!table0[table0[a].at_0x0a].at_0x0e)
	{
		b |= 0x40;

		table4[tablec * 2 + 1] = table0[a].at_0x0a;
		table8[tablec].at_0x04 = (u8)table0[a].at_0x0a;
		table8[tablec].at_0x01 = 0;
	}
	else
	{
		table8[tablec].at_0x04 = table0[a].at_0x0a;
	}

	b |= (u8)(tablec - param_2 - 1);

	table4[param_2 * 2 + param_3] = b;
	++param_1->at_0x0c;
}

static u32 HuffConvertData(struct at0 *param_1, byte_t const *srcp,
                           byte_t *dstp, u32 size, u32 maxLength,
                           u8 huffBitSize)
{
	u32 i, j, k;

	unk_t unsigned a = 0;
	unk_t unsigned b = 0;
	u32 length = 0;

	for (i = 0; i < size; ++i)
	{
		byte_t d;
		byte_t e = srcp[i];

		if (huffBitSize == 8)
		{
			a = a << param_1[e].at_0x0c | param_1[e].at_0x10;
			b += param_1[e].at_0x0c;

			if (length + b / 8 >= maxLength)
				return 0;

			for (j = 0; j < b / 8; ++j)
				dstp[length++] = a >> (b - ((j + 1) << 3));

			b %= 8;
		}
		else // if (huffBitSize == 4)
		{
			for (j = 0; j < 8 / 4; ++j)
			{
				if (j != 0)
					d = e >> 4;
				else
					d = e & 0x0f;

				a = (a << param_1[d].at_0x0c) | param_1[d].at_0x10;
				b += param_1[d].at_0x0c;

				if (length + b / 8 >= maxLength)
					return 0;

				for (k = 0; k < b / 8; ++k)
					dstp[length++] = a >> (b - (k + 1) * 8);

				b %= 8;
			}
		}
	}

	if (b)
	{
		if (length + 1 >= maxLength)
			return 0;
		else
			dstp[length++] = a << (8 - b);
	}

	while (length % 4 != 0)
		dstp[length++] = 0;

	// I Love Reinventing stwbrx !
	for (i = 0; i < length / sizeof(byte4_t); ++i)
	{
		byte_t tmp;

		tmp = dstp[i * sizeof(byte4_t) + 0];
		dstp[i * sizeof(byte4_t) + 0] = dstp[i * sizeof(byte4_t) + 3];
		dstp[i * sizeof(byte4_t) + 3] = tmp;

		tmp = dstp[i * sizeof(byte4_t) + 1];
		dstp[i * sizeof(byte4_t) + 1] = dstp[i * sizeof(byte4_t) + 2];
		dstp[i * sizeof(byte4_t) + 2] = tmp;
	}

	return length;
}
