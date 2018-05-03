/** 
 * @file zz_octree.cpp
 * @brief octree class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-mar-2002
 *
 * $Header: /engine/src/zz_octree.cpp 16    04-11-24 5:14p Choo0219 $
 * $History: zz_octree.cpp $
 * 
 * *****************  Version 16  *****************
 * User: Choo0219     Date: 04-11-24   Time: 5:14p
 * Updated in $/engine/src
 * 
 * *****************  Version 14  *****************
 * User: Choo0219     Date: 04-11-24   Time: 2:48p
 * Updated in $/engine/src
 * 
 * *****************  Version 13  *****************
 * User: Choo0219     Date: 04-11-23   Time: 11:42a
 * Updated in $/engine/src
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-07-14   Time: 10:47a
 * Updated in $/engine/src
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-07-02   Time: 3:12p
 * Updated in $/engine/src
 * delayed_load
 * camera collision
 * terrain index buffer
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-06-29   Time: 10:09a
 * Updated in $/engine/src
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-06-24   Time: 2:12p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-22   Time: 9:51a
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-02-24   Time: 11:55a
 * Updated in $/engine/src
 * Added  skip_no_collision in collect.
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-20   Time: 3:50p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-31   Time: 9:50a
 * Updated in $/engine/src
 * bug tracking comments added
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-30   Time: 6:39p
 * Updated in $/engine/src
 * getWorldObjectHeightInScene ¼öÁ¤
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
 * *****************  Version 27  *****************
 * User: Zho          Date: 03-11-29   Time: 6:26a
 * Updated in $/znzin11/engine/source
 * 
 * *****************  Version 26  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_bvolume.h"
#include "zz_system.h"
#include "zz_profiler.h"
#include "zz_visible.h"
#include "zz_octree.h"
#include "zz_model.h"

//#define ZZ_DEBUG_SCENE // enable to log scene debugging information

#ifdef ZZ_DEBUG_SCENE
#include <map>
#include <string>
#include "zz_terrain_block.h"
#include "zz_morpher.h"
#include "zz_ocean_block.h"
#include "zz_model.h"
#include "zz_particle_emitter.h"

struct _debug_item {
	bool exist;
	std::string name;
	_debug_item() : exist(0) {}
};
typedef std::map<zz_visible*, _debug_item> zz_debug_scene_map;
zz_debug_scene_map debug_scene_map; // to check inserted node. true(inserted), false(removed)
#endif

zz_octree::zz_octree() : root(NULL)
{
	infrustum_nodes.reserve(1000);
}

zz_octree::~zz_octree ()
{
	free_node(root);
#ifdef ZZ_DEBUG_SCENE
	zz_debug_scene_map::iterator it;
	int count = 0;
	ZZ_LOG("octree: scene_node dump...\n");
	for (it = debug_scene_map.begin(); it != debug_scene_map.end(); it++, count++) {
		ZZ_LOG("\t#%05d = status(%d), name('%s')\n",
			count, ((*it).second).exist, ((*it).second).name.c_str());
	}
	ZZ_LOG("octree: scene_node dump done\n");
#endif
}

// if found node can afford *obj*, then return that node
// and, if not found, return top self- *node* 
zz_octree::_octree_node * zz_octree::find_downward (zz_visible * obj, zz_octree::_octree_node * node)
{
	int i;
	_octree_node * cur_node = node;

	// if root was not created, now create one.
	// root should be freed in destructor
	if (!root) {
		assert(cur_node == NULL);
		root = alloc_node(NULL,
			ZZ_OCTREE_MINX, ZZ_OCTREE_MAXX,
			ZZ_OCTREE_MINY, ZZ_OCTREE_MAXY,
			ZZ_OCTREE_MINZ, ZZ_OCTREE_MAXZ);
		cur_node = root;
	}

	// outer loop : abort if no children
#ifdef _DEBUG
	static int depth = 0, maxdepth = -1;
	depth = 0; // for debugging
#endif 

	zz_assert(obj);
	zz_assert(obj->minmax);

	zz_assertf(obj->minmax[0].x <= obj->minmax[1].x, "[%s:%s] min.x[%f] <= max.x[%f]",
		obj->get_name(),
		obj->get_node_type()->type_name,
		obj->minmax[0].x, obj->minmax[1].x
		);

	zz_assertf(obj->minmax[0].y <= obj->minmax[1].y, "[%s:%s] min.y[%f] <= max.y[%f]",
		obj->get_name(),
		obj->get_node_type()->type_name,
		obj->minmax[0].y, obj->minmax[1].y
		);

	zz_assertf(obj->minmax[0].z <= obj->minmax[1].z, "[%s:%s] min.z[%f] <= max.z[%f]",
		obj->get_name(),
		obj->get_node_type()->type_name,
		obj->minmax[0].z, obj->minmax[1].z
		);

	while (cur_node->child[0]) { // if has children
		for (i = 0; i < 8; ++i) {
			if (is_inside_node(cur_node->child[i], obj->minmax[0], obj->minmax[1])) { // found
				cur_node = cur_node->child[i];
				break;
			}
		}
		// if we reach this point, then although there are children, none of them 
		// is big enough to contain the obj
		if (i == 8) {
			cur_node = node;
			break;
		}
		node = cur_node; // update node
#ifdef _DEBUG
		depth++;
#endif
	}

#ifdef _DEBUG
	if (depth > maxdepth) {
		maxdepth = depth;
		ZZ_LOG("octree: find_downward() depth = %d\n", maxdepth);
	}
#endif

	// this cur_node has no children, then return this cur_node
	// if this cur_node has too many children, it will be collapsed after call to collapse()
	// currently, we does not care about how many children are here
	return cur_node;
}

// insert object in the adequate node
// we begin by finding octree node which is enough to contain the object
bool zz_octree::insert (zz_visible * object)
{
	zz_assert(object);
	zz_assert(!object->_onode);
	zz_assert(object->parent_node == object->get_root());
	zz_assert(object->valid_bvolume);

	_octree_node * node = find_downward(object, root);
	zz_assert(node);

	_octree_node * found = NULL;

	while ((node->pmax.x - node->pmin.x) > ZZ_OCTREE_MIN_WIDTH) {
		if (node->child[0] == NULL) {
			subdivide(node);
		}
		found = find_downward(object, node);
		if (found == node) break;
		node = found;
	}

	// really add
	node->add(object);
	// set visible's otree node
	object->_onode = node;

#ifdef ZZ_DEBUG_SCENE
	if (!object->is_a(ZZ_RUNTIME_TYPE(zz_terrain_block)) &&
		!object->is_a(ZZ_RUNTIME_TYPE(zz_ocean_block)) &&
		!object->is_a(ZZ_RUNTIME_TYPE(zz_model)) &&
		!object->is_a(ZZ_RUNTIME_TYPE(zz_particle_emitter)) &&
		!object->is_a(ZZ_RUNTIME_TYPE(zz_morpher)))
	{ // only normal object
		if (debug_scene_map.find(object) != debug_scene_map.end()) { 
			if (debug_scene_map[object].exist) {
				ZZ_LOG("octree: insert(%s) failed. already exist\n", object->get_name());
			}
		}
		debug_scene_map[object].exist = true;
		debug_scene_map[object].name = object->get_name();
		if (object->get_num_render_unit() > 0) {
			debug_scene_map[object].name = object->get_material(0)->get_name();
			if (object->get_material(0)->get_num_textures() > 0) {
				debug_scene_map[object].name = object->get_material(0)->get_texture(0)->get_name();
			}
		}
	}
	if (0) {
		const zz_bounding_aabb * aabb = object->get_bvolume()->get_aabb();
		ZZ_LOG("octree::insert() [%+08.f:%+08.f,%+08.f:%+08.f,%+08.f:%+08.f]-[%+08.f:%+08.f,%+08.f:%+08.f,%+08.f:%+08.f]\n",
			node->pmin.x, node->pmax.x, node->pmin.y, node->pmax.y, node->pmin.z, node->pmax.z, 
			aabb->pmin.x, aabb->pmax.x, aabb->pmin.y, aabb->pmax.y, aabb->pmin.z, aabb->pmax.z);
	}
#endif
	return true;
}

// if not found, return false
// this version does not update octree tree structure
bool zz_octree::remove (zz_visible * object)
{
	assert(object);
	assert(object->is_a(ZZ_RUNTIME_TYPE(zz_visible)));

	_octree_node * node = reinterpret_cast<_octree_node *>(object->_onode);

	if (!node) return false;
	
#ifdef _DEBUG
	zz_list<zz_visible *>::iterator it = node->objects.find(object);
	assert(it != node->objects.end()); // remove failed
#endif
	
	// delete actually
	node->del(object);

	// clear visible's octree node information
	object->_onode = NULL;

#ifdef ZZ_DEBUG_SCENE
	if (!object->is_a(ZZ_RUNTIME_TYPE(zz_terrain_block)) &&
		!object->is_a(ZZ_RUNTIME_TYPE(zz_ocean_block)) &&
		!object->is_a(ZZ_RUNTIME_TYPE(zz_model)) &&
		!object->is_a(ZZ_RUNTIME_TYPE(zz_particle_emitter)) &&
		!object->is_a(ZZ_RUNTIME_TYPE(zz_morpher)))
	{ // only normal object
		if (debug_scene_map.find(object) == debug_scene_map.end()) { 
			ZZ_LOG("octree: remove(%s) failed. It has not been inserted yet.\n", object->get_name());
		}
		if (debug_scene_map[object].exist == false) {
			ZZ_LOG("octree: remove(%s) failed. It was already removed.\n", object->get_name());
		}
		debug_scene_map[object].exist = false;
		debug_scene_map[object].name = object->get_name();
		if (object->get_num_render_unit() > 0) {
			debug_scene_map[object].name = object->get_material(0)->get_name();
			if (object->get_material(0)->get_num_textures() > 0) {
				debug_scene_map[object].name = object->get_material(0)->get_texture(0)->get_name();
			}
		}
	}
#endif

	//ZZ_LOG("octree: remove(%s) completed\n", object->get_name());
	return true;
}

// simply remove and insert
bool zz_octree::refresh (zz_visible * vis)
{
	assert(vis);
	assert(vis->valid_bvolume);
	assert(vis->_onode);

	remove(vis);
	insert(vis);

	return true;
}

zz_octree::_octree_node * zz_octree::alloc_node(zz_octree::_octree_node * p,
                             float x0, float x1,
                             float y0, float y1,
                             float z0, float z1)
{
    //_octree_node * n = this->free_root;
	_octree_node * n = zz_new _octree_node;
    assert(n);
    //this->free_root = n->next;
    n->parent = p;
    n->pmin.x=x0; n->pmin.y=y0; n->pmin.z=z0;
    n->pmax.x=x1; n->pmax.y=y1; n->pmax.z=z1;
    return n;
}

void zz_octree::free_children (_octree_node * n)
{
	assert(n);

	for (int i = 0; i < 8; ++i) {
		if (n->child[i]) {
			free_node(n->child[i]);
			n->child[i] = NULL;
		}
	}
}

// [updated: 2003-02-21 by zho]
// We begin by freeing all children nodes, and
// clear octree node info of all visibles in object list.
// And then, delete itself
void zz_octree::free_node(_octree_node * n)
{
	if (!n) return;
	free_children(n);

	// this can be time-consuming task
	// if then, remove this and remove assert() in insert()
	zz_list<zz_visible *>::iterator it;
	//ZZ_LOG("octree: [%p]->free_node() called\n", n);
	for (it = n->objects.begin(); it != n->objects.end(); it++) {
		if (*it) {
			//ZZ_LOG("octree: [%p]->free_node(%s) done\n", n, (*it)->get_name());
			(*it)->_onode = NULL;
		}
	}

	if (n == root) root = NULL;
	ZZ_SAFE_DELETE(n);
}

void zz_octree::subdivide (zz_octree::_octree_node * node)
{
	assert(NULL == node->child[0]);

	// create new child-nodes
	float x = (node->pmin.x + node->pmax.x) * 0.5f;
	float y = (node->pmin.y + node->pmax.y) * 0.5f;
	float z = (node->pmin.z + node->pmax.z) * 0.5f;
	int i;
	for (i=0; i<8; i++) {
		float x0,x1,y0,y1,z0,z1;
		if (i & 1) { x0=node->pmin.x; x1=x; }
		else       { x0=x; x1=node->pmax.x; }
		if (i & 2) { y0=node->pmin.y; y1=y; }
		else       { y0=y; y1=node->pmax.y; }
		if (i & 4) { z0=node->pmin.z; z1=z; }
		else       { z0=z; z1=node->pmax.z; }
		node->child[i] = this->alloc_node(node,x0,x1,y0,y1,z0,z1);
	}
}

//void zz_octree::balance (_octree_node * node)
//{
//	if (node->child[0] && node->objects.size() <= 
//	---
//    if ((NULL != on->c[0]) && (on->all_num_elms <= this->subdiv_num)) {
//        // collapse all child nodes
//        this->collapse(on);
//    } else if ((NULL == on->c[0]) && (on->num_elms > this->subdiv_num)) {
//        // to many elements in leaf node -> subdivide
//        this->subdivide(on);
//    }
//
//    // balance() recursively on all children
//    if (on->c[0]) {
//        int i;
//        for (i=0; i<8; i++) this->balance(on->c[i]);
//    }
//}

//void zz_octree::balance_tree (void)
//{
//}

// from ati_terrain sample code
/////////////////////////////////////////////////////
//
// cullPlanes
// 
//  Uses the 6 frustum planes to cull excluded
// octants as the octree is descended. The polylists
// for each leaf found to be inside the frustum
// are placed into the RenderBin. Along with the
// polylists, the distance from the eye to the
// center of the node and whether or not the node
// was completely contained in the frustum
// is also recorded. 
//
////////////////////////////////////////////////////
void zz_octree::_octree_node::cull_planes(bool insert_onoff,zz_octree& octree, const zz_viewfrustum& view)
{
	if (all_num_objs <= 0) return;

	unsigned char zones[8]={0,0,0,0,0,0,0,0};
	vec3 pt[8];
	int ii;
	vec3 dir;
	float dist;
	
	zz_visible * vis;
	zz_list<zz_visible *>::iterator it, it_end;
	
	//compute the half dimensions
	float wd2 = .5f*(pmax.x - pmin.x); //float wd2=m_width/2.0f;
	float hd2 = .5f*(pmax.y - pmin.y); //float hd2=m_height/2.0f;
	float dd2 = .5f*(pmax.z - pmin.z); //float dd2=m_depth/2.0f;
	vec3 center(.5f*(pmax + pmin));

	bool hint;
    
	if(insert_onoff)
	{
		for (it = objects.begin(), it_end = objects.end(); it != it_end; ++it) {
			vis = (*it);
			(*it)->gather_visible(octree.infrustum_nodes);
		
			//// for test
			//int i = 0;
			//const zz_bounding_aabb * aabb = obj->get_bvolume()->get_aabb();
			//ZZ_LOG("octree: cull_planes(%d). octree(%d-%d, %d-%d, %d-%d)-vis(%d-%d, %d-%d, %d-%d), %s\n",
			//	i++, 
			//		int(pmin.x), int(pmax.x),
			//		int(pmin.y), int(pmax.y),
			//		int(pmin.z), int(pmax.z),
			//		int(aabb->pmin.x), int(aabb->pmax.x),
			//		int(aabb->pmin.y), int(aabb->pmax.y),
			//		int(aabb->pmin.z), int(aabb->pmax.z),
			//		obj->get_name()
			//		);
		}
	}	
	else
	{
	
		//ZZ_LOG("octree: cull_planes() all_num_objs(%d)\n", all_num_objs);

		//loop throught the eight corners of the node
		// and determine where they lie with respect
		// to the frustum planes
		//
		// This is sub-optimal as several points are
		// actually checked multiple times, but it
		// seems to make the code more understandable
		for(ii=0;ii<8;++ii){
			//compute the location of the current corner
			pt[ii].x=center.x+((ii&0x1)?wd2:-wd2);
			pt[ii].y=center.y+((ii&0x2)?hd2:-hd2);
			pt[ii].z=center.z+((ii&0x4)?dd2:-dd2);

			if((view.np.x*pt[ii].x+view.np.y*pt[ii].y+view.np.z*pt[ii].z+view.np.w)>-0.01f)
				zones[ii]|=0x01;
			else if((view.fp.x*pt[ii].x+view.fp.y*pt[ii].y+view.fp.z*pt[ii].z+view.fp.w)>-0.01f)
				zones[ii]|=0x02;

			if((view.lp.x*pt[ii].x+view.lp.y*pt[ii].y+view.lp.z*pt[ii].z+view.lp.w)>-0.01f)
				zones[ii]|=0x04;
			if((view.rp.x*pt[ii].x+view.rp.y*pt[ii].y+view.rp.z*pt[ii].z+view.rp.w)>-0.01f)
				zones[ii]|=0x08;

			if((view.tp.x*pt[ii].x+view.tp.y*pt[ii].y+view.tp.z*pt[ii].z+view.tp.w)>-0.01f)
				zones[ii]|=0x10;
			if((view.bp.x*pt[ii].x+view.bp.y*pt[ii].y+view.bp.z*pt[ii].z+view.bp.w)>-0.01f)
				zones[ii]|=0x20;
		}

		//if all of the corners are outside on of the boundaries
		// this node is excluded, so stop traversing
		//
		//This allows some nodes to sneak through that are at the corners
		// of the frustum, but ignoring that case was done to
		// simplify the code
		if (zones[0]&zones[1]&zones[2]&zones[3]&zones[4]&zones[5]&zones[6]&zones[7])
			return;

		//compute the distance squared
		dir = center;
		dist = dir.x*dir.x+dir.y*dir.y+dir.z*dir.z;

		//determine if node is completely contained
		hint=!((zones[0]|zones[1]|zones[2]|zones[3]|zones[4]|zones[5]|zones[6]|zones[7]));

		
		vec3 * minmax;
		if(hint)
		{
			insert_onoff = true;
			for (it = objects.begin(), it_end = objects.end(); it != it_end; ++it) {
				vis = (*it);
				(*it)->gather_visible(octree.infrustum_nodes);
			
				//// for test
				//int i = 0;
				//const zz_bounding_aabb * aabb = obj->get_bvolume()->get_aabb();
				//ZZ_LOG("octree: cull_planes(%d). octree(%d-%d, %d-%d, %d-%d)-vis(%d-%d, %d-%d, %d-%d), %s\n",
				//	i++, 
				//		int(pmin.x), int(pmax.x),
				//		int(pmin.y), int(pmax.y),
				//		int(pmin.z), int(pmax.z),
				//		int(aabb->pmin.x), int(aabb->pmax.x),
				//		int(aabb->pmin.y), int(aabb->pmax.y),
				//		int(aabb->pmin.z), int(aabb->pmax.z),
				//		obj->get_name()
				//		);
		    }
		
			// add this node into rendering list
			static int max_num_objects = 0;
			//if (objects.size() > max_num_objects) {
				//max_num_objects = objects.size();
				//ZZ_LOG("octree: max_num_objects = %d, range = (%07d_%07d, %07d_%07d, %07d_%07d)\n",
				//	max_num_objects, 
				//	(int)this->pmin.x, (int)this->pmax.x,
				//	(int)this->pmin.y, (int)this->pmax.y,
				//	(int)this->pmin.z, (int)this->pmax.z);
			//}
		}
		else
		{
		
			insert_onoff = false;
			for (it = objects.begin(), it_end = objects.end(); it != it_end; ++it) {
				vis = (*it);
				minmax = vis->get_minmax();
				assert(minmax);
				if (intersect(minmax[0], minmax[1], view)) {
					//octree.infrustum_nodes.push_back(*it);
					(*it)->gather_visible(octree.infrustum_nodes);
				}

				//// for test
				//int i = 0;
				//const zz_bounding_aabb * aabb = obj->get_bvolume()->get_aabb();
				//ZZ_LOG("octree: cull_planes(%d). octree(%d-%d, %d-%d, %d-%d)-vis(%d-%d, %d-%d, %d-%d), %s\n",
				//	i++, 
				//		int(pmin.x), int(pmax.x),
				//		int(pmin.y), int(pmax.y),
				//		int(pmin.z), int(pmax.z),
				//		int(aabb->pmin.x), int(aabb->pmax.x),
				//		int(aabb->pmin.y), int(aabb->pmax.y),
				//		int(aabb->pmin.z), int(aabb->pmax.z),
				//		obj->get_name()
				//		);
			}
		}
	
    }
	// traverse children if any
	for(ii=0;ii<8;++ii) {
		if (child[ii]) {
			child[ii]->cull_planes(insert_onoff,octree, view);
	 	}
		
	}
}

/*
[collect by aabbox]

octree -> bbox :

1) ISCONTAINED :
    add all nodes in this octree_node
2) OUTSIDE :
	ignore this octree_node
3) CLIPS, CONTAINS, ISEQUAL : 
   if has child octree node, then 
       recurse it
   else, add nodes which are in bbox
*/
void zz_octree::collect_by_minmax_recurse (
	_octree_node * node,
	std::vector<zz_visible *>& nodes,
	const vec3 minmax[2],
	bool skip_no_collision
)
{
	int i;
	int res;

	if (!node) return;

	res = zz_bounding_aabb::intersect(node->pmin, node->pmax, minmax[0], minmax[1]);
	if (res == zz_bounding_aabb::ISCONTAINED) {
		// add all nodes in this octree_node
		node->collect(nodes, skip_no_collision);
		return;
	}
	else if (res == zz_bounding_aabb::OUTSIDE) {
		return; // ignore
	}
	node->collect_by_minmax(nodes, minmax, skip_no_collision);

	// recurse it
	// outer loop : abort if no children
	if (node->child[0]) { // if has children
		for (i = 0; i < 8; ++i) {
			collect_by_minmax_recurse(node->child[i], nodes, minmax, skip_no_collision);
		}
	}
}

void zz_octree::cull_planes (const zz_viewfrustum& view)
{
	this->infrustum_nodes.clear();
	//ZZ_LOG("octree: cull_planes():infrustum_nodes.size(%d)\n", infrustum_nodes.size());
	if (!root) {
		ZZ_LOG("octree: cull_planes() no root\n");
		return;
	}
	root->cull_planes(false,*this, view);

	// to see octree node information(debugging purpose)
	//trace(root);
}

void zz_octree::trace (_octree_node * node)
{
	if (!node) return;
	
	if (node == root) {
		ZZ_LOG("trace() number of nodes(%d)\n", node->all_num_objs);
	}

	if (node->objects.size() != 0) {
		ZZ_LOG("octree::trace(). range[X, Y, Z]=[%+8.f..%+8.f, %+8.f..%+8.f, %+8.f..%+8.f] has #(%d) objects.\n",
			node->pmin.x, node->pmax.x, node->pmin.y, node->pmax.y, node->pmin.z, node->pmax.z, 
			node->objects.size());
	}

	for (int i = 0; i < 8; ++i) {
		trace(node->child[i]);
	}
}

// sync to collect_by_minmax()
void zz_octree::_octree_node::collect (
									   std::vector<zz_visible *>& nodes,
									   bool skip_no_collision
									   )
{
	zz_list<zz_visible*>::iterator it, it_end;
	zz_visible * vis;

	if (skip_no_collision) {
		for (it = objects.begin(), it_end = objects.end(); it != it_end; ++it) {
			// skip if it is not the compare_type or is descendant of parent_to_ignore_child
			vis = (*it);
			vis->gather_collidable(nodes); // includes children
		}
	}
	else {
		for (it = objects.begin(), it_end = objects.end(); it != it_end; ++it) {
			// skip if it is not the compare_type or is descendant of parent_to_ignore_child
			vis = (*it);
			vis->gather_visible(nodes); // includes children
		}
	}
	if (!this->child[0]) return;
	for (int i = 0; i < 8; ++i) {
		child[i]->collect(nodes, skip_no_collision);
	}
}

void zz_octree::_octree_node::collect_by_minmax (
	std::vector<zz_visible*>& nodes,
	const vec3 minmax[2],
	bool skip_no_collision // if it is true, skip none collision objects.
	)
{
	zz_list<zz_visible*>::iterator it, it_end;
	zz_visible * vis;
	if (skip_no_collision) {
		for (it = objects.begin(), it_end = objects.end(); it != it_end; ++it) {
			vis = (*it);
			if (vis->test_intersection_box_level(minmax[0], minmax[1], ZZ_CL_AABB)) { // intersect
				vis->gather_collidable(nodes);
			}
		}
	}
	else {
		for (it = objects.begin(), it_end = objects.end(); it != it_end; ++it) {
			vis = (*it);
			if (vis->test_intersection_box_level(minmax[0], minmax[1], ZZ_CL_AABB)) { // intersect
				vis->gather_visible(nodes);
			}
		}
	}
}
