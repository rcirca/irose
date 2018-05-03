/** 
 * @file zz_manager_mesh.h
 * @brief mesh manager class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-jan-2004
 *
 * $Header: /engine/include/zz_manager_mesh_ishared.h 3     04-07-10 4:31p Zho $
 * $History: zz_manager_mesh_ishared.h $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-07-10   Time: 4:31p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-07-07   Time: 9:57p
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-07-06   Time: 2:52p
 * Created in $/engine/include
 * manager_mesh_ishared added.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-07-05   Time: 9:37p
 * Updated in $/engine/include
 * vertex buffer redesigned
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-20   Time: 10:06a
 * Updated in $/engine/include
 * Mesh refactoring first version.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-15   Time: 4:26p
 * Created in $/engine/include
 * Redesigning mesh structure.
 */

#ifndef __ZZ_MANAGER_MESH_H__
#define __ZZ_MANAGER_MESH_H__

#ifndef __ZZ_MANAGER_H__
#include "zz_manager.h"
#endif

#ifndef __ZZ_INDEX_BUFFER_H__
#include "zz_index_buffer.h"
#endif

#ifndef __ZZ_MESH_TERRAIN_H__
#include "zz_mesh_terrain.h"
#endif

#ifndef __ZZ_MESH_OCEAN_H__
#include "zz_mesh_ocean.h"
#endif

//--------------------------------------------------------------------------------
class zz_manager_mesh_ishared : public zz_manager {
//--------------------------------------------------------------------------------
protected:

public:
	zz_manager_mesh_ishared ();
	virtual ~zz_manager_mesh_ishared ();

	// Creating/Restoring device
	static bool s_restore_device_objects (); // create unmanaged objects
	static bool s_invalidate_device_objects (); // destroy unmanaged objects

	ZZ_DECLARE_DYNAMIC(zz_manager_mesh_ishared);
};
//
////--------------------------------------------------------------------------------
//class zz_manager_mesh_terrain : public zz_manager_mesh_ishared {
////--------------------------------------------------------------------------------
//public:
//	zz_manager_mesh_terrain () :
//	  zz_manager_mesh_ishared() 
//	{
//	}
//
//	virtual ~zz_manager_mesh_terrain ()
//	{
//	}
//
//	ZZ_DECLARE_DYNAMIC(zz_manager_mesh_terrain);
//};
//
////--------------------------------------------------------------------------------
//class zz_manager_mesh_ocean : public zz_manager_mesh_ishared {
////--------------------------------------------------------------------------------
//public:
//	zz_manager_mesh_ocean () :
//	  zz_manager_mesh_ishared() 
//	{
//	}
//
//	virtual ~zz_manager_mesh_ocean ()
//	{
//	}
//
//	ZZ_DECLARE_DYNAMIC(zz_manager_mesh_ocean);
//};

#endif // _ZZ_MANAGER_MESH_H__