/** 
 * @file zz_skeleton.h
 * @brief skeleton class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-feb-2003
 *
 * $Header: /engine/include/zz_skeleton.h 7     04-06-22 9:50a Zho $
 * $History: zz_skeleton.h $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-06-08   Time: 10:35a
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-04-16   Time: 11:31a
 * Updated in $/engine/include
 * skeleton dummy includes rotation in dummy.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-03-19   Time: 11:27a
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-29   Time: 12:13p
 * Updated in $/engine/include
 * cleanup
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-05   Time: 2:19p
 * Updated in $/engine/include
 * added [set/get]_path()
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_SKELETON_H__
#define __ZZ_SKELETON_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#include <vector>

//--------------------------------------------------------------------------------
// skeleton does not have scale component.
// skeleton affects bone transform, which affects mesh vertex position.
class zz_skeleton : public zz_node {
//--------------------------------------------------------------------------------
private:
	zz_string path_;

	struct zz_skeleton_node {
		int parent_id;
		zz_string bone_name;
		vec3 translation; // local translation
		quat rotation; // local rotation
	};

	// point node for dummy link
	struct zz_dummy_node {
		int parent_id;
		zz_string dummy_name;
		vec3 translation;
		quat rotation;
	};

	int root_bone_index_; // index of root bone which has zero-parent id
	std::vector<zz_skeleton_node> nodes;
	std::vector<zz_dummy_node> dummies;

public:
	zz_skeleton();
	virtual ~zz_skeleton();

	bool load_skeleton (const char * path_in);
	uint32 get_num_dummies () const { return dummies.size(); }
	uint32 get_num_bones () const { return nodes.size(); }

	// get bone property
	int get_bone_parent_id (int index);
	const char * get_bone_name (int index);
	const vec3& get_bone_translation (int index);
	const quat& get_bone_rotation (int index);
	
	// get dummy property
	int get_dummy_parent_id (int index);
	const char * get_dummy_name (int index);
	const vec3& get_dummy_translation (int index);
	const quat& get_dummy_rotation (int index);

	int get_root_bone_index () const
	{
		return root_bone_index_;
	}

	bool set_path (const char * path_in);
	const char * get_path () const;

	ZZ_DECLARE_DYNAMIC(zz_skeleton)
};

inline const char *  zz_skeleton::get_path () const
{
	return path_.get();
}

inline int zz_skeleton::get_bone_parent_id (int index)
{
	assert(index >= 0);
	assert(index < (int)nodes.size());
	return nodes[index].parent_id;
}

inline const char * zz_skeleton::get_bone_name (int index)
{
	assert(index >= 0);
	assert(index < (int)nodes.size());
	return nodes[index].bone_name.get();
}

inline const vec3& zz_skeleton::get_bone_translation (int index)
{
	assert(index >= 0);
	assert(index < (int)nodes.size());
	return nodes[index].translation;
}

inline const quat& zz_skeleton::get_bone_rotation (int index)
{
	assert(index >= 0);
	assert(index < (int)nodes.size());
	return nodes[index].rotation;
}

inline int zz_skeleton::get_dummy_parent_id (int index)
{
	assert(index >= 0);
	assert(index < (int)dummies.size());
	return dummies[index].parent_id;
}

inline const char * zz_skeleton::get_dummy_name (int index)
{
	assert(index >= 0);
	assert(index < (int)dummies.size());
	return dummies[index].dummy_name.get();
}

inline const vec3& zz_skeleton::get_dummy_translation (int index)
{
	assert(index >= 0);
	assert(index < (int)dummies.size());
	return dummies[index].translation;
}

inline const quat& zz_skeleton::get_dummy_rotation (int index)
{
	assert(index >= 0);
	assert(index < (int)dummies.size());
	return dummies[index].rotation;
}


#endif // __ZZ_SKELETON_H__