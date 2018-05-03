/** 
 * @file zz_sound.cpp
 * @brief 3d sound class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-jan-2004
 *
 * $Header: /engine/src/zz_sound.cpp 5     04-02-11 2:05p Zho $
 * $History: zz_sound.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-27   Time: 7:00p
 * Updated in $/engine/src
 * Selective sound support.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-07   Time: 10:07a
 * Updated in $/engine/src
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-07   Time: 9:41a
 * Updated in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-07   Time: 9:36a
 * Created in $/engine/src
 * Added 3D sound system.
 * 
 */

#ifdef ZZ_USE_SOUND

#include "zz_tier0.h"
#include <windows.h>
#include <mmsystem.h>
#include <dxerr9.h>
#include <dsound.h>
#include "DSUtil.h"
#include "DXUtil.h"

#include "zz_sound.h"
#include "zz_manager_sound.h"
#include "zz_log.h"

ZZ_IMPLEMENT_DYNCREATE(zz_sound, zz_node)

extern CSoundManager * g_pSoundManager;

zz_sound::zz_sound () :
	pSound(NULL),
	pDS3DBuffer(NULL),
	num_buffers(1)
{
}

zz_sound::~zz_sound ()
{
	SAFE_DELETE(pSound);
	SAFE_RELEASE(pDS3DBuffer);
}

bool zz_sound::set_property (const char * wavefilename, unsigned int num_buffers_in)
{
	if (!wavefilename) return false;

	filename.set(wavefilename);

	num_buffers = num_buffers_in;

	return true;
}

bool zz_sound::load ()
{
	if (!g_pSoundManager) return false;

	if (pSound) return true; // already loaded

	GUID guid3DAlgorithm = GUID_NULL;

	LPTSTR wavefilename = const_cast<char *>(this->filename.get());

	if (FAILED(g_pSoundManager->Create( &pSound, wavefilename,  DSBCAPS_CTRL3D, guid3DAlgorithm, static_cast<DWORD>(num_buffers)))) {
		return false;
	}

	return true;
}

bool zz_sound::play (bool loop)
{
	return !(FAILED(pSound->Play(0, (loop) ? DSBPLAY_LOOPING : 0)));
}

bool zz_sound::stop ()
{
	return !(FAILED(pSound->Stop()));
}

bool zz_sound::is_playing ()
{
	return (pSound->IsSoundPlaying() == TRUE);
}

LPDIRECTSOUND3DBUFFER zz_sound::get_free_buffer ()
{
	LPDIRECTSOUND3DBUFFER pDS3DB;
	LPDIRECTSOUNDBUFFER pDSB;
	DWORD dwStatus = 0;

	for( DWORD i=0; i< this->num_buffers; i++ )
    {
		pDSB = pSound->GetBuffer(i);
        if( pDSB )
        {  
            dwStatus = 0;
            pDSB->GetStatus( &dwStatus );
			if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 ) {
				if (FAILED(pSound->Get3DBufferInterface( i, &pDS3DB ))) {
					ZZ_LOG("sound: get_free_buffer() failed. Get3DBufferInterface() failed.\n");
					return NULL;
				}
				return pDS3DB;
			}  
        }
    }
	return NULL;
}

bool zz_sound::set_position (const vec3& position_world_in)
{
	// get free buffer
	pDS3DBuffer = this->get_free_buffer();

	if (!pDS3DBuffer) return false;

	vec3 pos_sound;
	zz_manager_sound::convert_to_sound_space(pos_sound, position_world_in);

	return !FAILED(pDS3DBuffer->SetPosition(pos_sound.x, pos_sound.y, pos_sound.z, DS3D_DEFERRED)); // DS3D_IMMEDIATE); // DS3D_DEFERRED);
}

bool zz_sound::set_velocity (const vec3& velocity_world_in)
{
	// get free buffer
	pDS3DBuffer = this->get_free_buffer();

	if (!pDS3DBuffer) return false;

	vec3 velocity_sound;
	zz_manager_sound::convert_to_sound_space(velocity_sound, velocity_world_in);

	return !FAILED(pDS3DBuffer->SetVelocity(velocity_sound.x, velocity_sound.y, velocity_sound.z, DS3D_DEFERRED)); // DS3D_IMMEDIATE); // DS3D_DEFERRED);	
}

bool zz_sound::set_distance_min (float min_in)
{
	// get free buffer
	pDS3DBuffer = this->get_free_buffer();

	if (!pDS3DBuffer) return false;

	return !FAILED(pDS3DBuffer->SetMinDistance(min_in, DS3D_DEFERRED));
}

bool zz_sound::set_distance_max (float max_in)
{
	// get free buffer
	pDS3DBuffer = this->get_free_buffer();

	if (!pDS3DBuffer) return false;

	return !FAILED(pDS3DBuffer->SetMinDistance(max_in, DS3D_DEFERRED));
}

#endif // ZZ_USE_SOUND
