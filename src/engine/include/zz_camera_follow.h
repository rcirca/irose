/** 
 * @file zz_camera_follow.h
 * @brief follow camera class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    18-feb-2003
 *
 * $Header: /engine/include/zz_camera_follow.h 17    05-02-14 3:16p Choo0219 $
 * $History: zz_camera_follow.h $
 * 
 * *****************  Version 17  *****************
 * User: Choo0219     Date: 05-02-14   Time: 3:16p
 * Updated in $/engine/include
 * 
 * *****************  Version 16  *****************
 * User: Choo0219     Date: 04-12-02   Time: 11:42a
 * Updated in $/engine/include
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-07-19   Time: 12:15p
 * Updated in $/engine/include
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-07-01   Time: 11:48a
 * Updated in $/engine/include
 * 7.1.4526
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-06-24   Time: 2:12p
 * Updated in $/engine/include
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-03-25   Time: 11:18a
 * Updated in $/engine/include
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-03-25   Time: 10:33a
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-02-03   Time: 8:36p
 * Updated in $/engine/include
 * Added get_target()
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-12-10   Time: 9:47p
 * Updated in $/engine/include
 * When camera attached, reset time_weight_last = .02f (heuristic)
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/include
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-05   Time: 8:08p
 * Updated in $/engine/include
 * camera code cleanup
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-05   Time: 7:16p
 * Updated in $/engine/include
 * code cleanup
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-05   Time: 4:43p
 * Updated in $/engine/include
 * let stop follow_camera when properties are not changed.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-04   Time: 7:00p
 * Updated in $/engine/include
 * adjusted shake = 30.0f
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-04   Time: 5:55p
 * Updated in $/engine/include
 * more precise camera test
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
 * *****************  Version 19  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef __ZZ_CAMERA_FOLLOW_H__
#define __ZZ_CAMERA_FOLLOW_H__

#ifndef __ZZ_CAMERA_H__
#include "zz_camera.h"
#endif

class zz_model;

//--------------------------------------------------------------------------------
class zz_camera_follow : public zz_camera {
public:
	enum zz_follow_mode {
		LOOK_MODE = 0,
		BACK_MODE = 1
	};

private:
	struct zz_camera_follow_state {
		vec3 target_pos; // target position
		vec3 target_dir; // target lookat vector
		vec3 camera_pos; // camera eye position
		vec3 camera_dir; // camera to target direction
		float yaw; // left-right angle
		float pitch; // up-down angle
		float distance; // camera to target distance
	};

	// for internal use in update()
	float time_weight_;       // time weight

	// distance range
	float distance_range_min, distance_range_max; // min/max

	// camera state
	zz_camera_follow_state last_;
	zz_camera_follow_state current_;
	zz_camera_follow_state final_;

	zz_model * target_; // target node to follow
	zz_follow_mode follow_mode_; // 0 : back mode, 1 : look mode
	float height_added_; // camera focus height
	bool newly_attached_; // true if camera attached now. false when the other frames. set by attch_target()
	bool is_dirty_; // whether camera property has been updated or not. set by set_yaw(), set_distance(), set_pidth(), attach_target(), set_follow_mode()...
	bool now_following_;

	zz_time shake_lifetime;
	vec3 shake_min, shake_max;

	bool camera_effect_onoff;
    float final_time;
	float accumulate_time;

	bool apply_shake (zz_time diff_time); // return true if it is now shakeing

	float calc_yaw();
	bool update_backmode ();
	bool update_lookmode (float follow_yaw_last);
	void interpolate_camera (zz_time diff_time); // adjust camera properties by time
    void update_camera_collision();

	// get distance from camera bottom to contacted point
	// @return true if contacted, and false if not contacted
	bool get_distance_by_contact_point (float& adjusted_distance);

	float get_yaw_by_eye (const vec3& eye_pos);
	float get_pitch_by_eye (const vec3& eye_pos);
	float get_distance_by_eye (const vec3& eye_pos);
	void get_adjusted (float& adjusted_pitch, float& adjusted_distance, float& adjusted_yaw);

    
public:
	zz_camera_follow ();
	virtual ~zz_camera_follow ();

	bool attach_target (zz_model * target_vis);
	bool detach_target (void);

	virtual zz_model * get_target ();

	void get_target_pos (vec3& pos_out);
	bool get_target_dir (vec3& dir);

	void set_yaw (float new_yaw);
	void set_pitch (float new_pitch);
	void set_distance (float new_dist);

	void set_distance_range (float min_in, float max_in);

	float get_yaw ();
	float get_pitch ();
	float get_distance ();

	void set_height_added (float height);

	static void apply_yaw (vec3& new_camera_dir_out, float follow_yaw_in, const vec3& target_dir_in);
	static const vec3& apply_pitch (vec3& new_camera_dir_inout, float follow_pitch_in);

	void set_follow_mode (zz_follow_mode mode);

	virtual void update_time (bool recursive, zz_time diff_time);

	void set_shake (zz_time lifetime_in, const vec3& min_in, const vec3& max_in);

	bool get_camera_effect_onoff();
	void interpolate_camera_effect(zz_time diff_time);
	void play_camera_effect(float yaw, float pitch, float length, float time);
	void stop_camera_effect();

	ZZ_DECLARE_DYNAMIC(zz_camera_follow)
};

inline zz_model * zz_camera_follow::get_target ()
{
	return target_;
}

#endif // __ZZ_CAMERA_FOLLOW_H__