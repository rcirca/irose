#include "stdafx.h"
#include ".\cmemodlg.h"
#include "../../Game.h"
#include "../../Network/CNetwork.h"

#include "TEditBox.h"
#include "TButton.h"

CMemoDlg::CMemoDlg( int iDlgType )
{
	SetDialogType( iDlgType );
}

CMemoDlg::~CMemoDlg(void)
{

}

bool CMemoDlg::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		CWinCtrl* pCtrl = Find( IID_BTN_SEND );
		if( pCtrl->GetControlType() == CTRL_BUTTON )
		{
			CTButton* pBtn = (CTButton*)pCtrl;
			pBtn->SetText( STR_SEND );
		}
		return true;
	}
	return false;
}

void CMemoDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );
	
	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 30, 8, g_dwORANGE,
		CStr::Printf("To:%s", m_strName.c_str() ) );
}

unsigned CMemoDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ))
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			{
				switch( uiProcID )
				{
				case IID_BTN_CLOSE:
					Hide();
					break;
				case IID_BTN_SEND:
					SendMemo();
					Hide();
					break;
				default:
					break;
				}
				break;
			}
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CMemoDlg::SendMemo()
{
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	assert( pCtrl );
	if( pCtrl == NULL ) return;

	assert( pCtrl->GetControlType() == CTRL_EDITBOX );
	if( pCtrl->GetControlType() != CTRL_EDITBOX ) return;

	CTEditBox* pEditBox = (CTEditBox*)pCtrl;
	char* pszMemo = pEditBox->get_text();
	if( pszMemo == NULL ) return;
	if( strlen( pszMemo ) <= 0 ) return;

	g_pNet->Send_cli_MEMO( (char*)m_strName.c_str(), pszMemo );
}

void CMemoDlg::SetTargetName( const char* pszName )
{
	assert( pszName );
	if( pszName )
		m_strName = pszName;
}
