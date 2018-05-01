/** 
 * @file zz_terrain_block.h
 * @brief terrain block class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    03-mar-2003
 *
 * $Header: /engine/include/zz_terrain_block.h 12    04-09-05 6:51p Zho $
 * $History: zz_terrain_block.h $
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-09-05   Time: 6:51p
 * Updated in $/engine/include
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-08-16   Time: 4:20p
 * Updated in $/engine/include
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-07-12   Time: 12:13p
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-07-10   Time: 4:31p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-07-07   Time: 9:57p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-07-06   Time: 2:52p
 * Updated in $/engine/include
 * manager_mesh_ishared added.
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-05-29   Time: 9:46p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-04-20   Time: 3:53p
 * Updated in $/engine/include
 * use begin/end state
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_TERRAIN_BLOCK_H__
#define __ZZ_TERRAIN_BLOCK_H__

#ifndef __ZZ_VISIBLE_H__
#include "zz_visible.h"
#endif

#ifndef __ZZ_MESH_TERRAIN_H__
#include "zz_mesh_terrain.h"
#endif

class zz_mesh_tool;
class zz_terrain_block : public zz_visible {
	zz_mesh_terrain * terrain_mesh;

public:
	zz_terrain_block();
	virtual ~zz_terrain_block();

	static bool first_render; // if this render is the first time.
	static void begin_state (bool blended);
	static void end_state ();

	// for zonly
	static bool begin_state_zonly (); // return false if not support color enable
	static void end_state_zonly ();
	void render_zonly ();

	virtual void render (bool recursive);

	virtual bool get_transparent () { return false; }

	virtual const vec3& get_com_position_world ();

	virtual void update_bvolume (void);

	virtual void reset_bvolume (void);

	void add_runit (zz_mesh_terrain * mesh, zz_material * material, zz_light * light);

	virtual void update_time (bool recursive, zz_time diff_time);

	virtual void insert_scene ();
	virtual void remove_scene ();

	void set_index_order (int index_order_in)
	{
		assert(terrain_mesh);
		terrain_mesh->set_index_order(index_order_in);
	}

	int get_index_order ()
	{
		assert(terrain_mesh);
		terrain_mesh->get_index_order();
	}

	virtual void before_render ();
	virtual void after_render ();

	ZZ_DECLARE_DYNAMIC(zz_terrain_block);
};

class zz_terrain_block_rough : public zz_terrain_block {
public:
	zz_terrain_block_rough();

	static void begin_state (bool blended);
	static void end_state ();

	ZZ_DECLARE_DYNAMIC(zz_terrain_block_rough);
};

#endif // __ZZ_TERRAIN_BLOCK_H__