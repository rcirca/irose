/** 
 * @file zz_primitive.cpp
 * @brief primitive functions.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    18-nov-2002
 *
 * $Header: /engine/src/zz_primitive.cpp 5     04-11-07 5:56p Choo0219 $
 * $History: zz_primitive.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Choo0219     Date: 04-11-07   Time: 5:56p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-06-22   Time: 9:51a
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-06   Time: 1:26p
 * Updated in $/engine/src
 * Added sphere primitive.
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_renderer.h"
#include "zz_system.h"

void draw_box (const vec3& center, const vec3& half_length, const mat3& rotation, vec3 rgb)
{
	vec3 points[2][4];

	points[0][0].x = -half_length.x;
	points[0][0].y = -half_length.y;
	points[0][0].z = -half_length.z;

	points[0][1].x = half_length.x;
	points[0][1].y = -half_length.y;
	points[0][1].z = -half_length.z;

	points[0][2].x = half_length.x;
	points[0][2].y = half_length.y;
	points[0][2].z = -half_length.z;

	points[0][3].x = -half_length.x;
	points[0][3].y = half_length.y;
	points[0][3].z = -half_length.z;

	points[1][0].x = -half_length.x;
	points[1][0].y = -half_length.y;
	points[1][0].z = half_length.z;

	points[1][1].x = half_length.x;
	points[1][1].y = -half_length.y;
	points[1][1].z = half_length.z;

	points[1][2].x = half_length.x;
	points[1][2].y = half_length.y;
	points[1][2].z = half_length.z;

	points[1][3].x = -half_length.x;
	points[1][3].y = half_length.y;
	points[1][3].z = half_length.z;

	int i, j;
	vec3 temp;
	//ZZ_LOG("\n");
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			temp = mult(temp, rotation, points[i][j]);
			points[i][j] = temp + center;
		}
	}

	static zz_renderer * r = znzin->renderer;

	r->draw_line(points[0][0], points[0][1], rgb);
	r->draw_line(points[0][1], points[0][2], rgb);
	r->draw_line(points[0][2], points[0][3], rgb);
	r->draw_line(points[0][3], points[0][0], rgb);

	r->draw_line(points[1][0], points[1][1], rgb);
	r->draw_line(points[1][1], points[1][2], rgb);
	r->draw_line(points[1][2], points[1][3], rgb);
	r->draw_line(points[1][3], points[1][0], rgb);

	r->draw_line(points[0][0], points[1][0], rgb);
	r->draw_line(points[0][1], points[1][1], rgb);
	r->draw_line(points[0][2], points[1][2], rgb);
	r->draw_line(points[0][3], points[1][3], rgb);
}

void draw_aabb (const vec3& pmin, const vec3& pmax, vec3 rgb)
{
	vec3 points[2][4];

	points[0][0].x = pmin.x;
	points[0][0].y = pmin.y;
	points[0][0].z = pmin.z;

	points[0][1].x = pmax.x;
	points[0][1].y = pmin.y;
	points[0][1].z = pmin.z;

	points[0][2].x = pmax.x;
	points[0][2].y = pmax.y;
	points[0][2].z = pmin.z;

	points[0][3].x = pmin.x;
	points[0][3].y = pmax.y;
	points[0][3].z = pmin.z;

	points[1][0].x = pmin.x;
	points[1][0].y = pmin.y;
	points[1][0].z = pmax.z;

	points[1][1].x = pmax.x;
	points[1][1].y = pmin.y;
	points[1][1].z = pmax.z;

	points[1][2].x = pmax.x;
	points[1][2].y = pmax.y;
	points[1][2].z = pmax.z;

	points[1][3].x = pmin.x;
	points[1][3].y = pmax.y;
	points[1][3].z = pmax.z;
			
	static zz_renderer * r = znzin->renderer;
	
	r->draw_line(points[0][0], points[0][1], rgb);
	r->draw_line(points[0][1], points[0][2], rgb);
	r->draw_line(points[0][2], points[0][3], rgb);
	r->draw_line(points[0][3], points[0][0], rgb);

	r->draw_line(points[1][0], points[1][1], rgb);
	r->draw_line(points[1][1], points[1][2], rgb);
	r->draw_line(points[1][2], points[1][3], rgb);
	r->draw_line(points[1][3], points[1][0], rgb);

	r->draw_line(points[0][0], points[1][0], rgb);
	r->draw_line(points[0][1], points[1][1], rgb);
	r->draw_line(points[0][2], points[1][2], rgb);
	r->draw_line(points[0][3], points[1][3], rgb);
     
	
    
}

void draw_sphere (const vec3& center, float radius, vec3 rgb)
{
	const int num_points = 12;

	vec3 start, end, last;

	float theta;
	float i;
	zz_renderer * r = znzin->renderer;

	if (!r) return;

	// x-y plane
	start = center;
	start.y = center.y + radius;
	for (i = 1.0f; i <= num_points; i += 1.0f) {
		theta = float(i) / num_points * ZZ_TWO_PI;
		end.x = center.x + radius*sinf(theta);
		end.y = center.y + radius*cosf(theta);
		end.z = center.z;
		r->draw_line(start, end, rgb);
		start = end;
	}

	// y-z plane
	start = center;
	start.y = center.y + radius;
	for (i = 1.0f; i <= num_points; i += 1.0f) {
		theta = float(i) / num_points * ZZ_TWO_PI;
		end.x = center.x;
		end.y = center.y + radius*cosf(theta);
		end.z = center.z + radius*sinf(theta);
		r->draw_line(start, end, rgb);
		start = end;
	}

	// x-z plane
	start = center;
	start.x = center.x + radius;
	for (i = 1.0f; i <= num_points; i += 1.0f) {
		theta = float(i) / num_points * ZZ_TWO_PI;
		end.x = center.x + radius*cosf(theta);
		end.y = center.y;
		end.z = center.z + radius*sinf(theta);
		r->draw_line(start, end, rgb);
		start = end;
	}
}


