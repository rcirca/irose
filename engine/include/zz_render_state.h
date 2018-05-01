/** 
 * @file zz_render_state.h
 * @brief render state class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    10-mar-2002
 *
 * $Header: /engine/include/zz_render_state.h 20    04-10-19 1:58p Zho $
 */

#ifndef __ZZ_RENDER_STATE_H__
#define __ZZ_RENDER_STATE_H__

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

//--------------------------------------------------------------------------------
struct zz_render_state
{
	enum zz_render_state_texture_format {
		ZZ_RGB_565,   // 16 bit
		ZZ_RGB_888,   // 24 bit
		ZZ_RGBA_5551, // 16 bit alpha
		ZZ_RGBA_4444, // 16 bit alpha
		ZZ_RGBA_8888  // 32 bit alpha
	};

	enum zz_cull_mode_type {
		ZZ_CULLMODE_NONE = 0,
		ZZ_CULLMODE_CW = 1,
		ZZ_CULLMODE_CCW = 2
	};

	enum zz_texture_filter_type {
		ZZ_TEXF_NONE            = 0,
        ZZ_TEXF_POINT           = 1,
        ZZ_TEXF_LINEAR          = 2,
        ZZ_TEXF_ANISOTROPIC     = 3,
	};

	enum zz_fsaa_type {
		ZZ_FSAA_NONE = 0,
		ZZ_FSAA_TWO = 2,
		ZZ_FSAA_TREE = 3,
		ZZ_FSAA_FOUR = 4
	};

	zz_render_state_texture_format texture_format;
	int screen_width, screen_height, screen_depth;
	int buffer_width, buffer_height, buffer_depth;
	int vertex_shader_version;
	int pixel_shader_version;
	int num_adapters; // number of display devices
	int adapter; // current selected device, default is -1(means not set)
	int refresh_rate; // preffered monitor refresh rate. default is 0(means not set)
	bool use_fullscreen;
	bool use_hw_vertex_processing_support;
	bool use_flat_shading;
	bool use_texture;
	bool use_HAL;
	bool use_pixel_shader;
	bool use_vertex_shader;
	bool use_shadowmap;
	bool use_bumpmap;
	bool use_wire_mode;
	bool use_light;
	bool use_cull;
	bool use_draw_bounding_volume;
	bool use_alpha; // whether use alpha channel or not
	bool use_alpha_test;
	bool use_draw_viewfrustum;
	bool use_refresh_cull; // whether refresh cull list or not
	bool use_object_sorting; // for translucent objects
	bool use_polygon_sorting; // for translucent objcet polygon
	bool use_render_state; // if enabled, 
	bool use_lightmap;
	bool use_vsync; // vertical sync on or off. default is on
	bool use_debug_display; // whether display debug message on screen or not. default is false
	bool use_dynamic_textures; // whether support dynamic textures or not
	bool use_glow; // object glow
	bool use_glow_fullscene; // fullscene glow
	bool use_glow_ztest; // glow ztest.
	bool use_zonly_terrain; // whether render z-only blended terrain or not.
	bool use_draw_text_rect; // whether draw text rect for debugging
	bool use_font_texture; // whether use font_texture or not. default is true

	int fullscene_glow_type; // 0 : (one-one), 1 : destcorol-one

	// use fixed function mode if possible
	// cannot be set by script
	// set by renderer::initialize()
	bool use_fixed_mode_if_possible; 
	bool use_round_shadow;
	int shadowmap_blur_type; // 0, 1, 2, 4 tap filter. default 0(none)
	vec3 shadowmap_color; // multiplied by inverse into shadowmap

	// If the hardware does not support more than 4-simultaneous textures,
	// it is set to true.
	bool use_multipass;	
	int current_pass; // current rendering pass. 0(1st pass), 1(2nd pass) set by scene class

	zz_fsaa_type fsaa_type;

	vec3 clear_color;
	zz_cull_mode_type cull_mode;
	int shadowmap_size;
	int glowmap_size; // default is 128
	vec3 glow_color; // object glow color scale
	vec3 glow_color_fullscene; // fullscene glow color scale

	zz_texture_filter_type mipmap_filter;
	zz_texture_filter_type min_filter;
	zz_texture_filter_type mag_filter;
	int mipmap_level;
	int time_delay; 

	// fog related
	vec3 fog_color;
	float fog_start;
	float fog_end;
	float alpha_fog_start;
	float alpha_fog_end;
	bool use_fog;
	bool use_alpha_fog;

	// depth bias, not used anywhere
	int depth_bias;

	bool use_gamma; // whether we use gamma control
	bool use_game_gamma; // whether we use game gamma

	int lightmap_blend_style; // ZZ_TEXTUREOP

	float camera_transparency;

	float blend_factor; // for lightmap blending factor
	bool draw_shadowmap_viewport;

	int max_simultaneous_textures;

	int texture_loading_scale; // value to shift-right with original texture size. default is 0
	int texture_loading_format; // 0(unknown-original, default), 1(16bit), 2(compressed)
	int max_simultaneous_bone; // 1, 2, 4(default)
	int display_quality_level; // 0(auto), 1(highest,default), 2, 3, 4, 5(lowest)
	int min_framerate, max_framerate; // min/max framerate (integer > 0)

	bool use_file_time_stamp; // whether use file time stamp comparing or not, default is no

	// If set, use black shadowmap and subtract shadowmap from terrain.
	// If not set, use white shadowmap and modulate shadowmap with terrain.
	// This is determined by device caps MISCBLENDOP supporting.
	// Non-blackshadow is preferred, because it is more natural.
	// The blackshadow can be weird in some terrain textures.
	// This is not controlled by script.
	bool use_blackshadowmap;

	bool use_delayed_loading;

	bool use_terrain_lod; // terrain mesh index lod

	// lazy buffer size
	int lazy_texture_size, lazy_mesh_size, lazy_terrain_mesh_size, lazy_ocean_mesh_size;

	zz_render_state(void);
	virtual ~zz_render_state(void);

	void set_display_quality_level (int level);
};

#endif // __ZZ_RENDER_STATE_H__