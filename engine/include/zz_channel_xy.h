/** 
 * @file zz_channel_xy.h
 * @brief 2D-channel class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    12-feb-2003
 *
 * $Header: /engine/include/zz_channel_xy.h 2     04-07-07 3:29a Zho $
 */

#ifndef	__ZZ_CHANNEL_XY_H__
#define __ZZ_CHANNEL_XY_H__

#include <vector>

#ifndef __ZZ_CHANNEL_H__
#include "zz_channel.h"
#endif

class zz_motion;

//--------------------------------------------------------------------------------
class zz_channel_xy : public zz_channel {
	friend class zz_motion;

protected:
	vec2 * xys;
	unsigned int num_xys;

public:
	zz_channel_xy(void);
	virtual ~zz_channel_xy(void);

	// vector related
	virtual void assign (int size);
	virtual void clear (void);
	virtual int size (void);

	// set methods
	virtual void get_by_frame (int frame, void * data_pointer);
	virtual void get_by_time (zz_time time, int fps,  void * data_pointer);
	
	// set methods
	virtual void set_by_frame (int frame, void * data_pointer);

	ZZ_DECLARE_DYNAMIC(zz_channel_xy)
};

#endif // __ZZ_CHANNEL_XY_H__