/** 
 * @file zz_node.h
 * @brief node.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    08-feb-2002
 *
 * $Header: /engine/include/zz_node.h 15    04-08-11 6:20p Zho $
 * $History: zz_node.h $
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-08-11   Time: 6:20p
 * Updated in $/engine/include
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-07-31   Time: 8:51p
 * Updated in $/engine/include
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-07-22   Time: 11:56a
 * Updated in $/engine/include
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-07-19   Time: 12:15p
 * Updated in $/engine/include
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-07-12   Time: 5:50p
 * Updated in $/engine/include
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-07-10   Time: 4:31p
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-07-05   Time: 9:37p
 * Updated in $/engine/include
 * vertex buffer redesigned
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-06-25   Time: 3:03a
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-03-18   Time: 3:17p
 * Updated in $/engine/include
 * virtualize unlink_node.
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-03-01   Time: 4:56p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-17   Time: 2:11p
 * Updated in $/engine/include
 * Added get_all_children() that collects all children recursively.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-13   Time: 1:20a
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-15   Time: 4:26p
 * Updated in $/engine/include
 * Redesigning mesh structure.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:53p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 * 
 * *****************  Version 20  *****************
 * User: Zho          Date: 03-11-27   Time: 4:50p
 * Updated in $/znzin11/engine/include
 * - removed zz_resource class, added full-support of resource management
 * into zz_node, and cleaned up some codes.
 */

#ifndef __ZZ_NODE_H__
#define __ZZ_NODE_H__

#ifndef __ZZ_POINTER_H__
#include "zz_pointer.h"
#endif

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#ifndef __ZZ_NODE_TYPE_H__
#include "zz_node_type.h"
#endif

#ifndef __ZZ_HASH_TABLE_H__
#include "zz_hash_table.h"
#endif

#ifndef __ZZ_STRING_H__
#include "zz_string.h"
#endif

#ifndef __ZZ_LIST_H__
#include "zz_list.h"
#endif

#include <vector>

//--------------------------------------------------------------------------------
// Node class:
//
// This class is based on tree-node structure.
// This class has reference-counting system, but does not use it.
//--------------------------------------------------------------------------------
class zz_node {
private:
	// Unlink child, for internal use.
	void _unlink_child (zz_node * child);

protected:
	// Last updated time
	zz_time timestamp;

	// For node searching
	// The node_name should be unique.
	zz_string node_name;
	
	// Node relation as a tree structure.
	zz_node * parent_node;
	zz_list<zz_node *> children;
	// The parent node uses *it_by_parent* to find this node directly in the children list of the parent.
	// This is assigned in link_child().
	zz_list<zz_node *>::iterator it_by_parent;
	typedef zz_list<zz_node *>::iterator child_it;

	// If the newly created node has the same name as existing node,
	// then rename it by adding the unique_count number.
	static int unique_count; // For unique name & handle generating

	// For simple reference counting.
	// Used in addref() and release()
	unsigned long refcount;

	// Resource loading time weight (ex. by filesize)
	int load_weight;

	// Runtime type checking
	static const zz_node_type type_zz_node; // class type itself

	// Root node and name hash table.
	// - for name-tree structure.
	// - shared by all family member nodes.
	// - created in set_to_root() method.
	struct str_node_system {
		zz_hash_table <zz_node *> name_hash_table;
		zz_node * root;
	};
	
	// The root node creates node_system, and other nodes just reference this.
	zz_pointer<str_node_system> node_system;
	
protected:
	// Allocating node_system. The allocated node_system will be auto-deallocated at destruction time.
	void set_to_root ();

	// Generate unique key by increasing number(_unique_count)
	// ex) noname01, noname02,....
	const char * make_unique_key ();
	
	// Generate unique key by adding number at the end of the key name
	// ex) mynodename, mynodename01, mynodename02,....
	const char * make_numbered_key (const char * key_name);

	// Used only by traverse_node()
	void traverse_child_node (int level);

public:
	// Constructor & destructor
	zz_node ();
	virtual ~zz_node ();

	// Child node iterating
	typedef zz_list<zz_node *> child_type;
	typedef zz_list<zz_node *>::iterator child_iterator;

	// Reference counting
	unsigned long addref (void);
	virtual unsigned long release (void);
	unsigned long get_refcount () const; // get reference counting

	// Name
	const char * get_name () const;
	bool set_name (const char * new_name);
	void remove_name (); // removes name hash key from this hash table, and set null to this object name

	// Find child node from this hierarchy structure, considering
	// that this node is the root node of the searching tree.
	zz_node * find (const char * name_to_get);

	// Find any node which has the same type as this node
	// from the root node.
	virtual zz_node * find_type (const char * name_to_get);
	
	// Get total number of children from this node.
	int get_num_children ();

	// Link child node into current node.
	virtual void link_child (zz_node * node_to_be_child);
	
	// Remove link to child node.
	virtual void unlink_child (zz_node * node_to_remove);
	
	// Get children list.
	child_type& get_children () { return this->children; }

	// Get index-th child node : it is too slow. recommend not to use this
    zz_node * get_child_by_index (int index_of_child_to_find);

	// Traverse all sub-nodes and print the information(name and hierarchy)
    void traverse_node ();

	// dump ancestors and descendants names
	void dump_hierarchy ();

	// Get the highest root node, if not, returns null.
	// same as get_manger except return type
	zz_node * get_root () const
	{
		if (is_valid(node_system)) {
			return node_system->root;
		}
		return NULL;
	}

	// Get the parent node, if not, returns null.
	zz_node * get_parent () const;
	
	// Get hash table to traverse nodes in this family
	zz_hash_table<zz_node *> * get_hash_table ();

	// Check if this node is descended from *parent*(includes same object).
	bool is_descendant_of (const zz_node * parent) const; // return true if (this == parent)
	bool is_descendant_of_type (const zz_node_type * class_type) const; // return true if type(this) == type(class_type)
	
	// Only zz_manager calls this.
	virtual bool load (); // read file, allocate memory and upload to hardware
	virtual bool unload (); // release from hardware and free memory (not to save into disk)
	virtual bool unload_and_release () // if unload() was succeeded, then release() it. returns false if unload failed.
	{
		if (!unload())
			return false;
		release();
		return true;
	}
	virtual void init_reuse (); // initialize to be reused

	// load_weight := the weight to load. approximate time that we consume to load this resource. [unit: milli-seconds]
	virtual void set_load_weight (int load_weight_in); // set resource loading time weight
	virtual int get_load_weight () const; // get resource loading time weight

	// load weight. see load_weight
	virtual int get_load_priority () const { return 0; }

	// Bind/unbind this resource into the device
	// returns true if they did the real job.
	// if they return false, we dont have to do anything.
	virtual bool bind_device ();
	virtual bool unbind_device ();

	// Creating/Restoring device
	virtual bool restore_device_objects (); // create unmanaged objects
	virtual bool init_device_objects (); // create managed objects
	virtual bool invalidate_device_objects (); // destroy unmanaged objects
	virtual bool delete_device_objects (); // destroy managed objects

	// Runtime type checking
	virtual zz_node_type * get_node_type() const; // get class type
	static zz_node * create_instance(); // allocate memory for this class
	bool is_a (const zz_node_type * node_type) const; // if this is the instance of node_type class or derived class from node_type.
	bool is_type (const zz_node_type * node_type) const; // check if it is the same type



	// Retrieve all children recursively and push them back into the vector.
	template< typename T >
	zz_list< T* >& get_all_children( zz_list< T* >& children_out );
};

inline bool zz_node::is_type (const zz_node_type * node_type) const
{
	return (get_node_type() == node_type);
}

inline bool zz_node::is_a (const zz_node_type * node_type) const
{
	return get_node_type()->is_a(node_type);
}

inline zz_node_type * zz_node::get_node_type() const
{
	return (zz_node_type *)(&type_zz_node);
}

inline zz_node * zz_node::create_instance()
{
	zz_node * node = zz_new zz_node;
	return node;
}

inline zz_node * zz_node::get_parent () const
{
	return parent_node;
}

inline zz_hash_table<zz_node *> * zz_node::get_hash_table ()
{
	if (!get_impl(node_system)) return NULL;

	// maybe non-checking null pointer's reference make usp10.dll error?
	return &(node_system->name_hash_table);
}

inline int zz_node::get_num_children ()
{
	return children.size();
}

// find any node which has the same type as this node
// from the root node.
inline zz_node * zz_node::find_type (const char * name_to_get)
{
	zz_node * root = get_root();
	if (root) return get_root()->find(name_to_get);
	return NULL;
}

inline unsigned long zz_node::get_refcount () const
{
	return this->refcount;
}

inline bool zz_node::is_descendant_of (const zz_node * parent) const
{
	if (this == parent) return true; // parent is itself

	const zz_node * current = this;
	const zz_node * root = get_root();

	if (parent == root) return true; // always descendant

	while (current && current != root) {
		current = current->get_parent();
		if (current == parent) return true;
	}
	return false;
}

inline bool zz_node::is_descendant_of_type (const zz_node_type * class_type) const
{
	const zz_node * current = this;
	const zz_node * root = get_root();

	// current check
	if (current->is_a(class_type)) return true;

	while (current && current != root) {
		if (current->is_a(class_type)) return true;
		current = current->get_parent();
	}
	return false;
}

inline const char * zz_node::get_name () const
{
	return node_name.get();
}

inline const char * zz_node::make_numbered_key (const char * key_name)
{
	assert(key_name);
	static char new_name[ZZ_MAX_NODENAME_SIZE];
	
	sprintf(new_name, "%s%d", key_name, unique_count);
	unique_count++;
	assert(!find(new_name));
	return new_name;
}

inline bool zz_node::load ()
{
	return true;
}

inline void zz_node::init_reuse ()
{
}

inline bool zz_node::unload ()
{
	return true;
}

inline void zz_node::set_load_weight (int weight_in)
{
	load_weight = weight_in;
}

inline int zz_node::get_load_weight () const
{
	return load_weight;
}

inline bool zz_node::bind_device ()
{
	bool ret;
	ret = init_device_objects();
	ret |= restore_device_objects();

	return ret;
}

inline bool zz_node::unbind_device ()
{
	bool ret;
	ret = invalidate_device_objects();
	ret |= delete_device_objects();

	return ret;
}

// for restoring device
inline  bool zz_node::restore_device_objects ()
{
	return true;
}

inline  bool zz_node::init_device_objects ()
{
	return true;
}

inline  bool zz_node::invalidate_device_objects ()
{
	return true;
}

inline  bool zz_node::delete_device_objects ()
{
	return true;
}

template< typename T >
inline zz_list< T* >& zz_node::get_all_children( zz_list< T* >& children_out )
{
	int child_count = children.size();

	if (child_count == 0) return children_out;

	for (zz_list<zz_node *>::iterator it = children.begin();
		it != children.end(); ++it)
	{
		assert(*it);
		children_out.push_back( static_cast< T* >(*it) );
		(*it)->get_all_children( children_out );
	}
	
	return children_out;
}

#endif // __ZZ_NODE_H__