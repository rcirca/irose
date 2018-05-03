/** 
 * @file zz_bvolume_obb.cpp
 * @brief obb class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-nov-2002
 *
 * $Header: /engine/src/zz_bvolume.cpp 3     04-12-15 10:07a Choo0219 $
 * $History: zz_bvolume.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Choo0219     Date: 04-12-15   Time: 10:07a
 * Updated in $/engine/src
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-06-22   Time: 9:53a
 * Created in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-06   Time: 1:23p
 * Updated in $/engine/src
 * Added sphere version.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-22   Time: 8:05p
 * Updated in $/engine/src
 * Changed bv->half_length -> bv->local_half_length to be distinguished
 * from aabb or obb's half_length.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include <math.h>

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_type.h"
#include "zz_mem.h"
#include "zz_primitive.h"
#include "zz_octree.h"
#include "zz_bvolume.h"

zz_bvolume::zz_bvolume (vec3 local_center_in) :
aabb(NULL),
obb(NULL),
local_center(local_center_in),
local_radius(0.f),
local_half_length(vec3_null)
{
}

zz_bvolume::~zz_bvolume ()
{
	ZZ_SAFE_DELETE(aabb);
	ZZ_SAFE_DELETE(obb);
}

inline vec3 zz_bounding_obb::get_point (int index)
{
	float sign[3]; // xyz

	if (index & 1) sign[0] = -1.0f;
	else           sign[0] = 1.0f;
	if (index & 2) sign[1] = -1.0f;
	else           sign[1] = 1.0f;
	if (index & 4) sign[2] = -1.0f;
	else           sign[2] = 1.0f;

	return (center +
		sign[0]*half_length.x*rotation.col(0) + 
		sign[1]*half_length.y*rotation.col(1) + 
		sign[2]*half_length.z*rotation.col(2));
}

void zz_bvolume::set_obb (const vec3& center_in, const vec3& half_length_in)
{
	assert(!obb);
	assert(!aabb);

	obb = zz_new zz_bounding_obb;
	aabb = zz_new zz_bounding_aabb;

	// set local
	local_center = center_in;
	local_half_length = half_length_in; // CAUTION: share with obb
	local_radius = half_length_in.norm();
	
	// set sphere
	sphere.radius = local_radius;
	sphere.center = center_in;

	// set aabb
	aabb->center = center_in;
	aabb->half_length = half_length_in;
	aabb->pmin = center_in - half_length_in;
	aabb->pmax = center_in + half_length_in;

	// set obb
	obb->center = center_in;
	obb->half_length = half_length_in;
	obb->rotation = mat3_id; // no rotation
}

void zz_bvolume::set_obb (const vec3& half_length_in, const mat3& rotation_in)
{
	if (!obb) {
		obb = zz_new zz_bounding_obb;
	}
	if (!aabb) {
		aabb = zz_new zz_bounding_aabb;
	}
	assert(obb);
	assert(aabb);

	set_aabb(half_length_in);
	obb->half_length = half_length_in;
	obb->rotation = rotation_in;
	local_half_length = half_length_in; // CAUTION: share with aabb
}

void zz_bvolume::set_aabb (const vec3& half_length_in)
{
	if (!aabb) {
		aabb = zz_new zz_bounding_aabb;
	}
	assert(aabb);
	set_sphere(half_length_in.norm());
	aabb->half_length = half_length_in;
	aabb->pmin = aabb->center - half_length_in;
	aabb->pmax = aabb->center + half_length_in;
	local_half_length = half_length_in; // CAUTION: share with obb
}

void zz_bvolume::set_sphere (float radius_in)
{
	sphere.radius = radius_in;
	local_radius = radius_in;
}

// no rotation, 1-scale, zero local_center
// assumes this is only for aabb style bvolume
// ex. for terrain
void zz_bvolume::update_position (const vec3& position)
{
	if (obb) { // if has any obb, then set obb by arguments
		obb->center = position;
		// already set as obb->half_length = local_half_length;
		//obb->rotation = quat_id;
	}
	if (aabb) { // if has aabb and obb, then set aabb by obb
		aabb->center = position;
		aabb->pmin = position - local_half_length;
		aabb->pmax = position + local_half_length;
		//aabb->half_length = local_half_length;
	}
	// set sphere
	sphere.center = position;
	//already set as. sphere.radius = local_half_length.norm();
}

// updates bounding object(s) from transformation of the visible object
void zz_bvolume::update (const vec3& position, const mat3& rotation, const vec3& scale_in)
{
	// default volume : sphere
	// additional volume : aabb, obb, ....
	vec3 scale;
	scale.x = fabsf(scale_in.x);
	scale.y = fabsf(scale_in.y);
	scale.z = fabsf(scale_in.z);

	if (obb) { // if has any obb, then set obb by arguments
		obb->center = position + rotation * (local_center*scale);
		obb->half_length = local_half_length * scale;
		obb->rotation = rotation;
	}
	if (obb && aabb) { // if has aabb and obb, then set aabb by obb
		vec3 obb_point;
		vec3 pmin, pmax;
		aabb->center = obb->center;
		pmin = obb->get_point(0); // select any for start. In this case, use first index
		pmax = pmin;
		for (int i = 1; i < 8; ++i) { // note that index starts at 1, not 0.
			obb_point = obb->get_point(i);
			pmax.x = (obb_point.x > pmax.x) ? obb_point.x : pmax.x;
			pmax.y = (obb_point.y > pmax.y) ? obb_point.y : pmax.y;
			pmax.z = (obb_point.z > pmax.z) ? obb_point.z : pmax.z;

			pmin.x = (obb_point.x < pmin.x) ? obb_point.x : pmin.x;
			pmin.y = (obb_point.y < pmin.y) ? obb_point.y : pmin.y;
			pmin.z = (obb_point.z < pmin.z) ? obb_point.z : pmin.z;
		}
		aabb->pmin = pmin;
		aabb->pmax = pmax;
		aabb->half_length.x = pmax.x - aabb->center.x;
		aabb->half_length.y = pmax.y - aabb->center.y;
		aabb->half_length.z = pmax.z - aabb->center.z;
	}
	else if (aabb) { // if has aabb but no obb, set aabb by arguments
		aabb->center = position + rotation*(local_center*scale);
		aabb->half_length = local_half_length * scale;
		aabb->pmin = aabb->center - aabb->half_length;
		aabb->pmax = aabb->center + aabb->half_length;
	}
	// set sphere
	sphere.center = position + rotation*(local_center*scale);
	if (obb) sphere.radius = obb->half_length.norm();
	else if (aabb) sphere.radius = aabb->half_length.norm();
	else sphere.radius = local_radius*scale.x;
}

#define IN_BETWEEN(x, y, z) (((x) >= (y)) && ((x) <= (z)))

bool intersect (const zz_bounding_aabb& A, const zz_bounding_aabb& B)
{
	if ((A.pmin.x > B.pmax.x) || (A.pmax.x < B.pmin.x))
		return false;
	if ((A.pmin.y > B.pmax.y) || (A.pmax.y < B.pmin.y))
		return false;
	if ((A.pmin.z > B.pmax.z) || (A.pmax.z < B.pmin.z))
		return false;
	return true;
}

bool intersect (const zz_bounding_aabb& bounding_box, const vec3& origin, const vec3& direction)
{
    float fWdU[3], fAWdU[3], fDdU[3], fADdU[3], fAWxDdU[3], fRhs;
	vec3 kDiff; 
	 
    kDiff = origin - bounding_box.center;

    fWdU[0] = direction.x;
    fAWdU[0] = FABS(fWdU[0]);
    fDdU[0] = kDiff.x;
    fADdU[0] = FABS(fDdU[0]);
    if ( fADdU[0] > bounding_box.half_length.x && fDdU[0]*fWdU[0] >= 0.0f )
        return false;

    fWdU[1] = direction.y;
    fAWdU[1] = FABS(fWdU[1]);
    fDdU[1] = kDiff.y;
    fADdU[1] = FABS(fDdU[1]);
    if ( fADdU[1] > bounding_box.half_length.y && fDdU[1]*fWdU[1] >= 0.0f )
        return false;

    fWdU[2] = direction.z;
    fAWdU[2] = FABS(fWdU[2]);
    fDdU[2] = kDiff.z;
    fADdU[2] = FABS(fDdU[2]);
    if ( fADdU[2] > bounding_box.half_length.z && fDdU[2]*fWdU[2] >= 0.0f )
        return false;

    vec3 kWxD;
	cross(kWxD, direction, kDiff);

    fAWxDdU[0] = FABS(kWxD.x);
    fRhs = bounding_box.half_length.y*fAWdU[2] + bounding_box.half_length.z*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWxDdU[1] = FABS(kWxD.y);
    fRhs = bounding_box.half_length.x*fAWdU[2] + bounding_box.half_length.z*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = FABS(kWxD.z);
    fRhs = bounding_box.half_length.x*fAWdU[1] + bounding_box.half_length.y*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

    return true;
}

bool intersect_aabbbox(const vec3& min, const vec3& max, const vec3& origin, const vec3& direction)
{
    float fWdU[3], fAWdU[3], fDdU[3], fADdU[3], fAWxDdU[3], fRhs;
	vec3 kDiff; 
	vec3 center;
	vec3 length;

    center = 0.5f * (min + max);
	length = max-center;

	kDiff = origin - center;

    fWdU[0] = direction.x;
    fAWdU[0] = FABS(fWdU[0]);
    fDdU[0] = kDiff.x;
    fADdU[0] = FABS(fDdU[0]);
    if ( fADdU[0] > length.x && fDdU[0]*fWdU[0] >= 0.0f )
        return false;

    fWdU[1] = direction.y;
    fAWdU[1] = FABS(fWdU[1]);
    fDdU[1] = kDiff.y;
    fADdU[1] = FABS(fDdU[1]);
    if ( fADdU[1] > length.y && fDdU[1]*fWdU[1] >= 0.0f )
        return false;

    fWdU[2] = direction.z;
    fAWdU[2] = FABS(fWdU[2]);
    fDdU[2] = kDiff.z;
    fADdU[2] = FABS(fDdU[2]);
    if ( fADdU[2] > length.z && fDdU[2]*fWdU[2] >= 0.0f )
        return false;

    vec3 kWxD;
	cross(kWxD, direction, kDiff);

    fAWxDdU[0] = FABS(kWxD.x);
    fRhs = length.y*fAWdU[2] + length.z*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWxDdU[1] = FABS(kWxD.y);
    fRhs = length.x*fAWdU[2] + length.z*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = FABS(kWxD.z);
    fRhs = length.x*fAWdU[1] + length.y*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

    return true;
}



//const zz_bvolume_obb * zz_visible::get_obbox (void)
//{
//	if (!bv) return NULL;
//
//	bv->center = get_position() + bv_local_center;
//	bv->half_length = bv_half_length;
//	get_rotation().to_matrix(bv->rotation);
//
//	return bv;
//}

// fast implementation
////----------------------------------------------------------------------------
//bool MgcTestIntersection (const MgcBox3& rkBox0, const MgcBox3& rkBox1)
//{
//    // convenience variables
//    const MgcVector3* akA = rkBox0.Axes();
//    const MgcVector3* akB = rkBox1.Axes();
//    const MgcReal* afEA = rkBox0.Extents();
//    const MgcReal* afEB = rkBox1.Extents();
//
//    // compute difference of box centers, D = C1-C0
//    MgcVector3 kD = rkBox1.Center() - rkBox0.Center();
//
//    MgcReal aafC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
//    MgcReal aafAbsC[3][3];  // |c_{ij}|
//    MgcReal afAD[3];        // Dot(A_i,D)
//    MgcReal fR0, fR1, fR;   // interval radii and distance between centers
//    MgcReal fR01;           // = R0 + R1
//    
//    // axis C0+t*A0
//    aafC[0][0] = akA[0].Dot(akB[0]);
//    aafC[0][1] = akA[0].Dot(akB[1]);
//    aafC[0][2] = akA[0].Dot(akB[2]);
//    afAD[0] = akA[0].Dot(kD);
//    aafAbsC[0][0] = MgcMath::Abs(aafC[0][0]);
//    aafAbsC[0][1] = MgcMath::Abs(aafC[0][1]);
//    aafAbsC[0][2] = MgcMath::Abs(aafC[0][2]);
//    fR = MgcMath::Abs(afAD[0]);
//    fR1 = afEB[0]*aafAbsC[0][0]+afEB[1]*aafAbsC[0][1]+afEB[2]*aafAbsC[0][2];
//    fR01 = afEA[0] + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A1
//    aafC[1][0] = akA[1].Dot(akB[0]);
//    aafC[1][1] = akA[1].Dot(akB[1]);
//    aafC[1][2] = akA[1].Dot(akB[2]);
//    afAD[1] = akA[1].Dot(kD);
//    aafAbsC[1][0] = MgcMath::Abs(aafC[1][0]);
//    aafAbsC[1][1] = MgcMath::Abs(aafC[1][1]);
//    aafAbsC[1][2] = MgcMath::Abs(aafC[1][2]);
//    fR = MgcMath::Abs(afAD[1]);
//    fR1 = afEB[0]*aafAbsC[1][0]+afEB[1]*aafAbsC[1][1]+afEB[2]*aafAbsC[1][2];
//    fR01 = afEA[1] + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A2
//    aafC[2][0] = akA[2].Dot(akB[0]);
//    aafC[2][1] = akA[2].Dot(akB[1]);
//    aafC[2][2] = akA[2].Dot(akB[2]);
//    afAD[2] = akA[2].Dot(kD);
//    aafAbsC[2][0] = MgcMath::Abs(aafC[2][0]);
//    aafAbsC[2][1] = MgcMath::Abs(aafC[2][1]);
//    aafAbsC[2][2] = MgcMath::Abs(aafC[2][2]);
//    fR = MgcMath::Abs(afAD[2]);
//    fR1 = afEB[0]*aafAbsC[2][0]+afEB[1]*aafAbsC[2][1]+afEB[2]*aafAbsC[2][2];
//    fR01 = afEA[2] + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*B0
//    fR = MgcMath::Abs(akB[0].Dot(kD));
//    fR0 = afEA[0]*aafAbsC[0][0]+afEA[1]*aafAbsC[1][0]+afEA[2]*aafAbsC[2][0];
//    fR01 = fR0 + afEB[0];
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*B1
//    fR = MgcMath::Abs(akB[1].Dot(kD));
//    fR0 = afEA[0]*aafAbsC[0][1]+afEA[1]*aafAbsC[1][1]+afEA[2]*aafAbsC[2][1];
//    fR01 = fR0 + afEB[1];
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*B2
//    fR = MgcMath::Abs(akB[2].Dot(kD));
//    fR0 = afEA[0]*aafAbsC[0][2]+afEA[1]*aafAbsC[1][2]+afEA[2]*aafAbsC[2][2];
//    fR01 = fR0 + afEB[2];
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A0xB0
//    fR = MgcMath::Abs(afAD[2]*aafC[1][0]-afAD[1]*aafC[2][0]);
//    fR0 = afEA[1]*aafAbsC[2][0] + afEA[2]*aafAbsC[1][0];
//    fR1 = afEB[1]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][1];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A0xB1
//    fR = MgcMath::Abs(afAD[2]*aafC[1][1]-afAD[1]*aafC[2][1]);
//    fR0 = afEA[1]*aafAbsC[2][1] + afEA[2]*aafAbsC[1][1];
//    fR1 = afEB[0]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A0xB2
//    fR = MgcMath::Abs(afAD[2]*aafC[1][2]-afAD[1]*aafC[2][2]);
//    fR0 = afEA[1]*aafAbsC[2][2] + afEA[2]*aafAbsC[1][2];
//    fR1 = afEB[0]*aafAbsC[0][1] + afEB[1]*aafAbsC[0][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A1xB0
//    fR = MgcMath::Abs(afAD[0]*aafC[2][0]-afAD[2]*aafC[0][0]);
//    fR0 = afEA[0]*aafAbsC[2][0] + afEA[2]*aafAbsC[0][0];
//    fR1 = afEB[1]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][1];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A1xB1
//    fR = MgcMath::Abs(afAD[0]*aafC[2][1]-afAD[2]*aafC[0][1]);
//    fR0 = afEA[0]*aafAbsC[2][1] + afEA[2]*aafAbsC[0][1];
//    fR1 = afEB[0]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A1xB2
//    fR = MgcMath::Abs(afAD[0]*aafC[2][2]-afAD[2]*aafC[0][2]);
//    fR0 = afEA[0]*aafAbsC[2][2] + afEA[2]*aafAbsC[0][2];
//    fR1 = afEB[0]*aafAbsC[1][1] + afEB[1]*aafAbsC[1][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A2xB0
//    fR = MgcMath::Abs(afAD[1]*aafC[0][0]-afAD[0]*aafC[1][0]);
//    fR0 = afEA[0]*aafAbsC[1][0] + afEA[1]*aafAbsC[0][0];
//    fR1 = afEB[1]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][1];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A2xB1
//    fR = MgcMath::Abs(afAD[1]*aafC[0][1]-afAD[0]*aafC[1][1]);
//    fR0 = afEA[0]*aafAbsC[1][1] + afEA[1]*aafAbsC[0][1];
//    fR1 = afEB[0]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A2xB2
//    fR = MgcMath::Abs(afAD[1]*aafC[0][2]-afAD[0]*aafC[1][2]);
//    fR0 = afEA[0]*aafAbsC[1][2] + afEA[1]*aafAbsC[0][2];
//    fR1 = afEB[0]*aafAbsC[2][1] + afEB[1]*aafAbsC[2][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    return true;
//}

// easy implementation
////----------------------------------------------------------------------------
//bool Mgc::TestIntersection (const Box3& rkBox0, const Box3& rkBox1)
//{
//    // Cutoff for cosine of angles between box axes.  This is used to catch
//    // the cases when at least one pair of axes are parallel.  If this happens,
//    // there is no need to test for separation along the Cross(A[i],B[j])
//    // directions.
//    const float fCutoff = 0.999999f;
//    bool bExistsParallelPair = false;
//    int i;
//
//    // convenience variables
//    const Vector3* akA = rkBox0.Axes();
//    const Vector3* akB = rkBox1.Axes();
//    const Real* afEA = rkBox0.Extents();
//    const Real* afEB = rkBox1.Extents();
//
//    // compute difference of box centers, D = C1-C0
//    Vector3 kD = rkBox1.Center() - rkBox0.Center();
//
//    Real aafC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
//    Real aafAbsC[3][3];  // |c_{ij}|
//    Real afAD[3];        // Dot(A_i,D)
//    Real fR0, fR1, fR;   // interval radii and distance between centers
//    Real fR01;           // = R0 + R1
//
//    // axis C0+t*A0
//    for (i = 0; i < 3; i++)
//    {
//        aafC[0][i] = akA[0].Dot(akB[i]);
//        aafAbsC[0][i] = Math::FAbs(aafC[0][i]);
//        if ( aafAbsC[0][i] > fCutoff )
//            bExistsParallelPair = true;
//    }
//    afAD[0] = akA[0].Dot(kD);
//    fR = Math::FAbs(afAD[0]);
//    fR1 = afEB[0]*aafAbsC[0][0]+afEB[1]*aafAbsC[0][1]+afEB[2]*aafAbsC[0][2];
//    fR01 = afEA[0] + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A1
//    for (i = 0; i < 3; i++)
//    {
//        aafC[1][i] = akA[1].Dot(akB[i]);
//        aafAbsC[1][i] = Math::FAbs(aafC[1][i]);
//        if ( aafAbsC[1][i] > fCutoff )
//            bExistsParallelPair = true;
//    }
//    afAD[1] = akA[1].Dot(kD);
//    fR = Math::FAbs(afAD[1]);
//    fR1 = afEB[0]*aafAbsC[1][0]+afEB[1]*aafAbsC[1][1]+afEB[2]*aafAbsC[1][2];
//    fR01 = afEA[1] + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A2
//    for (i = 0; i < 3; i++)
//    {
//        aafC[2][i] = akA[2].Dot(akB[i]);
//        aafAbsC[2][i] = Math::FAbs(aafC[2][i]);
//        if ( aafAbsC[2][i] > fCutoff )
//            bExistsParallelPair = true;
//    }
//    afAD[2] = akA[2].Dot(kD);
//    fR = Math::FAbs(afAD[2]);
//    fR1 = afEB[0]*aafAbsC[2][0]+afEB[1]*aafAbsC[2][1]+afEB[2]*aafAbsC[2][2];
//    fR01 = afEA[2] + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*B0
//    fR = Math::FAbs(akB[0].Dot(kD));
//    fR0 = afEA[0]*aafAbsC[0][0]+afEA[1]*aafAbsC[1][0]+afEA[2]*aafAbsC[2][0];
//    fR01 = fR0 + afEB[0];
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*B1
//    fR = Math::FAbs(akB[1].Dot(kD));
//    fR0 = afEA[0]*aafAbsC[0][1]+afEA[1]*aafAbsC[1][1]+afEA[2]*aafAbsC[2][1];
//    fR01 = fR0 + afEB[1];
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*B2
//    fR = Math::FAbs(akB[2].Dot(kD));
//    fR0 = afEA[0]*aafAbsC[0][2]+afEA[1]*aafAbsC[1][2]+afEA[2]*aafAbsC[2][2];
//    fR01 = fR0 + afEB[2];
//    if ( fR > fR01 )
//        return false;
//
//    // At least one pair of box axes was parallel, so the separation is
//    // effectively in 2D where checking the "edge" normals is sufficient for
//    // the separation of the boxes.
//    if ( bExistsParallelPair )
//        return true;
//
//    // axis C0+t*A0xB0
//    fR = Math::FAbs(afAD[2]*aafC[1][0]-afAD[1]*aafC[2][0]);
//    fR0 = afEA[1]*aafAbsC[2][0] + afEA[2]*aafAbsC[1][0];
//    fR1 = afEB[1]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][1];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A0xB1
//    fR = Math::FAbs(afAD[2]*aafC[1][1]-afAD[1]*aafC[2][1]);
//    fR0 = afEA[1]*aafAbsC[2][1] + afEA[2]*aafAbsC[1][1];
//    fR1 = afEB[0]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A0xB2
//    fR = Math::FAbs(afAD[2]*aafC[1][2]-afAD[1]*aafC[2][2]);
//    fR0 = afEA[1]*aafAbsC[2][2] + afEA[2]*aafAbsC[1][2];
//    fR1 = afEB[0]*aafAbsC[0][1] + afEB[1]*aafAbsC[0][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A1xB0
//    fR = Math::FAbs(afAD[0]*aafC[2][0]-afAD[2]*aafC[0][0]);
//    fR0 = afEA[0]*aafAbsC[2][0] + afEA[2]*aafAbsC[0][0];
//    fR1 = afEB[1]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][1];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A1xB1
//    fR = Math::FAbs(afAD[0]*aafC[2][1]-afAD[2]*aafC[0][1]);
//    fR0 = afEA[0]*aafAbsC[2][1] + afEA[2]*aafAbsC[0][1];
//    fR1 = afEB[0]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A1xB2
//    fR = Math::FAbs(afAD[0]*aafC[2][2]-afAD[2]*aafC[0][2]);
//    fR0 = afEA[0]*aafAbsC[2][2] + afEA[2]*aafAbsC[0][2];
//    fR1 = afEB[0]*aafAbsC[1][1] + afEB[1]*aafAbsC[1][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A2xB0
//    fR = Math::FAbs(afAD[1]*aafC[0][0]-afAD[0]*aafC[1][0]);
//    fR0 = afEA[0]*aafAbsC[1][0] + afEA[1]*aafAbsC[0][0];
//    fR1 = afEB[1]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][1];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A2xB1
//    fR = Math::FAbs(afAD[1]*aafC[0][1]-afAD[0]*aafC[1][1]);
//    fR0 = afEA[0]*aafAbsC[1][1] + afEA[1]*aafAbsC[0][1];
//    fR1 = afEB[0]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    // axis C0+t*A2xB2
//    fR = Math::FAbs(afAD[1]*aafC[0][2]-afAD[0]*aafC[1][2]);
//    fR0 = afEA[0]*aafAbsC[1][2] + afEA[1]*aafAbsC[0][2];
//    fR1 = afEB[0]*aafAbsC[2][1] + afEB[1]*aafAbsC[2][0];
//    fR01 = fR0 + fR1;
//    if ( fR > fR01 )
//        return false;
//
//    return true;
//}


bool intersect (const zz_bounding_obb& A, const zz_bounding_obb& B)
{
	vec3 T; // distance between A.center and B.center
	mat3 R; // rotation matrix from B's coordinate to A's coordinate
	vec3 L; // separating axis
	float temp;

	T = A.center - B.center;
	mat3 m(A.rotation), tm;
	tm = transpose(m);
	R = tm * B.rotation;
	T = tm * T;
	
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
bool intersect (const zz_bounding_obb& bounding_box, const vec3& origin, const vec3& direction)
{
    float fWdU[3], fAWdU[3], fDdU[3], fADdU[3], fAWxDdU[3], fRhs;

	vec3 kDiff = origin - bounding_box.center;

    fWdU[0] = dot(direction, bounding_box.rotation.col(0));
    fAWdU[0] = FABS(fWdU[0]);
    fDdU[0] = dot(kDiff, bounding_box.rotation.col(0));
    fADdU[0] = FABS(fDdU[0]);
    if ( fADdU[0] > bounding_box.half_length.x && fDdU[0]*fWdU[0] >= 0.0f )
        return false;

    fWdU[1] = dot(direction, bounding_box.rotation.col(1));
    fAWdU[1] = FABS(fWdU[1]);
    fDdU[1] = dot(kDiff, bounding_box.rotation.col(1));
    fADdU[1] = FABS(fDdU[1]);
    if ( fADdU[1] > bounding_box.half_length.y && fDdU[1]*fWdU[1] >= 0.0f )
        return false;

    fWdU[2] = dot(direction, bounding_box.rotation.col(2));
    fAWdU[2] = FABS(fWdU[2]);
    fDdU[2] = dot(kDiff, bounding_box.rotation.col(2));
    fADdU[2] = FABS(fDdU[2]);
    if ( fADdU[2] > bounding_box.half_length.z && fDdU[2]*fWdU[2] >= 0.0f )
        return false;

    vec3 kWxD;
	cross(kWxD, direction, kDiff);

    fAWxDdU[0] = FABS(dot(kWxD, bounding_box.rotation.col(0)));
    fRhs = bounding_box.half_length.y*fAWdU[2] + bounding_box.half_length.z*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWxDdU[1] = FABS(dot(kWxD, bounding_box.rotation.col(1)));
    fRhs = bounding_box.half_length.x*fAWdU[2] + bounding_box.half_length.z*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = FABS(dot(kWxD, bounding_box.rotation.col(2)));
    fRhs = bounding_box.half_length.x*fAWdU[1] + bounding_box.half_length.y*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

    return true;
}



// from [noerror] method
// http://www.gamecode.org/article.php3?no=1393&page=0&current=0&field=tip
bool intersect (const zz_bounding_aabb& aabb, const zz_viewfrustum& view)
{
	vec4 plane[6]; // viewfrustum's normal
	vec3 minimum;

	plane[0] = vec4(view.np);
	plane[1] = vec4(view.fp);
	plane[2] = vec4(view.lp);
	plane[3] = vec4(view.rp);
	plane[4] = vec4(view.tp);
	plane[5] = vec4(view.bp);

	for (int i = 0; i < 6; i++) {
		// check normal
		minimum.x = plane[i].x < 0 ? aabb.pmax.x : aabb.pmin.x; 
		minimum.y = plane[i].y < 0 ? aabb.pmax.y : aabb.pmin.y; 
		minimum.z = plane[i].z < 0 ? aabb.pmax.z : aabb.pmin.z; 

		//if (plane_distance(viewfrustum_pos[i], bottom) > 0) 
		if ((dot(vec3(plane[i]), minimum) + plane[i].w) > 0) {
			return false;
		}
	}
	return true;
}


// from [noerror] method
// http://www.gamecode.org/article.php3?no=1393&page=0&current=0&field=tip
bool intersect (const vec3& pmin, const vec3& pmax, const zz_viewfrustum& view)
{
	vec4 plane[6]; // viewfrustum's normal
	vec3 minimum;

	plane[0] = vec4(view.np);
	plane[1] = vec4(view.fp);
	plane[2] = vec4(view.lp);
	plane[3] = vec4(view.rp);
	plane[4] = vec4(view.tp);
	plane[5] = vec4(view.bp);

	for (int i = 0; i < 6; i++) {
		// check normal
		minimum.x = plane[i].x < 0 ? pmax.x : pmin.x; 
		minimum.y = plane[i].y < 0 ? pmax.y : pmin.y; 
		minimum.z = plane[i].z < 0 ? pmax.z : pmin.z; 

		//if (plane_distance(viewfrustum_pos[i], bottom) > 0) 
		if ((dot(vec3(plane[i]), minimum) + plane[i].w) > 0) {
			return false;
		}
	}
	return true;
}


//// from [nebular] code
//bool intersect (const zz_bounding_aabb & aabb, float np[4], float fp[4],
//                float left[4], float right[4], float top[4],
//                float bottom[4], float pos[3])
//{
//	unsigned char zones[8]={0,0,0,0,0,0,0,0};
//	float pt[8][3];
//	int ii;
//	//float dir[3];
//	//float dist;
//	
//	//compute the half dimensions
//	float wd2 = (aabb.pmax.x - aabb.pmin.x) / 2.0f; //float wd2=m_width/2.0f;
//	float hd2 = (aabb.pmax.y - aabb.pmin.y) / 2.0f; //float hd2=m_height/2.0f;
//	float dd2 = (aabb.pmax.z - aabb.pmin.z) / 2.0f; //float dd2=m_depth/2.0f;
//	vec3 center;
//	center = .5f*(aabb.pmax + aabb.pmin);
//	//bool hint;
//
//	//loop throught the eight corners of the node
//	// and determine where they lie with respect
//	// to the frustum planes
//	//
//	// This is sub-optimal as several points are
//	// actually checked multiple times, but it
//	// seems to make the code more understandable
//	for(ii=0;ii<8;ii++){
//		//compute the location of the current corner
//		pt[ii][0]=aabb.center.x+((ii&0x1)?wd2:-wd2);
//		pt[ii][1]=aabb.center.y+((ii&0x2)?hd2:-hd2);
//		pt[ii][2]=aabb.center.z+((ii&0x4)?dd2:-dd2);
//
//		if((np[0]*pt[ii][0]+np[1]*pt[ii][1]+np[2]*pt[ii][2]+np[3])>-0.01f)
//			zones[ii]|=0x01;
//		else if((fp[0]*pt[ii][0]+fp[1]*pt[ii][1]+fp[2]*pt[ii][2]+fp[3])>-0.01f)
//			zones[ii]|=0x02;
//
//		if((left[0]*pt[ii][0]+left[1]*pt[ii][1]+left[2]*pt[ii][2]+left[3])>-0.01f)
//			zones[ii]|=0x04;
//		if((right[0]*pt[ii][0]+right[1]*pt[ii][1]+right[2]*pt[ii][2]+right[3])>-0.01f)
//			zones[ii]|=0x08;
//
//		if((top[0]*pt[ii][0]+top[1]*pt[ii][1]+top[2]*pt[ii][2]+top[3])>-0.01f)
//			zones[ii]|=0x10;
//		if((bottom[0]*pt[ii][0]+bottom[1]*pt[ii][1]+bottom[2]*pt[ii][2]+bottom[3])>-0.01f)
//			zones[ii]|=0x20;
//	
//		// if any point was contained within the view-frustum
//		if (zones[ii] == 0x00) { 
//			return true;
//		}
//	}
//	return false;
//}

void zz_bvolume::draw_bounding_volume (void)
{
	const int num_indexes = 8;
	static float color_indexes[8][3] = {
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 1.0f},
		{0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f}
	};

	static vec3 wire_color;
	static int index;
	index = int(this) / 100;
	index %= num_indexes;
	wire_color.set( color_indexes[index][0], color_indexes[index][1], color_indexes[index][2] );

	if (1) {
		const zz_bounding_obb * obb = get_obb();
		if (obb) draw_box(obb->center,	obb->half_length, obb->rotation, wire_color);
	}
	if (0) {
		const zz_bounding_aabb * aabb = get_aabb();
		draw_aabb(aabb->pmin, aabb->pmax, wire_color);
	}
	if (0) {
		const zz_bounding_sphere * sphere = get_sphere();
		draw_sphere(sphere->center, sphere->radius, wire_color);
	}
}

// sphere-sphere intersection
bool intersect (const zz_bounding_sphere& a, const zz_bounding_sphere& b)
{
	vec3 diff = a.center - b.center;
	float rsum = a.radius + b.radius;
	return dot(diff, diff) <= rsum*rsum;
}

// ray-sphere intersection
bool intersect (const zz_bounding_sphere& sphere, const vec3& origin, const vec3& direction)
{
	// quadratic is t^2 + 2*a1*t + a0 = 0
	vec3 q = origin - sphere.center;

	float a0 = dot(q, q) - sphere.radius*sphere.radius;
	
	if (a0 <= 0) {
		// origin is inside the sphere
		return true;
	}
	// else origin is outside the sphere

	float a1 = dot(direction, q);
	if (a1 >= 0) {
		// acute angle between origin-center and direction, center is "behind" ray
		return false;
	}

	// quadratic has a real root if discriminant is nonnegative
	return ( a1*a1 >= a0 );
}

// from [http://www.magic-software.com/Source/Distance/WmlDistVec3Tri3.cpp]
float distance_square_tri_point (const vec3& rkPoint,
								 const vec3& tri0, const vec3& tri1, const vec3& tri2,
								 float * pfSParam, float * pfTParam)
{
	vec3 edge0, edge1;
	edge0 = tri1 - tri0;
	edge1 = tri2 - tri0;
    vec3 kDiff = tri0 - rkPoint;
    float fA00 = edge0.sq_norm();
    float fA01 = dot(edge0, edge1);
    float fA11 = edge1.sq_norm();
    float fB0 = dot(kDiff, edge0);
    float fB1 = dot(kDiff, edge1);
    float fC = kDiff.sq_norm();
    float fDet = FABS(fA00*fA11-fA01*fA01);
    float fS = fA01*fB1-fA11*fB0;
    float fT = fA01*fB0-fA00*fB1;
    float fSqrDist;

    if ( fS + fT <= fDet )
    {
        if ( fS < (float)0.0 )
        {
            if ( fT < (float)0.0 )  // region 4
            {
                if ( fB0 < (float)0.0 )
                {
                    fT = (float)0.0;
                    if ( -fB0 >= fA00 )
                    {
                        fS = (float)1.0;
                        fSqrDist = fA00+((float)2.0)*fB0+fC;
                    }
                    else
                    {
                        fS = -fB0/fA00;
                        fSqrDist = fB0*fS+fC;
                    }
                }
                else
                {
                    fS = (float)0.0;
                    if ( fB1 >= (float)0.0 )
                    {
                        fT = (float)0.0;
                        fSqrDist = fC;
                    }
                    else if ( -fB1 >= fA11 )
                    {
                        fT = (float)1.0;
                        fSqrDist = fA11+((float)2.0)*fB1+fC;
                    }
                    else
                    {
                        fT = -fB1/fA11;
                        fSqrDist = fB1*fT+fC;
                    }
                }
            }
            else  // region 3
            {
                fS = (float)0.0;
                if ( fB1 >= (float)0.0 )
                {
                    fT = (float)0.0;
                    fSqrDist = fC;
                }
                else if ( -fB1 >= fA11 )
                {
                    fT = (float)1.0;
                    fSqrDist = fA11+((float)2.0)*fB1+fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDist = fB1*fT+fC;
                }
            }
        }
        else if ( fT < (float)0.0 )  // region 5
        {
            fT = (float)0.0;
            if ( fB0 >= (float)0.0 )
            {
                fS = (float)0.0;
                fSqrDist = fC;
            }
            else if ( -fB0 >= fA00 )
            {
                fS = (float)1.0;
                fSqrDist = fA00+((float)2.0)*fB0+fC;
            }
            else
            {
                fS = -fB0/fA00;
                fSqrDist = fB0*fS+fC;
            }
        }
        else  // region 0
        {
            // minimum at interior point
            float fInvDet = ((float)1.0)/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDist = fS*(fA00*fS+fA01*fT+((float)2.0)*fB0) +
                fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
        }
    }
    else
    {
        float fTmp0, fTmp1, fNumer, fDenom;

        if ( fS < (float)0.0 )  // region 2
        {
            fTmp0 = fA01 + fB0;
            fTmp1 = fA11 + fB1;
            if ( fTmp1 > fTmp0 )
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-2.0f*fA01+fA11;
                if ( fNumer >= fDenom )
                {
                    fS = (float)1.0;
                    fT = (float)0.0;
                    fSqrDist = fA00+((float)2.0)*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = (float)1.0 - fS;
                    fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                        fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
                }
            }
            else
            {
                fS = (float)0.0;
                if ( fTmp1 <= (float)0.0 )
                {
                    fT = (float)1.0;
                    fSqrDist = fA11+((float)2.0)*fB1+fC;
                }
                else if ( fB1 >= (float)0.0 )
                {
                    fT = (float)0.0;
                    fSqrDist = fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDist = fB1*fT+fC;
                }
            }
        }
        else if ( fT < (float)0.0 )  // region 6
        {
            fTmp0 = fA01 + fB1;
            fTmp1 = fA00 + fB0;
            if ( fTmp1 > fTmp0 )
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-((float)2.0)*fA01+fA11;
                if ( fNumer >= fDenom )
                {
                    fT = (float)1.0;
                    fS = (float)0.0;
                    fSqrDist = fA11+((float)2.0)*fB1+fC;
                }
                else
                {
                    fT = fNumer/fDenom;
                    fS = (float)1.0 - fT;
                    fSqrDist = fS*(fA00*fS+fA01*fT+((float)2.0)*fB0) +
                        fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
                }
            }
            else
            {
                fT = (float)0.0;
                if ( fTmp1 <= (float)0.0 )
                {
                    fS = (float)1.0;
                    fSqrDist = fA00+((float)2.0)*fB0+fC;
                }
                else if ( fB0 >= (float)0.0 )
                {
                    fS = (float)0.0;
                    fSqrDist = fC;
                }
                else
                {
                    fS = -fB0/fA00;
                    fSqrDist = fB0*fS+fC;
                }
            }
        }
        else  // region 1
        {
            fNumer = fA11 + fB1 - fA01 - fB0;
            if ( fNumer <= (float)0.0 )
            {
                fS = (float)0.0;
                fT = (float)1.0;
                fSqrDist = fA11+((float)2.0)*fB1+fC;
            }
            else
            {
                fDenom = fA00-2.0f*fA01+fA11;
                if ( fNumer >= fDenom )
                {
                    fS = (float)1.0;
                    fT = (float)0.0;
                    fSqrDist = fA00+((float)2.0)*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = (float)1.0 - fS;
                    fSqrDist = fS*(fA00*fS+fA01*fT+((float)2.0)*fB0) +
                        fT*(fA01*fS+fA11*fT+((float)2.0)*fB1)+fC;
                }
            }
        }
    }

    if ( pfSParam )
        *pfSParam = fS;

    if ( pfTParam )
        *pfTParam = fT;

    return FABS(fSqrDist);
}

bool intersect_sphere_tri (const zz_bounding_sphere& sphere, const vec3& p0, const vec3& p1, const vec3& p2)
{
    float distance_square = distance_square_tri_point(sphere.center, p0, p1, p2);
	float radius_square = sphere.radius * sphere.radius;

	return (distance_square < radius_square);
}

// loose sphere-triangle intersection
bool intersect_sphere_tri_loose (const zz_bounding_sphere& sphere, const vec3& p0, const vec3& p1, const vec3& p2)
{
	const vec3& center = sphere.center;
	float radius_square = sphere.radius*sphere.radius;

	if (center.distance_square(p0) < radius_square) return true;
	if (center.distance_square(p1) < radius_square) return true;
	if (center.distance_square(p2) < radius_square) return true;
	return false;
}
