/** 
 * @file zz_vertex_buffer.h
 * @brief vertex buffer class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-jan-2004
 *
 * $Header: /engine/include/zz_vertex_buffer.h 4     04-07-05 9:37p Zho $
 */

#ifndef __ZZ_VERTEX_BUFFER_H__
#define __ZZ_VERTEX_BUFFER_H__

#ifndef __ZZ_DEVICE_RESOURCE_H__
#include "zz_device_resource.h"
#endif

//--------------------------------------------------------------------------------
class zz_vertex_buffer : public zz_device_resource {
//--------------------------------------------------------------------------------
protected:
	int size_total; // total size(in bytes). passes to renderer

public:

	zz_vertex_buffer ();
	~zz_vertex_buffer ();

	// virtuals from zz_device_resource
	void create_buffer (int size_total_in);
	void update_buffer (const void * buffer_in);
	void destroy_buffer ();
};

#endif // __ZZ_VERTEX_BUFFER_H__