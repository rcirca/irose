/** 
 * @file zz_channel_rotation.h
 * @brief channel_rotation struct.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/include/zz_channel_rotation.h 2     04-07-07 3:29a Zho $
 */

#ifndef	__ZZ_CHANNEL_ROTATION_H__
#define __ZZ_CHANNEL_ROTATION_H__

#include <vector>

#ifndef __ZZ_CHANNEL_H__
#include "zz_channel.h"
#endif

class zz_motion;

//--------------------------------------------------------------------------------
class zz_channel_rotation : public zz_channel {
	friend class zz_motion;

protected:
	quat * rotations;
	unsigned int num_rotations;
	
public:
	zz_channel_rotation(void);
	virtual ~zz_channel_rotation(void);

	// vector related
	virtual void assign (int size);
	virtual void clear (void);
	virtual int size (void);

	// set methods
	virtual void get_by_frame (int frame, void * data_pointer);
	virtual void get_by_time (zz_time time, int fps, void * data_pointer);
	
	// set methods
	virtual void set_by_frame (int frame, void * data_poniter);

	ZZ_DECLARE_DYNAMIC(zz_channel_rotation)
};

#endif // __ZZ_CHANNEL_ROTATION_H__