/** 
 * @file zz_terrain_block.cpp
 * @brief terrain block class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-mar-2003
 *
 * $Header: /engine/src/zz_terrain_block.cpp 21    04-09-05 6:51p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_terrain_block.h"
#include "zz_render_state.h"
#include "zz_mesh.h"
#include "zz_material.h"
#include "zz_light.h"
#include "zz_shader.h"
#include "zz_renderer.h"
#include "zz_system.h"
#include "zz_material_terrain.h"
#include "zz_mesh_terrain.h"

ZZ_IMPLEMENT_DYNCREATE(zz_terrain_block, zz_visible)
ZZ_IMPLEMENT_DYNCREATE(zz_terrain_block_rough, zz_terrain_block)

zz_terrain_block::zz_terrain_block() :
	terrain_mesh(0)
{
	cast_shadow = false;
	receive_shadow = true;
	receive_fog = true;
	collision_level = ZZ_CL_POLYGON;
	draw_priority = ZZ_DP_HIGHEST;
	bv_type = ZZ_BV_OBB;
	runits.reserve(1);
}

zz_terrain_block::~zz_terrain_block ()
{
}

bool zz_terrain_block::first_render(true);

bool saved_use_vertex_shader;

void zz_terrain_block::begin_state (bool blended)
{	
	zz_render_state * state = znzin->get_rs();
	
	if (state->use_multipass) {
		int total_pass = 2; // hard coded
		int num_pass = 2; // hard-coded for terrain
		int current_pass = state->current_pass;

		if (current_pass >= num_pass)
			return;

		// not to render second pass light&shadow map, if not set
		if (num_pass > 1 && current_pass != 0) {
			if (!state->use_lightmap && !state->use_shadowmap) {
				return;
			}
		}
	}

	first_render = true;
	
	zz_material_terrain::begin();

	if (blended) {
		znzin->renderer->enable_zwrite(false);
	}
}

void zz_terrain_block::end_state ()
{
	// restore use_vertex_shader
	zz_material_terrain::end();

	znzin->renderer->enable_zwrite(true); // revert to original
}

bool zz_terrain_block::begin_state_zonly ()
{	
	zz_render_state * state = znzin->get_rs();
	
	first_render = true;
	
	zz_renderer * r = znzin->renderer;

	if (!r->support_color_write()) return false;

	r->set_zfunc(ZZ_CMP_LESSEQUAL);
	r->enable_alpha_blend(false, ZZ_BT_NONE);
	r->enable_alpha_test(false);
	r->enable_zbuffer(true);
	r->enable_zwrite(true);
	r->enable_color_write( 0 ); // not to write color
	r->set_texture(ZZ_HANDLE_NULL, 0, NULL);

	zz_shader * shader = zz_shader::terrain_shader;

	assert(r->get_render_where() == ZZ_RW_SCENE);

	zz_handle vshader_index = shader->get_vshader(SHADER_FORMAT_FOG, ZZ_RW_SCENE);
	zz_handle pshader_index = shader->get_pshader(SHADER_FORMAT_FOG, ZZ_RW_SCENE);

	assert(vshader_index != ZZ_HANDLE_NULL);

	r->set_vertex_shader(vshader_index);
	r->set_pixel_shader(pshader_index);

	return true;
}

void zz_terrain_block::end_state_zonly ()
{
	zz_renderer * r = znzin->renderer;
	if (r->support_color_write()) {
        r->enable_color_write(
			ZZ_COLORWRITEENABLE_RED |
			ZZ_COLORWRITEENABLE_GREEN |
			ZZ_COLORWRITEENABLE_BLUE |
			ZZ_COLORWRITEENABLE_ALPHA);
	}
}

void zz_terrain_block::render_zonly ()
{
	zz_runit& ru = runits[0];
	zz_mesh * mesh = ru.mesh;
	assert(mesh);

	zz_renderer * r = znzin->renderer;

	// set modelviewTM from modelview_worldTM
	if (first_render) {
		mat4 modelview_worldTM;
		get_modelview_worldTM(modelview_worldTM);
		r->set_modelview_matrix(modelview_worldTM);
		r->set_world_matrix(get_worldTM());
		r->set_light(ru.light);
		first_render = false;
	}
	
	zz_assert(mesh->get_device_updated());

	r->render(mesh, NULL, NULL);
}

void zz_terrain_block::render (bool recursive)
{	
	assert(runits.size() > 0);

	zz_runit& ru = runits[0];
	zz_mesh * mesh = ru.mesh;
	zz_material * mat = ru.material;
	const zz_shader * shader = (mat) ? mat->get_shader() : NULL;

	assert(mesh && mat && shader);

	mat->set_receive_fog(receive_fog_now);
	mat->set_receive_shadow(receive_shadow_now);

	zz_renderer * r = znzin->renderer;

	// set modelviewTM from modelview_worldTM
	if (first_render) {
		mat4 modelview_worldTM;
		get_modelview_worldTM(modelview_worldTM);
		r->set_modelview_matrix(modelview_worldTM);
		r->set_world_matrix(get_worldTM());
		r->set_light(ru.light);
		first_render = false;
	}

	zz_assert(mat->get_device_updated());
	zz_assert(mesh->get_device_updated());

	r->render(mesh, mat, NULL); // light already set in first_render 
}


const vec3& zz_terrain_block::get_com_position_world ()
{
	if (tm_cache_flag & TRANSFORM_CACHE_INDEX_COM_POSITION_WORLD) {
		return com_position_world;
	}
	
	tm_cache_flag |= TRANSFORM_CACHE_INDEX_COM_POSITION_WORLD;
	com_position_world = get_bvolume()->get_sphere()->center;
	return com_position_world;
}

void zz_terrain_block::update_bvolume (void)
{
	assert(valid_bvolume);
}

void zz_terrain_block::reset_bvolume (void)
{
	assert(bv_type == ZZ_BV_OBB);
	assert(!valid_bvolume);
	assert(terrain_mesh);
	
	minmax[0] = terrain_mesh->get_min();
	minmax[1] = terrain_mesh->get_max();

	assert(!bv);
	bv = zz_new zz_bvolume();
	bv->set_obb(.5f*(minmax[0] + minmax[1]), .5f*(minmax[1] - minmax[0]));

	valid_bvolume = true;

	// refresh in scene
	scene_refresh();

	assert(children.size() == 0);
}

void zz_terrain_block::add_runit (zz_mesh_terrain * mesh, zz_material * material, zz_light * light)
{
	terrain_mesh = mesh;
	zz_visible::add_runit(static_cast<zz_mesh*>(mesh), material, light);
}

void zz_terrain_block::update_time (bool recursive, zz_time diff_time)
{
	// invalidate infrustum
	infrustum = false;

	assert(valid_bvolume);
	assert(get_inscene());
}

// no need to update transform
// actually insert into scene immediately
void zz_terrain_block::insert_scene ()
{
	inscene = true;
	scene_refresh();
}

// no need to update transform
// actually remove from scene immediately
void zz_terrain_block::remove_scene ()
{
	inscene = false;
	scene_refresh();
}

void zz_terrain_block_rough::begin_state (bool blend)
{
	first_render = true;
	
	zz_material_terrain_rough::begin();
}

void zz_terrain_block_rough::end_state ()
{
	zz_material_terrain_rough::end();
}

zz_terrain_block_rough::zz_terrain_block_rough ()
{
	collision_level = ZZ_CL_NONE;
}

// same as ocean_block
void zz_terrain_block::before_render ()
{
	zz_runit& ru = runits[0];
	zz_mesh * mesh = ru.mesh;
	zz_material * mat = ru.material;

	zz_renderer * r = znzin->renderer;

	apply_lod(mesh, mat); // apply_lod must precede flush_device()

	mat->flush_device(true /* immediate */);

	if (!mesh->get_device_updated()) {
		mesh->flush_device(true /* immediate */);
	}
}

void zz_terrain_block::after_render ()
{
}
