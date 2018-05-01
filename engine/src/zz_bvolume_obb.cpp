/** 
 * @file zz_bvolume_obb.cpp
 * @brief obb class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-nov-2002
 *
 * $Id: zz_visible.h,v 1.1.1.1 2002/09/06 00:44:05 Jiho Choi Exp $
 */

#include "zz_tier0.h"
#include <math.h>
#include "zz_bvolume_obb.h"

bool intersect (const zz_bvolume_obb& A, const zz_bvolume_obb& B)
{
	vec3 T; // distance between A.center and B.center
	mat3 R; // rotation matrix from B's coordinate to A's coordinate
	vec3 L; // separating axis
	float temp;

	T = A.center - B.center;
	mat3 m(A.rotation);
	R = transpose(m) * B.rotation;

	// 1. L = A1
	temp = A.half_length.x + B.half_length.x*FABS(R._11) + B.half_length.y*FABS(R._12) + B.half_length.z*FABS(R._13);
	if (FABS(T.x) > temp) return false;

	// 2. L = A2
	temp = A.half_length.y + B.half_length.x*FABS(R._21) + B.half_length.y*FABS(R._22) + B.half_length.z*FABS(R._23);
	if (FABS(T.y) > temp) return false;

	// 3. L = A3
	temp = A.half_length.z + B.half_length.x*FABS(R._31) + B.half_length.y*FABS(R._32) + B.half_length.z*FABS(R._33);
	if (FABS(T.z) > temp) return false;

	// 4. L = RB1
	temp = A.half_length.x*FABS(R._11) + A.half_length.y*FABS(R._21) + A.half_length.z*FABS(R._31) + B.half_length.x;
	if (FABS(T.x*R._11 + T.y*R._21 + T.z*R._31) > temp) return false;

	// 5. L = RB2
	temp = A.half_length.x*FABS(R._12) + A.half_length.y*FABS(R._22) + A.half_length.z*FABS(R._32) + B.half_length.y;
	if (FABS(T.x*R._12 + T.y*R._22 + T.z*R._32) > temp) return false;

	// 6. L = RB3
	temp = A.half_length.x*FABS(R._13) + A.half_length.y*FABS(R._23) + A.half_length.z*FABS(R._33) + B.half_length.z;
	if (FABS(T.x*R._13 + T.y*R._23 + T.z*R._33) > temp) return false;

	// 7. L = A1xRB1
	temp = A.half_length.y*FABS(R._31) + A.half_length.z*FABS(R._21) + B.half_length.y*FABS(R._13) + B.half_length.z*FABS(R._12);
	if (FABS(-T.y*R._31 + T.z*R._21) > temp) return false;

	// 8. L = A1xRB2
	temp = A.half_length.y*FABS(R._32) + A.half_length.z*FABS(R._22) + B.half_length.x*FABS(R._13) + B.half_length.z*FABS(R._11);
	if (FABS(-T.y*R._32 + T.z*R._22) > temp) return false;

	// 9. L = A1xRB3
	temp = A.half_length.y*FABS(R._33) + A.half_length.z*FABS(R._23) + B.half_length.x*FABS(R._12) + B.half_length.y*FABS(R._11);
	if (FABS(-T.y*R._33 + T.z*R._23) > temp) return false;

	// 10. L = A2xRB1
	temp = A.half_length.x*FABS(R._31) + A.half_length.z*FABS(R._11) + B.half_length.y*FABS(R._23) + B.half_length.z*FABS(R._22);
	if (FABS(T.x*R._31 - T.z*R._11) > temp) return false;

	// 11. L = A2xRB2
	temp = A.half_length.x*FABS(R._32) + A.half_length.z*FABS(R._12) + B.half_length.x*FABS(R._23) + B.half_length.z*FABS(R._21);
	if (FABS(T.x*R._32 - T.z*R._12) > temp) return false;

	// 12. L = A2xRB3
	temp = A.half_length.x*FABS(R._33) + A.half_length.z*FABS(R._13) + B.half_length.x*FABS(R._22) + B.half_length.y*FABS(R._21);
	if (FABS(T.x*R._33 - T.z*R._13) > temp) return false;

	// 13. L = A3xRB1
	temp = A.half_length.x*FABS(R._21) + A.half_length.y*FABS(R._11) + B.half_length.y*FABS(R._33) + B.half_length.z*FABS(R._32);
	if (FABS(-T.x*R._21 + T.y*R._11) > temp) return false;

	// 14. L = A3xRB2
	temp = A.half_length.x*FABS(R._22) + A.half_length.y*FABS(R._12) + B.half_length.x*FABS(R._33) + B.half_length.z*FABS(R._31);
	if (FABS(-T.x*R._22 + T.y*R._12) > temp) return false;

	// 15. L = A3xRB3
	temp = A.half_length.x*FABS(R._23) + A.half_length.y*FABS(R._13) + B.half_length.x*FABS(R._32) + B.half_length.y*FABS(R._31);
	if (FABS(-T.x*R._23 + T.y*R._13) > temp) return false;

	return true;
}
//
//bool intersect (const zz_bvolume_obb& A, const vec3& origin, const vec3& direction, 
//				vec3& contact_point)
//{
//	float tmin = -9999999999.f;
//	float tmax = 9999999999.f;
//	vec3 p = A.center - origin;
//	vec3 a[3]; // normalized side direction of the box
//	float e, f, t1, t2, temp;
//
//	contact_point = origin;
//	for (int i = 0; i < 3; i++) {
//		a[i] = A.rotation.col(i);
//		a[i].normalize();
//		e = dot(a[i], p);
//		f = dot(a[i], direction);
//		if (FABS(f) > 0.01) {
//			t1 = (e + A.half_length[i])/f;
//			t2 = (e - A.half_length[i])/f;
//			if (t1 > t2) {
//				// swap
//				temp = t2;
//				t2 = t1;
//				t1 = temp;
//			}
//			if (t1 > tmin) tmin = t1;
//			if (t2 < tmax) tmax = t2;
//			if (tmin > tmax) return false;
//			if (tmax < 0) return false;
//			// ? maybe errata
//			if (((-e - A.half_length[i]) > 0) || (-e + A.half_length[i]) < 0) return false;
//		}
//	}
//	if (tmin > 0) {
//		contact_point = origin + tmin*direction;
//		return true;
//	}
//	contact_point = origin + tmax*direction;
//	return true;
//}

// from [http://www.magic-software.com]
// bool Mgc::TestIntersection (const Ray3& rkRay, const Box3& rkBox)
bool intersect (const zz_bvolume_obb& rkBox, const vec3& origin, const vec3& direction)
{
    float fWdU[3], fAWdU[3], fDdU[3], fADdU[3], fAWxDdU[3], fRhs;

	vec3 kDiff = origin - rkBox.center;

    fWdU[0] = dot(direction, rkBox.rotation.col(0));
    fAWdU[0] = FABS(fWdU[0]);
    fDdU[0] = dot(kDiff, rkBox.rotation.col(0));
    fADdU[0] = FABS(fDdU[0]);
    if ( fADdU[0] > rkBox.half_length.x && fDdU[0]*fWdU[0] >= 0.0f )
        return false;

    fWdU[1] = dot(direction, rkBox.rotation.col(1));
    fAWdU[1] = FABS(fWdU[1]);
    fDdU[1] = dot(kDiff, rkBox.rotation.col(1));
    fADdU[1] = FABS(fDdU[1]);
    if ( fADdU[1] > rkBox.half_length.y && fDdU[1]*fWdU[1] >= 0.0f )
        return false;

    fWdU[2] = dot(direction, rkBox.rotation.col(2));
    fAWdU[2] = FABS(fWdU[2]);
    fDdU[2] = dot(kDiff, rkBox.rotation.col(2));
    fADdU[2] = FABS(fDdU[2]);
    if ( fADdU[2] > rkBox.half_length.z && fDdU[2]*fWdU[2] >= 0.0f )
        return false;

    vec3 kWxD;
	cross(kWxD, direction, kDiff);

    fAWxDdU[0] = FABS(dot(kWxD, rkBox.rotation.col(0)));
    fRhs = rkBox.half_length.y*fAWdU[2] + rkBox.half_length.z*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWxDdU[1] = FABS(dot(kWxD, rkBox.rotation.col(1)));
    fRhs = rkBox.half_length.x*fAWdU[2] + rkBox.half_length.z*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = FABS(dot(kWxD, rkBox.rotation.col(2)));
    fRhs = rkBox.half_length.x*fAWdU[1] + rkBox.half_length.y*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

    return true;
}
