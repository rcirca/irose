/** 
 * @file zz_terrain.h
 * @brief terrain class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    04-sep-2002
 *
 * $Header: /engine/include/zz_terrain.h 1     03-11-30 8:06p Zho $
 * $History: zz_terrain.h $
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

#ifndef __ZZ_TERRAIN_H__
#define __ZZ_TERRAIN_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif


class zz_terrain : public zz_node {
public:
	zz_terrain () {}
	virtual ~zz_terrain () {}

	virtual void render () = 0;
	virtual bool is_loaded () = 0;
	virtual float get_terrain_height_world (float horiz, float vert) = 0;

	ZZ_DECLARE_DYNAMIC(zz_terrain)
};

#endif // __ZZ_TERRAIN_H__