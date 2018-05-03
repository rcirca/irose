/** 
 * @file zz_scene_octree.h
 * @brief scene simple interface.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    23-feb-2002
 *
 * $Header: /engine/include/zz_scene_octree.h 37    06-12-29 4:23a Choo0219 $
 */

#ifndef __ZZ_SCENE_OCTREE_H__
#define __ZZ_SCENE_OCTREE_H__

#define ZZ_MAX_COLLECT_PACKS 10

#include <vector>

#ifndef __ZZ_OCTREE_H__
#include "zz_octree.h"
#endif

#ifndef __ZZ_SCENE_H__
#include "zz_scene.h"
#endif

class zz_scene;
class zz_visible;
class zz_model;
struct zz_bounding_obb;

#define MAX_SCENE_SPHERE 5000
#define MAX_SCENE_OBB 200
#define MAX_SCENE_AABB 200   
#define MAX_SCENE_CYLINDER 200
#define MAX_SCENE_LINE 200
#define MAX_SCENE_OBB2 200
#define MAX_SCENE_AXIS 200

struct sphere_element{
	
	float vec[3];
	float r;
	int color;
 
};

struct aabb_element{

	float min[3];
	float max[3];
	int color;

};

struct cylinder_element{

	float vec[3];
	float r;
	float length;
    int color;

};

struct line_element{

	vec3 vec1;
	vec3 vec2;
};

struct obb2_element{
	
	vec3 center;
    quat rotation;

	float xlength;
	float ylength;
	float zlength;
};

struct axis_element{

	float size;
	zz_visible *vis;
};

/*
enum zz_render_component {
	ZZ_RC_NONE			= 0,
	ZZ_RC_TERRAIN		= (1 << 0),
	ZZ_RC_OCEAN			= (1 << 1),
	ZZ_RC_SKY			= (1 << 2),
	ZZ_RC_OPAQUE		= (1 << 3),
	ZZ_RC_TRANSPARENT	= (1 << 4),
	ZZ_RC_ALL			= (1 << 16)
};
*/

// simple octree scene interface
//--------------------------------------------------------------------------------
class zz_scene_octree : public zz_scene {
protected:
	zz_octree octree;
	bool validity; // if false, not yet culled.

	void render_all ();
	void render_cull ();
	
	// inview nodes
	typedef std::vector<zz_visible*> zz_vislist;

	std::vector<zz_visible*> delayed_nodes;
	std::vector<zz_visible*> visible_nodes;
	std::vector<zz_visible*> oterrain_nodes; // opaque terrain
	std::vector<zz_visible*> bterrain_nodes; // blend terrain
	std::vector<zz_visible*> rterrain_nodes; // rough terrain
	std::vector<zz_visible*> ocean_nodes;
	std::vector<zz_visible*> particle_nodes;

	// for statistics
	// these values are vaild only after render()
	int num_delayed;
	int num_visible;
	int num_oterrain;
	int num_bterrain;
	int num_rterrain;
	int num_ocean;
	int num_particles;
	int num_total;
	int num_glows; // number of objects to glow

	// for collision check
	std::vector<zz_visible*> collect_packs[ZZ_MAX_COLLECT_PACKS];

	zz_model *get_target (vec3& cam_pos_out);
	void update_distance (const vec3& cam_pos);
	void update_display_list (zz_time diff_time);

	// render component
	void render_ocean ();
	void render_transparent ();
	void render_opaque ();
	void render_oterrain (); // render opaque terrain
	void render_bterrain (); // render blended terrain
	void render_rterrain (); // render rough terrain
	void render_particle ();
	void render_fonts ();
	void render_glow ();
	void render_sky ();
	void render_sky2 ();
	void render_sky_glow ();
	void render_terrain2 ();

	void render_shadowmap_objects (std::vector<zz_visible*>& nodes, const vec3& compare_pos, float maximum_distance_square);

	// update receive_shadow property
	// returns the number of shadowed objects
	int update_receive_shadow (zz_model * target);

	void dump_view ();

	bool dump_viewfrustum, dump_scene;

public:
	zz_scene_octree ();
	~zz_scene_octree ();

	virtual void render ();
	virtual void clear ();
	virtual void build (); // build scene array
	virtual void insert (zz_visible * vis);
	virtual void remove (zz_visible * vis);
	virtual void refresh (zz_visible * vis);
	virtual void update (zz_time diff_time);
	virtual void update_object_sort ();
	virtual void update_init_list ();
	void update_camera (zz_time diff_time);
	void update_exp_camera_after (zz_time diff_time);
	void update_time (zz_time diff_time);
	void update_animation (zz_time diff_time);
	void update_transform (zz_time diff_time);
	void update_exp_camera (zz_time diff_time);
	virtual void before_render ();
	virtual void after_render ();

	virtual void cull ();

	void draw_frustum (); // for test

	int scene_sphere_number;
	sphere_element scene_sphere[MAX_SCENE_SPHERE];
	void input_scene_sphere(float x, float y, float z, float r);
	void draw_scene_sphere();
   	void reset_scene_sphere();

	int scene_obb_number;
	const zz_bounding_obb* scene_obb[MAX_SCENE_OBB];
	void input_scene_obb(const zz_bounding_obb *obb);
	void draw_scene_obb();
	void reset_scene_obb();
      
    int scene_aabb_number;
	aabb_element scene_aabb[MAX_SCENE_AABB];
	void input_scene_aabb(float min[3], float max[3], DWORD color);
	void draw_scene_aabb();
	void reset_scene_aabb();
      
    int scene_cylinder_number;
	cylinder_element scene_cylinder[MAX_SCENE_CYLINDER];
	void input_scene_cylinder(float x, float y, float z, float length, float r);
	void draw_scene_cylinder();
    void reset_scene_cylinder();

    int scene_line_number;
	line_element scene_line[MAX_SCENE_LINE];
	void input_scene_line(vec3& vec1, vec3& vec2);
	void draw_scene_line();
    void reset_scene_line();
	
    int scene_obb2_number;
	obb2_element scene_obb2[MAX_SCENE_OBB2];
	void input_scene_obb2(vec3& center, quat& rotation, float xlength, float ylength, float zlength);
	void draw_scene_obb2();
	void reset_scene_obb2();
	
	axis_element scene_axis[MAX_SCENE_AXIS];
	int scene_axis_number;
	void input_scene_axis(zz_visible *vis,float size);
	void draw_scene_axis();
	void reset_scene_axis();

    bool fog_onoff_mode;

	// whether cull() is done, or not
	// if object is inserted or removed, the scene is invalideted
	// only update() make the scene validate
	void invalidate ()
	{
		validity = false;
	}

	bool is_valid ()
	{
		return this->validity;
	}

	//// get nodes in scene
	//int get_num_scene_node ();
	//zz_visible * get_scene_node (int index);
	
	// get nodes in viewfrustum
	int get_num_viewfrustum_node ();
	zz_visible * get_viewfrustum_node (int index);

	// test function
	void render_shadowmap ();
	void render_shadowmap_viewport ();

	void collect_by_minmax (
		std::vector<zz_visible *>& nodes,
		const vec3 minmax[2],
		bool skip_no_collision)
	{
		octree.collect_by_minmax(nodes, minmax, skip_no_collision);
	}

	// returns number of collected nodes
	int collect_by_minmax (
		int pack_index,
		const vec3 minmax[2], 
		bool skip_no_collision);

	zz_visible * get_collect_node (int pack_index, int node_index)
	{
		assert(pack_index < ZZ_MAX_COLLECT_PACKS);
		return collect_packs[pack_index][node_index];
	}

	void trace_tree ()
	{
		octree.trace_tree();
	}

	// get the number of rendered objects
	void get_statistics (int * num_total_out, int * num_transparent_out, int * num_opaque_out, int * num_terrain_out, int * num_ocean_out);

	void set_dump_view (bool log_terrain, bool log_ocean, bool log_opaque, bool log_transparent);

	void get_render_list (zz_visible * vis);

	void increase_num_glows ()
	{
		++num_glows;
	}

	int get_num_glows ()
	{
		return num_glows;
	}

	void updateEx(zz_viewfrustum *viewfrustum, const vec3& cam_pos);

	ZZ_DECLARE_DYNAMIC(zz_scene_octree)
};

#endif // __ZZ_SCENE_OCTREE_H__