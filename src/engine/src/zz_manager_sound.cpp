/** 
 * @file zz_manager_sound.cpp
 * @brief 3d sound manager.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-jan-2004
 *
 * $Header: /engine/src/zz_manager_sound.cpp 5     04-06-24 2:12p Zho $
 * $History: zz_manager_sound.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-06-24   Time: 2:12p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-28   Time: 2:15p
 * Updated in $/engine/src
 * more bug-findable version.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-07   Time: 10:07a
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
#include "zz_manager_sound.h"
#include "zz_log.h"
#include "zz_system.h"
#include "zz_view.h"
#include "zz_camera.h"

#undef STRICT
#include "dsutil.cpp"

//comctl32.lib dxerr9.lib winmm.lib dsound.lib dxguid.lib odbc32.lib odbccp32.lib
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dxerr9.lib")
#pragma comment (lib, "dsound.lib")

ZZ_IMPLEMENT_DYNCREATE(zz_manager_sound, zz_manager)

CSoundManager * g_pSoundManager = NULL;
CSound* g_pSound = NULL;
LPDIRECTSOUND3DBUFFER g_pDS3DBuffer = NULL;   // 3D sound buffer
LPDIRECTSOUND3DLISTENER g_pDSListener  = NULL;   // 3D listener object

zz_manager_sound::zz_manager_sound ()
	//ds_(NULL),
	//listener_(NULL)
{
}

zz_manager_sound::~zz_manager_sound ()
{
	SAFE_RELEASE( g_pDSListener );
	SAFE_DELETE( g_pSoundManager );
}

//
//// from dsutil.cpp
////-----------------------------------------------------------------------------
//// Name: CSoundManager::SetPrimaryBufferFormat()
//// Desc: Set primary buffer to a specified format 
////       !WARNING! - Setting the primary buffer format and then using this
////                   same dsound object for DirectMusic messes up DirectMusic! 
////       For example, to set the primary buffer format to 22kHz stereo, 16-bit
////       then:   dwPrimaryChannels = 2
////               dwPrimaryFreq     = 22050, 
////               dwPrimaryBitRate  = 16
////-----------------------------------------------------------------------------
//bool zz_manager_sound::set_primary_buffer_format ( DWORD dwPrimaryChannels, 
//                                               DWORD dwPrimaryFreq, 
//                                               DWORD dwPrimaryBitRate )
//{
//    HRESULT hr;
//    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;
//
//    if( ds_ == NULL )
//        return false; // CO_E_NOTINITIALIZED
//
//    // Get the primary buffer 
//    DSBUFFERDESC dsbd;
//    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
//    dsbd.dwSize = sizeof(DSBUFFERDESC);
//    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
//    dsbd.dwBufferBytes = 0;
//    dsbd.lpwfxFormat   = NULL;
//       
//	if( FAILED( hr = ds_->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) ) {
//		ZZ_LOG("manager_sound: set_primary_buffer_format() failed. CreateSoundBuffer() failed.\n");
//		return false;
//	}
//
//    WAVEFORMATEX wfx;
//    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
//    wfx.wFormatTag = (WORD) WAVE_FORMAT_PCM; 
//    wfx.nChannels = (WORD) dwPrimaryChannels; 
//    wfx.nSamplesPerSec = (DWORD) dwPrimaryFreq; 
//    wfx.wBitsPerSample = (WORD) dwPrimaryBitRate; 
//    wfx.nBlockAlign = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
//    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);
//
//	if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) ) {
//		ZZ_LOG("managre_sound: set_primary_buffer_format() failed. SetFormat() failed.\n");
//		return false;
//	}
//
//    SAFE_RELEASE( pDSBPrimary );
//
//    return true;
//}
//
//// from dsutil.cpp
////-----------------------------------------------------------------------------
//// Name: CSoundManager::Get3DListenerInterface()
//// Desc: Returns the 3D listener interface associated with primary buffer.
////-----------------------------------------------------------------------------
//bool zz_manager_sound::get_3d_listener_interface ( LPDIRECTSOUND3DLISTENER* ppDSListener )
//{
//    HRESULT             hr;
//    DSBUFFERDESC        dsbdesc;
//    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;
//
//    if( ppDSListener == NULL )
//        return false; // E_INVALIDARG;
//
//    if( ds_ == NULL )
//        return false; // CO_E_NOTINITIALIZED;
//
//    *ppDSListener = NULL;
//
//    // Obtain primary buffer, asking it for 3D control
//    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
//    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
//    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
//	if( FAILED( hr = ds_->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) ) {
//		ZZ_LOG("manager_sound: get_3d_listner_interface() failed. CreateSoundBuffer() failed.\n");
//		return false;
//	}
//        
//    if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, 
//                                                  (VOID**)ppDSListener ) ) )
//    {
//        SAFE_RELEASE( pDSBPrimary );
//		ZZ_LOG("manager_sound: get_3d_listner_interface() failed. QueryInterface() failed.\n");
//        return false;
//    }
//
//    // Release the primary buffer, since it is not need anymore
//    SAFE_RELEASE( pDSBPrimary );
//
//    return true;
//}
//
//
//// from dsutil.cpp
//bool zz_manager_sound::create_sound ( zz_sound * sound, const char * strWaveFileName )
//{
//	DWORD dwCreationFlags = 0;
//	GUID guid3DAlgorithm = GUID_NULL;
//	HRESULT hr;
//    HRESULT hrRet = S_OK;
//    DWORD i;
//    LPDIRECTSOUNDBUFFER*apDSBuffer = NULL;
//    DWORD dwDSBufferSize = NULL;
//    CWaveFile *pWaveFile = NULL;
//
//    if( ds_ == NULL )
//        return false; // CO_E_NOTINITIALIZED;
//    if( strWaveFileName == NULL || sound == NULL )
//        return false; // E_INVALIDARG;
//
//    apDSBuffer = new LPDIRECTSOUNDBUFFER;
//
//    if( apDSBuffer == NULL )
//    {
//        hr = E_OUTOFMEMORY;
//        goto LFail;
//    }
//
//    pWaveFile = new CWaveFile();
//    if( pWaveFile == NULL )
//    {
//		ZZ_LOG("manager_sound: create_sound() failed. out of memory\n");
//        goto LFail;
//    }
//
//    pWaveFile->Open( strWaveFileName, NULL, WAVEFILE_READ );
//
//    if( pWaveFile->GetSize() == 0 )
//    {
//        // Wave is blank, so don't create it.
//        ZZ_LOG("manager_sound: create_sound() failed. wave file is blank.\n");
//        goto LFail;
//    }
//
//    // Make the DirectSound buffer the same size as the wav file
//    dwDSBufferSize = pWaveFile->GetSize();
//
//    // Create the direct sound buffer, and only request the flags needed
//    // since each requires some overhead and limits if the buffer can 
//    // be hardware accelerated
//    DSBUFFERDESC dsbd;
//    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
//    dsbd.dwSize = sizeof(DSBUFFERDESC);
//    dsbd.dwFlags = dwCreationFlags;
//    dsbd.dwBufferBytes = dwDSBufferSize;
//    dsbd.guid3DAlgorithm = guid3DAlgorithm;
//    dsbd.lpwfxFormat = pWaveFile->m_pwfx;
//    
//    // DirectSound is only guarenteed to play PCM data.  Other
//    // formats may or may not work depending the sound card driver.
//    hr = ds_->CreateSoundBuffer( &dsbd, &apDSBuffer, NULL );
//
//    // Be sure to return this error code if it occurs so the
//    // callers knows this happened.
//    if( hr == DS_NO_VIRTUALIZATION )
//        hrRet = DS_NO_VIRTUALIZATION;
//            
//    if( FAILED(hr) )
//    {
//        // DSERR_BUFFERTOOSMALL will be returned if the buffer is
//        // less than DSBSIZE_FX_MIN and the buffer is created
//        // with DSBCAPS_CTRLFX.
//        
//        // It might also fail if hardware buffer mixing was requested
//        // on a device that doesn't support it.
//        ZZ_LOG("manager_sound: create_sound() failed. CreateSoundBuffer() failed.\n");
//		            
//        goto LFail;
//    }
//
//    // Create the sound
//    sound->set_property( apDSBuffer, dwDSBufferSize, dwNumBuffers, pWaveFile, dwCreationFlags );
//    
//    SAFE_DELETE_ARRAY( apDSBuffer );
//    return true;
//
//LFail:
//    // Cleanup
//    SAFE_DELETE( pWaveFile );
//    SAFE_DELETE_ARRAY( apDSBuffer );
//    return false;
//}
//
//bool zz_manager_sound::initialize ()
//{
//	HRESULT hr;
//    // Create IDirectSound using the primary sound device
//	if( FAILED( hr = DirectSoundCreate8( NULL, &ds_, NULL ) ) ) {
//		ZZ_LOG("manager_sound: initialize() failed. DirectSoundCreate8() failed.\n");
//		return false;
//	}
//
//    // Set DirectSound coop level 
//	HWND hwnd = reinterpret_cast<HWND>(znzin->view->get_handle());
//	if( FAILED( hr = ds_->SetCooperativeLevel( hwnd, DSSCL_PRIORITY ) ) ) {
//		ZZ_LOG("manager_sound: initialize() failed. SetCooperativeLevel() failed.\n");
//		return false;
//	}
//   
//	if (!set_primary_buffer_format( 2, 22050, 16 )) {
//		return false;
//	}
//    
//    // Get the 3D listener, so we can control its params
//	if (!get_3d_listener_interface( &listener_ )) {
//		return false;
//	}
//
//	return true;
//}

bool zz_manager_sound::initialize ()
{
    // Create a static IDirectSound in the CSound class.  
    // Set coop level to DSSCL_PRIORITY, and set primary buffer 
    // format to stereo, 22kHz and 16-bit output.
	if (!g_pSoundManager) {
		g_pSoundManager = new CSoundManager();
	}
    if( NULL == g_pSoundManager )
    {
        DXTRACE_ERR_MSGBOX( TEXT("Initialize"), E_OUTOFMEMORY );
        return false;
    }

	HWND hwnd = reinterpret_cast<HWND>(znzin->view->get_handle());
	if (hwnd == NULL) {
		ZZ_LOG("manager_sound: initialize() failed. window handle was not assigned yet.\n");
		return false;
	}
    HRESULT hr = g_pSoundManager->Initialize( hwnd, DSSCL_PRIORITY );
       
    //hr |= g_pSoundManager->SetPrimaryBufferFormat( 2, 22050, 16 );
	hr |= g_pSoundManager->SetPrimaryBufferFormat( 1, 11025, 16 );
    
    // Get the 3D listener, so we can control its params
    hr |= g_pSoundManager->Get3DListenerInterface( &g_pDSListener );

    if( FAILED(hr) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("Get3DListenerInterface"), hr );
        return false;
    }

	// Set distance factor 
	float distance_factor = 0.01f; // znzin space(cm) => dsound space(meter)

	if (FAILED(g_pDSListener->SetDistanceFactor(distance_factor, DS3D_DEFERRED))) {
		ZZ_LOG("manager_sound: initialize() failed. SetDistanceFactor(%f) failed.\n", distance_factor);
		return false;
	}
	
	return true;
}

void zz_manager_sound::set_listener_position (const vec3& position)
{
	if (!g_pDSListener) return;

	vec3 pos_sound;
	convert_to_sound_space(pos_sound, position);
	g_pDSListener->SetPosition(pos_sound.x, pos_sound.y, pos_sound.z, DS3D_DEFERRED);
}

void zz_manager_sound::set_listener_orientation (const vec3& front, const vec3& top)
{
	if (!g_pDSListener) return;
	
	g_pDSListener->SetOrientation(front.x, front.z, front.y, top.x, top.z, top.y, DS3D_DEFERRED);
}

bool zz_manager_sound::update_listener ()
{
	if ( !g_pDSListener ) return false;

	// get system default camera
	zz_camera * camera = znzin->get_camera();
	assert(camera);

	// listener setting by camera
	vec3 front, top, right;
	front = camera->get_dir();
	right = camera->get_right();
	cross(top, right, front);

	this->set_listener_position(camera->get_eye());
	this->set_listener_orientation(front, top);
	g_pDSListener->CommitDeferredSettings();

	return true;
}

#endif ZZ_USE_SOUND