/** 
 * @file zz_motion_mixer.h
 * @brief motion mixer class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/include/zz_motion_mixer.h 3     04-06-23 9:33a Zho $
 * $History: zz_motion_mixer.h $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-03-01   Time: 4:56p
 * Updated in $/engine/include
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef	__ZZ_MOTION_MIXER_H__
#define __ZZ_MOTION_MIXER_H__

#ifndef __ZZ_MOTION_H__
#include "zz_motion.h"
#endif

//--------------------------------------------------------------------------------
class zz_motion_mixer : public zz_motion {
	float blend_weight;

public:	
	zz_motion_mixer(void);
	virtual ~zz_motion_mixer(void);

	// these methods use just first argument motion's.
	virtual int get_num_frames (void);
	virtual zz_time get_total_time (void);
	virtual int get_fps (void);
	virtual void get_channel_data (int channel_index, zz_time time, void * data);

	void set_blend_weight (float blend_weight) { this->blend_weight = blend_weight; }

	zz_motion * get_motion_arg (int index); // get argument motion by index (0-1)

	const vec3& get_initial_position (void);
	const quat& get_initial_rotation (void);

	ZZ_DECLARE_DYNAMIC(zz_motion_mixer)
};

#endif // __ZZ_MOTION_MIXER_H__