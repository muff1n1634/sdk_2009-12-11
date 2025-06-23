#ifndef RVL_SDK_KPAD_KZ_MOTION_PLUS_TEST_SUB_H
#define RVL_SDK_KPAD_KZ_MOTION_PLUS_TEST_SUB_H

/* no public header */

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include <revolution/kpad.h>

#if 0
#include <revolution/mtx/MTXTypes.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * external variables
 */

#ifdef __cplusplus
	extern "C" {
#endif

extern Vec Fxyz_0;
extern KPADMPDir e_dir;

/*******************************************************************************
 * functions
 */

int __KMPLS_f2i(f32 x);
f32 __KMPLS_normalize_Fxyz(Vec *vec);
void __KMPLS_normalize_directionXYZ(KPADMPDir *mpDir, f32);
void __KMPLS_make_vec_dir(KPADMPDir *mpDir, Vec *vecA, Vec *vecB);
void __KMPLS_mult_dir(KPADMPDir *a, KPADMPDir *b, KPADMPDir *dst);
void __KMPLS_linear_Fxyz(Vec *a, Vec *b, f32, Vec *dst);
void __KMPLS_linear_Direction(KPADMPDir *a, KPADMPDir *b, f32, KPADMPDir *dst);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_KPAD_KZ_MOTION_PLUS_TEST_SUB_H
