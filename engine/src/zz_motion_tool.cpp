/** 
 * @file zz_motion_tool.cpp
 * @brief motion tool class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    19-sep-2002
 *
 * $Header: /engine/src/zz_motion_tool.cpp 3     04-02-11 2:05p Zho $
 * $History: zz_motion_tool.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-08   Time: 7:56p
 * Updated in $/engine/src
 * motion blending reset
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
#include "zz_channel.h"
#include "zz_channel_position.h"
#include "zz_channel_rotation.h"
#include "zz_channel_xy.h"
#include "zz_channel_x.h"
#include "zz_motion.h"
#include "zz_motion_mixer.h"
#include "zz_motion_tool.h"
#include "zz_system.h"
#include "zz_manager.h"

zz_motion_tool::zz_motion_tool(void)
{
}

zz_motion_tool::~zz_motion_tool(void)
{
}

zz_motion_mixer * zz_motion_tool::create_blend_motion (const char * name, zz_motion * motion_arg1, zz_motion * motion_arg2, float blend_weight)
{
	zz_motion_mixer * blended_motion = NULL;

	// create motion
	blended_motion = (zz_motion_mixer *)znzin->motions->spawn(name, ZZ_RUNTIME_TYPE(zz_motion_mixer));
	assert(motion_arg1 && motion_arg2);
	assert(blended_motion);
	blended_motion->link_child(motion_arg1);
	blended_motion->link_child(motion_arg2);
	blended_motion->set_blend_weight(blend_weight);

	return blended_motion;
}

vec3 zz_motion_tool::blend_position (const vec3& pos1, const vec3& pos2, float t_0_to_1)
{
	float weight = sinf(ZZ_HALF_PI*t_0_to_1); // scurve(t_0_to_1);

	return ((1.f - weight)*pos1 + weight*pos2);
}

quat zz_motion_tool::blend_rotation (const quat& rot1, const quat& rot2, float t_0_to_1)
{
	float weight = sinf(ZZ_HALF_PI*t_0_to_1); // scurve(t_0_to_1);

	return qslerp(rot1, rot2, weight);
}

bool zz_motion_tool::load_motion (zz_motion * motion,
								  const char * motion_file_name,
								  bool use_loop,
								  int interp_position,
								  int interp_rotation,
								  float scale)
{
	assert(motion);
	assert(motion_file_name);
	if (!motion || !motion_file_name) return false;

	if (motion->load(motion_file_name, scale) == false) return false;

	motion->set_loop(use_loop);
	zz_interp_style pos_style, rot_style;

	pos_style = static_cast<zz_interp_style>((int)interp_position);
	motion->set_channel_interp_style(ZZ_RUNTIME_TYPE(zz_channel_position), pos_style);
	motion->set_channel_interp_style(ZZ_RUNTIME_TYPE(zz_channel_xy), pos_style); // set by position
	motion->set_channel_interp_style(ZZ_RUNTIME_TYPE(zz_channel_x), pos_style);  // set by position
	
	rot_style = static_cast<zz_interp_style>((int)interp_rotation);
	motion->set_channel_interp_style(ZZ_RUNTIME_TYPE(zz_channel_rotation), rot_style);

	return true;
}



