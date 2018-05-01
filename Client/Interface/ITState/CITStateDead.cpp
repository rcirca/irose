#include "stdafx.h"
#include ".\citstatedead.h"
#include "../it_mgr.h"
#include "../../Object.h"

CITStateDead::CITStateDead(void)
{
	m_iID = IT_MGR::STATE_DEAD;
}

CITStateDead::~CITStateDead(void)
{

}

void CITStateDead::Enter()
{
	g_itMGR.OpenDialog( DLG_TYPE_RESTART );
	g_pAVATAR->SetBattleTime( 0 );
	
	
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_PRIVATESTORE ) )
		pDlg->Hide();


}

void CITStateDead::Leave()
{
	g_itMGR.CloseDialog( DLG_TYPE_RESTART );
}

unsigned CITStateDead::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( uiMsg == WM_LBUTTONUP )
		CWinCtrl::SetMouseExclusiveCtrl( NULL );

	///채팅과 리스타트 다이얼로그만 처리한다.나머지는 무시한다.
	CTDialog* pDlg = NULL;

	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_RESTART ) )
		if( pDlg->Process( uiMsg, wParam, lParam ) )
			return uiMsg;

	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
		if( pDlg->Process( uiMsg, wParam, lParam ) )
			return uiMsg;

	return 0;
}