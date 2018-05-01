
#include "StdAfx.h"
#include "BasePAN.h"
#include "../IT_MGR.h"
#include "../CToolTipMgr.h"
#include "../../OBJECT.h"

CBasePAN::CBasePAN()
{

}

CBasePAN::~CBasePAN()
{

}

void CBasePAN::Update( POINT ptMouse )
{
	CTDialog::Update( ptMouse );

	///이하 툴팁 표시
	CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
	if( pDlg && pDlg != this )
			return ;

	WINCTRL_LIST_ITOR iter;
	CWinCtrl*	pCtrl;


	for( iter = m_listChild.begin(); iter != m_listChild.end(); ++iter)
	{
		pCtrl = *iter;
		if( pCtrl->GetControlType() == CTRL_IMAGE )
			continue;

		if( pCtrl->IsInside(ptMouse.x, ptMouse.y ) )
		{
			CToolTipMgr::GetInstance().RegUIInfo( ptMouse.x, ptMouse.y, GetDialogType(), pCtrl->GetControlID());
			break;
		}
	}
}

unsigned int CBasePAN::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			if(On_LButtonUP(iProcID))
				return uiMsg;
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

bool CBasePAN::On_LButtonUP( unsigned iProcID)
{
	switch( iProcID )
	{
	case BASE_BTN_CHAR_OPEN:		// 캐릭터창을 연다 
		g_itMGR.OpenDialog( DLG_TYPE_CHAR );
		return true;
	case BASE_BTN_INV_OPEN:			// 인벤토리창을 연다 
		return true;
	case BASE_BTN_SKILL_OPEN:		// 스킬창을 연다 
		g_itMGR.OpenDialog( DLG_TYPE_SKILL );
		return true;	
	case BASE_BTN_QUEST_OPEN:		// 퀘스트창을 연다 
		g_itMGR.OpenDialog( DLG_TYPE_QUEST );
		return true;
	case BASE_BTN_OPTION_OPEN:
		g_itMGR.OpenDialog( DLG_TYPE_OPTION );
		break;
	case BASE_BTN_COMMUNITY_OPEN:	// 커뮤니티창을 연다 
		///REMARK TEST CODE
		return true;
	case BASE_BTN_SYS_OPEN:			// 시스템 창을 연다 
		g_itMGR.OpenDialog( DLG_TYPE_SYSTEM );
		return true;
	}	

	return false;
}

