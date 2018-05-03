/** 
 * @file zz_texture.cpp
 * @brief texture class.
 * @author	Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date 18-apr-2002
 *
 * $Header: /engine/src/zz_texture.cpp 41    04-10-01 5:48p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font.h"
#include "zz_font_d3d.h"
#include "zz_texture.h"
#include "zz_system.h"
#include "zz_renderer.h"
#include "zz_profiler.h"
#include "zz_manager.h"
#include "zz_vfs.h"

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
#include "zz_vfs_thread.h"
#endif

//#define LOG_TEXTURE

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#endif

ZZ_IMPLEMENT_DYNCREATE(zz_texture, zz_node)

zz_texture::zz_texture() :
	last_time(0),
	mod_time(0),
	width(0),
	height(0),
	handle(ZZ_HANDLE_NULL),
	miplevels(-1),
	use_filter(true),
	format(ZZ_FMT_UNKNOWN),
	for_image(false),
	load_priority(1000000),
	last_settime(0),
	texture_locked(false)
{
	load_weight = 1;
	res.set_pool( zz_device_resource::ZZ_POOL_MANAGED );
}

zz_texture::~zz_texture ()
{
#ifdef USE_VFS_THREAD_TEXTURE_LOADING
	// close vfs_thread file that was opend in set_path()
	if (znzin->vfs_thread->is_valid_handle(file_handle)) {
		assert(znzin && znzin->vfs_thread);
		znzin->vfs_thread->close(file_handle); // opened in set_path() or download_texture()
		file_handle = zz_vfs_thread::zz_item_it(); // initialize
	}
#endif
	unload();
}

#if (0)
bool get_file_timestamp (const char * file_name, uint64 * t)
{
	ZZ_PROFILER_INSTALL(Pget_file_timestamp); // 0.260635

	char new_name[ZZ_MAX_STRING];
	if (zz_vfs::check_and_get_path(new_name, file_name) == false) return false;

	HANDLE hfile = ::CreateFile(
		new_name,
		GENERIC_READ, 
		0, // FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (hfile == INVALID_HANDLE_VALUE) return false;

	BY_HANDLE_FILE_INFORMATION file_info;
	FILETIME write_time;

	::GetFileInformationByHandle(hfile, &file_info);

	write_time = file_info.ftLastWriteTime; // this value represent the number of 100 nanosecond intervals
	ULARGE_INTEGER large_int;
	large_int.LowPart = write_time.dwLowDateTime;
	large_int.HighPart = write_time.dwHighDateTime;

	// to convert to millisecond intervals
	//*t = static_cast<uint64>(large_int.QuadPart / 10000); // 100 nano-second = 1/10000 milli-second
	*t = static_cast<uint64>(large_int.QuadPart); // 100 nano-second = 1/10000 milli-second
	::CloseHandle(hfile);

	return true;
}
#endif

bool get_file_timestamp (const char * file_name, uint64 * t)
{
	//ZZ_PROFILER_INSTALL(Pget_file_timestamp); // 0.231594
	return znzin->file_system.get_mtime(file_name, t);
}

bool zz_texture::load ()
{
	last_settime = znzin->get_current_time();
	return bind_device();
}

// unload() called by manager->kill
bool zz_texture::unload ()
{
	return unbind_device();
}

bool zz_texture::set (int stage)
{
	last_settime = znzin->get_current_time(); // update_last_settime()

	// move to top and then flush it
	if (!res.get_updated()) {
		assert(!ZZ_HANDLE_IS_VALID(this->handle));
	}
	return znzin->renderer->set_texture(handle, stage, get_path());
}

bool zz_texture::changed (void)
{
	if (res.get_dynamic()) return false;

	bool ret = false;
	uint64 last_time = mod_time;
	if (get_file_timestamp(get_path(), &mod_time)) {
		//ZZ_LOG("texture: mod_time = %s", ctime(&this->mod_time));
		return (mod_time > last_time);
	}
	// restore
	mod_time = last_time;
	return ret;
}

bool zz_texture::reload (void)
{
	if (res.get_dynamic()) return true;
	if (!res.get_updated()) return false;

	bool ret = false;

	// if (!changed()) return false;
	if (!unbind_device()) return false;
	bind_device();

	return false;
}

// CAUTION: if the same texture exists, return true.
bool zz_texture::load_real (zz_texture * tex)
{
	assert(!ZZ_HANDLE_IS_VALID(tex->handle));

	tex->handle = znzin->renderer->create_texture(tex);

#ifdef LOG_TEXTURE
	ZZ_LOG("texture: create_texture(%s-%dx%d, %d)\n",
		tex->get_path(), tex->get_width(), tex->get_width(), tex->handle);
#endif		

	assert(ZZ_HANDLE_IS_VALID(tex->handle));

	return true;
}

// create managed/systemmem/scratch pool objects
bool zz_texture::init_device_objects ()
{
	if (!res.is_init_device_objects()) { // skip default
		return true;
	}

	if (res.get_updated())
		return true;

	if (!load_real(this))
		return false;

	res.make_updated();

	return true;
}

// destroy managed/systemmem/scratch pool objects
bool zz_texture::delete_device_objects ()
{
	if (!res.is_delete_device_objects()) { // skip default
		return true;
	}

	if (!res.get_created()) {
		zz_assert(handle == ZZ_HANDLE_NULL);
		return true;
	}

#ifdef LOG_TEXTURE
	ZZ_LOG("texture: destroy_texture1(%s-%dx%d, %d)\n",
		get_path(), get_width(), get_width(), handle);
#endif

	znzin->renderer->destroy_texture(this);

	res.make_uncreated();
	handle = ZZ_HANDLE_NULL;
	
	return true;
}

// create default pool objects
bool zz_texture::restore_device_objects ()
{
	if (!res.is_restore_device_objects()) { // skip managed, systemmem, scratch
		return true;
	}

	if (res.get_updated())
		return true;

	if (!load_real(this))
		return false;

	res.make_updated();

	return true;
}

// destroy default_pool objects
bool zz_texture::invalidate_device_objects ()
{
	if (!res.is_invalidate_device_objects()) { // skip managed, systemmem, scratch
		return true;
	}

	if (!res.get_created()) {
		zz_assert(handle == ZZ_HANDLE_NULL);
		return true;
	}

	
#ifdef LOG_TEXTURE
	ZZ_LOG("texture: destroy_texture1(%s-%dx%d, %d)\n",
		get_path(), get_width(), get_width(), handle);
#endif

	znzin->renderer->destroy_texture(this);

	res.make_uncreated();
	handle = ZZ_HANDLE_NULL;
	
	return true;
}

bool zz_texture::set_path (const char * path_in)
{
	const char * last_path = file_name.get();

	zz_assert(path_in);
	zz_assertf(znzin->file_system.exist(path_in), "[%s] file not found\n", path_in);

	// not to rename old
	zz_assertf(file_name.get() == 0, "rename [%s] failed", file_name.get());

	// set path
	file_name.set(path_in);
	//file_name.set("etc/shadowover.dds");
	// request to open and delayed-load. autu-closed in renderer_d3d->download_texture()
#ifdef USE_VFS_THREAD_TEXTURE_LOADING
	file_handle = znzin->vfs_thread->open(path_in); // will be closed in destructor or download_texture()
#endif

	// set weight by file size
	// CAUTION: only for vfs_local or vfs_pkg
	const int average_filesize = 100000; // 100 KB
	const int load_byte_per_msec = 1000; // 1 KB

#if (1)
	int filesize = znzin->file_system.get_size(path_in);
	load_weight = 1 + filesize/load_byte_per_msec; // 1(minimum size) + 1ms/10000byte
#endif

	res.make_ready(); // Just setting path is enough to be loaded.

	return true;
}

void zz_texture::set_property (
	const char * path_in, int width_in, int height_in, bool is_dynamic_in, int miplevels_in,
	bool use_filter_in, zz_device_resource::zz_resource_pool pool_in,
	ZZ_FORMAT format_in,
	bool for_image_in)
{
	zz_assert(!res.get_created());

	//if (res.get_created()) {
	//	unbind_device(); // makes res.buffered = false
	//}

	// if dynamic, and then always use default pool.
	assert(!is_dynamic_in || (is_dynamic_in && (pool_in == zz_device_resource::ZZ_POOL_DEFAULT)));
	
	// if not support dynamic textures
	if (is_dynamic_in && !znzin->get_rs()->use_dynamic_textures) {
		is_dynamic_in = false;
		pool_in = zz_device_resource::ZZ_POOL_DEFAULT;
	}

	for_image = for_image_in; // usage for image texture

	res.set_dynamic(is_dynamic_in);
	res.set_pool(pool_in);
	width = width_in;
	height = height_in;
	load_weight = 1;
	miplevels = miplevels_in;
	use_filter = use_filter_in;
	format = format_in;
	if (path_in) {
		set_path(path_in);
	}
	return;
}

bool zz_texture::flush_device (bool immediate)
{
	if (res.get_updated()) return true;

	if (immediate) {
		if (znzin->textures->flush_entrance(this)) {
			last_settime = znzin->get_current_time(); // update_last_settime
			return true;
		}
		return false;
	}
	else {
		znzin->textures->promote_entrance(this);
		return false;
	}
}

void zz_texture::update_last_settime ()
{
	last_settime = znzin->get_current_time();
}
