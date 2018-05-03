/** 
 * @file zz_camera.cpp
 * @brief node.
 * @author	Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date 04-mar-2002
 *
 * $Header: /engine/src/zz_camera.cpp 38    02-01-01 1:13a Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_camera.h"
#include "zz_renderer.h"
#include "zz_vfs.h"
#include "zz_motion.h"
#include "zz_channel_position.h"
#include "zz_system.h"
#include "zz_view.h"
#include "zz_visible.h"

#define ZZ_CAMERA_MAGICNUMBER "ZCA0001"

ZZ_IMPLEMENT_DYNCREATE(zz_camera, zz_animatable)

zz_camera::zz_camera(void) :
	under_the_sea(false), 
	eye_channel_(NULL),
	center_channel_(NULL),
	up_channel_(NULL),
	fovy_near_far_channel_(NULL),
	fovy_(0),
	aspect_ratio_(0),
	near_plane_(0), far_plane_(0),
	eye_(vec3_null),
	center_(vec3_null),
	up_(vec3_null),
	pos_cur_(vec2_null), pos_last_(vec2_null),
	original_modelview_(mat4_id), our_modelview_(mat4_id),
	perspective_(true),
	local_eye_(vec3_null),
	speed_(0),
	last_eye_(vec3_null)
{
	modelview_matrix_ = mat4_id;
	projection_matrix_ = mat4_id;
	infrustum = true; // all cameras are in frustum.
	inscene = true;
	forced_transformation = false;
    forced_system_transformation = false;

  
	mass_a = 0.0f;
	mass_v = 0.0f;
	
    delta_time = 0.02f;

}

zz_camera::~zz_camera(void)
{
}

void zz_camera::look_at (const vec3& eye_in, const vec3& center_in, const vec3& up_in)
{
	modelview_matrix_ = mat4_id;
	
	// To recalculate orthogonal up vector
	vec3 new_up, right, direction(center_in - eye_in);
	cross(right, direction, up_in);
	cross(new_up, right, direction);
	eye_ = eye_in;
	center_ = center_in;
	up_ = new_up;

	::look_at(modelview_matrix_, eye_, center_, up_);

	//ZZ_LOG("camera: look_at() ->\n");
	//for (int i = 0; i < 4; i++) {
	//	ZZ_LOG("%f, %f, %f, %f\n", 
	//		modelview_matrix_.mat_array[i*4 + 0],
	//		modelview_matrix_.mat_array[i*4 + 1],
	//		modelview_matrix_.mat_array[i*4 + 2],
	//		modelview_matrix_.mat_array[i*4 + 3]);
	//}
}

void zz_camera::dolly (float dolly_factor)
{
	// recalc eye, center, up vector
	look_at_inv(modelview_matrix_, eye_, center_, up_);

	vec3 view_vector = center_ - eye_;
	normalize(view_vector);

	view_vector *= dolly_factor;
	center_ += view_vector;
	eye_ += view_vector;
	look_at(eye_, center_, up_);
	zz_visible::set_position(eye_ - local_eye_);
}

void zz_camera::truck (vec3 truck_vector)
{
	// ?
}

void zz_camera::rotate_x (float angle_degree)
{
	vec3 rotation_axis_x(1, 0, 0);
	mat4 matrix_rotation(mat4_id), matrix_result;
	matrix_rotation.set_rot(ZZ_TO_RAD*angle_degree, rotation_axis_x);
	
	mult(matrix_result, modelview_matrix_, matrix_rotation);
	set_transform(ZZ_MATRIX_MODELVIEW, matrix_result);
}

void zz_camera::rotate_y (float angle_degree)
{
	vec3 rotation_axis_x(0, 1, 0);
	mat4 matrix_rotation(mat4_id), matrix_result;
	matrix_rotation.set_rot(ZZ_TO_RAD*angle_degree, rotation_axis_x);
	
	mult(matrix_result, modelview_matrix_, matrix_rotation);
	set_transform(ZZ_MATRIX_MODELVIEW, matrix_result);
}

void zz_camera::rotate_z (float angle_degree)
{
	vec3 rotation_axis_x(0, 0, 1);
	mat4 matrix_rotation(mat4_id), matrix_result;
	matrix_rotation.set_rot(ZZ_TO_RAD*angle_degree, rotation_axis_x);
	
	mult(matrix_result, modelview_matrix_, matrix_rotation);
	set_transform(ZZ_MATRIX_MODELVIEW, matrix_result);
}

void zz_camera::set_transform (enum_transform_state_type matrix_type, const mat4& matrix_to_set)
{
	//assert(!perspective_);   //test
	switch (matrix_type) {
		case ZZ_MATRIX_MODELVIEW :
			modelview_matrix_ = matrix_to_set;
			look_at_inv(modelview_matrix_, eye_, center_, up_); // reset parameters
			
			if (!motion)
				zz_animatable::set_position(eye_ - local_eye_);  //test 10-16
			break;
		case ZZ_MATRIX_PROJECTION :
			projection_matrix_ = matrix_to_set;
			break;
	}

	//ZZ_LOG("camera: projection\n");
	//for (int i = 0; i < 4; i++) {
	//	ZZ_LOG("%f, %f, %f, %f,\n",
	//		projection_matrix_.mat_array[0*4+i],
	//		projection_matrix_.mat_array[1*4+i],
	//		projection_matrix_.mat_array[2*4+i],
	//		projection_matrix_.mat_array[3*4+i]);
	//}
	//ZZ_LOG("camera: modelview\n");
	//for (int i = 0; i < 4; i++) {
	//	ZZ_LOG("%f, %f, %f, %f,\n",
	//		modelview_matrix_.mat_array[0*4+i],
	//		modelview_matrix_.mat_array[1*4+i],
	//		modelview_matrix_.mat_array[2*4+i],
	//		modelview_matrix_.mat_array[3*4+i]);
	//}
}

const mat4& zz_camera::get_transform_const (enum_transform_state_type matrix_type)
{
	if (matrix_type == ZZ_MATRIX_MODELVIEW) {
		return modelview_matrix_;
	}
	else {
     	return projection_matrix_;
	}
}

void zz_camera::get_transform (enum_transform_state_type matrix_type, mat4& matrix_to_get)
{
	assert(!get_parent()->is_a(ZZ_RUNTIME_TYPE(zz_visible)));

#if (0) // for now, not using camera attached other visible
	if (this->get_parent() && this->get_parent()->is_a(ZZ_RUNTIME_TYPE(zz_visible))) { // if has parent node
		const mat4& m = this->get_worldTM();
		vec3 xaxis, yaxis, zaxis;
		xaxis = vec3(m.col(0)); // x-axis (top in max biped character)
		yaxis = vec3(m.col(1)); // y-axis (front in max biped character)
		zaxis = vec3(m.col(2)); // z-axis (left in max biped character)
		m.get_translation(eye_);
		center_ = eye_ + yaxis;
		up_ = xaxis;
		look_at_ortho(modelview_matrix_, eye_, center_, up_);
	}
#endif
	
	matrix_to_get = get_transform_const(matrix_type);
}

void zz_camera::set_aspect_ratio (float aspect_ratio_from)
{
	aspect_ratio_ = aspect_ratio_from;
	//assert(perspective_);
	if (perspective_) {;
		projection_matrix_._11 = projection_matrix_._22 / aspect_ratio_from;
	}
}

void zz_camera::set_frustum (float left, float right, float bottom, float top, float near_plane_in, float far_plane_in)
{
	assert(perspective_);
	projection_matrix_ = mat4_id;
	::frustum(projection_matrix_, left, right, bottom, top, near_plane_, far_plane_);
	//ZZ_LOG("camera: frustum() ->\n");
	//for (int i = 0; i < 4; i++) {
	//	ZZ_LOG("%f, %f, %f, %f\n", 
	//		projection_matrix_.mat_array[i*4 + 0],
	//		projection_matrix_.mat_array[i*4 + 1],
	//		projection_matrix_.mat_array[i*4 + 2],
	//		projection_matrix_.mat_array[i*4 + 3]);
	//}
}

void zz_camera::set_orthogonal (float width_in, float height_in, float near_plane_in, float far_plane_in)
{
	zz_assert(!perspective_);

	assert(width_in > 0);
	assert(height_in > 0);

	fovy_ = 0;
	aspect_ratio_ = width_in / height_in;
	near_plane_ = near_plane_in;
	far_plane_ = far_plane_in;
	::orthogonal(projection_matrix_, width_in, height_in, near_plane_, far_plane_);
}

void zz_camera::set_perspective (float fovy_in, float aspect_ratio_in, float near_plane_in, float far_plane_in)
{
	zz_assert(perspective_);

	fovy_ = fovy_in;
	aspect_ratio_ = aspect_ratio_in;
	near_plane_ = near_plane_in;
	far_plane_ = far_plane_in;

	::perspective(projection_matrix_, fovy_, aspect_ratio_, near_plane_, far_plane_);
}

void zz_camera::trackball (vec2 point_first, vec2 point_second, float trackball_size)
{
	quat quaternion;
	::trackball(quaternion, point_first, point_second, trackball_size);
	mat4 matrix_rotation(mat4_id);
	matrix_rotation.set_rot(quaternion);
	mat4 matrix_result;

	mult(matrix_result, modelview_matrix_, matrix_rotation);
	set_transform(ZZ_MATRIX_MODELVIEW, matrix_result);
}

bool zz_camera::load (const char * file_name)
{
	char magic_number[8];
	//char zca_id[256];

	zz_vfs camera_file;

	if (camera_file.open(file_name) == false) {
		ZZ_LOG("camera: load(%s) failed\n", file_name);
		return false;
	}

	camera_file.read_string(magic_number, 7);
	
    // verify magic_number
	if (strncmp(magic_number, ZZ_CAMERA_MAGICNUMBER, 7)) {
        return false; // wrong version or file structure
    }

	//camera_file.read_string(zca_id, 0);

	uint32 is_perspective;
	camera_file.read_uint32(is_perspective);
	perspective_ = (is_perspective) ? true : false;

	int i;
	for (i = 0; i < 16; i++) {
		camera_file.read_float(modelview_matrix_.mat_array[i]);
	}
	for (i = 0; i < 16; i++) {
		camera_file.read_float(projection_matrix_.mat_array[i]);
	}

	if (perspective_) {
		// will be rebuilded by fovy_, aspect_ratio_, near_plane, far_plane_
		// this is obsolete
		projection_matrix_.a23 *= ZZ_SCALE_IN;
	}
	else {
		projection_matrix_._11 /= ZZ_SCALE_IN;
		projection_matrix_._22 /= ZZ_SCALE_IN;
		projection_matrix_._33 /= ZZ_SCALE_IN;
	}

	camera_file.read_float(fovy_);
	camera_file.read_float(aspect_ratio_);
	camera_file.read_float(near_plane_);
	camera_file.read_float(far_plane_);

	near_plane_ = ZZ_XFORM_IN(near_plane_);
	far_plane_ = ZZ_XFORM_IN(far_plane_);

	// the max_exporter does not fill right matrix.
	// maybe we should transpose all elements, and apply orthogonal thing in exporter.
	// for now, we should rebuild projection matrix
	if (is_perspective) { // rebuild projection matrix
		set_perspective(fovy_, aspect_ratio_, near_plane_, far_plane_);
	}
	else {
		float width = 2.0f / projection_matrix_.a00;
		float height = 2.0f / projection_matrix_.a11;
		set_orthogonal(width, height, near_plane_, far_plane_);
	}

	camera_file.read_float(eye_.x);
	camera_file.read_float(eye_.y);
	camera_file.read_float(eye_.z);
	eye_ *= ZZ_SCALE_IN;
	
	local_eye_ = eye_;

	camera_file.read_float(center_.x);
	camera_file.read_float(center_.y);
	camera_file.read_float(center_.z);
	center_ *= ZZ_SCALE_IN;

	zz_visible::set_position(eye_ - local_eye_);

	camera_file.read_float(up_.x);
	camera_file.read_float(up_.y);
	camera_file.read_float(up_.z);

	camera_file.close();

	look_at(eye_, center_, up_);	

	return true;
}

bool zz_camera::save (const char * file_name)
{
	zz_vfs camera_file;

	if (camera_file.open(file_name, zz_vfs::ZZ_VFS_WRITE) == false) {
		ZZ_LOG("camera: save(%s) failed\n", file_name);
		return false;
	}
	camera_file.write_string(ZZ_CAMERA_MAGICNUMBER, 7 /* size */);
	
	camera_file.write_uint32(1); // force perspective

	int i;
	for (i = 0; i < 16; i++) {
		camera_file.write_float(modelview_matrix_.mat_array[i]);
	}

	for (i = 0; i < 16; i++) {
		camera_file.write_float(projection_matrix_.mat_array[i]);
	}

	camera_file.write_float(fovy_);
	camera_file.write_float(aspect_ratio_);
	camera_file.write_float(near_plane_*ZZ_SCALE_OUT);
	camera_file.write_float(far_plane_*ZZ_SCALE_OUT);

	camera_file.write_float(eye_.x*ZZ_SCALE_OUT);
	camera_file.write_float(eye_.y*ZZ_SCALE_OUT);
	camera_file.write_float(eye_.z*ZZ_SCALE_OUT);

	camera_file.write_float(center_.x*ZZ_SCALE_OUT);
	camera_file.write_float(center_.y*ZZ_SCALE_OUT);
	camera_file.write_float(center_.z*ZZ_SCALE_OUT);

	camera_file.write_float(up_.x);
	camera_file.write_float(up_.y);
	camera_file.write_float(up_.z);

	camera_file.close();

	return true;
}

bool zz_camera::apply_motion (void)
{
	if (!motion) return false; // no motion

	zz_time time = motion_controller.get_current_time();

	// vector3 channel was already adapted by scale.
	int fps = motion->get_fps();
	if (eye_channel_) {
		eye_channel_->get_by_time(time, fps, &eye_);
		eye_ += position;
	}
	if (center_channel_) {
		center_channel_->get_by_time(time, fps, &center_);
		center_ += position;
	}
	if (up_channel_) {
		up_channel_->get_by_time(time, fps, &up_);
		up_ *= ZZ_SCALE_OUT; // revert to original
		up_.normalize();
	}

	::look_at(modelview_matrix_, eye_, center_, up_);
	
	if (	fovy_near_far_channel_) {
		vec3 fovy_near_far;
		fovy_near_far_channel_->get_by_time(time, fps, &fovy_near_far);
		fovy_ = fovy_near_far.x*ZZ_SCALE_OUT; // revert to original value
		near_plane_ = fovy_near_far.y;
		far_plane_ = fovy_near_far.z;
		set_perspective(fovy_, aspect_ratio_, near_plane_, far_plane_);
	}
	return true;
}

void zz_camera::attach_motion (zz_motion * motion_to_attach)
{
	if (motion_to_attach) {
		eye_channel_ = static_cast<zz_channel_position *>(motion_to_attach->channels[0]);
		center_channel_ = static_cast<zz_channel_position *>(motion_to_attach->channels[1]);
		up_channel_ = static_cast<zz_channel_position *>(motion_to_attach->channels[2]);
		fovy_near_far_channel_ = static_cast<zz_channel_position *>(motion_to_attach->channels[3])	;
	}

	zz_animatable::attach_motion(motion_to_attach);
}

float zz_camera::get_fov()
{
	return fovy_;
}

void zz_camera::set_fov (float fov)
{
	zz_assert(perspective_);

	set_perspective(fov, this->aspect_ratio_, near_plane_, this->far_plane_);
}

void zz_camera::pan (vec2 pan_value)
{
	mat4 origin_mat, our_tm;
	this->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, origin_mat);
	mat4 pan_tm = mat4_id;
	pan_tm.set_translation(vec3(pan_value.x, pan_value.y, 0));

	set_transform(zz_camera::ZZ_MATRIX_MODELVIEW, origin_mat * pan_tm);
	//our_tm = origin_mat.inverse() * pan_tm * origin_mat;
	//this->set_transform(zz_camera::ZZ_MATRIX_MODELVIEW, origin_mat * our_tm);
}

void zz_camera::get_ray (vec2 screen_pos, vec3& origin, vec3& direction)
{
	// viewport range = [(-.5f, .5f), (-.5f, .5f)]
	vec3 v;
	
	// _11 = 2*n/(r - l), _22 = 2*n/(t - b)
	
	if(!znzin->screen_sfx.get_widescreen_mode())
	{
		v.x = (2.0f * screen_pos.x/znzin->view->get_width() - 1.0f) / projection_matrix_._11;
		v.y = (1.0f - 2.0f * screen_pos.y/znzin->view->get_height()) / projection_matrix_._22;
	}
	else
	{
	    zz_viewport view_port;
		znzin->screen_sfx.get_widescreen_viewport(view_port);
		int screen_x,screen_y;

		screen_x = screen_pos.x-view_port.x;
		screen_y = screen_pos.y-view_port.y;

		v.x = (2.0f * screen_x/view_port.width - 1.0f) / projection_matrix_._11;
		v.y = (1.0f - 2.0f * screen_y/view_port.height) / projection_matrix_._22;	 
	}
#ifdef	ZZ_ZRANGE_D3D
	v.z = -1.0f; // we use right-handed modelview, but d3d use left-handed projection
#else
	v.z = 1.0f;
#endif

	// convert ray vector on projection-space to world-space
	
	mat4 m;
	if(!znzin->camera_sfx.get_play_onoff())
		m = modelview_matrix_.inverse();
	else
	{
	   mat4 m2 = znzin->camera_sfx.get_steal_camera(); 	
	   m = m2.inverse(); 
	}
	
	direction.x = v.x*m._11 + v.y*m._12 + v.z*m._13;
	direction.y = v.x*m._21 + v.y*m._22 + v.z*m._23;
	direction.z = v.x*m._31 + v.y*m._32 + v.z*m._33;
	direction.normalize();

	// origin is the camera center
	// but, we have to clip by near plane
	origin.x = m.x;
	origin.y = m.y;
	origin.z = m.z;
	origin += 2.0f*near_plane_*direction;   //test 1-17
}

// from d3d
void zz_camera::pick (vec2 screen_pos, vec3& picked_position, float z_value)
{
	vec3 origin, direction;

	get_ray(screen_pos, origin, direction);

	// ray-plane intersection
	float t;
	vec3 plane_normal(0, 0, 1.0f);

	// t = -(z_value + dot(plane_normal, origin)) / (dot(normal, direction));
	// z_value means plane equation's D. Ax + By + Cz + D = 0. D = -NP0
	t = -(origin.z - z_value) / direction.z;

	picked_position = origin + t * direction;
	//ZZ_LOG("camera: pick(). origin(%f, %f, %f), t(%f), direction(%f, %f, %f)\n",
	//	origin.x, origin.y, origin.z, t, direction.x, direction.y, direction.z);

}

void zz_camera::move (vec3 deviation_world)
{
	// get eye, center, up vector
	look_at_inv(modelview_matrix_, eye_, center_, up_);

	center_ += deviation_world;
	eye_ += deviation_world;
	look_at(eye_, center_, up_);
}


// similar to move() function
void zz_camera::set_position (const vec3& position_world)
{
	// get eye, center, up vector
	look_at_inv(modelview_matrix_, eye_, center_, up_);

	vec3 camera_origin = eye_ - local_eye_; // maybe target

	vec3 translation = position_world - camera_origin;

	eye_ = eye_ + translation;
	center_ = center_ + translation;

	look_at(eye_, center_, up_);
	
	assert(this->parent_node == (zz_node*)(znzin->cameras));
	zz_animatable::set_position(position_world);
}

void zz_camera::trackball_begin (float x, float y)
{
	int screen_width, screen_height;
	screen_width = znzin->view->get_width();
	screen_height = znzin->view->get_height();

	pos_last_.x = x/screen_width - .5f; // (-.5, .5)
	pos_last_.y = .5f - y/screen_height; // (-.5, .5)

	get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, original_modelview_);
}

void zz_camera::trackball_rotate (float x, float y)
{
	int screen_width, screen_height;
	screen_width = znzin->view->get_width();
	screen_height = znzin->view->get_height();

	pos_cur_.x = x/screen_width - .5f;
	pos_cur_.y = .5f - y/screen_height;

	set_transform(zz_camera::ZZ_MATRIX_MODELVIEW, mat4(mat4_id));
	trackball(pos_cur_, pos_last_, 1.f);
	get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, our_modelview_);
	mat3 origin_rot(mat3_id);
	original_modelview_.get_rot(origin_rot);
	mat4 origin_rot_mat(mat4_id);
	origin_rot_mat.set_rot(origin_rot);
	our_modelview_ = original_modelview_ * origin_rot_mat.inverse() * our_modelview_ * origin_rot_mat;
	set_transform(ZZ_MATRIX_MODELVIEW, our_modelview_);
	//?get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, our_modelview_);
}

void zz_camera::trackball_end (float x, float y)
{
	get_transform(ZZ_MATRIX_MODELVIEW, our_modelview_);
}

void zz_camera::pan_begin (float x, float y)
{
	int screen_width, screen_height;
	screen_width = znzin->view->get_width();
	screen_height = znzin->view->get_height();

	pos_last_.x = x/screen_width - .5f; // (-.5, .5)
	pos_last_.y = .5f - y/screen_height; // (-.5, .5)

	get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, original_modelview_);
}

void zz_camera::pan_move (float x, float y, float pan_size)
{
	int screen_width, screen_height;
	screen_width = znzin->view->get_width();
	screen_height = znzin->view->get_height();

	pos_cur_.x = x/screen_width - .5f;
	pos_cur_.y = .5f - y/screen_height;

	set_transform(ZZ_MATRIX_MODELVIEW, mat4(mat4_id));
	pan(pan_size*vec2(pos_cur_ - pos_last_));
	get_transform(ZZ_MATRIX_MODELVIEW, our_modelview_);
	our_modelview_ = original_modelview_.inverse() * our_modelview_ * original_modelview_;
	set_transform(ZZ_MATRIX_MODELVIEW, original_modelview_ * our_modelview_);
	get_transform(ZZ_MATRIX_MODELVIEW, our_modelview_);
}

void zz_camera::pan_end (float x, float y)
{
	get_transform(ZZ_MATRIX_MODELVIEW, our_modelview_);
}

void zz_camera::look_at_from_matrix (void)
{
	look_at_inv(modelview_matrix_, eye_, center_, up_);
}

mat3& zz_camera::get_rotation_facing (mat3& m)
{
	// same rotation axis as camera
	m.set_col(0, get_right()); // set x-axis
	m.set_col(1, get_up()); // set y-axis
	m.set_col(2, -get_dir()); // set z-axis
	return m;
}

// world_axis = {0 : x, 1 : y, 2 : z}
mat3& zz_camera::get_rotation_facing_axis (mat3& m, int world_axis)
{
	vec3 x, y, z;
	if (world_axis == 0) { // world x-axis aligned
		// x = world x
		// y = up_
		// z = cross(x, y)
		x = vec3(1, 0, 0);
		y = get_up();
		cross(z, x, y);
		m.set_col(0, x);
		m.set_col(1, y);
		m.set_col(2, z);
	}
	else if (world_axis == 1) {
		// no need to implement this
	}
	else if (world_axis == 2) {
		// x = camera right
		// y = world z
		// z = cross(x, y)
		x = get_right();
		y = vec3(0, 0, 1);
		cross(z, x, y);
		m.set_col(0, x);
		m.set_col(1, y);
		m.set_col(2, z);
	}
	return m;
}

void zz_camera::world2screen (const vec3& world_pos, vec3& screen_pos)
{
	vec3 screen3;
	mat4 pmv;

	mult(pmv, this->projection_matrix_, this->modelview_matrix_);
	mult(screen3, pmv, world_pos);
	screen_pos.set(screen3.x, screen3.y, screen3.z);
	// screen_pos = ([-1, 1], [-1, 1])
	
	
	if(!znzin->screen_sfx.get_widescreen_mode())
	{
		float width, height;
		width = (float)znzin->get_rs()->buffer_width;
		height = (float)znzin->get_rs()->buffer_height;
		screen_pos.x = (.5f*screen_pos.x + .5f)*(width - 1);
		screen_pos.y = (.5f*screen_pos.y + .5f)*(height - 1);
		screen_pos.y = (height - 1 - screen_pos.y);
	}
	else
 	{
       zz_viewport view_port;
	   float screen_x,screen_y;
	   znzin->screen_sfx.get_widescreen_viewport(view_port);
       	   
	   screen_x=.5f*screen_pos.x + .5f;
       screen_y=.5f*screen_pos.y + .5f;

	   if(screen_x<0.0f || screen_x>1.0f)
			screen_x=10000;
	   else
	   {
			screen_pos.x =screen_x*(view_port.width - 1)+view_port.x;
	   }
	   
	   if(screen_y<0.0f || screen_y>1.0f)
			screen_y=10000;
	   else
	   {
			screen_pos.y = screen_y*(view_port.height - 1);
			screen_pos.y = (view_port.height - 1 - screen_pos.y)+view_port.y;  
	   }
	   
 
	}
}

float zz_camera::get_far_plane (void)
{
	return far_plane_;
}

float zz_camera::get_near_plane (void)
{
	return near_plane_;
}

const zz_viewfrustum& zz_camera::update_frustum (float margin)
{
	vec4& np = frustum_.np;
	vec4& fp = frustum_.fp;
	vec4& lp = frustum_.lp;
	vec4& rp = frustum_.rp;
	vec4& tp = frustum_.tp;
	vec4& bp = frustum_.bp;
	vec3 dir0;
	vec3 dir1;
	int ii;
	float dist,t,fac;
	float yon, hither, fov; // yon(far plane), hither(near plane)
	vec3 up, pos, dir, cross;

	yon = this->get_far_plane();
	hither = this->get_near_plane();
	fov = this->get_fov();
	
	if(znzin->camera_sfx.get_play_onoff())
	{
		mat4 inv_m;
		inv_m = znzin->camera_sfx.camera_sfx_m.inverse();
		
		up.x = inv_m._12;
		up.y = inv_m._22;
		up.z = inv_m._32;

		pos.x = inv_m._14;
		pos.y = inv_m._24;
		pos.z = inv_m._34;
		
		dir.x = -inv_m._13;
		dir.y = -inv_m._23;
		dir.z = -inv_m._33;

		cross.x = inv_m._11;
		cross.y = inv_m._21;
		cross.z = inv_m._31;
    }
	else
	{
		up = this->get_up();
		pos = this->get_eye();
		dir = this->get_dir();
		cross = this->get_right();
	}
	//compute the eight points defining the
	//bounds of the frustum
	//const float margin = (yon - hither)*.01f; block_size * 2
	if (perspective_) {
		for(ii=0;ii<8;ii++){
			dist=(ii&0x4)?yon:hither;
			//dist = (ii&0x4) ? dist + margin : dist - margin;

			frustum_.p[ii] = pos + dist*dir;

			if (perspective_)
				t = dist*tanf(0.5f*fov*ZZ_TO_RAD);
			else
				t = 2 / projection_matrix_._11;

			t += margin;

			t=(ii&0x2)?t:-t;
			frustum_.p[ii] += t*up;

			t = dist*tanf(0.5f*fov*ZZ_TO_RAD)*aspect_ratio_;
			t += margin;

			t=(ii&0x1)?-t:t;
			frustum_.p[ii] += t*cross;
		}
	}
	else { // orthogonal
		for(ii=0;ii<8;ii++){
			dist=(ii&0x4)?yon:hither;

			frustum_.p[ii] = pos + dist*dir;

			t = 1.0f / projection_matrix_._11 + margin; // projection_matrix._11 = 2 / width

			frustum_.p[ii] += ((ii&0x2)?t:-t) * up;

			t = 1.0f / projection_matrix_._22 + margin; // projection_matrix._22 = 2 / height

			frustum_.p[ii] += ((ii&0x1)?-t:t) * cross;
		}
	}

	//compute the near plane
	dir0 = frustum_.p[2] - frustum_.p[0];
	dir1 = frustum_.p[1] - frustum_.p[0];
	np[0]=dir0[1]*dir1[2]-dir0[2]*dir1[1];
	np[1]=-(dir0[0]*dir1[2]-dir0[2]*dir1[0]);
	np[2]=dir0[0]*dir1[1]-dir0[1]*dir1[0];
	fac=1.0f/sqrtf(np[0]*np[0]+np[1]*np[1]+np[2]*np[2]);
	np[0]*=fac;
	np[1]*=fac;
	np[2]*=fac;
	np[3]=-(np[0]*frustum_.p[0][0]+np[1]*frustum_.p[0][1]+np[2]*frustum_.p[0][2]);

	//compute the far plane
	dir0 = frustum_.p[5] - frustum_.p[4];
	dir1 = frustum_.p[6] - frustum_.p[4];
	fp[0]=dir0[1]*dir1[2]-dir0[2]*dir1[1];
	fp[1]=-(dir0[0]*dir1[2]-dir0[2]*dir1[0]);
	fp[2]=dir0[0]*dir1[1]-dir0[1]*dir1[0];
	fac=1.0f/sqrtf(fp[0]*fp[0]+fp[1]*fp[1]+fp[2]*fp[2]);
	fp[0]*=fac;
	fp[1]*=fac;
	fp[2]*=fac;
	fp[3]=-(fp[0]*frustum_.p[4][0]+fp[1]*frustum_.p[4][1]+fp[2]*frustum_.p[4][2]);

	//compute the top plane
	dir0 = frustum_.p[6] - frustum_.p[2];
	dir1 = frustum_.p[3] - frustum_.p[2];
	tp[0]=dir0[1]*dir1[2]-dir0[2]*dir1[1];
	tp[1]=-(dir0[0]*dir1[2]-dir0[2]*dir1[0]);
	tp[2]=dir0[0]*dir1[1]-dir0[1]*dir1[0];
	fac=1.0f/sqrtf(tp[0]*tp[0]+tp[1]*tp[1]+tp[2]*tp[2]);
	tp[0]*=fac;
	tp[1]*=fac;
	tp[2]*=fac;
	tp[3]=-(tp[0]*frustum_.p[2][0]+tp[1]*frustum_.p[2][1]+tp[2]*frustum_.p[2][2]);

	//compute the bottom plane
	dir0 = frustum_.p[1] - frustum_.p[0];
	dir1 = frustum_.p[4] - frustum_.p[0];
	bp[0]=dir0[1]*dir1[2]-dir0[2]*dir1[1];
	bp[1]=-(dir0[0]*dir1[2]-dir0[2]*dir1[0]);
	bp[2]=dir0[0]*dir1[1]-dir0[1]*dir1[0];
	fac=1.0f/sqrtf(bp[0]*bp[0]+bp[1]*bp[1]+bp[2]*bp[2]);
	bp[0]*=fac;
	bp[1]*=fac;
	bp[2]*=fac;
	bp[3]=-(bp[0]*frustum_.p[0][0]+bp[1]*frustum_.p[0][1]+bp[2]*frustum_.p[0][2]);

	//compute the left plane
	dir0 = frustum_.p[3] - frustum_.p[1];
	dir1 = frustum_.p[5] - frustum_.p[1];
	lp[0]=dir0[1]*dir1[2]-dir0[2]*dir1[1];
	lp[1]=-(dir0[0]*dir1[2]-dir0[2]*dir1[0]);
	lp[2]=dir0[0]*dir1[1]-dir0[1]*dir1[0];
	fac=1.0f/sqrtf(lp[0]*lp[0]+lp[1]*lp[1]+lp[2]*lp[2]);
	lp[0]*=fac;
	lp[1]*=fac;
	lp[2]*=fac;
	lp[3]=-(lp[0]*frustum_.p[1][0]+lp[1]*frustum_.p[1][1]+lp[2]*frustum_.p[1][2]);

	//compute the right plane
	dir0 = frustum_.p[4] - frustum_.p[0];
	dir1 = frustum_.p[2] - frustum_.p[0];
	rp[0]=dir0[1]*dir1[2]-dir0[2]*dir1[1];
	rp[1]=-(dir0[0]*dir1[2]-dir0[2]*dir1[0]);
	rp[2]=dir0[0]*dir1[1]-dir0[1]*dir1[0];
	fac=1.0f/sqrtf(rp[0]*rp[0]+rp[1]*rp[1]+rp[2]*rp[2]);
	rp[0]*=fac;
	rp[1]*=fac;
	rp[2]*=fac;
	rp[3]=-(rp[0]*frustum_.p[0][0]+rp[1]*frustum_.p[0][1]+rp[2]*frustum_.p[0][2]);

	return frustum_;
}

const zz_viewfrustum& zz_camera::get_frustum ()
{
	return this->frustum_;
}

void zz_camera::update_time (bool recursive, zz_time diff_time)
{
	if (!motion_controller.update(diff_time)) { // have some problem, stop motion
		// state transition. PLAY->STOP
		stop();
		// if it was playing, stop animation and apply this motion.
		if (apply_motion()) {
			invalidate_transform();
		}
	}
	// invalidate infrustum
	infrustum = false;
	invalidate_tm_minmax();

	update_speed(diff_time);

	if(forced_transformation)
	{
		calculate_force_interpolation(diff_time);
	}
	else if(forced_system_transformation)
	{
		calculate_force_interpolation_system(diff_time);
	}
}

void zz_camera::update_speed (zz_time diff_time)
{
	speed_ += last_eye_.distance(eye_);
	last_eye_ = eye_;

	speed_ -= float(ZZ_TIME_TO_MSEC(diff_time)) * 5.0f / 1000.0f; // default speed is 5 meter / 1000 msec
	
	const float MAX_SPEED = 10.0f; // 10 meter per second

	if (speed_ < 0) {
		speed_ = 0;
	}
	else if (speed_ > MAX_SPEED) {
		speed_ = MAX_SPEED;
	}

	//ZZ_LOG("camera: update_time(%04f)\n", speed_);
}

void zz_camera::input_forced_transformation_element(const vec3& position_, const quat& rotation_, float time)
{
	if(true/*!forced_transformation*/)
	{
		forced_transformation = true;
        
        mat4 mat;

		mat = modelview_matrix_.inverse();
				
		start_position = mat.get_position();
     	start_rotation = mat.get_rotation();
        final_position = position_;
		final_rotation = rotation_;
        current_time = 0.0f;
		total_time = time;
	    
		calculate_3rd_order_interpolation_func(0.25f);
	
	}
}

void zz_camera::calculate_force_interpolation(zz_time diff_time)
{
	current_time += diff_time / 4800.0f;
    
	if(current_time < total_time)
	{
		interpolation_func();
    
		current_position = start_position + t_ratio2 * (final_position - start_position);
		current_rotation = qslerp(start_rotation, final_rotation, t_ratio2);
	}
	else
	{
		forced_transformation = false;

		current_position = final_position;
		current_rotation = final_rotation;
	}

    mat4 mat;
    
	mat.set(current_position, current_rotation);
	modelview_matrix_ = mat.inverse();
	look_at_inv(modelview_matrix_, eye_, center_, up_);
}

void zz_camera::interpolation_func()
{
		 
	t_ratio1 = current_time / total_time;
	
/*	t = 2 * t_ratio1 - 1; // t' = 2t - 1  치환..
	t_ratio2 = 0.5f * ( 2*t - powf(t, 7)  + 1.0f );

   if(t_ratio2 < 0.0f)
	   t_ratio2 = 0.0f;

   if(t_ratio2 > 1.0f)
	   t_ratio2 = 1.0f;
*/

	if(t_ratio1 < t1_)
	{
		t_ratio2 = a1 + b1*t_ratio1 + c1*t_ratio1*t_ratio1 + d1*t_ratio1*t_ratio1*t_ratio1; 
	}
	else if(t_ratio1 < t2_)
	{
		t_ratio2 = t_ratio1;
	}
	else
	{
		t_ratio2 = a2 + b2*t_ratio1 + c2*t_ratio1*t_ratio1 + d2*t_ratio1*t_ratio1*t_ratio1; 
	}
}

void zz_camera::calculate_3rd_order_interpolation_func(float time_interval)
{
	t1_ = time_interval;
    t2_ = 1.0f - time_interval;
    
	float t_start, t_end;
   	
    mat4 mat,inv_mat;

	t_start = 0.0f;
	t_end = t1_;
	
	mat._11 = 1; mat._12 = t_start; mat._13 = t_start* t_start; mat._14 = t_start*t_start*t_start;
    mat._21 = 0.0f; mat._22 = 1; mat._23 = 2*t_start; mat._24 = 3*t_start*t_start;
    mat._31 = 1; mat._32 = t_end; mat._33 = t_end* t_end; mat._34 = t_end*t_end*t_end;
    mat._41 = 0.0f; mat._42 = 1; mat._43 = 2*t_end; mat._44 = 3*t_end*t_end;

    vec4 vec_value, vec_result;
	
	vec_value.x = t_start;  // f(start) 
	vec_value.y = 0;  // f`(start)
	vec_value.z = t_end; // f(end)
	vec_value.w = 1.0f;  // f`(end)

	inv_mat = mat.inverse();
	vec_result = inv_mat * vec_value;
   
    a1 = vec_result.x; b1 = vec_result.y; c1 = vec_result.z; d1 = vec_result.w;

	t_start = t2_;
	t_end = 1.0f;
	
	mat._11 = 1; mat._12 = t_start; mat._13 = t_start* t_start; mat._14 = t_start*t_start*t_start;
    mat._21 = 0.0f; mat._22 = 1; mat._23 = 2*t_start; mat._24 = 3*t_start*t_start;
    mat._31 = 1; mat._32 = t_end; mat._33 = t_end* t_end; mat._34 = t_end*t_end*t_end;
    mat._41 = 0.0f; mat._42 = 1; mat._43 = 2*t_end; mat._44 = 3*t_end*t_end;

 	
	vec_value.x = t_start;  // f(start) 
	vec_value.y = 1;  // f`(start)
	vec_value.z = t_end; // f(end)
	vec_value.w = 0.0f;  // f`(end)

	inv_mat = mat.inverse();
	vec_result = inv_mat * vec_value;
   
    a2 = vec_result.x; b2 = vec_result.y; c2 = vec_result.z; d2 = vec_result.w;

	t_start = t1_;
	t_end = t2_;
	
	mat._11 = 1; mat._12 = t_start; mat._13 = t_start* t_start; mat._14 = t_start*t_start*t_start;
    mat._21 = 0.0f; mat._22 = 1; mat._23 = 2*t_start; mat._24 = 3*t_start*t_start;
    mat._31 = 1; mat._32 = t_end; mat._33 = t_end* t_end; mat._34 = t_end*t_end*t_end;
    mat._41 = 0.0f; mat._42 = 1; mat._43 = 2*t_end; mat._44 = 3*t_end*t_end;

 	
	vec_value.x = t_start;  // f(start) 
	vec_value.y = 1;  // f`(start)
	vec_value.z = t_end; // f(end)
	vec_value.w = 1.0f;  // f`(end)

	inv_mat = mat.inverse();
	vec_result = inv_mat * vec_value;
}

void zz_camera::update_spring_element(float *system_value)
{
   mass_ = system_value[0]*16.0f+1.0f;
   b_value = system_value[1]*200.0f;
   k_value = system_value[2]*2000.0f+500.0f;
}

void zz_camera::calculate_spring_system()
{
   t_ratio1 = current_time / total_time;
	
   mass_a = (mass_*9.8f - b_value*mass_v + k_value*(10.0f*t_ratio1 - spring_length))/mass_;
   mass_v += mass_a*delta_time;
   spring_length += mass_v*delta_time;
   t_ratio2 = spring_length/10.0f;

}

void zz_camera::calculate_force_interpolation_system(zz_time diff_time)
{
	current_time += diff_time / 4800.0f;
    
	if(fabsf(t_ratio2) < 1.0f)
	{
		calculate_spring_system();
		
		current_position = start_position + t_ratio2 * (final_position - start_position);
		current_rotation = qslerp(start_rotation, final_rotation, t_ratio2);
	}
	else
	{
		forced_system_transformation = false;

		current_position = final_position;
		current_rotation = final_rotation;
	}

    mat4 mat;
    
	mat.set(current_position, current_rotation);
	modelview_matrix_ = mat.inverse();
	look_at_inv(modelview_matrix_, eye_, center_, up_);


}

void zz_camera::input_forced_transformation_system_element(const vec3& position_, const quat& rotation_, float *element_value, float time)
{
	if(!forced_transformation)
	{
		forced_system_transformation = true;
        
        mat4 mat;

		mat = modelview_matrix_.inverse();
				
		start_position = mat.get_position();
     	start_rotation = mat.get_rotation();
        final_position = position_;
		final_rotation = rotation_;
        current_time = 0.0f;
		total_time = time;
	    
		spring_length = 0.0f;
        mass_a = 0.0f;
		mass_v = 0.0f;
		t_ratio2 = 0.0f;
		t_ratio1 = 0.0f;
		update_spring_element(element_value);
	
	}
}

void zz_camera::set_forced_transformation(const vec3& position_, const quat& rotation_)
{
	mat4 mat;
	forced_transformation = false;
	mat.set(position_, rotation_);
	modelview_matrix_ = mat.inverse();
	look_at_inv(modelview_matrix_, eye_, center_, up_);
}
