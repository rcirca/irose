#include "stdafx.h"
#include ".\cchatroomdlg.h"
#include "../../Game.h"
#include "../../GameData/CChatRoom.h"
#include "../../Network/CNetwork.h"
#include "../it_mgr.h"


#include "TScrollBar.h"
#include "TEditBox.h"
#include "TListBox.h"
#include "JContainer.h"
#include "TabbedPane.h"
#include "TComboBox.h"
#include "TPane.h"
#include "ActionEvent.h"

const POINT c_ptDrawMemberName[] = { {  18, 60 }, { 18, 80 } ,{ 18, 100 }, { 18, 120 },
									 { 105, 60 }, {105, 80 }, {105, 100 }, {105, 120 }
									};

CChatRoomDlg::CChatRoomDlg( int iDlgType )
{
	SetDialogType( iDlgType );
	m_iTab	= TAB_MEMBER;
	m_bMinimize = false;
}

CChatRoomDlg::~CChatRoomDlg(void)
{
}

unsigned CChatRoomDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision())  return 0;


	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam,lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown( uiProcID, wParam, lParam );
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}


void CChatRoomDlg::AddChatMsg( const char* pszMsg, DWORD dwColor )
{
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_LIST );
	if( pCtrl )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		if( pListBox->GetValue() >= pListBox->GetMaximum() - pListBox->GetExtent() )
		{
			pListBox->AppendText( pszMsg, dwColor );
			pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_SCROLLBAR );
			if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
			{
				CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
				pScrollBar->SetValue( pListBox->GetMaximum() );
			}
		}
		else
		{
			pListBox->AppendText( pszMsg, dwColor, false );
		}
	}
}
void CChatRoomDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_MINIMIZE:
		Minimize();
		break;
	case IID_BTN_MAXIMIZE:
		Maximize();
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_MAKE:
		{
			CWinCtrl* pCtrl = FindChildInPane( IID_PANE_TOP, IID_TABBEDPANE );

			assert( pCtrl );
			if( pCtrl == NULL ) return;
			
			assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
			if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return;

			CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

			CJContainer* pContainer = pTabbedPane->GetTabContainer( TAB_CONFIG );
			assert( pContainer );
			if( pContainer == NULL ) return;
			
			pCtrl = pContainer->Find( IID_EDITBOX_TITLE );
			assert( pCtrl );
			if( pCtrl == NULL ) return;
			
			assert( pCtrl->GetControlType() == CTRL_EDITBOX );
			if( pCtrl->GetControlType() != CTRL_EDITBOX ) return;
			
			CTEditBox* pTitleEditBox = (CTEditBox*) pCtrl;
			char* pszTitle = pTitleEditBox->get_text();

			if( pszTitle && strlen( pszTitle ) > 0 )
			{
				pCtrl = pContainer->Find( IID_EDITBOX_PWD );
				assert( pCtrl );
				if( pCtrl == NULL ) return;
				
				assert( pCtrl->GetControlType() == CTRL_EDITBOX );
				if( pCtrl->GetControlType() != CTRL_EDITBOX ) return;
				
				CTEditBox* pTitleEditBox = (CTEditBox*) pCtrl;
				char* pszPwd = pTitleEditBox->get_text();
				if( pszPwd && strlen( pszPwd ) < 1 )
					pszPwd = NULL;

#pragma message( "현재 모든 채팅방은 공개방으로한다.")
				pszPwd = NULL;///현재 모든 채팅방은 공개방

				pCtrl = pContainer->Find( IID_COMBOBOX_MAXUSER );
				assert( pCtrl );
				if( pCtrl == NULL ) return;
				assert( pCtrl->GetControlType() == CTRL_COMBOBOX );
				if( pCtrl->GetControlType() != CTRL_COMBOBOX ) return;

				CTComboBox* pComboBox = (CTComboBox*)pCtrl;
				const char* pszValue = pComboBox->GetSelectedItem();
				int iMaxUserCount = 8;
				if( pszValue )
					iMaxUserCount = atoi( pszValue );

				CChatRoom::GetInstance().SendReqMakeRoom( 0, iMaxUserCount, pszTitle, pszPwd );
			}
			break;
		}
	default:
		break;
	}
}
bool CChatRoomDlg::Create( const char* IDD )
{
	if( !CTDialog::Create( IDD )) return false;
		
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_TOP,  IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		
		///VIDEO
		CJContainer* pContainer = pPane->GetTabContainer( TAB_CONFIG );
		assert( pContainer );
		pCtrl = pContainer->Find( IID_COMBOBOX_MAXUSER );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			for( int i = 2; i <= 8; ++i )
				pComboBox->AddItem( CStr::Printf("%d",i ));
		}


		pCtrl = pContainer->Find( IID_COMBOBOX_TYPE );
		if( pCtrl && pCtrl->GetControlType() == CTRL_COMBOBOX )
		{
			CTComboBox* pComboBox = (CTComboBox*)pCtrl;
			for( int i = 1; i <= 2; ++i )
				pComboBox->AddItem( CStr::Printf("%d",i ));
		}
	}

	pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_EDIT );
	if( pCtrl->GetControlType() == CTRL_EDITBOX )
		pCtrl->AddActionListener( this );	

	CChatRoom::GetInstance().AddObserver( this );
	return true;
}

void CChatRoomDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );
	//멤버 아이디 그리기

	const char* pszTitle = CChatRoom::GetInstance().GetTitle();
	if( pszTitle )
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], 
				true, 
				40,
				8,
				g_dwBLUE,  
				pszTitle );		

	if( m_iTab == TAB_MEMBER && !m_bMinimize)
	{
	
		std::list< CChatMember >::iterator iter;
		int i = 0;
		for( iter = m_Members.begin(); iter != m_Members.end(); ++iter,++i )
		{
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], 
				true, 
				c_ptDrawMemberName[i].x,
				c_ptDrawMemberName[i].y,
				g_dwBLACK,  
				iter->GetName() );		
		}
	}
}

void CChatRoomDlg::OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_TABBUTTON_MEMBER:
		m_iTab = TAB_MEMBER;
		break;
	case IID_TABBUTTON_CONFIG:
		m_iTab = TAB_CONFIG;
		break;
	default:
		break;
	}
}

void CChatRoomDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObj );
	if( pObj == NULL ) return;

	assert( strcmp( pObj->toString(),"ChatRoom" ) == 0 );
	if( strcmp( pObj->toString(),"ChatRoom" ) ) return;

	CTEventChatRoom* pEvent = (CTEventChatRoom*)pObj;
	
	int iServerIdx = pEvent->GetServerIdx();
	switch( pEvent->GetID() )
	{
	case CTEventChatRoom::EID_JOIN_MEMBER:
		{
			CChatMember Member;
			Member.SetName( pEvent->GetName() );
			Member.SetServerIdx( iServerIdx );
			m_Members.push_back( Member );
			break;
		}
	case CTEventChatRoom::EID_LEAVE_MEMBER:
		{
			std::list< CChatMember >::iterator iter;
			for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
			{
				if( iter->GetServerIdx() == iServerIdx )
				{
					m_Members.erase( iter );
					break;
				}
			}
			break;
		}
	case CTEventChatRoom::EID_SET_MASTER:
		{
			std::list< CChatMember >::iterator iter;
			for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
			{
				if( iter->GetServerIdx() == iServerIdx )
				{
					CChatMember Member = *iter;
					m_Members.erase( iter );
					m_Members.push_front( Member );
					break;
				}
			}
			break;
		}
	case CTEventChatRoom::EID_STATE_DEACTIVATED:
		break;
	case CTEventChatRoom::EID_STATE_ACTIVATED:
		{
			Show();
			CWinCtrl* pCtrl = FindChildInPane( IID_PANE_TOP, IID_TABBEDPANE );
			if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
			{
				CTabbedPane* pPane = (CTabbedPane*)pCtrl;
				pPane->SetSelectedIndex( TAB_MEMBER );
				m_iTab = TAB_MEMBER;
			}		
			break;
		}
	default:
		break;
	}
			
}

void CChatRoomDlg::Hide()
{
	CTDialog::Hide();

	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_LIST );
	if( pCtrl )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		pListBox->ClearText();
	}
	CChatRoom::GetInstance().Leave();
}

void CChatRoomDlg::SendChatMsg()
{
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_EDIT );
	
	if( pCtrl->GetControlType() != CTRL_EDITBOX )
		return;

	CTEditBox* pEditBox = (CTEditBox*)pCtrl;

	if( pEditBox->IsFocus() )
	{
		if( CChatRoom::GetInstance().GetState() == CChatRoom::STATE_ACTIVATED )
		{
			char* pszMsg = pEditBox->get_text();
			if( pszMsg && strlen( pszMsg ) >= 1 )
				g_pNet->Send_cli_CHATROOM_MSG( pszMsg );
		}
		else
		{
			AddChatMsg(pEditBox->get_text(), g_dwBLACK );
		}
		pEditBox->clear_text();

		///내가 채팅 메세지를 보내면 항상 리스트는 맨밑으로 이동
		CWinCtrl* pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_LIST);
		if( pCtrl )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			pCtrl = FindChildInPane( IID_PANE_BOTTOM, IID_CHAT_SCROLLBAR );
			if( pCtrl && pCtrl->GetControlType() == CTRL_SCROLLBAR )
			{
				CTScrollBar* pScrollBar = (CTScrollBar*)pCtrl;
				pScrollBar->SetValue( pListBox->GetMaximum() );
			}
		}
	}
}

void CChatRoomDlg::Show()
{
	CTDialog::Show();
	CWinCtrl* pCtrl = FindChildInPane( IID_PANE_TOP, IID_TABBEDPANE );

	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pPane = (CTabbedPane*)pCtrl;
		pPane->SetSelectedIndex( TAB_CONFIG );
		m_iTab = TAB_CONFIG;
	}

	if( m_bMinimize )
	{
		if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_BTN_MINIMIZE ) )
			pCtrl->Hide();
	}
	else
	{
		if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_BTN_MAXIMIZE ) )
			pCtrl->Hide();
	}


}
void CChatRoomDlg::RecvChatMsg( WORD wUserID, const char* pszMsg )
{
	std::list< CChatMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->GetServerIdx() == wUserID )
		{
			AddChatMsg( CStr::Printf( "%s> %s", iter->GetName(), pszMsg ),
				g_dwBLACK );
			break;
		}
	}

}
CWinCtrl* CChatRoomDlg::FindChildInPane( unsigned uiPaneID, unsigned uiChildID )
{
	CWinCtrl* pCtrl = Find( uiPaneID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_PANE )
	{
		CTPane* pPane = (CTPane*)pCtrl;
		return pPane->FindChild( uiChildID );
	}
	return NULL;
}

void CChatRoomDlg::Minimize()
{
	CWinCtrl* pCtrl = NULL;
	m_bMinimize = true;

	if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_BTN_MINIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_BTN_MAXIMIZE ) )
		pCtrl->Show();

	if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_TABBEDPANE ))
		pCtrl->Hide();

	if( pCtrl = Find( IID_PANE_BOTTOM ) )
	{
		pCtrl->SetOffset( 0, 47 );
		SetHeight( pCtrl->GetHeight() + 47 );
	}


	MoveWindow( m_sPosition );
}

void CChatRoomDlg::Maximize()
{
	CWinCtrl* pCtrl = NULL;
	m_bMinimize = false;

	if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_BTN_MINIMIZE ) )
		pCtrl->Show();

	if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_BTN_MAXIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = FindChildInPane( IID_PANE_TOP, IID_TABBEDPANE ))
		pCtrl->Show();

	if( pCtrl = Find( IID_PANE_BOTTOM ) )
	{
		pCtrl->SetOffset( 0, 147 );
		SetHeight( pCtrl->GetHeight() + 147 );
	}

	MoveWindow( m_sPosition );
}

unsigned CChatRoomDlg::ActionPerformed( CActionEvent* e )
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
				if( pCtrl && pCtrl->IsFocus() && pCtrl->GetControlID() == IID_CHAT_EDIT )
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