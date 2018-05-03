/** 
 * @file zz_index_buffer.h
 * @brief index buffer class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-jan-2004
 *
 * $Header: /engine/include/zz_index_buffer.h 7     04-07-06 2:52p Zho $
 */

#ifndef __ZZ_INDEX_BUFFER_H__
#define __ZZ_INDEX_BUFFER_H__

#ifndef __ZZ_DEVICE_RESOURCE_H__
#include "zz_device_resource.h"
#endif

//--------------------------------------------------------------------------------
class zz_index_buffer : public zz_device_resource {
//--------------------------------------------------------------------------------
protected:
	int size_total; // buffer size. in bytes

public:

	zz_index_buffer ();
	~zz_index_buffer ();

	static int get_index_size ()
	{
		return sizeof(unsigned short);
	}

	// virtuals from zz_device_resource
	void create_buffer (uint16 num_indices_in);
	void update_buffer (const void * buffer_in);
	void destroy_buffer ();
};

#endif // __ZZ_INDEX_BUFFER_H__