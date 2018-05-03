#include "stdafx.h"
#include "CGameStateSelectAvatar.h"
#include "CGame.h"
#include "SystemProcScript.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/IO_ImageRes.h"
#include "../interface/CUIMediator.h"
#include "../interface/ExternalUI/CSelectAvata.h"
#include "../CClientStorage.h"
#include "../IO_Terrain.h"
#include "../CCamera.h"
#include "../CSkyDOME.H"
#include "../GameData/CGameDataCreateAvatar.h"
#include "../CJustModelAVT.h"
#include "../country.h"

CGameStateSelectAvatar::CGameStateSelectAvatar(int iID)
{
	m_iStateID = iID;
}

CGameStateSelectAvatar::~CGameStateSelectAvatar(void)
{
}

int	CGameStateSelectAvatar::Update( bool bLostFocus )
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
		///카메라 모션이 멈춰있을경우에만 보이게 한다.
		if( getAnimatableState( g_pCamera->GetZHANDLE() ) == 0 )
		{
			g_EUILobby.Draw();
			D3DVECTOR vec = g_pCamera->Get_Position ();
		}

		CGameDataCreateAvatar::GetInstance().Update();
		::endSprite();		
	

		::endScene ();
		::swapBuffers();
	} 

	return 0;
}

int CGameStateSelectAvatar::Enter( int iPrevStateID )
{
	CGameDataCreateAvatar::GetInstance().ClearSelectedAvatar();
	CSystemProcScript::GetSingleton().CallLuaFunction( "EnterSelectAvatar", ZZ_PARAM_END );

	g_pSoundLIST->SetVolume( -10000 );
	g_EUILobby.CreateAvataListDlg();

	//박지호::아로아 데이터 초기화 
	if(CCountry::GetSingleton().IsApplyNewVersion())
		goddessMgr.Release();

	return 0;
}

int CGameStateSelectAvatar::Leave( int iNextStateID )
{
	CSystemProcScript::GetSingleton().CallLuaFunction( "LeaveSelectAvatar", ZZ_PARAM_END );
	return 0;
}


int CGameStateSelectAvatar::ProcMouseInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( getAnimatableState( g_pCamera->GetZHANDLE() ) != 0 )
		return 0;

	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;
	/// 아바타 모델에 대한 마우스 오버및 LButtonDown처리

	switch( uiMsg )
	{
	case WM_LBUTTONDOWN:
		{
			const char* pszName = CGameDataCreateAvatar::GetInstance().Pick_AVATAR( LOWORD(lParam), HIWORD(lParam) );
			if( pszName )
			{
				const char* pszSelectedName = CGameDataCreateAvatar::GetInstance().GetSelectedAvatarName();
				
				///이전 선택되어진 아바타와 같은 아바타라면 무시
				if( pszSelectedName && strcmp( pszName, pszSelectedName ) == 0 )
					break;

				CJustModelAVT* pPrevAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( pszSelectedName );
				CJustModelAVT* pNowAvt = CGameDataCreateAvatar::GetInstance().FindAvatar( pszName );
				int iPrevAvtDeleteState = 0;
				if( pPrevAvt && pPrevAvt->GetDeleteRemainSEC() )
					iPrevAvtDeleteState = 1;

				int iSex = 0;
				if( pNowAvt && pNowAvt->IsFemale() )
					iSex = 1;
				
				CSystemProcScript::GetSingleton().CallLuaFunction( "OnSelectAvatar", 
						ZZ_PARAM_INT,    CGameDataCreateAvatar::GetInstance().GetSelectedAvatarIndex(),
						ZZ_PARAM_STRING, pszName, 
						ZZ_PARAM_INT,	 iSex,
						ZZ_PARAM_STRING, CGameDataCreateAvatar::GetInstance().GetSelectedAvatarName(),
						ZZ_PARAM_INT,    iPrevAvtDeleteState,
						ZZ_PARAM_INT,    ( pNowAvt->GetDeleteRemainSEC() )?1:0,
						ZZ_PARAM_END );

				CGameDataCreateAvatar::GetInstance().SelectAvatar( pszName );

			}
			break;
		}
	case WM_LBUTTONDBLCLK:
		{
			const char* pszName = CGameDataCreateAvatar::GetInstance().Pick_AVATAR( LOWORD(lParam), HIWORD(lParam) );
			if( pszName )
			{
				CGameDataCreateAvatar::GetInstance().SelectAvatar( pszName );
				CSelectAvata* pDlg = (CSelectAvata*)g_EUILobby.GetEUI(EUI_SELECT_AVATA);
				pDlg->SendSelectAvataReq();
			}
			break;
		}
		break;
	default:
		break;
	}
	return 0;
}


int CGameStateSelectAvatar::ProcKeyboardInput( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( getAnimatableState( g_pCamera->GetZHANDLE() ) != 0 )
		return 0;

	if( g_EUILobby.MsgProc( uiMsg ,wParam, lParam) )
		return 1;

	return 0;
}

void CGameStateSelectAvatar::ServerDisconnected()
{
	g_EUILobby.ServerDisconnected();
}

