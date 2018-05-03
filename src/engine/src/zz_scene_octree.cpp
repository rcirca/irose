/** 
 * @file zz_scene_octree.cpp
 * @brief scene graph simple version.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    23-feb-2002
 *
 * $Header: /engine/src/zz_scene_octree.cpp 109   06-12-29 4:23a Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font.h"
#include "zz_scene_octree.h"
#include "zz_system.h"
#include "zz_bone.h"
#include "zz_camera_follow.h"
#include "zz_profiler.h"
#include "zz_bvolume.h"
#include "zz_primitive.h"
#include "zz_model.h"
#include "zz_mesh_tool.h"
#include "zz_sky.h"
#include "zz_terrain_block.h"
#include "zz_trail.h"
#include "zz_ocean_block.h"
#include "zz_morpher.h"
#include "zz_particle_emitter.h"
#include "zz_manager.h"
#include "zz_manager_font.h"
#include "zz_renderer.h"
#include "zz_material.h"
#include "zz_texture.h"
#include "zz_sfx.h"
#include "zz_path.h"
#include "zz_primitive.h"
#include "zz_bvolume.h"

#include <vector>
#include <algorithm>

using namespace std;

ZZ_IMPLEMENT_DYNCREATE(zz_scene_octree, zz_scene)

bool s_dump_view = false;
bool s_dump_view_flags[4] = { false, false, false, false };

//#ifdef _DEBUG
#define DRAW_BBOX
//#endif

#if (0)
#define PROFILER_RENDER_SUB(PROFILERNAME, MESSAGEFORMAT, NUM_SIZE) \
	ZZ_PROFILER_INSTALL(PROFILERNAME); \
	ZZ_LOG(MESSAGEFORMAT, NUM_SIZE);
#else
#define PROFILER_RENDER_SUB(PROFILERNAME, MESSAGEFORMAT, NUM_SIZE)
#endif

#if (0)
#define PROFILE_UPDATE_BEGIN(P) ZZ_PROFILER_BEGIN(P)
#define PROFILE_UPDATE_END(P) ZZ_PROFILER_END(P)
#else
#define PROFILE_UPDATE_BEGIN(P)
#define PROFILE_UPDATE_END(P)
#endif

zz_scene_octree::zz_scene_octree ()
{
	scene_sphere_number = 0;
    scene_obb_number = 0;
    scene_aabb_number = 0;
    scene_cylinder_number = 0;
    scene_line_number = 0;
	scene_obb2_number = 0;
    scene_axis_number = 0;
}

zz_scene_octree::~zz_scene_octree ()
{
}


//--------------------------------------------------------------------------------
//   Compute the planes defining the view frustum,
//  and pass them to the scene database for gross
//  culling. After the culling has completed, save
//  the number of polygons and render the scene.
//  from ATI's terrain sample. RenderLoop.cpp
//--------------------------------------------------------------------------------
void zz_scene_octree::cull () {
	if (!znzin->get_rs()->use_refresh_cull)
	{
		validity = true;
		return;
	}

	// if current list already be built, just skip this call
	if (validity == true) return;
	validity = true;

	zz_camera * cam = znzin->get_camera();
	if (!cam) return;

	const zz_viewfrustum& frustum = cam->update_frustum(200.0f*ZZ_SCALE_IN); // apply heuristic margin

	octree.cull_planes(frustum);
    
  
}

// render all visibles in the scene
void zz_scene_octree::render_all ()
{
	zz_visible * visible;
	zz_hash_table<zz_node*>::iterator it;
	zz_hash_table<zz_node*> * nodes = znzin->visibles->get_hash_table();

	for (it = nodes->begin(); it != nodes->end(); it++) {
		visible = (zz_visible *)(*it);
		if (visible->is_visible()) {
			visible->render();
		}
	}
}

bool compare_farther_than (zz_visible * a, zz_visible * b)
{
	int pa(a->get_draw_priority());
	int pb(b->get_draw_priority());

	if (pa != pb) return (pa < pb);
	
	return (a->get_camdist_square() > b->get_camdist_square());
}


bool compare_nearer_than (zz_visible * a, zz_visible * b)
{
	int pa(a->get_draw_priority());
	int pb(b->get_draw_priority());

	if (pa != pb) return (pa < pb);
	
	// a and b has the same draw_priority value
	return (a->get_camdist_square() < b->get_camdist_square());
}

// compare first material and return the bigger
bool compare_by_texture (zz_visible * a, zz_visible * b)
{
	assert(a);
	assert(b);
	assert(a->get_material(0));
	assert(b->get_material(0));

	zz_material * mat_a = a->get_material(0);
	zz_material * mat_b = b->get_material(0);

	return mat_a->get_texture(0) > mat_b->get_texture(0);
}

// compare all available textures
// it maybe be slower than compare_by_texture()
bool compare_by_texture_all (zz_visible * a, zz_visible * b)
{
	assert(a);
	assert(b);
	assert(a->get_material(0));
	assert(b->get_material(0));

	zz_material * mat_a = a->get_material(0);
	zz_material * mat_b = b->get_material(0);
	int num_tex_a = mat_a->get_num_textures();
	int num_tex_b = mat_b->get_num_textures();
	int num_tex = ZZ_MAX(num_tex_a, num_tex_b);
	if (num_tex <= 0) return true; // no texture, then skip comparision

	zz_handle h_a, h_b;
	zz_texture * tex_a, * tex_b;
	for (int i = num_tex; i >= 1; i--) {
		if (num_tex_a < i) return false;
		if (num_tex_b < i) return true;
		tex_a = mat_a->get_texture(i-1);
		tex_b = mat_b->get_texture(i-1);
		h_a = tex_a ? tex_a->get_texture_handle() : NULL;
		h_b = tex_b ? tex_b->get_texture_handle() : NULL;
		if (h_a != h_b) {
			return (h_a > h_b);
		}
	}	
	return false; // equal case
}


void zz_scene_octree::render_rterrain ()
{
	PROFILER_RENDER_SUB(Prender_rterrain, "scene_octree: render_rterrain(%d)\n", rterrain_nodes.size());
	
	num_rterrain = (int)rterrain_nodes.size();

	if (num_rterrain == 0) return;

	znzin->renderer->enable_fog(true);

	bool blended = true; 
	zz_terrain_block_rough::begin_state(blended);

#ifdef DRAW_BBOX
	if (znzin->get_rs()->use_draw_bounding_volume) {
		for (zz_vislist::iterator it = rterrain_nodes.begin(), it_end = rterrain_nodes.end(); it != it_end; ++it)
		{
			(*it)->render();
			if ((*it)->get_bvolume()) {
				(*it)->get_bvolume()->draw_bounding_volume();
			}
		}
	}
	else {
		for (zz_vislist::iterator it = rterrain_nodes.begin(), it_end = rterrain_nodes.end(); it != it_end; ++it)
		{
			(*it)->render();
		}
	}
#else
	for (zz_vislist::iterator it = rterrain_nodes.begin(), it_end = rterrain_nodes.end(); it != it_end; ++it)
	{
		(*it)->render();
	}
#endif

	znzin->renderer->enable_fog(false);

	zz_terrain_block_rough::end_state();
}

void zz_scene_octree::render_oterrain ()
{
	PROFILER_RENDER_SUB(Prender_oterrain, "scene_octree: render_oterrain(%d)\n", oterrain_nodes.size());

	bool& alpha_fog = znzin->get_rs()->use_alpha_fog;
	bool saved_alpha_fog = alpha_fog;

	num_oterrain = (int)oterrain_nodes.size();

	if (num_oterrain == 0) return;

	znzin->renderer->enable_fog(false);

	// enforce not to use alpha fog
	alpha_fog = false;
	bool blended;
	zz_terrain_block::begin_state(blended = false);

	for (zz_vislist::iterator it = oterrain_nodes.begin(), it_end = oterrain_nodes.end(); it != it_end; ++it)
	{
		(*it)->render();
	}

	zz_terrain_block::end_state();

	alpha_fog = saved_alpha_fog; // revert to original
}

void zz_scene_octree::render_bterrain ()
{
	PROFILER_RENDER_SUB(Prender_bterrain, "scene_octree: render_bterrain(%d)\n", bterrain_nodes.size());

	znzin->renderer->enable_fog(false);

	num_bterrain = (int)bterrain_nodes.size();

	if (num_bterrain == 0) return;

	// pre-render z-only terrain
	// this is for removing alpha-blended order artifacts.
	// we render z-value only first, and render ordinal color in the next.
	if (znzin->get_rs()->use_zonly_terrain) { // zonly pass
		if (zz_terrain_block::begin_state_zonly())
		{
			for (zz_vislist::iterator it = bterrain_nodes.begin(), it_end = bterrain_nodes.end(); it != it_end; ++it)
			{
				(static_cast<zz_terrain_block*>(*it))->render_zonly();
			}
		}

		zz_terrain_block::end_state_zonly();
	}

	// normal blended terrain
	bool blended = false;
	zz_terrain_block::begin_state(blended);

	for (zz_vislist::iterator it = bterrain_nodes.begin(), it_end = bterrain_nodes.end(); it != it_end; ++it)
	{
		(*it)->render();
	}

	zz_terrain_block::end_state();
}

void zz_scene_octree::render_opaque ()
{
	PROFILER_RENDER_SUB(Prender_opaque, "scene_octree: render_opaque(%d)\n", visible_nodes.size());

	znzin->renderer->enable_fog(false);

	num_visible = (int)visible_nodes.size();

#ifdef DRAW_BBOX
	if (znzin->get_rs()->use_draw_bounding_volume) {
		for (zz_vislist::iterator it = visible_nodes.begin(), it_end = visible_nodes.end(); it != it_end; ++it)
		{
			(*it)->render();
			if ((*it)->get_bvolume()) {
				(*it)->get_bvolume()->draw_bounding_volume();
			}
		}
	}
	else {
		for (zz_vislist::iterator it = visible_nodes.begin(), it_end = visible_nodes.end(); it != it_end; ++it)
		{
			(*it)->render();
		}
	}
#else
	for (zz_vislist::iterator it = visible_nodes.begin(), it_end = visible_nodes.end(); it != it_end; ++it)
	{
		(*it)->render();
	}
#endif
}

void zz_scene_octree::render_transparent ()
{
	num_delayed = (int)delayed_nodes.size();

	PROFILER_RENDER_SUB(Prender_transparent, "scene_octree: render_transparent(%d)\n", num_delayed);

#ifdef DRAW_BBOX
	if (znzin->get_rs()->use_draw_bounding_volume) {
		for (zz_vislist::iterator it = delayed_nodes.begin(), it_end = delayed_nodes.end(); it != it_end; ++it)
		{
			(*it)->render();
			if ((*it)->get_bvolume()) {
				(*it)->get_bvolume()->draw_bounding_volume();
			}
		}
	}
	else {
		for (zz_vislist::iterator it = delayed_nodes.begin(), it_end = delayed_nodes.end(); it != it_end; ++it)
		{
			(*it)->render();
		}
	}
#else
	for (zz_vislist::iterator it = delayed_nodes.begin(), it_end = delayed_nodes.end(); it != it_end; ++it)
	{
		(*it)->render();
	}
#endif
}

void zz_scene_octree::render_ocean ()
{
	zz_camera * cam = znzin->get_camera();
	if (cam->get_under_the_sea()) return; // no need to render if the camera is under the sea

	num_ocean = (int)ocean_nodes.size();

	PROFILER_RENDER_SUB(Prender_ocean, "scene_octree: render_ocean(%d)\n", num_ocean);

#ifdef DRAW_BBOX
	if (znzin->get_rs()->use_draw_bounding_volume) {
		for (zz_vislist::iterator it = ocean_nodes.begin(), it_end = ocean_nodes.end(); it != it_end; ++it)
		{
			(*it)->render();
			if ((*it)->get_bvolume()) {
				(*it)->get_bvolume()->draw_bounding_volume();
			}
		}
	}
	else {
		for (zz_vislist::iterator it = ocean_nodes.begin(), it_end = ocean_nodes.end(); it != it_end; ++it)
		{
			(*it)->render();
		}
	}
#else
	for (zz_vislist::iterator it = ocean_nodes.begin(), it_end = ocean_nodes.end(); it != it_end; ++it)
	{
		(*it)->render();
	}
#endif
}

void zz_scene_octree::render_fonts ()
{
	zz_font * font;
	if (!znzin->fonts) return;

	int i, num_fonts = znzin->fonts->get_num_children();

	PROFILER_RENDER_SUB(Prender_fonts, "scene_octree: render_fonts(%d)\n", num_fonts);

	for (i = 0; i < num_fonts; i++) {
		font = (zz_font*)znzin->fonts->get_child_by_index(i);
		font->render();
	}
}

void zz_scene_octree::render_particle ()
{
	PROFILER_RENDER_SUB(Prender_particle, "scene_octree: render_particle(%d)\n", particle_nodes.size());

	zz_visible * vis;
	num_particles = (int)particle_nodes.size();

	for (int i = 0; i < num_particles; ++i) {
		vis = particle_nodes[i];
		if((vis->get_visibility()) < 0.001f)
		continue;
		vis->render();
	}
}

void zz_scene_octree::render_sky ()
{
	PROFILER_RENDER_SUB(Prender_sky, "scene_octree: render_sky(%d)\n", znzin->sky ? 1 : 0);

	if (znzin->sky) {
		znzin->sky->set_ztest(false);
		znzin->sky->render(true);
	}
}

void zz_scene_octree::render_sky2 ()
{
	PROFILER_RENDER_SUB(Prender_sky, "scene_octree: render_sky(%d)\n", znzin->sky ? 1 : 0);

	znzin->renderer->enable_fog(false);

	if (znzin->sky) {
		znzin->sky->set_ztest(false);  
		znzin->sky->render(true);
	}
}

void zz_scene_octree::render_sky_glow ()
{
	PROFILER_RENDER_SUB(Prender_sky, "scene_octree: render_sky_golw(%d)\n", znzin->sky ? 1 : 0);

	znzin->renderer->enable_fog(false);

	if (znzin->sky) {
		znzin->sky->set_ztest(true);
		znzin->sky->render(true);
	}
}

void zz_scene_octree::render_cull ()
{
	//ZZ_PROFILER_INSTALL(Prender_cull);
	//ZZ_LOG("scene_octree:render_cull\n");

	// get render state
	zz_render_state * state = znzin->get_rs();
	int& pass = state->current_pass;

	znzin->renderer->enable_fog(false);
	
	// render terrain in 1st pass
	if (state->use_multipass) {
		// 1. render sky
		pass = 0; render_sky2();

		// 2. render terrains
		pass = 0; render_oterrain();
		pass = 1; render_oterrain();

		// 2. render terrains
		pass = 0; render_bterrain();
		pass = 1; render_bterrain();

		pass = 0; render_rterrain();

		// render opaque
		pass = 0; render_opaque();
		pass = 1; render_opaque();

		pass = 0; render_transparent();
		pass = 1; render_transparent();

		// render transparent
		pass = 0; render_glow();

		// render ocean
		pass = 0; render_ocean();

		// render particles
		pass = 0; render_particle();
	}
	else { // if single pass
		pass = 0;

		render_sky2();
		render_oterrain();
		render_bterrain();
		render_rterrain();
		render_opaque();
		render_transparent();
		render_glow();	
		render_ocean();
		render_particle();
	}
}

void zz_scene_octree::render_shadowmap_objects (vector<zz_visible*>& nodes, const vec3& compare_pos, float maximum_distance_square)
{
	zz_model * model;
	zz_animatable *animatable;
	zz_visible * vis;
	int i;
	vec3 world_position;
	int num_children = (int)nodes.size();
	float dist_square;
	int count_rendered = 0;
	for (i = 0; i < num_children; i++) {
		vis = nodes[i];
		if (!vis) continue;

		if (IS_A(vis, zz_model) && vis->get_shadow_onoff())   //model도 Shadow On/Off 추가 
		{
			vis->get_position_world(world_position);
			dist_square = world_position.distance_square(compare_pos);
			model = static_cast<zz_model*>(vis);
			model->set_camtarget_distance_square (dist_square);
			if (dist_square < maximum_distance_square)
			{
				model->render(true /* recursive */); // render recursively
			    count_rendered++;
			}
		}
		else if(IS_A(vis, zz_animatable) && vis->get_shadow_onoff() )
		{
			vis->get_position_world(world_position);
			
			zz_bvolume *bv = vis->get_bvolume();
			const zz_bounding_obb *obb;
			obb = bv->get_obb();
				
			dist_square = obb->center.distance_square(compare_pos);
			
			animatable = static_cast<zz_animatable*>(vis);
			animatable->set_camdist_square_recursive (dist_square);
			if (dist_square <  1.5 * maximum_distance_square)
			{
				animatable->render(true);
			    count_rendered++;
			}
		
		}
		
		
	}
	//ZZ_LOG("zz_scene_octree::render_shadowmap_objects(%d)\n", count_rendered);
}

void zz_scene_octree::render_shadowmap ()
{
	if (!znzin->get_rs()->use_shadowmap) return;

	zz_time t = znzin->get_current_time();

	// follow camera
	// TODO: use static camera
	zz_camera * cam = znzin->get_camera();
	zz_camera * lcam = znzin->get_camera_light();
	
	vec3 center_pos;
	zz_model * model = NULL;

	if (!cam) return;

	// get model if camera is camera_follow
	if (cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) { // if an model is targeted, we use this
		zz_camera_follow * follow_cam = reinterpret_cast<zz_camera_follow*>(cam);
		model = follow_cam->get_target();
	}
	
	if (model) {
		model->get_position_world(center_pos);
	}
	else {
        center_pos = cam->get_position();	
	}
	
	if (lcam) {
		// CAUTION: get_position() does not same as get_eye()
		// get_position() means camera target position(pivot) and get_eye() means camera lens position
		static vec3 pos_diff = lcam->get_position();
		static bool first = true;
		if (first) {
			//ZZ_LOG("scene_octree: lcam->pos_diff = (%f, %f, %f)\n", pos_diff.x, pos_diff.y, pos_diff.z);
			first = false;
		}
		lcam->set_position(center_pos + pos_diff);

		if (!model) { // if lcam is center(no target), then update center_pos to lcam.
			center_pos = lcam->get_position();
		}
	}
	
	if (znzin->get_rs()->use_round_shadow) return; // already rendered

	// CAUTION: must disable fog when rendering shadow
	// because shadow shader does not use oFog register
	bool old_fog_state = znzin->get_rs()->use_fog;
	znzin->get_rs()->use_fog = false; // DISABLE FOG

	const float maximum_distance_square = 5000.0f*5000.0f*ZZ_SCALE_IN*ZZ_SCALE_IN; // 20 meter x 20 meter

	znzin->get_rs()->current_pass = 0; // set current pass to zero
	
	znzin->renderer->begin_shadowmap();
	{
		//ZZ_PROFILER_BEGIN(Prender_shadowmap_render);
		// render only character shadows
		//ZZ_LOG("zz_scene_octree: render_shadowmap()\n");
		render_shadowmap_objects(visible_nodes, center_pos, maximum_distance_square);         
		render_shadowmap_objects(delayed_nodes, center_pos, maximum_distance_square);         
	}
	znzin->renderer->end_shadowmap();
	
	// revert fog state
	znzin->get_rs()->use_fog = old_fog_state;
}

void zz_scene_octree::render_shadowmap_viewport (void)
{
	// draw_shadowmap() has own begin()-end() block
	if (znzin->get_rs()->use_shadowmap) {
		//ZZ_PROFILER_BEGIN_CYCLE(Pdraw_shadowmap_viewport, 100);
		znzin->renderer->draw_shadowmap_viewport();
		//ZZ_PROFILER_END(Pdraw_shadowmap_viewport);
	}
}

int zz_scene_octree::update_receive_shadow (zz_model * target)
{
	if (!target) return 0;

	int count = 0;
	int i = 0;

	zz_vislist::iterator it;
	
	for (it = visible_nodes.begin(); it != visible_nodes.end(); ++it) {
		(*it)->set_receive_shadow_now(false);
	}
	for (it = oterrain_nodes.begin(); it != oterrain_nodes.end(); ++it) {
		(*it)->set_receive_shadow_now(false);
	}
	for (it = bterrain_nodes.begin(); it != bterrain_nodes.end(); ++it) {
		(*it)->set_receive_shadow_now(false);
	}
	for (it = delayed_nodes.begin(); it != delayed_nodes.end(); ++it) {
		(*it)->set_receive_shadow_now(false);
	}

	const float margin = 1000.0f*ZZ_SCALE_IN;

	zz_camera * lcam = znzin->get_camera_light();
    
	if(lcam)
	{
		lcam->update_frustum(0);
	}
	
	mat4 light_modelview;
	mat4 inv_light_modelview;
    vec3 minmax_v[2];

	lcam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, light_modelview);
	inv_light_modelview = light_modelview.inverse();
	const zz_viewfrustum& frustum = lcam->get_frustum();
    	
    minmax_v[0].x = minmax_v[0].y = minmax_v[0].z = 1000000.0f;
	minmax_v[1].x = minmax_v[1].y = minmax_v[1].z = -1000000.0f;
	
	for(i = 0; i < 8; i+=1)
	{
		if(minmax_v[0].x > frustum.p[i][0])
			minmax_v[0].x = frustum.p[i][0];

        if(minmax_v[1].x < frustum.p[i][0])
			minmax_v[1].x = frustum.p[i][0];
		
		if(minmax_v[0].y > frustum.p[i][1])
			minmax_v[0].y = frustum.p[i][1];

        if(minmax_v[1].y < frustum.p[i][1])
			minmax_v[1].y = frustum.p[i][1];

		if(minmax_v[0].z > frustum.p[i][2])
			minmax_v[0].z = frustum.p[i][2];

        if(minmax_v[1].z < frustum.p[i][2])
			minmax_v[1].z = frustum.p[i][2];
	}


	assert(target->get_bvolume());
	vec3 * target_minmax = target->get_minmax();
	assert(target_minmax);
	vec3 minmax[2];
	minmax[0].set(target_minmax[0].x - margin, target_minmax[0].y - margin, target_minmax[0].z - margin);
	minmax[1].set(target_minmax[1].x + margin, target_minmax[1].y + margin, target_minmax[1].z + margin);
	
	int num_nodes = collect_by_minmax(0 /* pack_index */, minmax_v, false /* do not skip none-collision object */);
	zz_visible * vis;

	float target_top = target_minmax[1].z;

	vec3 * cminmax;
	for (i = 0; i < num_nodes; ++i) {
		vis = this->get_collect_node(0, i);
		assert(vis);
		if (vis->is_descendant_of_type(ZZ_RUNTIME_TYPE(zz_model))) continue; // Do not care about the model visible.
		cminmax = vis->get_minmax();
		assert(cminmax);
		if (target_top < cminmax[0].z) continue;
        if (vis->is_a(ZZ_RUNTIME_TYPE(zz_terrain_block)))   //test  지형에만 그림
		vis->set_receive_shadow_now(true);         //test 
		count++;
	    //조성현 2005 11 - 18 Test
/*		zz_bvolume *bv = vis->get_bvolume();
		if(bv)
			input_scene_obb(bv->get_obb()); */
	    
	}
	return count;
}

void clear_infrustum (zz_visible * vis)
{
	if (!vis) return;
	assert( vis->is_a(ZZ_RUNTIME_TYPE(zz_visible)) );
	vis->set_infrustum(false);
}

void zz_scene_octree::update_object_sort (void)
{
	if (!znzin->get_rs()->use_object_sorting) return;

	//sort(visible_nodes.begin(), visible_nodes.end(), compare_by_texture);
	sort(delayed_nodes.begin(), delayed_nodes.end(), compare_farther_than);

	// texture > distance
	//sort(terrain_nodes.begin(), terrain_nodes.end(), compare_nearer_than);
	//sort(terrain_nodes.begin(), terrain_nodes.end(), compare_by_texture);
}

void zz_scene_octree::update_init_list (void)
{
	//// initialize list
	//for_each(delayed_nodes.begin(), delayed_nodes.end(), clear_infrustum);
	//for_each(visible_nodes.begin(), visible_nodes.end(), clear_infrustum);
	//for_each(terrain_nodes.begin(), terrain_nodes.end(), clear_infrustum);
	//for_each(ocean_nodes.begin(), ocean_nodes.end(), clear_infrustum);
	//for_each(particle_nodes.begin(), particle_nodes.end(), clear_infrustum);
	delayed_nodes.clear();
	visible_nodes.clear();
	oterrain_nodes.clear();
	bterrain_nodes.clear();
	rterrain_nodes.clear();
	ocean_nodes.clear();
	particle_nodes.clear();
}

zz_model * zz_scene_octree::get_target (vec3& cam_pos_out)
{
	zz_camera * cam = znzin->get_camera();
		
	if (!cam) {
		ZZ_LOG("scene_octree: get_target() failed. no cam\n");
		return NULL; // Do nothing
	}

	zz_model * target_out = NULL;
	
	if (cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		zz_camera_follow * cam_follow = static_cast<zz_camera_follow*>(cam);
		target_out = cam_follow->get_target();
		
		if(znzin->camera_sfx.get_play_onoff())
		{
			mat4 inv_m;
			inv_m = znzin->camera_sfx.camera_sfx_m.inverse();
			cam_pos_out.x = inv_m._14;
			cam_pos_out.y = inv_m._24;
			cam_pos_out.z = inv_m._34;
		}
		else
			cam_pos_out = cam->get_eye();
	}
	else {
		cam_pos_out = cam->get_eye();
	}
	return target_out;
}

void zz_scene_octree::update_distance (const vec3& cam_pos)
{
	zz_visible * vis;
	const float FOG_START_BACK = 1000*ZZ_SCALE_IN;

	vec3 vis_pos;
	float vis_camdist_square;

	float alpha_fog_start;
	alpha_fog_start = (znzin->get_rs()->alpha_fog_start > FOG_START_BACK) ? 
		znzin->get_rs()->alpha_fog_start - FOG_START_BACK : 0; // CAUTION hard-coded 1000
	float alpha_fog_end = znzin->get_rs()->alpha_fog_end;
	float alpha_fog_start2 = alpha_fog_start*alpha_fog_start;
	float alpha_fog_end2 = alpha_fog_end*alpha_fog_end;

	const std::vector<zz_visible *>& infrustum_nodes = octree.get_infrustum_nodes();
	int num_vis = (int)infrustum_nodes.size();

	zz_time current_time = znzin->get_current_time();
	
	const float IMMEDIATE_FLUSH_DISTANCE_SQUARE = 5000.0f*ZZ_SCALE_IN*5000.0f*ZZ_SCALE_IN;
	const float TOO_CLOSE_DISTANCE_SQUERE = 200.0f*ZZ_SCALE_IN*200.0f*ZZ_SCALE_IN;
	zz_node_type * node_type;
	
	
	bool use_seethru_lod = znzin->get_rs()->use_terrain_lod;
	for (zz_vislist::const_iterator it = infrustum_nodes.begin(), it_end = infrustum_nodes.end();
		it != it_end;
		++it)
	{
		vis = (*it);

		// Set it in view-frustum.
		vis->set_infrustum(true);

		// Set the camera distance of this visible.
		vis_camdist_square = vis->get_com_position_world().distance_square(cam_pos);

		vis->set_camdist_square(vis_camdist_square);

		// Fog setting
				
		if (vis->get_receive_fog() && !vis->is_a(ZZ_RUNTIME_TYPE(zz_ocean_block))) {
			const float TOO_LARGE_RADIUS = 3000.0f*ZZ_SCALE_IN; // 30m
			if (vis->get_radius() > TOO_LARGE_RADIUS) {
				
				if(!fog_onoff_mode)
					vis->set_receive_fog_now(true);
				else
					vis->set_receive_fog_now(false);
			}
			else 
				   vis->set_receive_fog_now(vis_camdist_square > alpha_fog_start2);
		}

		if (vis_camdist_square < IMMEDIATE_FLUSH_DISTANCE_SQUARE) {
			vis->flush_device(true);
		}

		node_type = vis->get_node_type();

		if (node_type == ZZ_RUNTIME_TYPE(zz_terrain_block_rough)) {
			rterrain_nodes.push_back(vis);
			continue;
		}
		else if (node_type == ZZ_RUNTIME_TYPE(zz_terrain_block)) {
			
			if (vis_camdist_square < alpha_fog_start2) {
				oterrain_nodes.push_back(vis);
			}
			else if (vis_camdist_square < alpha_fog_end2) {
				bterrain_nodes.push_back(vis);
			}
			// else ignore
			continue;
		}
		else if (node_type == ZZ_RUNTIME_TYPE(zz_particle_emitter)) {
			particle_nodes.push_back(vis);
			continue;
		}
		else if (node_type == ZZ_RUNTIME_TYPE(zz_morpher)) { // we assume morpher is a type of particle
			particle_nodes.push_back(vis);
		}
		else if (node_type == ZZ_RUNTIME_TYPE(zz_ocean_block)) {
			ocean_nodes.push_back(vis);
			continue;
		}

		if (vis->get_transparent()) {
			delayed_nodes.push_back(vis);
		}
		else {
			visible_nodes.push_back(vis);
		}

		if (use_seethru_lod) {
			// update seethru value by lod level
			if (!vis->update_seethru_with_fade(current_time)) continue; // skip if the transparency is zero.

			// set transparent if the model is too close with the camera
			if (vis_camdist_square < TOO_CLOSE_DISTANCE_SQUERE) {
				if (vis->is_descendant_of_type(ZZ_RUNTIME_TYPE(zz_model))) {
					vis->set_seethru((vis_camdist_square/TOO_CLOSE_DISTANCE_SQUERE));
				}
			}
		}
	}
}

// update display list
void zz_scene_octree::update_display_list (zz_time diff_time)
{
	vec3 cam_pos;
	zz_model * target = get_target(cam_pos);

	PROFILE_UPDATE_BEGIN(PCULL);
	cull();
	PROFILE_UPDATE_END(PCULL);

	PROFILE_UPDATE_BEGIN(PUPDATE_INIT_LIST);
	update_init_list();
	PROFILE_UPDATE_END(PUPDATE_INIT_LIST);

	PROFILE_UPDATE_BEGIN(PUPDATE_DISTANCE);
	update_distance(cam_pos);
	PROFILE_UPDATE_END(PUPDATE_DISTANCE);

	PROFILE_UPDATE_BEGIN(PUPDATE_OBJECT_SORT);
	update_object_sort();
	PROFILE_UPDATE_END(PUPDATE_OBJECT_SORT);

	PROFILE_UPDATE_BEGIN(PUPDATE_RECEIVE_SHADOW);
	update_receive_shadow(target);
	PROFILE_UPDATE_END(PUPDATE_RECEIVE_SHADOW);
}

void zz_scene_octree::refresh (zz_visible * vis)
{
	assert(vis);
	octree.refresh(vis);
}

void zz_scene_octree::update_camera (zz_time diff_time)
{
	zz_camera * cam = znzin->get_camera();

	assert(cam);

	if (cam) {
		cam->update_time(false, diff_time);                  
		cam->update_animation(false, diff_time);            
    }
	else {
		ZZ_LOG("scene_octree: update_camera() failed. no camera\n");
	}
}

void zz_scene_octree::update_exp_camera_after (zz_time diff_time)
{
	
	zz_camera * cam = znzin->get_camera();

	assert(cam);

	if (cam) {
		cam->update_camera_collision();          
    }
	else {
		ZZ_LOG("scene_octree: update_camera() failed. no camera\n");
	}

	
}


void zz_scene_octree::update (zz_time diff_time)
{
	// update info for fps
	znzin->get_fps(true);

	invalidate();

	update_camera (diff_time);

	PROFILE_UPDATE_BEGIN(PUPDATE_TIME);
	update_time(diff_time); // update transform
	PROFILE_UPDATE_END(PUPDATE_TIME);

	PROFILE_UPDATE_BEGIN(PUPDATE_DISPLAY_LIST);
	update_display_list(diff_time); // cull by bounding volume and set infrustum
	PROFILE_UPDATE_END(PUPDATE_DISPLAY_LIST);

	PROFILE_UPDATE_BEGIN(PUPDATE_ANIMATION);
	update_animation(diff_time); // update animation(includes vertex animation and bone animation) for infrustum nodes
	PROFILE_UPDATE_END(PUPDATE_ANIMATION);
}

void zz_scene_octree::updateEx(zz_viewfrustum *viewfrustum, const vec3& cam_pos)
{
	// update info for fps
	znzin->get_fps(true);

	invalidate();

	octree.cull_planes(*viewfrustum);

	PROFILE_UPDATE_BEGIN(PUPDATE_INIT_LIST);
	update_init_list();
	PROFILE_UPDATE_END(PUPDATE_INIT_LIST);

	PROFILE_UPDATE_BEGIN(PUPDATE_DISTANCE);
	update_distance(cam_pos);
	PROFILE_UPDATE_END(PUPDATE_DISTANCE);

	PROFILE_UPDATE_BEGIN(PUPDATE_OBJECT_SORT);
	update_object_sort();
	PROFILE_UPDATE_END(PUPDATE_OBJECT_SORT);

}


void zz_scene_octree::update_exp_camera (zz_time diff_time)
{
	// update info for fps
	znzin->get_fps(true);

	invalidate();
    
	update_camera (diff_time);

	PROFILE_UPDATE_BEGIN(PUPDATE_TIME);
	update_time(diff_time); // update transform
	PROFILE_UPDATE_END(PUPDATE_TIME);

    
	PROFILE_UPDATE_BEGIN(PUPDATE_DISPLAY_LIST);
	update_display_list(diff_time); // cull by bounding volume and set infrustum
	PROFILE_UPDATE_END(PUPDATE_DISPLAY_LIST);
	
	
	PROFILE_UPDATE_BEGIN(PUPDATE_ANIMATION);
	update_animation(diff_time); // update animation(includes vertex animation and bone animation) for infrustum nodes
	PROFILE_UPDATE_END(PUPDATE_ANIMATION);

}


void zz_scene_octree::update_transform (zz_time diff_time)
{
	zz_visible * vis;
	zz_node::child_type * vis_child = (zz_node::child_type *)(&znzin->visibles->get_children());
	zz_node::child_iterator it, it_end;

	try {
		for (it = vis_child->begin(), it_end = vis_child->end(); it != it_end; ++it) {
			vis = static_cast<zz_visible*>(*it);
			if (vis->get_inscene())
				vis->update_transform(diff_time);
		}
	}
	catch (...) {
		ZZ_LOG("scene_octree: [%x:%s:%s]->update_transform() failed.\n",
			vis,
			vis->get_name(),
			vis->get_node_type()->type_name);
	}
}

void zz_scene_octree::update_time (zz_time diff_time)
{
	// for behind camera objects, if it is sufficiently near the camera, update time
	const float UPDATE_TIME_BEHIND_DIST = 100.0f*100.0f;

	zz_visible * vis;
	zz_node::child_type * vis_child = (zz_node::child_type *)(&znzin->visibles->get_children());
	zz_node::child_iterator it, it_end;
	for (it = vis_child->begin(), it_end = vis_child->end(); it != it_end; ++it) {
		vis = static_cast<zz_visible*>(*it);
		if (vis->get_inscene()) {
			vis->update_time(true, diff_time);
		}
		else if (vis->get_camdist_square() < UPDATE_TIME_BEHIND_DIST) {
			vis->update_time(true, diff_time);
		}
	}
	// no need to update terarin_block

	// ocean_block
	vis_child = (zz_node::child_type *)(&znzin->ocean_blocks->get_children());
	zz_ocean_block * ocean;
	for (it = vis_child->begin(), it_end = vis_child->end(); it != it_end; ++it) {
		ocean = static_cast<zz_ocean_block*>(*it);
		assert(ocean->get_inscene());
		ocean->update_time(true, diff_time);
	}
}

void zz_scene_octree::update_animation (zz_time diff_time)
{
	zz_visible * vis;
	zz_node::child_type * vis_child = (zz_node::child_type *)(&znzin->visibles->get_children());
	zz_node::child_iterator it, it_end;

	for (it = vis_child->begin(), it_end = vis_child->end(); it != it_end; ++it) {
		//ZZ_PROFILER_BEGIN(Pupdate_animation1);
		vis = static_cast<zz_visible*>(*it);
		//ZZ_LOG("vis = %s, %s\n", vis->get_name(), vis->get_mesh_path(0));
		if (vis->get_infrustum()) {
			vis->update_animation(true, diff_time);
		}
		//ZZ_PROFILER_END(Pupdate_animation1);
	}

	vis_child = (zz_node::child_type *)(&znzin->ocean_blocks->get_children());
	for (it = vis_child->begin(), it_end = vis_child->end(); it != it_end; ++it) {
		vis = static_cast<zz_visible*>(*it);
		if (vis->get_infrustum()) {
			vis->update_animation(true, diff_time);
		}
	}
}

void zz_scene_octree::render ()
{
	PROFILER_RENDER_SUB(PRENDER, "render", 0);

	assert(znzin);
	assert(znzin->get_rs());
	zz_render_state * state = znzin->get_rs();
    zz_renderer_d3d * r = (zz_renderer_d3d *)znzin->renderer;

	// initialize statistics info
	num_delayed = 0;
	num_visible = 0;
	num_oterrain = 0;
	num_bterrain = 0;
	num_rterrain = 0;
	num_ocean = 0;
	num_total = 0;
	num_glows = 0;

			
	// render all mode
	if (state->use_cull) {
		render_cull(); // render cull mode
	}
	else {
		render_all();
	}
	
	draw_scene_sphere();
    draw_scene_obb();
	draw_scene_aabb();
    draw_scene_cylinder();
	draw_scene_obb2();
    draw_scene_line();
	draw_scene_axis();

//	if(znzin->camera_sfx.get_play_onoff())   //조성현 2006 10/23 옵저버 카메라 촬영모드.
//    znzin->camera_sfx.draw_camera();
	
	// render viewfrustum
	if (state->use_draw_viewfrustum) {
		draw_frustum(); // viewfrustum is updated only by cull()
	}

    

	if (s_dump_view) {
		dump_view();
	}
                              
	// render texts out
//	render_fonts();           

//	if(znzin->sfx_onoff)
//	znzin->sfxs->render_sfx();   

}

void zz_scene_octree::clear ()
{
	octree.clear();
}

void zz_scene_octree::draw_frustum ()
{
	vec3 rgb(1, 0, 0);
    mat4 camera_m;
	//zz_camera * cam = znzin->get_camera();
	zz_camera * cam = znzin->get_camera_light();
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;

	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, camera_m);
	
	r->draw_camera(camera_m);
	
	if(znzin->get_rs()->draw_shadowmap_viewport)
		r->draw_shadowmap();
	
	if (!cam) return;
	cam->update_frustum(0);  //test
	
	const zz_viewfrustum& frustum = cam->get_frustum();

	vec3 point8[] = {
		vec3(frustum.p[0][0], frustum.p[0][1], frustum.p[0][2]),
		vec3(frustum.p[1][0], frustum.p[1][1], frustum.p[1][2]),
		vec3(frustum.p[2][0], frustum.p[2][1], frustum.p[2][2]),
		vec3(frustum.p[3][0], frustum.p[3][1], frustum.p[3][2]),
		vec3(frustum.p[4][0], frustum.p[4][1], frustum.p[4][2]),
		vec3(frustum.p[5][0], frustum.p[5][1], frustum.p[5][2]),
		vec3(frustum.p[6][0], frustum.p[6][1], frustum.p[6][2]),
		vec3(frustum.p[7][0], frustum.p[7][1], frustum.p[7][2])
	};

//	znzin->renderer->draw_line(vec3(0, 0, 0), vec3(100, 100, 100), rgb);
	znzin->renderer->draw_line(point8[0], point8[1], rgb);
	znzin->renderer->draw_line(point8[1], point8[3], rgb);
	znzin->renderer->draw_line(point8[3], point8[2], rgb);
	znzin->renderer->draw_line(point8[2], point8[0], rgb);
	znzin->renderer->draw_line(point8[5], point8[4], rgb);
	znzin->renderer->draw_line(point8[4], point8[6], rgb);
	znzin->renderer->draw_line(point8[6], point8[7], rgb);
	znzin->renderer->draw_line(point8[7], point8[5], rgb);

	znzin->renderer->draw_line(point8[0], point8[4], rgb);
	znzin->renderer->draw_line(point8[1], point8[5], rgb);
	znzin->renderer->draw_line(point8[2], point8[6], rgb);
	znzin->renderer->draw_line(point8[3], point8[7], rgb);
}


void zz_scene_octree::input_scene_sphere(float x, float y, float z, float r)
{
	if( (scene_sphere_number+1) < MAX_SCENE_SPHERE )
	{
		scene_sphere[scene_sphere_number].vec[0] = x;
		scene_sphere[scene_sphere_number].vec[1] = y;
		scene_sphere[scene_sphere_number].vec[2] = z;
		scene_sphere[scene_sphere_number].r = r;
	    
		scene_sphere_number+=1;
	} 
}

void zz_scene_octree::draw_scene_sphere()
{
    if(scene_sphere_number > 0)
	{
		zz_renderer_d3d *r = (zz_renderer_d3d*)znzin->renderer;

		for(int i = 0; i < scene_sphere_number; i+=1)
		{
			r->draw_wire_sphere(scene_sphere[i].vec[0], scene_sphere[i].vec[1], scene_sphere[i].vec[2],scene_sphere[i].r);
        	
		}
	    scene_sphere_number = 0;
	}

}


void zz_scene_octree::input_scene_obb(const zz_bounding_obb *obb)
{
	if( (scene_obb_number+1) < MAX_SCENE_OBB )
	{
		scene_obb[scene_obb_number] = obb;
		scene_obb_number+=1;
	}
}

void zz_scene_octree::draw_scene_obb()
{

	
	if(scene_obb_number > 0)
	{
		const zz_bounding_obb *obb;
		vec3 wire_color;
        wire_color.set(0.0f,1.0f,1.0f);

		for(int i=0; i < scene_obb_number; i+=1)  
		{
			obb = scene_obb[i];
			draw_box(obb->center, obb->half_length, obb->rotation, wire_color);
		}
	
		scene_obb_number = 0;
	}
}

void zz_scene_octree::input_scene_aabb(float min[3], float max[3], DWORD color)
{

	if( (scene_aabb_number+1) < MAX_SCENE_AABB )
	{
		scene_aabb[scene_aabb_number].min[0] = ZZ_SCALE_IN * min[0];
		scene_aabb[scene_aabb_number].min[1] = ZZ_SCALE_IN * min[1];
		scene_aabb[scene_aabb_number].min[2] = ZZ_SCALE_IN * min[2];

		scene_aabb[scene_aabb_number].max[0] = ZZ_SCALE_IN * max[0];
		scene_aabb[scene_aabb_number].max[1] = ZZ_SCALE_IN * max[1];
		scene_aabb[scene_aabb_number].max[2] = ZZ_SCALE_IN * max[2];
		   
		scene_aabb[scene_aabb_number].color = color;

		scene_aabb_number+=1;
	}
}

void zz_scene_octree::draw_scene_aabb()
{
    if(scene_aabb_number > 0)
	{
		for(int i = 0; i < scene_aabb_number; i+=1)
		{
			draw_aabb(scene_aabb[i].min,scene_aabb[i].max,vec3(1.0f,0.2f,0.5f));
              
		}
        
		scene_aabb_number = 0;
	}
}


void zz_scene_octree::input_scene_cylinder(float x, float y, float z, float length, float r)
{
	if( (scene_cylinder_number+1) < MAX_SCENE_CYLINDER )
	{
		scene_cylinder[scene_cylinder_number].vec[0] = ZZ_SCALE_IN * x;
		scene_cylinder[scene_cylinder_number].vec[1] = ZZ_SCALE_IN * y;
		scene_cylinder[scene_cylinder_number].vec[2] = ZZ_SCALE_IN * z;
		scene_cylinder[scene_cylinder_number].r = ZZ_SCALE_IN * r;
	    scene_cylinder[scene_cylinder_number].length = ZZ_SCALE_IN * length;
	
		scene_cylinder_number+=1;
	} 
}

void zz_scene_octree::draw_scene_cylinder()
{
    if(scene_cylinder_number > 0)
	{
		zz_renderer_d3d *r = (zz_renderer_d3d*)znzin->renderer;

		for(int i = 0; i < scene_cylinder_number; i+=1)
		{
			r->draw_wire_cylinder(scene_cylinder[i].vec[0], scene_cylinder[i].vec[1], scene_cylinder[i].vec[2],scene_cylinder[i].length,scene_cylinder[i].r);
        	
		}
	    scene_cylinder_number = 0;
	}

}

void zz_scene_octree::input_scene_line(vec3& vec1, vec3& vec2)
{
	if( (scene_line_number+1) < MAX_SCENE_LINE )
	{
		scene_line[scene_line_number].vec1 = ZZ_SCALE_IN * vec1;
     	scene_line[scene_line_number].vec2 = ZZ_SCALE_IN * vec2;
      
	  
		scene_line_number += 1;
	} 
}

void zz_scene_octree::draw_scene_line()
{
    if(scene_line_number > 0)
	{
		zz_renderer_d3d *r = (zz_renderer_d3d*)znzin->renderer;
        vec3 color(1.0f,0.5f,0.3f);
		
		for(int i = 0; i < scene_line_number; i+=1)
		{
		 r->draw_line(scene_line[i].vec1,scene_line[i].vec2,color);
        	
		}
	    scene_line_number = 0;
	}

}

void zz_scene_octree::input_scene_obb2(vec3& center, quat& rotation, float xlength, float ylength, float zlength)
{
	if( (scene_obb2_number+1) < MAX_SCENE_OBB2 )
	{
		scene_obb2[scene_obb2_number].center = ZZ_SCALE_IN * center;
     	scene_obb2[scene_obb2_number].rotation = rotation;
        scene_obb2[scene_obb2_number].xlength = ZZ_SCALE_IN * xlength;
       	scene_obb2[scene_obb2_number].ylength = ZZ_SCALE_IN * ylength;
        scene_obb2[scene_obb2_number].zlength = ZZ_SCALE_IN * zlength;
		scene_obb2_number += 1;
	} 

}

void zz_scene_octree::draw_scene_obb2()
{
	if(scene_obb2_number > 0)
	{
		zz_renderer_d3d *r = (zz_renderer_d3d*)znzin->renderer;
		mat4 model_m;
		mat3 rotation_m;
		float min_vec[3], max_vec[3];
		for(int i=0; i<scene_obb2_number; i+=1)
		{
			scene_obb2[i].rotation.to_matrix(rotation_m);

			model_m._11 = rotation_m._11; model_m._21 = rotation_m._21; model_m._31 = rotation_m._31;  model_m._41 = 0.0f;
			model_m._12 = rotation_m._12; model_m._22 = rotation_m._22; model_m._32 = rotation_m._32;  model_m._42 = 0.0f;
			model_m._13 = rotation_m._13; model_m._23 = rotation_m._23; model_m._33 = rotation_m._33;  model_m._43 = 0.0f;
			model_m._14 = scene_obb2[i].center.x; model_m._24 = scene_obb2[i].center.y; model_m._34 = scene_obb2[i].center.z;  model_m._44 = 1.0f;

		
			min_vec[0]= -scene_obb2[i].xlength; max_vec[0]= scene_obb2[i].xlength; 
			min_vec[1]= -scene_obb2[i].ylength; max_vec[1]= scene_obb2[i].ylength; 
			min_vec[2]= -scene_obb2[i].zlength; max_vec[2]= scene_obb2[i].zlength; 
			
			vec3 color(0.5f,0.2f,1.0f);
		    r->draw_visible_boundingbox(model_m, min_vec, max_vec, D3DCOLOR_COLORVALUE(1.0f,1.0f,0.2f,1.0f));
		}
		scene_obb2_number = 0;
	}
}

void zz_scene_octree::input_scene_axis(zz_visible *vis, float size)
{
	
	if(vis != NULL)
	{
		if( (scene_axis_number+1) < MAX_SCENE_AXIS )
		{
			scene_axis[scene_axis_number].vis = vis;
			scene_axis[scene_axis_number].size = size;
			scene_axis_number += 1;
		}
	}
}

void zz_scene_octree::draw_scene_axis()
{
	if(scene_axis_number > 0)
	{
		zz_renderer_d3d *r = (zz_renderer_d3d*)znzin->renderer;
		mat4 vis_matrix;

		for(int i=0; i<scene_axis_number; i+=1)
		{
			vis_matrix = (scene_axis[i].vis)->get_worldTM();
            r->draw_axis_object(vis_matrix,scene_axis[i].size); 
			
		}
		scene_axis_number = 0;
	}
}

void zz_scene_octree::reset_scene_aabb()
{
	scene_aabb_number = 0;
}


void zz_scene_octree::reset_scene_sphere()
{
	scene_sphere_number = 0;

}

void zz_scene_octree::reset_scene_obb()
{
	scene_obb_number = 0;	  
 
}

void zz_scene_octree::reset_scene_cylinder()
{
	scene_cylinder_number = 0;	  
 
}

void zz_scene_octree::reset_scene_line()
{
	scene_line_number = 0;	  
 
}

void zz_scene_octree::reset_scene_obb2()
{
	scene_obb2_number = 0;
}

void zz_scene_octree::reset_scene_axis()
{
	scene_axis_number = 0;
}


void zz_scene_octree::insert (zz_visible * vis)
{
	//ZZ_PROFILER_INSTALL(Pscene_insert);
	assert(vis);

	octree.insert(vis);
}

void zz_scene_octree::remove (zz_visible * vis)
{
	assert(vis);

	if (octree.remove(vis)) {
		invalidate();
	}
}

// insert all visibles into the scene graph
void zz_scene_octree::build ()
{
	assert(znzin);

	zz_visible * vis;
	zz_hash_table<zz_node*>::iterator it;
	zz_hash_table<zz_node*> * nodes = znzin->visibles->get_hash_table();

	if (!nodes) {
		ZZ_LOG("scere_octree: build() failed. no visibles\n");
		return;
	}

	// reset scene graph to re-insert
	clear();

	for (it = nodes->begin(); it != nodes->end(); it++) {
		vis = (zz_visible *)(*it);

		// if not drawing bones, skip insert
#ifndef ZZ_DRAW_BONE
		if (vis->is_a(ZZ_RUNTIME_TYPE(zz_bone))) continue;
#endif // ZZ_DRAW_BONE

		if (vis->is_a(ZZ_RUNTIME_TYPE(zz_sky))) continue;
		//if (visible->is_visible()) {
		ZZ_LOG("scene_octree: added(%s)\n", vis->get_name());
		//	visible->lay_on_terrain();
		octree.insert(vis);
	}
	this->invalidate();
}

int zz_scene_octree::get_num_viewfrustum_node ()
{
	// validity check
	if (validity == false) {
		return 0;
	}
	return (int)this->octree.get_infrustum_nodes().size();
}

zz_visible * zz_scene_octree::get_viewfrustum_node (int index)
{
	// validity check
	if (validity == false) {
		return NULL;
	}
	assert(index < (int)octree.get_infrustum_nodes().size());
	return octree.get_infrustum_nodes()[index];
}

void zz_scene_octree::get_statistics (int * num_total_out, int * num_transparent_out, int * num_opaque_out, int * num_terrain_out, int * num_ocean_out)
{
	if (num_total_out) *num_total_out = num_total;
	if (num_transparent_out) {
		*num_transparent_out = num_delayed;
		*num_transparent_out += num_particles;
	}
	if (num_opaque_out) *num_opaque_out = num_visible;
	if (num_terrain_out) *num_terrain_out = num_oterrain;
	if (num_ocean_out) *num_ocean_out = num_ocean;
}

void dump_view_sub (const char * category, int index, zz_visible * vis)
{
	unsigned int runit;
	zz_mesh * mesh;
	zz_material * mat;
	zz_texture * tex;
	zz_string mesh_path;
	zz_string tex_path;

	ZZ_LOG("%7s: % 5d[%20s] dist(% 4dm) ", category, index, vis->get_name(), static_cast<int>(sqrtf(vis->get_camdist_square())));
	for (runit = 0; runit < vis->get_num_runits(); ++runit) {
		mesh = vis->get_mesh(runit);
		mat = vis->get_material(runit);
		tex = (mat) ? mat->get_texture(0) : NULL;
		if (mesh && mesh->get_path()) {
			zz_string mesh_full_path(mesh->get_path());
			zz_path::extract_name_ext(mesh_full_path, mesh_path);
		}
		else {
			mesh_path.set("NOMESH");
		}
		if (tex) {
			zz_string tex_full_path(tex->get_path());
			zz_path::extract_name_ext(tex_full_path, tex_path);
		}
		else {
			tex_path.set("NOTEX");
		}
		ZZ_LOG(" mesh(% 4d, %20s) mat(% 3dx% 3d, %20s)",
			(mesh) ? mesh->get_num_faces() : 0,
			mesh_path.get(),
			(tex) ? tex->get_width() : 0,
			(tex) ? tex->get_height() : 0,
			tex_path.get()
			);
	}
	ZZ_LOG("\n");
}

void zz_scene_octree::set_dump_view (bool log_terrain, bool log_ocean, bool log_opaque, bool log_transparent)
{
	if (log_terrain || log_ocean || log_opaque || log_transparent) {
		s_dump_view_flags[0] = log_terrain;
		s_dump_view_flags[1] = log_ocean;
		s_dump_view_flags[2] = log_opaque;
		s_dump_view_flags[3] = log_transparent;
		s_dump_view = true;
	}
	else {
		s_dump_view = false;
	}
}

void zz_scene_octree::dump_view ()
{	
	ZZ_LOG("scene_octree: dump_view()\n");

	// dump terrain
	if (s_dump_view_flags[0])
		for (int i = 0; i < num_oterrain; ++i)
			dump_view_sub("terrain", i, oterrain_nodes[i]);

	// dump ocean
	if (s_dump_view_flags[1])
		for (int i = 0; i < num_ocean; ++i)
			dump_view_sub("ocean", i, ocean_nodes[i]);

	// dump opaque
	if (s_dump_view_flags[2])
		for (int i = 0; i < num_visible; ++i)
			dump_view_sub("opaque", i, visible_nodes[i]);

	// dump ocean
	if (s_dump_view_flags[3])
		for (int i = 0; i < num_delayed; ++i)
			dump_view_sub("delayed", i, delayed_nodes[i]);
}

void zz_scene_octree::render_glow ()
{
	zz_render_state * rs = znzin->get_rs();


	if (!rs->use_glow && !rs->use_glow_fullscene) return;

	PROFILER_RENDER_SUB(Prender_glow, "scene_octree: render_glow()\n", 0);

	zz_renderer * r = znzin->renderer;

	bool restore_begin = false;
	if (r->scene_began()) {
		r->end_scene();
		restore_begin = true;
	}

	r->begin_glow();

	if (num_glows) {
		r->begin_scene(ZZ_RW_GLOW);
		render_opaque();
		render_transparent();
		//render_sky_glow();
		r->end_scene();
	}

	r->end_glow();

	if (restore_begin) {
		r->begin_scene(ZZ_RW_SCENE);
	}
}

int zz_scene_octree::collect_by_minmax (
					   int pack_index,
					   const vec3 minmax[2],
					   bool skip_no_collision)
{
	assert(pack_index < ZZ_MAX_COLLECT_PACKS);
	collect_packs[pack_index].clear();

	octree.collect_by_minmax(collect_packs[pack_index], minmax, skip_no_collision);

	return (int)collect_packs[pack_index].size();
}


void zz_scene_octree::before_render ()
{
	zz_visible * vis;

	if (znzin->sky) {
		znzin->sky->before_render();
	}

	const std::vector<zz_visible *>& infrustum_nodes = octree.get_infrustum_nodes();

	for (zz_vislist::const_iterator it = infrustum_nodes.begin(), it_end = infrustum_nodes.end();
		it != it_end;
		++it)
	{
		vis = (*it);
		vis->before_render();
	}
}

void zz_scene_octree::after_render ()
{
	zz_visible * vis;

	if (znzin->sky) {
		znzin->sky->after_render();
	}

	const std::vector<zz_visible *>& infrustum_nodes = octree.get_infrustum_nodes();

	for (zz_vislist::const_iterator it = infrustum_nodes.begin(), it_end = infrustum_nodes.end();
		it != it_end;
		++it)
	{
		vis = (*it);
		vis->after_render();
	}
}
