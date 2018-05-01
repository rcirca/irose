/** 
 * @file zz_camera.h
 * @brief camera class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-feb-2002
 *
 * $Header: /engine/include/zz_camera.h 14    06-01-03 10:41a Choo0219 $
 * $History: zz_camera.h $
 * 
 * *****************  Version 14  *****************
 * User: Choo0219     Date: 06-01-03   Time: 10:41a
 * Updated in $/engine/include
 * 
 * *****************  Version 13  *****************
 * User: Choo0219     Date: 05-12-29   Time: 8:41p
 * Updated in $/engine/include
 * 
 * *****************  Version 12  *****************
 * User: Choo0219     Date: 05-12-16   Time: 8:11p
 * Updated in $/engine/include
 * 
 * *****************  Version 11  *****************
 * User: Choo0219     Date: 05-02-14   Time: 3:16p
 * Updated in $/engine/include
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-09-16   Time: 6:23p
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-09-01   Time: 10:25p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-09-01   Time: 2:59p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-24   Time: 2:12p
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-03-25   Time: 10:33a
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-03-19   Time: 5:27p
 * Updated in $/engine/include
 * Added camera under the sea.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 11:35a
 * Updated in $/engine/include
 * void apply_motion() -> bool apply_motion()
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-04   Time: 5:55p
 * Updated in $/engine/include
 * more precise camera test
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:05p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:53p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:47p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:19p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef	__ZZ_CAMERA_H__
#define __ZZ_CAMERA_H__

#ifndef __ZZ_ANIMATABLE_H__
#include "zz_animatable.h"
#endif

#ifndef __ZZ_VIEWFRUSTUM_H__
#include "zz_viewfrustum.h"
#endif

class zz_channel_position;
class zz_model;

//--------------------------------------------------------------------------------
class zz_camera : public zz_animatable {
private:
	// internal variables for camera trackball
	vec2 pos_cur_, pos_last_;
	mat4 original_modelview_, our_modelview_;
	void trackball (vec2 point_first, vec2 point_second, float trackball_size);

protected:
	// camera animation channel
	zz_channel_position * eye_channel_; // eye position channel
	zz_channel_position * center_channel_; // center position channel
	zz_channel_position * up_channel_; // up direction channel
	zz_channel_position * fovy_near_far_channel_; // fovy, near, far channel

	// matrix transform
	mat4 projection_matrix_;
	mat4 modelview_matrix_;

	// perspective setting
	bool perspective_; // whether perspective or orthogonal
	float fovy_; // field of view of y-axis in degree. not in radian
	float aspect_ratio_; // view space width divided by height. aspect_ratio = width/height
	float near_plane_, far_plane_;
	
	// position & orientation
	vec3 eye_; // camera origin position
	vec3 center_; // camera viewing target center position
	vec3 up_; // up vector. simply y-vector

	vec3 local_eye_; // local eye position

	bool under_the_sea; // true if camera is under the sea

	zz_viewfrustum frustum_; // view frustum information. updated update_frustum() in zz_scene_octree::cull()
	
	vec3 last_eye_; // last eye position for calculate speed_
	float speed_; // camera position distance(meter) per second. updated by eye_. it is approximation. updated by update_time()

	void update_speed (zz_time diff_time); // update speed by eye_. call this at update_time() virtual calls

public:
	enum enum_transform_state_type {
		ZZ_MATRIX_MODELVIEW,
		ZZ_MATRIX_PROJECTION
	};

	zz_camera(void);
	virtual ~zz_camera(void);

	void zoom_in (float zoom_factor);
	void zoom_out (float zoom_factor);
	void pan (vec2 pan_value);
	void roll (float roll_value);
	void yaw (float yaw_value);
	void pitch (float pitch_value);
	virtual void dolly (float dolly_factor);
	void truck (vec3 truck_vector);
	void move (vec3 deviation_world);

	float get_fov (void);
	void set_fov (float fov);

	float get_far_plane (void);
	float get_near_plane (void);

	// not equal to get_position()
	// get_position() means get_center()
	const vec3& get_eye (void)
	{
		return eye_;
	}
	const vec3& get_up (void)
	{
		up_.normalize();
		return up_;
	}
	vec3 get_dir (void) {
		vec3 ret(center_ - eye_);
		ret.normalize();
		return ret;
	}
	vec3 get_right (void) {
		vec3 cross_vec;
		cross(cross_vec, get_dir(), get_up());
		cross_vec.normalize();
		return cross_vec;
	}

	void look_at_from_matrix (void);
	void look_at (const vec3& eye, const vec3& center, const vec3& up);
	void rotate_x (float angle_degree);
	void rotate_y (float angle_degree);
	void rotate_z (float angle_degree);
	void set_transform (enum_transform_state_type matrix_type, const mat4& matrix_to_set);
	void get_transform (enum_transform_state_type matrix_type, mat4& matrix_to_get);
	const mat4& get_transform_const (enum_transform_state_type matrix_type);

	void set_frustum (float left, float right, float bottom, float top, float near_plane, float far_plane);
	const zz_viewfrustum& update_frustum (float margin = 200.0f); // update viewfrustum plane and points
	const zz_viewfrustum& get_frustum (); // must be called after update_viewfrustum()

	/// @fovy field of view of y-axis in degree. not in radian
	/// @aspect view space width divided by height
	void set_perspective (float fovy, float aspect, float near_plane, float far_plane);
	void set_orthogonal (float width, float height, float near_plane, float far_plane);

	void set_aspect_ratio (float aspect_ratio_from);
	float get_aspect_ratio () {
		//projection_matrix._11 = projection_matrix._22 / aspect_ratio_from;
		return projection_matrix_._22 / projection_matrix_._11;
	}

	// camera trackball for mouse interaction
	void trackball_begin (float screen_x, float screen_y);
	void trackball_rotate (float screen_x, float screen_y);
	void trackball_end (float screen_x, float screen_y);

	// camera panning for mouse interaction
	void pan_begin (float screen_x, float screen_y);
	void pan_move (float screen_x, float screen_y, float pan_size);
	void pan_end (float screen_x, float screen_y);

	bool apply_motion (void);
	void attach_motion (zz_motion * motion_to_attach);

	void pick (vec2 screen_pos, vec3& picked_position, float z_value);
	void get_ray (vec2 screen_pos, vec3& origin, vec3& direction);

	bool load (const char * file_name);
	bool save (const char * file_name);

	// set center position
	virtual void set_position (const vec3& position_world);

	void world2screen (const vec3& world_pos, vec3& screen_pos);

	mat3& get_rotation_facing (mat3& m);
	mat3& get_rotation_facing_axis (mat3& m, int world_axis); // world_axis = {0 : x, 1 : y, 2 : z}

	// get near/far plane corner point position
	// [left|right][top|bottom] : if it is null, do not calculate it.
	const void get_corner_points (vec3 * lefttop, vec3 * leftbottom, vec3 * righttop, vec3 * rightbottom, bool near_or_far = true); 

	bool get_under_the_sea ()
	{
		return under_the_sea;
	}

	virtual void update_time (bool recursive, zz_time diff_time);
    
	virtual void update_camera_collision(){};

	virtual zz_model * get_target () { return 0; }

	float get_speed ()
	{
		return speed_;
	}
    
	bool forced_transformation;
	vec3 start_position, final_position, current_position;
	quat start_rotation, final_rotation, current_rotation;
    float total_time;
	float current_time;
	float t_ratio1,t_ratio2;
    
    void input_forced_transformation_element(const vec3& position_, const quat& rotation_, float time);
	void interpolation_func();
    void calculate_force_interpolation(zz_time diff_time);   
    void calculate_3rd_order_interpolation_func(float time_interval);

	float a1, b1, c1, d1;    // f(x) =  a1 + b1*x + c1*x^2 + d1*x^3  
	float a2, b2, c2, d2;
	float t1_, t2_;
	
	//Spring Damper System

	float mass_;    //camera의 질량..
	float k_value;  // spring 상수
	float b_value;   // damper 상수
    float spring_length;  // spinrg & damper System의 길이..
    float mass_a, mass_v;
    float delta_time;
    bool forced_system_transformation;	
   
	void update_spring_element(float *system_value);
	void calculate_spring_system();
	void calculate_force_interpolation_system(zz_time diff_time);
    void input_forced_transformation_system_element(const vec3& position_, const quat& rotation_,float *element_value, float time);
    void set_forced_transformation(const vec3& position_, const quat& rotation_);

	ZZ_DECLARE_DYNAMIC(zz_camera)
};

#endif // __ZZ_CAMERA_H__
