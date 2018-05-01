/** 
 * @file zz_render_state.h
 * @brief render_state class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    10-mar-2002
 *
 * $Header: /engine/src/zz_render_state.cpp 56    04-10-19 1:58p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_render_state.h"
#include "zz_renderer.h"
#include "zz_system.h"

#define ZZ_RS_SHADOWMAP_SIZE 512
#define ZZ_DEFAULT_SCREEN_WIDTH  320
#define ZZ_DEFAULT_SCREEN_HEIGHT 240
#define ZZ_DEFAULT_SCREEN_DEPTH   32
#define ZZ_DEFAULT_BUFFER_WIDTH  320
#define ZZ_DEFAULT_BUFFER_HEIGHT 240
#define ZZ_DEFAULT_BUFFER_DEPTH   32

zz_render_state::zz_render_state(void) :
	texture_format(zz_render_state::ZZ_RGBA_8888),
	screen_width(ZZ_DEFAULT_SCREEN_WIDTH),
	screen_height(ZZ_DEFAULT_SCREEN_HEIGHT),
	screen_depth(ZZ_DEFAULT_SCREEN_DEPTH),
	buffer_width(ZZ_DEFAULT_BUFFER_WIDTH),
	buffer_height(ZZ_DEFAULT_BUFFER_HEIGHT),
	buffer_depth(ZZ_DEFAULT_BUFFER_DEPTH),
	use_fullscreen(false),
	use_hw_vertex_processing_support(true),
	use_flat_shading(false),
	use_texture(true),
	use_HAL(true),
	use_vertex_shader(true),
	use_pixel_shader(false),
	use_render_state(true),
	cull_mode(zz_render_state::ZZ_CULLMODE_CW),
	shadowmap_size(ZZ_RS_SHADOWMAP_SIZE),
	use_wire_mode(false),
	clear_color(0, 0, 0),
	time_delay(0),
	fsaa_type(zz_render_state::ZZ_FSAA_NONE),
	use_light(true),
	use_fog(false),
	use_cull(true),
	use_draw_bounding_volume(false),
	use_alpha(false),
	use_alpha_test(true),
	use_draw_viewfrustum(false),
	use_refresh_cull(true),
	use_gamma(false), // for now, not work correctly, 2004/04/23
	use_game_gamma(false), // for now, not work correctly, 2004/04/23
	use_debug_display(false),
	mipmap_filter(ZZ_TEXF_LINEAR),
	min_filter(ZZ_TEXF_LINEAR),
	mag_filter(ZZ_TEXF_LINEAR),
	use_object_sorting(false),
	use_polygon_sorting(true),
	vertex_shader_version(0),
	pixel_shader_version(0),
	fog_color(1, 0, 0),
	fog_start(150.0f),
	fog_end(400.0f),
	alpha_fog_start(100.0f),
	alpha_fog_end(200.0f),
	use_multipass(false),
	use_lightmap(true),
	use_shadowmap(true),
	use_round_shadow(false),
	use_dynamic_textures(true),
	lightmap_blend_style(ZZ_TOP_MODULATE2X),
	camera_transparency(.3f),
	shadowmap_blur_type(0),
	use_fixed_mode_if_possible(false),
	draw_shadowmap_viewport(false),
	current_pass(0),
	shadowmap_color(1.0f, 1.0f, 1.0f),
	texture_loading_scale(0),
	use_blackshadowmap(false),
	texture_loading_format(0),
	max_simultaneous_bone(4),
	display_quality_level(0),
	min_framerate(15),
	max_framerate(60),
	use_file_time_stamp(false),
	use_vsync(true),
	use_delayed_loading(false),
	lazy_texture_size(0),
	lazy_mesh_size(0),
	lazy_terrain_mesh_size(0),
	lazy_ocean_mesh_size(0),
	num_adapters(1),
	adapter(-1),
	refresh_rate(0),
	max_simultaneous_textures(1),
	use_glow(false),
	use_glow_ztest(false),
	glowmap_size(128),
	glow_color(.01f, .01f, .01f),
	glow_color_fullscene(.016f, 0.016f, 0.016f),
	use_glow_fullscene(false),
	use_alpha_fog(true),
	use_terrain_lod(true),
	use_zonly_terrain(true),
	fullscene_glow_type(0),
	use_draw_text_rect(false),
	use_font_texture(true)
{
}

zz_render_state::~zz_render_state(void)
{
}

void zz_render_state::set_display_quality_level (int level)
{
	display_quality_level = level;

	static zz_render_state initial_state;
	static bool initial_state_saved = false;

	if (!initial_state_saved) {
		initial_state_saved = true;
		initial_state = *this;
	}

	zz_renderer * r = znzin->renderer;

	// before changing, delete renderer device
	if (r->is_active()) {
		r->invalidate_device_objects();
		r->delete_device_objects();
	}

	switch (level) {
		case 5: // 최하급품질
			this->texture_loading_scale = 2;
			this->texture_loading_format = 2;
			this->use_shadowmap = false;
			this->use_lightmap = false;
			this->shadowmap_size = 32;
			this->glowmap_size = 32;
			this->mipmap_level = -1;
			this->mipmap_filter = zz_render_state::zz_texture_filter_type(2);
			this->min_filter = zz_render_state::zz_texture_filter_type(2);
			this->mag_filter = zz_render_state::zz_texture_filter_type(2);
			this->use_glow = false;
			this->use_glow_fullscene = false;
			this->use_zonly_terrain = false;
			this->use_terrain_lod = false;
			this->use_glow_ztest = false;
			this->use_alpha_fog = false;
			znzin->set_use_time_weight(0);
			znzin->set_use_motion_interpolation(false);
			znzin->terrain_blocks->set_num_reuse(500);
			break;
		case 4: // 하급품질
			this->texture_loading_scale = 1;
			this->texture_loading_format = 2;
			this->use_shadowmap = false;
			this->use_lightmap = true;
			this->shadowmap_size = 64;
			this->glowmap_size = 64;
			this->mipmap_level = -1;
			this->mipmap_filter = zz_render_state::zz_texture_filter_type(2);
			this->min_filter = zz_render_state::zz_texture_filter_type(2);
			this->mag_filter = zz_render_state::zz_texture_filter_type(2);
			this->use_glow = false;
			this->use_glow_fullscene = false;
			this->use_zonly_terrain = false;
			this->use_terrain_lod = false;
			this->use_glow_ztest = false;
			this->use_alpha_fog = false;
			znzin->set_use_time_weight(1);
			znzin->set_use_motion_interpolation(false);
			znzin->terrain_blocks->set_num_reuse(500);
			break;
		case 3: // 중급 품질
			this->texture_loading_scale = 1;
			this->texture_loading_format = 0;
			this->use_shadowmap = true;
			this->use_lightmap = true;
			this->shadowmap_size = 128;
			this->glowmap_size = 128;
			this->mipmap_level = -1;
			this->mipmap_filter = zz_render_state::zz_texture_filter_type(2);
			this->min_filter = zz_render_state::zz_texture_filter_type(2);
			this->mag_filter = zz_render_state::zz_texture_filter_type(2);
			this->use_glow = true;
			this->use_glow_fullscene = false;
			this->use_zonly_terrain = false;
			this->use_terrain_lod = false;
			this->use_glow_ztest = false;
			this->use_alpha_fog = true;
			znzin->set_use_time_weight(1);
			znzin->set_use_motion_interpolation(true);
			znzin->terrain_blocks->set_num_reuse(500);
			break;
		case 2: // 상급 품질
			this->texture_loading_scale = 0;
			this->texture_loading_format = 0;
			this->use_shadowmap = true;
			this->use_lightmap = true;
			this->shadowmap_size = 256;
			this->glowmap_size = 256;
			this->mipmap_level = 3;
			this->mipmap_filter = zz_render_state::zz_texture_filter_type(2);
			this->min_filter = initial_state.min_filter;
			this->mag_filter = initial_state.mag_filter;
			this->use_glow = true;
			this->use_glow_fullscene = false;
			this->use_zonly_terrain = true;
			this->use_terrain_lod = false;
			this->use_glow_ztest = false;
			this->use_alpha_fog = true;
			znzin->set_use_time_weight(1);
			znzin->set_use_motion_interpolation(true);
			znzin->terrain_blocks->set_num_reuse(500);
			break;
		case 1: // 최상급 품질(설정파일대로)
		case 0:
			this->texture_loading_scale = initial_state.texture_loading_scale;
			this->texture_loading_format = initial_state.texture_loading_format;
			this->use_shadowmap = initial_state.use_shadowmap;
			this->use_lightmap = initial_state.use_lightmap;
			this->shadowmap_size = initial_state.shadowmap_size;
			this->glowmap_size = initial_state.glowmap_size;
			this->mipmap_level = initial_state.mipmap_level;
			this->mipmap_filter = initial_state.mipmap_filter;
			this->min_filter = initial_state.min_filter;
			this->mag_filter = initial_state.mag_filter;
			this->use_glow = initial_state.use_glow;
			this->use_glow_fullscene = initial_state.use_glow_fullscene;
			this->use_zonly_terrain = initial_state.use_zonly_terrain;
			this->use_terrain_lod = initial_state.use_terrain_lod;
			this->use_glow_ztest = false; // zhotest
			this->use_alpha_fog = initial_state.use_alpha_fog;
			znzin->set_use_time_weight(1);
			znzin->set_use_motion_interpolation(true);
			znzin->terrain_blocks->set_num_reuse(1000);
			break;
	}

	// after changing, restore rederer device
	if (r->is_active()) {
		r->init_device_objects();
		r->restore_device_objects();

		// invalidate all textures 
		znzin->textures->unbind_object_textures();
	}
}