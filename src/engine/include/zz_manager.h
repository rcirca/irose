/** 
 * @file zz_manager.h
 * @brief manager.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    25-feb-2002
 *
 * $Header: /engine/include/zz_manager.h 12    04-09-20 1:39p Zho $
 */

#ifndef __ZZ_MANAGER_H__
#define __ZZ_MANAGER_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_WAITING_LINE_H__
#include "zz_waiting_line.h"
#endif

//--------------------------------------------------------------------------------
// zz_manager class :
//
// - manages all children nodes in similar manner,
// - takes charge of creating and deleting node (ex, spawn(), kill()),
// - is in charge of device restoring(ex, invalidate_device_objects()...)
//--------------------------------------------------------------------------------
class zz_manager : public zz_node {
friend class zz_system;
	typedef bool (zz_node::* zz_device_objects_func)(); // for [init/invalidate/restore/delete]_device_objects()

	// This class will be used in for_each() as parameter.
	class zz_device_objects_callback {
	private:
		zz_device_objects_func f_; // Device objects related function pointer
	public:
		// Constructor with device object funcion.
		// CAUTION: This class does not have default constructor.
		zz_device_objects_callback (zz_device_objects_func f) : f_(f) {}

		// ()operator which will be used by for_each()
		// This method calls node->xxxx_device_objects().
		bool operator() (zz_node * node) {
			if (!node) {
				return false;
			}
			return (node->*f_)(); // Internaly, suitable virtual function will be called.
		}
	};

protected:
	zz_node * _current; // current selected node
	zz_waiting_line<zz_node> entrance_line; // created but not-loaded node list
	zz_waiting_line<zz_node> exit_line; // deleted but not-unloaded node list
	bool is_lazy; // whether it is lazy device mode or not. default is false.
	zz_time entrance_time_accumulated; // accumulated time for entrance list from last update
	zz_time exit_time_accumulated; // accumulated time for exit list from last update
	unsigned int num_reuse; // number of reusing objects. default is 0

	virtual void sort_waitings () // sort waitings
	{
	}

public:
	// constructor/destructor
	zz_manager ();
	virtual ~zz_manager ();

	virtual zz_node * get_current (); // get currently selected node. 
	virtual zz_node * set_current (zz_node * node_to_set_current); // select a node
	virtual zz_node * set_current (const char * name_to_set); // select a node by name

	//--------------------------------------------------------------------------------
	// Node lifetime :
	// spawn() -> (setting up...) -> load() -> (do something...) -> kill()
	// 
	//--------------------------------------------------------------------------------
	// Creates a new instance of class with type *node_type_to_spawn*
	// If *do_load* is true, the new object will be auto-loaded in device or memory after creation.
	zz_node * spawn (const char * baby_name, zz_node_type * node_type_to_spawn, bool do_load = true);
	
	// Find node by name, and if has any, then return it or create new one.
	zz_node * find_or_spawn (const char * baby_name, zz_node_type * node_type_to_spawn);
	
	// Removes a node by its pointer.
	bool kill (zz_node * node);

	// Load node into device or memory from file.
	// Normally, it reads a file and upload it to graphics device.
	void load (zz_node * node);

	// Unload node from device or memory and free instance.
	void unload (zz_node * node);

	// Frees all children's memory by delete operation.
	size_t release_children ();

	// For each child node, this method calls xxxx_device_objects() in system::xxxx_device_objects()
	void for_each (zz_device_objects_callback callback);

	// Updates all children nodes with update time.
	// This will causes entrance_line->flush() or exit_line->flush().
	void update (zz_time time_to_update);

	//--------------------------------------------------------------------------------
	// entrance_line : list, containing nodes which are waiting to enter into the device.
	// exit_line : list, contaning nodes which are waiting to exit from the device.
	//--------------------------------------------------------------------------------
	// adjust or flush entrance/exit line
	bool promote_entrance (zz_node * node);
	bool promote_exit (zz_node * node);
	bool flush_entrance (zz_node * node);
	bool flush_exit (zz_node * node);
	bool flush_entrance_all (); // flush entrance line
	bool flush_exit_all (); // flush exit line
	size_t get_entrance_size ();
	size_t get_exit_size ();
	bool remove_exit (zz_node * node);
	bool remove_entrance (zz_node * node);

	bool find_entrance (zz_node * node);
	bool find_exit (zz_node * node);

	// for lazy device update mode
	void set_lazy (size_t num_size_in);
	bool get_lazy ();

	// Set/get num_reuse.
	// The manager reuses already spawned node when new request come.
	// If num_reuse is zero, no reusing is done.
	// In every update, num_reuse nodes are not unloaded.
	// set_num_reuse() is called in zz_system instance at initializing phase.
	// the default value of num_reuse is zero.
	void set_num_reuse (unsigned int num_reuse_in);
	unsigned int get_num_reuse ();

	ZZ_DECLARE_DYNAMIC(zz_manager);
};

inline void zz_manager::set_num_reuse (unsigned int num_reuse_in)
{
	num_reuse = num_reuse_in;
}

inline unsigned int zz_manager::get_num_reuse ()
{
	return num_reuse;
}

inline void zz_manager::set_lazy (size_t num_size_in)
{
	if (num_size_in > 0) {
		is_lazy = true;
		entrance_line.set(zz_waiting_line<zz_node>::FOR_ENTRANCE, num_size_in);
		exit_line.set(zz_waiting_line<zz_node>::FOR_EXIT, num_size_in);
	}
	else {
		is_lazy = false;
		entrance_line.flush_n_pop((unsigned int)entrance_line.size());
		exit_line.flush_n_pop((unsigned int)exit_line.size());
	}
}

inline bool zz_manager::get_lazy ()
{
	return is_lazy;
}

#endif // __ZZ_MANAGER_H__