/** 
 * @file zz_animatable.h
 * @brief animatable class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-mar-2002
 *
 * $Header: /engine/src/zz_animatable.cpp 17    04-10-12 10:17p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_motion.h"
#include "zz_system.h"
#include "zz_profiler.h"
#include "zz_motion_mixer.h"
#include "zz_channel_position.h"
#include "zz_channel_rotation.h"
#include "zz_manager.h"
#include "zz_animatable.h"

ZZ_IMPLEMENT_DYNCREATE(zz_animatable, zz_visible)

zz_animatable::zz_animatable(void) :
	motion(NULL)
{
}

zz_animatable::~zz_animatable(void)
{
}

zz_motion_mixer * zz_animatable::blend_motion (zz_motion * motion_arg1, zz_motion * motion_arg2)
{
	zz_motion_mixer * blended_motion = NULL;

	// create motion
	blended_motion = (zz_motion_mixer *)znzin->motions->spawn(NULL, ZZ_RUNTIME_TYPE(zz_motion_mixer));
	assert(blended_motion);
	blended_motion->set_owner(this);
	blended_motion->link_child(motion_arg1);
	blended_motion->link_child(motion_arg2);

	return blended_motion;
}

void zz_animatable::attach_motion (zz_motion * motion_to_attach)
{
	motion = motion_to_attach;

	if (!motion) {
		motion_controller.stop();
		motion_controller.set_motion(NULL);
		return;
	}

	motion_controller.repeat_count = motion->get_loop() ? 0 : 1;
	motion_controller.stop();
	motion_controller.set_motion(motion_to_attach);

	zz_time max_interval = motion->get_total_time();
	zz_time interp_interval = motion->get_interp_interval();
	interp_interval = ZZ_MIN(interp_interval, max_interval);
	interp_interval = ZZ_MAX(interp_interval, 0);

	motion_controller.set_interp_interval(interp_interval);

	motion_controller.play();

	//apply_motion();
	//update(false, 0); // commented out because this could cause multi thread problem in morpher update that calls update_index_buffer()
}

void zz_animatable::detach_motion ()
{
	motion_controller.set_motion(NULL);
	if (!motion) return;
	if (motion->get_owner() == this)
		//ZZ_SAFE_RELEASE(motion);
		znzin->motions->kill(motion);
}

zz_time zz_animatable::get_motion_time ()
{
    return motion_controller.get_current_time();
}

unsigned int zz_animatable::get_motion_frame ()
{
	// get current frame index
	return motion_controller.get_current_frame();
}

bool zz_animatable::set_motion_time (zz_time set_time_in)
{
	motion_controller.set_current_time(set_time_in);
	return true;
}

bool zz_animatable::set_motion_frame (int frame)
{
	motion_controller.set_current_time(ZZ_FRAME_TO_TICK(frame, motion_controller.get_fps()));
	return true;
}

bool zz_animatable::apply_motion (void)
{
	//ZZ_PROFILER_INSTALL(Papply_motion);

	if (!motion) return false; // no motion

	int num_channels = motion->get_num_channels();

	if (num_channels == 0) return false;

	zz_time time = motion_controller.get_current_time();

	int fps = motion_controller.get_fps();

	uint32 channel_type;
	float scale_x;
	bool to_update = false;
	for (int channel_index = 0; channel_index < num_channels; channel_index++) {
        channel_type = motion->get_channel_type(channel_index);
		switch (channel_type) {
			case ZZ_CTYPE_POSITION:
				motion->get_channel_data(channel_index, time, &this->position, fps);
				to_update = true;
				break;
			case ZZ_CTYPE_ROTATION:
				motion->get_channel_data(channel_index, time, &this->rotation, fps);
				to_update = true;
				break;
			case ZZ_CTYPE_SCALE:
				motion->get_channel_data(channel_index, time, &scale_x, fps);
				scale.set(scale_x, scale_x, scale_x);
				to_update = true;
				break;
			case ZZ_CTYPE_ALPHA:
				motion->get_channel_data(channel_index, time, &this->visibility, fps);
				//ZZ_LOG("animatable: apply_motion(%s), channel(%d), visibility(%f)\n", 
				//	get_name(), channel_index, visibility);
				break;
		}
	}

	return true;
}

void zz_animatable::update_animation (bool recursive, zz_time diff_time)
{
	if (motion_controller.is_playing()) {
		// update animation if it is now playing and in viewfrustum
		if (apply_motion()) { // apply_motion is called only if PLAY state and PLAY->STOP state
			invalidate_transform();
		}
	}
	if (recursive && children.size() > 0)
		zz_visible::update_animation(recursive, diff_time);
}

void zz_animatable::update_time (bool recursive, zz_time diff_time)
{
	if (!motion_controller.update(diff_time)) { // have some problem, stop motion
		// state transition. PLAY->STOP
		stop();
		// if it was playing, stop animation and apply this motion.
		if (apply_motion()) {
			invalidate_transform();
		}
	}
	zz_visible::update_time(recursive, diff_time);
}
