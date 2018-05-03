/** 
 * @file zz_manager_sound.h
 * @brief 3d sound manager.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-jan-2004
 *
 * $Header: /engine/include/zz_manager_sound.h 3     04-01-28 2:15p Zho $
 * $History: zz_manager_sound.h $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-28   Time: 2:15p
 * Updated in $/engine/include
 * more bug-findable version.
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

#ifndef __ZZ_MANAGER_SOUND_H__
#define __ZZ_MANAGER_SOUND_H__

#ifdef ZZ_USE_SOUND

#ifndef __ZZ_MANAGER_H__
#include "zz_manager.h"
#endif

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

#include <windows.h>
#include <dsound.h>

#include "dsutil.h"

class zz_manager_sound : public zz_manager {
private:
	//LPDIRECTSOUND8 ds_;
	//LPDIRECTSOUND3DLISTENER listener_; // 3D listener object
	//DS3DBUFFER buffer_params_; // 3D buffer properties
	//DS3DLISTENER listener_params_; // Listener properties

	//bool set_primary_buffer_format ( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
	//bool get_3d_listener_interface ( LPDIRECTSOUND3DLISTENER* ppDSListener );
	//bool create ( LPTSTR strWaveFileName );

	void set_listener_position (const vec3& position);
	void set_listener_orientation (const vec3& front, const vec3& top);

public:
	zz_manager_sound ();
	virtual ~zz_manager_sound ();

	bool initialize ();

	static void convert_to_sound_space (vec3& pos_sound_space, const vec3& pos_world_space)
	{
		pos_sound_space.x = pos_world_space.x;
		pos_sound_space.y = pos_world_space.z; // swap y-z
		pos_sound_space.z = pos_world_space.y;
	}

	virtual bool update_listener ();

	ZZ_DECLARE_DYNAMIC(zz_manager_sound);
};

#endif // ZZ_USE_SOUND
#endif // __ZZ_MANAGER_SOUND_H__
