#include "StdAfx.h"
#include ".\tpane.h"

CTPane::CTPane(void)
{
	SetControlType( CTRL_PANE );
}

CTPane::~CTPane(void)
{

}

void CTPane::Update( POINT ptMouse )
{
	CWinCtrl::Update( ptMouse );
	m_Children.Update( ptMouse );
}

void CTPane::Draw()
{
	if( !IsVision() ) return;
	m_Children.Draw();
}

unsigned CTPane::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	if( unsigned uiProcID = m_Children.Process( uiMsg, wParam,lParam ) )
		return uiProcID;

	if( IsInside( LOWORD(lParam), HIWORD(lParam) ) )
		return m_iControlID;

	return 0;
}

void CTPane::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );
	m_Children.MoveWindow( m_sPosition );
}

void CTPane::Show()
{
	CWinCtrl::Show();
	m_Children.Show();
}

void CTPane::Hide()
{
	CWinCtrl::Hide();
	m_Children.Hide();
}

void CTPane::AddChild( CWinCtrl* pCtrl )
{
	m_Children.Add( pCtrl );
}

CWinCtrl* CTPane::FindChild( unsigned iID )
{
	return m_Children.Find( iID );
}

CWinCtrl* CTPane::Find( const char * szName )
{
	return m_Children.Find( szName );
}

