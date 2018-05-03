/** 
 * @file zz_node.cpp
 * @brief node class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    23-feb-2002
 *
 * $Header: /engine/src/zz_node.cpp 12    04-09-16 6:23p Zho $
 * $History: zz_node.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-09-16   Time: 6:23p
 * Updated in $/engine/src
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-07-22   Time: 11:56a
 * Updated in $/engine/src
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-06-25   Time: 3:03a
 * Updated in $/engine/src
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-03-01   Time: 4:56p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-02-21   Time: 12:06p
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-02-17   Time: 2:11p
 * Updated in $/engine/src
 * Added get_all_children() that collects all children recursively.
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-02-13   Time: 1:20a
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-14   Time: 2:03p
 * Updated in $/engine/src
 * _debug is_valid() fixed.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-14   Time: 12:00p
 * Updated in $/engine/src
 * Upgraded smart pointer interface.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-02   Time: 10:54a
 * Updated in $/engine/src
 * weight -> load_weight
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 22  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 03-11-27   Time: 4:50p
 * Updated in $/znzin11/engine/source
 * - removed zz_resource class, added full-support of resource management
 * into zz_node, and cleaned up some codes.
 */

#pragma warning(disable : 4786)

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_mem.h"
#include "zz_node.h"
#include "zz_log.h"
#include "zz_profiler.h"
#include "zz_system.h"

const zz_node_type zz_node::type_zz_node = {
	".zn", NULL, NULL
};

int zz_node::unique_count(1); // The handle 0 is not defined, and consequently we can think of 0 handle as not assigned yet.
//zz_system * zz_node::system(NULL);

zz_node::zz_node () : parent_node(NULL), node_name(), refcount(0), load_weight(0)
{
	set_name(make_unique_key());
	addref(); // CAUTION: do not addref() to node which is just created or spawned by manager
}

zz_node::~zz_node () 
{
	// CAUTION: DO NOT UNLINK FROM PARENT HERE
	// will be unlinked only from parent
	remove_name();
}

unsigned long zz_node::addref (void)
{
	//ZZ_LOG("node: [%s:%x]->addref(%d->%d) by\n",
	//	this->get_name(), this, refcount, refcount+1);
	return ++refcount;
}

unsigned long zz_node::release ()
{
	//ZZ_LOG("node: [%s:%x]->release(%d->%d)\n", this->get_name(), this, refcount, refcount-1);
	if (--refcount == 0) {
		zz_delete this;
		return 0;
	}
	return refcount;
}

// remove name from name_hash_table and clear node_name to zero
void zz_node::remove_name ()
{
	// remove from hash table
	if (is_valid(node_system)) {
		const char * name = get_name();
		if (name) {
			node_system->name_hash_table.remove(name);
			node_name.reset();
		}
	}
}

void zz_node::set_to_root () 
{
    // if already have a valid _node_system pointer, skip and reuse original root
	assert(!is_valid(node_system));
	node_system = zz_new str_node_system;
	node_system->root = this;
	//ZZ_LOG("node: %s is root.\n", get_name());
}

// if this node has no node_system, then create it and 
// let this node to be the root node.
// if this node has the parent node, then unlink this node from the parent node,
// and insert this node into the name-hash_table.
// root node cannot be a child of any other nodes.
void zz_node::link_child (zz_node * node) 
{
	// should have one
    assert(node);

	assert(this != node);

	// set this node to the root, if we does not have node_system
	if (!is_valid(node_system)) {
		// child node should have no node_system already.
		assert(!is_valid(node->node_system));
		set_to_root();
	}
#ifdef _DEBUG
	else { // if this node has node_system
		// if child-node has node_system
		if (is_valid(node->node_system)) {
			// should have same node_system (same root)
			assert(this->get_root() == node->get_root());
		}
		// root node cannot be a child of other node
		assert(node != node->get_root());
	}
#endif

	// if the node is new to this node_system
	if (!is_valid(node->node_system)) {
		// register node key into the name hash table
	    node_system->name_hash_table.insert(node->get_name(), node);
		node->node_system = this->node_system;
	}

	// reference count pair with unlink_child()
	//ZZ_ADDREF(node); // this must proceed to unlink_child(), or we may lost this node
	//ZZ_ADDREF(this);

	// we must call unlink_child() first!
	if (node->parent_node) {
		node->parent_node->unlink_child(node);
	}

	// insert node into child list
	// to access directly from this node to child node
    node->it_by_parent = children.push_back(node);

	// set up parent_node
	node->parent_node = this;

}

// this call only remove link to child
// it does not release child
void zz_node::_unlink_child (zz_node * child)
{
	assert(child);

	// disconnect parent to child relationship
	zz_list<zz_node *>::iterator it = child->it_by_parent;	
	children.erase(it);

	// disconnect child to parent relationship
	child->parent_node = NULL;
}

// called by unlink_node().
// remove link to child node.
void zz_node::unlink_child (zz_node * child) 
{
	_unlink_child(child);
	// reference count pair with link_child()
	//child->release();
	//this->release();
}

// _make_unique_key
const char * zz_node::make_unique_key () 
{
	if (node_name.get()) {
		node_system->name_hash_table.remove(node_name.get());
	}
	
	static char temp_name[ZZ_MAX_NODENAME_SIZE];
	//sprintf(temp_name, "%s%d", ZZ_DEFAULT_NODE_NAME, unique_count);
	//char * type_name = this->get_node_type()->type_name;
	sprintf(temp_name, "%s%d", this->get_node_type()->type_name, unique_count);
	unique_count++;
	return temp_name;
}

bool zz_node::set_name (const char * new_name) 
{
	const char * new_name_to_set = new_name;

	if (new_name == NULL) {
		new_name_to_set = make_unique_key();
	}

#ifdef _DEBUG
	// if already has the same name
	if ((node_name.size() > 0) && strcmp(new_name, node_name.get()) == 0) {
		assert(!"already has the same name");
	}
#endif

	// remove name from hash_table
	if (is_valid(node_system)) {
		//ZZ_LOG("node: rename(%s, %s)\n", node_name.get(), new_name);
		if (node_name.get()) {
			node_system->name_hash_table.remove(node_name.get());
		}
	}

	if (find(new_name)) {
		//ZZ_LOG("node: set_name(%s) exist same name\n", new_name);
		new_name_to_set = make_numbered_key(new_name);
	}
	
	node_name.set(new_name_to_set);

	// reinsert name to hash_table
	if (is_valid(node_system)) {
		node_system->name_hash_table.insert(new_name_to_set, this);
	}

	return true;

	//if (strlen(new_name) > strlen(node_name)) {
	//	zz_delete[] node_name;
	//	node_name = zz_new char[strlen(new_name) + 1];
	//}
	
	//strcpy(node_name, new_name);
}

// find by name
zz_node * zz_node::find (const char * name_to_get) 
{
    //assert(name_to_get);
	if (!name_to_get) return NULL;
    
	if (!is_valid(node_system)) {
		//ZZ_LOG("node: find(%s) failed. node_system not valid\n", name_to_get);
		return NULL;
	}
    
	zz_hash_table<zz_node *>::iterator it;
	it = node_system->name_hash_table.find(name_to_get);
	zz_node * found_node = NULL;
	if (it != node_system->name_hash_table.end()) {
		found_node = *it;
	}
	if (found_node == NULL) {
		//ZZ_LOG("node: find(%s) failed. hash_table.find failed\n", name_to_get);
		return NULL;
	}
    return found_node;
}

// index starts from 0
zz_node * zz_node::get_child_by_index (int index) 
{
    if ((index < 0) || (index >= (int)children.size())) {
        return NULL; // invalid index
    }

	child_iterator it;
	int i = 0;
	for (i = 0, it = children.begin(); it != children.end(); ++it, ++i) {
		if (i == index) {
			return *it;
		}
	}
    return NULL; // not found
}

void zz_node::traverse_child_node(int level) 
{
    int child_count = children.size();

    for (int i = 0; i < level; i++) {
        ZZ_LOG("  ");
    }
	ZZ_LOG("[%s]\n", get_name());

    if (child_count == 0) return;

    //for (i = 0; i < level; i++) {
    //    ZZ_LOG("  ");
    //}
    //ZZ_LOG("{\n");
	zz_list<zz_node *>::iterator it;

	for (it = children.begin(); it != children.end(); ++it) {
		(*it)->traverse_child_node(level+1);
    }

    //for (i = 0; i < level; i++) {
    //    ZZ_LOG("  ");
    //}
    //ZZ_LOG("}\n");
}

void zz_node::traverse_node() 
{
	if (!is_valid(node_system)) return;
    int node_count = node_system->name_hash_table.size();

	ZZ_LOG("node: traverse_node(): size(%d)\n", node_count);
    node_system->root->traverse_child_node(0);
    ZZ_LOG("\n\n");
}

void zz_node::dump_hierarchy ()
{
	if (!is_valid(node_system)) return;

	zz_node * parent = get_parent();
	zz_node * root = get_root();

	ZZ_LOG("DmBeGiN: [[%s]]", get_name());
	while (parent != NULL) {
		ZZ_LOG("<-[%s]", parent->get_name());
		parent = parent->get_parent();
	}
	ZZ_LOG("\n");

	traverse_child_node(0);
	
	ZZ_LOG("DmEnD\n");
}