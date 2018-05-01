#include "stdafx.h"
#include "CGameStateWarp.h"
#include <process.h>
#include "../Object.h"
#include "../CCamera.h"
#include "../CClientStorage.h"
#include "../IO_Terrain.h"
#include "../CSkyDOME.h"
#include "../Network/CNetwork.h"
#include "../interface/ExternalUI/ExternalUILobby.h"
#include "../interface/CLoading.h"
#include "../interface/it_mgr.h"
#include "../interface/CUIMediator.h"
#include "../interface/Dlgs/CMinimapDlg.h"
#include "../Country.h"

#include "CGame.h"
#include <crtdbg.h>

static bool tDone = false;
CGameStateWarp::CGameStateWarp(int iID)
{
	m_iStateID = iID;
}

CGameStateWarp::~CGameStateWarp(void)
{
}

int	CGameStateWarp::Update( bool bLostFocus )
{
#ifdef __THREADED_LOADING
	//Draw();
	DWORD ret = WaitForSingleObject( m_hThread, 100 );

	switch( ret )
	{
	case WAIT_OBJECT_0:
		if(tDone)
			CGame::GetInstance().ChangeState( CGame::GS_MAIN );
		break;
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		g_pCApp->SetExitGame();
		break;
	default:
		break;
	}
#endif
	return 0;
}

int CGameStateWarp::Enter( int iPrevStateID )
{
	pauseSpriteSFX( true );

	m_iPrevStateID = iPrevStateID;

	LogString( LOG_NORMAL, "FreeZone(%d)\n", g_pTerrain->GetZoneNO() );

	int iPrevZoneNum = g_pTerrain->GetZoneNO();
	int iNextZoneNum = 0;
	g_pTerrain->FreeZONE();

#ifdef __THREADED_LOADING
	gsv_TELEPORT_REPLY Reply;
	CGame::GetInstance().GetLoadingData( Reply );
	iNextZoneNum = Reply.m_nZoneNO;
	g_Loading.LoadTexture( Reply.m_nZoneNO, ZONE_PLANET_NO( Reply.m_nZoneNO ) );
	Draw();

	m_hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc, NULL, CREATE_SUSPENDED, NULL );
	SetThreadPriority( m_hThread,THREAD_PRIORITY_HIGHEST  );

	if( m_hThread )
	{
		tDone = false;
		ResumeThread( m_hThread );
	}
	else///Thread 생성 실패시 메인쓰레드에서 로딩하고 State를 바꾸어 준다.
#endif
	{
#ifndef __THREADED_LOADING
		gsv_TELEPORT_REPLY Reply;
		CGame::GetInstance().GetLoadingData( Reply );

		iNextZoneNum = Reply.m_nZoneNO;

		g_Loading.LoadTexture( Reply.m_nZoneNO, ZONE_PLANET_NO( Reply.m_nZoneNO ) );
		Draw();
#endif
		ThreadFunc(NULL);
		CGame::GetInstance().ChangeState( CGame::GS_MAIN );
	}

	if( CCountry::GetSingleton().IsApplyNewVersion() && g_pTerrain )
	{
		//int iZoneNum = TELEPORT_ZONE(nWarpIDX);
		///야외 필드에서 던전으로 입장시 부활존이 지정되어 있다면
		if( (0 == ZONE_IS_UNDERGROUND( iPrevZoneNum )) && ZONE_IS_UNDERGROUND(iNextZoneNum) && ZONE_REVIVE_ZONENO( iNextZoneNum ))
		{
			g_itMGR.AppendChatMsg( STR_CHANGED_STOREDTOWN_POS, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136)  );
		}
		///부활지점이 데이타로 정의된 던젼에서 필드로 이동시
		else if( ZONE_IS_UNDERGROUND( iPrevZoneNum ) && ZONE_REVIVE_ZONENO( iPrevZoneNum ) && (0 == ZONE_IS_UNDERGROUND( iNextZoneNum )) )
		{
			g_itMGR.AppendChatMsg( STR_RECOVERD_STOREDTOWN_POS, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136)   );
		}
	}

	return 0;
}
int CGameStateWarp::Leave( int iNextStateID )
{
	g_Loading.UnloadTexture();
	gsv_TELEPORT_REPLY Reply;
	CGame::GetInstance().GetLoadingData( Reply );

	g_pAVATAR->Warp_POSITION( Reply.m_PosWARP.x, Reply.m_PosWARP.y );

	g_pCamera->Attach ( g_pAVATAR->GetZMODEL() );	
	g_pCamera->Update ();
	/// Loading mode change	
//	g_pTerrain->SetLoadingMode( true );
//	::setDelayedLoad( true );

	CSkyDOME::Init( g_GameDATA.m_hShader_sky, g_GameDATA.m_hLight, ZONE_BG_IMAGE( g_pTerrain->GetZoneNO() ) );

	/// PAT Setting
	g_pAVATAR->m_bRunMODE	= Reply.m_btRunMODE;
	g_pAVATAR->m_btMoveMODE = Reply.m_btRideMODE;


//-------------------------------------------------------------------------------------------------
//박지호::2인승일때 자신의 카트를 생성하지 않도록 한다. 
#if defined(_GBC)	
	if( g_pAVATAR->m_btMoveMODE == MOVE_MODE_DRIVE)
	{
#else if 
	if( g_pAVATAR->m_btMoveMODE == MOVE_MODE_DRIVE || g_pAVATAR->m_btMoveMODE == MOVE_MODE_RIDEON )
	{
#endif 
//-------------------------------------------------------------------------------------------------
		g_pAVATAR->CreateCartFromMyData();
		g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState( true );
	}
	else
	{
		g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState( false );
	}

    pauseSpriteSFX( false );




	///카트, 캐슬기어 이용불가 지역에 카트, 캐슬기어 탑승하고 입장시 강제로 내리게 한다.
	if( CCountry::GetSingleton().IsApplyNewVersion() && g_pAVATAR->GetPetMode() >= 0 )
	{
		int iItemType = g_pAVATAR->m_Inventory.m_ItemRIDE[ 0 ].GetTYPE();
		int iItemNo   = g_pAVATAR->m_Inventory.m_ItemRIDE[ 0 ].GetItemNO();

		switch( ZONE_RIDING_REFUSE_FLAG( g_pTerrain->GetZoneNO() ) )
		{
		case 1:
			if( (ITEM_TYPE( iItemType, iItemNo ) / 100 == 5) && (ITEM_TYPE( iItemType, iItemNo ) % 10 == 1 ) )
			{
				g_pAVATAR->SetCMD_TOGGLE( Reply.m_btRideMODE + Reply.m_btRunMODE );				
				g_itMGR.AppendChatMsg( STR_CANT_USE_CART , IT_MGR::CHAT_TYPE_SYSTEM , D3DCOLOR_ARGB( 255, 206, 223, 136));				
				return false;
			}
			break;
		case 2:
			if( (ITEM_TYPE( iItemType, iItemNo ) / 100 == 5) && (ITEM_TYPE( iItemType, iItemNo ) % 10 == 2 ) )
			{
				g_pAVATAR->SetCMD_TOGGLE( Reply.m_btRideMODE + Reply.m_btRunMODE);
				g_itMGR.AppendChatMsg( STR_CANT_USE_CASTLEGEAR , IT_MGR::CHAT_TYPE_SYSTEM , D3DCOLOR_ARGB( 255, 206, 223, 136));				
				return false;
			}
			break;
		case 3:
			g_pAVATAR->SetCMD_TOGGLE( Reply.m_btRideMODE + Reply.m_btRunMODE);
			g_itMGR.AppendChatMsg(STR_CANT_USE_DRIVESKILL, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
			break;
		default:
			break;
		}
		g_pAVATAR->UpdateAbility();///이동속도 재계산
	}
	return 0;
}

void CGameStateWarp::Draw()///Test Code
{
	if( g_pCApp->IsActive() )
	{
		if ( !::beginScene() ) //  디바이스가 손실된 상태라면 0을 리턴하므로, 모든 렌더링 스킵
		{
			return;
		}
		
		::clearScreen();

		::beginSprite( D3DXSPRITE_ALPHABLEND );	

		g_Loading.Draw();

		::endSprite();

		::endScene();
		::swapBuffers();
	}
}

unsigned __stdcall CGameStateWarp::ThreadFunc( void* pArguments )
{
	gsv_TELEPORT_REPLY Reply;
	CGame::GetInstance().GetLoadingData( Reply );

//	::setDelayedLoad( 2 );
	
	::setDelayedLoad( 0 );

	LogString( LOG_NORMAL, "LoadZone(%d)\n", Reply.m_nZoneNO  );
	g_pTerrain->LoadZONE( Reply.m_nZoneNO );
	g_pTerrain->InitZONE( Reply.m_PosWARP.x, Reply.m_PosWARP.y );

	/// 개인상점 리스트 클리어
	g_UIMed.ResetPersonalStore();

	CMinimapDLG* pDlg = g_itMGR.GetMinimapDLG();
	
	if( pDlg )
		pDlg->SetMinimap( ZONE_MINIMAP_NAME( Reply.m_nZoneNO ), "3DData\\Control\\Res\\MiniMap_Arrow.tga", 
							ZONE_MINIMAP_STARTX( Reply.m_nZoneNO ), ZONE_MINIMAP_STARTY( Reply.m_nZoneNO ) );

	::setDelayedLoad( 2 );
	::setDelayedLoad( 1 );

	return 0;
}