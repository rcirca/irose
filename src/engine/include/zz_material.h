/** 
 * @file zz_material.h
 * @brief material class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    17-may-2002
 *
 * $Header: /engine/include/zz_material.h 19    05-05-12 10:17a Choo0219 $
 */

#ifndef __ZZ_MATERIAL_H__
#define __ZZ_MATERIAL_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_RENDER_STATE_H__
#include "zz_render_state.h"
#endif

#ifndef __ZZ_LOD_H__
#include "zz_lod.h"
#endif

// for material load/unload count test
//#define ZZ_MATERIAL_LOADTEST

#include <string>
#include <vector>
#include <map>

#define ZZ_TEXNUM_COLORMAP 0
#define ZZ_TEXNUM_NORMALMAP 1
#define ZZ_TEXNUM_NORMALIZATIONCUBEMAP 2
#define ZZ_TEXNUM_SHADOWMAP 3
//#define ZZ_TEXNUM_LIGHTMAP ?

#define ZZ_MAX_TEXTURESTAGE 4

class zz_shader;
class zz_texture;
class zz_system;
class zz_renderer;

//--------------------------------------------------------------------------------
struct zz_material_state {
//--------------------------------------------------------------------------------
	zz_render_state::zz_cull_mode_type cullmode; // triangle cull model

	bool fog; // whether use fog or not
	bool alpha_blend; // whether use alpha blending
	bool alpha_test; // if true, do alpha test
	bool zwrite; // if true, we write z-value into the depth buffer
	bool ztest; // if true, we write z-value only if greather than the backbuffer z-value
	int alpha_ref; // only alpha values which are greater than the alpha_ref will be written to the backbuffer
	int blend_type; // alpha blend type. defined with ZZ_BLEND_TYPE in zz_renderer.h
	color32 blend_factor; // alpha blend factor
	zz_glow_type glow_type; // glow type. default is ZZ_GLOW_NONE
	color32 glow_color; // default 0xffffffff
	color32 tfactor; // texture factor
	float alphavalue; // objectalpha * materialalpha

	// alpha blend property (only if the blendtype is ZZ_BT_CUSTOM)
	// affected only if texturealpha is turn on
	int blend_src; // source blend type
	int blend_dest; // destination blend type
	int blend_op; // blend operation

	int texture_address[ZZ_MAX_TEXTURESTAGE]; // ZZ_TEXTUREADDRESS in zz_renderer.h. default = ZZ_TADDRESS_WRAP
	bool texture_binding[ZZ_MAX_TEXTURESTAGE]; // whether bind texture or not

	void init ();
	void copy_to (zz_material_state& dest);
	void copy_from (const zz_material_state& src);
};

//--------------------------------------------------------------------------------
class zz_material : public zz_node {
//--------------------------------------------------------------------------------
private:
	friend class zz_system;

	bool _set_texture (int index, zz_texture * tex); // non-reference increased
	bool _push_texture (zz_texture * tex); // non-reference increased

protected:
	std::vector<zz_texture*> textures; // texture slot
	zz_shader * shader;
	zz_lod lod;
	bool use_specular; // whether use specular env map
	bool use_light; // if false, we do not use diffuse color. default value is true
	zz_glow_type glow_type; // default is ZZ_GLOW_NONE
	vec3 glow_color; // glow is disabled if glow_color is zero. default is vec3_one

	// cast_shadow, receive_shadow, receive_fog are not persistent property. These are only current property.
	bool cast_shadow; // if this material cast shadow
	bool receive_shadow; // if this material receives shadow
	bool receive_fog; // if this material is affected by fog

	bool texturealpha; // true if diffuse texture has alpha channel
	float objectalpha; // if objectalpha is less than 1.0f, it is modulated by texture diffuse alpha
	float materialalpha; // material default alpha. default is 1.0f

	zz_material_state state; // material state

	// static properties
	static zz_material_state s_state; // all materials share this state
	static zz_renderer * s_renderer; // set by system
	static zz_render_state * s_renderstate; // set by system

	void set_cullmode (zz_render_state::zz_cull_mode_type mode);
	zz_render_state::zz_cull_mode_type get_cullmode ();

public:
	zz_material(int default_texture_size = 0); // not to allocate surplus texture space
	virtual ~zz_material(void);

	static void apply_shared_property (int num_stage);
	virtual bool set (int pass = 0) = 0; // apply material property

	//--------------------------------------------------------------------------------
	// texture set/get
	//--------------------------------------------------------------------------------
	bool set_texture (int index, const char * filename);
	bool set_texture (int index, int width, int height);
	bool set_texture (int index, zz_texture * tex); // reference increased
	bool push_texture (zz_texture * tex); // reference increased
	bool push_texture (const char * filename);
	int get_num_textures () const;
	void reset_texture(void);
	zz_texture * get_texture (int index = 0) const;
	//--------------------------------------------------------------------------------

	void set_blend_factor (color32 blend_factor_in);
	color32 get_blend_factor () const;

	void set_shader (zz_shader * shader_in);
	const zz_shader * get_shader (void) const;

	void set_receive_fog (bool receive_fog_in);
	bool get_receive_fog (void) const;

	void set_cast_shadow (bool cast_shadow_in);
	bool get_cast_shadow (void) const;
	
	void set_receive_shadow (bool receive_shadow_in);
	bool get_receive_shadow (void) const;

	void set_use_specular (bool use_specular_or_not);
	bool get_use_specular () const;

	void set_texture_address (int stage_in, int texture_address_in);
	int get_texture_address (int stage_in) const;

	void set_use_light (bool use_light_or_not);
	bool get_use_light () const;

	int get_blendtype (void) const;
	void set_blendtype (int type_in);
	void set_blendtype (int blend_src_in, int blend_dest_in, int blend_op_in);

	void set_ztest (bool val);
	bool get_ztest (void) const;

	void set_zwrite (bool val);
	bool get_zwrite (void) const;

	void set_objectalpha (float alpha);
	float get_objectalpha (void) const;

	bool get_texturealpha () const;
	void set_texturealpha (bool true_if_texturealpha);

	void set_materialalpha (float alpha_in)
	{
		materialalpha = alpha_in;
		state.alphavalue = materialalpha * objectalpha;
	}
	float get_materialalpha () const
	{
		return materialalpha;
	}

	bool get_twoside () const;
	void set_twoside (bool true_or_false);

	bool get_alpha_test () const;
	void set_alpha_test (bool true_or_false);

	int get_alpha_ref () const;
	void set_alpha_ref (int new_val);

	// check if all textures were buffered in hardware.
	bool get_device_updated () const;

	// force all textures to be buffered in hardware.
	// if *immediate* is false, we request the manager to flush this for next the time or soon as soon as possible.
	// returns if an error occured.
	bool flush_device (bool immediate);

	// set/get LOD level
	const zz_lod& get_lod () const;
	void set_lod ( const zz_lod& lod_in );

	virtual bool is_valid_pass (int pass) const // only 0 pass is valid for default
	{
		return (pass == 0);
	}

	// get shader format for this material
	virtual int get_shader_format () const
	{
		return 0;
	}

	void set_glow_color (const vec3& color_in)
	{
		glow_color = color_in;
		if (color_in == vec3_null) {
			state.glow_color = 0;
		}
		else {
			state.glow_color = zz_color(color_in.x, color_in.y, color_in.z, 1.0f);
		}
	}

	const vec3& get_glow_color () const
	{
		return glow_color;
	}

	void set_glow_type (zz_glow_type gtype_in)
	{
		glow_type = gtype_in;
		state.glow_type = gtype_in;
	}

	zz_glow_type get_glow_type () const
	{
		return glow_type;
	}

	void set_object_glow_type (zz_glow_type gtype_in)
	{
		state.glow_type = gtype_in;
	}

	void set_object_glow_color (const vec3& color_in)
	{
		if (color_in == vec3_null) {
			state.glow_color = 0;
		}
		else {
			state.glow_color = zz_color(color_in.x, color_in.y, color_in.z, 1.0f);
		}
	}

	void set_load_priroty ( int priority_in );

	ZZ_DECLARE_DYNAMIC(zz_material)
};

inline const zz_lod& zz_material::get_lod () const
{
	return lod;
}

inline const zz_shader * zz_material::get_shader (void) const
{
	return shader;
}

inline int zz_material::get_blendtype (void) const
{
	return state.blend_type;
}

inline void zz_material::set_blendtype (int type_in)
{
	state.blend_type = type_in;
}

inline void zz_material::set_ztest (bool val)
{
	state.ztest = val;
}

inline bool zz_material::get_ztest (void) const
{
	return state.ztest;
}

inline void zz_material::set_zwrite (bool val)
{
	state.zwrite = val;
}

inline bool zz_material::get_zwrite (void) const
{
	return state.zwrite;
}

inline void zz_material::set_objectalpha (float alpha)
{
	objectalpha = (alpha > 1.0f) ? 1.0f : ((alpha < 0) ? 0 : alpha);
	state.alphavalue = materialalpha * objectalpha;
}

inline float zz_material::get_objectalpha (void) const
{
	return objectalpha;
}

inline bool zz_material::get_texturealpha () const
{
	return texturealpha;
}

inline void zz_material::set_texturealpha (bool true_if_texturealpha)
{
	state.alpha_blend = texturealpha = true_if_texturealpha;
}

inline bool zz_material::get_twoside () const
{
	return (state.cullmode == zz_render_state::ZZ_CULLMODE_NONE);
}

inline void zz_material::set_twoside (bool true_or_false)
{
	state.cullmode = (true_or_false) ? zz_render_state::ZZ_CULLMODE_NONE : zz_render_state::ZZ_CULLMODE_CW;
}

inline bool zz_material::get_alpha_test () const
{
	return state.alpha_test;
}

inline void zz_material::set_alpha_test (bool true_or_false)
{
	state.alpha_test = true_or_false;
}

inline int zz_material::get_alpha_ref () const
{
	return state.alpha_ref;
}

inline void zz_material::set_alpha_ref (int new_val)
{
	state.alpha_ref = new_val;
}

inline int zz_material::get_num_textures () const
{
	return (int)textures.size();
}

inline zz_texture * zz_material::get_texture (int index) const
{
	if (index >= (int)textures.size()) return NULL;

	return textures[index];
}

inline void zz_material::set_receive_fog (bool receive_fog_in)
{
	receive_fog = receive_fog_in;
}

inline bool zz_material::get_receive_fog (void) const
{
	return receive_fog;
}

inline void zz_material::set_cast_shadow (bool cast_shadow_in)
{
	cast_shadow = cast_shadow_in;
}

inline bool zz_material::get_cast_shadow (void) const
{
	return cast_shadow;
}

inline void zz_material::set_receive_shadow (bool receive_shadow_in)
{
	receive_shadow = receive_shadow_in;
}

inline bool zz_material::get_receive_shadow (void) const
{
	return receive_shadow;
}

inline void zz_material::set_use_specular (bool use_specular_or_not)
{
	use_specular = use_specular_or_not;
}

inline bool zz_material::get_use_specular () const
{
	return use_specular;
}

inline void zz_material::set_texture_address (int stage_in, int texture_address_in)
{
	assert(stage_in < ZZ_MAX_TEXTURESTAGE);
	state.texture_address[stage_in] = texture_address_in;
}

inline int zz_material::get_texture_address (int stage_in) const
{
	assert(stage_in < ZZ_MAX_TEXTURESTAGE);
	return state.texture_address[stage_in];
}

inline void zz_material::set_use_light (bool use_light_or_not)
{
	use_light = use_light_or_not;
}

inline bool zz_material::get_use_light () const
{
	return use_light;
}

inline void zz_material::set_cullmode (zz_render_state::zz_cull_mode_type mode)
{
	state.cullmode = mode;
}

inline zz_render_state::zz_cull_mode_type zz_material::get_cullmode ()
{
	return state.cullmode;
}

inline void zz_material::set_blend_factor (color32 blend_factor_in)
{
	state.blend_factor = blend_factor_in;
}

inline color32 zz_material::get_blend_factor () const
{
	return state.blend_factor;
}

#endif // __ZZ_MATERIAL_H__