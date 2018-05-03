#include "stdafx.h"
#include ".\directmusicplayer.h"
#include "../CApplication.h"

CDirectMusicPlayer::CDirectMusicPlayer(void)
{
}

CDirectMusicPlayer::~CDirectMusicPlayer(void)
{
}

bool CDirectMusicPlayer::Init()
{
	if( FAILED( m_MMPlayer.Init ( g_pCApp->GetHWND(), WM_MUSIC_EVENT ) ) )
	{
		MessageBox( g_pCApp->GetHWND(), "사운드 카드에 문제가 있습니다.", "ERROR", MB_OK );
		return false;
	}

	return true;
}

bool CDirectMusicPlayer::Play( const char* fName )
{
	Stop();

	if( FAILED( m_MMPlayer.Play( (char*)fName ) ) )
		return false;

	return true;
}

void CDirectMusicPlayer::Stop()
{
	m_MMPlayer.Stop();
}


void CDirectMusicPlayer::Run()
{
	m_MMPlayer.Run();
}

void CDirectMusicPlayer::Pause()
{
	m_MMPlayer.Pause();
}

void CDirectMusicPlayer::HandleEvent()
{
	m_MMPlayer.HandleEvent();
}


/// ??? 뭔가 이상하다...
void CDirectMusicPlayer::SetVolume( long lVolume )
{
	HRESULT hr = S_OK;
	// 할때마다 하는건 무식하지만.. 적당히 넣을곳이 없다.. 현재로는..
	if (FAILED(hr = m_MMPlayer.m_pGraphBuilder->QueryInterface(IID_IBasicAudio,   (void **)&m_pBA)))
		return;

	m_pBA->put_Volume( lVolume );

	SAFE_RELEASE(m_pBA);	
}