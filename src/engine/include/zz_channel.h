/** 
 * @file zz_channel.h
 * @brief channel struct.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    31-may-2002
 *
 * $Header: /engine/include/zz_channel.h 2     04-07-07 3:29a Zho $
 */

#ifndef	__ZZ_CHANNEL_H__
#define __ZZ_CHANNEL_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

enum  zz_interp_style {
	ZZ_INTERP_NONE = 0,
	ZZ_INTERP_LINEAR = 1,
	ZZ_INTERP_SLERP = 2, // for rotation only. aka. LINEAR
	ZZ_INTERP_SQUAD = 3, // for rotation only
	ZZ_INTERP_CATMULLROM = 4,
	ZZ_INTERP_TCB = 5 // not used yet
};

// channel type
// (be used in zz_max_exporter)
#define ZZ_CTYPE_NONE        (1 << 0)
#define ZZ_CTYPE_POSITION    (1 << 1)
#define ZZ_CTYPE_ROTATION    (1 << 2)
#define ZZ_CTYPE_NORMAL      (1 << 3)
#define ZZ_CTYPE_ALPHA       (1 << 4)
#define ZZ_CTYPE_UV0         (1 << 5)
#define ZZ_CTYPE_UV1         (1 << 6)
#define ZZ_CTYPE_UV2         (1 << 7)
#define ZZ_CTYPE_UV3         (1 << 8)
#define ZZ_CTYPE_TEXTUREANIM (1 << 9)
#define ZZ_CTYPE_SCALE       (1 << 10)

class zz_motion;
/*
channel : is compound of a list of rotation, position, tcb....
          A channel maintains the number of keys or frames in it.
		  A channel gives methods for adding and getting current key data.
		  channel pool is managed by channel manager (channels)
*/
//--------------------------------------------------------------------------------
class zz_channel : public zz_node {
	friend zz_motion;

protected:
	// referred index. used at bone_index or vertex_index
	int refer_id;
	uint32 channel_type;

protected:
	// interpolation type
	zz_interp_style interp_style;
		
public:
	zz_channel(zz_interp_style new_interp_style = ZZ_INTERP_NONE);
	virtual ~zz_channel(void) {};

	int get_refer_id ()
	{
		return refer_id;
	}

	void set_refer_id (int id);

	uint32 get_channel_type ()
	{
		return channel_type;
	}

	void set_channel_type (uint32 type);

	// array related
	virtual void assign (int size) = 0;
	virtual void clear (void) = 0;
	virtual int size (void) = 0;

	// get methods
	virtual void get_by_frame (int frame, void * data_pointer) = 0;
	virtual void get_by_time (zz_time time, int fps, void * data_pointer) = 0;
	
	// set methods
	virtual void set_by_frame (int frame, void * data_poniter) = 0;
	
	void set_interp_style (zz_interp_style interp_style);
	zz_interp_style get_interp_style (void);

	// get frame number from time. 
	// frame_to_time(start_frame) <= current < frame_to_time(next_frame)
	static void time_to_frame
		(zz_time current, int& start_frame, int& next_frame, float& ratio, int size, int fps);

	static zz_time frame_to_time (int frame, int fps)
	{
		return ZZ_FRAME_TO_TICK(frame, fps);
	}

	ZZ_DECLARE_DYNAMIC(zz_channel)
};

/*
  Motion blending is supported by *zz_motion_mixer* class.

  If an user presses the button that triggers character's motion to walk,
  then we get the walking motion data from motion pool
  and copy the motion sequence to the motion channel buffer of the character.
  At the same time, we push the command(ex. cycling command) to the end of the channel.
  Every frame, we look through the model's channel queue, and check if whether there 
  exist pending motion or not.

  The command queue is separated from the motion channel queue.
  
  The command queue gives informations about the next motion sequence :
    - next motion id and when the next motion will be triggered
	- next motion ids and blending options

  For the real-time motion generation, what we need for?
    - We always have to prepare the next motion data.
	how?
	motion channel and next motion description
skeleton or bone
  numbering rule
  from the root bone.
  numbering the order of bones by 
    root bone
	1. first child
	  1.1. first child
	  1.2. second child
	  1.3. third child
	2. second child
	  2.1. first child
	  2.2. second child
	  2.3. third child
	3. third child
	  3.1. first child
	  3.2. second child
	  3.3. third child
  the order is 
    1 -> 1.1 -> 1.2 -> 1.3 -> 2 -> 2.1 -> 2.2 -> 2.3 -> 3 -> 3.1 -> 3.2 -> 3.3
file data : key-base data or frame-base data
  channel data in pool : key-base data by time
  channel data in mixer : frame-base data

for motion retargetting & inclined slope walking & stairway walking
  1 model must have a frame base channel for two step at least.
  motion controller use this channel data for retargetting, then animate it every frame.

man.motion = blend(walking_motion*weight + running_motion*(1-weight))
man.motion = connect(walking_motion, running_motion, interval)
*/

#endif // __ZZ_CHANNEL_H__