/** 
 * @file zz_light_direct.cpp
 * @brief direct light class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-may-2002
 *
 * $Header: /engine/src/zz_light_direct.cpp 2     04-02-11 2:05p Zho $
 * $History: zz_light_direct.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
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
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_light_direct.h"


ZZ_IMPLEMENT_DYNCREATE(zz_light_direct, zz_light)

zz_light_direct::zz_light_direct(void) : zz_light()
{
	direction = vec4(0, 0, 0, 0);
}

zz_light_direct::~zz_light_direct(void)
{
}
