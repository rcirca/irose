#include "stdafx.h"
#include ".\oggmusicplayer.h"
#include "../Util/CFileSystem.h"
#include "Sound/MusicMgr.h"
#include "../CClientStorage.h"

COggMusicPlayer::COggMusicPlayer(void)
{
	m_pOggStream = NULL;
}

COggMusicPlayer::~COggMusicPlayer(void)
{
	Stop();
}

bool COggMusicPlayer::Init()
{
	return true;
}

bool COggMusicPlayer::Play( const char* fName )
{
	Stop();

	if ( (CVFSManager::GetSingleton()).IsExistFile( fName ) == false )	
	{
		return false;
	}

	if( g_pSoundLIST->m_SOUND.m_pDS == NULL )
	{
		return false;
	}

	m_pOggStream = new AOggSTREAM;
	if( m_pOggStream->OpenSoundStreamFile( (char*)fName, g_pSoundLIST->m_SOUND.m_pDS, 24, 4 ) == false )
	{
		assert( 0 && "m_pOggStream->OpenSoundStreamFile failed" );
		Stop();
		return false;
	}
	if( FAILED( m_pOggStream->PlayStreamBuffer( true, g_ClientStorage.GetBgmVolumeByIndex( g_ClientStorage.GetBgmVolumeIndex() ) ) 
 ) )
	{
		assert( 0 && "m_pOggStream->PlayStreamBuffer failed" );
		Stop();
		return false;
	}

	return true;
}

void COggMusicPlayer::Stop()
{
	if( m_pOggStream )
	{
		delete m_pOggStream;
		m_pOggStream = NULL;
	}
}

void COggMusicPlayer::Run()
{
	if( m_pOggStream )
	{
		//m_pOggStream->Run();
	}
}

void COggMusicPlayer::Pause()
{
	if( m_pOggStream )
	{
		//m_pOggStream->Pause();
	}
}

void COggMusicPlayer::SetVolume( long lVolume )
{
	if( m_pOggStream )
	{
		m_pOggStream->SetVolume( lVolume );
	}
}


void COggMusicPlayer::HandleEvent()
{

}