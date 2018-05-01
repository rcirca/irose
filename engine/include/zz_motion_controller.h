/** 
 * @file zz_motion_controller.h
 * @brief motion controller class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-oct-2002
 *
 * $Header: /engine/include/zz_motion_controller.h 4     04-06-29 10:09a Zho $
 * $History: zz_motion_controller.h $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-06-29   Time: 10:09a
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-16   Time: 3:21p
 * Updated in $/engine/include
 * renamed blend_duration to interp_interval
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:53p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_MOTION_CONTROLLER_H__
#define __ZZ_MOTION_CONTROLLER_H__

enum zz_motion_state {
	ZZ_STOPPED       = 0,
	ZZ_PLAYING       = 1,
	ZZ_PAUSED        = 2,
	ZZ_NOSPAWN       = 3,
	ZZ_READY_TO_PLAY = 4,
};

class zz_motion;

//--------------------------------------------------------------------------------
class zz_motion_controller {
private:
	// waited time to sync with start_delay.
	// set by play()
	zz_time _wait_start_delay;
	int _repeated_count;

	// Whether the current motion time exceeded the total time or not.
	// If we attached the new motion or stopped the current motion,
	//   then _repeating will be set to false.
	// If we are animating beyond the total animation time,
	//   then _repeating will be set to true.
	// Default value is false.
	bool _repeating;
	int _fps_motion; // motion fps
	
protected:
	// no need to have current_frame. This *time* has the strict correspondence to the frame
	// computed by : current_time = timer.get_time() - start_time;
	zz_time current_time;
	int current_frame;
	float speed; // default 1.0f. custum_fps = speed * motion->get_fps()
	zz_time start_delay; // time delay before starting
	int fps; // frame per second. updated by speed and motion

	zz_time total_time;
	int num_frames;

	zz_motion * motion;

	zz_motion_state motion_state;

	// motion blending interval time used to calculate blend_weight
	zz_time interp_interval; // blend_weight = current_time / interp_interval

public:
	int repeat_count; // 0 means infinite

	zz_motion_controller();
	virtual ~zz_motion_controller();

	zz_time get_current_time ()
	{
		return current_time;
	}

	void set_current_time (zz_time time_in);
	int get_current_frame ()
	{
		return current_frame;
	}

	bool has_motion ();
	bool is_playing ();
	bool is_stopped ();
	bool is_paused  ();
	int get_motion_state ();

	// motion control
	void play (void);
	void stop (void);
	void pause (void);

	zz_motion * get_motion ();

	// set motion
	// if motion_in is null, stop motion.
	void set_motion (zz_motion * motion_in);
	
	void set_speed (float new_speed);

	// if this update() fails, should have to stop animatable itself.
	bool update (zz_time diff_time);

	float get_speed ();
	zz_time get_start_delay ();
	void set_start_delay (zz_time delay_in);
	void set_interp_interval (zz_time interval);
	zz_time get_interp_interval ();

	int get_fps () { return fps; }

	// no need to use these
	//int convert_to_frame (zz_time time_in);
	//zz_time convert_to_time (int frame_in);

	float& get_blend_weight (float& blend_weight_out);

	// recalc num_frame, total_time, fps...
	void reset_speed ();
};

inline void zz_motion_controller::set_current_time (zz_time time_in)
{
	current_time = time_in;
	current_frame = ZZ_TICK_TO_FRAME(current_time, fps);
}

inline bool zz_motion_controller::has_motion (void)
{
	return (motion != NULL);
}

inline bool zz_motion_controller::is_playing ()
{
	return (motion_state == ZZ_PLAYING);
}

inline bool zz_motion_controller::is_stopped ()
{
	return (motion_state == ZZ_STOPPED);
}

inline bool zz_motion_controller::is_paused  ()
{
	return (motion_state == ZZ_PAUSED);
}

inline int zz_motion_controller::get_motion_state ()
{
	return motion_state;
}

inline zz_motion * zz_motion_controller::get_motion ()
{
	return motion;
}

inline void zz_motion_controller::set_speed (float new_speed)
{
	//assert(speed >= 0);
	speed = ZZ_MAX(new_speed, 0);
	reset_speed();
}

inline float zz_motion_controller::get_speed ()
{
	return speed;
}

inline zz_time zz_motion_controller::get_start_delay ()
{
	return start_delay;
}

inline void zz_motion_controller::set_start_delay (zz_time delay_in)
{
	start_delay = delay_in;
}

inline void zz_motion_controller::set_interp_interval (zz_time interval)
{
	interp_interval = interval;
}

inline zz_time zz_motion_controller::get_interp_interval ()
{
	return interp_interval;
}

//inline int zz_motion_controller::convert_to_frame (zz_time time_in)
//{
//	return ZZ_TICK_TO_FRAME(time_in, fps);
//}
//
//inline zz_time zz_motion_controller::convert_to_time (int frame_in)
//{
//	return ZZ_FRAME_TO_TICK(frame_in, fps);
//}

#endif // __ZZ_MOTION_CONTROLLER_H__