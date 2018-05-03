#include "stdafx.h"
#include ".\CITStateRideCart.h"
#include "../it_mgr.h"
#include "../../Object.h"

// 카트모드에서 할 수 있는 동작//

// 1. 하차하기,
// 2. 공통스킬 사용 (이모티콘등)
// 3. 카트 보조 스킬 사용 (현재 없음.)
// 4. 채팅 모드

CITStateRideCart::CITStateRideCart(void)
{
	m_iID = IT_MGR::STATE_RIDECART;
}


CITStateRideCart::~CITStateRideCart(void)
{

}


void CITStateRideCart::Enter()
{
	// 1, 2번 셋팅	
	g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState_Aid( true );	
}

void CITStateRideCart::Leave()
{
	g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState_Aid( false );
}

unsigned CITStateRideCart::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( uiMsg == WM_LBUTTONUP )
		CWinCtrl::SetMouseExclusiveCtrl( NULL );

	///채팅, 스킬, 단축키 다이얼로그만 처리한다.나머지는 무시한다.
	CTDialog* pDlg = NULL;

	// 4. 채팅모드
	if( pDlg = g_itMGR.FindDlg( DLG_TYPE_CHAT ) )
	{
		if( pDlg->Process( uiMsg, wParam, lParam ) )
		{
			return uiMsg;
		}
	}

	return 0;
}