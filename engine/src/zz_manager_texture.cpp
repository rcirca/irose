/** 
 * @file zz_manager_texture.cpp
 * @brief texture manager class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-jan-2004
 *
 * $Header: /engine/src/zz_manager_texture.cpp 8     04-09-16 6:23p Zho $
 */

#include "zz_tier0.h"
#include "zz_manager_texture.h"
#include "zz_system.h"
#include "zz_texture.h"
#include "zz_profiler.h"
#include "zz_renderer.h"
#include "zz_camera.h"

#include <algorithm>
#include <list>

ZZ_IMPLEMENT_DYNCREATE(zz_manager_texture, zz_manager)

zz_manager_texture::zz_manager_texture ()
{
}

zz_manager_texture::~zz_manager_texture ()
{
}

class texture_settime_compare : public std::binary_function<zz_texture *, zz_texture *, bool> 
{
public:
	bool operator() (const zz_texture* lhs, const zz_texture* rhs) const 
	{
		return lhs->get_last_settime() < rhs->get_last_settime();
	}
};

void zz_manager_texture::bind_next_frame (zz_texture * tex)
{
	assert(bind_next_list.size() < 1000);

	bind_next_list.push_back(tex);
}

void zz_manager_texture::update_delayed ()
{
	std::list<zz_texture*>::iterator it(bind_next_list.begin()), it_end(bind_next_list.end());
	
	while (it != it_end) {
		assert(*it);
		(*it++)->load();
	}
	bind_next_list.clear();
}

//#define UNBIND_ALL // unbind every case.

void zz_manager_texture::unbind_notset ()
{
	zz_hash_table<zz_node*>::iterator it, it_end;
	zz_hash_table<zz_node*> * nodes = get_hash_table();
	zz_texture * tex;

	if (!nodes) return;

	zz_camera * cam = znzin->get_camera();

	if (cam) {
		if (cam->get_speed() > 0) {
			// now, camera is moving, so do not unbind textures
			//ZZ_LOG("manager_texture: camera is moving\n");
			return;
		}
	}

	unsigned int max_tex_mem = znzin->renderer->get_max_texmem() >> 20; // byte -> megabyte
	unsigned int ava_tex_mem = znzin->renderer->get_available_texmem() >> 20;
	const unsigned int suff_tex_mem = 20; // sufficient available texture memory 20 MB
	
#if !defined(UNBIND_ALL)
	if (ava_tex_mem > suff_tex_mem) {
		if ((ava_tex_mem * 3) > max_tex_mem) {
			return;
		}
	}
#endif

	static zz_time s_accum_time = 0; // accumulated time
	const zz_time max_accum_time = ZZ_MSEC_TO_TIME(1000); // default 1000 msec

	s_accum_time += znzin->get_diff_time();

	// we do not unbind in every frame. we do only after some time elapsed
	// and the ratio is (ava_tex_mem / max_tex_mem)
	if (s_accum_time*max_tex_mem < max_accum_time*ava_tex_mem) // s_accum_time < max_accum_time * (ava_tex_mem / max_tex_mem)
		return;

	zz_time current_time = znzin->get_current_time();
	const zz_time MAX_DIFF_TIME = ZZ_MSEC_TO_TIME(10000); // default 10 second

	texlist.clear();

	for (it = nodes->begin(), it_end = nodes->end(); it != it_end; ++it) {
		tex = static_cast<zz_texture*>(*it);

		if (tex->get_lock_texture()) // skip texture that was locked for some reason
			continue;

		if (tex->get_last_settime() == current_time) // skip if it was used in the current frame
			continue;

		if (!tex->get_device_updated()) // skip if it was not device-ready
			continue;

		if (entrance_line.find(tex)) // skip if it is manager-controlled. it will be flushed by manager queue mechanism
			continue;

		if (exit_line.find(tex)) // skip if it is manager-controlled. it will be flushed by manager queue mechanism
			continue;

		texlist.push_back(tex); // insert the texture into the texlist
	}

	if (texlist.empty()) // skip if the texlist is empty
		return;

	texture_settime_compare texture_cf; // declare texture comparision function

	// sort texture list by last_settime
	std::sort(texlist.begin(), texlist.end(), texture_cf);

	tex = *texlist.begin(); // consider only the first
	assert(tex);

	// skip if the texture is recently used.
	if ((current_time - tex->get_last_settime()) < MAX_DIFF_TIME)
		return;

	s_accum_time = 0; // intialize accum_time

	tex->unbind_device();
	tex->update_last_settime();
}

// unbind object textures to change loading scheme.
// this method will be called to change texture loading quality level.
bool zz_manager_texture::unbind_object_textures ()
{
	zz_hash_table<zz_node*>::iterator it, it_end;
	zz_hash_table<zz_node*> * nodes = get_hash_table();
	zz_texture * tex;

	if (!nodes) return true;

	for (it = nodes->begin(), it_end = nodes->end(); it != it_end; ++it) {
		tex = static_cast<zz_texture*>(*it);

		if (tex->get_for_image()) { // no need to unbind sprite image textures as these texture's reloading scheme is not changed
			continue;
		}

		if (tex->get_lock_texture()) // skip if the texture is locked
			continue;

		tex->unbind_device();
	}

	return true;
}

void zz_manager_texture::sort_waitings ()
{
	entrance_line.sort_line();
	//entrance_line.dump ();
}