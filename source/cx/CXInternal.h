#ifndef RVL_SDK_CX_INTERNAL_H
#define RVL_SDK_CX_INTERNAL_H

/* internal header */

/*******************************************************************************
 * headers
 */

#include <decomp.h>

#include <revolution/types.h>

#include <revolution/cx/CXUncompression.h>

/*******************************************************************************
 * macros
 */

// personal macro
#define IN_BUFFER_AT(type_, buf_, offset_)	(*((__typeof__(type_) *)((u32)(buf_) + (u32)(offset_))))

#define CXiConvertEndian32_	CXiConvertEndian_

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

/*******************************************************************************
 * functions
 */

_Bool CXiVerifyHuffmanTable_(void const *, u8);
_Bool CXiLHVerifyTable(void const *, u8);

static inline byte4_t CXiConvertEndian_(byte4_t x)
{
	// clang-format off
	return ((x & 0xff000000) >> 24)
	     | ((x & 0x00ff0000) >>  8)
	     | ((x & 0x0000ff00) <<  8)
	     | ((x & 0x000000ff) << 24);
	// clang-format on
}

static inline byte2_t CXiConvertEndian16_(byte2_t x)
{
	// clang-format off
	return ((x & 0xff00) >> 8)
	     | ((x & 0x00ff) << 8);
	// clang-format on
}

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_CX_INTERNAL_H
