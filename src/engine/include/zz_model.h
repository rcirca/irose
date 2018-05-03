/** 
 * @file zz_model.h
 * @brief model class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    11-jun-2002
 *
 * $Header: /engine/include/zz_model.h 46    06-12-11 10:25a Choo0219 $
 */

#ifndef __ZZ_MODEL_H__
#define __ZZ_MODEL_H__

#ifndef __ZZ_ANIMATABLE_H__
#include "zz_animatable.h"
#endif

#ifndef __ZZ_MESH_TOOL_H__
#include "zz_mesh_tool.h"
#endif

#ifndef __ZZ_BONE_H__
#include "zz_bone.h"
#endif

#ifndef __ZZ_SKELETON_H__
#include "zz_skeleton.h"
#endif

#define ZZ_BLINK_TEST

typedef std::vector<zz_dummy *> zz_dummy_list;
typedef std::vector<zz_visible *> zz_item_list;

//--------------------------------------------------------------------------------
class zz_model : public zz_animatable {
private:
	vec3 _euler_angle_last; // last(current) euler_angle. actually applied angle
    bool blink_close_mode;
   

#ifdef ZZ_BLINK_TEST
	uint16 blink_current_delay_;
	uint16 blink_open_delay_;
	uint16 blink_close_delay_;
	enum {
		BLINK_CLOSE = 0,
		BLINK_OPEN = 1
	};
	uint16 blink_mode_;

	void init_blink_ ();
#endif

protected:

	int skip_frames; // skip frame for distant models
	vec3 euler_angle; // we use x as yaw, y as pitch, z as roll
	vec3 dir; // model direction
	vec3 normal_final; // final model normal for sticking on the terrain
	vec3 normal_last; // last interpolated normal
	vec3 normal_current; // current interpolated normal
	float normal_ratio; // normal interpolated ratio

	bool dir_left_first_; // if set true, check left direction first. default is true

	const static vec3 pivot_turn_axis;
	const static vec3 pivot_front_axis;

	// model consists of many bones
	// bone consists of bones, dummies, and items
	// dummy consts of items
	zz_bone_list bones; // bone list
	zz_dummy_list dummies; // dummies are static
	zz_item_list items; // in fact, these visibles are linked to a bone of this model.

	zz_bone * com_bone;

	zz_skeleton * skeleton;

	zz_animatable *collision_block;   //Ãæµ¹¿ë  //test 12_14
    
	bool apply_gravity_onoff; 
	
	float move_velocity; // moving velocity. positive to the front direction

	float camtarget_distance_square; // distance square from current camera target

	const quat& get_turned_initial_rotation_incremental (quat& rotation_out, zz_time time_diff_in);
	const quat& get_turned_initial_rotation (quat& rotation_out, const vec3& axis, float angle_radian);

	void recalc_dir (void);

	// sub-function of reset_bvolume()
	virtual void get_local_center (vec3& local_center_out, const vec3& mesh_min, const vec3& mesh_max);

	void update_blink (zz_time diff_time);

	// get children scene_min/max in world coordsys recursively
	virtual void expand_scene_minmax (vec3& scene_min_out, vec3& scene_max_out);
    void expand_scene_minmax_height (vec3& scene_min_out, vec3& scene_max_out);

public:

	bool camera_collision_onoff;
	bool fored_motion_blend0ff;
	
	zz_model(void);
	virtual ~zz_model(void);

	void set_blink_close_mode(bool true_false);
	bool get_blink_close_mode();

	static void deallocate_bones (zz_bone_list& bone_list);
	static void allocate_bones (zz_bone_list& bone_list, int num_bones);
	static void deallocate_dummies (zz_dummy_list& dummy_list);
	static void allocate_dummies (zz_dummy_list& dummy_list, int num_dummies);

	void bind_channels ();
	void attach_motion (zz_motion * motion_to_attach);
	
	
	void set_apply_gravity()
	{
		apply_gravity_onoff = true;
    }
	
	// attach skeleton and set built-in bone nodes
	bool attach_skeleton (zz_skeleton * skel);

	// At now, always returns true
	bool detach_skeleton ()
	{
		skeleton = NULL;
		return true;
	}

	virtual void render (bool recursive = false);

	uint32 get_num_bones (void) const
	{
		return bones.size();
	}

	zz_bone * get_bone (uint32 bone_index)
	{
		assert(bone_index < bones.size());
		return bones[bone_index];
	}

	zz_dummy * get_dummy (uint32 dummy_index)
	{
		assert(dummy_index < dummies.size());
		return dummies[dummy_index];
	}

	// get worldTM of the COM(center of mass = root bone = pelvis)
	const mat4& get_com_worldTM (mat4& com_worldTM)
	{
		// com_worldTM = get_worldTM * bones[0]->get_localTM()
		return mult(com_worldTM, get_worldTM(), bones[0]->get_localTM());
	}
	
	// get worldTM of the COM(center of mass = root bone = pelvis)
	const mat4& get_bone_worldTM (mat4& bone_worldTM,int bone_index)
	{
		// com_worldTM = get_worldTM * bones[0]->get_localTM()
		return bones[bone_index]->get_boneTM();
	}
	
	
	
	zz_bone * get_com_bone (void);

	const quat& get_com_rotation_world (void);

	const vec3& get_com_position_world (void);
    const vec3& get_bone_position_world (int bone_index);
	
	const vec3& get_com_position_local (void)
	{
		return get_com_bone()->get_position();
	}

	// TODO: apply_motion_by_frame()
	bool apply_motion (void);
    bool apply_motion_stop (void);

	// turn *relative* direction
	// TODEL: instead, use *_direction() call
	void turn (float angle); // updates yaw value of euler_angle

	// set *absolute* direction
	void  set_direction (float angle, bool immediate = false);
	float get_direction (void) const
	{
		return euler_angle.x;
	}

	// set/get move velocity
	// velocity = distance(cm) of the model per second.
	void  set_move_velocity (float velocity_in);
	float get_move_velocity (void) const;

	// to get/set look_at direction by positioning target
	// internally model's y-axis is front direction
	// min_degree : degree
	// return : true if reached to desired angle
	bool set_look_at (const vec3& there, bool immediate = false);
	vec3 get_look_at (void) const { return dir; } // get front vector

	virtual unsigned long release (void);
	
	// vis-ray intersection test and get the contact point
	virtual bool get_intersection_ray (const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, zz_mesh_tool::zz_select_method select_method);

	virtual void update_animation (bool recursive, zz_time diff_time);

	void stop_rotation()
	{
		euler_angle.x = _euler_angle_last.x;
	}


	// update transfrom. usually, done after rendering
	virtual void update_transform (zz_time diff_time);

	vec3& get_dummy_position_world (vec3& world_pos, int dummy_index);

	virtual void set_rotation (const quat& rotation_to_set);

	virtual float get_height (void); // get total height with children visibles

	void set_virtual_Transform (const vec4& pos);
	
	// get direction vector, if has motion
	// if does not have motion, return pivot_front_axis
	// this does not guarantee normalized vector.
	const vec3& get_direction_vector () const
	{
		return this->dir;
	}

	bool get_dir_left_first () const
	{
		return dir_left_first_;
	}

	void flip_dir_left_first ()
	{
		dir_left_first_ = !dir_left_first_;
	}

	float get_camtarget_distance_square () const;

	void set_camtarget_distance_square (float camtarget_distance_square_in);

	// link to dummy node
	void link_dummy (zz_visible * vis, uint32 dummy_index);
	// no unlink_dummy()
	// instead, use dummy->unlink_child

	virtual void link_child (zz_node * node_child);
	virtual void unlink_child (zz_node * node_child);

	// overloading for bone
	void link_child (zz_bone * bone_child);
	void link_child (zz_dummy * dummy_child);

	uint32 get_num_dummies () const
	{
		return (skeleton) ? skeleton->get_num_dummies() : 0;
	}

	// link/unlink item with model bone
	void link_bone (zz_visible * item_in, uint32 bone_index);
	void unlink_bone (zz_visible * item_in); // we do not use this anywhere

	// add/del item in this model's item list
	bool add_item (zz_visible * item_in);
	bool del_item (zz_visible * item_in); // returns false if the item_in is not found

	virtual void set_seethru_recursive (float seethru_in);
	virtual void set_visibility_recursive (float visibility_in);
    virtual void set_lighting_recursive(zz_light* light);

	void update_normal (zz_time diff_time);

	void set_normal (const vec3&  normal_in)
	{
		if (normal_final == normal_in) return; // if not a new setting, skip it.
		
		// reset interpolation setting
		normal_ratio = 0;
		normal_last = normal_current;
		normal_final = normal_in;
	}

	const vec3& get_normal (void)
	{
		return normal_final;
	}

	vec3& get_move_velocity_by_time (zz_time diff_time, vec3& velocity_out);

	zz_bone_list& get_bones ()
	{
		return bones;
	}

	// update bounding volume
	void update_bvolume (void);

	void input_collision_block(zz_animatable *block);
    zz_animatable* output_collision_block();
	void reset_collision_block();
	
	// do nothing
	virtual void gather_collidable (std::vector<zz_visible*>& collidable_holder);

	virtual void gather_visible (std::vector<zz_visible*>& visible_holder);

	virtual void insert_scene ();
	virtual void remove_scene ();

	virtual void update_time (bool recursive, zz_time diff_time);

	virtual void set_glow_recursive (zz_glow_type glow_type_in, const vec3& color_in);

	ZZ_DECLARE_DYNAMIC(zz_model)
};

//--------------------------------------------------------------------------------
// Get distance^2 from camera to target
//--------------------------------------------------------------------------------
inline float zz_model::get_camtarget_distance_square () const
{
	return camtarget_distance_square;
}

//--------------------------------------------------------------------------------
// Set distance^2 from camera to target
//--------------------------------------------------------------------------------
inline void zz_model::set_camtarget_distance_square (float camtarget_distance_square_in)
{
	camtarget_distance_square = camtarget_distance_square_in;
}

//--------------------------------------------------------------------------------
// Set model move velocity
//--------------------------------------------------------------------------------
inline void zz_model::set_move_velocity (float velocity_in)
{
	move_velocity = velocity_in;
}

//--------------------------------------------------------------------------------
// Get model move velocity
//--------------------------------------------------------------------------------
inline float zz_model::get_move_velocity (void) const
{
	return move_velocity;
}

#endif // __ZZ_MODEL_H__