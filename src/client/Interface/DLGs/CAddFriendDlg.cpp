#include "stdafx.h"
#include ".\caddfrienddlg.h"
#include "tgamectrl/teditbox.h"
#include "CCommDlg.h"
#include "../it_mgr.h"
#include "../../Network/CNetwork.h"
CAddFriendDlg::CAddFriendDlg( int iDlgType )
{
	SetDialogType( iDlgType );
}

CAddFriendDlg::~CAddFriendDlg(void)
{

}

unsigned CAddFriendDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
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
				case IID_BTN_CONFIRM:
					if( SendReqAddFriend() )
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

bool CAddFriendDlg::SendReqAddFriend()
{
	CWinCtrl* pCtrl = Find( IID_EDITBOX );
	assert( pCtrl );
	if( pCtrl == NULL ) return false;

	assert( pCtrl->GetControlType() == CTRL_EDITBOX );
	if(pCtrl->GetControlType() != CTRL_EDITBOX) return false;

	CTEditBox* pEditBox = (CTEditBox*)pCtrl;

	char* pszName = pEditBox->get_text();
	if( pszName && strlen(pszName) > 0 )
	{
		if( strcmpi( pszName, g_pAVATAR->Get_NAME() ))///자기 자신은 안된다.
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				if( pCommDlg->FindFriendByName( pszName ) == NULL )
					g_pNet->Send_cli_MCMD_APPEND_REQ( pszName );
				else
					g_itMGR.OpenMsgBox(STR_DUPLICATED_FRIENDNAME);
			}
			return true;
		}
	}
	return false;
}

