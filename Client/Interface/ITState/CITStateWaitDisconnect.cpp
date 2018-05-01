#include "stdafx.h"
#include ".\citstatewaitdisconnect.h"
#include "../IT_MGR.h"
#include "../../Network/CNetwork.h"
#include "../dlgs/cmsgboxspecial.h"
#include "../command/uicommand.h"
#include "../../System/CGame.h"
#include "../../System/TCmdSystem.h"

CITStateWaitDisconnect::CITStateWaitDisconnect(void)
{
	m_iID = IT_MGR::STATE_WAITDISCONNECT;
}

CITStateWaitDisconnect::~CITStateWaitDisconnect(void)
{

}

void CITStateWaitDisconnect::Enter()
{
	m_iWaitDisconnectedTime = g_itMGR.GetWaitDisconnectTime();
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MSGBOX_SPECIAL ) )
	{
		CMsgBoxSpecial* pSpecialMsgBox = (CMsgBoxSpecial*)pDlg;
		pSpecialMsgBox->SetString(CStr::Printf(STR_WAITTIME_EXITGAME ,m_iWaitDisconnectedTime ) );
		pSpecialMsgBox->SetModeless();


		pSpecialMsgBox->SetButtonType( CMsgBox::BT_CANCEL );

		CTCommand* pCmdCancel = new CTCmdCancelWaitDisconnect;
		pSpecialMsgBox->SetCommand( NULL, pCmdCancel );

		g_itMGR.OpenDialog( DLG_TYPE_MSGBOX_SPECIAL , false, 
			g_pCApp->GetWIDTH() / 2 - pSpecialMsgBox->GetWidth() / 2, 
			g_pCApp->GetHEIGHT() / 2 - pSpecialMsgBox->GetHeight() * 2);

	}
	m_dwEnterTime = g_GameDATA.GetGameTime();
	m_dwPrevUpdateTime = m_dwEnterTime;
}

void CITStateWaitDisconnect::Leave()
{
	g_itMGR.CloseDialog( DLG_TYPE_MSGBOX_SPECIAL );	
}

unsigned CITStateWaitDisconnect::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( uiMsg == WM_LBUTTONUP )
		CWinCtrl::SetMouseExclusiveCtrl( NULL );

	CTDialog* pDlg = NULL;


	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_MSGBOX_SPECIAL ) )
		if( pDlg->Process( uiMsg, wParam, lParam ) )
			return uiMsg;

	return 0;
}

void CITStateWaitDisconnect::Update( POINT ptMouse )
{
	DWORD dwCurrTime = g_GameDATA.GetGameTime();
	int	iWaitTime    = m_iWaitDisconnectedTime - ( dwCurrTime - m_dwEnterTime) / 1000;

	//if( iWaitTime <= 0 )
	//{
	//	if( g_itMGR.GetWaitDisconnectType() == 1 )
	//	{
	//		CTCommand* pCmd = new CTCmdChangeGameState( CGame::GS_EXITMAIN );
	//		CGame::GetInstance().AddTCommand( pCmd );
	//	}
	//	else
	//	{
	//		g_pCApp->SetExitGame();
	//	}
	//}
	
	if( iWaitTime > 0 && ((dwCurrTime - m_dwPrevUpdateTime) >= 1000) )
	{
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MSGBOX_SPECIAL ) )
		{
			CMsgBoxSpecial* pSpecialMsgBox = (CMsgBoxSpecial*)pDlg;
			pSpecialMsgBox->SetString(CStr::Printf( STR_WAITTIME_EXITGAME,iWaitTime ) );
		}
		m_dwPrevUpdateTime = dwCurrTime;
	}
}