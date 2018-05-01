/** 
 * @file zz_octree.h
 * @brief octree class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-mar-2002
 *
 * $Header: /engine/include/zz_octree.h 12    04-11-24 5:14p Choo0219 $
 * $History: zz_octree.h $
 * 
 * *****************  Version 12  *****************
 * User: Choo0219     Date: 04-11-24   Time: 5:14p
 * Updated in $/engine/include
 * 
 * *****************  Version 10  *****************
 * User: Choo0219     Date: 04-11-24   Time: 2:48p
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-07-14   Time: 10:47a
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-07-11   Time: 10:21p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-24   Time: 2:12p
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-24   Time: 11:55a
 * Updated in $/engine/include
 * Added  skip_no_collision in collect.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-06   Time: 1:26p
 * Updated in $/engine/include
 * Xform coordinate system
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-30   Time: 6:39p
 * Updated in $/engine/include
 * getWorldObjectHeightInScene ¼öÁ¤
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-04   Time: 7:00p
 * Updated in $/engine/include
 * apply zz_viewfrustum class
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
 * *****************  Version 19  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_OCTREE_H__
#define __ZZ_OCTREE_H__

#include <vector>
#include <assert.h>

#ifndef __ZZ_LIST_H__
#include "zz_list.h"
#endif

#ifndef __ZZ_VIEWFRUSTUM_H__
#include "zz_viewfrustum.h"
#endif

// about 82x82 km (1.0f = 1cm)
#define ZZ_OCTREE_MINX (-4096000.0f*ZZ_SCALE_IN)
#define ZZ_OCTREE_MAXX (+4096000.0f*ZZ_SCALE_IN)
#define ZZ_OCTREE_MINY (-4096000.0f*ZZ_SCALE_IN)
#define ZZ_OCTREE_MAXY (+4096000.0f*ZZ_SCALE_IN)
// minz and maxz has been shifted to the followings.
// because, if not, too many objects will be laid on the root octree node.
// all objects across the zero border are laid on the root octree node.
#define ZZ_OCTREE_MINZ (-4106000.0f*ZZ_SCALE_IN)
#define ZZ_OCTREE_MAXZ (+4086000.0f*ZZ_SCALE_IN)
//#define ZZ_OCTREE_MINZ (-4096000.0f*ZZ_SCALE_IN) // In this case, the root may have too many objects.
//#define ZZ_OCTREE_MAXZ (+4096000.0f*ZZ_SCALE_IN)

// MAXIMUM NODE DEPTH
#define ZZ_OCTREE_MIN_WIDTH (2000.0f*ZZ_SCALE_IN) // 20m

#define ZZ_OCTREE_MAX_NUMOBJS 1000 // currently not used

struct zz_node_type;


// converted from the nebula engine (noctree.h)
//--------------------------------------------------------------------------------
class zz_octree {
	friend class zz_visible;
private:	
	enum {
        ZZ_OCTREE_CLIPX0 = (1<<0),
        ZZ_OCTREE_CLIPX1 = (1<<1),
        ZZ_OCTREE_CLIPY0 = (1<<2),
        ZZ_OCTREE_CLIPY1 = (1<<3),
        ZZ_OCTREE_CLIPZ0 = (1<<4),
		ZZ_OCTREE_CLIPZ1 = (1<<5),
	}; 

	// octree node data structure
	class _octree_node {
	public:
		_octree_node * child[8]; // 8 children node pointer
		_octree_node * parent;
		_octree_node * root;
		int all_num_objs; // to get the number of objects that this node and its children have
		vec3 pmin, pmax; // axis aligned bounding volume corner point by world coordsys
		zz_list<zz_visible *> objects; // object list in this octree node

		zz_octree::_octree_node () : parent(NULL), root(NULL), all_num_objs(0),
			pmin(ZZ_OCTREE_MINX, ZZ_OCTREE_MINY, ZZ_OCTREE_MINZ),
			pmax(ZZ_OCTREE_MAXX, ZZ_OCTREE_MAXY, ZZ_OCTREE_MAXZ)
		{
				// memset
				for (int i = 0; i < 8; ++i) {
					child[i] = NULL;
				}
		}

		~_octree_node () {
		}

		void add (zz_visible * obj) {
			_octree_node * node = this;
			do {
                node->all_num_objs++;
			} while (node = node->parent);

			//ZZ_LOG("octree: node:add() node(%f:%f,%f:%f,%f:%f)\n",
			//	this->pmin.x, this->pmax.x,
			//	this->pmin.y, this->pmax.y,
			//	this->pmin.z, this->pmax.z
			//	);

			this->objects.push_back(obj);
		}

		void del (zz_visible * obj) {
			zz_list<zz_visible *>::iterator it;
			it = objects.find(obj);
			
			assert(it != objects.end());
			objects.erase(it);

			_octree_node * node = this;
			// decrease all_num_objs upto the root
			do {
				node->all_num_objs--;
				assert(node->all_num_objs >= 0);
			} while (node = node->parent);
			//this->objects.erase
		}

		void cull_planes (bool insert_onoff,zz_octree& octree, const zz_viewfrustum& view);

		// sync to collect_by_minmax()
		void collect (
			std::vector<zz_visible *>& nodes,
			bool skip_no_collision
		);

		// sync to collect()
		void collect_by_minmax (
			std::vector<zz_visible*>& nodes,
			const vec3 minmax[2],
			bool skip_no_collision // if it is true, skip none collision objects.
		);
	};

protected:
	_octree_node * root;

	bool is_inside_node (_octree_node * node, const vec3& pmin, const vec3& pmax);
	_octree_node * find_downward (zz_visible * obj, _octree_node * node);
	void balance (_octree_node * node);

	void subdivide (_octree_node * node);
	// void collapse (_octree_node * object);

	_octree_node * alloc_node(_octree_node * p,
                             float x0, float x1,
                             float y0, float y1,
                             float z0, float z1);
	
	void free_node (_octree_node * p);
	void free_children (_octree_node *p);

//	void collect (_octree_node * node, std::vector<zz_visible *>& nodes);
	void collect_by_minmax_recurse (
		_octree_node * node,
		std::vector<zz_visible *>& nodes,
		const vec3 minmax[2],
		bool skip_no_collision
	);

	void trace (_octree_node * node);

	std::vector<zz_visible *> infrustum_nodes; // visible nodes in viewfrustum. 

public:
	zz_octree ();
	~zz_octree ();

	const std::vector<zz_visible *>& get_infrustum_nodes ()
	{
		return infrustum_nodes;
	}

	// return true if succeeded
	bool insert (zz_visible * object);

	// return true if succeeded
	bool remove (zz_visible * object);

	bool refresh (zz_visible * object);

	void clear (void)
	{
		free_node(root);
		infrustum_nodes.clear();
	}

	//void balance_tree (void);

	void cull_planes (const zz_viewfrustum& view);

	// collects all nodes, and returns the number of collected nodes
	//int collect_by_viewfrustum (std::vector<zz_visible *>& nodes, const mat4& view_matrix);
	void zz_octree::collect_by_minmax (std::vector<zz_visible *>& nodes, const vec3 minmax[2], bool skip_no_collision)
	{
		collect_by_minmax_recurse(root, nodes, minmax, skip_no_collision);
	}

	// trace octree node information for debugging purpose
	void trace_tree()
	{
		trace(root);
	}
};


// Is pmin-pmax inside the octree node?
inline bool zz_octree::is_inside_node (_octree_node * node, const vec3& pmin, const vec3& pmax)
{
    int clip = 0;
	assert(node);

	assert(pmin.x <= pmax.x);
	assert(pmin.y <= pmax.y);
	assert(pmin.z <= pmax.z);

	// [min, max)
	if (pmin.x < node->pmin.x) clip |= ZZ_OCTREE_CLIPX0;
	if (pmax.x > node->pmax.x) clip |= ZZ_OCTREE_CLIPX1;
	if (pmin.y < node->pmin.y) clip |= ZZ_OCTREE_CLIPY0;
	if (pmax.y > node->pmax.y) clip |= ZZ_OCTREE_CLIPY1;
	if (pmin.z < node->pmin.z) clip |= ZZ_OCTREE_CLIPZ0;
	if (pmax.z > node->pmax.z) clip |= ZZ_OCTREE_CLIPZ1;
	return (0 == clip);
}

#endif // __ZZ_OCTREE_H__