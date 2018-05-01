/** 
 * @file zz_camera_follow.cpp
 * @brief follow camera class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    18-feb-2003
 *
 * $Header: /engine/src/zz_camera_follow.cpp 58    06-12-29 4:22a Choo0219 $
 */

/* camera_follow has two mode:
1. look-mode
2. back-mode
*/

#include "zz_tier0.h"
#include <math.h>
#include "zz_algebra.h"
#include "zz_system.h"
#include "zz_model.h"
#include "zz_morpher.h"
#include "zz_terrain_block.h"
#include "zz_profiler.h"
#include "zz_camera_follow.h"
#include "zz_ocean_block.h"
#include "zz_particle_emitter.h"
#include "zz_trail.h"
#include "zz_sky.h"
#include "zz_bvolume.h"
#include "zz_sfx.h"
#include "zz_interface.h"

ZZ_IMPLEMENT_DYNCREATE(zz_camera_follow, zz_camera)

// some constants
const float MIN_DISTANCE_THRESHOLD = 10.0f*ZZ_SCALE_IN;
const float MAX_DISTANCE_THRESHOLD = 100.0f*ZZ_SCALE_IN;
const float DEFAULT_HEIGHT_ADDED = 130.0f*ZZ_SCALE_IN;
const float MIN_PITCH_RADIAN = -60.0f*ZZ_TO_RAD;
const float MAX_PITCH_RADIAN = 80.0f*ZZ_TO_RAD;
const float PITCH_RADIAN_RANGE = MAX_PITCH_RADIAN - MIN_PITCH_RADIAN;
const vec3 DEFAULT_TARGET_POS(0, 0, 0);
const vec3 INITIAL_CAMERA_POS_AT_ATTACH(0, -100*ZZ_SCALE_IN, 1000*ZZ_SCALE_IN);
const float DEFAULT_DISTANCE_RANGE_MIN = 100.0f*ZZ_SCALE_IN;
const float DEFAULT_DISTANCE_RANGE_MAX = 5000.0f*ZZ_SCALE_IN;

// constant epsilon values
const float EPSILON_DISTANCE = 1.0f*ZZ_SCALE_IN;
const float EPSILON_PITCH= .001f;
const float EPSILON_YAW = .001f;
const float EPSILON_TARGETPOS = 1.0f*ZZ_SCALE_IN;
const float EPSILON_TARGETDIR = .00001f;

zz_camera_follow::zz_camera_follow () :
	target_(NULL),
	now_following_(true),
	follow_mode_(BACK_MODE),
	height_added_(DEFAULT_HEIGHT_ADDED),
	is_dirty_(true),
	newly_attached_(true),
	shake_lifetime(0),
	shake_min(vec3_null),
	shake_max(vec3_null),
	distance_range_min(DEFAULT_DISTANCE_RANGE_MIN),
	distance_range_max(DEFAULT_DISTANCE_RANGE_MAX)
{
	current_.yaw = 0.0f;
	current_.pitch = 0.0f;
	current_.distance = 1000.0f*ZZ_SCALE_IN;
	current_.camera_dir = vec3_null;
	current_.camera_pos = vec3_null;
	current_.target_dir = vec3_null;
	current_.target_pos = vec3_null;

	last_ = current_;
	final_ = current_;

	time_weight_ = 0;
	collision_level = ZZ_CL_SPHERE;
    
	camera_effect_onoff = false;
    final_time = 0.0f;
	accumulate_time = 0.0f;

}

zz_camera_follow::~zz_camera_follow ()
{
}

//--------------------------------------------------------------------------------
// set height to be added to character's center
//--------------------------------------------------------------------------------
void zz_camera_follow::set_height_added (float height)
{
	height_added_ = height;
	is_dirty_ = true;
}

//--------------------------------------------------------------------------------
// get target position
//--------------------------------------------------------------------------------
void zz_camera_follow::get_target_pos (vec3& pos_out)
{
	if (!target_) {
		pos_out = DEFAULT_TARGET_POS;
		pos_out.z += height_added_;
		return;
	}

#if (1)
	target_->get_position_world(pos_out);
	pos_out.z += height_added_;
#else
	target_->get_com_position_world(pos_out);;
#endif
}

//--------------------------------------------------------------------------------
// attach target
//--------------------------------------------------------------------------------
bool zz_camera_follow::attach_target (zz_model * target_vis)
{
	if (!target_vis) {
		ZZ_LOG("camera_follow: attach_target() failed. target not found\n");
		return false;
	}

	if (target_ == target_vis) return true;

	target_ = target_vis;
	
	get_target_pos(final_.target_pos);

	look_at(final_.target_pos + INITIAL_CAMERA_POS_AT_ATTACH, final_.target_pos, vec3(0, 0, 1));
	newly_attached_ = true;
	final_.camera_pos = get_eye();
	last_.camera_pos = final_.camera_pos;
	last_.target_pos = final_.target_pos;
	is_dirty_ = true;
	return true;
}

//--------------------------------------------------------------------------------
// detach target
//--------------------------------------------------------------------------------
bool zz_camera_follow::detach_target (void)
{
	if (!target_) {
		ZZ_LOG("camera_follow: detach_target() failed. no target\n");
		return false;
	}
	target_ = NULL;
	is_dirty_ = true;
	return true;
}

//--------------------------------------------------------------------------------
// apply yaw
//--------------------------------------------------------------------------------
void zz_camera_follow::apply_yaw (vec3& new_camera_dir_out, float follow_yaw_in, const vec3& target_dir_in)
{
	mat3 rotate_around_zaxis(mat3_id);
	rotate_around_zaxis.set_rot(follow_yaw_in, vec3(0, 0, 1));
	// new_camera_dir = rotate_arount_zaxis * target_dir
	mult(new_camera_dir_out, rotate_around_zaxis, target_dir_in);
}

//--------------------------------------------------------------------------------
// apply pitch
//--------------------------------------------------------------------------------
const vec3& zz_camera_follow::apply_pitch (vec3& new_camera_dir_inout, float follow_pitch_in)
{
	// [MIN_PITCH_RADIAN, MIN_PITCH_RADIAN + PITCH_RADIAN_RANGE]
	float pitch_angle = (follow_pitch_in * PITCH_RADIAN_RANGE + MIN_PITCH_RADIAN);
	
	new_camera_dir_inout.normalize();
	float cosf_pitch_angle = cosf(pitch_angle);
	new_camera_dir_inout.x *= cosf_pitch_angle;
	new_camera_dir_inout.y *= cosf_pitch_angle;
	new_camera_dir_inout.z = -sinf(pitch_angle);
	//ZZ_LOG("camera_follow: new_camera_dir.normalize() = %f\n", new_camera_dir_out.sq_norm());
	return new_camera_dir_inout;
}

//--------------------------------------------------------------------------------
// get target look at
//--------------------------------------------------------------------------------
bool zz_camera_follow::get_target_dir (vec3& dir)
{
	if (!target_) {
		dir.set(0, 1, 0);
		return false;
	}
	dir = target_->get_look_at();
	dir.normalize();
	return true;
}

//--------------------------------------------------------------------------------
// calculate yaw for follow mode
//--------------------------------------------------------------------------------
float zz_camera_follow::calc_yaw ()
{
	// we should move camera and target
	vec3 displacement = final_.target_pos - last_.target_pos;

	final_.camera_pos = get_eye() + displacement;
	
	// camera to target
	final_.camera_dir = final_.target_pos - final_.camera_pos;	

	// ignore pitch
	final_.camera_dir.z = 0;
	final_.camera_dir.normalize();

	// update target_dir
	get_target_dir(final_.target_dir);

	float dot_val = dot(final_.target_dir, final_.camera_dir);

	dot_val = ZZ_MIN(dot_val, 1.0f);
	dot_val = ZZ_MAX(dot_val, -1.0f);

	float new_yaw = static_cast<float>(acos(dot_val));
	vec3 cross_vector;
	cross(cross_vector, final_.target_dir, final_.camera_dir);

	if (cross_vector.z < 0) // invert
		new_yaw = -new_yaw;
	return new_yaw;
}

//--------------------------------------------------------------------------------
// update for back-mode
//--------------------------------------------------------------------------------
bool zz_camera_follow::update_backmode (void)
{	
	// apply yaw factor to target_dir
	apply_yaw(final_.camera_dir, current_.yaw, final_.target_dir);

	// camera_dir interpolation. *pitch* was not yet applied to the camera_dir.
	final_.camera_dir = last_.camera_dir + time_weight_*(final_.camera_dir - last_.camera_dir);
	final_.camera_dir.normalize();

	// use not-pitched target direction only for backmode
	last_.camera_dir = final_.camera_dir;

	// apply pitch.
	// CAUTION: must call apply_pitch() after camera_dir interpolation
	apply_pitch(final_.camera_dir, current_.pitch);

	// apply distance
	final_.camera_pos = final_.target_pos - current_.distance * final_.camera_dir;
	
	// recalc camera & target position
	final_.camera_pos = last_.camera_pos + time_weight_*(final_.camera_pos - last_.camera_pos);
	final_.target_pos = last_.target_pos + time_weight_*(final_.target_pos - last_.target_pos);
	
	look_at(final_.camera_pos, final_.target_pos, vec3(0, 0, 1));

	// save last_target_pos for look_mode
	last_.target_pos = final_.target_pos;
	last_.camera_pos = final_.camera_pos;
	return true;
}

//--------------------------------------------------------------------------------
// follow the target as near as possible
//--------------------------------------------------------------------------------
bool zz_camera_follow::update_lookmode (float follow_yaw_last)
{	
	bool move_camera;
	// distance between last_target and current target position
	float target_diff = last_.target_pos.distance(final_.target_pos);

	// distance between camera and current target
	float target_dist = (target_) ? distance(target_) : get_position().distance(vec3_null);

	// update by camera-target difference
	// 1. we start moving camera position if the target_diff reaches a certain amount.
	// 2. we stop moving camera position if the target_diff is too small.
	if (now_following_) { // if we started moving camera position.
		if (target_diff > MIN_DISTANCE_THRESHOLD) { // now moving
			move_camera = true; // should move
		}
		else { // we are already very close to each other
			now_following_ = false; // quit following mode
			move_camera = true; // but keep in camera moving state for now
		}
	}
	else if (target_diff > MAX_DISTANCE_THRESHOLD) { // we should start moving the camera position now.
		now_following_ = true;
		move_camera = true;
	}
	else { // we do not need to move the camera position
		move_camera = false;
	}

	// force moving the camera for test
	if (!znzin->get_use_time_weight()) move_camera = true;

	if (move_camera) { // if we should move camera and target
		vec3 displacement;
		float damp = (target_diff / MAX_DISTANCE_THRESHOLD);
		// get target displacement by last and current
		displacement = .5f*time_weight_*(final_.target_pos - last_.target_pos); // .5f to make slower than 1.0f
		// update last_target_pos
		last_.target_pos += displacement;
		move(displacement);
	}
	//camera_dir = target_->get_position() - this->get_eye(); // update camera_dir for later use in update()	
	final_.camera_dir = last_.target_pos - get_eye();

	// recalc yaw, because eye position was changed in update_lookmode()
	// and, we need the difference between yaw and yaw_last
	float yaw_diff = final_.yaw - last_.yaw;
	current_.yaw = calc_yaw();
	final_.yaw = current_.yaw + yaw_diff;
	// and apply new_value
	current_.yaw += time_weight_*(final_.yaw - current_.yaw);

	apply_yaw(final_.camera_dir, current_.yaw, final_.target_dir);

	// save last_camera_dir(non-pitched) for back-mode
	last_.camera_dir = final_.camera_dir;

	// apply pitch
	apply_pitch(final_.camera_dir, current_.pitch);

	final_.camera_pos = last_.target_pos - current_.distance * final_.camera_dir;

	look_at(final_.camera_pos, last_.target_pos, vec3(0, 0, 1));
	last_.camera_pos = final_.camera_pos; // for back-mode

	return move_camera;
}

//--------------------------------------------------------------------------------
// set to follow mode
//--------------------------------------------------------------------------------
void zz_camera_follow::set_follow_mode (zz_camera_follow::zz_follow_mode mode)
{
	if (mode == zz_camera_follow::BACK_MODE) { 
		if (follow_mode_ == zz_camera_follow::LOOK_MODE) { // to back-mode
			final_.yaw = 0.0f; // force yaw = 0
		}
	}
	follow_mode_ = mode;
	is_dirty_ = true;
}	

//--------------------------------------------------------------------------------
// adjust camera properties
//--------------------------------------------------------------------------------
void zz_camera_follow::interpolate_camera (zz_time diff_time)
{
	const float TIME_LIMIT = 1.0f / 200.0f; // for done in 200 milli second
	const float NEW_TIME_WEIGHT = 1.0f;
	const float MAX_TIME_WEIGHT = 1.0f;
	const float MIN_TIME_WEIGHT = 0.0f;
	const float MAX_DIFF_TIME_MSEC = 500.0f; // 1000 = 1 second

	float diff_time_millisecond = (float)ZZ_TIME_TO_MSEC(diff_time);
	bool too_slow = (diff_time_millisecond > MAX_DIFF_TIME_MSEC );

	if (newly_attached_ || !znzin->get_use_time_weight() || too_slow) {
		time_weight_ = NEW_TIME_WEIGHT;
		newly_attached_ = false;
	}
	else {
		// calculate time_weight_
		time_weight_ = diff_time_millisecond * TIME_LIMIT;

		// for too slow speed, apply time_weight_ limit
		time_weight_ = ZZ_MIN(time_weight_, MAX_TIME_WEIGHT);
		time_weight_ = ZZ_MAX(time_weight_, MIN_TIME_WEIGHT);
	}

	//ZZ_LOG("camera_follow: time_weight = %10.5f\n", time_weight_);

	//final_.pitch range = [0,1] <= MIN_PITCH_RADIAN <-> (MIN_PITCH_RADIAN+pitch_range)
	final_.pitch = ZZ_MAX(final_.pitch, .0f);
	final_.pitch = ZZ_MIN(final_.pitch, 1.0f);
	current_.pitch = ZZ_MAX(current_.pitch, .0f);
	current_.pitch = ZZ_MIN(current_.pitch, 1.0f);
	
	final_.distance = ZZ_MAX(final_.distance, distance_range_min);
	final_.distance = ZZ_MIN(final_.distance, distance_range_max);

	current_.distance += time_weight_*(final_.distance - current_.distance);
	current_.pitch += time_weight_*(final_.pitch - current_.pitch);
	current_.yaw += time_weight_*(final_.yaw - current_.yaw);
}

//--------------------------------------------------------------------------------
// update
//--------------------------------------------------------------------------------
void zz_camera_follow::update_time (bool recursive, zz_time diff_time)
{
	update_speed(diff_time); // update camera speed

	// save last properties
	last_.pitch = current_.pitch;
	last_.yaw = current_.yaw;
	last_.distance = current_.distance;
	
	// distance between last_target and current target position
	float target_pos_diff = 0;
	float camera_dir_diff = 0;
	
	if (!target_) return; // if no target was set, do nothing.

	// retrieve target position, and save into target_pos
	get_target_pos(final_.target_pos);

	target_pos_diff = last_.target_pos.distance(final_.target_pos);
	if (target_pos_diff > EPSILON_TARGETPOS) is_dirty_ = true;
	if (follow_mode_ == BACK_MODE) is_dirty_ = true;

	if (shake_lifetime != 0) is_dirty_ = true;

	// if not changed any property, then do nothing.
	if (!is_dirty_) return;

	// adjust camera properties(follow_pitch/yaw/distance)
	if(!camera_effect_onoff)
	interpolate_camera(diff_time);
	else
    interpolate_camera_effect(diff_time);

	// set target direction
	get_target_dir(final_.target_dir);

	// update lookmode|backmode
	if (follow_mode_ == LOOK_MODE) {
		update_lookmode(last_.yaw);
	}
	else { // BACK_MODE
		update_backmode();
	}

	// apply shakeed animation
	apply_shake(diff_time);
}
	
void zz_camera_follow::update_camera_collision()	
{	
	
	// get adjusted distance value by camera to target ray.
	float adjusted_distance;

	bool contacted = false;

	get_target_pos(final_.target_pos);
	
	if (collision_level != ZZ_CL_NONE) {
		contacted = get_distance_by_contact_point(adjusted_distance);
	}

	if (contacted) {
		current_.distance = adjusted_distance;
		// re-adjust by changed follow_distance_current
		final_.camera_pos = last_.target_pos - current_.distance * final_.camera_dir;
		look_at(final_.camera_pos, last_.target_pos, vec3(0, 0, 1));
		last_.camera_pos = final_.camera_pos; // for back-mode
	}

	// if we have almost done our job, then do not update anymore.
	if ((FABS(current_.distance - last_.distance) < EPSILON_DISTANCE) &&
        (FABS(current_.pitch - last_.pitch) < EPSILON_PITCH) &&
		(FABS(current_.yaw - last_.yaw) < EPSILON_YAW))
	{
		// onli for look mode
		// we have almost done out job, take a rest.
		is_dirty_ = false; // not  to updated again
	    if(camera_effect_onoff)
		stop_camera_effect();
	}
}

//--------------------------------------------------------------------------------
// get yaw by eye for follow mode
//--------------------------------------------------------------------------------
float zz_camera_follow::get_yaw_by_eye (const vec3& eye_pos)
{
	// camera to target
	final_.camera_dir = final_.target_pos - eye_pos;	
	// ignore pitch
	final_.camera_dir.z = 0;
	final_.camera_dir.normalize();

	// update target_dir
	get_target_dir(final_.target_dir);

	float dot_val = dot(final_.target_dir, final_.camera_dir);
	dot_val = ZZ_MIN(dot_val, 1.0f);
	dot_val = ZZ_MAX(dot_val, -1.0f);
	float new_yaw = static_cast<float>(acos(dot_val));
	vec3 cross_vector;
	cross(cross_vector, final_.target_dir, final_.camera_dir);

	if (cross_vector.z < 0) // invert
		new_yaw = -new_yaw;
	return new_yaw;
}

//--------------------------------------------------------------------------------
// get pitch by eye for follow mode
//--------------------------------------------------------------------------------
float zz_camera_follow::get_pitch_by_eye (const vec3& eye_pos)
{
	// [MIN_PITCH_RADIAN, MIN_PITCH_RADIAN + PITCH_RADIAN_RANGE]
	vec3 eye_pos_z = eye_pos;
	eye_pos_z.z = final_.target_pos.z; // projected eye on the [target_pos.z] plane.

	float pitch_angle; // in radian
	pitch_angle = cosf(eye_pos_z.distance(final_.target_pos) / eye_pos.distance(final_.target_pos));
	if (eye_pos.z < eye_pos_z.z) {
		// invert angle
		pitch_angle = -pitch_angle;
	}

	float new_pitch = (pitch_angle - MIN_PITCH_RADIAN) / PITCH_RADIAN_RANGE;
	return new_pitch;
}

//--------------------------------------------------------------------------------
// get distance by eye for follow mode
//--------------------------------------------------------------------------------
float zz_camera_follow::get_distance_by_eye (const vec3& eye_pos)
{
	// assumes target_pos already be set
	return eye_pos.distance(final_.target_pos);
}

//--------------------------------------------------------------------------------
// set yaw
//--------------------------------------------------------------------------------
void zz_camera_follow::set_yaw (float new_yaw)
{
	final_.yaw = new_yaw;
	is_dirty_ = true;
}

//--------------------------------------------------------------------------------
// set pitch
//--------------------------------------------------------------------------------
void zz_camera_follow::set_pitch (float new_pitch)
{
	final_.pitch = new_pitch;
	is_dirty_ = true;
}

//--------------------------------------------------------------------------------
// set distance
//--------------------------------------------------------------------------------
void zz_camera_follow::set_distance (float new_dist)
{
	final_.distance = new_dist;
	is_dirty_ = true;
}

//--------------------------------------------------------------------------------
// get yaw
//--------------------------------------------------------------------------------
float zz_camera_follow::get_yaw ()
{
	return final_.yaw;
}

//--------------------------------------------------------------------------------
// get pitch
//--------------------------------------------------------------------------------
float zz_camera_follow::get_pitch ()
{
	return final_.pitch;
}

//--------------------------------------------------------------------------------
// get_distance
//--------------------------------------------------------------------------------
float zz_camera_follow::get_distance ()
{
	return final_.distance;
}


//--------------------------------------------------------------------------------
// get distance from camera to contact point with terrains or buildings
//--------------------------------------------------------------------------------
bool zz_camera_follow::get_distance_by_contact_point ( float& adjusted_distance )
{
/*
                  follow_distance
                        adjusted_distance
   eye +-------------+----------------------+ target_pos
       |             |   theta ( __________/
       |             |__________/  
	   | ___________/ \      contact_min_distance
	   +/              \
   eye_bottom           contact_point
                 target_to_eye_bottom
*/
	// 1. collect neiborhood objects which are collidable with camera
	// 2. shoot ray(target_pos->camera) to objects except for model node
	// 3. the objects must be a terrain block or building object
	// 4. find the nearest contact point
	// 5. adjust distance by contact point
	static std::vector<zz_visible*> vis(200);
	vis.clear();

	update_frustum(0); // update frustum with no margin to get tight *frustum*. default frustum has loose frustum.

	vec3 range_minmax[2];
	range_minmax[0] = -final_.distance + final_.target_pos;
	range_minmax[1] = final_.distance + final_.target_pos;

	znzin->scene.collect_by_minmax(vis, range_minmax, false /* skip_no_collision */);

	vec3 checkpoint[3]; // currently use three checkpoint(bottom left, bottom right and bottom center of viewfrustum
	checkpoint[0] = vec3(frustum_.p[0]); // bottom left
	checkpoint[1] = vec3(frustum_.p[1]); // bottom right
	checkpoint[2] = .5f*(vec3(frustum_.p[0]) + vec3(frustum_.p[1])); // bottom center

	vec3 dir[3];
	dir[0] = checkpoint[0] - final_.target_pos;
	dir[1] = checkpoint[1] - final_.target_pos;
	dir[2] = checkpoint[2] - final_.target_pos;

	
	vec3 target_to_checkpoint[3];;
	target_to_checkpoint[0] = dir[0];
	target_to_checkpoint[1] = dir[1];
	target_to_checkpoint[2] = dir[2];
	
	dir[0].normalize();
	dir[1].normalize();
	dir[2].normalize();

		
	vec3 contact_point;
	vec3 contact_normal;
	float contact_distance, contact_min_distance = current_.distance;
	bool contacted = false;
	const float minimum_constance_distance = 10.0f*ZZ_SCALE_IN; // heuristic constant value
	int num_vis = (int)vis.size();
	int check_index;
	int check_where = -1;
	zz_visible * contact_what;
    vec3 plane_normal;
	vec3 plane_position;
	float plane_d;
    mat4 view_matrix;

	view_matrix = modelview_matrix_.inverse();
	
	plane_normal.x = -view_matrix._13;
	plane_normal.y = -view_matrix._23;
	plane_normal.z = -view_matrix._33;
    
    plane_position.x = view_matrix._14;
	plane_position.y = view_matrix._24;
	plane_position.z = view_matrix._34;

    plane_d = -dot(plane_normal,plane_position);
     
    // set under_the_sea
	under_the_sea = false;
	znzin->sfxs->clear_sfx();
	const zz_bounding_sphere * sphere;
	bool on_the_terrain = false;
	zz_ocean_block * ocean;
	zz_visible * v;

	for (int i = 0; i < num_vis; i++) {
		if (0) {
			// for now, consider only terrain block objects
			if (!vis[i]->is_a(ZZ_RUNTIME_TYPE(zz_terrain_block))) continue;
		}
		else {
			v = vis[i];
         
		
		   
			// check if this camera is under the sea
			if (IS_A(v, zz_ocean_block)) {
				if (under_the_sea) { // already under the sea
					continue;
				}
				ocean = static_cast<zz_ocean_block*>(v);
				
				if (ocean->get_intersection_ray(final_.camera_pos, vec3(0, 0, 1.0f), contact_point, contact_normal)) {
					// this is under the sea
					znzin->sfxs->push_sfx(zz_manager_sfx::OCEAN);
					under_the_sea = true;
				}
				continue;
			}

			if(v->is_a(ZZ_RUNTIME_TYPE(zz_model)))
			{
				if(!((zz_model*)v)->camera_collision_onoff)
					continue;
			}
			else
			{
				if (v->is_descendant_of_type(ZZ_RUNTIME_TYPE(zz_model)) ||
					v->is_a(ZZ_RUNTIME_TYPE(zz_particle_emitter)) ||
					v->is_a(ZZ_RUNTIME_TYPE(zz_trail)) ||
					v->is_a(ZZ_RUNTIME_TYPE(zz_morpher)) ||
					v->is_a(ZZ_RUNTIME_TYPE(zz_sky)) ||
					(v->get_collision_level() == ZZ_CL_NONE) ||
					(ZZ_IS_NOTCAMERACOLLISION(v->get_collision_level())))
				    {
						continue;
					}

			}
			
			assert(v->get_bvolume());

		
			const float max_seethru_sphere_radius = 500.0f*ZZ_SCALE_IN; // make transparent if raidus is less than this.
			sphere = v->get_bvolume()->get_sphere();
			if (sphere && (sphere->radius < max_seethru_sphere_radius)) { // too small
				continue;
			}
				
			// skip if seethru is not one.
			if (v->get_seethru() < 1.0f) {
				continue;
			}

			if (IS_A(v, zz_terrain_block)) {
				on_the_terrain = true;
			}

			if (0 && !v->is_a(ZZ_RUNTIME_TYPE(zz_terrain_block))) { // if we want to skip too close objects.
				zz_bvolume * target_bv = target_->get_bvolume();
				if (target_bv) {
					const zz_bounding_sphere * sp = target_bv->get_sphere();
					if (sp) {
						if (v->test_intersection_sphere(*sp)) {
							continue; // extract objects which collide with target model.
						}
					}
				}
			}
		}

		for (check_index = 0; check_index < 3; check_index++) {
			if (vis[i]->get_intersection_ray(final_.target_pos, dir[check_index], contact_point, contact_normal)) {
				contact_distance =  current_.distance-(dot(contact_point,plane_normal)+plane_d);
				if ((contact_distance > minimum_constance_distance) && (contact_distance < contact_min_distance)) {
					contacted = true;
					check_where = check_index;
					contact_min_distance = contact_distance;
					contact_what = vis[i];
				}
			}
		}
	}

	if (on_the_terrain && under_the_sea) {
		under_the_sea = false; // ignore if on the terrain
	}

	if (!contacted) { // no node intersected
		adjusted_distance = current_.distance; // do nothing
		return false;
	}
	assert(check_where >= 0);
	const float margin_min_distance = 0.0f*ZZ_SCALE_IN;   //test 1-17
	float target_to_checkpoint_distance = target_to_checkpoint[check_where].norm();
	contact_min_distance -= margin_min_distance;
	adjusted_distance = ZZ_MAX(minimum_constance_distance-margin_min_distance, contact_min_distance);
	return true; // contacted
}

bool zz_camera_follow::apply_shake (zz_time diff_time)
{
	if (shake_lifetime == 0) return false;

	if (shake_lifetime <= diff_time) {
		shake_lifetime = 0; // do not modify
		return true;
	}
	else {
		shake_lifetime -= diff_time;
	}

	vec3 shakeed_cam_pos, shakeed_target_pos;

	vec3 shake_min_shrinked(shake_min), shake_max_shrinked(shake_max);

	shakeed_cam_pos = random_number(final_.camera_pos + shake_min_shrinked, final_.camera_pos + shake_max_shrinked);
	shakeed_target_pos = random_number(last_.target_pos + shake_min_shrinked, last_.target_pos + shake_max_shrinked);

	look_at(shakeed_cam_pos, shakeed_target_pos, vec3(0, 0, 1));

	return true;
}

void zz_camera_follow::set_shake (zz_time lifetime_in, const vec3& min_in, const vec3& max_in)
{
	shake_lifetime = lifetime_in;
	shake_min = min_in;
	shake_max = max_in;
	is_dirty_ = true;
}

void zz_camera_follow::set_distance_range (float min_in, float max_in)
{
	distance_range_min = min_in;
	distance_range_max = max_in;
}


bool zz_camera_follow::get_camera_effect_onoff()
{
	return camera_effect_onoff; 
}

void zz_camera_follow::interpolate_camera_effect(zz_time diff_time)
{  
	float d_t; 
	const float MAX_TIME_WEIGHT = 1.0f;
	const float MIN_TIME_WEIGHT = 0.0f;
	
	
	d_t = 0.001f*ZZ_TIME_TO_MSEC(diff_time);
   	time_weight_ = d_t/(final_time-accumulate_time);    
    accumulate_time+=d_t; 
	
	time_weight_ = ZZ_MIN(time_weight_, MAX_TIME_WEIGHT);
	time_weight_ = ZZ_MAX(time_weight_, MIN_TIME_WEIGHT);
	
//	final_.pitch = ZZ_MAX(final_.pitch, .0f);
//	final_.pitch = ZZ_MIN(final_.pitch, 1.0f);
//	current_.pitch = ZZ_MAX(current_.pitch, .0f);
//	current_.pitch = ZZ_MIN(current_.pitch, 1.0f);
	
//	final_.distance = ZZ_MAX(final_.distance, distance_range_min);
//	final_.distance = ZZ_MIN(final_.distance, distance_range_max);

	current_.distance += time_weight_*(final_.distance - current_.distance);
	current_.pitch += time_weight_*(final_.pitch - current_.pitch);
	current_.yaw += time_weight_*(final_.yaw - current_.yaw);
	

	if(accumulate_time >= final_time)
	{
		stop_camera_effect();
	    time_weight_ = 1.0f;
	}
}

void zz_camera_follow::play_camera_effect(float yaw, float pitch, float length, float time)
{
    final_.yaw = yaw;
	final_.pitch = pitch;
    final_.distance = length;
    final_time = time;   
	camera_effect_onoff = true;
	is_dirty_ = true;
   
}

void zz_camera_follow::stop_camera_effect()
{
	accumulate_time = 0.0f;
	final_time = 0.0f;
	camera_effect_onoff = false;
}