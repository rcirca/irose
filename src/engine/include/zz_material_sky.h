/** 
 * @file zz_terrain_simple.h
 * @brief simple terrain class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    19-may-2003
 *
 * $Header: /engine/include/zz_material_sky.h 4     04-07-11 3:06p Zho $
 */

#ifndef __ZZ_MATERIAL_SKY_H__
#define __ZZ_MATERIAL_SKY_H__

#ifndef __ZZ_MATERIAL_H__
#include "zz_material.h"
#endif

class zz_renderer;

//--------------------------------------------------------------------------------
class zz_material_sky : public zz_material {
//--------------------------------------------------------------------------------
protected:
	float blend_ratio_; // if first texture is full blending, blend_ratio will be 1.0f. if second texture is full blending, blend_ratio will be 0.0f.
	void set_sky_one (void); // set one texture sky material
	void set_sky_two (void); // set two texture sky material

public:

	zz_material_sky (void);
	virtual ~zz_material_sky (void);
	virtual bool set (int pass);
	
	float get_blend_ratio ();
	void set_blend_ratio (float blend_ratio_in);

	ZZ_DECLARE_DYNAMIC(zz_material_sky)
};

inline float zz_material_sky::get_blend_ratio ()
{
	return blend_ratio_;
}

inline void zz_material_sky::set_blend_ratio (float blend_ratio_in)
{
	blend_ratio_ = blend_ratio_in;
}
#endif __ZZ_MATERIAL_SKY_H__