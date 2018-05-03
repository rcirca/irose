#include "stdafx.h"
#include ".\musicmgr.h"
#include "MusicPlayer.h"
#include "DirectMusicPlayer.h"
#include "OggMusicPlayer.h"
#include "MSSMusicPlayer.h"

/// for singleton
CMusicMgr _MusicMgr;


CMusicMgr::CMusicMgr(void)
{
	m_pPlayer		= NULL;
	m_bReadyDevice	= false;
}

CMusicMgr::~CMusicMgr(void)
{
}

bool CMusicMgr::Init( PLAYER_TYPE playerType )
{
	Clear();

	switch( playerType )
	{
		case DIRECT_MUSIC_TYPE:
			m_pPlayer = new CDirectMusicPlayer();
			break;
		case OGG_MUSIC_TYPE:
			m_pPlayer = new COggMusicPlayer();
			break;
		case MSS_MUSIC_TYPE:
			m_pPlayer = new CMSSMusicPlayer();
			break;
	}

	if( m_pPlayer->Init() == false )
	{
		m_bReadyDevice = false;
		return false;
	}

	m_bReadyDevice = true;
	return true;
}

void CMusicMgr::Clear()
{
	if( m_pPlayer )
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	m_bReadyDevice = false;
}

bool CMusicMgr::Play( const char* fName )
{
	if( m_bReadyDevice )
	{
		if( m_pPlayer )
		{
			return m_pPlayer->Play( fName );
		}
	}

	return false;
}

void CMusicMgr::Stop()
{
	if( m_bReadyDevice )
	{
		if( m_pPlayer )
		{
			m_pPlayer->Stop();
		}
	}
}

void CMusicMgr::Run()
{
	if( m_bReadyDevice )
	{
		if( m_pPlayer )
		{
			m_pPlayer->Run();
		}
	}
}


void CMusicMgr::Pause()
{
	if( m_bReadyDevice )
	{
		if( m_pPlayer )
		{
			m_pPlayer->Pause( );
		}
	}
}

void CMusicMgr::SetVolume( long lVolume )
{
	if( m_bReadyDevice )
	{
		if( m_pPlayer )
		{
			m_pPlayer->SetVolume( lVolume );
		}
	}
}

void CMusicMgr::HandleEvent()
{
	if( m_bReadyDevice )
	{
		if( m_pPlayer )
		{
			m_pPlayer->HandleEvent();
		}
	}
}