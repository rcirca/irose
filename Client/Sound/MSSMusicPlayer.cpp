#include "stdafx.h"
#include ".\mssmusicplayer.h"

#ifdef _USE_MSS
#pragma comment( lib , "mss32.lib")
#endif

CMSSMusicPlayer::CMSSMusicPlayer(void)
{
#ifdef _USE_MSS

	m_hDig			= 0;
	m_hStream		= 0;
	
	m_Paused		= 0;

#endif
}

CMSSMusicPlayer::~CMSSMusicPlayer(void)
{
#ifdef _USE_MSS
	//
	// Shut down driver, digital services, and process services
	//

	AIL_close_digital_driver( m_hDig );

	AIL_shutdown();
#endif
}

void CMSSMusicPlayer::HandleEvent()
{
#ifdef _USE_MSS
#endif
}

bool CMSSMusicPlayer::Init()
{
#ifdef _USE_MSS
	//
	// set the redist directory and statup the system
	//

	AIL_set_redist_directory( MSS_DIR_UP_TWO "redist" );

	AIL_startup();

	//
	// open a digital driver
	//

	m_hDig = AIL_open_digital_driver( 44100, 16, 2, 0 );

	if( m_hDig == 0 )
	{
		assert( 0 && "open_digital_driver" );
		return false;
	}
#endif

    return true;
}

bool CMSSMusicPlayer::Play( const char* fName )
{
#ifdef _USE_MSS
	Stop();
	//
	// open the stream handle
	//

	m_hStream = AIL_open_stream( m_hDig, fName, 0 );
	if( m_hStream == 0 )
	{
		assert( 0 && "AIL_open_stream error" );
		return false;
	}

	//
	// loop the stream forever
	//

	AIL_set_stream_loop_count( m_hStream, 0 );

	AIL_start_stream( m_hStream );
#endif

	return true;
}

void CMSSMusicPlayer::Stop()
{
#ifdef _USE_MSS
	//
	// Clean up
	//

	AIL_close_stream( m_hStream );	
#endif
}

void CMSSMusicPlayer::Run()
{
#ifdef _USE_MSS
	AIL_pause_stream( m_hStream, 0 );
#endif
}

void CMSSMusicPlayer::Pause()
{
#ifdef _USE_MSS
	AIL_pause_stream( m_hStream, 1 );
#endif
}

void CMSSMusicPlayer::SetVolume( long lVolume )
{
#ifdef _USE_MSS
	/// DS 의 볼륨단위를 MSS 단위로변환 ( -10000 ~ 0 => 0.0f ~ 1.0f )
	AIL_set_stream_volume_pan( m_hStream, (float)( lVolume + 10000 ) / 10000.0f, 0.5 );
#endif
}