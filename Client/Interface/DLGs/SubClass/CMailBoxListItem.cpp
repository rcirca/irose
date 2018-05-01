#include "stdafx.h"
#include ".\cmailboxlistitem.h"

#include "../CMemoViewDlg.h"
#include "../../it_mgr.h"
#include "../../../Game.h"
#include "../../../Util/classTime.h"

#include "ZListBox.h"
CMailBoxListItem::CMailBoxListItem( const char* pszName, const char* pszMemo , DWORD dwRecvTime )
{
	SetHeight( 20 );
	SetWidth( 174 );
	assert( pszName && pszMemo );

	classTIME::AbsSecondToSystem( dwRecvTime, m_RecvTime );

	if( pszName && pszMemo )
	{
		m_strName = pszName;
		m_strMemo = pszMemo;
	}
}

CMailBoxListItem::~CMailBoxListItem(void)
{
}

void CMailBoxListItem::Draw()
{
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	///Èû
	if( IsSelected() )
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 0, 0, g_dwYELLOW, 
		CStr::Printf("From %s(%d/%d/%d)",m_strName.c_str(), m_RecvTime.wMonth, m_RecvTime.wDay , m_RecvTime.wHour ));
	else
		::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 0, 0, g_dwBLACK, 
		CStr::Printf("From %s(%d/%d/%d)",m_strName.c_str(), m_RecvTime.wMonth, m_RecvTime.wDay , m_RecvTime.wHour ));

}

unsigned CMailBoxListItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsInside( LOWORD(lParam), HIWORD( lParam )) ) return 0;

	switch( uiMsg )
	{
	case WM_LBUTTONDOWN:
		SetSelected();
		return GetControlID();
	case WM_LBUTTONDBLCLK:
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MEMOVIEW );
			assert( pDlg );
			if( pDlg )
			{
				CMemoViewDlg* pMemoViewDlg = (CMemoViewDlg*)pDlg;
				pMemoViewDlg->SetTargetName( m_strName.c_str() );
				pMemoViewDlg->SetContent( m_strMemo.c_str() );
				pMemoViewDlg->SetID( GetControlID() );
				g_itMGR.OpenDialog( DLG_TYPE_MEMOVIEW, false );
			}
			break;
		}
	default:
		break;
	}
	
	return 0;
}
void CMailBoxListItem::SetSelected()
{
	if( m_pParent )
	{
		if( m_pParent->GetControlType() == CTRL_ZLISTBOX )
		{
			CZListBox* pList = ( CZListBox*)m_pParent;
			pList->DeselectAll();
		}
	}
	CWinCtrl::SetSelected();
}
