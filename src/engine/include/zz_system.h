/** 
 * @file zz_system.h
 * @brief system class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    25-feb-2002
 *
 * $Header: /engine/include/zz_system.h 35    05-12-16 8:11p Choo0219 $
 */

#ifndef __ZZ_SYSTEM_H__
#define __ZZ_SYSTEM_H__

#ifndef __ZZ_TIMER_H__
#include "zz_timer.h"
#endif

#ifndef __ZZ_MESH_TOOL_H__
#include "zz_mesh_tool.h"
#endif

#ifndef __ZZ_MOTION_TOOL_H__
#include "zz_motion_tool.h"
#endif

#ifndef __ZZ_SCENE_H__
#include "zz_scene.h"
#endif

#ifndef __ZZ_SCENE_OCTREE_H__
#include "zz_scene_octree.h"
#endif

#ifndef __ZZ_GAMMA_H__
#include "zz_gamma.h"
#endif

#ifndef __ZZ_MANAGER_H__
#include "zz_manager.h"
#endif

#ifndef __ZZ_MANAGER_MESH_ISHARED_H__
#include "zz_manager_mesh_ishared.h"
#endif

#ifndef __ZZ_MANAGER_VISIBLE_H__
#include "zz_manager_visible.h"
#endif

#ifndef __ZZ_MANAGER_TEXTURE_H__
#include "zz_manager_texture.h"
#endif

#include "zz_sfx.h"

class zz_node;
class zz_morpher;
class zz_sky;
class zz_timer;
class zz_view;
class zz_view_d3d;
class zz_renderer;
class zz_motion_tool;
class zz_mesh_tool;
class zz_script_lua;
class zz_vfs;
class zz_manager;
class zz_font;
class zz_vfs_thread;
class zz_vfs_pkg_system;
struct zz_render_state;
class zz_manager_sfx;
class zz_manager_font;

#ifdef ZZ_USE_SOUND
class zz_manager_sound;
#endif

struct zz_system_info {
	char video_string[256];
	char engine_string[256];
};

//--------------------------------------------------------------------------------
class zz_system : zz_node {
private:
	friend class zz_view_d3d;

	zz_timer timer;
	zz_time current_time;
	zz_time last_time; // last time adjusted
	zz_time last_time_; // last time real
	zz_time force_diff_time; // to force setting the diff_time
	bool use_force_diff_time; // true if use force diff_time

	bool use_time_weight; // for now, camera_follow class uses this
	bool use_fixed_framerate;

	int last_sleep_time_msec; // extra time(msec) of last frame. by sleep()
	bool use_motion_interpolation; // whether use smooth interpolation or not, default is true
	float camdist_square_for_motion_interpolation; // if positive, only near objects are motion interpolated

	// read current time from timer, and set current_time to the current time
	// CAUTION: call this only at once per frame!
	zz_time set_current_time (void);
	zz_time set_current_time (zz_time time);
	zz_time add_time (zz_time time_to_add);

	// Internal system timer
	void timer_start ();
	void timer_stop ();
	void timer_reset ();

	// This method calls the manager->for_each(xxxx_device_objects),
	// and used in system::[restory|invalidate|init|delete]_device_objects().
	bool call_device_objects_func_ (zz_manager::zz_device_objects_func func);

#ifndef ZZ_IGNORE_TRIGGERVFS
	//--------------------------------------------------------------------------------
	// Two TriggerVFS file system handle for main thread and worker thread
	static zz_vfs_pkg_system * pkg_system_; // For main thread. All default vfs file system uses pkg_system.
	static zz_vfs_pkg_system * pkg_system_worker_; // For worker thread. Only one worker vfs_thread will use this.
	//--------------------------------------------------------------------------------
#endif

	zz_camera * camera; // default current camera
	zz_camera * camera_light; // shadow camera

	zz_render_state * rs; // render state

	zz_light * light; // default light

public:
	// utility class
	zz_mesh_tool mesh_tool;
	zz_motion_tool motion_tool;
	zz_system_info sysinfo;

	// system-dependent
	zz_view * view;
	zz_renderer * renderer;

	// sky
	zz_sky * sky;

	// system font
	zz_font * font_sys;

	// objects
	zz_manager * cameras;
	zz_manager * meshes;
	zz_manager_mesh_ishared * terrain_meshes; // mesh only for terrains
	zz_manager_mesh_ishared * rough_terrain_meshes; // mesh only for terrains
	zz_manager_mesh_ishared * ocean_meshes; // mesh only for oceans
	zz_manager * materials;
	zz_manager_visible * visibles;
	zz_manager * lights;
	zz_manager * shaders;
	zz_manager * channels;
	zz_manager * motions;
	zz_manager * skeletons;
	zz_manager_texture * textures;
	zz_manager * terrain_blocks;
	zz_manager * ocean_blocks;
	zz_manager * rough_terrain_blocks;
	zz_manager_font * fonts;
	zz_manager * cursors;

    zz_screen_sfx screen_sfx; 
    zz_camera_sfx camera_sfx;
    zz_sprite_sfx sprite_sfx;
	zz_avatar_selection_sfx avatar_selection_sfx;
    zz_moving_camera_screen_sfx moving_camera_sfx;


#ifdef ZZ_USE_SOUND
	zz_manager_sound * sounds;
#endif

	zz_manager_sfx * sfxs; // post-render special effects
    bool sfx_onoff;                            // cho
	
	zz_scene_octree scene;
	
	// scripting system
	zz_script_lua * script;

	// global variables in a system
	static zz_gamma gamma_black;
	static zz_gamma gamma_white;
	static zz_gamma gamma_normal;
	static uint64 ticks_per_second;

	static zz_vfs file_system; // default system filesystem object to access general filesystem method
	// vfs_thread for threaded texture loading. Currently, this is used in only for texture.

#ifndef ZZ_IGNORE_TRIGGERVFS
	static zz_vfs_thread * vfs_thread;
#endif

	static float linear_gravity;

	bool manager_flush_delayed (zz_manager * man, bool flush_entrance, bool flush_exit);

public:
	zz_system ();
	virtual ~zz_system ();

	enum zz_system_category {
		ZZ_SC_ALL = 0,
		ZZ_SC_CAMERA,
		ZZ_SC_CHANNEL,
		ZZ_SC_LIGHT,
		ZZ_SC_MATERIAL,
		ZZ_SC_MESH, // except for TERRAIN/ROUGH_TERRAIN/OCEAN MESH
		ZZ_SC_MODEL,
		ZZ_SC_MOTION,
		ZZ_SC_VISIBLE,
		ZZ_SC_MORPHER,
		ZZ_SC_TERRAIN,
		ZZ_SC_HELPER,
		ZZ_SC_SHADER,
		ZZ_SC_SKELETON,
		ZZ_SC_TEXTURE,
		ZZ_SC_TERRAIN_BLOCK,
		ZZ_SC_ROUGH_TERRAIN_BLOCK,
		ZZ_SC_PARTICLE,
		ZZ_SC_OCEAN_BLOCK,
		ZZ_SC_ROUGH_TERRAIN_MESH,
		ZZ_SC_TERRAIN_MESH,
		ZZ_SC_OCEAN_MESH,
		ZZ_SC_FONT,
		ZZ_SC_CURSOR,
		ZZ_SC_SOUND,
		ZZ_SC_NONE
	};

	virtual zz_node * find (const char * name_to_get, int categories = ZZ_SC_ALL);
	static float get_fps (bool count_on = false);
	zz_render_state * get_rs (void) { return rs; } // get render state
	zz_time get_current_time ();

	// get the difference between current time and last time
	zz_time get_diff_time ();
	void set_diff_time (zz_time diff_in);

	void tick_time ();
	bool get_use_time_weight ();
	void set_use_time_weight (bool use_or_not);

	bool get_use_fixed_framerate ();
	void set_use_fixed_framerate (bool use_or_not);

	bool get_use_motion_interpolation ();
	void set_use_motion_interpolation (bool use_or_not);

	float get_motion_interp_range ();
	void set_motion_interp_range (float distance_square);

	// called by renderer
	bool restore_device_objects (); // create unmanaged objects
	bool invalidate_device_objects (); // destroy unmanaged objects
	bool init_device_objects (); // create managed objects
	bool delete_device_objects (); // destroy managed objects

	int manager_update (int time_to_update); // unit ms.
	void sleep (); // wait not to exceed maximum framerate
	int get_last_sleep_time ();

	bool flush_delayed (bool flush_entrance, bool flush_exit); // flush all delayed objects

	void set_linear_gravity (float linear_gravity_in)
	{
		linear_gravity = linear_gravity_in;
	}

	float get_linear_gravity ()
	{
		return linear_gravity;
	}

#ifndef ZZ_IGNORE_TRIGGERVFS
	//--------------------------------------------------------------------------------
	// pkg filesystem related
	//--------------------------------------------------------------------------------
	// Opens pkg filesystems for main and worker thread. And then, returns the main thread's pkg_system.
	static zz_vfs_pkg_system * set_pkg_system (const char * filesystem_name_in);

	// Returns the main thread's pkg_system.
	// This is for default vfs file system.
	static zz_vfs_pkg_system * get_pkg_system ();
	//--------------------------------------------------------------------------------
#endif

	// set default camera
	void set_camera (zz_camera * cam_in)
	{
		assert(cam_in);
		camera = cam_in;
	}

	zz_camera * get_camera ()
	{
		return camera;
	}

	// set light camera for shadow casting
	void set_camera_light (zz_camera * cam_in)
	{
		assert(cam_in);
		camera_light = cam_in;
	}
	
	zz_camera * get_camera_light ()
	{
		return camera_light;
	}

	void set_light (zz_light * light_in)
	{
		assert(light_in);
		light = light_in;
	}

	zz_light * get_light ()
	{
		return light;
	}

	void initialize ();

	ZZ_DECLARE_DYNAMIC(zz_system)
};

inline zz_time zz_system::set_current_time (void)
{
	return set_current_time(timer.get_time());
}

inline zz_time zz_system::set_current_time (zz_time time)
{
	if (!timer.is_running())
		timer.start();
	assert(current_time >= last_time);
	last_time_ = last_time = current_time;
	return current_time = time;
}	

inline zz_time zz_system::add_time (zz_time time_to_add)
{
	return set_current_time(current_time + time_to_add);
}

inline void zz_system::timer_start ()
{
	timer.start();
	last_time = 0;
	set_current_time();
}

inline void zz_system::timer_stop ()
{
	timer.stop();
}

inline void zz_system::timer_reset ()
{
	timer.reset();
	set_current_time();
}

inline zz_time zz_system::get_current_time ()
{
	if (!timer.is_running())
		set_current_time();
	return current_time;
}

// get the difference between current time and last time
inline zz_time zz_system::get_diff_time ()
{
	return (current_time - last_time);
}

inline void zz_system::set_diff_time (zz_time diff_in)
{
	assert(diff_in >= 0);
	force_diff_time = diff_in;
	use_force_diff_time = true;
}

inline bool zz_system::get_use_time_weight ()
{
	return use_time_weight;
}

inline void zz_system::set_use_time_weight (bool use_or_not)
{
	use_time_weight = use_or_not;
}

inline bool zz_system::get_use_fixed_framerate ()
{
	return use_fixed_framerate;
}

inline void zz_system::set_use_fixed_framerate (bool use_or_not)
{
	use_fixed_framerate = use_or_not;
}

inline bool zz_system::get_use_motion_interpolation ()
{
	return use_motion_interpolation;
}

inline void zz_system::set_use_motion_interpolation (bool use_or_not)
{
	use_motion_interpolation = use_or_not;
}

inline float zz_system::get_motion_interp_range ()
{
	return camdist_square_for_motion_interpolation;
}

inline void zz_system::set_motion_interp_range (float distance_square)
{
	camdist_square_for_motion_interpolation = distance_square;
}

inline int zz_system::get_last_sleep_time ()
{
	return last_sleep_time_msec;
}

#ifdef ZZ_IN_SYSTEM_CPP
zz_system * znzin = NULL;
#else
extern zz_system * znzin;
#endif // ZZ_BY_SYSTEM_CPP

#endif // __ZZ_SYSTEM_H__