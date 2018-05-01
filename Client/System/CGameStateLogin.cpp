#include "stdafx.h"
#include "cgamestatelogin.h"
#include "CGame.h"

#include "../Game.h"
#include "../TGameCtrl/TMsgBox.h"
#include "../Network/CNetwork.h"

#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/CTFontImpl.h"
#include "../Interface/IO_ImageRes.h"
#include "../interface/CUIMediator.h"
#include "../Interface/IT_MGR.H"
#include "../Interface/Command/CTCmdHotExec.h"
#include "../Interface/Cursor/CCursor.h"

#include "../Common/IO_Quest.h"

#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "../CSkyDOME.H"
//#include "../nProtect/nProtect.h"
#include "../Misc/GameUtil.h"
#include "TIme2.h"
#include "SystemProcScript.h"



CGameStateLogin::CGameStateLogin(int iID)
{
	m_iStateID = iID;
}

CGameStateLogin::~CGameStateLogin(void)
{	
}


int	CGameStateLogin::Update( bool bLostFocus )
{
	g_EUILobby.Update();
	
	g_pCamera->Update ();

	D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
	g_pTerrain->SetCenterPosition( PosENZIN.x, PosENZIN.y );

	g_DayNNightProc.ShowAllEffect( false );

	::updateScene ();

	// processing  ...
	if ( !bLostFocus ) 
	{
		if ( !::beginScene() ) //  디바이스가 손실된 상태라면 0을 리턴하므로, 모든 렌더링 스킵
		{
			return 0;
		}

		::clearScreen();
		::renderScene();
		
	
		::beginSprite( D3DXSPRITE_ALPHABLEND );

		/// Screen message display
		g_UIMed.Draw();

		/// UI display
		g_EUILobby.Draw();
		
		CTIme::GetInstance().Draw();
		
	
		::endSprite();		
	

		::endScene ();
		::swapBuffers();
	}
	return 0;
}

int CGameStateLogin::ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;

	return 0;
}

int CGameStateLogin::ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg ) 
	{
		case WM_CHAR :
			{
				switch ( wParam )
				{
					case '1':
						{
							//g_pCamera->SetMotion("Cameras\\camera01.zmo");
							//g_pCamera->Set_Position ( 520000.0f, 520000.0f, 0.0f );
						}
						break;
					case '2':
						break;
				}
			}
			break;
	}

	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;	

	return 0;
}

int CGameStateLogin::Enter( int iPrevStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "EnterLogin", ZZ_PARAM_INT, (int)g_GameDATA.GetTime(),ZZ_PARAM_END );

	g_EUILobby.InitEUIManager();
	g_EUILobby.CreateLoginDlg();
	
//	CTCommand* pCmd = new CTCmdExit;
//	g_pTerrain->SetLoadingMode( false );
//	::setDelayedLoad( 2 );	


	int iOldSightRange = g_GameDATA.m_nSightRange;
	g_GameDATA.m_nSightRange = 50;

	g_pCamera->Update ();
	D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
	g_pTerrain->InitZONE( PosENZIN.x, PosENZIN.y );
	g_pTerrain->SetCenterPosition (PosENZIN.x, PosENZIN.y);	
	

	/// Init skybox
	CSkyDOME::Init( g_GameDATA.m_hShader_sky, g_GameDATA.m_hLight, 0 );
	
	
	//::setDelayedLoad( 1 );

	g_GameDATA.m_nSightRange = iOldSightRange;

	if( g_GameDATA.m_is_NHN_JAPAN )
	{
		CTDialog* pDlg =  g_EUILobby.GetEUI( EUI_LOGIN );
		CLogin* pLoginDlg = ( CLogin*)pDlg;
		pLoginDlg->SetID( g_GameDATA.m_Account.Get() );
		///Decoding후 넣는다.
		//pLoginDlg->SetPassword( g_GameDATA.m_Password.Get() );
		pLoginDlg->ConnectLoginServer();
		pLoginDlg->Hide();
	}
	/// exec Enter script



	return 0;
}

int CGameStateLogin::Leave( int iNextStateID )
{
	/// exec Leave script
	CSystemProcScript::GetSingleton().CallLuaFunction( "LeaveLogin", ZZ_PARAM_END );
	//m_npKeyCrypt.CloseHandle();
	return 0;
}

void CGameStateLogin::ServerDisconnected()
{
	//g_EUILobby.ServerDisconnected();
}

void CGameStateLogin::AcceptedConnectLoginSvr()
{
	CGame::GetInstance().ProcWndMsg(WM_USER_CLOSE_MSGBOX,0,0 ); 
	///접속되었다 ID와 패스워드를 날리자.

	CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) ;
	assert( pDlg );
	if( pDlg )
	{
		CLogin*	pLoginDlg = (CLogin*)pDlg;
		pLoginDlg->SendLoginReq();
	}
}