#ifndef RVL_SDK_KPAD_MOTION_PLUS_H
#define RVL_SDK_KPAD_MOTION_PLUS_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include <revolution/kpad.h>

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef void KMPLSSamplingCallback(KPADChannel chan, KPADMPStatus *mpStatus);

/*******************************************************************************
 * functions
 */

void KMPLSInit(void *ptr);
u32 KMPLSGetLibWorkSize(void);
BOOL KMPLSIsInit(void);
void KMPLSShutdown(void);
void KMPLSReset(KPADChannel chan);

void KMPLSSetAngle(KPADChannel chan, f32 x, f32 y, f32 z);
void KMPLSSetDirection(KPADChannel chan, KPADMPDir *dir);
void KMPLSSetDirectionMag(KPADChannel chan, f32 mag);
void KMPLSSetDegreeMag(KPADChannel chan, f32 x, f32 y, f32 z);

void KMPLSEnableZeroPlay(KPADChannel chan);
void KMPLSDisableZeroPlay(KPADChannel chan);
f32 KMPLSIsEnableZeroPlay(KPADChannel chan);
void KMPLSSetZeroPlayParam(KPADChannel chan, f32 param1);
void KMPLSGetZeroPlayParam(KPADChannel chan, f32 *param1);
void KMPLSInitZeroPlayParam(KPADChannel chan);

void KMPLSEnableZeroDrift(KPADChannel chan);
void KMPLSDisableZeroDrift(KPADChannel chan);
f32 KMPLSIsEnableZeroDrift(KPADChannel chan);
void KMPLSSetZeroDriftParam(KPADChannel chan, f32 param1, s32 param2,
                            f32 param3);
void KMPLSGetZeroDriftParam(KPADChannel chan, f32 *param1, s32 *param2,
                            f32 *param3);
void KMPLSInitZeroDriftParam(KPADChannel chan);

void KMPLSSetZeroDriftStandard(KPADChannel chan);
void KMPLSSetZeroDriftTight(KPADChannel chan);
void KMPLSSetZeroDriftLoose(KPADChannel chan);

void KMPLSEnableDirRevise(KPADChannel chan);
void KMPLSDisableDirRevise(KPADChannel chan);
f32 KMPLSIsEnableDirRevise(KPADChannel chan);
void KMPLSSetDirReviseParam(KPADChannel chan, f32 param1);
void KMPLSGetDirReviseParam(KPADChannel chan, f32 *param1);
void KMPLSInitDirReviseParam(KPADChannel chan);

void KMPLSSetDirReviseBase(KPADChannel chan, KPADMPDir *dir);

void KMPLSEnableAccRevise(KPADChannel chan);
void KMPLSDisableAccRevise(KPADChannel chan);
f32 KMPLSIsEnableAccRevise(KPADChannel chan);
void KMPLSSetAccReviseParam(KPADChannel chan, f32 param1, f32 param2);
void KMPLSGetAccReviseParam(KPADChannel chan, f32 *param1, f32 *param2);
void KMPLSInitAccReviseParam(KPADChannel chan);

void KMPLSEnableDpdRevise(KPADChannel chan);
void KMPLSDisableDpdRevise(KPADChannel chan);
f32 KMPLSIsEnableDpdRevise(KPADChannel chan);
void KMPLSSetDpdReviseParam(KPADChannel chan, f32 param1);
void KMPLSGetDpdReviseParam(KPADChannel chan, f32 *param1);
void KMPLSInitDpdReviseParam(KPADChannel chan);

void KMPLSSetLimitDamp(f32 limit);
void KMPLSGetLimitDamp(f32 *limit);
void KMPLSInitLimitDamp(void);
u32 KMPLSGetCountX(KPADChannel chan);
u32 KMPLSGetCountY(KPADChannel chan);
u32 KMPLSGetCountZ(KPADChannel chan);
f32 KMPLSGetZeroCountX(KPADChannel chan);
f32 KMPLSGetZeroCountY(KPADChannel chan);
f32 KMPLSGetZeroCountZ(KPADChannel chan);

void KMPLSSetKpadRingBuffer(KPADChannel chan, KPADUnifiedWpadStatus *,
                            KPADUnifiedWpadStatus *, u32);
void KMPLSRead(KPADChannel chan, KPADMPStatus *mpStatus, s32 offset, s32 count,
               void *kpad_p, f64);
void KMPLSSetSamplingCallback(KMPLSSamplingCallback *samplingCb);
void KMPLSStartCalibration(KPADChannel chan);
f32 KMPLSWorkCalibration(KPADChannel chan);
void KMPLSStopCalibration(KPADChannel chan);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_KPAD_MOTION_PLUS_H
