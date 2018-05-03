/** 
 * @file zz_mesh_terrain.CPP
 * @brief terrain mesh class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-oct-2003
 *
 * $Header: /engine/include/zz_mesh_ishared.h 8     04-07-21 12:11p Zho $
 */

#ifndef __ZZ_MESH_ISHARED_H__
#define __ZZ_MESH_ISHARED_H__

#ifndef __ZZ_MESH_H__
#include "zz_mesh.h"
#endif

#ifndef __ZZ_POINTER_H__
#include "zz_pointer.h"
#endif

// number of quadrilateral on one axis
// this will be used for key to access shared index list
enum INDEX_ORDER {
	OCEAN_INDEX_ORDER = 11,
	TERRAIN_INDEX_ORDER = 0,
	ROUGH_TERRAIN_INDEX_ORDER = 12,
	NUM_INDEX_ORDER = 13
};

class zz_manager_mesh_ishared;

// This is for square terrain(or ocean) mesh.
// : can share single index buffer.
// : construct square mesh.
//--------------------------------------------------------------------------------
class zz_mesh_ishared : public zz_mesh {
//--------------------------------------------------------------------------------
	friend class zz_manager_mesh_ishared;

public:
	zz_mesh_ishared (int key_in = 0);
	virtual ~zz_mesh_ishared ();

	virtual bool load ();
	virtual bool unload ();
	virtual void init_reuse ();

	void set_num_faces (uint16 num_faces_in);

	virtual void create_index_buffer ();
	virtual void destroy_index_buffer ();
	virtual void update_index_buffer ();

	// initialize/destroy static member variables
	static void init_static (int key_in); // prepare s_indices for this key
	static void dest_static (int key_in); // clear s_inidices for this key
	static void init_static_all (); // initialize(clear) all s_indices data
	static void dest_static_all (); // destroy all s_indices data

	virtual bool restore_device_objects (); // create unmanaged objects
	virtual bool init_device_objects (); // create managed objects
	virtual bool invalidate_device_objects (); // destroy unmanaged objects
	virtual bool delete_device_objects (); // destroy managed objects

	void set_index_order (int index_order_in);

	int get_index_order ()
	{
		return key;
	}

protected:
	struct zz_index {
		zz_index_buffer * ibuf_res;
		uint16 * ibuf;
		uint16 num_indices;
	};

	// key is the index to s_indices
	// key is mapped to number of faces in vertical
	int key; // key to index s_indices. 0 <= key < MAX_INDEX_ORDER

	static zz_index s_indices[NUM_INDEX_ORDER];
	
	static zz_index * get_shared_indices ()
	{
		return s_indices;
	}


public:
	ZZ_DECLARE_DYNAMIC(zz_mesh_ishared)
};

#endif // __ZZ_MESH_ISHARED_H__