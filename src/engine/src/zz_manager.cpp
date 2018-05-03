/** 
 * @file zz_manager.cpp
 * @brief manager class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    26-feb-2002
 *
 * $Header: /engine/src/zz_manager.cpp 34    04-09-20 7:39p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_mem.h"
#include "zz_manager.h"
#include "zz_node.h"
#include "zz_list.h"
#include "zz_renderer_d3d.h"
#include "zz_view_d3d.h"
#include "zz_node_type.h"
#include "zz_system.h"
#include "zz_camera.h"
#include "zz_mesh.h"
#include "zz_manager.h"
#include "zz_visible.h"
#include "zz_animatable.h"
#include "zz_script.h"
#include "zz_light.h"
#include "zz_light_omni.h"
#include "zz_light_direct.h"
#include "zz_bone.h"
#include "zz_profiler.h"

#define ZZ_DEFAULT_DELETED 0

ZZ_IMPLEMENT_DYNCREATE(zz_manager, zz_node)

zz_manager::zz_manager () :
	zz_node(),
	_current(NULL), 
	entrance_line(zz_waiting_line<zz_node>::FOR_ENTRANCE, 0),
	exit_line(zz_waiting_line<zz_node>::FOR_EXIT, 0),
	is_lazy(false),
	num_reuse(0)
{
	entrance_time_accumulated = 0;
	exit_time_accumulated = 0;
}

zz_manager::~zz_manager ()
{
	// clear_deleted();
}

size_t zz_manager::release_children ()
{
	size_t count = 0;
	zz_hash_table<zz_node*>::iterator it;
	zz_hash_table<zz_node*> * nodes = this->get_hash_table();

	this->addref(); // not to be released this
	//ZZ_LOG("manager: [%s] is now releasing sub-objects.\n", this->get_name());
	if (nodes) {
		int initial_size = nodes->size();
		while ((it = nodes->begin()) != nodes->end()) {
			//ZZ_LOG("manager: %04d. [%s] were not released.(refcount=%d)\n", count, (*it)->get_name(), (*it)->get_refcount());
			// force delete
			count++;
			zz_delete (*it);
		}
	}
	if (count > 0) {
		//ZZ_LOG("manager: [%s] had total %d objects that were not released. force deleted.\n", this->get_name(), count);
	}
	zz_node * node;
	count = exit_line.size();
	while (count > 0) {
		node = exit_line.pop();
		//ZZ_LOG("manager: %04d. [%s] were not released.(refcount=%d)\n", count, node->get_name(), node->get_refcount());
		count = exit_line.size();
		zz_delete node;
	}
	//entrance_line.flush(entrance_line.size());

	// force delete
	zz_delete this;
	return count; // total release count (does not mean the number of unreleased objects)
}

// get the current node
zz_node * zz_manager::get_current ()
{
	return _current;
}

// make node to current selection
zz_node * zz_manager::set_current (const char * name_to_set)
{
	zz_node * save = _current;
	_current = find(name_to_set);
	return save;
}

// returns old current
zz_node * zz_manager::set_current (zz_node * node_to_set_current)
{
	zz_node * save = _current;
	assert(node_to_set_current);
	_current = node_to_set_current;
	return save;
}

// <-> kill()
zz_node * zz_manager::spawn (const char * baby_name, zz_node_type * node_type_to_spawn, bool do_load)
{
	zz_node * new_born = NULL;
	
	//if (!node_type_to_spawn) return NULL;
	assert(node_type_to_spawn);

	// if reuning was turned on, then get the exit_line's.
	if (num_reuse > 0) {
		if (!exit_line.empty()) { // if there are one more exiting items.
			new_born = exit_line.pop(); // get it!
			if (new_born && !baby_name) { // if has no name
				new_born->set_name(0); // reset as unique name to clear old name
			}
		}
	}

	if (!new_born) { // if not found existing instance, then create new instance.
		new_born = node_type_to_spawn->create_instance();
	}

	assert(new_born);
	
	if (baby_name) { // if has specified name, rename to it.
		new_born->set_name(baby_name);
	}
	// else, use default name

	// link to this manager. all instances should be linked to the manager.
	link_child(new_born);
	
	// set current
	_current = new_born;

	// if we load() immediately after creating.
	if (do_load) {
		load(new_born);
	}
	//ZZ_LOG("manager: %s new_born (%s::%s)\n", this->get_name(), new_born->get_node_type()->type_name, new_born->get_name());
	
	return new_born;
}

void zz_manager::load (zz_node * node)
{
	zz_assert(znzin);
	zz_assert(node);

	// remove first
	exit_line.remove(node);

	if (!znzin->get_rs()->use_delayed_loading || (node->get_load_weight() == 0)) { // from zz_waiting_line::flush()
		// try loading
		if (node->load()) {
			return;
		}
	}
	entrance_line.push(node);
}

void zz_manager::unload (zz_node * node)
{
	zz_assert(node);
	zz_assert(znzin);

	// remove first
	entrance_line.remove(node); // not entered yet, but have to kill

	if (!znzin->get_rs()->use_delayed_loading) { // from zz_waiting_line::flush()
		// try unloading
		if (node->unload_and_release()) {
			return;
		}
		zz_assert(0);
	}

	node->init_reuse();

	exit_line.push(node); // include release
}

// first of all, find node with same name, and spawn if not have any matching
zz_node * zz_manager::find_or_spawn (const char * baby_name, zz_node_type * node_type_to_spawn)
{
	zz_node * new_born = NULL;
	
	if (baby_name) { // omitting spawn_name is possible
		new_born = this->find(baby_name);
	}
	if (new_born) {
		return new_born;
	}
	return spawn(baby_name, node_type_to_spawn);
}

// <-> spawn()
// only if refcount is 1, really delete it
bool zz_manager::kill (zz_node * node)
{
	assert(node);

	if (!children.empty()) { // replace _current to the first child
        _current = *(children.begin());
	}

	zz_node * parent_node = node->get_parent();

	unsigned long node_ref_count = node->get_refcount();

	if (node_ref_count != 1) { // if still refered by someone, then just call release().
		//ZZ_LOG("manager: %s killed (%s::%s)\n", this->get_name(), node->get_node_type()->type_name, node->get_name());
		if (find_entrance(node)) {
			// if it is in the entrance line, flush it first. 
			// if we don not flush entrance, we get the mangled node that will be deleted by other object's releasing still in entrance line not by kill().
			flush_entrance(node);
		}
		node->release();
		return true;	
	}
	
	assert(node_ref_count != 0);

	// Someone is still refering this.
	// First, unlink, pop from delayed queue(include release) it!

	// Not only release, but also kill actually!
	// disconnect all relationship with children.
	child_type * child = (zz_node::child_type *)(&node->get_children());
	child_iterator it(child->begin());
	while (it != child->end()) {
		// CAUTION: do not use "it++" because link_child() affects _it_ !
		node->unlink_child(*it);
		it = child->begin();
	}

	// disconnect from parent
	if (parent_node) {
		parent_node->unlink_child(node);
	}
	
	//ZZ_LOG("manager: %s full-killed (%s::%s)\n", this->get_name(), node->get_node_type()->type_name, node->get_name());

	// do manager's unload()
	unload(node);
	return true;
}

void zz_manager::for_each (zz_device_objects_callback callback)
{
	zz_hash_table<zz_node*>::iterator it, it_end;
	zz_hash_table<zz_node*> * nodes = get_hash_table(); // All children nodes could be accessed via name hash table.

	if (!nodes) {
		return;
	}

	//ZZ_LOG("manager: for_each(%x)", nodes);
	//ZZ_LOG("(%s)-%d\n", get_name(), nodes->size());

	for (it = nodes->begin(), it_end = nodes->end(); it != it_end; ++it) {
		callback(*it);
	}
}

// If time_to_update is zero, all accumulated time info is ignored. it means *new start*!!.
// When last frame was so heavy, we should refresh it.
// If not, we will get slow frames in a while.
void zz_manager::update (zz_time time_to_update)
{
	if ((exit_line.size() == 0) && (entrance_line.size() == 0)) return;
	
	sort_waitings(); // sort lines

	if (!znzin->get_rs()->use_delayed_loading) {
		exit_line.flush_n_pop((int)exit_line.size());
		entrance_line.flush_n_pop((int)entrance_line.size());
		entrance_time_accumulated = 0;
		exit_time_accumulated = 0;
		return;
	}
	
	if (time_to_update == 0) { // initialize acculumated
		entrance_time_accumulated = 0;
		exit_time_accumulated = 0;
	}
	else {
		// distribute time
		entrance_time_accumulated += 1 + time_to_update/10;
		exit_time_accumulated += 1 + time_to_update/10;
	}
	zz_node * node;
	zz_time t;
	zz_time time_weight;

	//--------------------------------------------------------------------------------
	// for exit
	node = exit_line.back();	
	t = ZZ_TIME_TO_MSEC(exit_time_accumulated);
	time_weight = (!node) ? t : static_cast<zz_time>(node->get_load_weight());

	// not to unload reusing nodes, bound to max_flush
	int max_flush = (exit_line.size() > num_reuse) ? (exit_line.size() - num_reuse) : 0;

	try {
		while ((max_flush-- > 0) && node && (t > time_weight)) {
			//ZZ_LOG("manager: [%s]->update() exit->flush(%s)\n", get_name(), node ? node->get_name() : "(null)");
			exit_line.flush_n_pop(1);
			
			node = exit_line.back();

			if (!node) continue; // skip if no node

			exit_time_accumulated -= ZZ_MSEC_TO_TIME(t);
			t = ZZ_TIME_TO_MSEC(exit_time_accumulated);

			time_weight = static_cast<zz_time>(node->get_load_weight());
		}
	}
	catch (...) {
		if (node) {
			ZZ_LOG("manager: [%s]->update() exit->flush(%s) failed.\n", get_name(), node->get_name());
		}
		else {
			ZZ_LOG("manager: [%s]->update() exit->flush() failed.\n", get_name());
		}
		throw;
	}
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// for entrance
	node = entrance_line.back();
	t = ZZ_TIME_TO_MSEC(entrance_time_accumulated);
	time_weight = (!node) ? t : static_cast<zz_time>(node->get_load_weight());

	try {
		while (node && (t > time_weight)) {
			//ZZ_LOG("manager: [%s]->update() entrance->flush(%s)\n", get_name(), node ? node->get_name() : "(null)");
			if (entrance_line.flush_node(node)) {
				entrance_time_accumulated -= ZZ_MSEC_TO_TIME(t);
				entrance_line.pop();
			}
			else { // not loaded, so reinsert to front
				entrance_line.pop();
				entrance_line.push(node); // re-insert
			}

			node = entrance_line.back();

			if (!node) continue; // skip if no node

			t = ZZ_TIME_TO_MSEC(entrance_time_accumulated);

			time_weight = static_cast<zz_time>(node->get_load_weight());
		}
	}
	catch (...) {
		if (node) {
			ZZ_LOG("manager: [%s]->update() entrance->flush(%s) failed.\n", get_name(), node->get_name());
		}
		else {
			ZZ_LOG("manager: [%s]->update() entrance->flush() failed.\n", get_name());
		}
		 throw;
	}
	//--------------------------------------------------------------------------------
}

bool zz_manager::promote_entrance (zz_node * node)
{
	if (entrance_line.empty()) return false;
	return entrance_line.to_back(node);
}

bool zz_manager::promote_exit (zz_node * node)
{
	if (exit_line.empty()) return false;
	return exit_line.to_back(node);
}

bool zz_manager::flush_entrance (zz_node * node)
{
	if (promote_entrance(node)) {
		entrance_line.flush_n_pop(1);
		return true;
	}
	// else not in entrance line
	return entrance_line.flush_node(node); // direct flush
}

bool zz_manager::flush_exit (zz_node * node)
{
	if (promote_exit(node)) {
		exit_line.flush_n_pop(1);
		return true;
	}
	// else not in exit line
	return exit_line.flush_node(node); // direct flush
}

size_t zz_manager::get_entrance_size ()
{
	return entrance_line.size();
}

size_t zz_manager::get_exit_size ()
{
	return exit_line.size();
}

bool zz_manager::flush_entrance_all ()
{
	return entrance_line.flush_all(); // flush all
}

bool zz_manager::flush_exit_all ()
{
	return exit_line.flush_all();
}

bool zz_manager::remove_exit (zz_node * node)
{
	if (exit_line.remove(node)) {
		return true;
	}
	return false;
}

bool zz_manager::remove_entrance (zz_node * node)
{
	if (entrance_line.remove(node)) {
		return true;
	}
	return false;
}

bool zz_manager::find_entrance (zz_node * node)
{
	return entrance_line.find(node);
}

bool zz_manager::find_exit (zz_node * node)
{
	return exit_line.find(node);
}

