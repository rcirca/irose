/** 
 * @file zz_visible.cpp
 * @brief visible class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-mar-2002
 *
 * $Header: /engine/src/zz_visible.cpp 90    05-12-16 8:11p Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font.h"
#include "zz_mesh.h"
#include "zz_material.h"
#include "zz_light.h"
#include "zz_system.h"
#include "zz_camera.h"
#include "zz_renderer.h"
#include "zz_terrain.h"
#include "zz_profiler.h"
#include "zz_model.h"
#include "zz_manager.h"
#include "zz_motion.h"
#include "zz_visible.h"
#include "zz_shader.h"
#include "zz_texture.h"
#include "zz_scene.h"
#include "zz_system.h"

#include <algorithm>

#define ZZ_FADE_TIME ZZ_MSEC_TO_TIME(1000) // in milliseconds

#define USE_CACHE_TM

#define DEFAULT_VOLUME (0.1f)

ZZ_IMPLEMENT_DYNCREATE(zz_visible, zz_node)

vec3 zz_visible::gravity(0, 0, -5.8f);
zz_scene * zz_visible::scene = NULL;

zz_runit::zz_runit ()
	: mesh(NULL), material(NULL), light(NULL),
	glow_type(ZZ_GLOW_NOTSET), glow_color(vec3_null)
{
}

void zz_runit::release ()
{
	ZZ_SAFE_RELEASE(mesh);
	ZZ_SAFE_RELEASE(material);
	ZZ_SAFE_RELEASE(light);
}

zz_visible::zz_visible(void) :
	zz_node(), 
	position(vec3_null),
	rotation(quat_id),
	scale(vec3_one),
	tm_cache_flag(0),
	visibility(1.0f),
	bv(NULL),
	bv_type(ZZ_BV_NONE),
	num_runits(0),
	_onode(NULL),
	cast_shadow(false),
	receive_shadow(false),
	receive_fog(true),
	cast_shadow_now(false),
	receive_shadow_now(false),
	receive_fog_now(true),
	collision_level(ZZ_CL_NONE),
	wire_mode(false),
	draw_priority(ZZ_DP_NORMAL),
	clip_face(ZZ_CLIP_FACE_NONE),
	_user_data(NULL),
	billboard_one_axis_order(-1),
	billboard_axis_order(-1),
	seethru(1.0f),
	prev_position_world(vec3_null),
	camdist_square(0),
	infrustum(false),
	shadow_onoff(false),
	range_square_start(0.0f),
	range_square_end(0.0f),
	fade_state(FADE_NONE),
	fade_start_time(0),
	velocity(vec3_null),
	force(vec3_null),
	mass(0),
	use_gravity(false),
	valid_bvolume(false),
	transparent(false),
	inscene(false),
	glow_type(ZZ_GLOW_NOTSET),
	glow_color(vec3_null),
	delayed_load(true),
	forced_visibility_mode(false)
{
	runits.reserve(2);
}

zz_visible::~zz_visible(void)
{
	if (_onode) {
		znzin->scene.remove(this);
	}
	ZZ_SAFE_DELETE(bv);
}

const mat4& zz_visible::get_worldTM (void)
{
#ifdef USE_CACHE_TM
	if (tm_cache_flag & TRANSFORM_CACHE_INDEX_WORLD_TM) {
		return worldTM;
	}
#endif
	mult(worldTM, get_parentTM(), get_localTM()); // worldTM = get_parentTM() * get_localTM();
	tm_cache_flag |= TRANSFORM_CACHE_INDEX_WORLD_TM;
	return worldTM;
}

const mat4& zz_visible::get_world_inverseTM (void)
{
#ifdef USE_CACHE_TM
	if (tm_cache_flag & TRANSFORM_CACHE_INDEX_WORLD_INVERSE_TM) {
		return world_inverseTM;
	}
#endif
	invert_rot_trans(world_inverseTM, get_worldTM());
	tm_cache_flag |= TRANSFORM_CACHE_INDEX_WORLD_INVERSE_TM;
	return world_inverseTM;
}

const mat4& zz_visible::get_localTM (void)
{
#ifdef USE_CACHE_TM
	if (tm_cache_flag & TRANSFORM_CACHE_INDEX_LOCAL_TM) {
		return localTM;
	}
#endif
	localTM.set(position, rotation);
	localTM.mult_scale(scale);
	tm_cache_flag |= TRANSFORM_CACHE_INDEX_LOCAL_TM;
	return localTM;
}

const mat4& zz_visible::get_parentTM (void)
{
	// parentTM does not use cached TM
	if (!parent_node || !parent_node->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		return mat4_id;
	}
	zz_visible * parent_visible = static_cast<zz_visible *>(parent_node);
	return parent_visible->get_worldTM();
}

template<class T>
void invalidate_transform_template (zz_node * node)
{
	if (!node) return;
	(static_cast<T*>(node))->invalidate_transform_downward();
}

typedef void (*zz_func_invalidate_transform)(zz_node * node);

inline zz_func_invalidate_transform zz_for_each (zz_list<zz_node*>::iterator _first, zz_list<zz_node*>::iterator _last, zz_func_invalidate_transform func)
{
	for (; _first != _last; ++_first) {
		func(*_first);
	}
	return func;
}

// recurse all children and make invalid bounding volume/tm_cache
void zz_visible::invalidate_transform_downward (void)
{
	if (!invalidate_tm_minmax()) return; // we have already done. no need to proceed

	// At using std::for_each, /GL optimization may generate incorrect code for dirty_tm().
	//std::for_each(children.begin(), children.end(), dirty_tm<zz_visible>);
	//zz_for_each(children.begin(), children.end(), invalidate_transform_template<zz_visible>);
	zz_list<zz_node *>::iterator it, it_end;
	for (it = children.begin(), it_end = children.end(); it != it_end; ++it) {
		(static_cast<zz_visible*>(*it))->invalidate_transform_downward();
	}
}

// for all ancestor node, make invalid minmax
void zz_visible::invalidate_minmax_upward ()
{
	if (!invalidate_minmax_once()) return;

	zz_node * node = parent_node;
	zz_visible * vis;
	while (node && (node != get_root())) {
		vis = static_cast<zz_visible*>(node);
		if (!invalidate_minmax_once()) return; // we have already done. no need to proceed
		node = node->get_parent();
	}
}

bool zz_visible::invalidate_minmax_once (void)
{
	if (!valid_bvolume) return false; // already dirty, just skip
	valid_bvolume = false; // auto-invalidate bvolume
	return true;
}

// invalidate tm and minmax
bool zz_visible::invalidate_tm_minmax (void)
{
	if (tm_cache_flag == 0) return false; // already dirty, just skip
	tm_cache_flag = 0; // make all flags to be false.
	valid_bvolume = false; // invalid transform includes invalid bvolume
	return true;
}

void zz_visible::invalidate_transform ()
{
	invalidate_transform_downward();
	invalidate_minmax_upward();
    invalidate_tm_minmax();              //test 12-8
}

void zz_visible::get_modelviewTM (mat4& modelview_matrix)
{
	assert(znzin->get_camera());

	znzin->get_camera()->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, modelview_matrix);
}

const mat4& zz_visible::get_modelviewTM_const ()
{
	assert(znzin->get_camera());

	return znzin->get_camera()->get_transform_const(zz_camera::ZZ_MATRIX_MODELVIEW);
}

void zz_visible::get_modelview_worldTM (mat4& modelview_worldTM)
{
	// world_modelTM = modelviewTM * worldTM;
	mult(modelview_worldTM, get_modelviewTM_const(), get_worldTM());
	//mult(modelview_worldTM, znzin->renderer->get_modelview_matrix(), get_worldTM());
}

// Traverses all children, and render them.
void zz_visible::render_children (bool recursive)
{
	zz_list<zz_node *>::iterator it, it_end;
	zz_visible * child;
	for (it = children.begin(), it_end = children.end(); it != it_end; ++it) {
		child = (zz_visible *)(*it);
		child->render(recursive);
	}
}

// objective:
// 1. recurse update_animation for all children
// 2. apply motion (for animatable)
// 3. 
// We would be happy if update_animation() is called only for visibles in viewfrustum.
void zz_visible::update_animation (bool recursive, zz_time diff_time)
{
	if (children.empty()) return;
	if (!recursive) return;
	
	// Traverses children, and update them.
	zz_list<zz_node *>::iterator it, it_end;
	zz_visible * child;
	for (it = children.begin(), it_end = children.end(); it != it_end; ++it) {
		child = static_cast<zz_visible *>(*it);
		child->update_animation(true, diff_time); // by recursive mode
	}
}

void zz_visible::render (bool recursive)
{
	if (!is_visible()) return;

	static zz_renderer * r = znzin->renderer;
	assert(r);

#ifdef ZZ_DRAW_BONE
	if (is_a(ZZ_RUNTIME_TYPE(zz_model))) vis->set_visibility(0.7f);
#endif

	// rotate to face camera
	if ((billboard_one_axis_order >= 0) ||
		(billboard_axis_order >= 0)) { // enabled
		zz_camera * cam = znzin->get_camera();
		if (cam) {
			if (billboard_one_axis_order >= 0) { // one-axis billboard
				rotate_by_axis_facing_camera(cam->get_eye(), billboard_one_axis_order & 0x03, 
					(billboard_one_axis_order >> 2) & 0x03);
			}
			else { // full-axis billboard
				rotate_facing_camera(cam);
			}
			invalidate_transform();
		}
	}

	if ((r->get_render_where() == ZZ_RW_SHADOWMAP) && !cast_shadow && !shadow_onoff) {
		
	}
	else {
		for (unsigned int i = 0; i < num_runits; ++i) {
			render_runit(i);
		}
	}

	// traverse children, and render them
	if (recursive) {
		render_children(true); // to render shadow, TODEL: THIS
	}
}

void zz_visible::apply_lod (zz_mesh * mesh, zz_material * mat)
{
#if (1)
	zz_assert(mesh);
	zz_assert(mat);

	// set material/mesh lod level
	//const float LOD_THRESHOLD_NEAR = 10000.0f*ZZ_SCALE_IN*10000.0f*ZZ_SCALE_IN;
	//const float LOD_THRESHOLD_MEDIUM = 13000.0f*ZZ_SCALE_IN*13000.0f*ZZ_SCALE_IN;

	const float LOD_BIG_RADIUS = 1000.0f * ZZ_SCALE_IN;
	
	float radius_square = get_radius();
	radius_square *= radius_square;

	//zhotest
	const float LOD_THRESHOLD_NEAR = 5000.0f*ZZ_SCALE_IN*5000.0f*ZZ_SCALE_IN;
	const float LOD_THRESHOLD_MEDIUM = 13000.0f*ZZ_SCALE_IN*13000.0f*ZZ_SCALE_IN;

	int priority = static_cast<int>(camdist_square - radius_square);

	if (priority < 0) priority = 0;

	mat->set_load_priroty( priority );

	if (camdist_square < LOD_THRESHOLD_NEAR) {
		mat->set_lod(zz_lod::LOD_NEAR);
		mesh->set_lod(zz_lod::LOD_NEAR);
	}
	else if (camdist_square < LOD_THRESHOLD_MEDIUM) {
		mat->set_lod(zz_lod::LOD_MEDIUM);
		mesh->set_lod(zz_lod::LOD_MEDIUM);
	}
	else {
		mat->set_lod(zz_lod::LOD_FAR);
		mesh->set_lod(zz_lod::LOD_FAR);
	}
	mesh->apply_lod();
#endif
}

bool zz_visible::flush_device (bool immediate)
{
	zz_mesh * mesh;
	zz_material * mat;

	for (unsigned int i = 0; i < num_runits; ++i) {
		mesh = runits[i].mesh;
		mat = runits[i].material;

		if (!mat->flush_device(immediate)) {
			return false;
		}

		if (!mesh->flush_device(immediate)) {
			return false;
		}
	}

	return true;
}

void zz_visible::render_runit (unsigned int runit_index)
{
	assert(runit_index < runits.size());

	assert(visibility != 0);
	assert(seethru != 0);

	zz_runit& ru = runits[runit_index];

	zz_mesh * mesh = ru.mesh;
	zz_material * mat = ru.material;
	zz_light * light = ru.light;
	const zz_shader * shader = (mat) ? mat->get_shader() : NULL;

	// light can be null
	assert(mesh && mat && shader);

	if (!mat->get_device_updated())
		return;

	if (!mesh->get_device_updated())
		return;

	// convoy object now property to material property
	// visible-dependent material setting
	mat->set_receive_fog(receive_fog_now);
	mat->set_receive_shadow(receive_shadow_now);
	mat->set_cast_shadow(cast_shadow_now);
	
	static const float MIN_GLOW_RANGE_SQUARE = 50.0f*50.0f; // 50 meter

	float glow_dist_scale(1.0f);
	
	if (!znzin->get_rs()->use_glow_ztest) {
		glow_dist_scale = 1.0f - sqrtf(this->camdist_square / MIN_GLOW_RANGE_SQUARE);
		if (glow_dist_scale < 0)
			glow_dist_scale = 0;
	}

	// if visible has own glow type
	if (glow_type == ZZ_GLOW_NOTSET) {
		if (ru.glow_type == ZZ_GLOW_NOTSET) {
			if (mat->get_glow_type() != ZZ_GLOW_NONE) { // if material has glow type
				znzin->scene.increase_num_glows();
			}
			mat->set_object_glow_type(mat->get_glow_type());
			mat->set_object_glow_color(glow_dist_scale * mat->get_glow_color());
		}
		else {
			if (ru.glow_type != ZZ_GLOW_NONE) { // use ru's one
				znzin->scene.increase_num_glows();
			}
			mat->set_object_glow_type(ru.glow_type);
			mat->set_object_glow_color(glow_dist_scale * ru.glow_color);
		}
	}
	else {
		if (glow_type != ZZ_GLOW_NONE) {
			znzin->scene.increase_num_glows();
		}
		// affects material property
		mat->set_object_glow_type(glow_type);
		mat->set_object_glow_color(glow_dist_scale * glow_color);
	}

	// in fixed mode setting, use fixed mode if possible
	// (except for model mesh)
	static zz_render_state * state = znzin->get_rs();
	static zz_renderer * r = znzin->renderer;

	// set modelviewTM from modelview_worldTM
	mat4 modelview_worldTM;
	get_modelview_worldTM(modelview_worldTM);
	r->set_modelview_matrix(modelview_worldTM);   
	r->set_world_matrix(get_worldTM());
	
	// light can be NULL
	state->use_light = (light != NULL);

	mat->set_objectalpha(visibility * seethru);

	assert(mesh->get_device_updated());
		
	// set clip face for this mesh
	mesh->set_clip_face(clip_face);
	
	r->render(mesh, mat, light);
}


void zz_visible::render_runit_ex (unsigned int runit_index)
{
	assert(runit_index < runits.size());

	assert(visibility != 0);
	assert(seethru != 0);

	zz_runit& ru = runits[runit_index];

	zz_mesh * mesh = ru.mesh;
	zz_material * mat = ru.material;
	zz_light * light = ru.light;
	const zz_shader * shader = (mat) ? mat->get_shader() : NULL;

	// light can be null
	assert(mesh && mat && shader);

	if (!mat->get_device_updated())
		return;

	if (!mesh->get_device_updated())
		return;

	// convoy object now property to material property
	// visible-dependent material setting
	mat->set_receive_fog(receive_fog_now);
	mat->set_receive_shadow(receive_shadow_now);
	mat->set_cast_shadow(cast_shadow_now);
	
	static const float MIN_GLOW_RANGE_SQUARE = 50.0f*50.0f; // 50 meter

	float glow_dist_scale(1.0f);
	
	if (!znzin->get_rs()->use_glow_ztest) {
		glow_dist_scale = 1.0f - sqrtf(this->camdist_square / MIN_GLOW_RANGE_SQUARE);
		if (glow_dist_scale < 0)
			glow_dist_scale = 0;
	}

	// if visible has own glow type
	if (glow_type == ZZ_GLOW_NOTSET) {
		if (ru.glow_type == ZZ_GLOW_NOTSET) {
			if (mat->get_glow_type() != ZZ_GLOW_NONE) { // if material has glow type
				znzin->scene.increase_num_glows();
			}
			mat->set_object_glow_type(mat->get_glow_type());
			mat->set_object_glow_color(glow_dist_scale * mat->get_glow_color());
		}
		else {
			if (ru.glow_type != ZZ_GLOW_NONE) { // use ru's one
				znzin->scene.increase_num_glows();
			}
			mat->set_object_glow_type(ru.glow_type);
			mat->set_object_glow_color(glow_dist_scale * ru.glow_color);
		}
	}
	else {
		if (glow_type != ZZ_GLOW_NONE) {
			znzin->scene.increase_num_glows();
		}
		// affects material property
		mat->set_object_glow_type(glow_type);
		mat->set_object_glow_color(glow_dist_scale * glow_color);
	}

	// in fixed mode setting, use fixed mode if possible
	// (except for model mesh)
	static zz_render_state * state = znzin->get_rs();
	static zz_renderer * r = znzin->renderer;

	mat4 buffer_m;
	
	buffer_m = r->get_modelview_matrix();   
	buffer_m = r->get_world_matrix();
	
	
	// light can be NULL
	state->use_light = (light != NULL);

	mat->set_objectalpha(visibility * seethru);

	assert(mesh->get_device_updated());
		
	// set clip face for this mesh
	mesh->set_clip_face(clip_face);
	
	r->render(mesh, mat, light);
    r->draw_axis(30.0f);
}

void zz_visible::clear_runit (void)
{
	for (unsigned int i = 0; i < num_runits; ++i) {
		runits[i].release();
	}
	runits.clear();
	num_runits = 0;

	transparent = false;
	if (bv_type != ZZ_BV_NONE) {
		invalidate_minmax_upward();
		reset_bvolume();
	}
}

void zz_visible::pop_runit (void)
{
	if (num_runits < 0) return;

	zz_runit& ru = runits[num_runits-1];

	ru.release();
	runits.pop_back();
	--num_runits;

	transparent = get_transparent();
	if (bv_type != ZZ_BV_NONE) {
		invalidate_minmax_upward();
		reset_bvolume();
	}
}

void zz_visible::get_axis (int xyz012, vec3& axis_vector)
{
	mat3 rot;
	rotation.to_matrix(rot);

	axis_vector = rot.col(xyz012);
}

void zz_visible::set_axis (int xyz012, const vec3& axis_vector)
{
	mat3 rot;
	rotation.to_matrix(rot);
	rot.set_col(xyz012, axis_vector);
	rotation.from_matrix(rot);
	
	invalidate_transform();
}

bool zz_visible::get_transparent ()
{
	// check material alpha
	for (unsigned int i = 0; i < num_runits; ++i) {
		if (runits[i].material->get_texturealpha()) {
			return true;
		}
	}
	return false;
}

void zz_visible::link_child (zz_node * node_to_be_child)
{
	// make parent-child relation
	zz_node::link_child(node_to_be_child);

	// cast to visible
	zz_visible * child = static_cast<zz_visible*>(node_to_be_child);

	// invalidate transform
	child->invalidate_transform();

	// check bv_type and if has, invalidate minmax and reset_bvolume
	if (bv_type != ZZ_BV_NONE) {
		invalidate_minmax_upward();
		reset_bvolume();
	}

	// propagate the inscene state into child. this is recursive job.
	if (inscene) {
		child->insert_scene();
	}
	else {
		child->remove_scene();
	}
}

void zz_visible::build_mesh_minmax (vec3& mesh_min_out, vec3& mesh_max_out)
{
	if (num_runits == 0) {
		mesh_min_out = mesh_max_out = vec3_null;
		return;
	}

	vec3 mesh_min(vec3_null), mesh_max(vec3_null);

	zz_mesh * mesh;
	// for all meshes
	for (unsigned int i = 0; i < num_runits; ++i) {
		mesh = runits[i].mesh;
        mesh_min = mesh->get_min();
		mesh_max = mesh->get_max();
		
		if (i == 0) { // set initial min/max as the first mesh's
			mesh_min_out = mesh_min;
			mesh_max_out = mesh_max;
		}
		else { // compare min/max and save it
			if (mesh_min.x < mesh_min_out.x) mesh_min_out.x = mesh_min.x;
			if (mesh_min.y < mesh_min_out.y) mesh_min_out.y = mesh_min.y;
			if (mesh_min.z < mesh_min_out.z) mesh_min_out.z = mesh_min.z;
			if (mesh_max.x > mesh_max_out.x) mesh_max_out.x = mesh_max.x;
			if (mesh_max.y > mesh_max_out.y) mesh_max_out.y = mesh_max.y;
			if (mesh_max.z > mesh_max_out.z) mesh_max_out.z = mesh_max.z;
		}
	}
}

void zz_visible::update_bvolume_sub (const vec3& pos_world_in, const quat& quat_world_in, const vec3& scale_in)
{
	assert(bv);
	mat3 rot(quat_world_in);
	bv->update(pos_world_in, rot, scale_in); // scale problem. use method in 3dsmax	
}

void zz_visible::get_local_center (vec3& local_center_out, const vec3& mesh_min, const vec3& mesh_max)
{
    local_center_out = .5f * (mesh_max + mesh_min);
}

// create new bounding volume and reset bv by default local_center:
// 1. calculate min/max of the geometry
// 2. find the difference between the bounding volume center position and the pivot position
// 3. set bounding volume
void zz_visible::reset_bvolume (void)
{
	if (bv_type == ZZ_BV_NONE) return; // skip 

	assert(!valid_bvolume);

	vec3 mesh_min, mesh_max;
	build_mesh_minmax(mesh_min, mesh_max);

	vec3 local_center;
	get_local_center(local_center, mesh_min, mesh_max);
	
	if (!bv) { // create bounding volume, if not have one
		bv = zz_new zz_bvolume(local_center);
	}

	bv->set_local_center(local_center);

	switch (bv_type) {
	case ZZ_BV_SPHERE:
		bv->set_sphere(.5f*(mesh_max.x - mesh_min.x)); // not correct
		break;
	case ZZ_BV_AABB:
		bv->set_aabb(.5f*(mesh_max - mesh_min));
		break;
	case ZZ_BV_OBB:
		bv->set_obb(.5f*(mesh_max - mesh_min));
		break;
	default:
		assert(0 && "not supported bounding_volume type"); // cannot reach here!
	}
	invalidate_minmax_upward();
}

const vec3& zz_visible::get_scale_world (vec3& world_scale)
{
	world_scale = this->scale;

	// traverse parent
	zz_node * current = this->get_parent();
	while (current && current->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		world_scale *= ((zz_visible *)current)->scale;
		current = (zz_visible *)current->get_parent();
	}
	return world_scale;
}

const vec3& zz_visible::get_position_world (vec3& world_position)
{
	if (tm_cache_flag & TRANSFORM_CACHE_INDEX_POSITION_WORLD) {
		world_position = position_world;
		return world_position;
	}

	get_worldTM().get_translation(position_world);
	tm_cache_flag |= TRANSFORM_CACHE_INDEX_POSITION_WORLD;
	world_position = position_world;
	
	return world_position;
}

const vec3& zz_visible::get_com_position_world ()
{
	if (tm_cache_flag & TRANSFORM_CACHE_INDEX_COM_POSITION_WORLD) {
		return com_position_world;
	}
	
	tm_cache_flag |= TRANSFORM_CACHE_INDEX_COM_POSITION_WORLD;
	return get_position_world(com_position_world);
}

const quat& zz_visible::get_com_rotation_world ()
{
	if (tm_cache_flag & TRANSFORM_CACHE_INDEX_COM_ROTATION_WORLD) {
		return com_rotation_world;
	}
	
	tm_cache_flag |= TRANSFORM_CACHE_INDEX_COM_ROTATION_WORLD;
	return get_rotation_world(com_rotation_world);
}

const quat& zz_visible::get_rotation_world (quat& world_rotation)
{
	world_rotation = this->rotation;
	// CAUTION: THIS METHOD DO NOT WORK
	// DEL: get_worldTM().get_rotation().to_matrix(rot_world);

	// traverse parent
	zz_node * current = this->get_parent();
	assert(current);
	while (current != get_root()) { // for case current is zz_manager
		world_rotation = ((zz_visible*)current)->get_rotation() * world_rotation;
		current = current->get_parent();
		assert(current);
	}
	return world_rotation;
}

zz_mesh * zz_visible::get_mesh (unsigned int index)
{
	return (index < num_runits) ? runits[index].mesh : NULL;
}

zz_material * zz_visible::get_material (unsigned int index)
{
	return (index < num_runits) ? runits[index].material : NULL;
}

zz_light * zz_visible::get_light (unsigned int index)
{
	return (index < num_runits) ? runits[index].light : NULL;
}

void zz_visible::rotate_by_axis (const float& theta_radian, const vec3& axis)
{
	static mat3 original, relative, final;
	rotation.to_matrix(original); // set current rotation from quaternion
	relative.set_rot(theta_radian, axis); // set relative rotation
	mult(final, relative, original); // final = relative x original
	rotation.from_matrix(final);
	rotation.normalize();
}

unsigned long zz_visible::release (void)
{
	for (std::vector<zz_runit>::iterator it = runits.begin(), it_end = runits.end();
		it != it_end; ++it)
	{
		(*it).release();
	}
	return zz_node::release();
}

void zz_visible::rotate_by_vector_absolute (const vec3& start, const vec3& end)
{
	// assumes that start and end must be normalized first.
	rotation.set_rot(start, end);
	rotation.normalize();
}

//// matrix version
//void zz_visible::rotate_by_vector_relative (const vec3& start, const vec3& end)
//{
// ZZ_PROFILER_BEGIN(Pmatrix_version);
//	mat3 original, relative, final;
//	rotation.to_matrix(original); // set current rotation from quaternion
//	relative.set_rot(start, end);
//	mult(final, relative, original); // final = original x relative
//	rotation.from_matrix(final);
//	rotation.normalize();
//  ZZ_PROFILER_END(Pmatrix_version);
//}

// quaternion version
void zz_visible::rotate_by_vector_relative (const vec3& start, const vec3& end)
{
	//ZZ_PROFILER_BEGIN(Pquaternion_version);
	quat original, relative, final;
	original = rotation;
	relative.set_rot(start, end);
	rotation = relative * original;
	rotation.normalize();
	//ZZ_PROFILER_END(Pquaternion_version);
}

bool zz_visible::get_intersection_ray_level (
	const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, 
	zz_collision_level collision_level_in,
	zz_mesh_tool::zz_select_method select_method)
{
	//ZZ_PROFILER_INSTALL(get_intersection_ray_level); // 0.817086 msec. in [get_intersection_ray_level]
	if (collision_level_in == ZZ_CL_NONE) {
		// use visible default collision leves
		collision_level_in = collision_level;
	}
	if (this->collision_level == ZZ_CL_NONE) return false;

	const zz_bvolume * bv = get_bvolume();

	bool result;
	
	if (!bv) return false;

	//==================================
	// sphere level
	const zz_bounding_sphere * sphere = bv->get_sphere();
	if (sphere) {
		result = ::intersect(*sphere, ray_origin, ray_direction);
		if (!result) return false;
		if (ZZ_IS_SPHERE_LEVEL(collision_level_in)) {
			contact_point = sphere->center;
			return true; // stop here
		}
	}

	assert( !ZZ_IS_AABB_LEVEL(collision_level_in) && "not yet implemented" );
	//=================================
	// aabb level
	//const zz_bounding_abb * aabb = bv->get_aabb();
	//if (!aabb) { // no bounding aabb
	//	ZZ_LOG("visible: intersect_ray() failed. no bounding aabb\n");
	//	return false;
	//}
	//result = ::intersect(*aabb, ray_origin, ray_direction); // NOT IMPLEMENTED YET!
	//if (!result) return false;
	//if (collision_level == ZZ_CL_AABB) {
	//	contact_point = aabb->center;
	//	return true; // stop here
	//}

	//=================================
	// obb level
	const zz_bounding_obb * obb = bv->get_obb();
	if (obb) {
		result = ::intersect(*obb, ray_origin, ray_direction);
		if (!result) return false;
		if (ZZ_IS_OBB_LEVEL(collision_level_in)) {
			contact_point = obb->center;
			return true; // stop here
		}
	}

	//=================================
	// polygon level
	// iterate all sub-mesh and find intersect point
	bool ret;
	for (unsigned int i = 0; i < num_runits; ++i) {
		ret = zz_mesh_tool::get_intersection_mesh_ray(this->get_mesh(i), &get_worldTM(), &ray_origin, &ray_direction, &contact_point, &contact_normal, select_method);
		if (ret) {
			return true;
		}
	}
	return false;
}

bool zz_visible::get_intersection_ray_level_ex (
	const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, vec3& mesh_p1, vec3& mesh_p2, vec3& mesh_p3,
	zz_collision_level collision_level_in,
	zz_mesh_tool::zz_select_method select_method)
{
	//ZZ_PROFILER_INSTALL(get_intersection_ray_level); // 0.817086 msec. in [get_intersection_ray_level]
	if (collision_level_in == ZZ_CL_NONE) {
		// use visible default collision leves
		collision_level_in = collision_level;
	}
	if (this->collision_level == ZZ_CL_NONE) return false;

	const zz_bvolume * bv = get_bvolume();

	bool result;
	
	if (!bv) return false;

	//==================================
	// sphere level
	const zz_bounding_sphere * sphere = bv->get_sphere();
	if (sphere) {
		result = ::intersect(*sphere, ray_origin, ray_direction);
		if (!result) return false;
		if (ZZ_IS_SPHERE_LEVEL(collision_level_in)) {
			contact_point = sphere->center;
			return true; // stop here
		}
	}

	assert( !ZZ_IS_AABB_LEVEL(collision_level_in) && "not yet implemented" );
	//=================================
	// aabb level
	//const zz_bounding_abb * aabb = bv->get_aabb();
	//if (!aabb) { // no bounding aabb
	//	ZZ_LOG("visible: intersect_ray() failed. no bounding aabb\n");
	//	return false;
	//}
	//result = ::intersect(*aabb, ray_origin, ray_direction); // NOT IMPLEMENTED YET!
	//if (!result) return false;
	//if (collision_level == ZZ_CL_AABB) {
	//	contact_point = aabb->center;
	//	return true; // stop here
	//}

	//=================================
	// obb level
	const zz_bounding_obb * obb = bv->get_obb();
	if (obb) {
		result = ::intersect(*obb, ray_origin, ray_direction);
		if (!result) return false;
		if (ZZ_IS_OBB_LEVEL(collision_level_in)) {
			contact_point = obb->center;
			return true; // stop here
		}
	}

	//=================================
	// polygon level
	// iterate all sub-mesh and find intersect point
	bool ret;
	for (unsigned int i = 0; i < num_runits; ++i) {
		ret = zz_mesh_tool::get_intersection_mesh_ray_ex(this->get_mesh(i), &get_worldTM(), &ray_origin, &ray_direction, &contact_point, &contact_normal, &mesh_p1, &mesh_p2, &mesh_p3, select_method);
		if (ret) {
			return true;
		}
	}
	return false;
}


bool zz_visible::get_intersection_ray (const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, zz_mesh_tool::zz_select_method select_method)
{
	return get_intersection_ray_level(ray_origin, ray_direction, contact_point, contact_normal, ZZ_CL_POLYGON, select_method);
}

bool zz_visible::get_intersection_ray_ex (const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, vec3& mesh_p1, vec3& mesh_p2, vec3& mesh_p3, zz_mesh_tool::zz_select_method select_method)
{
	return get_intersection_ray_level_ex(ray_origin, ray_direction, contact_point, contact_normal, mesh_p1, mesh_p2, mesh_p3, ZZ_CL_POLYGON, select_method);
}


// for now, just obb comparison
// TODO: polygon level intersection determining
bool zz_visible::test_intersection_node (zz_visible * vis, zz_collision_level collision_depth)
{
	if (collision_depth == ZZ_CL_NONE) return false;

	if (this == vis) {
		//ZZ_LOG("visible: test_intersection_node() failed. same nodes\n");
		return false;
	}

	const zz_bvolume * bv_a = this->get_bvolume();
	const zz_bvolume * bv_b = vis->get_bvolume();

	if (!bv_a || !bv_b) {
		return false; // no bounding box
	}

	const zz_bounding_sphere * sphere_a = bv_a->get_sphere();
	const zz_bounding_sphere * sphere_b = bv_b->get_sphere();

	if (!sphere_a || !sphere_b) { // no bounding sphere
		ZZ_LOG("visible: test_intersection_node() failed. no sphere\n");
		return false;
	}

	// sphere-sphere
	if (::intersect(*sphere_a, *sphere_b)) {
		if (collision_depth == ZZ_CL_SPHERE) return true; // until here

		const zz_bounding_aabb * aabb_a = bv_a->get_aabb();
		const zz_bounding_aabb * aabb_b = bv_b->get_aabb();
		
		if (!aabb_a || !aabb_b) {
			return true; // true?
		}
		// aabb-aabb
		if (::intersect(*aabb_a, *aabb_b)) {
			if (collision_depth == ZZ_CL_AABB) return true; // until here

			const zz_bounding_obb * obb_a = bv_a->get_obb();
			const zz_bounding_obb * obb_b = bv_b->get_obb();
			if (!obb_a || !obb_b) {
				return true; // true?
			}
			// obb-obb
			if (::intersect(*obb_a, *obb_b)) {
				// TODO: polygon level check
				return true;
			}
		}
	}
	return false;
}

bool zz_visible::test_intersection_box_level (const vec3& min_in, const vec3& max_in, zz_collision_level collision_level)
{
	if (ZZ_IS_OBB_LEVEL(collision_level) || 
		ZZ_IS_POLYGON_LEVEL(collision_level))
	{
		return false;
	}

	if (!minmax) return false; // not in scene

	if (zz_bounding_aabb::intersect(minmax[0], minmax[1], min_in, max_in) == zz_bounding_aabb::OUTSIDE) return false;
	return true;
}

void zz_visible::set_mesh (unsigned int index, zz_mesh * mesh)
{
	assert(index < num_runits);
	zz_runit& ru = runits[index];
	if (ru.mesh) {
		ZZ_SAFE_RELEASE(ru.mesh);
	}
	ru.mesh = mesh;

	ZZ_ADDREF(mesh);

	if (bv_type != ZZ_BV_NONE)
		reset_bvolume();
}

void zz_visible::set_material (unsigned int index, zz_material * material)
{
	assert(index < num_runits);
	zz_runit& ru = runits[index];
	if (ru.material) {
		ZZ_SAFE_RELEASE(ru.material);
	}
	ru.material = material;
	ZZ_ADDREF(material);

	transparent = get_transparent();
}

	
void zz_visible::set_light (unsigned int index, zz_light * light)
{
	assert(index <= num_runits);

	zz_runit& ru = runits[index];
	if (ru.light) {
		ZZ_SAFE_RELEASE(ru.light);
	}
	ru.light = light;

	ZZ_ADDREF(light);
}

void zz_visible::add_runit (zz_mesh * mesh, zz_material * material, zz_light * light)
{
	zz_runit ru;
	ru.mesh = mesh;
	ru.material = material;
	ru.light = light;
	runits.push_back(ru);
	++num_runits;

	ZZ_ADDREF(mesh);
	ZZ_ADDREF(material);
	ZZ_ADDREF(light);

	transparent = get_transparent();

	if (bv_type != ZZ_BV_NONE) {
		reset_bvolume();
	}
}

// used by unlink node
void zz_visible::send_to_world ()
{
	vec3 world_scale, world_position;
	quat world_rotation;
	
	get_scale_world(world_scale);
	get_position_world(world_position);
	get_rotation_world(world_rotation);

	scale = world_scale;
	position = world_position;
	rotation = world_rotation;

	invalidate_transform();
}

// preserve worldTM
// modify position, rotation, scale in local by to_vis
void zz_visible::send_to_local (zz_visible * to_vis)
{
	vec3 scale_parent(vec3_one), scale_world(vec3_one);
	vec3 pos_parent(vec3_null), pos_world(vec3_null);
	quat rot_parent(quat_id), rot_world(quat_id);
	
	if (to_vis) {
		to_vis->get_scale_world(scale_parent);
		to_vis->get_position_world(pos_parent);
		to_vis->get_rotation_world(rot_parent);
	}

	get_scale_world(scale_world);
	get_position_world(pos_world);
	get_rotation_world(rot_world);

	position = pos_world - pos_parent;
	rotation = rot_world * rot_parent.inverse();
	scale = scale_world / scale_parent;

	invalidate_transform();
}

// axis = 0(x), 1(y), 2(z)
void zz_visible::rotate_by_axis_facing_camera (const vec3& cam_pos_world, int rotation_axis_index, int direction_axis_index)
{
	if (rotation_axis_index == direction_axis_index) return;

	mat3 new_rot;
	mat3 rot(get_rotation());

	vec3 perpendicular_axis;
	vec3 direction_axis;
	vec3 rotation_axis;
	
	switch (rotation_axis_index) {
		case 0: // x
			rotation_axis = vec3(1.0f, 0, 0);
			break;
		case 1: // y
			rotation_axis = vec3(0, 1.0f, 0);
			break;
		case 2: // z
			rotation_axis = vec3(0, 0, 1.0f);
			break;
	}

	vec3 cam_pos;
	// transform to local coordinate system
	zz_visible * parent = static_cast<zz_visible*>(this->get_parent());
	if (parent && parent->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		mult(cam_pos, parent->get_world_inverseTM(), cam_pos_world);
	}
	else { // do not have parent visible node
        cam_pos = cam_pos_world;
	}

	direction_axis = cam_pos;
	//float between_angle = dot(direction_axis, cam_pos);
	//if ((between_angle < 0.0f) || (between_angle > 1.0f)) { // opposite direction is more close to camera
	//	// make direction_axis to negative camera
	//	direction_axis = -cam_pos;
	//}
	//else {
	//	direction_axis = cam_pos;
	//}
	direction_axis.normalize();
	
	cross(perpendicular_axis, direction_axis, rotation_axis);
	cross(direction_axis, rotation_axis, perpendicular_axis);

	int perpendicular_axis_index;
	int diff = rotation_axis_index - direction_axis_index;
	int sum = rotation_axis_index + direction_axis_index;

	// get perpendicular axis index
	if      (sum == 1) perpendicular_axis_index = 2;
	else if (sum == 2) perpendicular_axis_index = 1;
	else if (sum == 3) perpendicular_axis_index = 0;
	
	// get perpendicular vector
	if ((diff == 1) || (diff == -2)) {
		cross(perpendicular_axis, direction_axis, rotation_axis);
	}
	else {
		cross(perpendicular_axis, rotation_axis, direction_axis);
	}

	// set new_rotation
	new_rot.set_col(direction_axis_index, direction_axis);
	new_rot.set_col(perpendicular_axis_index, perpendicular_axis);
	new_rot.set_col(rotation_axis_index, rotation_axis);

	this->set_rotation(quat(new_rot));
}

// axis = 0(x), 1(y), 2(z)
void zz_visible::rotate_facing_camera (zz_camera * cam)
{
	mat3 facing_rot;
	
	cam->get_rotation_facing(facing_rot);

	zz_node * parent = static_cast<zz_node*>(this->get_parent());
	if (parent && parent->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		mat3 world_facing_rot(facing_rot);
		mat3 parent_inverse_rot;
		zz_visible * vis_parent = static_cast<zz_visible*>(parent);
		vis_parent->get_world_inverseTM().get_rot(parent_inverse_rot);
		// facing_world_rotation = parent_rotation * local_rotation
		// local_rotation = parent_inverse_rotation * facing_world_rotation
		mult(facing_rot, parent_inverse_rot, world_facing_rot);
	}
	if (0) { // not working code
		mat3 original_rot;
		this->get_rotation().to_matrix(original_rot);
		mat3 rotate_axis;
		rotate_axis.set_rot(original_rot.col(billboard_axis_order), facing_rot.col(billboard_axis_order));
		mat3 final_rot;
		mult(final_rot, rotate_axis, original_rot);
		this->set_rotation(quat(final_rot));
	}
	this->set_rotation(quat(facing_rot));
}

float zz_visible::get_height (void)
{
	assert(bv);

	const zz_bounding_aabb * aabb = this->bv->get_aabb();
	return 2.0f*aabb->half_length.z;
}

float zz_visible::get_radius (bool select_smaller)
{
	if (!bv) return 0;

	const zz_bounding_obb * obb = this->bv->get_obb();

	if (!obb) {
		const zz_bounding_sphere * sp = this->bv->get_sphere();
		zz_assert(sp);
		return sp->radius;
	}

	float x = obb->half_length.x;
	float y = obb->half_length.y;

	if (select_smaller) {
		return (x < y) ? x : y;
	}
	return (x >= y) ? x : y;
}

bool zz_visible::test_intersection_sphere_moving (const zz_bounding_sphere& sphere, const vec3& prev_center, vec3 * closest_center, float max_distance_square)
{
	vec3 next_center = sphere.center;
	zz_bounding_sphere current_sphere(sphere.radius, prev_center);
	
	float distance2 = prev_center.distance_square(sphere.center); // distance^2
	float diameter2 = 4.0f*sphere.radius*sphere.radius; // diameter^2

	bool intersected = test_intersection_sphere(sphere); // current sphere test
	if (distance2 > max_distance_square) { // too far
		return intersected; // not to deep test, and return simple current sphere test
	}
	vec3 direction = sphere.center - prev_center;
	direction.normalize();
	direction *= sphere.radius;
	int iteration_count = 0;
	const int max_iteration_count = 10;
	while (!intersected && (distance2 > diameter2) && (iteration_count < max_iteration_count)) {
		current_sphere.center += direction; // move near the next center
		intersected = test_intersection_sphere(current_sphere); // is it intersected?
		iteration_count++;
		distance2 = current_sphere.center.distance_square(next_center);
		// testcode
		//ZZ_LOG("test_intersection_sphere_moving(%s, #%d, %f/%f)\n", get_name(), 
		//	iteration_count, distance2, diameter2);
	}

	if (closest_center) {
		*closest_center = current_sphere.center;
	}
	return intersected;
}

bool zz_visible::test_intersection_sphere (const zz_bounding_sphere& sphere)
{
	return this->test_intersection_sphere_level(sphere, ZZ_CL_POLYGON); // use this polygon
}

bool zz_visible::test_intersection_sphere_level (
	const zz_bounding_sphere& sphere_target,
    zz_collision_level collision_level_in)
{
	if (collision_level_in == ZZ_CL_NONE) {
		// use visible default collision leves
		collision_level_in = collision_level;
	}
	if (this->collision_level == ZZ_CL_NONE) return false;

	const zz_bvolume * bv = get_bvolume();

	bool result;
	
	//==================================
	// sphere level
	const zz_bounding_sphere * sphere_source = bv->get_sphere();
	if (!sphere_source) { // no bounding sphere
		ZZ_LOG("visible: test_intersect_sphere_level() failed. no bounding sphere\n");
		return false;
	}
	result = ::intersect(*sphere_source, sphere_target);
	if (!result) return false;
	if (ZZ_IS_SPHERE_LEVEL(collision_level_in)) {
		return true; // stop here
	}

	assert( !ZZ_IS_AABB_LEVEL(collision_level_in) && "not yet implemented" );

	// skip aabb & obb test

	//=================================
	// polygon level
	// iterate all sub-mesh and find intersect point
	for (unsigned i = 0; i < get_num_runits(); ++i) {
		if (zz_mesh_tool::test_intersection_mesh_sphere(get_mesh(i), &get_worldTM(), &sphere_target))
		{
			return true;
		}
	}
	return false;
}

void zz_visible::priority_to ()
{
	// set mesh and texture priority to top
	unsigned int i = 0, j = 0;
	zz_manager * man;
	zz_material * mat;
	zz_mesh * mesh;
	zz_texture * tex;
	zz_runit * ru;
	for (i = 0; i < num_runits; i++) {
		ru = &runits[i];
		mesh = ru->mesh;
		if (mesh) {
			man = (zz_manager*)mesh->get_root();
			assert(man->is_a(ZZ_RUNTIME_TYPE(zz_manager)));
			man->promote_entrance(mesh);
		}
		mat = ru->material;
		if (mat) {
			unsigned int num_textures = mat->get_num_textures();
			for (j = 0; j < num_textures; ++j) {
				tex = mat->get_texture(j);
				man = (zz_manager*)tex->get_root();
				assert(man->is_a(ZZ_RUNTIME_TYPE(zz_manager)));
				man->promote_entrance(tex);
			}
		}
	}
}

void zz_visible::set_billboard_one_axis (bool use_billboard, int rotation_axis_index, int direction_axis_index)
{
	if (!use_billboard) {
		this->billboard_one_axis_order = -1;
	}
	else {
		assert((rotation_axis_index >= 0) && (direction_axis_index >= 0));
		assert((rotation_axis_index <= 2) && (direction_axis_index <= 2));

		if (rotation_axis_index == direction_axis_index) {
			this->billboard_one_axis_order = -1; // disable
		}
		else {
			this->billboard_one_axis_order = (direction_axis_index << 2) + rotation_axis_index;
		}
	}
	this->billboard_axis_order = -1; // invalidate full-axis billboarding
}

void zz_visible::set_billboard_axis (bool use_billboard, int axis_to_camera_index)
{
	if (!use_billboard) {
		this->billboard_axis_order = -1;
	}
	else {
		this->billboard_axis_order = axis_to_camera_index;
	}
	this->billboard_one_axis_order = -1; // invalidate one-axis billboarding
}

// not using this for performance
class set_camdistance_recurse_func : public std::unary_function<zz_visible*, void> {
	float camdistance;

public:
	set_camdistance_recurse_func (float camdistance_in) : camdistance(camdistance_in) {}

	void operator() (zz_visible* vis) {
		vis->set_camdist_square(camdistance);
	}
};

void zz_visible::set_camdist_square_recursive (float distance_square)
{
	set_camdist_square( distance_square );

	if (children.empty()) return;

	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it)
	{
		assert(*it);
		(static_cast<zz_visible*>(*it))->set_camdist_square_recursive( distance_square );
	}
}

// not using this for performance
class set_visibility_recurse_func : public std::unary_function<zz_visible*, void> {
	float visibility;

public:
	set_visibility_recurse_func (float visibility_in) : visibility(visibility_in) {}

	void operator() (zz_visible* vis) {
		vis->set_visibility(visibility);
	}
};

void zz_visible::set_visibility_recursive (float visibility_in)
{
	set_visibility(visibility_in);
    
	if( visibility_in > 0.0001f)
		set_forced_visibility(false);
	else
		set_forced_visibility(true);

	if (children.empty()) return;

	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it)
	{
		assert(*it);
		(static_cast<zz_visible*>(*it))->set_visibility_recursive( visibility_in );
	}
}

void zz_visible::set_lighting_recursive (zz_light* light )
{
    for (unsigned int i = 0; i < num_runits; ++i) {
			set_light(i,light);	
    }

	if (children.empty()) return;

	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it)
	{
		assert(*it);
		(static_cast<zz_visible*>(*it))->set_lighting_recursive(light);
	}
}

void zz_visible::set_shadow_onoff_recursive (bool onoff)
{
	set_shadow_onoff(onoff);
    
	if (children.empty()) return;

	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it)
	{
		assert(*it);
		(static_cast<zz_visible*>(*it))->set_shadow_onoff_recursive(onoff);
	}
}


class set_seethru_recurse_func : public std::unary_function<zz_visible*, void> {
	float seethru;

public:
	set_seethru_recurse_func (float seethru_in) : seethru(seethru_in) {}

	void operator() (zz_visible* vis) {
		vis->set_seethru(seethru);
	}
};

void zz_visible::set_seethru_recursive (float seethru_in)
{
	set_seethru(seethru_in);

	if (children.empty()) return;

	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it)
	{
		assert(*it);
		(static_cast<zz_visible*>(*it))->set_seethru_recursive( seethru_in );
	}
}

bool zz_visible::update_seethru_with_fade (zz_time current)
{
	seethru = 1.0f; // default

	if (range_square_start == range_square_end) {
		return true;
	}

	if (fade_state == FADE_NONE) { // init setting
		if (in_range()) {
			seethru = 1.0f;
			fade_state = FADE_IN_DONE;
			return true;
		}
		else {
			seethru = 0.0f;
			fade_state = FADE_OUT_DONE;
			return false;
		}
	}

	if (fade_state == FADE_IN_DONE) {
		seethru = 1.0f;
		if ((camdist_square < range_square_start) || (camdist_square > range_square_end)) {
			set_fade_out(current);
			return true;
		}
		else {
			return true; // to render
		}
	}
	else if (fade_state == FADE_OUT_DONE) {
		seethru = 0.0f;
		if ((camdist_square > range_square_start) && (camdist_square < range_square_end)) {
			set_fade_in(current);
			return false;
		}
		else {
			return false; // not to render
		}
	}

	assert((fade_state == FADE_IN) || (fade_state == FADE_OUT));

	if (fade_state == FADE_IN) {
		seethru = float(current - fade_start_time)/ZZ_FADE_TIME;
		if (seethru >= 1.0f) {
			seethru = 1.0f;
			fade_state = FADE_IN_DONE;
			return true;
		}
	}
	else if (fade_state == FADE_OUT) {
		seethru = 1.0f - float(current - fade_start_time)/ZZ_FADE_TIME;
		if (seethru <= 0) {
			seethru = 0;
			fade_state = FADE_OUT_DONE;
			return false;
		}
	}
	return true;
}

void zz_visible::accumulate_gravity (zz_time diff_time)
{
	if (!use_gravity) return;
	const float gravity_force_coef = 1.0f/ZZ_TICK_PER_SEC;
	force.z += float(diff_time) * gravity_force_coef * gravity.z;
}


void zz_visible::scene_refresh ()
{
	assert(scene);
	if (inscene) { // object should be in scene
        if (_onode) { // actually  in scene
			if (get_root() == parent_node) {
				scene->refresh(this);
			}
			else { // no need to be actually in scene
				scene->remove(this);
			}
		}
		else { // not actually in scene
			if (get_root() == parent_node) {
				scene->insert(this);
			}
			else {
				// do nothing
			}
		}
	}
	else { // should be not in scene
        if (_onode) { // if actually in scene
			scene->remove(this);
		}
	}
}

void	zz_visible::gather_visible (std::vector<zz_visible*>& visible_holder)
{
	if (inscene)
		visible_holder.push_back(this);

	if (children.empty()) return;
	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it) {
		(static_cast<zz_visible*>(*it))->gather_visible(visible_holder);
	}
}

void	zz_visible::gather_collidable (std::vector<zz_visible*>& collidable_holder)
{
	if (inscene && (collision_level != ZZ_CL_NONE)) {
		collidable_holder.push_back(this);
	}

	if (children.empty()) return;
	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it) {
		(static_cast<zz_visible*>(*it))->gather_collidable(collidable_holder);
	}
}

void zz_visible::init_scene_minmax (void)
{
	assert(valid_bvolume);

	if (!bv) {
		get_position_world(minmax[0]);
		minmax[1] = minmax[0];
	}
	else {
		switch (bv_type)
		{
		case ZZ_BV_SPHERE:
			{
				const zz_bounding_sphere * sphere = bv->get_sphere();
				minmax[0] = -sphere->radius + sphere->center;
				minmax[1] = sphere->radius + sphere->center;
			}
			break;
		case ZZ_BV_OBB: // same as aabb
		case ZZ_BV_AABB:
			{
				const zz_bounding_aabb * aabb = bv->get_aabb();
				minmax[0] = aabb->pmin;
				minmax[1] = aabb->pmax;
			}
			break;
		case ZZ_BV_NONE:
		default:
			get_position_world(minmax[0]);
			minmax[1] = minmax[0];
			break;
		}
	}
}

void zz_visible::expand_scene_minmax (vec3& scene_min_out, vec3& scene_max_out)
{
	assert(valid_bvolume);
	assert(!children.empty());

	// expand from children
	zz_visible * child;
	vec3 * child_minmax;
	for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it) {
		child = static_cast<zz_visible *>(*it);
		child_minmax = child->get_minmax();

		assert(child->is_valid_bvolume());
		if (child_minmax[0].x < scene_min_out.x) scene_min_out.x = child_minmax[0].x;
		if (child_minmax[0].y < scene_min_out.y) scene_min_out.y = child_minmax[0].y;
		if (child_minmax[0].z < scene_min_out.z) scene_min_out.z = child_minmax[0].z;
		if (child_minmax[1].x > scene_max_out.x) scene_max_out.x = child_minmax[1].x;
		if (child_minmax[1].y > scene_max_out.y) scene_max_out.y = child_minmax[1].y;
		if (child_minmax[1].z > scene_max_out.z) scene_max_out.z = child_minmax[1].z;
		///assert((scene_max_out.x - scene_min_out.x) < 1000.f);
	}
}

// recursively update bounding volume info
// 1. rebuild bounding bolume by new position/rotation/scale
// 2. update scene minmax
// caution: update_bvolume() does not create bounding volume, use reset_bvolume() instead.
void zz_visible::update_bvolume (void)
{
	if (valid_bvolume) return;

	if (bv) {
		vec3 accumulated_position, accumulated_scale;
		quat accumulated_rotation;

		get_position_world(accumulated_position);
		get_rotation_world(accumulated_rotation);
		get_scale_world(accumulated_scale);

		update_bvolume_sub(accumulated_position, accumulated_rotation, accumulated_scale);
	}

	if (!children.empty()) {
		for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it) {
			(static_cast<zz_visible *>(*it))->update_bvolume();
		}
		valid_bvolume = true;
		init_scene_minmax();
		expand_scene_minmax(minmax[0], minmax[1]);
	}
	else {
		valid_bvolume = true;
		init_scene_minmax();
	}
	scene_refresh();
}

//template<class T>
//void update_recursive (zz_node * node)
//{
//	if (!node) return;
//	(static_cast<T*>(node))->update_recursive(true);
//}

// update_time should be called every frame.
// if not, it is happen not to see some objects forever.
// objcetive:
// 1. update bounding volume if has been changed
// 2. recurse update_time for all children nodes
// 3. refresh scene
void zz_visible::update_time (bool recursive, zz_time diff_time)
{
	// invalidate infrustum
	infrustum = false;

	update_bvolume();

	// do nothing, just recurse it!
	if (recursive && (!children.empty())) {
		// Traverses children, and update them.
		for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it) {
			(static_cast<zz_visible *>(*it))->update_time(true, diff_time); // by recursive mode
		}
	}
}

// set inscene to true
// delayed scene refresh to be refreshed in update_time()
// CAUTION: insert_scene () should propagate to all ancestors.
// but, it does not care about descendants.
void zz_visible::insert_scene ()
{
	inscene = true;

#if (0)
	if (parent_node != get_root()) {
		assert(((zz_visible*)(parent_node))->get_inscene() == inscene);
	}
#endif

	// check if bvolume is valid
	update_bvolume();
	scene_refresh();

	// propagate to all children
	if (!children.empty()) {
		for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it) {
			(static_cast<zz_visible *>(*it))->insert_scene();
		}
	}
}

// set inscene to false
// delayed scene refresh to be refreshed in update_time()
// CAUTION: remove_scene() should propagate to all descendants.
// but, it does not care about ancestors.
void zz_visible::remove_scene ()
{
	inscene = false;
	
#if (0)
	if (parent_node != get_root()) {
		assert(((zz_visible*)(parent_node))->get_inscene() == inscene);
	}
#endif

	// no need to update bvolume.
	scene_refresh();

	// propagate to all children
	if (!children.empty()) {
		for (child_it it = children.begin(), it_end = children.end(); it != it_end; ++it) {
			(static_cast<zz_visible *>(*it))->remove_scene();
		}
	}
}

void zz_visible::set_glow_recursive (zz_glow_type glow_type_in, const vec3& color_in)
{
	glow_type = glow_type_in;
	glow_color = color_in;

	if (children.empty()) return;

	zz_list<zz_node *>::iterator it, it_end;
	for (it = children.begin(), it_end = children.end(); it != it_end; ++it) {
		static_cast<zz_visible*>(*it)->set_glow_recursive(glow_type_in, color_in);
	}
}

void zz_visible::set_glow_runit (unsigned int runit_in, zz_glow_type glow_type_in, const vec3& color_in)
{
	zz_assert(runit_in < num_runits);

	runits[runit_in].glow_type = glow_type_in;
	runits[runit_in].glow_color = color_in;
}

void zz_visible::before_render ()
{
	zz_mesh * mesh;
	zz_material * mat;

	for (unsigned int runit_index = 0; runit_index < num_runits; ++runit_index) {
		zz_runit& ru = runits[runit_index];

		mesh = ru.mesh;
		mat = ru.material;
		zz_assert(mesh);
		zz_assert(mat);
		apply_lod(mesh, mat);

		if (!mat->get_device_updated()) {
			znzin->materials->load(mat);
			mat->flush_device(delayed_load);
		}
	}
}

void zz_visible::after_render ()
{


}
