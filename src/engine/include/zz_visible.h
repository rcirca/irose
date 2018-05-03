/** 
 * @file zz_visible.h
 * @brief visible class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-mar-2002
 *
 * $Header: /engine/include/zz_visible.h 51    05-12-16 8:11p Choo0219 $
 */

#ifndef __ZZ_VISIBLE_H__
#define __ZZ_VISIBLE_H__

#include <vector>

#ifndef __zz_bvolume_H__
#include "zz_bvolume.h"
#endif

#ifndef __ZZ_MESH_TOOL_H__
#include "zz_mesh_tool.h"
#endif

class zz_mesh;
class zz_light;
class zz_material;
class zz_camera;
class zz_shader;
class zz_scene;

#define TRANSFORM_CACHE_INDEX_WORLD_TM						(1 << 0)
#define TRANSFORM_CACHE_INDEX_WORLD_INVERSE_TM		(1 << 1)
#define TRANSFORM_CACHE_INDEX_LOCAL_TM							(1 << 2)
#define TRANSFORM_CACHE_INDEX_POSITION_WORLD				(1 << 3)
#define TRANSFORM_CACHE_INDEX_ROTATION_WORLD			(1 << 4)
#define TRANSFORM_CACHE_INDEX_COM_POSITION_WORLD	(1 << 5)
#define TRANSFORM_CACHE_INDEX_COM_ROTATION_WORLD	(1 << 6)
#define TRANSFORM_CACHE_ALL												((1 << 7) - 1)

#define ZZ_VISIBILITY_THRESHOLD (.99f) // this is being used only for zz_visible class

// do not mix the order. client uses it as integer. just add new
enum zz_draw_priority {
	ZZ_DP_HIGHEST = 0, // draw first
	ZZ_DP_HIGH    = 1,
	ZZ_DP_NORMAL  = 2,
	ZZ_DP_LOW     = 3,
	ZZ_DP_LOWEST  = 4  // draw last
};

// mesh-material-light set
struct zz_runit {
	zz_mesh * mesh;
	zz_material * material;
	zz_light * light;
	
	// per runit glow property
	zz_glow_type glow_type; // same as zz_visible's
	vec3 glow_color; // same as zz_visible's

	zz_runit ();

	void release();
};

class zz_visible : public zz_node {
	friend class zz_octree; // access to _onode
private:
	// octree node handle
	void * _onode; // this is managed only by zz_octree class
	void * _user_data; // set by client user who uses this engine.

protected:
	
	mat4 test_model;
	// true if this visible is to be in scene.
	// false if this visible is not to be in scene.
	// inscene does not mean this visible is actually in scene.
	// only top visibles can be actually in scene.
	// in update_bvolume(), actual insert/refresh/remove scene is accomplished by inscene and _onode.
	// set/get by set_inscene()/get_inscene().
	bool inscene; // it is to be in scene or not. it is set by octree

	// caution: visible's worldTM does not contain modelview matrix.
	mat4 worldTM; // to world transformation
	mat4 world_inverseTM; // precalculated for convience
	mat4 localTM; // local transformation
	vec3 position_world; // cached position world. When it was not cached, then you can get it by get_worldTM().get_transition()
	quat rotation_world; // cached rotation world. 
	vec3 com_position_world; // cached COM(center of mass) position world.
	quat com_rotation_world; // cached COM(center of mass) rotation world.

	// caching strategy
	// if set to true, it has no need to recalc.
	unsigned short tm_cache_flag; // bitset

	// basic transformation
	vec3 position;
	quat rotation;
	vec3 scale;

	// last frame's position in world coordinates
	// updated in update()
	vec3 prev_position_world;
    quat prev_rotation_world;

	// basic render unit
	unsigned int num_runits; 
	std::vector<zz_runit> runits;

	// distance from camera
	float camdist_square; // updated only in zz_scene_octree::update_display_list()

	// build scene minmax in world coordsys
	virtual void init_scene_minmax (void);

	// get mesh min/max in object coordinate system
	// sub-function of reset_bvolume()
	virtual void build_mesh_minmax (vec3& mesh_min_out, vec3& mesh_max_out);

	// get children scene_min/max in world coordsys recursively
	virtual void expand_scene_minmax (vec3& scene_min_out, vec3& scene_max_out);


	// 2-sized vec3 array. bounding aabb for scene node building. 
	// minmax[0] := scene min
	// minmax[1] := scene max
	// created by create_minmax() and destroyed by delete_minmax()
	// ~zz_is_visible() automatically delete_minmax().
	// minmax is used when we insert the visible into the scene, or remove from the scene.
	vec3 minmax[2];

	// bounding volume:
	// only visibles owning mesh can have bounding volume
	// (dummy visible does not have bounding volume)
	// bv should be update when updating runits
	zz_bvolume * bv; // bounding volume pointer. bv is created on demand, and destroyed at destructor
	zz_bvolume_type bv_type; // bounding volume type. it can be one of "sphere", "aabb", and "obb".
	
	// initialize and rebuild bounding volume. 
	// sub-function of reset_bvolume()
	virtual void get_local_center (vec3& local_center_out, const vec3& mesh_min, const vec3& mesh_max);

	// create(if does not have one) and set bounding volume
	virtual void reset_bvolume (void);

	// invalidate bvolume upward(to ancestors).
	// invalidating the minmax affects all ancestors.
	void invalidate_minmax_upward (void);

	// invalidate transform downward(to descendants).
	// invalidating the TM affects all descendats.
	void invalidate_transform_downward (void); 

	// invalidate transform and bounding-volume only for this node.
	// invalidating TM always brings invalidating minmax.
	virtual bool invalidate_tm_minmax (void); // return false if already dirty

	// invalidate bvolume only for this node
	virtual bool invalidate_minmax_once (void); // return false if already dirty

	// sub-function of update_bvolume (void)
	// update bounding volume by current transform (position, rotation, and scale)
	void update_bvolume_sub (const vec3& pos_world_in, const quat& quat_world_in, const vec3& scale_in);
	
	float visibility; // if zero, this object does not need to be rendered.
	float seethru; // see-thru factor by camera. default is 1.0f. update every frame by scene
	bool infrustum; // whether this node is in view-frustum or not
    bool shadow_onoff;
	
	
	int draw_priority; // alpha transparency sorting priority. default is 0

	// whether this node is to be updated from the scene.
	// whether this node's bounding volume is up-to-dated.
	// if it is false, we need to rebuild bounding volume and refresh from the scene in update()
	bool valid_bvolume;

	bool transparent; // true if has transparent material

	// these properties affect the matrial property.
	// these properties are updated by the scene.
	// *static* material ability property.
	bool cast_shadow;
	bool receive_shadow;
	bool receive_fog;
	// *dynamic* material ability property.
	bool cast_shadow_now;
	bool receive_shadow_now;
	bool receive_fog_now;

	zz_glow_type glow_type; // default type is ZZ_GLOW_NOTSET
	vec3 glow_color; // if vec_null, glow is disabled. default is null. because, final_glow = material_glow*visible_glow

	bool wire_mode;
	zz_collision_level collision_level; // collision level of this object
	
	// for physics
	vec3 velocity;
	float mass; // unit kg
	vec3 force; // force
	static vec3 gravity; // this will be out from here
	bool use_gravity; // whether its motion path is controlled by gravity or not

	static zz_scene * scene; // scenegraph pointer that controls this visible

	// visibility range from camera
	// the scene renders this visible only if the camdist is in the range (range_start, range_end).
	// default values are (range_start = 0, range_end = 0)
	// this value is applied only if range_start != range_end
	float range_square_start, range_square_end;

	// only following sequences are available:
	// FADE_IN_DONE -> FADE_OUT
	// FADE_OUT_DONE -> FADE_IN
	// FADE_IN -> FADE_IN_DONE
	// FADE_OUT -> FADE_OUT_DONE
	enum e_fade_state {
		FADE_NONE = 0,
		FADE_IN_DONE = 1,
		FADE_OUT = 2,
		FADE_OUT_DONE = 3,
		FADE_IN = 4,
	};

	// for fade in/out
	e_fade_state fade_state; // current fade state
	zz_time fade_start_time; // fade in/out start time for seethru variable computation

	// for constrainted billboard
	// -1(no use), 1-2 bit : rotation_axis, 3-4 bit : direction_axis, 5 bit : 
	int billboard_one_axis_order;
	int billboard_axis_order; // for rotate_facing_camera(). -1(no use), 1-2 bit : x-axis, 3-4 bit : y-axis

	// protected member functions
	void render_children (bool recursive = true);
	virtual void render_runit (unsigned int runit_index = 0);
    void render_runit_ex (unsigned int runit_index = 0);

	// axis : 0(x), 1(y), 2(z)
	void rotate_by_axis_facing_camera (const vec3& cam_pos_world, int rotation_axis_index, int directing_axis_index);
	void rotate_facing_camera (zz_camera * cam);

	// refresh this visible from scene
	// update scenegraph 
	// aka. scene->refresh(this)
	void scene_refresh ();

	void * get_onode () const
	{
		return _onode;
	}

	bool delayed_load; // load texture (or something) delayed. default is true

    bool forced_visibility_mode;

protected:
	zz_clip_face clip_face; // 0 : does not care, 1 : clip face first, 2 : clip face last, 3 : whether clip face or not. for eye-blinking, befault is false

public:

	// create/delete scene minmax
	vec3 * get_minmax ()
	{
		return minmax;
	}
	
	virtual unsigned long release (void);

	bool is_valid_bvolume ()
	{
		return valid_bvolume;
	}

	zz_visible(void);
	virtual ~zz_visible(void);

	static void set_scene (zz_scene * scene_in) { scene = scene_in; }

	zz_time get_timestamp () const { return timestamp; }
	void set_timestamp (zz_time timestamp_in) { timestamp = timestamp_in; }

	const mat4& get_worldTM ();
	const mat4& get_world_inverseTM ();
	const mat4& get_localTM ();
	const mat4& get_parentTM (); // get parent world TM

	virtual void invalidate_transform ();

	// do not call this. because that does not set position, rotation, scale member
	// instead, call the separate call (set_position(), set_rotation(), set_scale)
	//void set_localTM (const mat4& mat_to_set)
	//{
	//	localTM = mat_to_set;
	//	use_cached_localTM = true;
	//	invalidate_transform();
	//}

	virtual void set_position (const vec3& position_to_set)
	{
		position = position_to_set;
	}

	virtual void set_rotation (const quat& rotation_to_set)
	{
		rotation = rotation_to_set;
	}

	void set_rotation_local(const quat& rotation_to_set)
	{
		rotation = rotation_to_set;
	}
	void set_scale (const vec3& scale_to_set)
	{
		scale = scale_to_set;
	}

	const vec3& get_position (void) const
	{
		return position;
	}

	const quat& get_rotation (void) const
	{
		return rotation;
	}

	const vec3& get_scale (void) const
	{
		return scale;
	}

	void get_axis (int xyz012, vec3& axis_vector);
	void set_axis (int xyz012, const vec3& axis_vector);

	// if has one more transparent material
	virtual bool get_transparent ();

	// if it uses alpha-blending, then render later by distance-sorting at scene->render()
	bool get_delayed_render ()
	{
		return transparent;
	}

	//void set_delayed_render (bool true_if_delayed_render);
	
	virtual void render (bool recursive = false);

	void get_modelviewTM (mat4& modelview_matrix);
	const mat4& get_modelviewTM_const (); // get_modelviewTM const version
	
	void get_modelview_worldTM (mat4& modelview_worldTM); // modelview + world

	virtual void add_runit (zz_mesh * mesh, zz_material * material, zz_light * light);

	// clear all render unit
	void clear_runit (void);
	void pop_runit (void);

	unsigned int get_num_runits ()
	{
		return num_runits;
	}
	
	zz_mesh * get_mesh (unsigned int index);
	const char * get_mesh_path (unsigned int index = 0)
	{
		zz_mesh * mesh = get_mesh(index);
		return (mesh) ? mesh->get_path() : "null";
	}

	zz_material * get_material (unsigned int index);

	zz_light * get_light (unsigned int index);

	void set_mesh (unsigned int index, zz_mesh * mesh);
	void set_material (unsigned int index, zz_material * material);
	void set_light (unsigned int index, zz_light * light);

	virtual void link_child (zz_node * node_to_be_child);

	void set_bvolume_type (zz_bvolume_type bv_type_in)
	{
		bv_type = bv_type_in;
	}

	zz_bvolume_type get_bvolume_type ()
	{
		return bv_type;
	}

	zz_bvolume * get_bvolume (void)
	{
		return bv;
	}
	
	// update bounding volume
	// make the valid_bvolume true.
	// recursively update bvolume
	virtual void update_bvolume (void);

	float distance (zz_visible * vis)
	{
		return position.distance(vis->get_position());
	}

	float distance_square (zz_visible * vis)
	{
		return position.distance_square(vis->get_position());
	}

	void set_camdist_square (float distance_square)
	{
		camdist_square = distance_square;
	}

	void set_camdist_square_recursive (float distance_square);

	float get_camdist_square ()
	{
		return camdist_square;
	}

	virtual void update_time (bool recursive, zz_time diff_time); // update transform
	virtual void update_animation (bool recursive, zz_time diff_time); // apply current motion into this
	virtual void update_transform (zz_time diff_time) {}

	virtual void set_visibility_recursive (float visibility_in);
    
	virtual void set_lighting_recursive (zz_light* light ); 
	
	void set_shadow_onoff_recursive(bool onoff);
	 

	float get_visibility ()
	{
		return visibility;
	}

	// only used by scene_octree::update_display_list()
	void set_visibility (float visibility_in)
	{
		visibility = ZZ_MIN( 1.0f, ZZ_MAX( 0.0f, visibility_in ) );
	}

	void set_shadow_onoff(bool onoff)
	{
		shadow_onoff = onoff;
	}
	
	// is actually visible(or renderable) currently.
	bool is_visible ()
	{
		return ((visibility > 0.0f) && (seethru > 0.0f));
	}

	bool get_shadow_onoff()
	{
		return shadow_onoff; 
	}
	
	// rotate by axis relatively
	void rotate_by_axis (const float& theta_radian, const vec3& axis);

	// normalize vectors before you use
	void rotate_by_vector_absolute (const vec3& start, const vec3& end);
	void rotate_by_vector_relative (const vec3& start, const vec3& end);

	void set_cast_shadow (bool true_or_false)
	{
		cast_shadow = true_or_false;
	}
	bool get_cast_shadow ()
	{
		return cast_shadow;
	}
	void set_receive_shadow (bool true_or_false)
	{
		receive_shadow = true_or_false;
	}
	bool get_receive_shadow ()
	{
		return receive_shadow;
	}
	void set_receive_fog (bool true_or_false)
	{
		receive_fog = true_or_false;
	}
	bool get_receive_fog ()
	{
		return receive_fog;
	}

	void set_cast_shadow_now (bool true_or_false)
	{
		cast_shadow_now = cast_shadow && true_or_false;
	}
	bool get_cast_shadow_now ()
	{
		return cast_shadow_now;
	}
	void set_receive_shadow_now (bool true_or_false)
	{
		receive_shadow_now = receive_shadow && true_or_false;
	}
	bool get_receive_shadow_now ()
	{
		return receive_shadow_now;
	}
	void set_receive_fog_now (bool true_or_false)
	{
		receive_fog_now = receive_fog && true_or_false;
	}
	bool get_receive_fog_now ()
	{
		return receive_fog_now;
	}

	// object-ray intersection test and get the contact point
	virtual bool get_intersection_ray (const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, zz_mesh_tool::zz_select_method select_method = zz_mesh_tool::ZZ_SM_NEAREST);
	virtual bool get_intersection_ray_ex (const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, vec3& mesh_p1, vec3& mesh_p2, vec3& mesh_p3,  zz_mesh_tool::zz_select_method select_method = zz_mesh_tool::ZZ_SM_NEAREST);
	
	bool test_intersection_sphere (const zz_bounding_sphere& sphere);
	bool test_intersection_sphere_moving (const zz_bounding_sphere& sphere, const vec3& prev_center, vec3 * closest_center, float max_distance_square);

	// object-vis intersection test
	bool test_intersection_node (zz_visible * vis, zz_collision_level collision_depth);

	void set_collision_level (zz_collision_level level) { collision_level = level; }
	zz_collision_level get_collision_level () { return collision_level; }

	bool get_intersection_ray_level (
		const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, 
		zz_collision_level collision_level,
		zz_mesh_tool::zz_select_method select_method);

	bool get_intersection_ray_level_ex (
		const vec3& ray_origin, const vec3& ray_direction, vec3& contact_point, vec3& contact_normal, vec3& mesht_p1, vec3& mesht_p2, vec3& mesht_p3, 
		zz_collision_level collision_level,
		zz_mesh_tool::zz_select_method select_method);
	
	
	bool test_intersection_sphere_level (const zz_bounding_sphere& sphere_target, zz_collision_level collision_level_in);

	bool test_intersection_box_level (const vec3& min_in, const vec3& max_in, zz_collision_level collision_level);

	void set_wire_mode (bool mode) { wire_mode = mode; }
	bool get_wire_mode (void) { return wire_mode; }

	const vec3& get_scale_world (vec3& world_scale);
	const vec3& get_position_world (vec3& world_position);
	const quat& get_rotation_world (quat& world_rotation);

	virtual const vec3& get_com_position_world ();
	virtual const quat& get_com_rotation_world ();

	void set_draw_priority (int new_value) { draw_priority = new_value; }
	int get_draw_priority () { return draw_priority; }

	// set position&rotation&scale by worldTM
	void send_to_world ();

	// adjust pos&rot&scale by to_vis in sync with the same worldTM
	// to_vis can be null
	void send_to_local (zz_visible * to_vis);

	void set_clip_face (zz_clip_face clip_face_in) { clip_face = clip_face_in; }
	zz_clip_face get_clip_face () { return clip_face; }

	void set_userdata (void * data)
	{
		this->_user_data = data;
	}

	const void * const get_userdata () const
	{
		return this->_user_data;
	}

	void set_billboard_one_axis(bool use_billboard, int rotation_axis_index, int direction_axis_index);
	void set_billboard_axis (bool use_billboard, int axis_to_camera_index = 2);

	bool is_seethru () const
	{
		return (seethru < 1.0f);
	}

	float get_seethru () const
	{
		return this->seethru;
	}

	void set_seethru (float seethru_in)
	{
		this->seethru = seethru_in;
	}

	virtual void set_seethru_recursive (float seethru_in);

	virtual float get_height (); // get bounding box height
	// get horizontal radius. does not care vertical radius
	// @select_smaller true to select smaller radius between two axis
	virtual float get_radius (bool select_smaller = false);

	void priority_to ();

	// Saves previous position world
	const vec3& save_prev_position_world (void)
	{
		return get_position_world(prev_position_world);
	}

	const quat& save_prev_rotation_world (void)
	{
		return get_rotation_world(prev_rotation_world);
	
	}
	
	const vec3& get_prev_position_world ()
	{
		return prev_position_world;
	}

	const quat& get_prev_rotation_world ()
	{
		return prev_rotation_world; 
	}
	
	void set_infrustum ( bool infrustum_in )
	{
		infrustum = infrustum_in;
	}

	bool get_infrustum ()
	{
		return infrustum;
	}

	void set_range_square ( float range_square_start_in, float range_square_end_in )
	{
		range_square_start = range_square_start_in;
		range_square_end = range_square_end_in;
	}

	void get_range_square ( float& range_square_start_out, float& range_square_end_out )
	{
		range_square_start_out = range_square_start;
		range_square_end_out = range_square_end;
	}

	bool in_range ()
	{
		if (range_square_start == range_square_end) return true;
		if (camdist_square < range_square_start) return false;
		if (camdist_square > range_square_end) return false;
		return true;
	}

	void set_fade_in (zz_time fade_in_time)
	{
		fade_state = FADE_IN;
		fade_start_time = fade_in_time;
	}

	void set_fade_out (zz_time fade_out_time)
	{
		fade_state = FADE_OUT;
		fade_start_time = fade_out_time;
	}

	// return true if this visible have to be rendered.
	bool update_seethru_with_fade (zz_time current);

	const vec3& get_velocity ()
	{
		return velocity;
	}

	const vec3& get_force ()
	{
		return force;
	}

	float get_mass ()
	{
		return mass;
	}

	void set_velocity (const vec3& velocity_in)
	{
		velocity = velocity_in;
	}

	void set_force (const vec3& force_in)
	{
		force = force_in;
	}

	float set_mass (float mass_in)
	{
		mass = mass_in;
	}

	static void set_gravity (const vec3& gravity_in)
	{
		gravity = gravity_in;
	}

	void set_use_gravity (bool use_gravity_in)
	{
		use_gravity = use_gravity_in;
	}

	bool get_use_gravity ()
	{
		return use_gravity;
	}

	void accumulate_gravity (zz_time diff_time);

	void apply_lod (zz_mesh * mesh, zz_material * mat);

	// whether it should be in scene or not.
	bool get_inscene ()
	{
		return inscene;
	}

	
	// gather all children visibles that is collidable, including this
	virtual void gather_collidable (std::vector<zz_visible*>& collidable_holder);

	// gather all children visibles including this
	virtual void gather_visible (std::vector<zz_visible*>& collidable_holder);

	// insert to scene / remove from scene
	virtual void insert_scene (); // insert this(and all children) into scene
	virtual void remove_scene (); // remove this(and all children) from scene

	void set_glow (zz_glow_type glow_type_in, const vec3& color_in)
	{
		glow_type = glow_type_in;
		glow_color = color_in;
	}

	virtual void set_glow_recursive (zz_glow_type glow_type_in, const vec3& color_in);

	void set_glow_runit (unsigned int runit_in, zz_glow_type glow_type_in, const vec3& color_in);

	bool flush_device (bool immediate);

	// before_render: prepare something before rendering
	// eg. load textures and meshes, apply LOD
	virtual void before_render (); // called before render
	
	// after_render: clean up after rendering
	// eg. release temporal memory used only during rendering
	virtual void after_render (); // called after render

    void set_forced_visibility (bool true_or_false)
	{
		forced_visibility_mode = true_or_false;
	}

	bool get_force_visibility ()
	{
		return forced_visibility_mode;
	}


	ZZ_DECLARE_DYNAMIC(zz_visible)
};


#endif //__ZZ_VISIBLE_H__