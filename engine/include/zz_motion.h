/** 
 * @file zz_motion.h
 * @brief motion class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/include/zz_motion.h 9     05-07-11 10:06a Choo0219 $
 * $History: zz_motion.h $
 * 
 * *****************  Version 9  *****************
 * User: Choo0219     Date: 05-07-11   Time: 10:06a
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-10-15   Time: 2:47p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-07-07   Time: 3:29a
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-03-01   Time: 4:56p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-23   Time: 10:21a
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-19   Time: 1:55a
 * Updated in $/engine/include
 * added get_path() and file_name member.
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
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef	__ZZ_MOTION_H__
#define __ZZ_MOTION_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_CHANNEL_H__
#include "zz_channel.h"
#endif

#include <vector>

class zz_mesh;
class zz_animatable;

//--------------------------------------------------------------------------------
class zz_motion : public zz_node {
protected:
	uint32 fps; // frame per second
	uint32 num_frames;
	bool do_loop; // does this motion have cycle

	zz_time interp_interval; // time delay to be interpolated between last motion and new motion

	// Any assigned value to owner variable means that this motion is temporary.
	// This motion should be deallocated by owner animatable object after using it.
	// If this motion is the shared motion, do not set the *owner* pointer. It should be NULL.
	zz_animatable * owner;

	// for initial motion transform
	// determined at loading time (zz_motion::load())
	vec3 initial_position;
	quat initial_rotation;
	
	// animation moving direction vector (end_frame_position - start_frame_position)
	// determined at loading time (zz_motion::load())
	vec3 direction_vector; 

	zz_string filename; // motion file name
	unsigned int num_channels;

public:
	zz_channel ** channels; // stay in znzin->channels
	
	zz_motion(void);
	virtual ~zz_motion(void);

	bool load (const char * file_name, float scale_in_load = 1.0f);
	virtual bool unload ();

	virtual int get_num_frames (void) { return num_frames; }
	virtual zz_time get_total_time (int custum_fps = 0) {
		if (custum_fps == 0) custum_fps = get_fps();
		return zz_time(float(ZZ_TICK_PER_SEC)*(get_num_frames()-1)/custum_fps);
	}
	virtual int get_fps (void) { return fps; }
	//void set_fps (int fps) { this->fps = fps; }

	int get_num_channels ()
	{
		return num_channels;
	}

	void set_loop (bool is_loop);
	bool get_loop (void);

	void set_channel_interp_style (zz_node_type * channel_type, zz_interp_style style);
	const vec3& get_initial_position (void) { return initial_position; }
	const quat& get_initial_rotation (void) { return initial_rotation; }
	mat4& get_initialTM (mat4& initial_tm) {
		initial_tm = mat4_id;
		initial_tm.set_translation(initial_position);
		initial_tm.set_rot(initial_rotation);
		return initial_tm;
	}
	void set_owner (zz_animatable * owner) { this->owner = owner; }
	zz_animatable * get_owner () { return this->owner; }

	// get by frame
	virtual void get_channel_data (int channel_index, int frame, void * data);

	// get by time
	virtual void get_channel_data (int channel_index, zz_time time, void * data, int custum_fps = 0);
	virtual void set_channel_data (int channel_index, zz_time time, void * data, int custum_fps = 0);

	uint32 get_channel_type ( int channel_index );
	
	const vec3& get_direction_vector (void) { return direction_vector; }

	void set_interp_interval (zz_time interval_in);
	zz_time get_interp_interval (void) { return interp_interval; }
	
	const char * get_path () { return filename.get(); }

	// update mesh with motion
	// @mesh mesh
	// @current_frame current frame
	// @num_verts number of vertices
	// @alpha_out receives visibility value
	// @return true if got alpha(visiblity) animation,
	bool update_mesh (zz_mesh * mesh, int current_frame, int num_verts, float& alpha_out);

	ZZ_DECLARE_DYNAMIC(zz_motion)
};

inline void zz_motion::get_channel_data (int channel_index, zz_time time, void * data, int custum_fps)
{
//	assert(channel_index < (int)num_channels);
//	assert(channels[channel_index]);
	
	if(channel_index < (int)num_channels)
	channels[channel_index]->get_by_time(time, custum_fps ? custum_fps : fps, data);
}

inline void zz_motion::get_channel_data (int channel_index, int frame, void * data)
{
//	assert(frame <= get_num_frames());
//	assert(channel_index < (int)num_channels);
	
	if(channel_index < (int)num_channels)
	channels[channel_index]->get_by_frame(frame, data);
}

inline void zz_motion::set_channel_data (int channel_index, zz_time time, void * data, int custum_fps)
{
	assert(channel_index < (int)num_channels);
	channels[channel_index]->set_by_frame(ZZ_TICK_TO_FRAME(time, custum_fps ? custum_fps : fps), data);
}

inline uint32 zz_motion::get_channel_type ( int channel_index )
{
	assert(channel_index < (int)num_channels);
	return channels[channel_index]->get_channel_type();
}

inline void zz_motion::set_interp_interval (zz_time interval_in)
{
	interp_interval = interval_in;
}

inline void zz_motion::set_loop (bool is_loop)
{
	do_loop = is_loop;
}

inline bool zz_motion::get_loop (void)
{
	return do_loop;
}

#endif // __ZZ_MOTION_H__