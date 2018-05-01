/** 
 * @file zz_motion_controller.h
 * @brief motion controller class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-oct-2002
 *
 * $Header: /engine/src/zz_motion_controller.cpp 6     04-07-14 6:28p Zho $
 * $History: zz_motion_controller.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-07-14   Time: 6:28p
 * Updated in $/engine/src
 * SYSTEM TIME REDESIGNED
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-06-29   Time: 10:09a
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-16   Time: 3:21p
 * Updated in $/engine/src
 * renamed blend_duration to interp_interval
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
 * *****************  Version 22  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_motion.h"
#include "zz_motion_controller.h"

#define ZZ_DEFAULT_SPEED (1.0f)
#define ZZ_DEFAULT_FPS (30)

zz_motion_controller::zz_motion_controller() :
	_repeating(false),
	_fps_motion(ZZ_DEFAULT_FPS),
	_wait_start_delay(0),
	_repeated_count(0),
	motion(NULL),
	speed(ZZ_DEFAULT_SPEED),
	start_delay(0),
	repeat_count(0),
	motion_state(ZZ_STOPPED),
	interp_interval(0),
	fps(ZZ_DEFAULT_FPS),
	current_time(0), 
	current_frame(0),
	num_frames(0),
	total_time(0)
{
}

zz_motion_controller::~zz_motion_controller()
{
}

void zz_motion_controller::play (void)
{
	if (is_playing()) return;

	if (motion_state == ZZ_STOPPED) {
		// initialize
		current_time = 0;
		current_frame = 0;
		_wait_start_delay = 0;
		_repeated_count = 0;
		_repeating = false;
	}

	if (start_delay > 0) { // if we have to wait until start_delay
		motion_state = ZZ_READY_TO_PLAY;
	}
	else {
		motion_state = ZZ_PLAYING;
	}
}

void zz_motion_controller::pause (void)
{
	if (is_paused()) return;

	motion_state = ZZ_PAUSED;
}

void zz_motion_controller::stop (void)
{
	if (is_stopped()) return;

	_repeating = false; // initialize _repeating to false
	motion_state = ZZ_STOPPED;
}

// @return false if state is PLAY -> STOP, true if state is not changed or play state.
bool zz_motion_controller::update (zz_time diff_time)
{
	assert(diff_time >= 0);
	
	// no need to have motion. ex) particle_emitter does not have motion

	switch (this->motion_state) {
		case ZZ_READY_TO_PLAY:
			{
				this->_wait_start_delay += diff_time;
				//ZZ_LOG("motion_contllor::update(). start_delay(%d), _wait_start_delay(%d)\n",
				//	start_delay, _wait_start_delay);

				if (this->_wait_start_delay >= this->start_delay) {
					motion_state = ZZ_PLAYING;
					current_time = _wait_start_delay - start_delay; // apply difference
				}
				else {
					return true; // do nothing
				}
			}
			break;
		case ZZ_PLAYING:
			break; // go forward
		default:
			// maybe STOPPED
			return true; // do nothing
	}

	if (num_frames <= 0 || total_time <= 0) {
		current_frame = 0;
		current_time = 0;
		return true;
	}

	current_time += diff_time;

	if (repeat_count > 0) { // finite repeat count
		if (current_time > total_time) { // if exceeds total_time
			_repeated_count += (current_time / total_time);
			current_time = current_time%total_time;
		}
		if (_repeated_count >= repeat_count) { // already exceeded repeat count
			current_time = total_time; 	// set to the end time
			current_frame = num_frames-1;
			return false; // let the animatable to stop
		}
	}
	// infinite repeat count
	else if (current_time > total_time) { // if exceeds total_time
		current_time = current_time%total_time;
		_repeating = true;
		return true;
	}

	current_frame = ZZ_TICK_TO_FRAME(current_time, fps);
	return true;
}

float& zz_motion_controller::get_blend_weight (float& blend_weight_out)
{
	if (!_repeating && (current_time < interp_interval)) {
		blend_weight_out = (float)current_time / interp_interval;
		blend_weight_out *= blend_weight_out;
	}
	else {
		blend_weight_out = 1.0f;
	}
	return blend_weight_out;
}

void zz_motion_controller::set_motion (zz_motion * motion_in)
{
	motion = motion_in;
	reset_speed();
}

void zz_motion_controller::reset_speed ()
{
	if (!motion) {
		num_frames = 0;
		total_time = 0;
		current_time = 0;
		current_frame = 0;
		return;
	}

	_fps_motion = motion->get_fps();
	fps = int(speed * float(_fps_motion));

	// if time exceeds maximum time, rewind it
	num_frames = motion->get_num_frames();
	total_time = ZZ_FRAME_TO_TICK(num_frames-1, fps);
}
