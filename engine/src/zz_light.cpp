/** 
 * @file zz_light.cpp
 * @brief light class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    18-may-2002
 *
 * $Header: /engine/src/zz_light.cpp 2     04-02-11 2:05p Zho $
 * $History: zz_light.cpp $
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_light.h"
#include "zz_system.h"

ZZ_IMPLEMENT_DYNAMIC(zz_light, zz_node)

zz_light::zz_light(void) : zz_node()
{
	enabled = true;
	shadowable = true;
	position = vec4(0, 0, 100, 0);
	diffuse = vec4(0, 0, 0, 0);
	ambient = vec4(0, 0, 0, 0);
	specular = vec4(0, 0, 0, 0);
	//light_index_in_renderer = znzin->renderer->create_light(this);
}

zz_light::~zz_light(void)
{
	//if (system && renderer) {
	//	znzin->renderer->enable_light(light_index_in_renderer, false);
	//}
}

//void zz_light::set_light ()
//{
//	znzin->renderer->set_light(light_index_in_renderer);
//}