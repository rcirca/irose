/** 
 * @file zz_channel_rotation.cpp
 * @brief channel_rotation struct.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/src/zz_channel_rotation.cpp 3     04-07-07 3:29a Zho $
 * $History: zz_channel_rotation.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-07-07   Time: 3:29a
 * Updated in $/engine/src
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
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_channel_rotation.h"
#include "zz_algebra.h"
#include "zz_profiler.h"

ZZ_IMPLEMENT_DYNCREATE(zz_channel_rotation, zz_channel)

zz_channel_rotation::zz_channel_rotation(void) : zz_channel(ZZ_INTERP_SQUAD),
	rotations(0),
	num_rotations(0)
{
}

zz_channel_rotation::~zz_channel_rotation(void)
{
	clear();
}


void zz_channel_rotation::assign (int size)
{
	num_rotations = (unsigned int)size;
	rotations = zz_new quat [size];
}

void zz_channel_rotation::clear (void)
{
	num_rotations = 0;
	ZZ_SAFE_DELETE_ARRAY(rotations);
}

int zz_channel_rotation::size (void)
{
	return num_rotations;
}

void update (quat& q_cur, quat& q_new, bool interpolate)
{
	if (interpolate) {
		// DO INTERPOLATION
		quat q_diff = q_cur.inverse() * q_new; // q_cur * q_diff = q_new
		q_diff.normalize();
		const float limit_angle = 20.f;
		const float threshold = limit_angle*.5f*ZZ_TO_RAD; // threshold = (limit_angle/2)
		const float adjust_angle = 10.0f;
		const float adjust_threshold = adjust_angle*.5f*ZZ_TO_RAD;

		// we assume that q.w = cos(theta/2). 
		// CAUTION: theta may be [-pi, pi], but acosf assumes that theta is [0, 2pi]
		float theta_d2 = acosf(q_diff.w); // theta >= 0

		// if got the big angle(> 180 degree)
		if (theta_d2 > threshold) {
			// construct vector componet
			vec3 v(q_diff.x, q_diff.y, q_diff.z);

			if (theta_d2 > ZZ_PI*.5f) { // do opposite direction, if exceeds pi/2
				v = -v; // invert direction
			}
			axis_to_quat(q_diff, v, adjust_threshold);
			q_cur = q_cur * q_diff;
			q_cur.normalize();
		}
		else {
			q_cur = q_new;
		}
	}
	else {
		q_cur = q_new;
	}
}

void zz_channel_rotation::get_by_frame (int frame, void * data_pointer)
{
	assert(frame < (int)num_rotations);
	assert(frame >= 0);
	quat * rot_data = static_cast<quat *>(data_pointer);
	*rot_data = rotations[frame];
}

void zz_channel_rotation::get_by_time (zz_time time, int fps, void * data_pointer)
{
	int start_frame, next_frame;
	float ratio;
	quat new_rot;
	quat * rot_data = static_cast<quat *>(data_pointer);
	
	//ZZ_PROFILER_BEGIN_CYCLE(Pget_by_time, 100);
	switch (interp_style) {
		case ZZ_INTERP_LINEAR:
		case ZZ_INTERP_SLERP:
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_rotations, fps);
			new_rot = qslerp(rotations[start_frame], rotations[next_frame], ratio);
			break;
		case ZZ_INTERP_SQUAD:
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_rotations, fps);
			int prev_start, next_next;
			prev_start = (start_frame == 0) ? 0 : start_frame - 1;
			next_next = (next_frame == (num_rotations-1)) ? next_frame : next_frame + 1;
			new_rot = qsquad_spline(rotations[prev_start], rotations[start_frame],
				rotations[next_frame], rotations[next_next], ratio);
			break;
		default: // ZZ_INTERP_NONE
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_rotations, fps);
			new_rot = rotations[start_frame];
			break;
	}
	update(*rot_data, new_rot, false);

	//ZZ_LOG("channel_rotation: name(%s), interp_style(%d), time(%d), start(%d), next(%d), ratio(%f), fps(%d), data(%f, %f, %f, %f)\n",
	//	this->get_name(),
	//	interp_style, time, start_frame, next_frame, ratio, fps,
	//	rot_data->w, 
	//	rot_data->x, 
	//	rot_data->y, 
	//	rot_data->z);
	
	//ZZ_PROFILER_END(Pget_by_time);
}

void zz_channel_rotation::set_by_frame (int frame, void * data_pointer)
{
	rotations[frame] = *(quat *)data_pointer;

	//// force theta to positive
	//quat q = rotations[frame];
	//vec3 v(q.x, q.y, q.z);
	//vec3 nv = v;
	//nv.normalize();
	//float theta_d2 = acosf(q.w);
	//float sintheta_d2 = sinf(theta_d2);
	//float big_val = q.x;
	//int index = 0;
	//// find greatest
	//if (q.y > big_val) { index = 1; big_val = q.y; }
	//if (q.z > big_val) { index = 2; big_val = q.z; }
	//if (big_val*sintheta_d2*nv[index] < 0) { // theta is negative[ sign(v) ?= sign(sin(theta/2)*nv) ]
	//	assert(!"haha");
	//	q.x = -q.x;
	//	q.y = -q.y;
	//	q.z = -q.z;
	//	q.w = -q.w;
	//	rotations[frame] = q;
	//}
}
