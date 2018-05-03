/** 
 * @file zz_device_resource.h
 * @brief device resource class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-nov-2003
 *
 * $Header: /engine/include/zz_device_resource.h 8     04-10-08 7:04p Zho $
 */

#ifndef __ZZ_DEVICE_RESOURCE_H__
#define __ZZ_DEVICE_RESOURCE_H__

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#ifndef __ZZ_REFCOUNT_H__
#include "zz_refcount.h"
#endif

#include <assert.h>

class zz_device_resource {
public:
	enum zz_resource_pool { // same as D3DPOOL in d3d9types.h
		ZZ_POOL_DEFAULT = 0,
		ZZ_POOL_MANAGED = 1,
		ZZ_POOL_SYSTEMMEM = 2,
		ZZ_POOL_SCRATCH = 3,
		ZZ_POOL_FORCE_DWORD = 0x7fffffff // TO BE USED IN D3D RENDERER INTERFACE
	};
	
	enum zz_resource_state {
		ZZ_STATE_INIT = 0, // initial state
		ZZ_STATE_READY = 1 << 0, // ready to create. system memory things are ready.
		ZZ_STATE_CREATED = 1 << 1, // created in h/w.
		ZZ_STATE_UPDATED = 1 << 2, // update in h/w.
	};

protected:
	zz_resource_pool pool; 
	zz_resource_state state; // state. none->ready->created->updated
	bool dynamic; // whether it is dynamic or not
	zz_handle device_handle;

public:
	zz_device_resource() :
		pool(ZZ_POOL_DEFAULT),
		state(ZZ_STATE_INIT),
		dynamic(false),
		device_handle(ZZ_HANDLE_NULL)
	{
	}

	zz_device_resource(const zz_device_resource& res_in) :
		pool(res_in.pool),
		state(res_in.state),
		dynamic(res_in.dynamic),
		device_handle(ZZ_HANDLE_NULL)
	{
	}

	zz_device_resource(zz_resource_pool pool_in, zz_resource_state state_in, bool dynamic_in) :
		pool(pool_in),
		state(state_in),
		dynamic(dynamic_in),
		device_handle(ZZ_HANDLE_NULL)
	{
	}

	// get state
	const zz_resource_state& get_state () const
	{
		return state;
	}

	// if it's time to create
	bool to_create () const
	{
		return (get_ready() && !get_created());
	}
	
	// if it's time to update
	bool to_update () const
	{
		return get_created() && !get_updated();
	}

	// if it's time to destroy
	bool to_destroy () const
	{
		return get_created();
	}

	// make current state to ready
	void make_ready ()
	{
		assert(!get_ready());
		state = zz_resource_state(int(state) | int(ZZ_STATE_READY));
	}

	// make current state to created
	void make_created ()
	{
		assert(!get_created());
		state = zz_resource_state(int(state) | int(ZZ_STATE_READY) |  int(ZZ_STATE_CREATED));
	}

	// make current state to updated
	void make_updated ()
	{
		// can be already-updated
		state = zz_resource_state(int(state) | int(ZZ_STATE_READY) | int(ZZ_STATE_CREATED) | int(ZZ_STATE_UPDATED));
	}

	void make_init ()
	{
		state = ZZ_STATE_INIT;
	}

	void make_unready ()
	{
		state = zz_resource_state(int(state) & ~(int(ZZ_STATE_READY)));
	}

	void make_uncreated ()
	{
		state = zz_resource_state(int(state) & ~(int(ZZ_STATE_CREATED) | int(ZZ_STATE_UPDATED)));
	}

	void make_unupdated ()
	{
		state = zz_resource_state(int(state) & ~(int(ZZ_STATE_UPDATED)));
	}

	bool get_ready () const
	{
		return (state & ZZ_STATE_READY) > 0;
	}

	bool get_created () const
	{
		return (state & ZZ_STATE_CREATED) > 0;
	}

	bool get_updated () const
	{
		return (state & ZZ_STATE_UPDATED) > 0;
	}

	void set_pool (zz_resource_pool pool_in)
	{
		pool = pool_in;
	}

	const zz_resource_pool& get_pool () const
	{
		return pool;
	}

	void set_dynamic (bool dynamic_in)
	{
		dynamic = dynamic_in;
	}

	bool get_dynamic () const
	{
		return dynamic;
	}

	// return true if it's time to create/update managed, systemmem, scratch
	bool is_init_device_objects () const
	{
		// not managed, systemmem, scratch, do nothing
		if (pool == ZZ_POOL_DEFAULT)
			return false;
		return true;
	}

	// return true if it's time to destroy managed/systemmem/scratch pool objects
	bool is_delete_device_objects () const
	{
		// not managed, systemmem, scratch, do nothing
		if (pool == ZZ_POOL_DEFAULT)
			return false;
		return true;
	}

	// return true if it's time to create/update default pool objects
	bool is_restore_device_objects () const
	{
		// not default, do nothing
		if (pool != ZZ_POOL_DEFAULT)
			return false;
		return true;
	}

	// return true if it's time to destroy default_pool objects
	bool is_invalidate_device_objects () const
	{
		// not default, do nothing
		if (pool != ZZ_POOL_DEFAULT)
			return false;
		return true;
	}

	// used by renderer
	zz_handle get_device_handle () const
	{
		return device_handle;
	}
};

#endif // __ZZ_DEVICE_RESOURCE_H__