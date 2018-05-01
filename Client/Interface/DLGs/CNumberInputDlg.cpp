//*------------------------------------------------------------------------------------
/// 2004 / 6 /11 . 외부에서 입력된 Command의 삭제는 하지 않는다.
//*------------------------------------------------------------------------------------
#include "stdafx.h"
#include ".\cnumberinputdlg.h"
#include "../../../TGameCtrl/TEditBox.h"
#include "../../../TGameCtrl/TButton.h"
#include "../Command/CTCmdNumberInput.h"

#include "../../Game.h"
#include "../it_mgr.h"

CNumberInputDlg::CNumberInputDlg(void)
{
	m_iLastInputNumber	= 0;
	m_iMaxNumber		= -1;
	m_pCmd				= NULL;
}

CNumberInputDlg::~CNumberInputDlg(void)
{
}

unsigned int CNumberInputDlg::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() ) return 0;

	if( uiMsg == WM_KEYDOWN && wParam == VK_RETURN )
	{
		OnLButtonUp( IID_BTN_OK );
		return uiMsg;
	}
	/// 계산기창에서 ESC키가 오면 닫기
	if( uiMsg == WM_KEYDOWN && wParam == VK_ESCAPE )
	{
		OnLButtonUp( IID_BUTTON_CLOSE );
		return uiMsg;
	}

	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch(uiMsg)
		{
		case WM_LBUTTONDOWN:
			SetFocusEditBox();
			break;
		case WM_LBUTTONUP:
			OnLButtonUp( iProcID );
			break;
		default:
			break;
		}
		return uiMsg;
	}


	return 0;
}

void CNumberInputDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

}

void CNumberInputDlg::OnLButtonUp( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_BTN_0:	AppendText("0");break;
	case IID_BTN_1:	AppendText("1");break;
	case IID_BTN_2:	AppendText("2");break;
	case IID_BTN_3:	AppendText("3");break;
	case IID_BTN_4:	AppendText("4");break;
	case IID_BTN_5:	AppendText("5");break;
	case IID_BTN_6: AppendText("6");break;
	case IID_BTN_7:	AppendText("7");break;
	case IID_BTN_8:	AppendText("8");break;
	case IID_BTN_9: AppendText("9");break;

	case IID_BUTTON_CLOSE:
		m_iLastInputNumber = 0;
		ClearCommand();
		Hide();
		break;
	case IID_BTN_DEL:
		DeleteOneChar();
		break;
	case IID_BTN_OK:
		m_iLastInputNumber = GetNumberFromEditBox();
		SendCommand();
		Hide();
		break;
	case IID_BTN_MAX:
		if( m_iMaxNumber >= 0 )
		{
			m_iLastInputNumber = m_iMaxNumber;
			SendCommand();
			Hide();
/*			char szTemp[256] = {0};
			_i64toa( m_iMaxNumber,szTemp,10);
			AppendText( szTemp );*/
		}
		break;
	default:
		break;
	}
}

//void CNumberInputDlg::ChangeButtonState( unsigned int iID, int iState )
//{
//	CWinCtrl* pCtrl = Find( iID );
//	if( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON )
//	{
//		CTButton* pButton = (CTButton*)pCtrl;
//		pButton->SetButtonState( iState );
//	}
//}
void CNumberInputDlg::DeleteOneChar()
{
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
	{
		CTEditBox* pEditBox = (CTEditBox*)pCtrl;

		/// 0 : delete key
		/// 1 : backspace
		pEditBox->delete_one_char(1);
	}
}
void CNumberInputDlg::AppendText( char* szText )
{
	if( szText == NULL )
		return;

	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
	{
		CTEditBox* pEditBox = (CTEditBox*)pCtrl;
		pEditBox->AppendText( szText );
	}
}
__int64 CNumberInputDlg::GetNumberFromEditBox()
{
	
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
	{
		CTEditBox* pEditBox = (CTEditBox*)pCtrl;
		char szText[255] = {0}; 
		char* pszText = pEditBox->get_text(); 
		if( pszText == NULL )
			return 0;


		int iSize = 0;
		if( sizeof( szText ) <= strlen( pszText ) )
			iSize = sizeof( szText ) - 1;
		else
			iSize = strlen( pszText );

		strncpy( szText, pszText ,iSize);
		
		__int64 iRet = _atoi64( szText );

		///맥스 제한이 걸렸고 입력값이 맥스보다 클경우
		if( m_iMaxNumber >= 0 && m_iMaxNumber < iRet )
			iRet = m_iMaxNumber;
	
		return iRet;
	}
	return 0;
}
void CNumberInputDlg::Show()
{
	CTDialog::Show();
	SetFocusEditBox();
}

void CNumberInputDlg::SetFocusEditBox()
{
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	if( pCtrl->GetControlType() != CTRL_EDITBOX )
		return;

	pCtrl->SetFocus( true );

}

void CNumberInputDlg::ClearCommand()
{
	if( m_pCmd )
	{
		m_pCmd = NULL;
		m_pCommandParam = NULL;
	}
}

void CNumberInputDlg::SendCommand()
{
	if( m_pCmd )
	{
		if( m_iLastInputNumber > 0 )
		{
			m_pCmd->SetNumber( m_iLastInputNumber );
			m_pCmd->Exec( m_pCommandParam );
		}
		ClearCommand();
	}
}

/// Invoker에 전달 안된 Command는 삭제한다.
void CNumberInputDlg::SetCommand( CTCmdNumberInput* pCmd, CTObject* pObj )
{
	m_pCmd			= pCmd;
	m_pCommandParam = pObj;
}

void CNumberInputDlg::Hide()
{
	CTDialog::Hide();
	ClearCommand();
}