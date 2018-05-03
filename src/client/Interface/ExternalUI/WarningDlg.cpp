#include "stdafx.h"
#include ".\warningdlg.h"

#include "CExternalUIManager.h"
#include "ExternalUILobby.h"

#include "../Command/CTCmdHotExec.h"

#include "../../Game.h"
#include "../../Network/CNetwork.h"
#include "../../Network/Net_Prototype.h"
#include "../../System/CGame.h"
#include "../../Util/Localizing.h"
#include "../../CClientStorage.h"
#include "../../Country.h"

//resource
#include "IO_ImageRes.h"
#include "ResourceMgr.h"
#include "TListBox.h"
#include "TScrollBar.h"

CWarningDlg::CWarningDlg(void)
{
}

CWarningDlg::~CWarningDlg(void)
{
}

///
/// set observer class
///
void CWarningDlg::SetEUIObserver( CExternalUIManager* pObserver )
{
	m_pEUIManager = pObserver;
}

bool CWarningDlg::Create( const char* szIDD )
{	
	CTDialog::Create( szIDD );
	return true;
}


UINT CWarningDlg::Process( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	UINT Ret			= 0;
	CWinCtrl*  pCtrl	= NULL;

	if( !IsVision())
		return Ret;

	if( uiMsg == WM_KEYDOWN )
	{
		switch( wParam )
		{		
		case VK_RETURN:
			{					
				break;
			}
		default:
			break;
		}
	}

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process( uiMsg, wParam, lParam ))
	{
		switch( uiMsg )
		{
		case WM_LBUTTONDOWN:
			OnLButtonDown( iProcID );
			break;
		case WM_LBUTTONUP:
			OnLButtonUp( iProcID );
			break;
		default:
			break;
		}
		Ret = uiMsg;
	}

	return Ret;
}

void CWarningDlg::Hide()
{
	CTDialog::Hide();
}

void CWarningDlg::Show()
{
	CTDialog::Show();	

	if( CWinCtrl* pCtrl = Find( IID_IMG_BG ) )
	{
		POINT pt;		
		pt.x = 0 , pt.y = 0;
		pCtrl->MoveWindow(pt);

		pCtrl->SetScaleWidth( (float)g_pCApp->GetWIDTH() / pCtrl->GetWidth() );
		pCtrl->SetScaleHeight( (float)g_pCApp->GetHEIGHT() / pCtrl->GetHeight() );
	
	}


	CWinCtrl* pCtrl = NULL;
	if( pCtrl = Find( IID_LISTBOX_MESSAGE ) )
	{
		FILE * fp = NULL;

		char szTemp[4096] = {0,};

		CTListBox* pListBox = (CTListBox*)pCtrl;		

		if( (fp =fopen("3Ddata\\Control\\Xml\\Agreement.hlp", "r")) )
		{
			while( fgets(szTemp, 4096, fp) )
			{
                OutputDebugString(szTemp);
				
				pListBox->AppendText( szTemp, g_dwWHITE );

			}
		}
		fclose( fp );
		
		

		// 리스트 박스 처음으로 이동.
		pListBox->SetValue( 0 );

		pCtrl = Find( IID_LISTBOX_MESSAGE_SCROLLBAR );
		if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
		{
			CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
			pScrollBar->SetValue( 0 );
		}
	}


	if( CCountry::GetSingleton().IsUSA() )
	{
		if( CWinCtrl* pCtrl = Find( IID_IMG_TALA_SERVER ) )
		{
			pCtrl->Hide();
		}
	}
	else if( CCountry::GetSingleton().IsPhilipplines() )
	{
		if( CWinCtrl* pCtrl = Find( IID_IMG_USA_SERVER ) )
		{
			pCtrl->Hide();
		}
	}


}


void CWarningDlg::MsgBoxProc(  unsigned int iMsgType, short int iButton )
{
	//???
}

void CWarningDlg::OnLButtonUp( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_BTN_OK:
        CGame::GetInstance().ChangeState( CGame::GS_LOGIN );
		break;
	case IID_BTN_CANCEL:
		g_EUILobby.m_bLoginSuccess = false;			
		g_pCApp->SetExitGame();
		break;
	default:
		break;
	}
}

void CWarningDlg::OnLButtonDown( unsigned iProcID )
{	
}

void CWarningDlg::Draw()
{
	if( !IsVision() ) return;

	CTDialog::Draw();
}
