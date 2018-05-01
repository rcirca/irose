#include "stdafx.h"
#include "cgamestateexitmain.h"
#include <process.h>
#include "CGame.h"
#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "../interface/CLoading.h"
#include "../interface/dlgs/CItemDlg.h"
#include "../Network/CNetwork.h"
#include "System_FUNC.h"
#include "SystemProcScript.h"
#include "../CSkyDOME.h"
#include "../gamedata/cprivatestore.h"
#include "../gamedata/cclan.h"
#include "../gamedata/cparty.h"
#include "../interface/it_mgr.h"
#include "../interface/ClanMarkManager.h"
#include "../interface/dlgs/QuickToolBAR.h"
#include "../interface/dlgs/CCommDlg.h"
#include "../CClientStorage.h"


int CGameStateExitMain::m_iBackGroundZone  = 1;

CGameStateExitMain::CGameStateExitMain( int iID )
{
	m_iStateID  = iID;
	m_iBackGroundZone = 1;
}

CGameStateExitMain::~CGameStateExitMain(void)
{

}


int	CGameStateExitMain::Update( bool bLostFocus )
{	
	return 0;
}

int CGameStateExitMain::Enter( int iPrevStateID )
{ 
	/// 배경으로 사용할 존 번호를 얻어온다.
	m_iBackGroundZone = SC_GetBGZoneNO();

//	m_hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc, NULL, CREATE_SUSPENDED, NULL );
////	SetThreadPriority( m_hThread,THREAD_PRIORITY_HIGHEST  );
//
//	if( m_hThread )
//	{
//		ResumeThread( m_hThread );
//	}
//	else///Thread 생성 실패시 메인쓰레드에서 로딩하고 State를 바꾸어 준다.
	{
		g_pCamera->Detach();
		g_pTerrain->FreeZONE();
		g_Loading.LoadTexture( m_iBackGroundZone, ZONE_PLANET_NO( m_iBackGroundZone ) );

		Draw();

		/// Clear 시 퀵슬롯에 있는 Data 를 먼저 삭제해주어야 한다.
		/// Item이나 스킬이 지워질때 저장되어 있던 아이템(스킬)등을 자동으로
		/// 서버에 패킷을 보내서 뺀다. 2004/8/20
		CClan::GetInstance().Clear();
		CPrivateStore::GetInstance().Clear();
		CParty::GetInstance().Leave();

	


		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY ) )
		{
			CCommDlg* pCommDlg = (CCommDlg*)pDlg;
			pCommDlg->ClearFriendList();
			pCommDlg->ClearMemoList();
			pCommDlg->ClearMemoList();
		}


		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_ITEM ) )
		{
			CItemDlg* pItemDlg = (CItemDlg*)pDlg;
			std::list<S_InventoryData> list;
			pItemDlg->GetVirtualInventory( list );

			///CClientDB::GetInstance().SetInventoryData( g_pAVATAR->Get_NAME(), list );
			g_ClientStorage.SetInventoryData( g_pAVATAR->Get_NAME(), list );
		}

		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUICKBAR ) )
		{
			CQuickBAR* pQuickbarDlg = (CQuickBAR*)pDlg;
			pQuickbarDlg->Clear();
		}

#ifdef _NEW_UI //2nd SkillBar
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUICKBAR_EXT ) )
		{
			CQuickBAR* pQuickbarDlg = (CQuickBAR*)pDlg;
			pQuickbarDlg->Clear();
		}	
#endif

		CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
		pItemSlot->Clear();

		CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
		pSkillSlot->ClearSlot();

		CClanSkillSlot* pClanSkillSlot = CClan::GetInstance().GetClanSkillSlot();
		pClanSkillSlot->ClearSlot();

		g_pAVATAR->ResetClan();
		g_pObjMGR->Del_Object( g_pAVATAR );
		
		g_pAVATAR = NULL;
 

		CClanMarkManager::GetSingleton().Free();
		ThreadFunc(NULL);
		
		g_pNet->Send_cli_CHAR_LIST();


		g_itMGR.CloseDialogAllExceptDefaultView();
		g_itMGR.ClearNotifyButtons();



		SYSTEMTIME time;
		ZeroMemory( &time, sizeof( SYSTEMTIME ));
		CClan::GetInstance().SetClanMarkRegTime( time );
	}
	
	return 0; 
}

int CGameStateExitMain::Leave( int iNextStateID )
{ 
	g_Loading.UnloadTexture();

	::setDelayedLoad( 0 );
	g_pTerrain->LoadZONE( CGameStateExitMain::m_iBackGroundZone );
	///
	/// 카메라 모션은 32_32 기준으로 만들어졌다.. 모션적용을 위해서 보정한다.
	///
	D3DVECTOR PosENZIN;
	PosENZIN.x = 520000.0f;
	PosENZIN.y = 520000.0f;
	PosENZIN.z = 0.0f;

	g_pTerrain->InitZONE( PosENZIN.x, PosENZIN.y );

	CSkyDOME::Init( g_GameDATA.m_hShader_sky, g_GameDATA.m_hLight, 0 );

	HNODE camera_motion = loadMotion( "camera_motion", "3DData\\Title\\Camera01_inSelect01.zmo", 1, 4, 3, 1, 0 );
	HNODE motion_camera = loadCamera( "motion_camera", "cameras/camera01.zca", camera_motion );

	g_pCamera->Init( motion_camera );
	g_pCamera->Set_Position( PosENZIN );

	controlAnimatable( motion_camera, 1 );
	setRepeatCount( motion_camera, 1 );

	g_DayNNightProc.ShowAllEffect( false );

	::setDelayedLoad( 2 );	
	::setDelayedLoad( 0 );	
	return 0;
}

unsigned __stdcall CGameStateExitMain::ThreadFunc( void* pArguments )
{
//	::setDelayedLoad( 0 );

//	g_pTerrain->SetLoadingMode( true );


	return 0;
}

void CGameStateExitMain::Draw()
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