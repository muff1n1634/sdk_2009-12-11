#ifndef RVL_SDK_CX_SECURE_UNCOMPRESSION_H
#define RVL_SDK_CX_SECURE_UNCOMPRESSION_H

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

typedef s32 CXSecureResult;
enum CXSecureResult_et
{
	CX_SECURE_ERR_OK				=  0,
	CX_SECURE_ERR_BAD_FILE_TYPE		= -1,
	CX_SECURE_ERR_BUFFER_TOO_SMALL	= -2,
	CX_SECURE_ERR_BUFFER_TOO_LARGE	= -3,
	CX_SECURE_ERR_BAD_FILE_SIZE		= -4,
	CX_SECURE_ERR_BAD_FILE_TABLE	= -5,

#define CXSECURE_ESUCCESS	CX_SECURE_ERR_OK
// in a heavy deviation from POSIX,
#define CXSECURE_EBADTYPE	CX_SECURE_ERR_BAD_FILE_TYPE
#define CXSECURE_E2SMALL	CX_SECURE_ERR_BUFFER_TOO_SMALL
#define CXSECURE_E2BIG		CX_SECURE_ERR_BUFFER_TOO_LARGE
#define CXSECURE_EBADSIZE	CX_SECURE_ERR_BAD_FILE_SIZE
#define CXSECURE_EBADTABLE	CX_SECURE_ERR_BAD_FILE_TABLE
};

/*******************************************************************************
 * functions
 */

CXSecureResult CXSecureUncompressAny(void const *compressed, u32 length,
                                     void *uncompressed);

CXSecureResult CXSecureUncompressRL(void const *compressed, u32 length,
                                    void *uncompressed);
CXSecureResult CXSecureUncompressLZ(void const *compressed, u32 length,
                                    void *uncompressed);
CXSecureResult CXSecureUncompressHuffman(void const *compressed, u32 length,
                                         void *uncompressed);
CXSecureResult CXSecureUnfilterDiff(void const *compressed, u32 length,
                                    void *uncompressed);
CXSecureResult CXSecureUncompressLH(void const *compressed, u32 length,
                                    byte_t *uncompressed, u16 *);
CXSecureResult CXSecureUncompressLRC(void const *compressed, u32 length,
                                     byte_t *uncompressed, unsigned *);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_CX_SECURE_UNCOMPRESSION_H
