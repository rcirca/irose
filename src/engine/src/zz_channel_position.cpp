/** 
 * @file zz_channel_position.cpp
 * @brief channel_position struct.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/src/zz_channel_position.cpp 3     04-07-07 3:29a Zho $
 * $History: zz_channel_position.cpp $
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
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_node.h"
#include "zz_algebra.h"
#include "zz_channel_position.h"
#include "zz_profiler.h"

ZZ_IMPLEMENT_DYNCREATE(zz_channel_position, zz_channel)

zz_channel_position::zz_channel_position(void) : zz_channel(ZZ_INTERP_CATMULLROM),
positions(0),
num_positions(0)
{
}

zz_channel_position::~zz_channel_position(void)
{
	clear();
}


void zz_channel_position::assign (int size)
{
	num_positions = (unsigned int)size;
	positions = zz_new vec3 [size];
}

void zz_channel_position::clear (void)
{
	num_positions = 0;
	ZZ_SAFE_DELETE_ARRAY(positions);
}

int zz_channel_position::size (void)
{
	return (int)num_positions;
}

void update (vec3& cur_pos, vec3& new_pos, bool interpolate)
{
	if (interpolate) {
		static vec3 diff_vec;
		static float delta;
		const float threshold = 20.0f; // angle limit
		const float new_delta = 10.0f;
		
		diff_vec = new_pos - cur_pos;
		delta = diff_vec.norm();
		diff_vec.normalize();
		if (delta > threshold) {
			cur_pos += new_delta*diff_vec;
		}
		else {
			cur_pos = new_pos;
		}
	}
	else {
		cur_pos = new_pos;
	}
}

void zz_channel_position::get_by_frame (int frame, void * data_pointer)
{
	assert(frame < (int)num_positions);
	assert(frame >= 0);
	vec3 * pos_data = static_cast<vec3 *>(data_pointer);
	*pos_data = positions[frame];
}

void zz_channel_position::get_by_time (zz_time time, int fps, void * data_pointer)
{
	int start_frame, next_frame;
	float ratio;
	vec3 new_pos;
	vec3 * pos_data = static_cast<vec3 *>(data_pointer);
	
	switch (interp_style) {
		case ZZ_INTERP_LINEAR:
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_positions, fps);
			lerp(new_pos, ratio, positions[start_frame], positions[next_frame]);
			break;
		case ZZ_INTERP_CATMULLROM:
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_positions, fps);
			int prev_start, next_next;
			prev_start = (start_frame == 0) ? 0 : start_frame - 1;
			next_next = (next_frame == (num_positions-1)) ? next_frame : next_frame + 1;
			new_pos = catmull_rom(positions[prev_start], positions[start_frame],
				positions[next_frame], positions[next_next], ratio);
			break;
		default: // ZZ_INTERP_NONE
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_positions, fps);
			new_pos = positions[start_frame];
			//ZZ_LOG("channel_position: start_frame = %d\n", start_frame);
			break;
	}
	update(*pos_data, new_pos, false);
}

void zz_channel_position::set_by_frame (int frame, void * data_pointer)
{
	positions[frame] = *(vec3 *)data_pointer;
}
