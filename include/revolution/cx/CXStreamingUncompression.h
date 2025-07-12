#ifndef RVL_SDK_CX_STREAMING_UNCOMPRESSION_H
#define RVL_SDK_CX_STREAMING_UNCOMPRESSION_H

/*******************************************************************************
 * headers
 */

#include <decomp.h>

#include <revolution/types.h>

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

/* Technically CXStreamingResult is the same as CXSecureResult, but it is
 * deduplicated here to avoid pulling in CXSecureCompression.h
 */
typedef s32 CXStreamingResult;
enum CXStreamingResult_et
{
	CX_STREAMING_ERR_OK					=  0,
	CX_STREAMING_ERR_BAD_FILE_TYPE		= -1,
	CX_STREAMING_ERR_BUFFER_TOO_SMALL	= -2,
	CX_STREAMING_ERR_BUFFER_TOO_LARGE	= -3,
	CX_STREAMING_ERR_BAD_FILE_SIZE		= -4,
	CX_STREAMING_ERR_BAD_FILE_TABLE		= -5,

#define CXSTREAM_ESUCCESS	CX_STREAMING_ERR_OK
#define CXSTREAM_EBADTYPE	CX_STREAMING_ERR_BAD_FILE_TYPE
#define CXSTREAM_E2SMALL	CX_STREAMING_ERR_BUFFER_TOO_SMALL
#define CXSTREAM_E2BIG		CX_STREAMING_ERR_BUFFER_TOO_LARGE
#define CXSTREAM_EBADSIZE	CX_STREAMING_ERR_BAD_FILE_SIZE
#define CXSTREAM_EBADTABLE	CX_STREAMING_ERR_BAD_FILE_TABLE
};

typedef struct CXUncompContextRL
{
	byte_t			*at_0x00;	// size 0x04, offset 0x00
	unk4_t signed	at_0x04;	// size 0x04, offset 0x04
	unk4_t			at_0x08;	// size 0x04, offset 0x08
	unk2_t unsigned	at_0x0c;	// size 0x02, offset 0x0c
	unk1_t unsigned	at_0x0e;	// size 0x01, offset 0x0e
	u8				at_0x0f;	// size 0x01, offset 0x0f
} CXUncompContextRL; // size 0x10?

typedef struct CXUncompContextLZ
{
	byte_t			*at_0x00;	// size 0x04, offset 0x00
	unk4_t			at_0x04;	// size 0x04, offset 0x04
	unk4_t			at_0x08;	// size 0x04, offset 0x08
	unk4_t signed	at_0x0c;	// size 0x04, offset 0x0c
	unk1_t unsigned	at_0x10;	// size 0x01, offset 0x10
	unk1_t unsigned	at_0x11;	// size 0x01, offset 0x11
	unk1_t unsigned	at_0x12;	// size 0x01, offset 0x12
	u8				at_0x13;	// size 0x01, offset 0x13
	unk1_t unsigned	at_0x14;	// size 0x01, offset 0x14
	char pad0_[3];
} CXUncompContextLZ; // size 0x18?

typedef struct CXUncompContextHuffman
{
	unk4_t			*at_0x00;	// size 0x04, offset 0x00
	unk4_t			at_0x04;	// size 0x04, offset 0x04
	unk4_t			at_0x08;	// size 0x04, offset 0x08
	byte_t			*at_0x0c;	// size 0x04, offset 0x0c
	unk4_t unsigned	at_0x10;	// size 0x04, offset 0x10
	unk4_t unsigned	at_0x14;	// size 0x04, offset 0x14
	s16				at_0x18;	// size 0x01, offset 0x18
	unk1_t unsigned	at_0x1a;	// size 0x01, offset 0x1a
	unk1_t unsigned	at_0x1b;	// size 0x01, offset 0x1b
	unk1_t unsigned	at_0x1c;	// size 0x01, offset 0x1c
	u8				at_0x1d;	// size 0x01, offset 0x1d
	char pad0_[2];
	byte_t			at_0x20[2];	// size 0x??, offset 0x20
} CXUncompContextHuffman; // size 0x??

typedef struct CXUncompContextLH
{
	unk1_t unsigned	*at_0x000;		// size 0x004, offset 0x000
	unk4_t			at_0x004;		// size 0x004, offset 0x004
	unk4_t			at_0x008;		// size 0x004, offset 0x008
	u16				at_0x00c[2];	// size 0x???, 0ffset 0x00c
	char pad0_[0x80c - (0x00c + 0x004)];
	u16				at_0x80c[1];	// size 0x???, 0ffset 0x80c
	char pad1_[0x88c - (0x80c + 0x002)];
	void			*at_0x88c;		// size 0x004, offset 0x88c
	unk4_t			at_0x890;		// size 0x004, offset 0x890
	unk4_t			at_0x894;		// size 0x004, offset 0x894
	unk4_t			at_0x898;		// size 0x004, offset 0x898
	unk4_t			at_0x89c;		// size 0x004, offset 0x89c
	unk4_t			at_0x8a0;		// size 0x004, offset 0x8a0
	unk2_t unsigned	at_0x8a4;		// size 0x002, offset 0x8a4
	unk1_t signed	at_0x8a6;		// size 0x001, offset 0x8a6
	unk1_t unsigned	at_0x8a7;		// size 0x001, offset 0x8a7
} CXUncompContextLH; // size 0x8a8?

typedef struct CXUncompContextLRC
{
	unk1_t unsigned	*at_0x0000;	// size 0x0004, offset 0x0000
	unk4_t			at_0x0004;	// size 0x0004, offset 0x0004
	unk4_t			at_0x0008;	// size 0x0004, offset 0x0008
	unk_t unsigned	at_0x000c;	// size 0x0004, offset 0x000c
	char pad0_[0x080c - (0x000c + 0x0004)];
	unk_t unsigned	at_0x080c;	// size 0x0004, offset 0x080c
	char pad1_[0x100c - (0x080c + 0x0004)];
	unk_t unsigned	at_0x100c;	// size 0x0004, offset 0x100c
	char pad2_[0x500c - (0x100c + 0x0004)];
	unk_t unsigned	at_0x500c;	// size 0x0004, offset 0x500c
	char pad3_[0x900c - (0x500c + 0x0004)];
	unk4_t			at_0x900c;	// size 0x0004, offset 0x900c
	unk4_t			at_0x9010;	// size 0x0004, offset 0x9010
	unk4_t			at_0x9014;	// size 0x0004, offset 0x9014
	unk4_t			at_0x9018;	// size 0x0004, offset 0x9018
	unk4_t			at_0x901c;	// size 0x0004, offset 0x901c
	unk4_t			at_0x9020;	// size 0x0004, offset 0x9020
	unk1_t unsigned	at_0x9024;	// size 0x0001, offset 0x9024
	unk1_t unsigned	at_0x9025;	// size 0x0001, offset 0x9025
	unk2_t unsigned	at_0x9026;	// size 0x0002, offset 0x9026
	u8				at_0x9028;	// size 0x0001, offset 0x9028
} CXUncompContextLRC; // size 0x902c?

/*******************************************************************************
 * functions
 */

void CXInitUncompContextRL(CXUncompContextRL *context, byte_t *);
void CXInitUncompContextLZ(CXUncompContextLZ *context, byte_t *);
void CXInitUncompContextHuffman(CXUncompContextHuffman *context, unk4_t *);
void CXInitUncompContextLH(CXUncompContextLH *context, byte_t *);
void CXInitUncompContextLRC(CXUncompContextLRC *context, byte_t *);

CXStreamingResult CXReadUncompRL(CXUncompContextRL *context,
                                 void const *compressed, u32 size);
CXStreamingResult CXReadUncompLZ(CXUncompContextLZ *context,
                                 void const *compressed, u32 size);
CXStreamingResult CXReadUncompHuffman(CXUncompContextHuffman *context,
                                      void const *compressed, u32 size);
CXStreamingResult CXReadUncompLH(CXUncompContextLH *context,
                                 void const *compressed, u32 size);
CXStreamingResult CXReadUncompLRC(CXUncompContextLRC *context,
                                  void const *compressed, u32 size);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_CX_STREAMING_UNCOMPRESSION_H
