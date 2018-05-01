/** 
 * @file zz_texture.h
 * @brief texture class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-feb-2002
 *
 * $Header: /engine/include/zz_texture.h 23    04-09-15 10:40a Zho $
 */

#ifndef __ZZ_TEXTURE_H__
#define __ZZ_TEXTURE_H__

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#include <list>

#include <time.h> // for time_t

#ifndef __ZZ_DEVICE_RESOURCE_H__
#include "zz_device_resource.h"
#endif

#ifndef __ZZ_SYSTEM_H__
#include "zz_system.h"
#endif

#ifndef __ZZ_VFS_THREAD_H__
#include "zz_vfs_thread.h"
#endif

//#define USE_VFS_THREAD_TEXTURE_LOADING

// all textures are MANAGED texture objects
class zz_texture : public zz_node {
protected:
	uint64 last_time; // last checked time
	uint64 mod_time;  // modified time for reload checking in changed().

	// last texture used time by set()
	// set() updates last_settime.
	// if last_settime is the current time, then we consider that the texture was used in the scene.
	// thus, we should not unload it if texture memory is insufficient.
	zz_time last_settime;

	zz_handle handle; // the handle by which we can access the d3d texture pointer.

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
	zz_vfs_thread::zz_item_it file_handle; // the handle by which we can access the texture file opened by thread
#endif

	zz_string file_name; // texture file name

	// resource management information
	zz_device_resource res; // vertex buffer resource info

	int width, height; // used only if dynamic
	int miplevels; // default is -1(use of .dds), 0(all), 1-n(use n)
	bool use_filter; // default is true
	ZZ_FORMAT format; // color format defined in zz_renderer.h
	
	bool for_image; // whether this texture is for image texture(ex. used by draw_sprite)

	static bool load_real (zz_texture * tex); // actually load texture

	int load_priority;

	// lock state which indicates this texture cannot be unloaded automatically.
	// if this is true, manager_texture::unbind_object_textures() and unbind_notset() will not unload it.
	// ex) ocean textures that is animated textures.
	// ex) textures that client application uses directly by get_texture() or get_texture_pointer()
	bool texture_locked; 

public:
	bool changed (); // returns true, if texture file was modified

	zz_texture();
	virtual ~zz_texture ();

	// resource management
	virtual bool load (); // we assume that the texture file name is the same as the node-name.
	virtual bool unload ();

	// for restoring device
	virtual bool restore_device_objects (); // create unmanaged objects
	virtual bool init_device_objects (); // create managed objects
	virtual bool invalidate_device_objects (); // destroy unmanaged objects
	virtual bool delete_device_objects (); // destroy managed objects

	bool set (int stage = 0); // set texture
	bool reload (); // reload texture from disk. returns true if reloaded.
	
	zz_handle get_texture_handle () const;
	bool get_dynamic () const;

	void set_texture_handle (zz_handle handle_in)
	{
		handle = handle_in;
	}

	int get_width () const;
	int get_height () const;

	void set_width ( int width ); // call by renderer::download_texture()
	void set_height ( int height ); // call by renderer::download_texture()

	bool set_path (const char * path_in);
	const char * get_path () const;

	void set_miplevels (int miplevels_in);
	int get_miplevels () const ;

	void set_filter (bool use_filter_in);
	bool get_filter () const;

	zz_device_resource::zz_resource_pool get_pool () const;

	void set_property (const char * path_in, int width_in, int height_in,
		bool is_dynamic_in, int miplevels_in, bool use_filter_in,
		zz_device_resource::zz_resource_pool pool_in,
		ZZ_FORMAT format_in  = ZZ_FMT_UNKNOWN,
		bool for_image_in = false);

	ZZ_FORMAT get_format () const;

	bool get_for_image () const;

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
	// set/get file handle
	zz_vfs_thread::zz_item_it get_file_handle () const;
	void set_file_handle ( zz_vfs_thread::zz_item_it file_handle_in );
#endif

	// resource was updated or not
	bool get_device_updated () const;

	// force to be buffed in hardware.
	bool flush_device (bool immediate);

	virtual int get_load_priority () const
	{
		return load_priority;
	}

	void set_load_priority (int priority_in)
	{
		if (priority_in < load_priority)
			load_priority = priority_in;
	}

	zz_time get_last_settime () const
	{
		return last_settime;
	}

	void update_last_settime (); // update last_settime to current time

	void lock_texture ()
	{
		texture_locked = true;
	}

	void unlock_texture ()
	{
		texture_locked = false;
	}

	bool get_lock_texture () const
	{
		return texture_locked;
	}

	ZZ_DECLARE_DYNAMIC(zz_texture)
};

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
inline zz_vfs_thread::zz_item_it zz_texture::get_file_handle () const
{
	return file_handle;
}

inline void zz_texture::set_file_handle ( zz_vfs_thread::zz_item_it file_handle_in )
{
	file_handle = file_handle_in;
}
#endif

inline zz_handle zz_texture::get_texture_handle () const
{
	return handle;
}

inline bool zz_texture::get_dynamic () const 
{
	return res.get_dynamic();
}

inline int zz_texture::get_width () const 
{
	return width;
}

inline int zz_texture::get_height () const 
{
	return height;
}

inline const char * zz_texture::get_path () const
{
	return file_name.get();
}

inline void zz_texture::set_miplevels (int miplevels_in)
{
	miplevels = miplevels_in;
}

inline int zz_texture::get_miplevels () const 
{
	return miplevels;
}

inline void zz_texture::set_filter (bool use_filter_in)
{
	use_filter = use_filter_in;
}

inline bool zz_texture::get_filter () const
{
	return use_filter;
}


inline zz_device_resource::zz_resource_pool zz_texture::get_pool () const
{
	return res.get_pool();
}

inline ZZ_FORMAT zz_texture::get_format () const
{
	return format;
}

inline bool zz_texture::get_for_image () const
{
	return for_image;
}

inline bool zz_texture::get_device_updated () const
{
	return res.get_updated();
}

inline void zz_texture::set_width ( int width_in )
{
	this->width = width_in;
}

inline void zz_texture::set_height ( int height_in )
{
	this->height = height_in;
}



#endif // __ZZ_TEXTURE_H__