/** 
 * @file zz_light_direct.cpp
 * @brief direct light class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-may-2002
 *
 * $Header: /engine/src/zz_light_point.cpp 1     04-02-20 12:17p Zho $
 * $History: zz_light_point.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-02-20   Time: 12:17p
 * Created in $/engine/src
 */

#include "zz_tier0.h"
#include "zz_light_point.h"

ZZ_IMPLEMENT_DYNCREATE(zz_light_point, zz_light)

zz_light_point::zz_light_point(void) : zz_light(),
	attenuation(0, 0, 0, 0)
{
}

zz_light_point::~zz_light_point(void)
{
}
