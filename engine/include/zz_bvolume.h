/** 
 * @file zz_bvolume.h
 * @brief bounding volume class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    18-dec-2002
 *
 * $Header: /engine/include/zz_bvolume.h 3     04-12-15 10:07a Choo0219 $
 * $History: zz_bvolume.h $
 * 
 * *****************  Version 3  *****************
 * User: Choo0219     Date: 04-12-15   Time: 10:07a
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-06-22   Time: 9:53a
 * Created in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-05-19   Time: 8:12p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-29   Time: 11:30p
 * Updated in $/engine/include
 * 2d intersection added
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-23   Time: 5:18p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-22   Time: 8:05p
 * Updated in $/engine/include
 * Changed bv->half_length -> bv->local_half_length to be distinguished
 * from aabb or obb's half_length.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:05p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:53p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:47p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:19p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef __ZZ_BVOLUME_H__
#define __ZZ_BVOLUME_H__

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

enum zz_bvolume_type {
	ZZ_BV_NONE		= 0,
	ZZ_BV_SPHERE	= 1,
	ZZ_BV_AABB		= 2,
	ZZ_BV_OBB		= 3,
	ZZ_BV_FRUSTUM	= 4
};

//--------------------------------------------------------------------------------
struct zz_bounding_sphere {
//--------------------------------------------------------------------------------
	vec3 center; // world coordsys
	float radius;

	zz_bounding_sphere () : center(vec3_null), radius(0.0f) {}
	zz_bounding_sphere (float radius_in, const vec3& center_in) : center(center_in), radius(radius_in) {}
	zz_bounding_sphere (const zz_bounding_sphere& sphere_in) : center(sphere_in.center), radius(sphere_in.radius) {}
};

//--------------------------------------------------------------------------------
struct zz_bounding_aabb {
//--------------------------------------------------------------------------------
	vec3 center; // world coordsys
	vec3 pmin; // world coordsys
	vec3 pmax; // world coordsys
	vec3 half_length;
    
	enum {
        OUTSIDE     = 0,
        ISEQUAL     = (1<<0),
        ISCONTAINED = (1<<1),
        CONTAINS    = (1<<2),
        CLIPS       = (1<<3),
    };

	zz_bounding_aabb () : center(vec3_null),
		pmin(vec3_null), pmax(vec3_null),
		half_length(vec3_null)
	{}

	zz_bounding_aabb (const vec3& pmin_in, const vec3& pmax_in) {
		pmin = pmin_in;
		pmax = pmax_in;
		center = .5f * (pmin + pmax);
		half_length = pmax - center;
	}

	// from [nebula bbox3.h]
	//--- check if box intersects, contains or is contained in other box
	//--- by doing 3 projection tests for each dimension, if all 3 test 
	//--- return true, then the 2 boxes intersect
	static int line_test(float v0, float v1, float w0, float w1)
	{
		// quick rejection test
		if ((v1<w0) || (v0>w1)) return OUTSIDE;
		else if ((v0==w0) && (v1==w1)) return ISEQUAL;
		else if ((v0>=w0) && (v1<=w1)) return ISCONTAINED;
		else if ((v0<=w0) && (v1>=w1)) return CONTAINS;
		else return CLIPS;
	}

	static int intersect (const vec3& min_src, const vec3& max_src, const vec3& min_dest, const vec3& max_dest)
	{
		int and_code = 0xffff;
		int or_code  = 0;
		int cx,cy,cz;
		cx = line_test(min_src.x, max_src.x, min_dest.x, max_dest.x);
		and_code&=cx; or_code|=cx;
		cy = line_test(min_src.y, max_src.y, min_dest.y, max_dest.y);
		and_code&=cy; or_code|=cy;
		cz = line_test(min_src.z, max_src.z, min_dest.z, max_dest.z);
		and_code&=cz; or_code|=cz;

		if (or_code == 0) return OUTSIDE;
		else if (and_code != 0) {
			return and_code;
		} else {
			// only if all test produced a non-outside result,
			// an intersection has occured
			if (cx && cy && cz) return CLIPS;
			else return OUTSIDE;
		}
	}

	int intersect (const vec3& pmin_in, const vec3& pmax_in) const
	{
		return zz_bounding_aabb::intersect(pmin, pmax, pmin_in, pmax_in);
	}

	// 2d version
	int intersect2d (const vec3& pmin_in, const vec3& pmax_in) const {
		int and_code = 0xffff;
		int or_code  = 0;
		int cx,cy;
		cx = line_test(pmin.x, pmax.x, pmin_in.x, pmax_in.x);
		and_code&=cx; or_code|=cx;
		cy = line_test(pmin.y, pmax.y, pmin_in.y, pmax_in.y);
		and_code&=cy; or_code|=cy;

		if (or_code == 0) return OUTSIDE;
		else if (and_code != 0) {
			return and_code;
		} else {
			// only if all test produced a non-outside result,
			// an intersection has occured
			if (cx && cy) return CLIPS;
			else return OUTSIDE;
		}
	}
};

//--------------------------------------------------------------------------------
struct zz_bounding_obb {
//--------------------------------------------------------------------------------
	vec3 center;
	vec3 half_length;
	mat3 rotation;

	zz_bounding_obb () : center(vec3_null), half_length(vec3_null), rotation(mat3_id) {}

	// get index'th point
	vec3 get_point (int index);
};

struct zz_bounding_viewfrustum {
	vec4 near_plane;
	vec4 far_plane;
	vec4 left_plane;
	vec4 right_plane;
	vec4 top_plane;
	vec4 bottom_plane;
	vec3 position;
};

// bounding volume interface
//--------------------------------------------------------------------------------
class zz_bvolume {
//--------------------------------------------------------------------------------
private:
	// source values
	vec3 local_center; // local center from object coordinate system(or root bone's coordinate system)
	float local_radius; // initial radius
	vec3 local_half_length; // initial half_length

	zz_bounding_sphere sphere; // default
	zz_bounding_aabb * aabb; // optional
	zz_bounding_obb * obb; // optional

public:
	zz_bvolume (vec3 local_center_in = vec3_null);

	~zz_bvolume ();

	// reconstruct upper level bounding objects(obb->aabb->sphere)
	void set_local_center (const vec3& local_center_in)
	{
		local_center = local_center_in;
	}

	const vec3 get_local_half_length () const
	{
		return local_half_length;
	}

	void set_obb (const vec3& center_in, const vec3& half_length_in); // no rotation
	void set_obb (const vec3& half_length_in, const mat3& rotation_in = mat3_id);
	void set_aabb (const vec3& half_length_in);
	void set_sphere (float radius_in);

	void update (const vec3& position, const mat3& rotation = mat3_id, const vec3& scale_in = vec3_one);

	// only for position of AABB style
	void update_position (const vec3& position);

	const zz_bounding_sphere * get_sphere () const { return &sphere; }
	const zz_bounding_aabb * get_aabb () const { return aabb; }
	const zz_bounding_obb * get_obb () const { return obb; }

	zz_bvolume_type get_type () const
	{
		if (obb) return ZZ_BV_OBB;
		if (aabb) return ZZ_BV_AABB;
		return ZZ_BV_SPHERE;
	}

	void draw_bounding_volume (void);
};
// zz_visible::create_bounding_volume()
// -> set_local_center()
// -> set_aabb() or set_obb() or set_sphere()
// -> set_position() or set_rotation() or set_scale()
// -> update()

class zz_viewfrustum;
bool intersect (const zz_bounding_aabb& aabb, const zz_viewfrustum& view);
bool intersect (const vec3& pmin, const vec3& pmax, const zz_viewfrustum& view);

// aabb-aabb
bool intersect (const zz_bounding_aabb& A, const zz_bounding_aabb& B);
bool intersect (const zz_bounding_aabb& bounding_box, const vec3& origin, const vec3& direction);
bool intersect_aabbbox(const vec3& min, const vec3& max, const vec3& origin, const vec3& direction);

// obb-obb, obb-ray
bool intersect (const zz_bounding_obb& A, const zz_bounding_obb& B);
bool intersect (const zz_bounding_obb& bounding_box, const vec3& origin, const vec3& direction);
//bool intersect (const zz_bvolume_obb& A, const vec3& origin, const vec3& direction, vec3& contact_point);

// sphere-sphere, sphere-ray
bool intersect (const zz_bounding_sphere& a, const zz_bounding_sphere& b);
bool intersect (const zz_bounding_sphere& a, const vec3& origin, const vec3& direction);

// sphere-triangle intersection
bool intersect_sphere_tri (const zz_bounding_sphere& sphere, const vec3& p0, const vec3& p1, const vec3& p2);

// loose sphere-triangle intersection
bool intersect_sphere_tri_loose (const zz_bounding_sphere& sphere, const vec3& p0, const vec3& p1, const vec3& p2);

float distance_square_tri_point (const vec3& rkPoint,
								 const vec3& tri0, const vec3& tri1, const vec3& tri2,
								 float * pfSParam = 0, float * pfTParam = 0);
#endif // __ZZ_BVOLUME_H__
