/** 
 * @file zz_skeleton.cpp
 * @brief skeleton class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-feb-2003
 *
 * $Header: /engine/src/zz_skeleton.cpp 15    04-06-10 4:07p Zho $
 * $History: zz_skeleton.cpp $
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-06-10   Time: 4:07p
 * Updated in $/engine/src
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-06-08   Time: 10:35a
 * Updated in $/engine/src
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-04-16   Time: 11:31a
 * Updated in $/engine/src
 * skeleton dummy includes rotation in dummy.
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-03-19   Time: 11:27a
 * Updated in $/engine/src
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-03-10   Time: 10:41a
 * Updated in $/engine/src
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-02-27   Time: 5:49p
 * Updated in $/engine/src
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-02-26   Time: 12:01p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-02-12   Time: 7:02p
 * Updated in $/engine/src
 * Added close() before the end of skeleton load function.
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-02-06   Time: 1:28p
 * Updated in $/engine/src
 * Xform coordinate system
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-29   Time: 12:13p
 * Updated in $/engine/src
 * cleanup
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-05   Time: 2:19p
 * Updated in $/engine/src
 * added [set/get]_path()
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/src
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-02   Time: 6:28p
 * Updated in $/engine/src
 * In type.h, added "zz_vfs.h"
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
 * *****************  Version 18  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_skeleton.h"
#include "zz_vfs_pkg.h"
#include "zz_log.h"

#define ZZ_ZMD_VERSION2 "ZMD0002"
#define ZZ_ZMD_VERSION3 "ZMD0003"

ZZ_IMPLEMENT_DYNCREATE(zz_skeleton, zz_node)

//#define LOG_SKELETON_INFO

zz_skeleton::zz_skeleton() :
	root_bone_index_(-1)
{
	nodes.reserve(100);
	dummies.reserve(10);
}

zz_skeleton::~zz_skeleton()
{
}

bool zz_skeleton::load_skeleton (const char * path_in)
{
	zz_vfs_pkg skel_file;
	char magic_number[8];
	uint32 parent_id = 0;
	
	if (skel_file.open(path_in) == false) {
		ZZ_LOG("skeleton: load_skeleton(%s) file open failed\n", path_in);
		return false;
	}
	
	// header section
	skel_file.read_string(magic_number, 7);
	int version = 3;
	if (strncmp(magic_number, ZZ_ZMD_VERSION2, 7) == 0) {
		version = 2;
	}
	else if (strncmp(magic_number, ZZ_ZMD_VERSION3, 7) == 0) {
		version = 3;
	}
	else {
		ZZ_LOG("skeleton: load_skeleton(%s) failed. [%s] version required\n",
			path_in, ZZ_ZMD_VERSION3);
		return false;
	}

	uint32 num_bones;
	char name[ZZ_MAX_STRING];
	uint32 i;

	skel_file.read_uint32(num_bones);
	vec3 translation;
	quat rotation;

	assert(num_bones < MAX_NUM_BONES);
	if (num_bones >= MAX_NUM_BONES) {
		ZZ_LOG("skeleton: load_skeleton(%s) failed. num_bones(%d)\n", path_in, num_bones);
		num_bones = 0;
	}

	nodes.reserve(num_bones);

	// read each bone
	for (i = 0; i < num_bones; i++) {
		// set name
		skel_file.read_uint32(parent_id);
		skel_file.read_string(name);

		// set translation
		skel_file.read_float3(translation.vec_array);
		translation.x *= ZZ_SCALE_IN;
		translation.y *= ZZ_SCALE_IN;
		translation.z *= ZZ_SCALE_IN;

		// set skeleton rotation
		skel_file.read_float(rotation.w);
		skel_file.read_float(rotation.x);
		skel_file.read_float(rotation.y);
		skel_file.read_float(rotation.z);

		// set nodes
		nodes.push_back(zz_skeleton_node());
		nodes[i].bone_name.set(name);
		nodes[i].translation = translation;
		nodes[i].rotation    = rotation;
		nodes[i].parent_id   = parent_id;

		if (parent_id == i) { // this is root bone
			root_bone_index_ = i;
		}
#ifdef LOG_SKELETON_INFO
		ZZ_LOG("skeleton: read_bone(%d, %s)\n", i, name);
#endif // LOG_SKELETON_INFO
	}
	if (root_bone_index_ < 0) {
		ZZ_LOG("skeleton: load_skeleton(%s) failed. cannot find root bone.\n", name);
		root_bone_index_ = 0;
	}

	// read dummy nodes
	uint32 num_dummies;
	zz_dummy_node dummy;

	skel_file.read_uint32(num_dummies);
	
	assert(num_dummies < MAX_NUM_DUMMIES);
	if (num_dummies >= MAX_NUM_DUMMIES) {
		ZZ_LOG("skeleton: load_skeleton(%s) failed. num_dummies(%d)\n", path_in, num_dummies);
		num_dummies = 0;
	}

	// prepare dummies
	dummies.reserve(num_dummies + 1); // + 1 means extra added dummy
	rotation = quat_id;
	for (i = 0; i < num_dummies; i++) {
		skel_file.read_string(name);
		skel_file.read_uint32(parent_id);
		skel_file.read_float3(translation.vec_array);
		if (version == 3) { // version3 includes dummy rotation
			skel_file.read_float(rotation.w);
			skel_file.read_float(rotation.x);
			skel_file.read_float(rotation.y);
			skel_file.read_float(rotation.z);
		}
		translation.x *= ZZ_SCALE_IN;
		translation.y *= ZZ_SCALE_IN;
		translation.z *= ZZ_SCALE_IN;

		dummies.push_back(zz_dummy_node());
		dummies[i].dummy_name.set(name);
		dummies[i].parent_id = static_cast<int>(parent_id);
		dummies[i].translation = translation;
		dummies[i].rotation = rotation;

		const char * parent_name = nodes[parent_id].bone_name.get();
#ifdef LOG_SKELETON_INFO
		ZZ_LOG("skeleton: read_dummy(%s:%d). parent=(%s:%d)\n",	name, i, parent_name, parent_id);
#endif
	}

	// add extra root bone dummy
	dummies.push_back(zz_dummy_node());
	dummies[num_dummies].dummy_name.set("_p1");
	dummies[num_dummies].parent_id = 0;
	dummies[num_dummies].translation = vec3_null;
	dummies[num_dummies].rotation = quat_id;

	set_path(path_in);

#ifdef LOG_SKELETON_INFO
	ZZ_LOG("skeleton: load_skeleton(%s). filename(%s), num_bones(%d), num_dummies(%d)\n",
		get_name(), path_in, num_bones, num_dummies);
#endif

	skel_file.close();

	return true;
}

bool zz_skeleton::set_path (const char * path_in)
{
	assert(!path_.get());
	assert(path_in);
	if (path_.get()) {
		ZZ_LOG("skeleton: [%s:%s]->set_path(%s) failed. already have a path(%s).\n", get_name(), path_in, path_.get());
		return false;
	}
	path_.set(path_in);
	return true;
}
