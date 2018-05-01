/** 
 * @file zz_system.cpp
 * @brief system class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    25-feb-2002
 *
 * $Header: /engine/src/zz_system.cpp 88    04-11-09 3:10p Jeddli $
 */

#define ZZ_IN_SYSTEM_CPP

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font.h"
#include "zz_font_d3d.h"
#include "zz_cursor.h"
#include "zz_renderer_d3d.h"
#include "zz_view_d3d.h"
#include "zz_os.h"
#include "zz_skeleton.h"
#include "zz_texture.h"
#include "zz_sky.h"
#include "zz_particle_emitter.h"
#include "zz_profiler.h"
#include "zz_log.h"
#include "zz_manager.h"
#include "zz_manager_font.h"
#include "zz_vfs.h"
#include "zz_scene.h"
#include "zz_script.h"
#include "zz_script_lua.h"
#include "zz_timer.h"
#include "zz_system.h"
#include "zz_shader.h"
#include "zz_vfs_pkg.h"
#include "zz_manager_sound.h"
#include "zz_pointer.h"
#include "zz_sfx.h"
#include "zz_material.h"
#include "zz_version_info.inc"

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
#include "zz_vfs_thread.h"
#endif
#include "zz_path.h"

#define ZZ_MESH_DELETED_SIZE 1000

ZZ_IMPLEMENT_DYNCREATE(zz_system, zz_node)

zz_gamma zz_system::gamma_normal(0);
zz_gamma zz_system::gamma_white(1);
zz_gamma zz_system::gamma_black(2);
uint64 zz_system::ticks_per_second = 0;
zz_vfs zz_system::file_system;

#ifndef ZZ_IGNORE_TRIGGERVFS
zz_vfs_pkg_system * zz_system::pkg_system_ = NULL;
zz_vfs_pkg_system * zz_system::pkg_system_worker_ = NULL;
zz_vfs_thread *zz_system::vfs_thread = NULL;
#endif // ZZ_IGNORE_TRIGGERVFS

float zz_system::linear_gravity = -800.0f*ZZ_SCALE_IN / 1000; // -2 m / 1000 msec

zz_system::zz_system () : 
		current_time(0),
		last_time(0),
		last_time_(0),
		force_diff_time(0),
		use_force_diff_time(false),
		use_time_weight(true), use_fixed_framerate(false), sky(NULL),
		use_motion_interpolation(true),
		camdist_square_for_motion_interpolation(1000.0f*1000.0f),
		camera(0),
		camera_light(0),
		light(0)
{	
	sprintf(sysinfo.engine_string, ZZ_VERSION_STRING);
	sprintf(sysinfo.video_string, "N/A");

	//zz_mesh::print_strip_indices(65, 17);

	zz_os::initialize(); // only once

	zz_os::get_ticks_per_second(ticks_per_second); // read ticks per second

	sfxs = zz_new zz_manager_sfx;

	script = (zz_script_lua *)ZZ_RUNTIME_TYPE(zz_script_lua)->create_instance();
	script->set_name(".lua_script");

	view = (zz_view_d3d *)ZZ_RUNTIME_TYPE(zz_view_d3d)->create_instance();
	view->set_name(".view");

	renderer = (zz_renderer_d3d *)ZZ_RUNTIME_TYPE(zz_renderer_d3d)->create_instance();
	renderer->set_name(".renderer");
	
	rs = renderer->get_state();

	zz_material::s_renderer = renderer;
	zz_material::s_renderstate = renderer->get_state();
	
	zz_mesh_ishared::init_static_all();

	// initialize managers
	visibles = (zz_manager_visible *)ZZ_RUNTIME_TYPE(zz_manager_visible)->create_instance();
	visibles->set_name(".visibles");

	textures = (zz_manager_texture *)ZZ_RUNTIME_TYPE(zz_manager_texture)->create_instance();
	textures->set_name(".textures");

	meshes = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	meshes->set_name(".meshes");

	materials = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	materials->set_name(".materials");

	terrain_meshes = (zz_manager_mesh_ishared *)ZZ_RUNTIME_TYPE(zz_manager_mesh_ishared)->create_instance();
	terrain_meshes->set_num_reuse(500);
	terrain_meshes->set_name(".terrain_meshes");
	
	rough_terrain_meshes = (zz_manager_mesh_ishared *)ZZ_RUNTIME_TYPE(zz_manager_mesh_ishared)->create_instance();
	rough_terrain_meshes->set_num_reuse(50);
	rough_terrain_meshes->set_name(".rough_terrain_meshes");

	ocean_meshes = (zz_manager_mesh_ishared *)ZZ_RUNTIME_TYPE(zz_manager_mesh_ishared)->create_instance();
	ocean_meshes->set_name(".ocean_meshes");

	lights = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	lights->set_name(".lights");

	shaders = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	shaders->set_name(".shaders");

	channels = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	channels->set_name(".channels");

	motions = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	motions->set_name(".motions");

	skeletons = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	skeletons->set_name(".skeletons");

	terrain_blocks = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	terrain_blocks->set_name(".terrain_blocks");

	rough_terrain_blocks = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	rough_terrain_blocks->set_name(".rough_terrain_blocks");

	ocean_blocks = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	ocean_blocks->set_name(".ocean_blocks");

	fonts = (zz_manager_font *)ZZ_RUNTIME_TYPE(zz_manager_font)->create_instance();
	fonts->set_name(".fonts");

	cursors = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	cursors->set_name(".cursors");

	cameras = (zz_manager *)ZZ_RUNTIME_TYPE(zz_manager)->create_instance();
	cameras->set_name(".cameras");

#ifdef ZZ_USE_SOUND
	sounds = (zz_manager_sound *)ZZ_RUNTIME_TYPE(zz_manager_sound)->create_instance();
	sounds->set_name(".sounds");
#endif

	//--------------------------------------------------------------------------------
	// global variables setting
	//--------------------------------------------------------------------------------
    sfx_onoff = true;            //cho

	//ZZ_LOG("system: initialized ok.\n");
}

void zz_system::initialize ()
{
	textures->set_lazy(get_rs()->lazy_texture_size);
	meshes->set_lazy(get_rs()->lazy_mesh_size);
	terrain_meshes->set_lazy(get_rs()->lazy_terrain_mesh_size);
	rough_terrain_meshes->set_lazy(get_rs()->lazy_terrain_mesh_size);
	ocean_meshes->set_lazy(get_rs()->lazy_ocean_mesh_size);

	// init default system font
	font_sys = static_cast<zz_font *>(fonts->spawn("systemfont", ZZ_RUNTIME_TYPE(zz_font_d3d)));
	font_sys->set_font_property(
		"Arial",
		DEFAULT_CHARSET,
		9,
		ZZ_COLOR32_ARGB(255, 255, 255, 255),
		ZZ_COLOR32_ARGB(255, 0, 0, 0),
		//false, false, zz_font::OUTLINE_TYPE_NONE);
		false, false, zz_font::OUTLINE_TYPE_SIMPLE);

	assert(rs);
	
	//font_sys->set_cache_property(1, 256, 256);

	zz_visible::set_scene(&scene);
}

zz_system::~zz_system ()
{
	zz_visible::set_scene(0);

	if (font_sys) {
		assert(font_sys->is_a(ZZ_RUNTIME_TYPE(zz_font)));
		fonts->kill(font_sys);
		font_sys = NULL;
	}
	scene.clear();

	if (sfxs) { zz_delete sfxs; sfxs = NULL; }

	zz_material::s_renderer = NULL;
	zz_material::s_renderstate = NULL;

	// for garbage collecting
	if (cameras) { cameras->release_children(); cameras = NULL; }
	if (materials) { materials->release_children(); materials = NULL; }
	if (meshes) { meshes->release_children(); meshes = NULL; }
	if (terrain_meshes) { terrain_meshes->release_children(); terrain_meshes = NULL; }
	if (rough_terrain_meshes) { rough_terrain_meshes->release_children(); rough_terrain_meshes = NULL; }
	if (ocean_meshes) { ocean_meshes->release_children(); ocean_meshes = NULL; }
	if (materials) { materials->release_children(); materials = NULL; }
	if (lights) { lights->release_children(); lights = NULL; }
	if (shaders) { shaders->release_children(); shaders = NULL; }
	if (visibles) { visibles->release_children(); visibles = NULL; }
	if (motions) { motions->release_children(); motions = NULL; } // motions must precedes channels
	if (channels) { channels->release_children(); channels = NULL; }
	if (skeletons) { skeletons->release_children(); skeletons = NULL; }
	if (textures) { textures->release_children(); textures = NULL; }
	if (terrain_blocks) { terrain_blocks->release_children(); terrain_blocks = NULL; }
	if (rough_terrain_blocks) { rough_terrain_blocks->release_children(); rough_terrain_blocks = NULL; }
	if (ocean_blocks) { ocean_blocks->release_children(); ocean_blocks = NULL; }
	if (fonts) { fonts->release_children(); fonts = NULL; }
	if (cursors) { cursors->release_children(); cursors = NULL; }

	if (renderer)
		renderer->cleanup();

	zz_mesh_ishared::dest_static_all();

#ifdef ZZ_USE_SOUND
	if (sounds) { sounds->release_children(); sounds = NULL; }
#endif

	// renderer should be destroyed after meshes,
	// because destructor of zz_mesh use znzin->renderer->destroy_vertex_buffer()
	// and destroy_index_buffer()
	ZZ_SAFE_RELEASE(renderer);
	ZZ_SAFE_RELEASE(view);
	ZZ_SAFE_RELEASE(script); // script class is not a manager 	

	rs = NULL;

#ifndef ZZ_IGNORE_TRIGGERVFS
	ZZ_SAFE_DELETE(pkg_system_);
	
	// Deletes vfs_thread first, and delet pkg_system later
#ifdef USE_VFS_THREAD_TEXTURE_LOADING
	// deleting vfs_thread must be after deleting resources(eg, textures)
	// textures class closes their vfs_thread file in their destructor.
	ZZ_SAFE_DELETE(vfs_thread);
	ZZ_SAFE_DELETE(pkg_system_worker_);
#endif
#endif // ZZ_IGNORE_TRIGGERVFS
}

zz_node * zz_system::find (const char * name_to_get, int categories)
{
	zz_node * found = NULL;

	//--------------------------------------------------------------------------------
	// CAUTION: 
	// "models" and "morpher" manager does not really exist.
	// Instead, we can use visibles manager for those objects.
	// These objects are to be connected with each other
	// in the same node_system and so we need to combine them in a manager.
	//--------------------------------------------------------------------------------
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_CAMERA) {
		found = this->cameras->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_CHANNEL) {
		found = this->channels->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_LIGHT) {
		found = this->lights->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_MATERIAL) {
		found = this->materials->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_MESH) {
		found = this->meshes->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_ROUGH_TERRAIN_MESH) {
		found = this->rough_terrain_meshes->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_TERRAIN_MESH) {
		found = this->terrain_meshes->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_OCEAN_MESH) {
		found = this->ocean_meshes->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_MODEL) {
		found = this->visibles->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_MOTION) {
		found = this->motions->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_VISIBLE) {
		found = this->visibles->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_MORPHER) {
		found = this->visibles->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_SHADER) {
		found = this->shaders->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_SKELETON) {
		found = this->skeletons->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_TEXTURE) {
		found = this->textures->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_ROUGH_TERRAIN_BLOCK) {
		found = this->rough_terrain_blocks->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_TERRAIN_BLOCK) {
		found = this->terrain_blocks->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_PARTICLE) {
		found = this->visibles->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_OCEAN_BLOCK) {
		found = this->ocean_blocks->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_FONT) {
		found = this->fonts->find(name_to_get);
		if (found) return found;
	}
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_CURSOR) {
		found = this->cursors->find(name_to_get);
		if (found) return found;
	}
#ifdef ZZ_USE_SOUND
	if ((categories == ZZ_SC_ALL) || categories & ZZ_SC_SOUND) {
		found = this->fonts->find(name_to_get);
		if (found) return found;
	}
#endif

	return found;
}

float zz_system::get_fps (bool count_on)
{
	static bool started = false;
	static const int fps_interval = 1; // every X seconds, check fps
	static uint64 start_time, end_time;
	static float result_fps = 0;
	static int render_count = 0;
	static uint64 ticks_per_second;

	if (!started) {
		zz_os::get_ticks(start_time);
		started = true;
	}
	zz_os::get_ticks(end_time);
	
	zz_os::get_ticks_per_second(ticks_per_second);
	
	if (end_time - start_time > (uint64)(fps_interval * ticks_per_second)) {
		result_fps = (float)(render_count) * int64(ticks_per_second) / int64(end_time - start_time);
		//ZZ_LOG("system: fps = %f\n", result_fps);
		start_time = end_time;
		render_count = 0;
	}
	if (count_on) render_count++;
	return result_fps;
}

#define USE_SMOOTH_FRAMERATE

void zz_system::tick_time ()
{
	if (use_fixed_framerate) {
		add_time(80);
	}
#ifdef USE_SMOOTH_FRAMERATE
	else {
		static float accum_error = 0; // accumulated diff error
		const zz_time MAX_ACCUM_ERROR = ZZ_TICK_PER_SEC / 1; // max error = 1 sec delay
		const zz_time MIN_ACCUM_ERROR = ZZ_TICK_PER_SEC / 10; // min error = .1 sec delay

		const int MIN_FRAMERATE = 5; // not to get slower than min_framerate
		const zz_time MAX_DIFF = ZZ_TICK_PER_SEC / MIN_FRAMERATE; // time(1sec) / frame
		const zz_time ERROR_MAX_DIFF = ZZ_TICK_PER_SEC*1; // 1 sec

		zz_time now = timer.get_time();
		zz_time diff = now - last_time_; // new real diff

		if (use_force_diff_time) {
			diff = force_diff_time;
			use_force_diff_time = false;
			force_diff_time = 0;
		}

		if (diff > ERROR_MAX_DIFF) { // zhotest
			diff = ERROR_MAX_DIFF;
		}

		if (0) {
			if (diff > MAX_DIFF) {
				accum_error += diff - MAX_DIFF;
				if (accum_error > MAX_ACCUM_ERROR) { // if error exceeded max error
					diff = zz_time(accum_error) + MAX_DIFF;
					accum_error = 0;
				}
				else {
					diff = MAX_DIFF;
				}
			}
			else {
				if (accum_error < MIN_ACCUM_ERROR) {
					diff += zz_time(accum_error);
					accum_error = 0;
				}
				else {
					diff += MIN_ACCUM_ERROR;
					accum_error -= float(MIN_ACCUM_ERROR);
					assert(accum_error >= 0);
				}
			}
		}
		else if (1) {
			const zz_time ACCUM_ERROR_FULL = 2 * ZZ_TICK_PER_SEC; // not to exceed 2 sec
			const float KP = .3f; // magic constants. damper
			zz_time extra_add = 0;

			accum_error += float(diff); // it's current time

			if (accum_error > ACCUM_ERROR_FULL) {
				extra_add = zz_time(accum_error) - ACCUM_ERROR_FULL;
				accum_error = ACCUM_ERROR_FULL;
			}

			assert(accum_error <= ACCUM_ERROR_FULL);

			assert(KP <= 1.0f);
			diff = zz_time (KP * accum_error * accum_error / ACCUM_ERROR_FULL);
			

			accum_error -= diff;
			diff += extra_add;

			assert(accum_error >= 0);
		}

		set_current_time(now); // update current_time and last_time
		last_time = now - diff; // overwrite last_time set by set_current_time
		last_time_ = now;

		//ZZ_LOG("system: tick_time(). diff(%10d),last(%10d), now(%10d)\n", diff, last_time, now);
		//ZZ_LOG("%f, \n", accum_error);
		//ZZ_LOG("%d, \n", diff);
	}
#else // NON SMOOTH FRAMERATE
	else { // dynamic frame rate
		set_current_time();
	}
#endif
}

bool zz_system::call_device_objects_func_ (zz_manager::zz_device_objects_func func)
{
	//bool saved_state = state()->use_delayed_loading;
	//state()->use_delayed_loading = false;

	int progress = 0;
	//ZZ_LOG("%d0-", progress++);

	// for shader
	if (shaders)
		shaders->for_each(zz_manager::zz_device_objects_callback(func));
	//ZZ_LOG("%d1-", progress++);

	// for mesh
	if (meshes) 
		meshes->for_each(zz_manager::zz_device_objects_callback(func));
	//ZZ_LOG("%d2-", progress++);

	// for terrain mesh
	if (terrain_meshes)
		terrain_meshes->for_each(zz_manager::zz_device_objects_callback(func));
	//ZZ_LOG("%d3-", progress++);

	// for rough terrain mesh
	if (rough_terrain_meshes)
		rough_terrain_meshes->for_each(zz_manager::zz_device_objects_callback(func));
	//ZZ_LOG("%d4-", progress++);

	// for ocean mesh
	if (ocean_meshes)
		ocean_meshes->for_each(zz_manager::zz_device_objects_callback(func));
	//ZZ_LOG("%d5-", progress++);

	// for texture
	if (textures)
		textures->for_each(zz_manager::zz_device_objects_callback(func));
	//ZZ_LOG("%d6-", progress++);

	// for visibles
	if (visibles)
		visibles->for_each(zz_manager::zz_device_objects_callback(func));
	//ZZ_LOG("%d7-", progress++);

	// for fonts
	if (fonts)
		fonts->for_each(zz_manager::zz_device_objects_callback(func));
	//ZZ_LOG("%d8-", progress++);

	// for cursors
	if (cursors)
		cursors->for_each(zz_manager::zz_device_objects_callback(func));
	//ZZ_LOG("%d9-", progress++);

	//ZZ_LOG("EnDp\n");
	//state()->use_delayed_loading = saved_state;
	return true;
}

bool zz_system::invalidate_device_objects ()
{
	zz_manager_mesh_ishared::s_invalidate_device_objects();
	//ZZ_LOG("system: INV-");
	return call_device_objects_func_(&zz_node::invalidate_device_objects);
}

bool zz_system::init_device_objects ()
{
	//ZZ_LOG("system: INI-");
	return call_device_objects_func_(&zz_node::init_device_objects);
}

bool zz_system::delete_device_objects ()
{
	//ZZ_LOG("system: DEL-");
	return call_device_objects_func_(&zz_node::delete_device_objects);
}

bool zz_system::restore_device_objects ()
{
	zz_manager_mesh_ishared::s_restore_device_objects();
	//ZZ_LOG("system: RES-");
	return call_device_objects_func_(&zz_node::restore_device_objects);
}	

int zz_system::manager_update (int time_to_update)
{
	//if (!state()->use_delayed_loading) return 0;

	static uint64 start = 0, end = 0, diff = 0;

	zz_os::get_ticks(start);

	if (1) {
		// Currently, managers using lazy device manipulation method are:
		//   meshes, terrain_meshes, ocean_meshes, and textures.
		// This can be set by script interface, ::setLazyBufferSize()
		zz_time total = ZZ_MSEC_TO_TIME(time_to_update);

		if (meshes->get_lazy()) {
			meshes->update(total);
		}
		if (rough_terrain_meshes->get_lazy()) {
			rough_terrain_meshes->update(total);
		}
		if (terrain_meshes->get_lazy()) {
			terrain_meshes->update(total);
		}
		if (ocean_meshes->get_lazy()) {
			ocean_meshes->update(total);
		}
		if (textures->get_lazy()) {
			textures->update(total);
			textures->unbind_notset();
		}
	}

	zz_os::get_ticks(end);
	diff = end - start;

	return int(1000*(double(diff)/ticks_per_second));;
}

// Wait if we exceed maximum frame rate.
void zz_system::sleep ()
{
	static zz_time start(0), end(0), diff(0);

	end = timer.get_time();

	diff = end - start;

	float current_swap_msec = float(ZZ_TIME_TO_MSEC(diff));
	float max_swap_msec = 1000.0f/float(get_rs()->max_framerate); // 30 frame/sec = (30/1000) (frame / msec). -> (1000/30) msec / frame
	float min_swap_msec = 1000.0f/float(get_rs()->min_framerate);

	//ZZ_LOG("system: sleep() min_swap_msec = %f, max_framerate(%f), current_swap_msec(%f)\n",
	//	min_swap_msec, float(get_rs()->max_framerate), current_swap_msec);

	last_sleep_time_msec = int(min_swap_msec - current_swap_msec);
	
	int min_update_msec = ZZ_TIME_TO_MSEC(diff); // minumum update time in msec
	min_update_msec *= min_update_msec;

	const int max_sleep_msec = 5; // maximum sleep time in msec

	// flush resources conserving min framerate
	//ZZ_LOG("system: wait_flush(current(%dms) + surplus(%dms))\n", int(current_swap_msec), int(last_sleep_time_msec));
	if (last_sleep_time_msec > min_update_msec) {
		int time_to_update = last_sleep_time_msec;
		int manager_update_msec = manager_update(time_to_update); // apply just half
		current_swap_msec += manager_update_msec;
		//ZZ_LOG("system: sleep() update(%dms)\n", time_to_update);
	}
	else { // we cannot afford to update
		//ZZ_LOG("system: sleep() update(%dms)v no room to update\n", min_update_msec);
		//manager_update(0); // initialize accumulated time of manager. it does not update anything.
		manager_update(min_update_msec); // minimum update
	}
	
	last_sleep_time_msec = int(max_swap_msec - current_swap_msec);
	if (last_sleep_time_msec > 0) { // sleep conserving max framerate
		//ZZ_LOG("system: sleep() sleep(%dms)\n", last_sleep_time_msec);
		const bool do_not_exceed_max_framerate = true;
		if (!do_not_exceed_max_framerate) {
			last_sleep_time_msec = ZZ_MIN(last_sleep_time_msec, max_sleep_msec);
		}
		::Sleep(last_sleep_time_msec);
	}
	else {
		::Sleep(1); // minimum sleep
	}

	start = timer.get_time();
}

bool zz_system::manager_flush_delayed (zz_manager * man, bool flush_entrance, bool flush_exit)
{
	if (!man) return true;
	if (!man->get_lazy()) return true;

	bool ret_entrance(true), ret_exit(true);

	if (flush_entrance) {
		ret_entrance = man->flush_entrance_all();
		if (!ret_entrance) {
			ZZ_LOG("system: flush_delayed() failed. %s/entrance.\n", man->get_name());
		}
	}
	
	if (flush_exit) {
		ret_exit = man->flush_exit_all();
		if (!ret_exit) {
			ZZ_LOG("system: flush_delayed() failed. %s/exit.\n", man->get_name());
		}
	}
	return (ret_entrance && ret_exit);
}

// Flush all delayed manager state
bool zz_system::flush_delayed (bool flush_entrance, bool flush_exit)
{
	bool ret = true;
	ret &= manager_flush_delayed(textures, flush_entrance, flush_exit);
	ret &= manager_flush_delayed(meshes, flush_entrance, flush_exit);
	ret &= manager_flush_delayed(terrain_meshes, flush_entrance, flush_exit);
	ret &= manager_flush_delayed(ocean_meshes, flush_entrance, flush_exit);
	return ret;
}

#ifndef ZZ_IGNORE_TRIGGERVFS
zz_vfs_pkg_system * zz_system::set_pkg_system (const char * filesystem_name_in)
{
	assert(!pkg_system_);
	assert(!pkg_system_worker_);

	if (pkg_system_ || pkg_system_worker_) {
		ZZ_LOG("system: open_pkg_system(%s) failed. already exist.\n", filesystem_name_in);
		return NULL; 
	}

	//--------------------------------------------------------------------------------
	// for main thread
	pkg_system_ = zz_new zz_vfs_pkg_system; // will be deleted in destructor
	if (!pkg_system_->open_filesystem(filesystem_name_in)) {
		ZZ_SAFE_DELETE(pkg_system_);
		ZZ_LOG("system: set_pkg_system(%s) failed.\n", filesystem_name_in);
		return NULL;
	}
	//--------------------------------------------------------------------------------

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
	//--------------------------------------------------------------------------------
	// set another pkg_system for vfs_thread
	pkg_system_worker_ = zz_new zz_vfs_pkg_system; // will be deleted in destructor
	if (!pkg_system_worker_->open_filesystem(filesystem_name_in)) {
		ZZ_SAFE_DELETE(pkg_system_worker_);
		ZZ_LOG("system: set_pkg_system(%s:2) failed.\n", filesystem_name_in);
		return NULL;
	}
	
	// vfs_thread should be called after open_filesystem() or set_filesystem()
	// because, vfs_pkg is thread-unsafe.
	vfs_thread = zz_new zz_vfs_thread;
	vfs_thread->set_worker_pkg_system(pkg_system_worker_);
	vfs_thread->thread_start();
	vfs_thread->set_thread_priority(zz_thread::BELOW_NORMAL);
	//--------------------------------------------------------------------------------
#endif

	return pkg_system_;
}

zz_vfs_pkg_system * zz_system::get_pkg_system ()
{
	return pkg_system_;
}

#endif // ZZ_IGNORE_TRIGGERVFS
