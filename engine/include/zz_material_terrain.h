/** 
 * @file zz_material_terrain.h
 * @brief terrain material class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-feb-2003
 *
 * $Header: /engine/include/zz_material_terrain.h 11    04-07-29 10:47a Zho $
 */

#ifndef __ZZ_MATERIAL_TERRAIN_H__
#define __ZZ_MATERIAL_TERRAIN_H__

#ifndef __ZZ_MATERIAL_H__
#include "zz_material.h"
#endif

class zz_renderer;
class zz_shader;

//--------------------------------------------------------------------------------
class zz_material_terrain : public zz_material {
protected:
	// color state
	void set_first ();
	void set_first_second ();
	void set_first_second_light ();
	void set_first_second_shadow ();
	void set_first_second_light_shadow ();
	void set_first_light ();
	void set_first_light_shadow ();
	void set_first_shadow();
	void set_light ();
	void set_light_shadow ();
	void set_shadow ();

	// alpha state
	void set_alpha_stage (int pass, bool use_lightmap, bool use_shadowmap);

	void set_test_blend ();

	void set_blend_rterrain_alpha_stage ();
	void set_first_rough ();

	enum e_material_state {
		MS_NONE = 0,
		MS_FIRST,
		MS_FIRST_SECOND,
		MS_FIRST_SECOND_LIGHT,
		MS_FIRST_SECOND_SHADOW,
		MS_FIRST_SECOND_LIGHT_SHADOW,
		MS_FIRST_LIGHT,
		MS_FIRST_LIGHT_SHADOW,
		MS_FIRST_SHADOW,
		MS_LIGHT,
		MS_LIGHT_SHADOW,
		MS_SHADOW,
		MS_ALPHA,
		MS_ALPHA_FOG,
		MS_ALPHA_SHADOW,
		MS_ALPHA_SHADOW_FOG,
		MS_ALPHA_LIGHT,
		MS_ALPHA_LIGHT_FOG,
		MS_ALPHA_LIGHT_SHADOW,
		MS_ALPHA_LIGHT_SHADOW_FOG,
		MS_GLOW,
		MS_TEST_BLEND
	};

	static e_material_state color_state;
	static e_material_state alpha_state;

public:

	zz_material_terrain (void);
	virtual ~zz_material_terrain (void);
	virtual bool set (int pass);
	
	virtual int get_shader_format () const;

	static void begin ();
	static void end ();

	ZZ_DECLARE_DYNAMIC(zz_material_terrain)
};

class zz_material_terrain_rough : public zz_material_terrain {
public:

	static void begin ();
	static void end ();

	virtual bool set (int pass);

	ZZ_DECLARE_DYNAMIC(zz_material_terrain_rough);
};

#endif __ZZ_MATERIAL_TERRAIN_H__