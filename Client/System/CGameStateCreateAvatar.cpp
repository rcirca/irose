#include "stdafx.h"
#include "CGameStateCreateAvatar.h"
#include "CGame.h"
#include "SystemProcScript.h"

#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../interface/CUIMediator.h"
#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "../CJustModelAVT.h"

#include "../GameData/CGameDataCreateAvatar.h"

#include "tgamectrl/time2.h"
CGameStateCreateAvatar::CGameStateCreateAvatar( int iID )
{ 
	m_iStateID = iID; 
}

CGameStateCreateAvatar::~CGameStateCreateAvatar(void)
{

}

int	CGameStateCreateAvatar::Update( bool bLostFocus )
{
	g_EUILobby.Update();

	g_pCamera->Update ();
	
	D3DVECTOR PosENZIN = g_pCamera->Get_Position ();	
	g_pTerrain->SetCenterPosition( PosENZIN.x, PosENZIN.y );

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

int CGameStateCreateAvatar::Enter( int iPrevStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "EnterCreateAvatar", ZZ_PARAM_END );
	g_EUILobby.CreateCreateAvataDlg();

	//m_pCreateAvatar->Init( "test1model", 0, 0, 0, 0, 3, 0, 0, 0, 0, 0 );
	//m_pCreateAvatar->SetRace( 0 );
	//m_pCreateAvatar->UpdateModel();
	//m_pCreateAvatar->LoadModelVisible();
	//m_pCreateAvatar->InsertToScene();
	//
	//m_pCreateAvatar->SetPosition(520005.f, 520018.f, 747.f);
	return 0;
}

int CGameStateCreateAvatar::Leave( int iNextStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "LeaveCreateAvatar", ZZ_PARAM_END );
	return 0;
}

int CGameStateCreateAvatar::ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;

	return 0;
}

int CGameStateCreateAvatar::ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;

	return 0;
}

