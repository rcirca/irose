/** 
 * @file zz_bvolume_obb.h
 * @brief obb class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-nov-2002
 *
 * $Id: zz_visible.h,v 1.1.1.1 2002/09/06 00:44:05 Jiho Choi Exp $
 */

#ifndef __ZZ_BVOLUME_OBB_H__
#define __ZZ_BVOLUME_OBB_H__

#include "zz_type.h"
#include "zz_algebra.h"

struct zz_bvolume_obb {
	vec3 center;
	vec3 half_length;
	mat3 rotation;

	zz_bvolume_obb () : center(0, 0, 0), half_length(0, 0, 0), rotation(mat3_id) {}
	~zz_bvolume_obb () {}
};

bool intersect (const zz_bvolume_obb& A, const zz_bvolume_obb& B);
//bool intersect (const zz_bvolume_obb& A, const vec3& origin, const vec3& direction, vec3& contact_point);
bool intersect (const zz_bvolume_obb& rkBox, const vec3& origin, const vec3& direction);

#endif // __ZZ_BVOLUME_OBB_H__
