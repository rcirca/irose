/** 
 * @file zz_animatable.h
 * @brief animatable class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    31-may-2002
 *
 * $Header: /engine/include/zz_animatable.h 4     04-10-12 10:17p Zho $
 * $History: zz_animatable.h $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-10-12   Time: 10:17p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-11   Time: 11:35a
 * Updated in $/engine/include
 * void apply_motion() -> bool apply_motion()
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:05p
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
 * User: Zho          Date: 03-11-30   Time: 7:47p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:19p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 16  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef __ZZ_ANIMATABLE_H__
#define __ZZ_ANIMATABLE_H__

#ifndef __ZZ_VISIBLE_H__
#include "zz_visible.h"
#endif

#ifndef __ZZ_TIMER_H__
#include "zz_timer.h"
#endif

#ifndef __ZZ_MOTION_CONTROLLER__
#include "zz_motion_controller.h"
#endif

class zz_motion;
class zz_motion_mixer;
class zz_channel_position;
class zz_channel_rotation;

//--------------------------------------------------------------------------------
class zz_animatable : public zz_visible {
protected:
	zz_motion * motion; // copied snapshot from motion_controller
	zz_motion_controller motion_controller;

public:
	zz_animatable(void);
	virtual ~zz_animatable(void);

	// apply motion
	virtual bool apply_motion (void); // returns true if motion was successfully applied.
	virtual void attach_motion (zz_motion * motion_to_attach);
	virtual void detach_motion ();
	virtual void update_time (bool recursive, zz_time diff_time);
	virtual void update_animation (bool recursive, zz_time diff_time);

	zz_time get_motion_time ();
	unsigned int get_motion_frame ();

	bool set_motion_time (zz_time set_time);
	bool set_motion_frame (int frame);

	zz_motion * get_motion (void);
	zz_motion_mixer * blend_motion (zz_motion * motion_arg1, zz_motion * motion_arg2);
	
	virtual void set_repeat_count (int count);
	virtual int get_repeat_count ();
	
	float get_motion_speed ();
	void set_motion_speed (float speed_in);

	virtual void play ();
	virtual void stop ();
	virtual void pause ();
	virtual int get_motion_state();

	zz_time get_start_delay ();
	void set_start_delay (zz_time delay_in);

	ZZ_DECLARE_DYNAMIC(zz_animatable)
};

inline zz_motion * zz_animatable::get_motion (void)
{
	return motion;
}

inline void zz_animatable::set_repeat_count (int count)
{
	motion_controller.repeat_count = count;
}

inline int zz_animatable::get_repeat_count ()
{
	return motion_controller.repeat_count;
}

inline float zz_animatable::get_motion_speed ()
{
	return motion_controller.get_speed();
}

inline void zz_animatable::set_motion_speed (float speed_in)
{
	motion_controller.set_speed(speed_in);
}

inline void zz_animatable::play ()
{
	motion_controller.play();
}

inline void zz_animatable::stop ()
{
	motion_controller.stop();
}

inline void zz_animatable::pause ()
{
	motion_controller.pause();
}

inline int zz_animatable::get_motion_state()
{
	return motion_controller.get_motion_state();
}

inline zz_time zz_animatable::get_start_delay ()
{
	return motion_controller.get_start_delay();
}

inline void zz_animatable::set_start_delay (zz_time delay_in)
{
	motion_controller.set_start_delay(delay_in);
}

#endif //__ZZ_ANIMATABLE_H__