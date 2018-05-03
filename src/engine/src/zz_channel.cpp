/** 
 * @file zz_channel.cpp
 * @brief channel class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/src/zz_channel.cpp 3     04-07-07 3:29a Zho $
 * $History: zz_channel.cpp $
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_node.h"
#include "zz_channel.h"

ZZ_IMPLEMENT_DYNAMIC(zz_channel, zz_node)

zz_channel::zz_channel(zz_interp_style new_interp_style) : interp_style(new_interp_style)
{
}

void zz_channel::set_interp_style (zz_interp_style new_interp_style)
{
	interp_style = new_interp_style;
}

zz_interp_style zz_channel::get_interp_style (void)
{
	return interp_style;
}

void zz_channel::set_channel_type (uint32 type)
{
	channel_type = type;
}

void zz_channel::time_to_frame (zz_time current, int& start_frame, int& next_frame, float& ratio, int size, int fps)
{
	assert(fps > 0);
	start_frame = ZZ_TICK_TO_FRAME(current, fps);
	next_frame = start_frame+1;
	ratio = float(current - ZZ_FRAME_TO_TICK(start_frame, fps)) * fps / ZZ_TICK_PER_SEC;
	start_frame = (start_frame >= size) ? start_frame - size : start_frame;
	next_frame = (next_frame >= size) ? next_frame - size : next_frame;		
}


void zz_channel::set_refer_id (int id)
{
	refer_id = id;
}