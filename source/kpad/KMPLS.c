#include "kmpls_main.h"

/*******************************************************************************
 * headers
 */

#include <math.h>

#include <decomp.h>
#include <macros.h>

#include <revolution/types.h>

#include <revolution/kpad.h>
#include "KZMplsTestSub.h"

#include <revolution/wpad.h>
#include "../wpad/WPAD.h"

#if 0
#include <revolution/os/OSAssert.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * macros
 */

#undef NULL
#define NULL					((void *)0)

#define CALIBRATION_CONSTANT	5242.879375 // see set_calibration_data

/*******************************************************************************
 * types
 */

typedef s8 ControlMplsStatus;
enum ControlMplsStatus_et
{
	ControlMplsFailed = -1,
	ControlMplsNotRun = 0,
	ControlMplsSucceeded = 1,
};

typedef struct
{
	KPADMPStatus			mpStatus;			// size 0x03c, offset 0x000
	Vec						at_0x03c;			// size 0x00c, offset 0x03c
	Vec						at_0x048;			// size 0x004, offset 0x048
	f32						at_0x054;			// size 0x004, offset 0x054
	f32						at_0x058;			// size 0x004, offset 0x058
	f32						at_0x05c;			// size 0x004, offset 0x05c
	KPADMPDir				dir;				// size 0x024, offset 0x060
	KPADMPDir				dirReviseBase;		// size 0x024, offset 0x084
	f32						dirMag;				// size 0x004, offset 0x0a8
	f32						at_0x0ac;			// size 0x004, offset 0x0ac
	f32						at_0x0b0;			// size 0x004, offset 0x0b0
	f32						at_0x0b4;			// size 0x004, offset 0x0b4
	u8						calibrating;		// size 0x001, offset 0x0b8
	u8						dampenXCount;		// size 0x001, offset 0x0b9 // unused counter?
	u8						dampenYCount;		// size 0x001, offset 0x0ba // unused counter?
	u8						dampenZCount;		// size 0x001, offset 0x0bb // unused counter?
	u8						unitNo;				// size 0x001, offset 0x0bc
	u8						zeroPlayEnabled;	// size 0x001, offset 0x0bd
	u8						zeroDriftEnabled;	// size 0x001, offset 0x0be
	u8						dirReviseEnabled;	// size 0x001, offset 0x0bf
	u8						accReviseEnabled;	// size 0x001, offset 0x0c0
	u8						dpdReviseEnabled;	// size 0x001, offset 0x0c1
	s8						wpadCBStatus;		// size 0x001, offset 0x0c2
	/* 1 byte padding */
	f32						at_0x0c4;			// size 0x004, offset 0x0c4
	f32						zeroPlayParam;		// size 0x004, offset 0x0c8
	f32						zeroDriftParam1;	// size 0x004, offset 0x0cc
	unk4_t signed			zeroDriftParam2;	// size 0x004, offset 0x0d0
	f32						zeroDriftParam3;	// size 0x004, offset 0x0d4
	f32						dirReviseParam;		// size 0x004, offset 0x0d8
	Vec2					accReviseParam;		// size 0x008, offset 0x0dc
	f32						dpdReviseParam;		// size 0x004, offset 0x0e4
	f32						at_0x0e8;			// size 0x004, offset 0x0e8
	f32						at_0x0ec;			// size 0x004, offset 0x0ec
	f32						dirRevised;			// size 0x004, offset 0x0f0
	f32						accRevised;			// size 0x004, offset 0x0f4
	f32						dpdRevised;			// size 0x004, offset 0x0f8
	f32						at_0x0fc;			// size 0x004, offset 0x0fc
	BOOL					aimingModeEnabled;	// size 0x004, offset 0x100
	f32						sensorHeight;		// size 0x004, offset 0x104
	f64						highPitchScale;		// size 0x008, offset 0x108
	f64						highYawScale;		// size 0x008, offset 0x110
	f64						highRollScale;		// size 0x008, offset 0x118
	f64						lowPitchScale;		// size 0x008, offset 0x120
	f64						lowYawScale;		// size 0x008, offset 0x128
	f64						lowRollScale;		// size 0x008, offset 0x130
	WPADMplsCalibration		mplsCalibHigh;		// size 0x01c, offset 0x138
	WPADMplsCalibration		mplsCalibLow;		// size 0x01c, offset 0x154
	Vec						fxyz;				// size 0x004, offset 0x170
	u32						at_0x17c;			// size 0x004, offset 0x17c
	s32						at_0x180[3][0x100];	// size 0x004, offset 0x180
	f64						at_0xd80;			// size 0x008, offset 0xd80
	f64						at_0xd88;			// size 0x008, offset 0xd88
	f64						at_0xd90;			// size 0x008, offset 0xd90
	KPADUnifiedWpadStatus	*at_0xd98;		// size 0x004, offset 0xd98
	KPADUnifiedWpadStatus	*at_0xd9c;			// size 0x004, offset 0xd9c
	u32						at_0xda0;		// size 0x004, offset 0xda0
	Vec						degreeMag;			// size 0x00c, offset 0xda4
} kmpls_inst_st;

typedef struct
{
	f32						dampLimit;
	KMPLSSamplingCallback	*samplingCb;
	kmpls_inst_st			inst[WPAD_MAX_CONTROLLERS];
} kmpls_st;

/*******************************************************************************
 * local function declarations
 */

static void reset_stability(KPADChannel chan);
static void wpad_callback_func(WPADChannel chan, WPADResult result);
static void work_calibration(KPADChannel chan);

static void set_unit_no(KPADChannel chan, s32 unitNo);

static void dpd_revise_scale(KPADChannel chan);

static f32 move_mpls_orient(kmpls_inst_st *kmplsInst, Vec *, Vec *, Vec *);
static f32 revise_dir_dir(KPADChannel chan);
static f32 revise_dir_acc(KPADChannel chan, KPADMPDir *dir, Vec *acc);
static f32 revise_dir_dpd(KPADChannel chan, KPADMPDir *dir, KPADStatus *status);
static void calc_mpls_dir(kmpls_inst_st *kmplsInst);
static s32 f64data(f64 data);
static s32 get_mpls_data_x(kmpls_inst_st *kmplsInst, WPADMPStatus *wmpStatus);
static s32 get_mpls_data_y(kmpls_inst_st *kmplsInst, WPADMPStatus *wmpStatus);
static s32 get_mpls_data_z(kmpls_inst_st *kmplsInst, WPADMPStatus *wmpStatus);
static void read_mpls(KPADChannel chan, s32 offset, s32 count);
static void set_calibration_data(KPADChannel chan);

/*******************************************************************************
 * variables
 */

// .data
static s32 mpls_assign_deg[3][3] =
{
	{1600, 1600, 1600},
	{1600, 1600, 1600},
	{1600, 1600, 1600}
};

static s32 mpls_assign_ct[3][3] =
{
	{0x007fffff, 0x007fffff, 0x007fffff},
	{0x007fffff, 0x007fffff, 0x007fffff},
	{0x007fffff, 0x007fffff, 0x007fffff}
};

// .sbss
static kmpls_st *kmpls = nullptr;

/*******************************************************************************
 * functions
 */

static void reset_stability(KPADChannel chan)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	if (kmpls == nullptr)
		return;

	u32 a = (kmplsInst->at_0x17c + 0xff) % 0x100;

	kmplsInst->at_0x180[0][a] = kmplsInst->at_0x180[1][a] =
		kmplsInst->at_0x180[2][a] = 0x80000000;

	kmplsInst->at_0x0ec = 0.0f;
}

static void wpad_callback_func(KPADChannel chan, WPADResult result)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	if (kmpls == nullptr)
		return;

	if (result == WPAD_ESUCCESS)
		kmplsInst->wpadCBStatus = ControlMplsSucceeded;
	else
		kmplsInst->wpadCBStatus = ControlMplsFailed;
}

static void work_calibration(KPADChannel chan)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	if (kmpls == nullptr)
		return;

	if (kmplsInst->wpadCBStatus == ControlMplsNotRun)
	{
		reset_stability(chan);
		return;
	}

	if (kmplsInst->wpadCBStatus < ControlMplsNotRun) // ControlMplsFailed
	{
		kmplsInst->wpadCBStatus = ControlMplsNotRun;
		WPADiControlMpls(chan, WPAD_MPLS_CMD_80, &wpad_callback_func);

		return;
	}

	if (kmplsInst->at_0x0ec != 1.0f)
		return;

	if (kmplsInst->at_0x0c4 <= 0.01f)
	{
		kmplsInst->calibrating = false;
		return;
	}

	kmplsInst->wpadCBStatus = ControlMplsNotRun;
	// a calibration command?
	WPADiControlMpls(chan, WPAD_MPLS_CMD_80, &wpad_callback_func);

	kmplsInst->at_0x0c4 -= 0.01f;
	reset_stability(chan);
}

void KMPLSInit(void *ptr)
{
	static u8 initialized ATTR_UNUSED;

	OSAssert_Line(285, ptr != NULL);

	kmpls = ptr;
	KMPLSInitLimitDamp();

	kmpls->samplingCb = nullptr;

	KPADChannel chan = KPAD_MAX_CONTROLLERS - 1;
	do
	{
		kmpls->inst[chan].calibrating = false;
		kmpls->inst[chan].aimingModeEnabled = KPADIsEnableAimingMode(chan);
		kmpls->inst[chan].sensorHeight = KPADGetSensorHeight(chan);

		dpd_revise_scale(chan);
		set_unit_no(chan, 2);

		KMPLSReset(chan);
	} while (--chan >= 0);
}

u32 KMPLSGetLibWorkSize(void)
{
	return ROUND_UP(sizeof *kmpls, 32);
}

BOOL KMPLSIsInit(void)
{
	return BOOLIFY_TERNARY(kmpls != nullptr);
}

void KMPLSShutdown(void)
{
	kmpls = nullptr;
}

void KMPLSReset(KPADChannel chan)
{
	// ERRATUM: uses kmpls before assertion, uses chan before assertion
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	OSAssertMessage_Line(
		330, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(331, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmplsInst->zeroPlayEnabled = false;
	kmplsInst->zeroDriftEnabled = true;
	kmplsInst->dirReviseEnabled = false;
	kmplsInst->accReviseEnabled = true;
	kmplsInst->dpdReviseEnabled = true;

	KMPLSInitZeroPlayParam(chan);
	KMPLSInitZeroDriftParam(chan);
	KMPLSInitDirReviseParam(chan);
	KMPLSInitAccReviseParam(chan);
	KMPLSInitDpdReviseParam(chan);

	KMPLSSetAngle(chan, 0.0f, 0.0f, 0.0f);
	KMPLSSetDirection(chan, &e_dir);
	KMPLSSetDirReviseBase(chan, &e_dir);

	kmplsInst->mpStatus.mpls = kmplsInst->at_0x048 = kmplsInst->fxyz = Fxyz_0;

	kmplsInst->dampenXCount = kmplsInst->dampenYCount = kmplsInst->dampenZCount = 0;

	kmplsInst->at_0x0e8 =
	kmplsInst->at_0x0ec =
	kmplsInst->dirRevised =
	kmplsInst->accRevised =
	kmplsInst->dpdRevised = 0.0f;

	kmplsInst->dirMag = 1.0f;
	kmplsInst->degreeMag.x = 1.0f;
	kmplsInst->degreeMag.y = 1.0f;
	kmplsInst->degreeMag.z = 1.0f;

	int i = ARRAY_LENGTH(*kmplsInst->at_0x180) - 1;
	do
	{
		kmplsInst->at_0x180[0][i] =
		kmplsInst->at_0x180[1][i] =
		kmplsInst->at_0x180[2][i] = 0;
	} while (--i >= 0);

	kmplsInst->at_0x17c = 0;

	kmplsInst->at_0xd80 = kmplsInst->at_0xd88 = kmplsInst->at_0xd90 = 0.0f;

	kmplsInst->highPitchScale = 0;
	kmplsInst->calibrating = false;
}

static void set_unit_no(KPADChannel chan, s32 unitNo)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	if (kmpls == nullptr)
		return;

	if (kmplsInst->unitNo != unitNo)
		kmplsInst->highPitchScale = 0.0;

	kmplsInst->unitNo = unitNo;
	kmplsInst->at_0x054 = 1.0
	                    / (360.0 / mpls_assign_deg[kmplsInst->unitNo][0]
	                       * mpls_assign_ct[kmplsInst->unitNo][0]);
	kmplsInst->at_0x058 = 1.0
	                    / (360.0 / mpls_assign_deg[kmplsInst->unitNo][1]
	                       * mpls_assign_ct[kmplsInst->unitNo][1]);
	kmplsInst->at_0x05c = 1.0
	                    / (360.0 / mpls_assign_deg[kmplsInst->unitNo][2]
	                       * mpls_assign_ct[kmplsInst->unitNo][2]);
}

void KMPLSSetAngle(KPADChannel chan, f32 x, f32 y, f32 z)
{
	OSAssertMessage_Line(
		404, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(405, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].mpStatus.angle.x = x;
	kmpls->inst[chan].mpStatus.angle.y = y;
	kmpls->inst[chan].mpStatus.angle.z = z;
}

void KMPLSSetDirection(KPADChannel chan, KPADMPDir *dir)
{
	OSAssertMessage_Line(
		416, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(417, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].mpStatus.dir = kmpls->inst[chan].dir = *dir;
}

void KMPLSSetDirectionMag(KPADChannel chan, f32 mag)
{
	OSAssertMessage_Line(
		430, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(431, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dirMag = mag;
}

void KMPLSSetDegreeMag(KPADChannel chan, f32 x, f32 y, f32 z)
{
	OSAssertMessage_Line(
		440, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(441, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].degreeMag.x = x;
	kmpls->inst[chan].degreeMag.y = y;
	kmpls->inst[chan].degreeMag.z = z;
}

void KMPLSEnableZeroPlay(KPADChannel chan)
{
	OSAssertMessage_Line(
		455, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(456, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].zeroPlayEnabled = true;
}

void KMPLSDisableZeroPlay(KPADChannel chan)
{
	OSAssertMessage_Line(
		465, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(466, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].zeroPlayEnabled = false;
}

f32 KMPLSIsEnableZeroPlay(KPADChannel chan)
{
	OSAssertMessage_Line(
		475, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(476, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return -1.0f;

	if (kmpls->inst[chan].zeroPlayEnabled)
		return kmpls->inst[chan].at_0x0e8;
	else
		return -1.0f;
}

void KMPLSSetZeroPlayParam(KPADChannel chan, f32 param_2)
{
	OSAssertMessage_Line(
		489, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(490, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].zeroPlayParam = param_2;
}

void KMPLSGetZeroPlayParam(KPADChannel chan, f32 *param_2)
{
	OSAssertMessage_Line(
		499, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(500, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	*param_2 = kmpls->inst[chan].zeroPlayParam;
}

void KMPLSInitZeroPlayParam(KPADChannel chan)
{
	OSAssertMessage_Line(
		509, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(510, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].zeroPlayParam = 0.005f;
}

void KMPLSEnableZeroDrift(KPADChannel chan)
{
	OSAssertMessage_Line(
		523, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(524, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].zeroDriftEnabled = true;
}

void KMPLSDisableZeroDrift(KPADChannel chan)
{
	OSAssertMessage_Line(
		533, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(534, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].zeroDriftEnabled = false;
}

f32 KMPLSIsEnableZeroDrift(KPADChannel chan)
{
	OSAssertMessage_Line(
		543, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(544, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return -1.0f;

	if (kmpls->inst[chan].zeroDriftEnabled)
		return kmpls->inst[chan].at_0x0ec;
	else
		return -1.0f;
}

void KMPLSSetZeroDriftParam(KPADChannel chan, f32 param1, s32 param2,
                            f32 param3)
{
	OSAssertMessage_Line(
		557, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(558, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	// ERRATUM: uses kmpls before test
	kmpls->inst[chan].zeroDriftParam1 = param1;
	kmpls->inst[chan].zeroDriftParam2 = param2;
	kmpls->inst[chan].zeroDriftParam3 = param3;

	if (kmpls == nullptr)
		return;

	if (kmpls->inst[chan].zeroDriftParam2 < 2)
		kmpls->inst[chan].zeroDriftParam2 = 2;
	else if (kmpls->inst[chan].zeroDriftParam2 > 256)
		kmpls->inst[chan].zeroDriftParam2 = 256;
}

void KMPLSGetZeroDriftParam(KPADChannel chan, f32 *param1, s32 *param2,
                            f32 *param3)
{
	OSAssertMessage_Line(
		575, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(576, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	*param1 = kmpls->inst[chan].zeroDriftParam1;
	*param2 = kmpls->inst[chan].zeroDriftParam2;
	*param3 = kmpls->inst[chan].zeroDriftParam3;
}

void KMPLSInitZeroDriftParam(KPADChannel chan)
{
	OSAssertMessage_Line(
		587, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(588, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	KMPLSSetZeroDriftStandard(chan);
}

void KMPLSSetZeroDriftStandard(KPADChannel chan)
{
	OSAssertMessage_Line(
		597, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(598, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].zeroDriftParam1 = 0.01f;
	kmpls->inst[chan].zeroDriftParam2 = 200;
	kmpls->inst[chan].zeroDriftParam3 = 0.02f;
}

void KMPLSSetZeroDriftTight(KPADChannel chan)
{
	OSAssertMessage_Line(
		609, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(610, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].zeroDriftParam1 = 0.005f;
	kmpls->inst[chan].zeroDriftParam2 = 200;
	kmpls->inst[chan].zeroDriftParam3 = 0.01f;
}

void KMPLSSetZeroDriftLoose(KPADChannel chan)
{
	OSAssertMessage_Line(
		621, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(622, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].zeroDriftParam1 = 0.02f;
	kmpls->inst[chan].zeroDriftParam2 = 200;
	kmpls->inst[chan].zeroDriftParam3 = 0.04f;
}

void KMPLSEnableDirRevise(KPADChannel chan)
{
	OSAssertMessage_Line(
		637, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(638, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dirReviseEnabled = true;
}

void KMPLSDisableDirRevise(KPADChannel chan)
{
	OSAssertMessage_Line(
		647, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(648, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dirReviseEnabled = false;
}

f32 KMPLSIsEnableDirRevise(KPADChannel chan)
{
	OSAssertMessage_Line(
		657, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(658, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return -1.0f;

	if (kmpls->inst[chan].dirReviseEnabled)
		return kmpls->inst[chan].dirRevised;
	else
		return -1.0f;
}

void KMPLSSetDirReviseParam(KPADChannel chan, f32 param_2)
{
	OSAssertMessage_Line(
		671, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(672, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dirReviseParam = param_2;
}

void KMPLSGetDirReviseParam(KPADChannel chan, f32 *param_2)
{
	OSAssertMessage_Line(
		681, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(682, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	*param_2 = kmpls->inst[chan].dirReviseParam;
}

void KMPLSInitDirReviseParam(KPADChannel chan)
{
	OSAssertMessage_Line(
		691, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(692, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dirReviseParam = 0.5f;
}

void KMPLSSetDirReviseBase(KPADChannel chan, KPADMPDir *dir)
{
	OSAssertMessage_Line(
		701, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(702, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dirReviseBase = *dir;
}

void KMPLSEnableAccRevise(KPADChannel chan)
{
	OSAssertMessage_Line(
		715, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(716, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].accReviseEnabled = true;
}

void KMPLSDisableAccRevise(KPADChannel chan)
{
	OSAssertMessage_Line(
		725, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(726, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].accReviseEnabled = false;
}

f32 KMPLSIsEnableAccRevise(KPADChannel chan)
{
	OSAssertMessage_Line(
		735, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(736, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return -1.0f;

	if (kmpls->inst[chan].accReviseEnabled)
		return kmpls->inst[chan].accRevised;
	else
		return -1.0f;
}

void KMPLSSetAccReviseParam(KPADChannel chan, f32 param_2, f32 param_3)
{
	OSAssertMessage_Line(
		749, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(750, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].accReviseParam.x = param_2;
	kmpls->inst[chan].accReviseParam.y = param_3;
}

void KMPLSGetAccReviseParam(KPADChannel chan, f32 *param_2, f32 *param_3)
{
	OSAssertMessage_Line(
		760, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(761, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	*param_2 = kmpls->inst[chan].accReviseParam.x;
	*param_3 = kmpls->inst[chan].accReviseParam.y;
}

void KMPLSInitAccReviseParam(KPADChannel chan)
{
	OSAssertMessage_Line(
		771, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(772, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].accReviseParam.x = 0.03f;
	kmpls->inst[chan].accReviseParam.y = 0.4f;
}

void KMPLSEnableDpdRevise(KPADChannel chan)
{
	OSAssertMessage_Line(
		789, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(790, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dpdReviseEnabled = true;
}

void KMPLSDisableDpdRevise(KPADChannel chan)
{
	OSAssertMessage_Line(
		799, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(800, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dpdReviseEnabled = false;
}

f32 KMPLSIsEnableDpdRevise(KPADChannel chan)
{
	OSAssertMessage_Line(
		809, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(810, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return -1.0f;

	if (kmpls->inst[chan].dpdReviseEnabled)
		return kmpls->inst[chan].dpdRevised;
	else
		return -1.0f;
}

void KMPLSSetDpdReviseParam(KPADChannel chan, f32 param_2)
{
	OSAssertMessage_Line(
		823, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(824, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dpdReviseParam = param_2;
}

void KMPLSGetDpdReviseParam(KPADChannel chan, f32 *param_2)
{
	OSAssertMessage_Line(
		832, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(833, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	*param_2 = kmpls->inst[chan].dpdReviseParam;
}

void KMPLSInitDpdReviseParam(KPADChannel chan)
{
	OSAssertMessage_Line(
		841, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(842, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].dpdReviseParam = 0.05f;
}

static void dpd_revise_scale(KPADChannel chan)
{
	f32 zero, sensorHeight, c, d, e, f;

	// configurable macro constant?
	zero = 0.0f;

	if (kmpls == nullptr)
		return;

	if (kmpls->inst[chan].aimingModeEnabled)
		if (WPADGetSensorBarPosition() == WPAD_SENSOR_BAR_TOP)
			sensorHeight = -0.2f;
		else
			sensorHeight = 0.2f;
	else
		sensorHeight = -kmpls->inst[chan].sensorHeight;

	// more configurable macro constants?
	e = 1.0f;
	f = 0.75f;
	c = sqrtf(e * e + f * f);

	if (zero < 0.0f)
		e += zero;
	else
		e -= zero;

	if (sensorHeight < 0.0f)
		f += sensorHeight;
	else
		f -= sensorHeight;

	if (e < f)
		d = e;
	else
		d = f;

	kmpls->inst[chan].at_0x0fc = c / d;

	(void)c; // ?
}

void KMPLSSetLimitDamp(f32 limit)
{
	if (kmpls == nullptr)
		return;

	kmpls->dampLimit = limit;
}

void KMPLSGetLimitDamp(f32 *limit)
{
	if (kmpls == nullptr)
		return;

	*limit = kmpls->dampLimit;
}

void KMPLSInitLimitDamp(void)
{
	if (kmpls == nullptr)
		return;

	kmpls->dampLimit = 0.6f;
}

u32 KMPLSGetCountX(KPADChannel chan)
{
	if (kmpls == nullptr)
		return 0;

	return kmpls->inst[chan].at_0x180[0][kmpls->inst[chan].at_0x17c];
}

u32 KMPLSGetCountY(KPADChannel chan)
{
	if (kmpls == nullptr)
		return 0;

	return kmpls->inst[chan].at_0x180[1][kmpls->inst[chan].at_0x17c];
}

u32 KMPLSGetCountZ(KPADChannel chan)
{
	if (kmpls == nullptr)
		return 0;

	return kmpls->inst[chan].at_0x180[2][kmpls->inst[chan].at_0x17c];
}

f32 KMPLSGetZeroCountX(KPADChannel chan)
{
	if (kmpls == nullptr)
		return 0;

	return kmpls->inst[chan].fxyz.x;
}

f32 KMPLSGetZeroCountY(KPADChannel chan)
{
	if (kmpls == nullptr)
		return 0;

	return kmpls->inst[chan].fxyz.y;
}

f32 KMPLSGetZeroCountZ(KPADChannel chan)
{
	if (kmpls == nullptr)
		return 0;

	return kmpls->inst[chan].fxyz.z;
}

static f32 move_mpls_orient(kmpls_inst_st *kmplsInst, Vec *param_2,
                            Vec *param_3, Vec *param_4)
{
	Vec vecA;
	vecA.x = param_2->x - param_3->x;
	vecA.y = param_2->y - param_3->y;
	vecA.z = param_2->z - param_3->z;
	f32 a = sqrtf(vecA.x * vecA.x + vecA.y * vecA.y + vecA.z * vecA.z);

	Vec vecB;
	vecB.x = param_2->x - param_4->x;
	vecB.y = param_2->y - param_4->y;
	vecB.z = param_2->z - param_4->z;

	f32 b = vecB.x * vecA.x + vecB.y * vecA.y + vecB.z * vecA.z;
	if (b >= 0.0f)
		return 0.0f;

	b = param_2->x * vecB.x + param_2->y * vecB.y + param_2->z * vecB.z;
	vecB.x -= b * param_2->x;
	vecB.y -= b * param_2->y;
	vecB.z -= b * param_2->z;

	b = sqrtf(vecB.x * vecB.x + vecB.y * vecB.y + vecB.z * vecB.z);
	if (b < 0.001f)
		return 0.0f;

	b = a * kmplsInst->dirReviseParam / b;
	param_2->x -= b * vecB.x;
	param_2->y -= b * vecB.y;
	param_2->z -= b * vecB.z;

	return b;
}

static f32 revise_dir_dir(KPADChannel chan)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	if (kmpls == nullptr)
		return 0.0f;

	f32 a, b;
	b = move_mpls_orient(kmplsInst, &kmplsInst->mpStatus.dir.X,
	                         &kmplsInst->dir.X, &kmplsInst->dirReviseBase.X);
	a = move_mpls_orient(kmplsInst, &kmplsInst->mpStatus.dir.Y,
	                         &kmplsInst->dir.Y, &kmplsInst->dirReviseBase.Y);

	if (a > b)
		b = a;

	a = move_mpls_orient(kmplsInst, &kmplsInst->mpStatus.dir.Z,
	                         &kmplsInst->dir.Z, &kmplsInst->dirReviseBase.Z);

	if (a > b)
		b = a;

	__KMPLS_normalize_directionXYZ(&kmplsInst->mpStatus.dir, 2.999f);

	return b;
}

static f32 revise_dir_acc(KPADChannel chan, KPADMPDir *dir, Vec *acc)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	if (kmpls == nullptr)
		return 0.0f;

	f32 a = acc->x * acc->x + acc->y * acc->y + acc->z * acc->z;
	if (a == 0.0f)
		return 0.0f;

	a = sqrtf(a);

	f32 b;
	if (a < 1.0f)
	{
		if (a <= 1.0f - kmplsInst->accReviseParam.y)
			return 0.0f;

		b = 1.0f / kmplsInst->accReviseParam.y
		      * (a - (1.0f - kmplsInst->accReviseParam.y));
	}
	else
	{
		if (a >= 1.0f + kmplsInst->accReviseParam.y)
			return 0.0f;

		b = -1.0f / kmplsInst->accReviseParam.y
		      * (a - (1.0f + kmplsInst->accReviseParam.y));
	}

	b *= b;
	b *= kmplsInst->accReviseParam.x;
	a = 1.0f / a;

	Vec vecA, vecB;
	Vec vecC;
	vecC.x = a * acc->x;
	vecC.y = a * acc->y;
	vecC.z = a * acc->z;

	// clang-format off
	vecA.x = vecC.x * dir->X.x + vecC.y * dir->Y.x + vecC.z * dir->Z.x;
	vecA.y = vecC.x * dir->X.y + vecC.y * dir->Y.y + vecC.z * dir->Z.y;
	vecA.z = vecC.x * dir->X.z + vecC.y * dir->Y.z + vecC.z * dir->Z.z;
	// clang-format on

	vecB.x = vecA.x + -vecA.x * b;
	vecB.y = vecA.y + (-1.0f - vecA.y) * b;
	vecB.z = vecA.z + -vecA.z * b;

	if (__KMPLS_normalize_Fxyz(&vecB) == 0.0f)
		return 0.0f;

	KPADMPDir mpDirA, mpDirB;
	__KMPLS_make_vec_dir(&mpDirA, &vecA, &vecB);
	__KMPLS_mult_dir(&mpDirA, dir, &mpDirB);

	*dir = mpDirB;
	__KMPLS_normalize_directionXYZ(dir, 2.999f);

	vecA.x -= vecB.x;
	vecA.y -= vecB.y;
	vecA.z -= vecB.z;

	a = sqrtf(vecA.x * vecA.x + vecA.y * vecA.y + vecA.z * vecA.z);
	return a;
}

static f32 revise_dir_dpd(KPADChannel chan, KPADMPDir *dir, KPADStatus *status)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	f32 a = 0.383864f;

	if (kmpls == nullptr)
		return 0.0f;

	if (kmpls->inst[chan].aimingModeEnabled != KPADIsEnableAimingMode(chan)
	    || kmpls->inst[chan].sensorHeight != KPADGetSensorHeight(chan))
	{
		kmpls->inst[chan].aimingModeEnabled = KPADIsEnableAimingMode(chan);
		kmpls->inst[chan].sensorHeight = KPADGetSensorHeight(chan);

		dpd_revise_scale(chan);
	}

	a /= kmplsInst->at_0x0fc;

	f32 b;
	KPADMPDir dirX, dirY, dirZ;
	Vec vecA, vecB;
	if (status->dpd_valid_fg == 2)
	{
		dirX.X.x = status->horizon.x;
		dirX.X.y = status->horizon.y;
		dirX.X.z = 0.0f;

		dirX.Y.x = -status->horizon.y;
		dirX.Y.y = status->horizon.x;
		dirX.Y.z = 0.0f;

		vecA = vecB = dirX.Z = e_dir.Z;
		vecB.x = -a * status->pos.x;

		__KMPLS_normalize_Fxyz(&vecB);
		__KMPLS_make_vec_dir(&dirZ, &vecA, &vecB);
		__KMPLS_mult_dir(&dirZ, &dirX, &dirY);

		vecA = vecB = dirY.Z;

		b = sqrtf(dir->Z.x * dir->Z.x + dir->Z.z * dir->Z.z);

		vecB.x *= b;
		vecB.z *= b;
		vecB.y = dir->Z.y;

		__KMPLS_make_vec_dir(&dirZ, &vecA, &vecB);
		__KMPLS_mult_dir(&dirZ, &dirY, &dirX);

		dirY = *dir;

		__KMPLS_linear_Direction(dir, &dirX,
		                         kmplsInst->dpdReviseParam, dir);

		dirY.X.x -= dir->X.x;
		dirY.X.y -= dir->X.y;
		dirY.X.z -= dir->X.z;

		dirY.X.x =
			dirY.X.x * dirY.X.x + dirY.X.y * dirY.X.y + dirY.X.z * dirY.X.z;

		dirY.Y.x -= dir->Y.x;
		dirY.Y.y -= dir->Y.y;
		dirY.Y.z -= dir->Y.z;

		dirY.Y.x =
			dirY.Y.x * dirY.Y.x + dirY.Y.y * dirY.Y.y + dirY.Y.z * dirY.Y.z;

		dirY.Z.x -= dir->Z.x;
		dirY.Z.y -= dir->Z.y;
		dirY.Z.z -= dir->Z.z;

		dirY.Z.x =
			dirY.Z.x * dirY.Z.x + dirY.Z.y * dirY.Z.y + dirY.Z.z * dirY.Z.z;

		if (dirY.X.x > dirY.Y.x)
		{
			if (dirY.X.x > dirY.Z.x)
				return sqrtf(dirY.X.x);
			else
				return sqrtf(dirY.Z.x);
		}
		else
		{
			if (dirY.Y.x > dirY.Z.x)
				return sqrtf(dirY.Y.x);
			else
				return sqrtf(dirY.Z.x);
		}
	}

	return 0.0f;
}

static void calc_mpls_dir(kmpls_inst_st *kmplsInst)
{
	KPADMPDir dir = kmplsInst->mpStatus.dir;
	f32 a, b;

	b = kmplsInst->dirMag * kmplsInst->mpStatus.mpls.x;
	a = kmplsInst->at_0x0b0 * b;
	a = 1.0f + a * a * 0.0001f;
	a *= kmplsInst->at_0x0b4 * b;

	kmplsInst->mpStatus.dir.Y.x += a * dir.Z.x;
	kmplsInst->mpStatus.dir.Y.y += a * dir.Z.y;
	kmplsInst->mpStatus.dir.Y.z += a * dir.Z.z;

	kmplsInst->mpStatus.dir.Z.x -= a * dir.Y.x;
	kmplsInst->mpStatus.dir.Z.y -= a * dir.Y.y;
	kmplsInst->mpStatus.dir.Z.z -= a * dir.Y.z;

	b = kmplsInst->dirMag * kmplsInst->mpStatus.mpls.y;
	a = kmplsInst->at_0x0b0 * b;
	a = 1.0f + a * a * 0.0001f;
	a *= kmplsInst->at_0x0b4 * b;

	kmplsInst->mpStatus.dir.Z.x += a * dir.X.x;
	kmplsInst->mpStatus.dir.Z.y += a * dir.X.y;
	kmplsInst->mpStatus.dir.Z.z += a * dir.X.z;

	kmplsInst->mpStatus.dir.X.x -= a * dir.Z.x;
	kmplsInst->mpStatus.dir.X.y -= a * dir.Z.y;
	kmplsInst->mpStatus.dir.X.z -= a * dir.Z.z;

	b = kmplsInst->dirMag * kmplsInst->mpStatus.mpls.z;
	a = kmplsInst->at_0x0b0 * b;
	a = 1.0f + a * a * 0.0001f;
	a *= kmplsInst->at_0x0b4 * b;

	kmplsInst->mpStatus.dir.X.x += a * dir.Y.x;
	kmplsInst->mpStatus.dir.X.y += a * dir.Y.y;
	kmplsInst->mpStatus.dir.X.z += a * dir.Y.z;

	kmplsInst->mpStatus.dir.Y.x -= a * dir.X.x;
	kmplsInst->mpStatus.dir.Y.y -= a * dir.X.y;
	kmplsInst->mpStatus.dir.Y.z -= a * dir.X.z;

	__KMPLS_normalize_directionXYZ(&kmplsInst->mpStatus.dir, 2.999f);
}

static s32 f64data(f64 data)
{
	if (data < 0.0)
		return data - 0.5;
	else
		return data + 0.5;
}

static s32 get_mpls_data_x(kmpls_inst_st *kmplsInst, WPADMPStatus *wmpStatus)
{
	f64 a, b, c, d;

	if (wmpStatus->stat & WPAD_MPLS_STATUS_PITCH_SLOW_MODE)
	{
		c = kmplsInst->mplsCalibLow.pitchZero;
		d = kmplsInst->lowPitchScale;
	}
	else
	{
		c = kmplsInst->mplsCalibHigh.pitchZero;
		d = kmplsInst->highPitchScale;
	}

	b = d * (wmpStatus->pitch - c);
	b -= kmplsInst->at_0xd80;
	a = 50.0 * b;

	if (a < 0.0)
		a = -a;

	if (a < 0.1)
		a = 0.1;
	else if (a > 1.0)
		a = 1.0;

	kmplsInst->at_0xd80 += b * a;

	return f64data(kmplsInst->at_0xd80 * kmplsInst->degreeMag.x);
}

static s32 get_mpls_data_y(kmpls_inst_st *kmplsInst, WPADMPStatus *wmpStatus)
{
	f64 a, b, c, d;

	if (wmpStatus->stat& WPAD_MPLS_STATUS_YAW_SLOW_MODE)
	{
		c = kmplsInst->mplsCalibLow.yawZero;
		d = kmplsInst->lowYawScale;
	}
	else
	{
		c = kmplsInst->mplsCalibHigh.yawZero;
		d = kmplsInst->highYawScale;
	}

	b = d * (wmpStatus->yaw - c);
	b -= kmplsInst->at_0xd88;
	a = 50.0 * b;

	if (a < 0.0)
		a = -a;

	if (a < 0.1)
		a = 0.1;
	else if (a > 1.0)
		a = 1.0;

	kmplsInst->at_0xd88 += b * a;

	return f64data(kmplsInst->at_0xd88 * kmplsInst->degreeMag.y);
}

static s32 get_mpls_data_z(kmpls_inst_st *kmplsInst, WPADMPStatus *wmpStatus)
{
	f64 a, b, c, d;

	if (wmpStatus->stat & WPAD_MPLS_STATUS_ROLL_SLOW_MODE)
	{
		c = kmplsInst->mplsCalibLow.rollZero;
		d = kmplsInst->lowRollScale;
	}
	else
	{
		c = kmplsInst->mplsCalibHigh.rollZero;
		d = kmplsInst->highRollScale;
	}

	b = d * (wmpStatus->roll - c);
	b -= kmplsInst->at_0xd90;
	a = 50.0 * b;

	if (a < 0.0)
		a = -a;

	if (a < 0.1)
		a = 0.1;
	else if (a > 1.0)
		a = 1.0;

	kmplsInst->at_0xd90 += b * a;

	return f64data(kmplsInst->at_0xd90 * kmplsInst->degreeMag.z);
}

static void read_mpls(KPADChannel chan, s32 offset, s32 count)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	if (count <= 0)
		return;

	if (kmpls == nullptr)
		return;

	KPADMPStatus mpStatus;
	Vec vec;
	KPADUnifiedWpadStatus *uwStatus;

	// I definitely know what all of these are
	s32 stack_0x64;
	s32 stack_0x60;
	s32 stack_0x5c;
	s32 stack_0x58;
	s32 stack_0x54;
	s32 stack_0x50;
	s32 stack_0x4c;
	s32 stack_0x48;
	s32 stack_0x44;
	s32 stack_0x40;
	s32 stack_0x3c;
	s32 stack_0x38;
	s32 stack_0x34;
	s32 stack_0x30;
	s32 stack_0x2c;
	s32 stack_0x28;
	s32 stack_0x24;
	s32 stack_0x20;

	// zeroDrift
	f32 zdP1;
	s32 zdP2;
	f32 zdP3;

	s32 index;

	u8 zeroDriftEnabled;

	f32 a;

	if (kmplsInst->calibrating)
	{
		zeroDriftEnabled = true;

		zdP1 = kmplsInst->at_0x0c4;
		zdP2 = 1500.0f * kmplsInst->at_0x0c4;
		zdP3 = 0.9f;
	}
	else
	{
		zeroDriftEnabled = kmpls->inst[chan].zeroDriftEnabled;

		zdP1 = kmpls->inst[chan].zeroDriftParam1;
		zdP2 = kmpls->inst[chan].zeroDriftParam2;
		zdP3 = kmpls->inst[chan].zeroDriftParam3;
	}

	stack_0x40 = -(stack_0x3c = mpls_assign_ct[kmplsInst->unitNo][0]);
	stack_0x38 = -(stack_0x34 = mpls_assign_ct[kmplsInst->unitNo][1]);
	stack_0x30 = -(stack_0x2c = mpls_assign_ct[kmplsInst->unitNo][2]);

	stack_0x28 = __KMPLS_f2i(zdP1 / kmplsInst->at_0x054);
	stack_0x24 = __KMPLS_f2i(zdP1 / kmplsInst->at_0x058);
	stack_0x20 = __KMPLS_f2i(zdP1 / kmplsInst->at_0x05c);

	if (stack_0x28 == 0)
		++stack_0x28;

	if (stack_0x24 == 0)
		++stack_0x24;

	if (stack_0x20 == 0)
		++stack_0x20;

	kmplsInst->dampenXCount = kmplsInst->dampenYCount = kmplsInst->dampenZCount = 0;

	kmplsInst->at_0x0e8 = 1.0f;
	kmplsInst->at_0x0ec = 1.0f;

	do
	{
		index = offset - count - 1;

		if (index < 0)
			index += kmplsInst->at_0xda0 + 16;

		if (index >= 16)
			uwStatus = kmplsInst->at_0xd9c + (index - 16);
		else
			uwStatus = kmplsInst->at_0xd98 + index;

		vec = kmplsInst->at_0x03c;
		kmplsInst->at_0x03c = kmplsInst->mpStatus.mpls;

		if (uwStatus->u.core.err != WPAD_ESUCCESS)
		{
			kmplsInst->mpStatus.mpls.x += kmplsInst->at_0x048.x *=
				kmpls->dampLimit;
			kmplsInst->mpStatus.mpls.y += kmplsInst->at_0x048.y *=
				kmpls->dampLimit;
			kmplsInst->mpStatus.mpls.z += kmplsInst->at_0x048.z *=
				kmpls->dampLimit;

			kmplsInst->at_0x0e8 = 0.0f;
			kmplsInst->at_0x0ec = 0.0f;
		}
		else
		{
			// % ARRAY_LENGTH(*kmplsInst->at_0x180)
			kmplsInst->at_0x17c = kmplsInst->at_0x17c + 1 & 0xff;

			// x

			if (kmplsInst->unitNo < 2)
				stack_0x44 = get_mpls_data_x(kmplsInst, &uwStatus->u.mp);
			else
				stack_0x44 = uwStatus->u.mp.ext.fs.fsAccZ;

			kmplsInst->at_0x180[0][kmplsInst->at_0x17c] = stack_0x44;
			stack_0x5c = 1;
			stack_0x64 = stack_0x44 - __KMPLS_f2i(kmplsInst->fxyz.x);

			if (stack_0x64 <= stack_0x40 || stack_0x64 >= stack_0x3c)
			{
				kmplsInst->mpStatus.mpls.x += kmplsInst->at_0x048.x *=
					kmpls->dampLimit;

				++kmplsInst->dampenXCount;
				kmplsInst->at_0x0e8 = 0.0f;
			}
			else
			{
				kmplsInst->mpStatus.mpls.x = stack_0x44;
				stack_0x48 = stack_0x44;
				stack_0x64 = stack_0x44 - stack_0x28;
				stack_0x60 = stack_0x44 + stack_0x28;

				// % ARRAY_LENGTH(*kmplsInst->at_0x180)
				stack_0x50 = (kmplsInst->at_0x17c - 1) & 0xff;
				stack_0x4c = (kmplsInst->at_0x17c - zdP2) & 0xff;

				do
				{
					if (kmplsInst->at_0x180[0][stack_0x50] < stack_0x64)
						break;

					if (kmplsInst->at_0x180[0][stack_0x50] > stack_0x60)
						break;

					stack_0x48 += kmplsInst->at_0x180[0][stack_0x50];
					++stack_0x5c;
					stack_0x50 = stack_0x50 - 1 & 0xff;
				} while (stack_0x50 != stack_0x4c);

				a = (f32)(stack_0x5c - 1) / (f32)(zdP2 - 1);

				a *= a;
				a *= a;
				a *= a;
				a *= a;
				a *= a;

				kmplsInst->mpStatus.mpls.x += a
				                            * ((f32)stack_0x48 / (f32)stack_0x5c
				                               - kmplsInst->mpStatus.mpls.x);

				if (zeroDriftEnabled)
				{
					a *= zdP3;
					kmplsInst->fxyz.x +=
						a * (kmplsInst->mpStatus.mpls.x - kmplsInst->fxyz.x);
				}

				kmplsInst->mpStatus.mpls.x =
					kmplsInst->at_0x054
					* (kmplsInst->mpStatus.mpls.x - kmplsInst->fxyz.x);

				if (kmplsInst->zeroPlayEnabled)
				{
					if (kmplsInst->mpStatus.mpls.x >= -kmplsInst->zeroPlayParam
					    && kmplsInst->mpStatus.mpls.x
					           <= kmplsInst->zeroPlayParam)
					{
						if (kmplsInst->mpStatus.mpls.x < 0.0)
							a = -kmplsInst->mpStatus.mpls.x;
						else
							a = kmplsInst->mpStatus.mpls.x;

						a = 1.0f - a / kmplsInst->zeroPlayParam;

						if (a < kmplsInst->at_0x0e8)
							kmplsInst->at_0x0e8 = a;

						kmplsInst->mpStatus.mpls.x = 0.0f;
					}
					else
					{
						kmplsInst->at_0x0e8 = 0.0f;
					}
				}

				if (kmplsInst->unitNo == 1)
				{
					kmplsInst->at_0x048.x =
						(kmplsInst->mpStatus.mpls.x - vec.x) * 0.5f;
				}
				else
				{
					kmplsInst->at_0x048.x =
						kmplsInst->mpStatus.mpls.x - kmplsInst->at_0x03c.x;
				}
			}

			// y

			if (kmplsInst->unitNo < 2)
				stack_0x44 = get_mpls_data_y(kmplsInst, &uwStatus->u.mp);
			else
				stack_0x44 = -uwStatus->u.mp.ext.fs.fsAccX;

			kmplsInst->at_0x180[1][kmplsInst->at_0x17c] = stack_0x44;
			stack_0x58 = 1;
			stack_0x64 = stack_0x44 - __KMPLS_f2i(kmplsInst->fxyz.y);

			if (stack_0x64 <= stack_0x38 || stack_0x64 >= stack_0x34)
			{
				kmplsInst->mpStatus.mpls.y += kmplsInst->at_0x048.y *=
					kmpls->dampLimit;
				++kmplsInst->dampenYCount;
				kmplsInst->at_0x0e8 = 0.0f;
			}
			else
			{
				kmplsInst->mpStatus.mpls.y = stack_0x44;
				stack_0x48 = stack_0x44;
				stack_0x64 = stack_0x44 - stack_0x24;
				stack_0x60 = stack_0x44 + stack_0x24;
				stack_0x50 = (kmplsInst->at_0x17c - 1) & 0xff;
				stack_0x4c = (kmplsInst->at_0x17c - zdP2) & 0xff;

				do
				{
					if (kmplsInst->at_0x180[1][stack_0x50] < stack_0x64)
						break;

					if (kmplsInst->at_0x180[1][stack_0x50] > stack_0x60)
						break;

					stack_0x48 += kmplsInst->at_0x180[1][stack_0x50];
					++stack_0x58;
					stack_0x50 = (u8)(stack_0x50 - 1);
				} while (stack_0x50 != stack_0x4c);

				a = (f32)(stack_0x58 - 1) / (f32)(zdP2 - 1);

				a *= a;
				a *= a;
				a *= a;
				a *= a;
				a *= a;

				kmplsInst->mpStatus.mpls.y += a
				                            * ((f32)stack_0x48 / (f32)stack_0x58
				                               - kmplsInst->mpStatus.mpls.y);

				if (zeroDriftEnabled)
				{
					a *= zdP3;
					kmplsInst->fxyz.y +=
						a
						* (kmplsInst->mpStatus.mpls.y - kmplsInst->fxyz.y);
				}

				kmplsInst->mpStatus.mpls.y =
					kmplsInst->at_0x058
					* (kmplsInst->mpStatus.mpls.y - kmplsInst->fxyz.y);

				if (kmplsInst->zeroPlayEnabled)
				{
					if (kmplsInst->mpStatus.mpls.y >= -kmplsInst->zeroPlayParam
					    && kmplsInst->mpStatus.mpls.y
					           <= kmplsInst->zeroPlayParam)
					{
						if (kmplsInst->mpStatus.mpls.y < 0.0)
							a = -kmplsInst->mpStatus.mpls.y;
						else
							a = kmplsInst->mpStatus.mpls.y;

						a = 1.0f - a / kmplsInst->zeroPlayParam;

						if (a < kmplsInst->at_0x0e8)
							kmplsInst->at_0x0e8 = a;

						kmplsInst->mpStatus.mpls.y = 0.0f;
					}
					else
					{
						kmplsInst->at_0x0e8 = 0.0f;
					}
				}

				if (kmplsInst->unitNo == 1)
				{
					kmplsInst->at_0x048.y =
						(kmplsInst->mpStatus.mpls.y - vec.y) * 0.5f;
				}
				else
				{
					kmplsInst->at_0x048.y =
						kmplsInst->mpStatus.mpls.y - kmplsInst->at_0x03c.y;
				}
			}

			// z

			if (kmplsInst->unitNo < 2)
				stack_0x44 = get_mpls_data_z(kmplsInst, &uwStatus->u.mp);
			else
				stack_0x44 = uwStatus->u.mp.ext.fs.fsAccY;

			kmplsInst->at_0x180[2][kmplsInst->at_0x17c] = stack_0x44;
			stack_0x54 = 1;
			stack_0x64 = stack_0x44 - __KMPLS_f2i(kmplsInst->fxyz.z);

			if (stack_0x64 <= stack_0x30 || stack_0x64 >= stack_0x2c)
			{
				kmplsInst->mpStatus.mpls.z += kmplsInst->at_0x048.z *=
					kmpls->dampLimit;
				++kmplsInst->dampenZCount;
				kmplsInst->at_0x0e8 = 0.0f;
			}
			else
			{
				kmplsInst->mpStatus.mpls.z = stack_0x44;
				stack_0x48 = stack_0x44;
				stack_0x64 = stack_0x44 - stack_0x20;
				stack_0x60 = stack_0x44 + stack_0x20;
				stack_0x50 = (kmplsInst->at_0x17c - 1) & 0xff;
				stack_0x4c = (kmplsInst->at_0x17c - zdP2) & 0xff;

				do
				{
					if (kmplsInst->at_0x180[2][stack_0x50] < stack_0x64)
						break;

					if (kmplsInst->at_0x180[2][stack_0x50] > stack_0x60)
						break;

					stack_0x48 += kmplsInst->at_0x180[2][stack_0x50];
					++stack_0x54;
					stack_0x50 = (u8)(stack_0x50 - 1);
				} while (stack_0x50 != stack_0x4c);

				a = (f32)(stack_0x54 - 1) / (f32)(zdP2 - 1);

				a *= a;
				a *= a;
				a *= a;
				a *= a;
				a *= a;

				kmplsInst->mpStatus.mpls.z += a
				                            * ((f32)stack_0x48 / (f32)stack_0x54
				                               - kmplsInst->mpStatus.mpls.z);

				if (zeroDriftEnabled)
				{
					a *= zdP3;
					kmplsInst->fxyz.z +=
						a
						* (kmplsInst->mpStatus.mpls.z - kmplsInst->fxyz.z);
				}

				kmplsInst->mpStatus.mpls.z =
					kmplsInst->at_0x05c
					* (kmplsInst->mpStatus.mpls.z - kmplsInst->fxyz.z);

				if (kmplsInst->zeroPlayEnabled)
				{
					if (kmplsInst->mpStatus.mpls.z >= -kmplsInst->zeroPlayParam
					    && kmplsInst->mpStatus.mpls.z
					           <= kmplsInst->zeroPlayParam)
					{
						if (kmplsInst->mpStatus.mpls.z < 0.0)
							a = -kmplsInst->mpStatus.mpls.z;
						else
							a = kmplsInst->mpStatus.mpls.z;

						a = 1.0f - a / kmplsInst->zeroPlayParam;

						if (a < kmplsInst->at_0x0e8)
							kmplsInst->at_0x0e8 = a;

						kmplsInst->mpStatus.mpls.z = 0.0f;
					}
					else
					{
						kmplsInst->at_0x0e8 = 0.0f;
					}
				}

				if (kmplsInst->unitNo == 1)
				{
					kmplsInst->at_0x048.z =
						(kmplsInst->mpStatus.mpls.z - vec.z) * 0.5f;
				}
				else
				{
					kmplsInst->at_0x048.z =
						kmplsInst->mpStatus.mpls.z - kmplsInst->at_0x03c.z;
				}
			}

			if (stack_0x5c < stack_0x58)
			{
				if (stack_0x5c < stack_0x54)
					stack_0x54 = stack_0x5c;
			}
			else
			{
				if (stack_0x58 < stack_0x54)
					stack_0x54 = stack_0x58;
			}

			a = (f32)(stack_0x54 - 1) / (f32)(zdP2 - 1);

			if (a < kmplsInst->at_0x0ec)
				kmplsInst->at_0x0ec = a;
		}

		kmplsInst->mpStatus.angle.x +=
			kmplsInst->at_0x0ac * kmplsInst->mpStatus.mpls.x;
		kmplsInst->mpStatus.angle.y +=
			kmplsInst->at_0x0ac * kmplsInst->mpStatus.mpls.y;
		kmplsInst->mpStatus.angle.z +=
			kmplsInst->at_0x0ac * kmplsInst->mpStatus.mpls.z;

		calc_mpls_dir(kmplsInst);

		if (kmpls->samplingCb != nullptr)
		{
			// not full copy?
			mpStatus.mpls = kmplsInst->mpStatus.mpls;
			mpStatus.angle = kmplsInst->mpStatus.angle;
			mpStatus.dir = kmplsInst->mpStatus.dir;

			(*kmpls->samplingCb)(chan, &mpStatus);
		}
	} while (--count);

	kmpls->samplingCb = nullptr;
}

static void set_calibration_data(KPADChannel chan)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	if (kmpls == nullptr)
		return;

	if (kmplsInst->unitNo >= 2)
	{
		kmplsInst->highPitchScale = 0.0;
		return;
	}

	WPADiGetMplsCalibration(chan, &kmplsInst->mplsCalibHigh,
	                        &kmplsInst->mplsCalibLow);

	if (kmplsInst->mplsCalibHigh.pitchScale == 0.0f
	    || kmplsInst->mplsCalibHigh.yawScale == 0.0f
	    || kmplsInst->mplsCalibHigh.rollScale == 0.0f
	    || kmplsInst->mplsCalibLow.pitchScale == 0.0f
	    || kmplsInst->mplsCalibLow.yawScale == 0.0f
	    || kmplsInst->mplsCalibLow.rollScale == 0.0f)
	{
		kmplsInst->highPitchScale = 0.0;
		return;
	}

	f64 degrees = kmplsInst->mplsCalibHigh.degrees * CALIBRATION_CONSTANT;
	kmplsInst->highPitchScale	= degrees / kmplsInst->mplsCalibHigh.pitchScale;
	kmplsInst->highYawScale		= degrees / kmplsInst->mplsCalibHigh.yawScale;
	kmplsInst->highRollScale	= degrees / kmplsInst->mplsCalibHigh.rollScale;

	degrees = kmplsInst->mplsCalibLow.degrees * CALIBRATION_CONSTANT;
	kmplsInst->lowPitchScale	= degrees / kmplsInst->mplsCalibLow.pitchScale;
	kmplsInst->lowYawScale		= degrees / kmplsInst->mplsCalibLow.yawScale;
	kmplsInst->lowRollScale		= degrees / kmplsInst->mplsCalibLow.rollScale;

	kmplsInst->wpadCBStatus = ControlMplsSucceeded; // ?

	KMPLSStartCalibration(chan);
}

void KMPLSSetKpadRingBuffer(KPADChannel chan, KPADUnifiedWpadStatus *param_2,
                            KPADUnifiedWpadStatus *param_3, u32 param_4)
{
	// ERRATUM: uses kmpls before test
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	if (kmpls == nullptr)
		return;

	kmplsInst->at_0xd98 = param_2;
	kmplsInst->at_0xd9c = param_3;
	kmplsInst->at_0xda0 = param_4;
}

void KMPLSRead(KPADChannel chan, KPADMPStatus *mpStatus, s32 offset,
               s32 count, void *kpad_p, f64 param_6)
{
	// ERRATUM: uses kmpls before test, uses chan before assertion
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	// ERRATUM: uses kpad_p before assertion
	KPADStatus *kp = kpad_p;

	WPADDeviceType devType;
	WPADMplsCommand mplsCmd;

	OSAssert_Line(1684, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));
	OSAssert_Line(1685, kpad_p != NULL);

	if (kmpls == nullptr)
		return;

	s32 unitNo = 2;

	if (WPADProbe(chan, &devType) != WPAD_ENODEV
	    && (devType == WPAD_DEV_MOTION_PLUS
	        || devType == WPAD_DEV_MPLS_PT_FS
	        || devType == WPAD_DEV_MPLS_PT_CLASSIC
	        || devType == WPAD_DEV_MPLS_PT_UNKNOWN))
	{
		mplsCmd = WPADiGetMplsStatus(chan);

		if (mplsCmd == WPAD_DEV_MODE_MPLS_PT_MAIN)
		{
			unitNo = 0;
		}
		else if (mplsCmd != WPAD_DEV_MODE_NORMAL)
		{
			if (devType == WPAD_DEV_MOTION_PLUS)
				unitNo = 0;
			else
				unitNo = 1;

			kmplsInst->unitNo = unitNo;
		}
	}

	set_unit_no(chan, unitNo);

	// unitNo always != 2
	if (count > 0 && kpad_p != nullptr && unitNo != 2)
	{
		kmplsInst->at_0x0ac = param_6;
		kmplsInst->at_0x0b0 = 360.0 * param_6;
		kmplsInst->at_0x0b4 = M_TAU * param_6;

		if (kmplsInst->highPitchScale == 0.0)
			set_calibration_data(chan);

		kmplsInst->dir = kmplsInst->mpStatus.dir;

		read_mpls(chan, offset, count);

		if (kmplsInst->calibrating)
			work_calibration(chan);

		if (kmplsInst->dirReviseEnabled)
			kmplsInst->dirRevised = revise_dir_dir(chan);
		else
			kmplsInst->dirRevised = 0.0f;

		if (kmplsInst->accReviseEnabled)
		{
			kmplsInst->accRevised =
				revise_dir_acc(chan, &kmplsInst->mpStatus.dir, &kp->acc);
		}
		else
		{
			kmplsInst->accRevised = 0.0f;
		}

		if (kmplsInst->dpdReviseEnabled)
		{
			kmplsInst->dpdRevised =
				revise_dir_dpd(chan, &kmplsInst->mpStatus.dir, kp);
		}
		else
		{
			kmplsInst->dpdRevised = 0.0f;
		}
	}

	// not full copy?
	mpStatus->mpls	= kmplsInst->mpStatus.mpls;
	mpStatus->angle	= kmplsInst->mpStatus.angle;
	mpStatus->dir	= kmplsInst->mpStatus.dir;
}

void KMPLSSetSamplingCallback(KMPLSSamplingCallback *samplingCb)
{
	if (kmpls == nullptr)
		return;

	kmpls->samplingCb = samplingCb;
}

void KMPLSStartCalibration(KPADChannel chan)
{
	// ERRATUM: uses kmpls before assertion and test, uses chan before assertion
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	OSAssertMessage_Line(1795, kmpls != nullptr,
	                     "KMPLS workarea is not registered. "
	                     "Please call KMPLSInit first!");
	OSAssert_Line(1796, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmplsInst->calibrating = true;
	kmplsInst->at_0x0c4 = 0.03f;
	reset_stability(chan);
}

f32 KMPLSWorkCalibration(KPADChannel chan)
{
	// ERRATUM: uses kmpls before assertion, uses chan before assertion
	kmpls_inst_st *kmplsInst = &kmpls->inst[chan];

	OSAssertMessage_Line(
		1810, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(1811, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return -1.0f;

	f32 a;
	if (kmplsInst->calibrating)
	{
		a = (kmplsInst->at_0x0c4 - 0.01f) / 0.02f;

		if (a < 0.0f)
			a = 0.0f;
		else if (a > 1.0f)
			a = 1.0f;
	}
	else
	{
		a = -1.0f;
	}

	return a;
}

void KMPLSStopCalibration(KPADChannel chan)
{
	OSAssertMessage_Line(
		1826, kmpls != nullptr,
		"KMPLS workarea is not registered. Please call KMPLSInit first!");
	OSAssert_Line(1827, (chan >= 0) && (chan < WPAD_MAX_CONTROLLERS));

	if (kmpls == nullptr)
		return;

	kmpls->inst[chan].calibrating = false;
}
