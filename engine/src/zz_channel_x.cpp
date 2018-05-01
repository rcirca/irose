/** 
 * @file zz_channel_x.cpp
 * @brief 1D-channel class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    12-feb-2003
 *
 * $Header: /engine/src/zz_channel_x.cpp 3     04-07-07 3:29a Zho $
 * $History: zz_channel_x.cpp $
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_node.h"
#include "zz_channel.h"
#include "zz_channel_x.h"
#include "zz_profiler.h"

ZZ_IMPLEMENT_DYNCREATE(zz_channel_x, zz_channel)

zz_channel_x::zz_channel_x(void) : zz_channel(ZZ_INTERP_LINEAR), floats(0), num_floats(0)
{
}

zz_channel_x::~zz_channel_x(void)
{
	clear();
}

void zz_channel_x::assign (int size)
{
	num_floats = size;
	floats = zz_new float [size];
}

void zz_channel_x::clear (void)
{
	num_floats = 0;
	ZZ_SAFE_DELETE_ARRAY(floats);
}

int zz_channel_x::size (void)
{
	return (int)num_floats;
}

void zz_channel_x::get_by_frame (int frame, void * data_pointer)
{
	assert(frame < (int)num_floats);
	assert(frame >= 0);
	float * data = static_cast<float *>(data_pointer);
	
	*data = floats[frame];
}

void zz_channel_x::get_by_time (zz_time time, int fps, void * data_pointer)
{
	int start_frame, next_frame;
	float ratio;
	float * data = static_cast<float *>(data_pointer);
	
	switch (interp_style) {
		case ZZ_INTERP_LINEAR:
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_floats, fps);
			// lerp
			*data = floats[start_frame]*ratio + floats[next_frame]*(1.0f-ratio);
			break;
		default: // ZZ_INTERP_NONE
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_floats, fps);
			*data = floats[start_frame];
			break;
	}
}

void zz_channel_x::set_by_frame (int frame, void * data_pointer)
{
	floats[frame] = *(float *)data_pointer;
}
