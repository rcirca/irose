/** 
 * @file zz_model.cpp
 * @brief model class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    12-jun-2002
 *
 * $Header: /engine/src/zz_model.cpp 85    06-12-11 10:25a Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_mesh.h"
#include "zz_bone.h"
#include "zz_system.h"
#include "zz_channel_position.h"
#include "zz_channel_rotation.h"
#include "zz_profiler.h"
#include "zz_motion_mixer.h"
#include "zz_ik_limb.h"
#include "zz_skeleton.h"
#include "zz_manager.h"
#include "zz_model.h"
#include "zz_vfs.h"
#include <algorithm>

int global_current_frame = 0;
float global_rotation_value = 0.0f;

// CAUTION: model coordinate system depend on 3ds max character stduio default coordinate system
const vec3 zz_model::pivot_turn_axis  = vec3(0, 0, 1);  // pelvis pivot's z-axis
const vec3 zz_model::pivot_front_axis = vec3(0, -1, 0); // negative y
const vec3 ORIGIN_NORMAL(0, 0, 1.0f);

ZZ_IMPLEMENT_DYNCREATE(zz_model, zz_animatable)

zz_model::zz_model(void) :
	euler_angle(vec3_null),
	_euler_angle_last(vec3_null),
	skeleton(NULL), 
	collision_block(NULL),
	apply_gravity_onoff(false),
	dir(pivot_front_axis),
	dir_left_first_(true),
	normal_final(ORIGIN_NORMAL),
	normal_current(ORIGIN_NORMAL),
	normal_last(ORIGIN_NORMAL),
	normal_ratio(0),
	move_velocity(0),
	com_bone(0),
	skip_frames(0)
{
#ifdef ZZ_BLINK_TEST
	init_blink_();
#endif // ZZ_BLINK_TEST

	set_collision_level( ZZ_CL_OBB );
	set_bvolume_type( ZZ_BV_OBB );
	set_cast_shadow( true );
	set_receive_shadow( false );

	runits.reserve(5); // body1, body2, arms, foot, etc...
	//bones.reserve(30);
	//dummies.reserve(10);
	//items.reserve(10);
    shadow_onoff = true;
    camera_collision_onoff = false;
	fored_motion_blend0ff = false;
}

zz_model::~zz_model(void)
{
	// force delete
	if (!bones.empty()) 
		deallocate_bones(bones);
	if (!dummies.empty())
		deallocate_dummies(dummies);
}

#ifdef ZZ_BLINK_TEST
void zz_model::init_blink_ ()
{
	blink_mode_ = BLINK_CLOSE; // to be opened at next time
	blink_open_delay_ = 4000;
	blink_close_delay_ = 100;
	blink_current_delay_ = blink_close_delay_; // to be opened at next time
    blink_close_mode = false;
}
#endif // ZZ_BLINK_TEST

unsigned long zz_model::release (void)
{
	if (refcount == 1) {
		if (!bones.empty()) 
			deallocate_bones(bones);
		if (!dummies.empty())
			deallocate_dummies(dummies);
	}
	return zz_animatable::release();
}

//--------------------------------------------------------------------------------
// Deallocate bones
//--------------------------------------------------------------------------------
void zz_model::deallocate_bones (zz_bone_list& bone_list)
{
	zz_assert(!bone_list.empty());
	zz_delete [] bone_list[0];
	bone_list.clear();
}

void zz_model::deallocate_dummies (zz_dummy_list& dummy_list)
{
	zz_assert(!dummy_list.empty());
	zz_delete [] dummy_list[0];
	dummy_list.clear();
}

//--------------------------------------------------------------------------------
// Allocate bones
//-------------------------------------------------------------------------------
void zz_model::allocate_bones (zz_bone_list& bone_list, int num_bone_list)
{
	assert(bone_list.empty());
	assert(num_bone_list);
	zz_bone * new_bone = zz_new zz_bone [num_bone_list];
	for (int i = 0; i < num_bone_list; ++i) {
		bone_list.push_back(new_bone + i);
	}
}

// make template
void zz_model::allocate_dummies (zz_dummy_list& dummy_list, int num_dummy_list)
{
	assert(dummy_list.empty());
	assert(num_dummy_list);
	zz_dummy * new_dummy = zz_new zz_dummy [num_dummy_list];
	for (int i = 0; i < num_dummy_list; ++i) {
		new_dummy[i].set_bvolume_type(ZZ_BV_NONE);
		dummy_list.push_back(new_dummy + i);
	}
}

#ifdef ZZ_BLINK_TEST
void _set_blink_sub (zz_model * model, zz_clip_face clip_face)
{
	if (model->get_num_bones() < 5) return; // has no face bone

	zz_bone * bone = model->get_bone(4);

	if (!bone) return;

	int num_child = bone->get_num_children();	
	zz_visible * child;
	for (int i = 0; i < num_child; i++) {
		child = static_cast<zz_visible*>(bone->get_child_by_index(i));		
		child->set_clip_face(clip_face);
	}
}
#endif

//--------------------------------------------------------------------------------
// Render the model
//--------------------------------------------------------------------------------
void zz_model::render (bool recursive)
{
	if (!is_visible()) return;

#ifdef ZZ_BLINK_TEST
	if (blink_mode_ == BLINK_CLOSE) {
		_set_blink_sub(this, ZZ_CLIP_FACE_LAST);
	}
	else {
		_set_blink_sub(this, ZZ_CLIP_FACE_FIRST);
	}
#endif // ZZ_BLINK_TEST

	uint16 iskel, imeshbone;
	uint16 imesh;
	uint16 num_mesh_bones;
	uint16 num_bones(bones.size());

	zz_mesh * mesh;

	assert(num_runits == runits.size());

	for (imesh = 0; imesh < num_runits; ++imesh) { // for all meshes in the model
		mesh = runits[imesh].mesh; // Get the current mesh.		
		assert(mesh);

		if (!mesh->get_device_updated()) 
			mesh->flush_device(true); // immediate load. if the mesh was not loaded, bone_indices is not set.
		
		num_mesh_bones = mesh->bone_indices.size();
		for (imeshbone = 0; imeshbone < num_mesh_bones; ++imeshbone) { // for all bones
			iskel = mesh->bone_indices[imeshbone]; // Get the current bone index(=skeleton index).
			assert(iskel < num_bones); // The skeleton index should not exceed the number of bones.
			
			bones[iskel]->set_boneTM_to_shader(imeshbone);
		}
		zz_visible::render_runit(imesh);
	}

	if (recursive) {
		for (zz_item_list::iterator it = items.begin(), it_end = items.end(); it != it_end; ++it) {
			(*it)->render(true);
		}
	}
}

//--------------------------------------------------------------------------------
// Turn the model by angle
//--------------------------------------------------------------------------------
void zz_model::turn (float angle)
{
	euler_angle.x += angle; // affect yaw
	recalc_dir();
}

//--------------------------------------------------------------------------------
// Apply motion
//--------------------------------------------------------------------------------
bool zz_model::apply_motion () // absolute time
{
	if (!motion) return false;

	if (!infrustum) 
	return false; // skip if not in viewfrustum

	if (1) { // zhotest
		const float max_dist = 200.0f;
		const float min_dist = 40.0f;

		if (skip_frames-- > 0) {
			return false; // skip if too far
		}

		float l_cam_dist = sqrtf(camdist_square);

		// skip frame is zero
		if (l_cam_dist > max_dist) {
			skip_frames = 10;
		}
		else if (l_cam_dist < min_dist) {
			skip_frames = 0;
		}
		else {
			skip_frames = int(10.0f * (l_cam_dist - min_dist) / (max_dist - min_dist)); // maximum frame skipping is 10 frame
		}
	}

	unsigned int num_bones = bones.size();
	assert(num_bones > 0);

	// get current frame index
	int current_frame =	motion_controller.get_current_frame();
	zz_time current_time = motion_controller.get_current_time();
	float interp_range = znzin->get_motion_interp_range();
	int fps = motion_controller.get_fps();

	// TODO: get blend weight only if blend_weight was enabled.
	float blend_weight;

	// for each bone, apply channel information
	if (znzin->get_use_motion_interpolation() && (camtarget_distance_square < interp_range) && !fored_motion_blend0ff)
	{
		motion_controller.get_blend_weight(blend_weight);
		for (unsigned int bone_index = 0; bone_index < num_bones; ++bone_index) {
			bones[bone_index]->apply_channel_by_time(current_time, motion, fps, blend_weight);
		}
	}
	else {
		blend_weight = 1.0f;
		for (unsigned int bone_index = 0; bone_index < num_bones; ++bone_index) {
			bones[bone_index]->apply_channel_by_frame(current_frame, motion, blend_weight);
		}
	}

	return true;
}

//--------------------------------------------------------------------------------
// Apply motion
//--------------------------------------------------------------------------------
bool zz_model::apply_motion_stop () // absolute time
{
	if (!motion) return false;

	if (1) { // zhotest
		const float max_dist = 200.0f;
		const float min_dist = 40.0f;

		if (skip_frames-- > 0) {
			return false; // skip if too far
		}

		float l_cam_dist = sqrtf(camdist_square);

		// skip frame is zero
		if (l_cam_dist > max_dist) {
			skip_frames = 10;
		}
		else if (l_cam_dist < min_dist) {
			skip_frames = 0;
		}
		else {
			skip_frames = int(10.0f * (l_cam_dist - min_dist) / (max_dist - min_dist)); // maximum frame skipping is 10 frame
		}
	}

	unsigned int num_bones = bones.size();
	assert(num_bones > 0);

	// get current frame index
	int current_frame =	motion_controller.get_current_frame();
	zz_time current_time = motion_controller.get_current_time();
	float interp_range = znzin->get_motion_interp_range();
	int fps = motion_controller.get_fps();

	// TODO: get blend weight only if blend_weight was enabled.
	float blend_weight;

	// for each bone, apply channel information
	if (znzin->get_use_motion_interpolation() && (camtarget_distance_square < interp_range))
	{
		motion_controller.get_blend_weight(blend_weight);
		for (unsigned int bone_index = 0; bone_index < num_bones; ++bone_index) {
			bones[bone_index]->apply_channel_by_time(current_time, motion, fps, blend_weight);
		}
	}
	else {
		blend_weight = 1.0f;
		for (unsigned int bone_index = 0; bone_index < num_bones; ++bone_index) {
			bones[bone_index]->apply_channel_by_frame(current_frame, motion, blend_weight);
		}
	}

	return true;
}


#ifdef ZZ_BLINK_TEST
void zz_model::update_blink (zz_time diff_time)
{
	
	if(!blink_close_mode)
	{
		blink_current_delay_ += static_cast<uint16>(ZZ_TIME_TO_MSEC(diff_time)); // update blink delay

		if (blink_mode_ == BLINK_OPEN) {
			if (blink_current_delay_ > blink_open_delay_) {
				blink_current_delay_ = 0;
				blink_mode_ = BLINK_CLOSE;
				// set blink state
				blink_close_delay_ = rand()%100 + 10; // range (10, 110)
			}
		}
		else { // close mode
			if (blink_current_delay_ > blink_close_delay_) {
				blink_current_delay_ = 0;
				blink_mode_ = BLINK_OPEN;
				// set blink state
				blink_open_delay_ = rand()%3000 + 100; // range (200, 5200)
			}
		}
	}
	else
	{
		blink_mode_ = BLINK_CLOSE;
		blink_current_delay_ = 0;
	}

}
#endif

void zz_model::set_blink_close_mode(bool true_false)
{
	blink_close_mode = true_false;
}

bool zz_model::get_blink_close_mode()
{
   return blink_close_mode;
}


// Updates motion and blinking(temporary)
void zz_model::update_animation (bool recursive, zz_time diff_time)
{
#ifdef ZZ_BLINK_TEST
	if (infrustum) {
		update_blink(diff_time);
	}
#endif

	// set model rotation
	//rotation = get_turned_initial_rotation(pivot_turn_axis, euler_angle.x*ZZ_TO_RAD);
	get_turned_initial_rotation_incremental(rotation, diff_time);

	// apply normal rotation
	update_normal(diff_time);

	if (recursive) {
		zz_animatable::update_animation(true, diff_time);
	}
}

//--------------------------------------------------------------------------------
// Get COM(center of mass) bone pointer
//--------------------------------------------------------------------------------
zz_bone * zz_model::get_com_bone (void)
{
	assert(skeleton);
	assert(com_bone);
	return com_bone;
}

//--------------------------------------------------------------------------------
// Update local_center:
// The local_center:
// - is the offset vector from bounding volume origin to model pelvis.
// - does not care about world-scale, -rotation, and -position.
// - is in model coordinate system.
// - is updated only in reset_bvolume().
// - is used to compute world coordinates of bounding volume(obb, aabb).
//--------------------------------------------------------------------------------
void zz_model::get_local_center (vec3& local_center_out, const vec3& mesh_min, const vec3& mesh_max)
{
	// Set bv's local center, because the model class's origin is set by pelvis.
	// Set local center to the offset vector which directs from the pelvis to the center of bounding box.
	// In this context, *local* means in model coordinate system.
	vec3 pelvis_local;
	assert(skeleton);
	uint32 root_skeleton_bone_index = skeleton->get_root_bone_index();
	assert(root_skeleton_bone_index >= 0);
	assert(root_skeleton_bone_index < skeleton->get_num_bones());

	pelvis_local = skeleton->get_bone_translation(root_skeleton_bone_index); // get local position of root skeleton bone(pelvis)

	vec3 bv_center_local = .5f * (mesh_min + mesh_max);
	local_center_out = bv_center_local - pelvis_local; // get offset vector from pelvis to center
}

const quat& zz_model::get_com_rotation_world (void)
{
	if (tm_cache_flag & TRANSFORM_CACHE_INDEX_COM_ROTATION_WORLD) {
		return com_rotation_world;
	}
	
	tm_cache_flag |= TRANSFORM_CACHE_INDEX_COM_ROTATION_WORLD;

	// CAUTION: do not use root_bone->get_worldTM()
	// because bone and mesh were not aligned
	mat3 root_rot_world;
	com_bone->get_boneTM().get_rot(root_rot_world);
	
	// nullize scaling factors (is not working correctly in non-uniform scaling)
	if (scale != vec3_one) {
		int i, j;
		float fscale;
		for (i = 0; i < 3; ++i) {
			fscale = this->scale[i];
			for (j = 0; j < 3; ++j) {
				root_rot_world(i, j) = root_rot_world(i, j) / fscale;
			}
		}
	}
	com_rotation_world.from_matrix(root_rot_world);
	return com_rotation_world;
}

//--------------------------------------------------------------------------------
// Update Bounding Volume
//--------------------------------------------------------------------------------
void zz_model::update_bvolume (void)
{
	if (valid_bvolume) return;

	zz_assert(bv_type != ZZ_BV_NONE);

	if (bv) {
		zz_assert(com_bone);
		update_bvolume_sub(get_com_position_world(), get_com_rotation_world(), get_scale());
	}

	// expand scene minmax by items
	if (!items.empty()) {
		zz_item_list::iterator it, it_end;
		for (it = items.begin(), it_end = items.end(); it != it_end; ++it) {
			(*it)->update_bvolume();
		}
		valid_bvolume = true;
		init_scene_minmax();
		expand_scene_minmax(minmax[0], minmax[1]);
	}
	else {
		valid_bvolume = true;
		init_scene_minmax();
	}

	// refresh in scene
	scene_refresh();
}

//--------------------------------------------------------------------------------
// Bind channels
//--------------------------------------------------------------------------------
void zz_model::bind_channels ()
{
	zz_channel ** channels;

	if (motion->is_a(ZZ_RUNTIME_TYPE(zz_motion_mixer))) {
		zz_motion_mixer * mix_motion = static_cast<zz_motion_mixer *>(motion);
		channels = mix_motion->get_motion_arg(0)->channels;
	}
	else {
		channels = motion->channels;
	}

	unsigned int num_channels = motion->get_num_channels();
	int bone_id;
	int num_bones = (int)bones.size();
	unsigned int i;

#ifdef DEBUG
	// This code block is for debugging only.
	// This checks that the max_bone_index of all meshes in this model does not exceed the num_bones, or not.
	{
		int max_bone_index;
		for (i = 0; i < num_runits; ++i) {
			max_bone_index = 	runits[i].mesh->get_max_bone_index();
			if ((num_bones > 0) && (max_bone_index >= num_bones)) {
				ZZ_LOG("model: bind_channel() failed. model(%s)->mesh(#%d:%s)'s max bone index(%d) exceeded num_bones(%d).\n",
					get_name(), i, runits[i].mesh->get_path(), max_bone_index, num_bones);
				assert(!"motion bone index exceeded");
			}
		}
	}
#endif

	for (i = 0; i < num_channels; ++i) {
		bone_id = channels[i]->get_refer_id();

#ifdef DEBUG
		assert(bone_id < num_bones && "bone_id exceeded number of bones. check motion and model!");
		if (bone_id >= num_bones) {
			static int s_bone_id = -1;
			static int s_num_bones = -1;
			if ((s_bone_id != bone_id) || (s_num_bones != num_bones)) {
				ZZ_LOG("model: bind_channels() failed. channel refered bone_id dismatch\n");
				ZZ_LOG("       bone_id(%d) exceeded num_bones(%d)\n",bone_id, num_bones);
				ZZ_LOG("       at model(%s), skeleton(%s), motion(%s)\n",
					this->get_name(), this->skeleton->get_name(), this->motion->get_name());
				s_bone_id = bone_id;
				s_num_bones = num_bones;
			}
			return;
		}
#endif

		if (channels[i]->is_a(ZZ_RUNTIME_TYPE(zz_channel_position))) {
			bones[bone_id]->set_position_channel_index(i);
		}
		else if (channels[i]->is_a(ZZ_RUNTIME_TYPE(zz_channel_rotation))) {
			if (num_bones > bone_id) {
			zz_bone * one = bones[bone_id];
			if (one) {
				bones[bone_id]->set_rotation_channel_index(i);
			}
			else {
				assert(0);
			}
			}
		}
		// else error
	}	
}

//--------------------------------------------------------------------------------
// Attach motion
//--------------------------------------------------------------------------------
void zz_model::attach_motion (zz_motion * motion_to_attach)
{
	//ZZ_LOG("model:attach_motion(%s)\n", get_name());

	invalidate_transform();
	
#ifdef _TESTCODE
	ZZ_LOG("model: attach_motion(%s, %s)\n", get_name(), motion_to_attach->get_path());
#endif

	bool starting_motion = true;
	if (motion) {
		detach_motion();
		starting_motion = false;
	}
	zz_animatable::attach_motion(motion_to_attach);

	if (motion_to_attach == NULL) return;

	bind_channels();

	if (motion_to_attach->is_a(ZZ_RUNTIME_TYPE(zz_motion_mixer))) {
		motion_to_attach->set_owner(this);
	}

	recalc_dir();
}


//--------------------------------------------------------------------------------
// Attach skeleton
// CAUTION:
// do not use scale_in_load. it is obsolete now
//--------------------------------------------------------------------------------
bool zz_model::attach_skeleton (zz_skeleton * skel)
{
	uint32 num_bones, num_dummies;
	uint32 parent_index;
	static char bone_fullname[ZZ_MAX_STRING];
	
	assert(skel);
	if (!skel) {
		ZZ_LOG("model: load_model() failed. no skeleton\n");
		return false;
	}

	assert(skeleton == NULL);

	// set skeleton member
	skeleton = skel;
	
	num_bones = (uint32)skel->get_num_bones();
	num_dummies = (uint32)skel->get_num_dummies();

	assert(num_bones > 0);

	//ZZ_LOG("model: attach_skeleton(%s). num_bones(%d)\n", skel->get_name(), num_bones);
	allocate_bones(bones, num_bones);
	allocate_dummies(dummies, num_dummies);

	uint32 i;
	vec3 translation;
	quat rotation;

	const char * bone_name;
	zz_bone * current_bone;

	// first pass:
	// read each bone, and name it
	for (i = 0; i < num_bones; ++i) {
		current_bone = bones[i];
		// set model
		current_bone->set_model(this);

		// set parent index
		parent_index = skel->get_bone_parent_id(i);
		
		// set translation
		translation = skel->get_bone_translation(i); // ignore scale_in_load
		current_bone->set_position(translation);
		
		// set rotation
		rotation = skel->get_bone_rotation(i);
		current_bone->set_rotation(rotation);

#ifdef _DEBUG
		// set bone name
		bone_name = skel->get_bone_name(i);
		sprintf(bone_fullname, "%s_%s", this->get_name(), bone_name);
		current_bone->set_name(bone_fullname);
#endif

		// link child
		// parent bone was already allocated in previous pass.
		// we assumes that all bone nodes are sorted by parent-to-children order.
		if (i == 0) { // for root bone
			link_child(current_bone); // model includes root bone
		}
		else {
			assert(parent_index < i);
			// make parent-child relationship
			bones[parent_index]->link_child(current_bone);
		}

		// calculate bone offset tm
		current_bone->calculate_bone_offsetTM();

		//ZZ_LOG("model: [%s].bones[%d] = [%s], parent = [%d]\n", get_name(), i, bone_fullname, parent_indices[i]);
	}

	// dummy
	zz_dummy * current_dummy;
	for (i = 0; i < num_dummies; ++i) {
		current_dummy = dummies[i];

		// set model
		link_child(current_dummy);

		// set parent index
		parent_index = skel->get_dummy_parent_id(i);
		
		// set translation
		translation = skel->get_dummy_translation(i);
		current_dummy->set_position(translation);
		
		// set rotation
		rotation = skel->get_dummy_rotation(i);
		current_dummy->set_rotation(rotation);

		// set bone name
		bone_name = skel->get_dummy_name(i);
		sprintf(bone_fullname, "%s_%s", this->get_name(), bone_name);
		current_dummy->set_name(bone_fullname);

		bones[parent_index]->link_child(current_dummy);

		// no need to calculate bone offsetTM
	}

	// set root bone
	int com_bone_index = skel->get_root_bone_index();
	assert(com_bone_index < (int)num_bones);
	com_bone = bones[com_bone_index];

	return true;
}

//--------------------------------------------------------------------------------
// Set direction
//--------------------------------------------------------------------------------
void zz_model::set_direction (float angle, bool immediate)
{
	euler_angle.x = angle;
	if (immediate) {
		_euler_angle_last.x = angle;
	}
	recalc_dir();
	get_turned_initial_rotation(rotation, pivot_turn_axis, euler_angle.x*ZZ_TO_RAD);
}

//--------------------------------------------------------------------------------
// Recalculate direction vector
//
// calculate model's front vector
//--------------------------------------------------------------------------------
void zz_model::recalc_dir (void)
{
	// get_axis(_Y_, from);
	// get the rotation quaternion which rotates the model around the up-vector of the model
	static quat init_rotation;
	static mat3 mat;
	get_turned_initial_rotation(init_rotation, pivot_turn_axis,euler_angle.x*ZZ_TO_RAD);   //test euler_angle.x*ZZ_TO_RAD);
	init_rotation.to_matrix(mat);

	// get the character's front moving direction
	vec3 motion_dir = pivot_front_axis;
	if (motion) { // apply new motion dir
		motion_dir = motion->get_direction_vector();
	}
	mult(dir, mat, motion_dir);
}

//--------------------------------------------------------------------------------
// Set up the look-at vector of model.
// This is hard-coded. refactor it.
//--------------------------------------------------------------------------------
bool zz_model::set_look_at (const vec3& there, bool immediate) // internally model's y-axis is front direction
{
	bool ret = false;
	vec3 to, from, origin;
	to = there - position;
	from = get_look_at();

	origin = pivot_front_axis; // character's original front direction by character studio default
	to.z = 0.; // project vector to the ground
	to.normalize();
	from.z = 0.; // project vector to the ground
	from.normalize();

	vec3 cross_vector;
	cross(cross_vector, from, to);

	float cos_angle;
	get_angle(cos_angle, from, to);
 
	const float MIN_DEGREE = 360;
	if (fabs(cos_angle*ZZ_TO_DEG) < MIN_DEGREE) { // if it is close to to_vector   //주치 범위 0 - 180 
		// rebuild cos_angle and cross_vector
		get_angle(cos_angle, origin, to);
		cross(cross_vector, origin, to);
		if (cross_vector.z < 0) { // reversed order
			euler_angle.x = -cos_angle*ZZ_TO_DEG;
		}
		else {
			euler_angle.x = cos_angle*ZZ_TO_DEG;
		}		
	}
	else {                                                        // 절대 나올수 없는 조건
		if (cross_vector.z < 0) { // increase direction with counter-clock wise
			this->euler_angle.x -= MIN_DEGREE;
		}
		else {
			this->euler_angle.x += MIN_DEGREE;
		}
	}

	if (immediate) {
		_euler_angle_last.x = euler_angle.x;
	}
	
	recalc_dir();
	return true;
}

//--------------------------------------------------------------------------------
// Get turned initial rotation by incremental method
//
// If the rotation is too big to reach the final angle at once, we have to divide the angle
// into several small angles.
//
// *_eular_angle_last* is the really affected angle.
// *euler_angle* is the target angle to which we will approach.
//--------------------------------------------------------------------------------
const quat& zz_model::get_turned_initial_rotation_incremental (quat& rotation_out, zz_time time_diff_in)
{
	const float angle_per_time = 3.0f*360.0f / ZZ_TICK_PER_SEC;  // test  3.f*360.0f / ZZ_TICK_PER_SEC; // velocity thing

	float angle_diff = euler_angle.x - _euler_angle_last.x; // difference between target angle and last(current) angle
	if (angle_diff == 0.0f) { // not changed
		goto goto_get_turned_initial_rotation_incremental_end;
	}
	
	float new_angle_diff = angle_per_time * time_diff_in; // ideal angle difference by computation
	
	// choose the closest angle   각을 -180 에서 180 로 고정
	if (angle_diff > 180.0f) {
		angle_diff = -(360.0f - angle_diff); // convert to negative direction
	}
	else if (angle_diff < -180.0f) {
		angle_diff = 360.0f + angle_diff; // convert to positive direction
	}

#if (1) // If we need more smooth version, then scale by interpolation factor(t).   ----------------- Interpolation------------------------
	float diff_real_and_comp = FABS(FABS(angle_diff) - new_angle_diff);
	// *t* is the interpolation factor. affected angle range is [0.0f - 45.0f]
	float t = (diff_real_and_comp*diff_real_and_comp)/(45.0f*45.0f);
	if (t > 1.0f) t = 1.0f;
	if (t < 0.3f) t = 0.3f; // clamp if less than 0.3f
	// scale new_angle_diff by interpolation factor *t*
	new_angle_diff *= t;
#endif                                                                            //   -------------------------------------------------------

	if (new_angle_diff < FABS(angle_diff)) { // if angle difference is too small
		if (angle_diff > 0) { // positive direction
			_euler_angle_last.x += new_angle_diff;
		}
		else { // negative direction
			_euler_angle_last.x -= new_angle_diff;
		}
	}
	else {
		_euler_angle_last.x = euler_angle.x;
	}

 //  _euler_angle_last.x = euler_angle.x;  //test


goto_get_turned_initial_rotation_incremental_end:
	
	recalc_dir();
	
	return get_turned_initial_rotation(rotation_out, pivot_turn_axis, _euler_angle_last.x*ZZ_TO_RAD);
}


//--------------------------------------------------------------------------------
// Get turned rotation
//--------------------------------------------------------------------------------
const quat& zz_model::get_turned_initial_rotation (quat& rotation_out, const vec3& axis, float angle_radian)
{
	axis_to_quat(rotation_out, axis, angle_radian);
	//turned.normalize(); // if not, rotation can be weird.
	return  rotation_out; 
}

//--------------------------------------------------------------------------------
// Link to dummy node.
// 
// Get the dummy node's parent, translate and link to parent node
//--------------------------------------------------------------------------------
void zz_model::link_dummy (zz_visible * vis, uint32 dummy_index)
{
	zz_assert(vis);
	zz_assert(skeleton);
	zz_assertf(dummy_index < skeleton->get_num_dummies(),
		"link_dummy(%s, %d) failed. dummy_index(%d) is less than num_dummies(%d).",
		vis->get_name(), dummy_index, dummy_index, skeleton->get_num_dummies());
	
	zz_dummy * dummy = get_dummy(dummy_index);
	assert(dummy);

	add_item(vis);
	dummy->link_child(vis);
	vis->set_cast_shadow(cast_shadow);

	//ZZ_LOG("model: link_dummy [%s] to [%s] done\n", vis->get_name(), parent->get_name());	
}


//--------------------------------------------------------------------------------
// Get dummy position in world coordinate system
//--------------------------------------------------------------------------------
vec3& zz_model::get_dummy_position_world (vec3& world_pos, int dummy_index )
{
	assert(skeleton);
	assert(dummy_index < (int)skeleton->get_num_dummies());
	
	vec3 local_pos = skeleton->get_dummy_translation(dummy_index);
	
	int parent_id = skeleton->get_dummy_parent_id(dummy_index);
	zz_visible * parent = this->get_bone(parent_id);
	assert(parent);

	mult(world_pos, parent->get_worldTM(), local_pos);
	return world_pos;
}


//--------------------------------------------------------------------------------
// Check the intersection ray and model, and return contact point if it was intersected.
//
// vis-ray intersection test and get the contact point
//--------------------------------------------------------------------------------
bool zz_model::get_intersection_ray (const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, zz_mesh_tool::zz_select_method select_method)
{
	return get_intersection_ray_level(ray_origin, ray_direction, contact_point, contact_normal, ZZ_CL_OBB, select_method);
}

//--------------------------------------------------------------------------------
// Set rotation of model
//--------------------------------------------------------------------------------
void zz_model::set_rotation (const quat& rotation_to_set)
{
	// initial model direction is -y axis, assuming that bone was not considered.

	// rotate -y axis by quaternion
	vec3 dir;
	mat3 rot(rotation_to_set);
	mult(dir, rot, pivot_front_axis);

	// project to x-y plane
	dir.z = 0;
	dir.normalize();

	// rebuild cos_angle and cross_vector
	// from zz_model::set_look_at()
	float cos_angle;
	vec3 cross_vector;
	get_angle(cos_angle, pivot_front_axis, dir);
	cross(cross_vector, pivot_front_axis, dir);
	if (cross_vector.z < 0) { // reversed order
		euler_angle.x = -cos_angle*ZZ_TO_DEG;
	}
	else {
		euler_angle.x = cos_angle*ZZ_TO_DEG;
	}	
	recalc_dir();
}

void _get_height_sub (zz_visible * vis, float& top, float& bottom)
{
	if ((vis->get_node_type() != ZZ_RUNTIME_TYPE(zz_model)) &&
		(vis->get_node_type() != ZZ_RUNTIME_TYPE(zz_visible)) &&
		(vis->get_node_type() != ZZ_RUNTIME_TYPE(zz_bone)))
	{
		return;
	}

	if (vis->get_bvolume()) {
		const zz_bounding_aabb * aabb = vis->get_bvolume()->get_aabb();
		top = (aabb->pmax.z > top) ? aabb->pmax.z : top;
		bottom = (aabb->pmin.z < bottom) ? aabb->pmin.z : bottom;
	}

	int num_child = vis->get_num_children();	
	zz_visible * child;
	for (int i = 0; i < num_child; i++) {
		child = static_cast<zz_visible*>(vis->get_child_by_index(i));
		_get_height_sub(child, top, bottom);
	}
}

//--------------------------------------------------------------------------------
// Get the model height
//--------------------------------------------------------------------------------
float zz_model::get_height (void)
{
	if (bv_type == ZZ_BV_NONE) return 0;

	vec3 height_vmin, height_vmax;
	if (!valid_bvolume) {
					
		if (bv) {
			zz_assert(com_bone);
			update_bvolume_sub(get_com_position_world(), get_com_rotation_world(), get_scale());
		}

		// expand scene minmax by items
		if (!items.empty()) {
			zz_item_list::iterator it, it_end;
			for (it = items.begin(), it_end = items.end(); it != it_end; ++it) {
				(*it)->update_bvolume();
			}
			valid_bvolume = true;
			init_scene_minmax();
			
		}
		else {
			valid_bvolume = true;
			init_scene_minmax();
		   
		}

		// refresh in scene
		scene_refresh();
			
	}

	height_vmin = minmax[0];
	height_vmax = minmax[1];
	
	if (!items.empty())
		expand_scene_minmax_height(height_vmin, height_vmax);

	return (height_vmax.z - height_vmin.z); 
}


//--------------------------------------------------------------------------------
// Link item
//--------------------------------------------------------------------------------
bool zz_model::add_item (zz_visible * item_in)
{
	return (item_in) ? (items.push_back(item_in), true) : false;
}

//--------------------------------------------------------------------------------
// Unlink item
//--------------------------------------------------------------------------------
bool zz_model::del_item (zz_visible * item_in)
{
	std::vector<zz_visible*>::iterator it = std::find(items.begin(), items.end(), item_in);
	if (it != items.end()) {
		items.erase(it);
		return true;
	}
	return false;
}

void zz_model::link_bone (zz_visible * item_in, uint32 bone_index)
{
	assert(bone_index < get_num_bones());
	zz_bone * bone = get_bone(bone_index);
	assert(bone);
	add_item(item_in);
	bone->link_child(item_in);
	item_in->set_cast_shadow(cast_shadow);
}

void zz_model::unlink_bone (zz_visible * child_in)
{
	del_item(child_in);
	unlink_child(child_in);
}

void zz_model::set_seethru_recursive (float seethru_in)
{
	set_seethru(seethru_in);
	for (std::vector<zz_visible*>::iterator it = items.begin(), it_end = items.end();
		it != it_end; ++it) {
		(*it)->set_seethru(seethru_in);
	}
}

void zz_model::set_visibility_recursive (float visibility_in)
{
	set_visibility(visibility_in);
	for (std::vector<zz_visible*>::iterator it = items.begin(), it_end = items.end();
		it != it_end; ++it) {
		(*it)->set_visibility(visibility_in);
	}
	
	for (zz_bone_list::iterator bit = bones.begin(), bit_end = bones.end(); bit != bit_end; ++bit) {
		(*bit)->set_visibility_recursive( visibility_in);
	}
	
	for (zz_dummy_list::iterator dit = dummies.begin(), dit_end = dummies.end(); dit != dit_end; ++dit) {
		(*dit)->set_visibility_recursive(visibility_in);
	}
}

void zz_model::set_lighting_recursive(zz_light* light)
{
	 for (unsigned int i = 0; i < num_runits; ++i) {
			set_light(i,light);	
    }

	for (std::vector<zz_visible*>::iterator it = items.begin(), it_end = items.end();
		it != it_end; ++it) {
		(*it)->set_lighting_recursive(light);
	}
	
	for (zz_bone_list::iterator bit = bones.begin(), bit_end = bones.end(); bit != bit_end; ++bit) {
		(*bit)->set_lighting_recursive(light);
	}
	
	for (zz_dummy_list::iterator dit = dummies.begin(), dit_end = dummies.end(); dit != dit_end; ++dit) {
		(*dit)->set_lighting_recursive(light);
	}
}

vec3& zz_model::get_move_velocity_by_time (zz_time diff_time, vec3& velocity_out)
{
	// set model position
	// .. the *velocity* is the distance of the model per second.
	const float move_force_coefficient = 1.0f/(float(ZZ_TICK_PER_SEC));

	// update velocity_vector
	velocity_out = get_look_at();
	velocity_out *= move_force_coefficient * move_velocity * (float)diff_time;
	return velocity_out;
}

//--------------------------------------------------------------------------------
// Update model normal by linear interpolation
//--------------------------------------------------------------------------------
void zz_model::update_normal (zz_time diff_time)
{
	const float REACH_SECOND  = 2.0f / ZZ_TICK_PER_SEC; // to reach in REACH_TIME second

	if (normal_final != ORIGIN_NORMAL) {
		if (normal_current != normal_final) {
			normal_ratio += diff_time * REACH_SECOND;
			if (normal_ratio > 1.0f) {
				normal_ratio = 1.0f;
				normal_current = normal_final;
			}
			else {
				lerp(normal_current, normal_ratio, normal_last, normal_final);
			}
		}
		quat normal_rotation;
		normal_rotation.set_rot(ORIGIN_NORMAL, normal_current);
		normal_rotation.normalize();
		rotation = normal_rotation * rotation;
	}
}

//--------------------------------------------------------------------------------
// Updates model position, and rotation.
//--------------------------------------------------------------------------------
void zz_model::update_transform (zz_time diff_time)
{
	save_prev_position_world();
	save_prev_rotation_world();
	
	unsigned int num_bones = bones.size();
	
	for (unsigned int bone_index = 0; bone_index < num_bones; ++bone_index) {
			bones[bone_index]->save_prev_position_world();
			bones[bone_index]->save_prev_rotation_world();
	}
	
	
	if (move_velocity != 0.0f || apply_gravity_onoff) {
		// update 
		// update gravity
		//force = vec3_null;
		accumulate_gravity(diff_time);
		velocity.z += force.z;

		// update move
		vec3 move_velocity_by_time;
		get_move_velocity_by_time(diff_time, move_velocity_by_time);
		velocity.x = move_velocity_by_time.x;
		velocity.y = move_velocity_by_time.y;
		position += velocity;
		
		if(apply_gravity_onoff)
		{
			float distancez;
			distancez = znzin->get_linear_gravity() *ZZ_TIME_TO_MSEC(diff_time);
		    position.z+=distancez;
			apply_gravity_onoff = false;
		}
		
		invalidate_transform();
	}
}

void zz_model::set_virtual_Transform (const vec4& pos)
{
	
	vec4 inv_pos;
	
	worldTM._14 = pos.x; worldTM._24 = pos.y; worldTM._34 = pos.z; worldTM._44 = 1.0f;
   	
	mult(inv_pos,worldTM,pos);
    world_inverseTM._14 = inv_pos.x; world_inverseTM._24 = inv_pos.y; world_inverseTM._34 = inv_pos.z; world_inverseTM._44 = 1.0f;

    tm_cache_flag |= TRANSFORM_CACHE_INDEX_WORLD_TM;

}

const vec3& zz_model::get_com_position_world (void)
{
	if (tm_cache_flag & TRANSFORM_CACHE_INDEX_COM_POSITION_WORLD) {
		return com_position_world;
	}

	tm_cache_flag |= TRANSFORM_CACHE_INDEX_COM_POSITION_WORLD;
	return get_com_bone()->get_position_world(com_position_world);
}

const vec3& zz_model::get_bone_position_world (int bone_index)
{
	vec3 position_world;
	
	return bones[bone_index]->get_position_world(position_world);
}

void zz_model::expand_scene_minmax (vec3& scene_min_out, vec3& scene_max_out)
{
	assert(valid_bvolume);
	assert(!children.empty());

	// expand from children
	zz_visible * child;
	const vec3 * child_minmax;
	for (zz_item_list::iterator it = items.begin(), it_end = items.end(); it != it_end; ++it) {
		child = (*it);
		child_minmax = child->get_minmax();

		assert(child->is_valid_bvolume());
		if (child_minmax[0].x < scene_min_out.x) scene_min_out.x = child_minmax[0].x;
		if (child_minmax[0].y < scene_min_out.y) scene_min_out.y = child_minmax[0].y;
		if (child_minmax[0].z < scene_min_out.z) scene_min_out.z = child_minmax[0].z;
		if (child_minmax[1].x > scene_max_out.x) scene_max_out.x = child_minmax[1].x;
		if (child_minmax[1].y > scene_max_out.y) scene_max_out.y = child_minmax[1].y;
		if (child_minmax[1].z > scene_max_out.z) scene_max_out.z = child_minmax[1].z;
		assert((scene_max_out.x - scene_min_out.x) < 100.f);
	}
}


void zz_model::expand_scene_minmax_height (vec3& scene_min_out, vec3& scene_max_out)
{
	
	assert(valid_bvolume);
	assert(!children.empty());

	// expand from children
	
	zz_visible * child;
	
	const char *name;
	const vec3 * child_minmax;
	for (zz_item_list::iterator it = items.begin(), it_end = items.end(); it != it_end; ++it) {
		child = (*it);
	  	name = child->get_name();
		if(!strstr(name,"eft"))
		{
			child_minmax = child->get_minmax();

			assert(child->is_valid_bvolume());
			if (child_minmax[0].x < scene_min_out.x) scene_min_out.x = child_minmax[0].x;
			if (child_minmax[0].y < scene_min_out.y) scene_min_out.y = child_minmax[0].y;
			if (child_minmax[0].z < scene_min_out.z) scene_min_out.z = child_minmax[0].z;
			if (child_minmax[1].x > scene_max_out.x) scene_max_out.x = child_minmax[1].x;
			if (child_minmax[1].y > scene_max_out.y) scene_max_out.y = child_minmax[1].y;
			if (child_minmax[1].z > scene_max_out.z) scene_max_out.z = child_minmax[1].z;
			assert((scene_max_out.x - scene_min_out.x) < 100.f);
		}
	}
}
void zz_model::gather_collidable (std::vector<zz_visible*>& collidable_holder)
{
	if (inscene)
		collidable_holder.push_back(this);

	if (items.empty()) return;

	for (zz_item_list::iterator it = items.begin(), it_end = items.end(); it != it_end; ++it) {
		(*it)->gather_collidable(collidable_holder);
	}
}

void zz_model::gather_visible (std::vector<zz_visible*>& visible_holder)
{
	if (inscene)
		visible_holder.push_back(this);
	
	if (items.empty()) return;
	
	for (zz_item_list::iterator it = items.begin(), it_end = items.end(); it != it_end; ++it) {
		(*it)->gather_visible(visible_holder);
	}
}

void zz_model::insert_scene ()
{
	inscene = true;

#if (0)
	if (parent_node != get_root()) {
		assert(((zz_visible*)(parent_node))->get_inscene() == inscene);
	}
#endif

	zz_assert(bv_type != ZZ_BV_NONE);

	update_bvolume(); // includes scene_refresh()
	scene_refresh();

	for (zz_bone_list::iterator bit = bones.begin(), bit_end = bones.end(); bit != bit_end; ++bit) {
		(*bit)->insert_scene_one();
	}
	for (zz_dummy_list::iterator dit = dummies.begin(), dit_end = dummies.end(); dit != dit_end; ++dit) {
		(*dit)->insert_scene_one();
	}
	for (zz_item_list::iterator it = items.begin(), it_end = items.end(); it != it_end; ++it) {
		(*it)->insert_scene();
	}
}

void zz_model::remove_scene ()
{
	inscene = false;

#if (0)
	if (parent_node != get_root()) {
		assert(((zz_visible*)(parent_node))->get_inscene() == inscene);
	}
#endif

	// no need to update_bvolume. we only need removing.
	scene_refresh();

	for (zz_bone_list::iterator bit = bones.begin(), bit_end = bones.end(); bit != bit_end; ++bit) {
		(*bit)->remove_scene_one();
	}
	for (zz_dummy_list::iterator dit = dummies.begin(), dit_end = dummies.end(); dit != dit_end; ++dit) {
		(*dit)->remove_scene_one();
	}
	for (zz_item_list::iterator it = items.begin(), it_end = items.end(); it != it_end; ++it) {
		(*it)->remove_scene();
	}
}

void zz_model::link_child (zz_dummy * dummy_child)
{
	zz_node::link_child(static_cast<zz_node*>(dummy_child));
	zz_assert(dummy_child->get_num_children() == 0);
}

void zz_model::link_child (zz_bone * bone_child)
{
	// make parent-child relation
	zz_node::link_child(static_cast<zz_node*>(bone_child));
	zz_assert(bone_child->get_num_children() == 0);
}

void zz_model::link_child (zz_node * node_child)
{
	// add item into itemlist if it is not a bone
	if (!node_child->is_a(ZZ_RUNTIME_TYPE(zz_bone))) {
		add_item(static_cast<zz_visible*>(node_child));
	}
    zz_animatable::link_child(node_child); // for now, zz_animatable does not have link_child
}

void zz_model::unlink_child (zz_node * node_child)
{
	// remove item from itemlist if it is not a bone
	if (!node_child->is_a(ZZ_RUNTIME_TYPE(zz_bone))) {
		del_item(static_cast<zz_visible*>(node_child));
	}
    zz_animatable::unlink_child(node_child); // for now, zz_animatable does not have unlink_child
}

void zz_model::update_time (bool recursive, zz_time diff_time)
{
	
	
//	recalc_dir();   //test

	if (!motion_controller.update(diff_time)) { // have some problem, stop motion
		// state transition. PLAY->STOP
		stop();
		// if it was playing, stop animation and apply this motion.
		if (apply_motion_stop()) {
			invalidate_transform();
		}
	}

	// invalidate infrustum
	infrustum = false;

	if (bv_type != ZZ_BV_NONE)
		update_bvolume();

	if (recursive && !items.empty()) {
		for (zz_item_list::iterator it = items.begin(), it_end = items.end(); it != it_end; ++it)
		{
			(*it)->update_time(recursive, diff_time);
		}
	}
}

void zz_model::set_glow_recursive (zz_glow_type glow_type_in, const vec3& color_in)
{
	set_glow(glow_type_in, color_in);
	
	if (items.empty()) return;

	for (zz_item_list::iterator it = items.begin(), it_end = items.end(); it != it_end; ++it)
	{
		//(*it)->set_glow_recursive(glow_type_in, color_in);
	    (*it)->set_glow(glow_type_in, color_in);
	}
}

void zz_model::input_collision_block(zz_animatable *block)
{ 
	collision_block = block;

}

zz_animatable* zz_model::output_collision_block()
{
	return collision_block;
}

void zz_model::reset_collision_block()
{
	collision_block = NULL;
}
