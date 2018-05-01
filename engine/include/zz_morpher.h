/** 
 * @file zz_morpher.h
 * @brief morpher base class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    08-oct-2002
 *
 * $Header: /engine/include/zz_morpher.h 5     04-07-07 3:29a Zho $
 */

#ifndef __ZZ_MORPHER_H__
#define __ZZ_MORPHER_H__

#ifndef __ZZ_ANIMATABLE_H__
#include "zz_animatable.h"
#endif


// mesh morpher
class zz_morpher : public zz_animatable {
	// channel type combination. ex) ZZ_CTYPE_POSITION | ZZ_CTYPE_NORMAL | ZZ_CTYPE_UV0
	uint32 channel_types;

public:
	zz_morpher ();

	virtual ~zz_morpher () {}

	virtual bool apply_motion (void);

	virtual void render (bool recursive);

	virtual void update_animation (bool recursive, zz_time diff_time);

	ZZ_DECLARE_DYNAMIC(zz_morpher)
};

#endif __ZZ_MORPHERER_H__