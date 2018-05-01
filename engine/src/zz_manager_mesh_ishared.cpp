/** 
 * @file zz_manager_mesh.cpp
 * @brief mesh manager class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-jan-2004
 *
 * $Header: /engine/src/zz_manager_mesh_ishared.cpp 3     04-07-10 4:31p Zho $
 * $History: zz_manager_mesh_ishared.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-07-10   Time: 4:31p
 * Updated in $/engine/src
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-07-07   Time: 9:57p
 * Updated in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-07-06   Time: 2:52p
 * Created in $/engine/src
 * manager_mesh_ishared added.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-07-05   Time: 9:38p
 * Updated in $/engine/src
 * vertex buffer redesigned
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-20   Time: 10:06a
 * Updated in $/engine/src
 * Mesh refactoring first version.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-15   Time: 4:26p
 * Created in $/engine/src
 * Redesigning mesh structure.
 */

#include "zz_tier0.h"
#include "zz_manager_mesh_ishared.h"

ZZ_IMPLEMENT_DYNCREATE(zz_manager_mesh_ishared, zz_manager)

zz_manager_mesh_ishared::zz_manager_mesh_ishared ()
{
}

zz_manager_mesh_ishared::~zz_manager_mesh_ishared ()
{
	invalidate_device_objects();
}

bool zz_manager_mesh_ishared::s_restore_device_objects ()
{
	zz_mesh_ishared::zz_index * indices = zz_mesh_ishared::get_shared_indices();
	for (int i = 0; i < NUM_INDEX_ORDER; ++i) {
		if (indices[i].num_indices) {
			indices[i].ibuf_res->create_buffer(indices[i].num_indices);
			indices[i].ibuf_res->update_buffer(indices[i].ibuf);
		}
	}
	return true;
}

bool zz_manager_mesh_ishared::s_invalidate_device_objects ()
{
	zz_mesh_ishared::zz_index * indices = zz_mesh_ishared::get_shared_indices();
	for (int i = 0; i < NUM_INDEX_ORDER; ++i) {
		if (indices[i].num_indices) {
			indices[i].ibuf_res->destroy_buffer();
		}
	}
	return true;
}
