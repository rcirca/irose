/** 
 * @file zz_sound.h
 * @brief 3d sound class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-jan-2004
 *
 * $Header: /engine/include/zz_sound.h 4     04-01-28 2:15p Zho $
 * $History: zz_sound.h $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-28   Time: 2:15p
 * Updated in $/engine/include
 * more bug-findable version.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-27   Time: 7:00p
 * Updated in $/engine/include
 * Selective sound support.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-07   Time: 10:07a
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-07   Time: 9:36a
 * Created in $/engine/include
 * Added 3D sound system.
 * 
 */

#ifndef __ZZ_SOUND_H__
#define __ZZ_SOUND_H__

//#define ZZ_USE_SOUND

#ifdef ZZ_USE_SOUND

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

#include "dsutil.h"

class zz_manager_sound;

class zz_sound : public zz_node {
	friend class zz_manager_sound;

private:
	CSound * pSound;	
	LPDIRECTSOUND3DBUFFER pDS3DBuffer; // DIRECT SOUND 3D BUFFER

	zz_string filename;
	vec3 position;
	unsigned int num_buffers;

	LPDIRECTSOUND3DBUFFER get_free_buffer ();

public:
	zz_sound ();
	virtual ~zz_sound ();

	// this is valid before load()
	bool set_property (const char * wavefilename, unsigned int num_buffers_in);

	virtual bool load ();

	bool play (bool loop = false);

	bool stop ();

	bool is_playing ();

	bool set_position (const vec3& position_world_in);
	bool set_velocity (const vec3& velocity_world_in);
	bool set_distance_min (float min_in);
	bool set_distance_max (float max_in);

	ZZ_DECLARE_DYNAMIC(zz_sound);
};

#endif // ZZ_USE_SOUND
#endif // __ZZ_SOUND_H__
