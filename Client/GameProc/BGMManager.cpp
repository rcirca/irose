#include "stdafx.h"
#include ".\bgmmanager.h"
#include "../Sound/MusicMgr.h"
#include "../Common/IO_STB.h"

#include "../CClientStorage.h"
#include "../Game.h"


CBGMManager _bgmManager;

CBGMManager::CBGMManager(void)
{
	m_iCurrentVolumeIdx			= 0;
	m_iStartVolumeIdx			= 0;
	m_bToFadeOut				= false;

	m_dwTransitionPrevTime		= g_GameDATA.GetGameTime();
	m_dwElapsedTime				= 0;

	m_bToNight					= false;	

	m_bEnableTransition			= false;
}

CBGMManager::~CBGMManager(void)
{
}

/// 밤낮 변경에 따른 배경음악 전이..
void CBGMManager::ChangeBGMMusicWithDayAndNight( int iZoneNO, bool bToNight )
{	
	/// 밤으로 변하는거라면..
	//if( bToNight )
	//{		
	//	CMusicMgr::GetSingleton().Play ( ZONE_BG_MUSIC_DAY( iZoneNO ) );
	//}else
	//{
	//	CMusicMgr::GetSingleton().Play ( ZONE_BG_MUSIC_NIGHT( iZoneNO ) );
	//}

	m_iZoneNO = iZoneNO;

	m_iCurrentVolumeIdx			= g_ClientStorage.GetBgmVolumeIndex();
	m_iStartVolumeIdx			= g_ClientStorage.GetBgmVolumeIndex();
	m_bToFadeOut				= true;

	m_dwTransitionPrevTime		= g_GameDATA.GetGameTime();
	m_dwElapsedTime				= 0;

	m_bToNight					= bToNight;	

	m_bEnableTransition			= true;
}

void CBGMManager::EndTransition()
{
	m_bEnableTransition			= false;
	CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex( m_iCurrentVolumeIdx ) );
}

void CBGMManager::Proc()
{
	if( !m_bEnableTransition )
		return;	

	DWORD dwCurrentTime			= g_GameDATA.GetGameTime();
	DWORD dwElapsedPrevFrame	= dwCurrentTime - m_dwTransitionPrevTime;

	m_dwTransitionPrevTime		= dwCurrentTime;

	m_dwElapsedTime += dwElapsedPrevFrame;

	/// 1초에 한번씩 갱싱

	if( m_dwElapsedTime > 1000 )
	{
		m_dwElapsedTime -= 1000;

		if( m_bToFadeOut )
		{
			m_iStartVolumeIdx -= 1;
			/// sound off
			if( m_iStartVolumeIdx < 0 )
			{
				m_bToFadeOut	= false;
				m_iStartVolumeIdx	= 0;

				/// 밤으로 변하는거라면..
				if( m_bToNight )
				{		
					CMusicMgr::GetSingleton().Play ( ZONE_BG_MUSIC_NIGHT( m_iZoneNO ) );
				}else
				{
					CMusicMgr::GetSingleton().Play ( ZONE_BG_MUSIC_DAY( m_iZoneNO ) );
				}
			}
		}else
		{
			m_iStartVolumeIdx += 1;
			/// sound off
			if( m_iStartVolumeIdx > m_iCurrentVolumeIdx )
			{
				m_bToFadeOut	= true;
				m_iStartVolumeIdx	= m_iCurrentVolumeIdx;

				m_bEnableTransition = false;
			}
		}

		CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex( m_iStartVolumeIdx ) );
	}	
}