#include "KZMplsTestSub.h"

/*******************************************************************************
 * headers
 */

#include <math.h> // sqrtf

#include <revolution/types.h> // f32

#include <revolution/kpad.h> // KPADMPDir

#if 0
#include <revolution/mtx/vec.h> // Vec
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * variables
 */

// .data
KPADMPDir e_dir =
{
	{1.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 1.0f}
};

// .bss
Vec Fxyz_0;

/*******************************************************************************
 * functions
 */

int __KMPLS_f2i(f32 x)
{
	if (x < 0.0f)
		return (int)(x - 0.5f);
	else
		return (int)(x + 0.5f);
}

f32 __KMPLS_normalize_Fxyz(Vec *vec)
{
	f32 hypot, invhypot;

	hypot = sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	if (hypot != 0.0f)
	{
		invhypot = 1.0f / hypot;
		vec->x *= invhypot;
		vec->y *= invhypot;
		vec->z *= invhypot;
	}

	return hypot;
}

void __KMPLS_normalize_directionXYZ(KPADMPDir *dir, f32 maxSqrtTotal)
{
	KPADMPDir tmpDir;
	f32 sqrtTotal, invhypot;

	do
	{
		invhypot = 1.0f
		         / sqrtf(dir->X.x * dir->X.x + dir->X.y * dir->X.y
		                 + dir->X.z * dir->X.z);

		dir->X.x *= invhypot;
		dir->X.y *= invhypot;
		dir->X.z *= invhypot;

		invhypot = 1.0f
		         / sqrtf(dir->Y.x * dir->Y.x + dir->Y.y * dir->Y.y
		                 + dir->Y.z * dir->Y.z);

		dir->Y.x *= invhypot;
		dir->Y.y *= invhypot;
		dir->Y.z *= invhypot;

		invhypot = 1.0f
		         / sqrtf(dir->Z.x * dir->Z.x + dir->Z.y * dir->Z.y
		                 + dir->Z.z * dir->Z.z);

		dir->Z.x *= invhypot;
		dir->Z.y *= invhypot;
		dir->Z.z *= invhypot;

		tmpDir.X.x = dir->Y.y * dir->Z.z - dir->Y.z * dir->Z.y;
		tmpDir.X.y = dir->Y.z * dir->Z.x - dir->Y.x * dir->Z.z;
		tmpDir.X.z = dir->Y.x * dir->Z.y - dir->Y.y * dir->Z.x;

		tmpDir.Y.x = dir->Z.y * dir->X.z - dir->Z.z * dir->X.y;
		tmpDir.Y.y = dir->Z.z * dir->X.x - dir->Z.x * dir->X.z;
		tmpDir.Y.z = dir->Z.x * dir->X.y - dir->Z.y * dir->X.x;

		tmpDir.Z.x = dir->X.y * dir->Y.z - dir->X.z * dir->Y.y;
		tmpDir.Z.y = dir->X.z * dir->Y.x - dir->X.x * dir->Y.z;
		tmpDir.Z.z = dir->X.x * dir->Y.y - dir->X.y * dir->Y.x;

		sqrtTotal =
			sqrtf(tmpDir.X.x * tmpDir.X.x + tmpDir.X.y * tmpDir.X.y
		          + tmpDir.X.z * tmpDir.X.z);
		invhypot = 1.0f / sqrtTotal;

		dir->X.x = (invhypot * tmpDir.X.x + dir->X.x) * 0.5f;
		dir->X.y = (invhypot * tmpDir.X.y + dir->X.y) * 0.5f;
		dir->X.z = (invhypot * tmpDir.X.z + dir->X.z) * 0.5f;

		invhypot =
			sqrtf(tmpDir.Y.x * tmpDir.Y.x + tmpDir.Y.y * tmpDir.Y.y
		          + tmpDir.Y.z * tmpDir.Y.z);
		sqrtTotal += invhypot;
		invhypot = 1.0f / invhypot;

		dir->Y.x = (invhypot * tmpDir.Y.x + dir->Y.x) * 0.5f;
		dir->Y.y = (invhypot * tmpDir.Y.y + dir->Y.y) * 0.5f;
		dir->Y.z = (invhypot * tmpDir.Y.z + dir->Y.z) * 0.5f;

		invhypot =
			sqrtf(tmpDir.Z.x * tmpDir.Z.x + tmpDir.Z.y * tmpDir.Z.y
		          + tmpDir.Z.z * tmpDir.Z.z);
		sqrtTotal += invhypot;
		invhypot = 1.0f / invhypot;

		dir->Z.x = (invhypot * tmpDir.Z.x + dir->Z.x) * 0.5f;
		dir->Z.y = (invhypot * tmpDir.Z.y + dir->Z.y) * 0.5f;
		dir->Z.z = (invhypot * tmpDir.Z.z + dir->Z.z) * 0.5f;
	} while (sqrtTotal < maxSqrtTotal);
}

void __KMPLS_make_vec_dir(KPADMPDir *dir, Vec *a, Vec *b)
{
	Vec c, d, e;
	f32 invhypot, f, g, h, i, j, k, l;

	g = a->y * b->z;
	j = a->z * b->y;
	c.x = g - j;

	h = a->z * b->x;
	k = a->x * b->z;
	c.y = h - k;

	i = a->x * b->y;
	l = a->y * b->x;
	c.z = i - l;

	invhypot = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);

	if (invhypot == 0.0f)
	{
		*dir = e_dir;
		return;
	}

	invhypot = 1.0f / invhypot;
	c.x *= invhypot;
	c.y *= invhypot;
	c.z *= invhypot;

	d.x = a->y * c.z - a->z * c.y;
	d.y = a->z * c.x - a->x * c.z;
	d.z = a->x * c.y - a->y * c.x;

	e.x = b->y * c.z - b->z * c.y;
	e.y = b->z * c.x - b->x * c.z;
	e.z = b->x * c.y - b->y * c.x;

	dir->X.x = c.x * c.x + e.x * d.x + b->x * a->x;

	invhypot = c.y * c.x;
	dir->X.y = e.y * d.x + invhypot + i;

	f = c.z * c.x;
	dir->X.z = e.z * d.x + f + k;

	dir->Y.x = e.x * d.y + invhypot + l;
	dir->Y.y = c.y * c.y + e.y * d.y + b->y * a->y;

	invhypot = c.z * c.y;
	dir->Y.z = e.z * d.y + invhypot + g;

	dir->Z.x = e.x * d.z + f + h;
	dir->Z.y = e.y * d.z + invhypot + j;
	dir->Z.z = c.z * c.z + e.z * d.z + b->z * a->z;
}

void __KMPLS_mult_dir(KPADMPDir *a, KPADMPDir *b, KPADMPDir *dst)
{
	dst->X.x = b->X.x * a->X.x + b->X.y * a->Y.x + b->X.z * a->Z.x;
	dst->X.y = b->X.x * a->X.y + b->X.y * a->Y.y + b->X.z * a->Z.y;
	dst->X.z = b->X.x * a->X.z + b->X.y * a->Y.z + b->X.z * a->Z.z;

	dst->Y.x = b->Y.x * a->X.x + b->Y.y * a->Y.x + b->Y.z * a->Z.x;
	dst->Y.y = b->Y.x * a->X.y + b->Y.y * a->Y.y + b->Y.z * a->Z.y;
	dst->Y.z = b->Y.x * a->X.z + b->Y.y * a->Y.z + b->Y.z * a->Z.z;

	dst->Z.x = b->Z.x * a->X.x + b->Z.y * a->Y.x + b->Z.z * a->Z.x;
	dst->Z.y = b->Z.x * a->X.y + b->Z.y * a->Y.y + b->Z.z * a->Z.y;
	dst->Z.z = b->Z.x * a->X.z + b->Z.y * a->Y.z + b->Z.z * a->Z.z;
}

void __KMPLS_linear_Fxyz(Vec *a, Vec *b, f32 param_3, Vec *dst)
{
	dst->x = a->x + (b->x - a->x) * param_3;
	dst->y = a->y + (b->y - a->y) * param_3;
	dst->z = a->z + (b->z - a->z) * param_3;
}

void __KMPLS_linear_Direction(KPADMPDir *a, KPADMPDir *b, f32 param_3,
                              KPADMPDir *dst)
{
	__KMPLS_linear_Fxyz(&a->X, &b->X, param_3, &dst->X);
	__KMPLS_linear_Fxyz(&a->Y, &b->Y, param_3, &dst->Y);
	__KMPLS_linear_Fxyz(&a->Z, &b->Z, param_3, &dst->Z);

	__KMPLS_normalize_directionXYZ(dst, 2.999f);
}
