/** 
 * @file zz_light_direct.h
 * @brief direct light class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-may-2002
 *
 * $Header: /engine/include/zz_light_point.h 1     04-02-20 12:17p Zho $
 * $History: zz_light_point.h $
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-02-20   Time: 12:17p
 * Created in $/engine/include
 */

#ifndef __ZZ_LIGHT_POINT_H__
#define __ZZ_LIGHT_POINT_H__

#ifndef __ZZ_LIGHT_H__
#include "zz_light.h"
#endif

//--------------------------------------------------------------------------------
class zz_light_point : public zz_light {
public:
	vec4 attenuation; // point_light = 1/(a.x + a.y*distance + a.z*distance^2)

	zz_light_point(void);
	virtual ~zz_light_point(void);

	ZZ_DECLARE_DYNAMIC(zz_light_point)
};

#endif //__ZZ_LIGHT_POINT_H__