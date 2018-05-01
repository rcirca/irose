#include "stdafx.h"
#include ".\cmemoviewdlg.h"
#include "../../Game.h"
#include "../../SqliteDB.h"
#include "../../object.h"
#include "../it_mgr.h"
#include "CCommDlg.h"


#include "TButton.h"
CMemoViewDlg::CMemoViewDlg(void)
{
}

CMemoViewDlg::~CMemoViewDlg(void)
{
}
bool CMemoViewDlg::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{	
		CWinCtrl* pCtrl = Find( IID_BTN_DELETE );
		if( pCtrl->GetControlType() == CTRL_BUTTON )
		{
			CTButton* pBtn = (CTButton*)pCtrl;
			pBtn->SetText( STR_DELETE );
		}
		return true;
	}
	return false;
}
void CMemoViewDlg::Draw()
{
	if(!IsVision() ) return;
	CTDialog::Draw();

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );
	
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 40, 8, g_dwORANGE, 
		CStr::Printf("From:%s", m_strName.c_str() ) );

	RECT rcDraw ={ 10, 30, 200, 100 };
	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL], true, &rcDraw, g_dwBLACK, DT_WORDBREAK ,m_strContent.c_str() );

}

unsigned CMemoViewDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsVision() ) return 0;

	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ))
	{
		if( uiMsg == WM_LBUTTONUP )
		{
			switch( uiProcID )
			{
			case IID_BTN_DELETE:
				
				CSqliteDB::GetInstance().DeleteMemo( m_iID );				/// 클라이언트 DB에서 삭제
				
				if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY ) )
				{
					CCommDlg* pCommDlg = (CCommDlg*)pDlg;
					pCommDlg->DeleteMemo( m_iID );
				}
				Hide();
				break;
			case IID_BTN_CLOSE:
			case IID_BTN_OK:
				Hide();
				break;
			default:
				break;
			}
		}
		return uiMsg;
	}
	return 0;
}

void CMemoViewDlg::SetTargetName( const char* pszName )
{
	assert( pszName );
	if( pszName )
		m_strName = pszName;
}

void CMemoViewDlg::SetContent( const char* pszContent )
{
	assert( pszContent );
	if( pszContent )
		m_strContent = pszContent;
}

void CMemoViewDlg::SetID( int iID )
{
	m_iID = iID;
}