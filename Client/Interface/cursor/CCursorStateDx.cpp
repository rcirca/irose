#include "stdafx.h"
#include ".\ccursorstatedx.h"
#include "../../CApplication.h"
#include "resource.h"
CCursorStateDx::CCursorStateDx(void)
{
	m_iState = CCursor::STATE_DX;
	LoadCursor();
}

CCursorStateDx::~CCursorStateDx(void)
{
	//for( int i = 0 ; i < CCursor::CURSOR_MAX; ++i )
	//	unloadCursor( m_hDxCursor[ i ] );
	UnloadCursor();

}
void CCursorStateDx::LoadCursor()
{
	HINSTANCE hInstance = CApplication::Instance()->GetHINS();
	m_hCursor[ CCursor::CURSOR_DEFAULT ]   = (HCURSOR)LoadImage( hInstance ,MAKEINTRESOURCE( IDC_CURSOR_DEFAULT ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );

	//::LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_DEFAULT ) );
	//m_hDxCursor[ CCursor::CURSOR_DEFAULT ] = loadCursor( "CURSOR_DEFAULT",m_hCursor[ CCursor::CURSOR_DEFAULT] );

	m_hCursor[ CCursor::CURSOR_ATTACK ] = (HCURSOR)LoadImage( hInstance, MAKEINTRESOURCE( IDC_CURSOR_ATTACK ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_ATTACK ) );
	//m_hDxCursor[ CCursor::CURSOR_ATTACK ] = loadCursor( "CURSOR_ATTACK", m_hCursor[ CCursor::CURSOR_ATTACK] );

	m_hCursor[ CCursor::CURSOR_ITEM_INVEN ] = (HCURSOR)LoadImage( hInstance ,MAKEINTRESOURCE( IDC_CURSOR_ITEM_INVEN ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_ITEM_INVEN ) );
	//m_hDxCursor[ CCursor::CURSOR_ITEM_INVEN ] = loadCursor( "CURSOR_ITEM_INVEN", m_hCursor[ CCursor::CURSOR_ITEM_INVEN ] );

	m_hCursor[ CCursor::CURSOR_ITEM_PICK ] = (HCURSOR)LoadImage( hInstance,MAKEINTRESOURCE( IDC_CURSOR_ITEM_PICK ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_ITEM_PICK ) );
	//m_hDxCursor[ CCursor::CURSOR_ITEM_PICK ] = loadCursor( "CURSOR_ITEM_PICK", m_hCursor[ CCursor::CURSOR_ITEM_PICK ] );

	m_hCursor[ CCursor::CURSOR_LEFT ] = (HCURSOR)LoadImage( hInstance, MAKEINTRESOURCE( IDC_CURSOR_LEFT ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_LEFT ) );
	//m_hDxCursor[ CCursor::CURSOR_LEFT ] = loadCursor( "CURSOR_LEFT", m_hCursor[ CCursor::CURSOR_LEFT ] );

	m_hCursor[ CCursor::CURSOR_RIGHT ] = (HCURSOR)LoadImage( hInstance, MAKEINTRESOURCE( IDC_CURSOR_RIGHT ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_RIGHT ) );
	//m_hDxCursor[ CCursor::CURSOR_RIGHT ] = loadCursor( "CURSOR_RIFHT", m_hCursor[ CCursor::CURSOR_RIGHT ] );

	m_hCursor[ CCursor::CURSOR_NPC ] = (HCURSOR)LoadImage( hInstance, MAKEINTRESOURCE( IDC_CURSOR_NPC ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_NPC ) );
	//m_hDxCursor[ CCursor::CURSOR_NPC ] = loadCursor( "CURSOR_NPC", m_hCursor[ CCursor::CURSOR_NPC ]);

	m_hCursor[ CCursor::CURSOR_USER ] = (HCURSOR)LoadImage( hInstance, MAKEINTRESOURCE( IDC_CURSOR_USER ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_USER ) );
	//m_hDxCursor[ CCursor::CURSOR_USER ] = loadCursor( "CURSOR_USER", m_hCursor[ CCursor::CURSOR_USER ] );

	m_hCursor[ CCursor::CURSOR_WHEEL ] = (HCURSOR)LoadImage( hInstance, MAKEINTRESOURCE( IDC_CURSOR_WHEEL ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	///\LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_WHEEL ) );
	//m_hDxCursor[ CCursor::CURSOR_WHEEL ] = loadCursor( "CURSOR_WHEEL" , m_hCursor[ CCursor::CURSOR_WHEEL ] );

	m_hCursor[ CCursor::CURSOR_NOUI ] = (HCURSOR)LoadImage( hInstance, MAKEINTRESOURCE( IDC_CURSOR_NOUI ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_NOUI ) );
	//m_hDxCursor[ CCursor::CURSOR_NOUI ] = loadCursor( "CURSOR_NOUI", m_hCursor[ CCursor::CURSOR_NOUI ] );

	m_hCursor[ CCursor::CURSOR_REPAIR ] = (HCURSOR)LoadImage( hInstance,MAKEINTRESOURCE( IDC_CURSOR_REPAIR ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_CURSOR_REPAIR ) );
	//m_hDxCursor[ CCursor::CURSOR_REPAIR ] =  loadCursor( "CURSOR_REPAIR", m_hCursor[ CCursor::CURSOR_REPAIR ] );

	m_hCursor[ CCursor::CURSOR_APPRAISAL ] = (HCURSOR)LoadImage( hInstance,MAKEINTRESOURCE( IDC_IDENTIFY ), IMAGE_CURSOR,0,0, LR_DEFAULTSIZE );
	//LoadCursor( CApplication::Instance()->GetHINS(), MAKEINTRESOURCE( IDC_IDENTIFY ) );
	//m_hDxCursor[ CCursor::CURSOR_APPRAISAL ] =  loadCursor( "CURSOR_APPRAISAL", m_hCursor[ CCursor::CURSOR_APPRAISAL ] );
}

void CCursorStateDx::UnloadCursor()
{
	for( int i = 0 ; i < CCursor::CURSOR_MAX; ++i )
		DestroyCursor( m_hCursor[i] );

}

/**
* nVidia계열의 비디도 카드에서 윈도우의 Depth와 게임의 Depth가 틀릴경우 Cursor resource가 깨지는 경우가 발생한다(위치도 틀려짐)
* resetScreen시 항상 Cursor를 Reload한다.
*
* @Author		최종진
* @Date			2004/10/12
*/ 
void CCursorStateDx::ReloadCursor()
{
	UnloadCursor();
	LoadCursor();
	SetCursorType( CCursor::GetInstance().GetCurrCursorType() );
}

int CCursorStateDx::SetCursorType( int iType )
{
	if( g_GameDATA.m_bNoUI )
	{
		SetCursor( m_hCursor[ CCursor::CURSOR_NOUI ] );
		//SetCursor( NULL );		
		//showCursor( m_hDxCursor[ CCursor::CURSOR_NOUI ] );
	}
	else
	{
		SetCursor( m_hCursor[ iType ] );

		//SetCursor( NULL );
		//showCursor( m_hDxCursor[ iType ] );

		//if( !g_GameDATA.m_bShowCursor )
		//	SetCursor( NULL );
	}
	return 1;
}