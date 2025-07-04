#ifndef RVL_SDK_CX_COMPRESSION_H
#define RVL_SDK_CX_COMPRESSION_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * functions
 */

#ifdef __cplusplus
	extern "C" {
#endif

u32 CXCompressLZImpl(byte_t const *srcp, u32 size, byte_t *dstp,
                     void *work, BOOL);
u32 CXCompressRL(byte_t const *srcp, u32 size, byte_t *dstp);
u32 CXCompressHuffman(byte_t const *srcp, u32 size, byte_t *dstp,
                      u8 huffBitSize, void *work);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_CX_COMPRESSION_H
