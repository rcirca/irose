/** 
 * @file zz_trail.h
 * @brief trail class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    26-may-2003
 *
 * $Header: /engine/include/zz_trail.h 8     04-08-16 4:20p Zho $
 * $History: zz_trail.h $
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-08-16   Time: 4:20p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-17   Time: 6:16p
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-17   Time: 4:46p
 * Updated in $/engine/include
 * reference counting bug fixed. by check vbuffer before do reference
 * counting in invalidate_device_objects()
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-16   Time: 10:43p
 * Updated in $/engine/include
 * smooth trail near starting point by double zero quad spline.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-15   Time: 6:12p
 * Updated in $/engine/include
 * readded reference count
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-15   Time: 4:56p
 * Updated in $/engine/include
 * device restore added
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_TRAIL_H__
#define __ZZ_TRAIL_H__

#include <d3d9.h>
#include <list>

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

#ifndef __ZZ_VISIBLE_H__
#include "zz_visible.h"
#endif

class zz_texture;

typedef struct 
{
    vec3     position;
	vec2     uv;
} VERTEX_TRAIL;
#define D3DFVF_TRAIL (D3DFVF_XYZ | D3DFVF_TEX1)

class zz_trail : public zz_visible {
private:
	static LPDIRECT3DDEVICE9       device;
	static LPDIRECT3DVERTEXBUFFER9 vbuffer;
	static int vb_ref_count;

	zz_texture * texture;
	int last_temp_points; // to be popped in next frame

	// resource management information
	zz_device_resource res; // vertex buffer resource info

public:
	enum zz_control_state {
		ZZ_CS_STOP  = 0,
		ZZ_CS_START = 1,
		ZZ_CS_PAUSE = 2,
		ZZ_CS_NOSPAWN = 3
	};

protected:
	std::list<vec3> sp; // world start positions
	std::list<vec3> ep; // world end positions
	std::list<zz_time> point_time; // life time
	vec3 local_sp, local_ep; // initial local positions
	float world_length;
	vec3 sp_quad[4]; // spline : 3->2->1->0
	vec3 ep_quad[4];
	zz_time time_quad[4];
	zz_time duration;

	vec4 trail_color;
	float num_segments; // number of points to be created in 1 tick( 4800 TICKS )
	int num_points; // number of points to render
	int total_points; // number of total live points

	zz_time life_time; // trail current life time
	zz_time total_life_time; // trail total life time

	float trail_length; // total trail length
	float trail_length_excess; // excess of total trail length

	float distance_per_point; // create 1 points every X distance ( unit: cm )

	zz_control_state control_state; // 0 : stopped, 1 : started, 2 : paused

	bool unload ();

	bool interp_curve;

	void shift_points (const vec3& new_world_sp, const vec3& new_world_ep); // shifts quad points
	void push_points (int num_to_add);
	void pop_points ();
	bool update_vertex_buffer ();
	void setup_render_state ();
	void stop();

	virtual bool restore_device_objects ();
	virtual bool invalidate_device_objects ();
	
public:

	zz_trail();
	virtual ~zz_trail();

	bool load (
		vec4 trail_color_in,
		float distance_per_point_in,
		zz_time duration_in_millisecond,
		bool use_curve,
		const char * texture_path, 
		const vec3& start_point_in,
		const vec3& end_point_in);

	virtual void render (bool recursive = false);
	virtual void update_animation (bool recursive, zz_time diff_time);

	void set_control_state (zz_control_state state);
	zz_control_state get_control_state (void) { return control_state; }

	virtual void before_render ();
	virtual void after_render ();

	ZZ_DECLARE_DYNAMIC(zz_trail)
};


#endif __ZZ_TRAIL_H__
