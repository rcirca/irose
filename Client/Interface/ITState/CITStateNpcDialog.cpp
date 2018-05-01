#include "stdafx.h"
#include ".\citstatenpcdialog.h"
#include "../IT_MGR.h"
#include "../SFX/SFXManager.h"

CITStateNpcDialog::CITStateNpcDialog(void)
{
	m_iID = IT_MGR::STATE_NPCDIALOG;
}

CITStateNpcDialog::~CITStateNpcDialog(void)
{
}
void CITStateNpcDialog::Enter()
{
	//g_GameDATA.m_bNoUI = true;
	setClearColor( 0.15f, 0.15f, 0.15f );
	CSFXManager::GetSingleton().StartWideScreenEffect();	/// 와이드 스크린으로 변경
}
void CITStateNpcDialog::Leave()
{
	//g_GameDATA.m_bNoUI = false;
	CSFXManager::GetSingleton().StopWideScreenEffect();		/// 기본 화면으로 변경
	setClearColor( 1.0f, 1.0f, 1.0f );
}

unsigned CITStateNpcDialog::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( uiMsg == WM_LBUTTONUP )
		CWinCtrl::SetMouseExclusiveCtrl( NULL );

	CTDialog* pDlg = NULL;
	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_DIALOG ) )
		if( pDlg->Process( uiMsg, wParam, lParam ) )
			return uiMsg;

	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_SELECTEVENT ) )
		if( pDlg->Process( uiMsg, wParam, lParam ) )
			return uiMsg;


	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_EVENTDIALOG ) )
		if( pDlg->Process( uiMsg, wParam, lParam ) )
			return uiMsg;

	return 0;
}

void CITStateNpcDialog::Update( POINT ptMouse )
{
	CTDialog* pDlg = NULL;
	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_DIALOG ) )
		pDlg->Update( ptMouse );

	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_SELECTEVENT ) )
		pDlg->Update( ptMouse );

	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_EVENTDIALOG ) )
		pDlg->Update( ptMouse );



}

bool CITStateNpcDialog::Draw()
{
	CTDialog* pDlg = NULL;
	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_DIALOG ) )
		pDlg->Draw();

	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_SELECTEVENT ) )
		pDlg->Draw();

	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_EVENTDIALOG ) )
		pDlg->Draw();
	return false;
}