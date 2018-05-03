/** 
 * @file zz_motion_mixer.cpp
 * @brief motion mixer class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/src/zz_motion_mixer.cpp 3     04-06-23 9:33a Zho $
 * $History: zz_motion_mixer.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
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
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_motion.h"
#include "zz_motion_mixer.h"
#include "zz_channel_position.h"
#include "zz_channel_rotation.h"
#include "zz_system.h"

ZZ_IMPLEMENT_DYNCREATE(zz_motion_mixer, zz_motion);

zz_motion_mixer::zz_motion_mixer(void)
{
}

zz_motion_mixer::~zz_motion_mixer(void)
{
}

zz_motion * zz_motion_mixer::get_motion_arg (int index)
{
	assert(this->get_child_by_index(index));
	
	return ((zz_motion *)this->get_child_by_index(index));
}

int zz_motion_mixer::get_num_frames (void)
{
	return get_motion_arg(0)->get_num_frames();
}


zz_time zz_motion_mixer::get_total_time (void)
{
	return get_motion_arg(0)->get_total_time();
}

int zz_motion_mixer::get_fps (void)
{
	return get_motion_arg(0)->get_fps();
}

void zz_motion_mixer::get_channel_data (int channel_index, zz_time time, void * data)
{
	// mixer does not use its channel data. instead, connected node's channel data.
	// blending occures here.
	// this channel is rotation_channel,

	//// for test
	//this->get_motion_arg(0)->get_channel_data(channel_index, time, data);
	//return;

	zz_motion * arg1, * arg2;

	arg1 = this->get_motion_arg(0);
	arg2 = this->get_motion_arg(1);

	assert(arg1 && arg2);

	vec3 position1, position2, * position3;
	quat rotation1, rotation2, * rotation3;

	//_t_ means relative time in the total time() of 1st argument motion.
	float t = float(time)/ arg1->get_total_time();
	zz_time time2 = long(t*arg2->get_total_time());

		
	if (arg1->channels[channel_index]->is_a(ZZ_RUNTIME_TYPE(zz_channel_position))) {
		assert(arg2->channels[channel_index]->is_a(ZZ_RUNTIME_TYPE(zz_channel_position)));
		arg1->get_channel_data(channel_index, time, (void *)&position1);
		arg2->get_channel_data(channel_index, time2, (void *)&position2);
		position3 = static_cast<vec3 *>(data);
		*position3 = znzin->motion_tool.blend_position(position1, position2, t);
	}
	if (arg1->channels[channel_index]->is_a(ZZ_RUNTIME_TYPE(zz_channel_rotation))) {
		assert(arg2->channels[channel_index]->is_a(ZZ_RUNTIME_TYPE(zz_channel_rotation)));
		arg1->get_channel_data(channel_index, time, (void *)&rotation1);
		arg2->get_channel_data(channel_index, time2, (void *)&rotation2);
		rotation3 = (quat *)data;
		*rotation3 = znzin->motion_tool.blend_rotation(rotation1, rotation2, t);
	}		
}

const vec3& zz_motion_mixer::get_initial_position (void)
{
	return this->get_motion_arg(0)->get_initial_position();
}

const quat& zz_motion_mixer::get_initial_rotation (void)
{
	return this->get_motion_arg(0)->get_initial_rotation();
}

