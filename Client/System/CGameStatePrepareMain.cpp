#include "stdafx.h"
#include "CGameStatePrepareMain.h"
#include <process.h>
#include "../Object.h"
#include "../CCamera.h"
#include "../CClientStorage.h"
#include "../IO_Terrain.h"
#include "../CSkyDOME.h"
#include "../interface/ExternalUI/ExternalUILobby.h"
#include "../interface/CLoading.h"
#include "../interface/it_mgr.h"
#include "../interface/CUIMediator.h"
#include "../interface/Dlgs/CCommDlg.h"
#include "../interface/Dlgs/CMinimapDlg.h"
#include "../interface/Dlgs/CItemDlg.h"
#include "../interface/Cursor/CCursor.h"
#include "../GameData/CGameDataCreateAvatar.h"
#include "../GameData/CClan.h"
#include "../JCommandState.h"
#include "../CClientStorage.h"
#include "CGame.h"
#include "../Network/CNetwork.h"
#include <crtdbg.h>

#include "TControlMgr.h"

static bool tDone = false;
CGameStatePrepareMain::CGameStatePrepareMain(int iID)
{
	m_iStateID = iID;
}

CGameStatePrepareMain::~CGameStatePrepareMain(void)
{
}

int	CGameStatePrepareMain::Update( bool bLostFocus )
{
#ifdef __THREADED_LOADING
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

int CGameStatePrepareMain::Enter( int iPrevStateID )
{
	m_iPrevStateID = iPrevStateID;

	///외부메뉴에서 사용되었던 Data & Resource를 Unload한다.
	CGameDataCreateAvatar::GetInstance().Clear();
	LogString( LOG_NORMAL, "FreeZone(%d)\n", g_pTerrain->GetZoneNO() );
	g_pTerrain->FreeZONE();

	resetScreen();
	CCursor::GetInstance().ReloadCursor();
#ifdef __THREADED_LOADING
	gsv_TELEPORT_REPLY Reply;
	CGame::GetInstance().GetLoadingData( Reply );
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
		g_Loading.LoadTexture( Reply.m_nZoneNO, ZONE_PLANET_NO( Reply.m_nZoneNO ) );
		Draw();
#endif
		ThreadFunc(NULL);
		CGame::GetInstance().ChangeState( CGame::GS_MAIN );
	}
	return 0;
}

int CGameStatePrepareMain::Leave( int iNextStateID )
{
	g_Loading.UnloadTexture();
	gsv_TELEPORT_REPLY Reply;
	CGame::GetInstance().GetLoadingData( Reply );


	float fCenterZ = g_pTerrain->GetHeight( Reply.m_PosWARP.x, Reply.m_PosWARP.y ) + 150;	

	D3DVECTOR PosENZIN;

	PosENZIN.x = Reply.m_PosWARP.x;
	PosENZIN.y = Reply.m_PosWARP.y;
	PosENZIN.z = fCenterZ;
	
	g_pCamera->Set_Position ( PosENZIN.x, PosENZIN.y, PosENZIN.z );		
	g_pCamera->ResetToAvatarCamera();


	g_pCamera->Set_Position ( PosENZIN );

	g_pTerrain->SetCenterPosition ( PosENZIN.x, PosENZIN.y );		

	g_pObjMGR->Add_AvtCHAR( g_pAVATAR, PosENZIN, g_pAVATAR->Get_SEX() );		
	
	CSkyDOME::Init( g_GameDATA.m_hShader_sky, g_GameDATA.m_hLight, ZONE_BG_IMAGE( g_pTerrain->GetZoneNO() ) );
	

	g_pCamera->Attach ( g_pAVATAR->GetZMODEL() );	
	g_pCamera->Update ();
	
	/// Loading mode change	
//	g_pTerrain->SetLoadingMode( true );
//	::setDelayedLoad( true );


	///<--------------------------------------------------
	///사용자가 저장한 시야옵션을 재설정해준다.
	t_OptionVideo option;
	g_ClientStorage.GetVideoOption(option);
	g_ClientStorage.ApplyCameraOption( option.iCamera );

	/// 이펙트 옵션 설정
	g_pSoundLIST->SetVolume( g_ClientStorage.GetEffectVolumeByIndex( g_ClientStorage.GetEffectVolumeIndex()) );
	
	UINT uiControlType = g_ClientStorage.GetControlType();
	if( uiControlType )
		g_UserInputSystem.ChangeUserInputStyle( SEVENHEARTS_USER_INPUT_STYLE );
	else
		g_UserInputSystem.ChangeUserInputStyle( DEFAULT_USER_INPUT_STYLE );
	
	g_pAVATAR->UpdateInventory();

	CClan& Clan = CClan::GetInstance();
	
	g_pAVATAR->SetClan( Clan.GetClanNo(), Clan.GetMarkBack(), Clan.GetMarkCenter(), Clan.GetName() , Clan.GetLevel(), Clan.GetClass() );

	Clan.RequestMemberList();
	
	g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV(  g_pAVATAR->Get_LEVEL() , g_pAVATAR->Get_JOB());

	//it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_NORMAL );
	
	t_OptionKeyboard keyboard_option;

	g_ClientStorage.GetKeyboardOption( keyboard_option );
	if( keyboard_option.iChattingMode == 0 )
		it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_NORMAL );
	else
		it_SetKeyboardInputType( CTControlMgr::INPUTTYPE_AUTOENTER );


	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
		pDlg->Show();



	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY ) )
	{
		CCommDlg* pCommDlg = (CCommDlg*)pDlg;
		pCommDlg->ClearMemoList();
	}
	g_pNet->Send_cli_MEMO_CNT_REQ();

	///카트 탑승 상태에 따른 액티브 스킬 활성화/비활성화 Setting
	g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState( false );


	return 0;
}

void CGameStatePrepareMain::Draw()
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

unsigned __stdcall CGameStatePrepareMain::ThreadFunc( void* pArguments )
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
	
	

	
	std::list<S_InventoryData> Inventory;
	//CClientDB::GetInstance().GetInventoryData( g_pAVATAR->Get_NAME(), Inventory );
	g_ClientStorage.GetInventoryData( g_pAVATAR->Get_NAME(), Inventory );
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_ITEM ) )
	{
		CItemDlg* pItemDlg = (CItemDlg*)pDlg;
		pItemDlg->ApplySavedVirtualInventory( Inventory );
	}
	
	tDone = true;

	return 0;
}