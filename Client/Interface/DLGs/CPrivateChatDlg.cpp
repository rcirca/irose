#include "stdafx.h"
#include ".\cprivatechatdlg.h"
#include "Net_Prototype.h"
#include "../../Network/CNetwork.h"
#include "../../Game.h"
#include "../it_mgr.h"
#include "../Command/CTCmdOpenDlg.h"
#include "CCommDlg.h"
#include "subclass/CFriendListItem.h"


#include "tgamectrl/tlistbox.h"
#include "tgamectrl/teditbox.h"
#include "tgamectrl/tscrollbar.h"
#include "tgamectrl/actionevent.h"
CPrivateChatDlg::CPrivateChatDlg( int iDlgType )
{
	SetDialogType( iDlgType );
}

CPrivateChatDlg::~CPrivateChatDlg(void)
{

}

bool CPrivateChatDlg::Create( const char* IDD )
{
	if( CTDialog::Create( IDD ) )
	{
		CWinCtrl* pCtrl = Find( IID_EDITBOX );
		assert( pCtrl );
		if( pCtrl )
			pCtrl->AddActionListener( this );

		return true;
	}
	return false;
}

void CPrivateChatDlg::SetOther( DWORD dwUserTag, BYTE btStatus, const char* pszName )
{
	m_dwUserTag = dwUserTag;
	m_btStatus	= btStatus;
	assert( pszName );
	if( pszName )
		m_strName = pszName ;
}
	
void CPrivateChatDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	char* pszStatus;
	switch( m_btStatus )
	{
	case FRIEND_STATUS_HUNT:// 사냥중
		pszStatus = STR_MYSTATE_IN_HUNT;
		break;
	case FRIEND_STATUS_STORE:// 장사중
		pszStatus = STR_MYSTATE_IN_TRADE;
		break;
	case FRIEND_STATUS_QUEST:// 퀘스트중
		pszStatus = STR_MYSTATE_IN_QUEST;
		break;
	case FRIEND_STATUS_EAT:// 식사중
		pszStatus = STR_MYSTATE_IN_EAT;
		break;
	case FRIEND_STATUS_REST:// 휴식중
		pszStatus = STR_MYSTATE_IN_BREAK;
		break;
	case FRIEND_STATUS_REFUSED:///대화거절
	case FRIEND_STATUS_DELETED:///상대방이 나를 친구목록에서 제외했다.
	case FRIEND_STATUS_OFFLINE:// 접속끊김		: 친구목록의 m_dwUserTAG가 접속끊김
		pszStatus = "OffLine";
		break;
	case FRIEND_STATUS_ONLINE:// 접속됨		: 친구목록의 m_dwUserTAG가 접속했다.
	case FRIEND_STATUS_NORMAL:
	default:
		pszStatus = "OnLine";
		break;
	}

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 30, 8, g_dwORANGE,
		CStr::Printf("To:%s(%s)", m_strName.c_str(), pszStatus ) );

}

unsigned CPrivateChatDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
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

///메세지를 보낼때마다 상대방의 상태를 체크해서 m_btStatus를 Update한다.만약 리스트에서 삭제되었다면 창을 닫는다.
void CPrivateChatDlg::SendChatMsg()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
	assert( pDlg );
	if( pDlg == NULL ) return;

	CCommDlg* pCommDlg = (CCommDlg*)pDlg;
	
	CFriendListItem* pItem = pCommDlg->FindFriend( m_dwUserTag );
	if( pItem == NULL )
	{
		Hide();
		g_itMGR.OpenMsgBox( STR_NOT_FOUND_FRIEND );
		return;
	}
	
	m_btStatus = pItem->GetStatus();

	switch( m_btStatus )
	{
	case FRIEND_STATUS_REFUSED:///대화거절
	case FRIEND_STATUS_DELETED:///상대방이 나를 친구목록에서 제외했다.
	case FRIEND_STATUS_OFFLINE:// 접속끊김		: 친구목록의 m_dwUserTAG가 접속끊김
		///대화거절
		break;
	default:
		{
			CWinCtrl* pCtrl = Find( IID_EDITBOX );
			assert( pCtrl );
			if( pCtrl )
			{
				CTEditBox* pEditBox = (CTEditBox*)pCtrl;
				char* pszMsg = pEditBox->get_text();
				if( pszMsg )
				{
					g_pNet->Send_cli_MESSENGER_CHAT( m_dwUserTag, pszMsg );
					std::string strMsg = g_pAVATAR->Get_NAME();
					strMsg.append("> ");
					strMsg.append( pszMsg );
					AddChatMsg( strMsg.c_str() );		

					pEditBox->clear_text();
				}
			}
			break;
		}
	}
}

void CPrivateChatDlg::AddChatMsg( const char* pszMsg )
{
	CWinCtrl* pCtrl = Find( IID_LISTBOX );
	assert( pCtrl );
	if( pCtrl == NULL ) return;

	assert( pCtrl->GetControlType() == CTRL_LISTBOX );
	if( pCtrl->GetControlType() != CTRL_LISTBOX ) return ;

	CTListBox* pListBox = (CTListBox*)pCtrl;
	pListBox->AppendText( pszMsg, g_dwBLACK );

	pCtrl = Find( IID_SCROLLBAR );
	assert( pCtrl );
	if( pCtrl == NULL ) return;
	assert( pCtrl->GetControlType() == CTRL_SCROLLBAR );
	if( pCtrl->GetControlType() != CTRL_SCROLLBAR ) return;

	CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
	pScrollBar->SetValue( pListBox->GetMaximum() );
}

void CPrivateChatDlg::RecvChatMsg(DWORD dwUserTag, const char* pszMsg )
{
	assert( m_dwUserTag == dwUserTag );
	if( m_dwUserTag != dwUserTag ) return;///같은창에 2명이 중복될수는 없다.

	assert( pszMsg );
	if( pszMsg == NULL ) return;
	
	std::string strMsg = m_strName;
	strMsg.append("> ");
	strMsg.append( pszMsg );

	AddChatMsg( strMsg.c_str() );
}

void CPrivateChatDlg::Hide()
{
	CTDialog::Hide();
	CTCmdClosePrivateChatDlg* pCmd = new CTCmdClosePrivateChatDlg( m_dwUserTag );
	g_itMGR.AddTCommand( DLG_TYPE_MAX, pCmd );
}

unsigned CPrivateChatDlg::ActionPerformed( CActionEvent* e )
{
	assert( e );
	if( e == NULL ) return 0;

	switch( e->GetID() )
	{
	case WM_KEYDOWN:
		{
			if( e->GetWParam() == VK_RETURN )
			{
				CWinCtrl* pCtrl = e->GetSource();
				assert( pCtrl );
				if( pCtrl && pCtrl->IsFocus() && pCtrl->GetControlType() == CTRL_EDITBOX )
				{
					SendChatMsg();
					return pCtrl->GetControlID();
				}
			}
		}
		break;
	default:
		break;
	}
	return 0;
}