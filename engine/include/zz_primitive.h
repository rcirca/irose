/** 
 * @file zz_primitive.h
 * @brief primitive functions.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    18-nov-2002
 *
 * $Header: /engine/include/zz_primitive.h 2     04-02-06 1:26p Zho $
 * $History: zz_primitive.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-06   Time: 1:26p
 * Updated in $/engine/include
 * Added sphere primitive.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
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

#ifndef __ZZ_PRIMITIVE_H__
#define __ZZ_PRIMITIVE_H__

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif


void draw_aabb (const vec3& pmin, const vec3& pmax, vec3 rgb);
void draw_box (const vec3& center, const vec3& half_length, const mat3& rotation, vec3 rgb);
void draw_sphere (const vec3& center, float radius, vec3 rgb);

#endif // __ZZ_PRIMITIVE_H__