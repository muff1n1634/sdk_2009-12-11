#ifndef RVL_SDK_CX_H
#define RVL_SDK_CX_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef byte_t CXCompressionType;
enum CXCompressionType_et
{
	CX_COMPRESSION_TYPE_LEMPEL_ZIV	= 0x10,
	CX_COMPRESSION_TYPE_HUFFMAN		= 0x20,
	CX_COMPRESSION_TYPE_RUN_LENGTH	= 0x30,
	CX_COMPRESSION_TYPE_LH			= 0x40,
	CX_COMPRESSION_TYPE_LRC			= 0x50,
	CX_COMPRESSION_TYPE_FILTER_DIFF	= 0x80,

	CX_COMPRESSION_TYPE_MASK		= 0xf0
};

typedef struct CXCompressionHeader
{
	CXCompressionType	type;	// size 0x01, offset 0x00
	byte_t				stat;	// size 0x01, offset 0x01 // Meaning changes depending on type
	/* 2 bytes padding */
	u32					size;	// size 0x04, offset 0x04
} CXCompressionHeader; // size 0x08

/*******************************************************************************
 * functions
 */

CXCompressionHeader CXGetCompressionHeader(void const *compressed);

static inline int CXGetCompressionType(void const *compressed)
{
	return *(byte_t *)compressed & CX_COMPRESSION_TYPE_MASK;
}

u32 CXGetUncompressedSize(void const *compressed);

#ifdef __cplusplus
	}
#endif

/*******************************************************************************
 * sub-headers
 */

#include <revolution/cx/CXCompression.h>
#include <revolution/cx/CXSecureUncompression.h>
#include <revolution/cx/CXStreamingUncompression.h>
#include <revolution/cx/CXUncompression.h>

#endif // RVL_SDK_CX_H
