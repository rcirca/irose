/** 
 * @file zz_channel_xyy.cpp
 * @brief 2D-channel class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    12-feb-2003
 *
 * $Header: /engine/src/zz_channel_xy.cpp 3     04-07-07 3:29a Zho $
 * $History: zz_channel_xy.cpp $
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
#include "zz_algebra.h"
#include "zz_channel.h"
#include "zz_channel_xy.h"

ZZ_IMPLEMENT_DYNCREATE(zz_channel_xy, zz_channel)

zz_channel_xy::zz_channel_xy(void) : zz_channel(ZZ_INTERP_LINEAR), xys(0), num_xys(0)
{
}

zz_channel_xy::~zz_channel_xy(void)
{
	clear();
}

void zz_channel_xy::assign (int size)
{
	num_xys = size;
	xys = zz_new vec2 [size];
}

void zz_channel_xy::clear (void)
{
	num_xys = 0;
	ZZ_SAFE_DELETE_ARRAY(xys);
}

int zz_channel_xy::size (void)
{
	return (int)num_xys;
}

void zz_channel_xy::get_by_frame (int frame, void * data_pointer)
{
	assert(frame < (int)num_xys);
	assert(frame >= 0);
	
	vec2 * data = static_cast<vec2 *>(data_pointer);
	
	*data = xys[frame];
}

void zz_channel_xy::get_by_time (zz_time time, int fps, void * data_pointer)
{
	int start_frame, next_frame;
	float ratio;
	vec2 * data = static_cast<vec2 *>(data_pointer);
	
	switch (interp_style) {
		case ZZ_INTERP_LINEAR:
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_xys, fps);
			// lerp
			data->x = xys[start_frame].x*ratio + xys[next_frame].x*(1.0f-ratio);
			data->y = xys[start_frame].y*ratio + xys[next_frame].y*(1.0f-ratio);
			break;
		default: // ZZ_INTERP_NONE
			time_to_frame(time, start_frame, next_frame, ratio, (int)num_xys, fps);
			*data = xys[start_frame];
			break;
	}
}

void zz_channel_xy::set_by_frame (int frame, void * data_pointer)
{
	xys[frame] = *(vec2 *)data_pointer;
}
