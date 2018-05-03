#include "stdafx.h"
#include ".\cgamestateloginvirtual.h"
#include "../Network/CNetwork.h"
#include "../Interface/Command/CTCmdHotExec.h"
#include "../Game.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../CCamera.h"
#include "../IO_Terrain.h"
#include "../interface/CUIMediator.h"
#include "SystemProcScript.h"
#include "CGame.h"
CGameStateLoginVirtual::CGameStateLoginVirtual( int iID )
{
	m_iStateID = iID;
}

CGameStateLoginVirtual::~CGameStateLoginVirtual(void)
{
}

int CGameStateLoginVirtual::Update(	bool bLostFocus )
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
		if ( ::beginScene() ) //  디바이스가 손실된 상태라면 0을 리턴하므로, 모든 렌더링 스킵
		{
			::clearScreen();
			::renderScene();
			
		
			::beginSprite( D3DXSPRITE_ALPHABLEND );

			/// Screen message display
			g_UIMed.Draw();

			/// UI display
			g_EUILobby.Draw();
			
			::endSprite();		

			::endScene ();
			::swapBuffers();
		}
	}
	return 0;
}

int CGameStateLoginVirtual::Enter( int iPrevStateID )
{
	g_pNet->DisconnectFromServer();
	return 1;
}

void CGameStateLoginVirtual::ServerDisconnected()
{
}
void CGameStateLoginVirtual::WorldServerDisconnected()
{
	if ( !g_pNet->ConnectToServer( g_GameDATA.m_ServerIP.Get(), g_GameDATA.m_wServerPORT, NS_CON_TO_LSV ) )
	{
		CTCmdExit Command;
		Command.Exec( NULL );
	}
}
int CGameStateLoginVirtual::Leave( int iNextStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "LeaveLoginVirtual", ZZ_PARAM_END );
	return 1;
}

void CGameStateLoginVirtual::AcceptedConnectLoginSvr()
{
	const std::string japan_partner_string = CGame::GetInstance().GetJapanPartnerString();

	if( japan_partner_string.empty() )
	{
		g_pNet->Send_cli_LOGIN_REQ( g_GameDATA.m_Account.Get(), g_GameDATA.m_Password.Get() , !g_GameDATA.m_is_NHN_JAPAN );
	}
	else
	{
		std::string temp_id;
		temp_id = g_GameDATA.m_Account.Get();
		temp_id.append( japan_partner_string.c_str() );
		g_pNet->Send_cli_LOGIN_REQ( (char*)temp_id.c_str(), g_GameDATA.m_Password.Get(), !g_GameDATA.m_is_NHN_JAPAN );
	}
}

int CGameStateLoginVirtual::ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	return 1;
}
int CGameStateLoginVirtual::ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	return 1;
}
