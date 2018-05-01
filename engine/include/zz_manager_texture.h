/** 
 * @file zz_manager_texture.h
 * @brief texture manager class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-jun-2004
 *
 * $Header: /engine/include/zz_manager_texture.h 5     04-09-15 10:40a Zho $
 */

#ifndef __ZZ_MANAGER_TEXTURE_H__
#define __ZZ_MANAGER_TEXTURE_H__

#ifndef __ZZ_MANAGER_H__
#include "zz_manager.h"
#endif

#include <list>

class zz_texture;

class zz_manager_texture : public zz_manager {
protected:
	std::list<zz_texture*> bind_next_list; // to be binded in the next frame list
	std::vector<zz_texture*> texlist;

	virtual void sort_waitings ();

public:
	zz_manager_texture ();
	virtual ~zz_manager_texture ();
	
	virtual bool unbind_object_textures (); // unbind all textures (except for image texture)

	// flush the texture in the next frame.
	// this texture will not be in queue and flushed directly.
	void bind_next_frame (zz_texture * tex); // flush texture in the next frame. not in queue

	void update_delayed (); // update delayed

	void unbind_notset (); // unbind textures if not set recently

	unsigned int get_texlist_size () const // call this after unbind_notset()
	{
		return texlist.size();
	}

	ZZ_DECLARE_DYNAMIC(zz_manager_texture);
};

#endif // _ZZ_MANAGER_TEXTURE_H__