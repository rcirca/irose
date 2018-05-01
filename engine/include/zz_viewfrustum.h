/** 
 * @file zz_octree.h
 * @brief view frustum plane class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    04-dec-2003
 *
 * $Header: /engine/include/zz_viewfrustum.h 1     03-12-04 7:01p Zho $
 * $History: zz_viewfrustum.h $
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-04   Time: 7:01p
 * Created in $/engine/include
 * new added
 */

#ifndef __ZZ_VIEWFRUSTUM_H__
#define __ZZ_VIEWFRUSTUM_H__

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

//--------------------------------------------------------------------------------
// viewfrustum plane class
//--------------------------------------------------------------------------------
class zz_viewfrustum {
public:
	vec4 np, fp, lp, rp, tp, bp; // near/far/left/right/top/bottom plane equation
	vec3 p[8]; // eight frustum points. calculated by camera::update_frustum()

	zz_viewfrustum ()
		: np(vec4_null), fp(vec4_null), lp(vec4_null), rp(vec4_null), tp(vec4_null), bp(vec4_null)
	{}

	zz_viewfrustum (vec4 np_in, vec4 fp_in, vec4 lp_in, vec4 rp_in, vec4 tp_in, vec4 bp_in)
		: np(np_in), fp(fp_in), lp(lp_in), rp(rp_in), tp(tp_in), bp(bp_in)
	{}

	void set (vec4 np_in, vec4 fp_in, vec4 lp_in, vec4 rp_in, vec4 tp_in, vec4 bp_in)
	{
		np = np_in;
		fp = fp_in;
		lp = lp_in;
		rp = rp_in;
		tp = tp_in;
		bp = bp_in;
	}
};

#endif // __ZZ_VIEWFRUSTUM_H__