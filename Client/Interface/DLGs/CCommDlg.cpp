#include "stdafx.h"
#include ".\ccommdlg.h"
#include "../it_mgr.h"
#include "../../Network/CNetwork.h"
#include "subclass/CFriendListItem.h"
#include "subclass/CMailBoxListItem.h"
#include "subclass/CChatRoomListItem.h"
#include "../Command/UICommand.h"
#include "../../SqliteDB.h"
#include "../../Country.h"
#include "../../System/CGame.h"

#include "tgamectrl/tabbedpane.h"
#include "tgamectrl/jcontainer.h"
#include "tgamectrl/zlistbox.h"

CCommDlg::CCommDlg( int iDlgType )
{
	SetDialogType( iDlgType );
}

CCommDlg::~CCommDlg(void)
{

}


unsigned CCommDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;



	int iCurrSelectTab = 0;
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_TABBEDPANE )
	{
		CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;
		iCurrSelectTab = pTabbedPane->GetSelectedIndex();
	}

	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:
			{
				if( uiProcID == IID_TABBUTTON_FRIEND )
				{
					if( CCountry::GetSingleton().IsJapan() )
					{
						if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
						{
							CWinCtrl* pCtrl = Find( IID_TABBEDPANE );

							assert( pCtrl );
							if( pCtrl == NULL ) return 0;
							
							assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
							if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return 0;

							CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

							pTabbedPane->SetSelectedIndex( iCurrSelectTab );

							g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_FREINDLIST, CMsgBox::BT_OK, false );
						}
					}
				}						
				else if( uiProcID == IID_TABBUTTON_CHATROOM )
				{
					if( CCountry::GetSingleton().IsJapan() )
					{
						if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
						{
							CWinCtrl* pCtrl = Find( IID_TABBEDPANE );

							assert( pCtrl );
							if( pCtrl == NULL ) return 0;

							assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
							if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return 0;

							CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

							pTabbedPane->SetSelectedIndex( iCurrSelectTab );

							g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_CHATROOM, CMsgBox::BT_OK, false );
						}
					}
				}
			}
			break;
		default:

			break;
		}
		return uiMsg;
	}
	return 0;
}
void CCommDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_REMOVEFRIEND:
		{
			CZListBox* pZListBox = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
			assert( pZListBox );
			if( pZListBox && pZListBox->GetSelectedItemIndex() >= 0 )
			{
				CWinCtrl* pCtrl = pZListBox->GetItem( pZListBox->GetSelectedItemIndex() );
				if( pCtrl )
				{
					CFriendListItem* pItem = (CFriendListItem*)pCtrl;
					CTCmdRemoveFriend* pCmd = new CTCmdRemoveFriend( pItem->GetUserTag() );

					g_itMGR.OpenMsgBox( CStr::Printf(F_STR_QUERY_DELETE_FRIEND, pItem->GetName() ),
						CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd, NULL );
				}
			}
			break;
		}
	case IID_BTN_ADDFRIEND:
		g_itMGR.OpenDialog( DLG_TYPE_ADDFRIEND );
		//g_itMGR.OpenDialog( DLG_TYPE_MEMO );
		break;
	case IID_BTN_CREATECHATROOM:
		if( !g_itMGR.IsDlgOpened( DLG_TYPE_CHATROOM ) )
			g_itMGR.OpenDialog( DLG_TYPE_CHATROOM );
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	default:
		break;
	}
}
void CCommDlg::AddFriend( DWORD dwUserTag, BYTE btStatus, const char* pszName )
{
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return;

	CFriendListItem* pItem = new CFriendListItem( dwUserTag, btStatus, pszName );
	pZlist->Add( pItem );

}
void CCommDlg::ClearFriendList()
{
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return;

	pZlist->Clear();

}
void CCommDlg::ChangeFriendStatus( DWORD dwUserTag, BYTE btStatus )
{
	CWinCtrl*	pCtrl = NULL;	
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return;

	CFriendListItem* pItem = NULL;
	for( int i = 0; i < pZlist->GetSize(); ++i )
	{
		pCtrl = pZlist->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = ( CFriendListItem* )pCtrl;
			if( pItem->GetUserTag() == dwUserTag )
			{
				pItem->SetStatus( btStatus );
				break;
			}
		}
	}
}

CZListBox* CCommDlg::GetZListBox( int iTab, int iListID )
{
	CWinCtrl* pCtrl = Find( IID_TABBEDPANE );

	assert( pCtrl );
	if( pCtrl == NULL ) return NULL;
	
	assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
	if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return NULL;

	CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

	CJContainer* pContainer = pTabbedPane->GetTabContainer( iTab );
	assert( pContainer );
	if( pContainer == NULL ) return NULL;
	
	pCtrl = pContainer->Find( iListID );
	assert( pCtrl );
	if( pCtrl == NULL ) return NULL;
	
	assert( pCtrl->GetControlType() == CTRL_ZLISTBOX );
	if( pCtrl->GetControlType() != CTRL_ZLISTBOX ) return NULL;
	
	return (CZListBox*)pCtrl;
}

void CCommDlg::RemoveFriend( DWORD dwUserTag )
{
	CWinCtrl*	pCtrl = NULL;	
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return;

	CFriendListItem* pItem = NULL;
	for( int i = 0; i < pZlist->GetSize(); ++i )
	{
		pCtrl = pZlist->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = ( CFriendListItem* )pCtrl;
			if( pItem->GetUserTag() == dwUserTag )
			{
				pZlist->DelItem( i );
				//pItem->SetStatus( btStatus );
				break;
			}
		}
	}
}

CFriendListItem* CCommDlg::FindFriend( DWORD dwUserTag )
{
	CWinCtrl*	pCtrl = NULL;	
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return NULL;

	CFriendListItem* pItem;
	for( int i = 0; i < pZlist->GetSize(); ++i )
	{
		pCtrl = pZlist->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = (CFriendListItem*)pCtrl;
			if( pItem->GetUserTag() == dwUserTag )
				return pItem;
		}
	}
	return NULL;
}
CFriendListItem* CCommDlg::FindFriendByName( const char* pszName )
{
	assert( pszName );
	if( pszName == NULL ) return NULL;

	CWinCtrl*	pCtrl = NULL;	
	CZListBox* pZlist = GetZListBox( TAB_FRIEND, IID_ZLIST_FRIEND );
	assert( pZlist );
	if( pZlist == NULL ) return NULL;

	CFriendListItem* pItem;
	for( int i = 0; i < pZlist->GetSize(); ++i )
	{
		pCtrl = pZlist->GetItem( i );
		assert( pCtrl );
		if( pCtrl )
		{
			pItem = (CFriendListItem*)pCtrl;
			if( strcmpi( pszName, pItem->GetName() ) == 0 )
				return pItem;
		}
	}
	return NULL;
}

void CCommDlg::Show()
{
	CTDialog::Show();
	
	LoadMemoFromDB();

	ClearChatRoomList();
	//g_pNet->Send_cli_CHAT_ROOM_LIST( 0, 0 );
	if( CCountry::GetSingleton().IsJapan( ) )
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
		{
			CWinCtrl* pCtrl = Find( IID_TABBEDPANE );

			assert( pCtrl );
			if( pCtrl == NULL ) return;
			
			assert( pCtrl->GetControlType() == CTRL_TABBEDPANE );
			if( pCtrl->GetControlType() != CTRL_TABBEDPANE ) return;

			CTabbedPane* pTabbedPane = (CTabbedPane*)pCtrl;

			pTabbedPane->SetSelectedIndex( TAB_MAILBOX );
		}        
	}
}

void CCommDlg::ClearMemoList()
{
	CZListBox* pZlist = GetZListBox( TAB_MAILBOX, IID_ZLIST_MAILBOX );
	assert( pZlist );
	if( pZlist == NULL ) return;

	pZlist->Clear();
}

void CCommDlg::ClearChatRoomList()
{
	CZListBox* pZlist = GetZListBox( TAB_CHATROOM, IID_ZLIST_CHATROOM );
	assert( pZlist );
	if( pZlist == NULL ) return;

	pZlist->Clear();
}

void CCommDlg::AddMemo( int iRowid, const char* pszFromName, const char* pszMemo , DWORD dwRecvTime )
{
	CZListBox* pZlist = GetZListBox( TAB_MAILBOX, IID_ZLIST_MAILBOX );
	assert( pZlist );
	if( pZlist == NULL ) return;


	CMailBoxListItem* pItem = new CMailBoxListItem( pszFromName, pszMemo, dwRecvTime  );
	pZlist->Add( pItem );
	pItem->SetControlID( iRowid );
}

void CCommDlg::DeleteMemo( int iIndex )
{
	CZListBox* pZlist = GetZListBox( TAB_MAILBOX, IID_ZLIST_MAILBOX );
	assert( pZlist );
	if( pZlist == NULL ) return;

	pZlist->DelItemByControlID( iIndex );
}
void CCommDlg::AddChatRoom( const char* pszTitle, BYTE btRoomType, short nRoomID, BYTE btUserCount )
{
	CZListBox* pZList = GetZListBox( TAB_CHATROOM, IID_ZLIST_CHATROOM );
	assert( pZList );
	if( pZList == NULL ) return;

	CChatRoomListItem* pItem = NULL;
	pItem = new CChatRoomListItem( nRoomID, btRoomType, btUserCount, pszTitle );
	pZList->Add( pItem );

}

void CCommDlg::LoadMemoFromDB()
{
	ClearMemoList();
	CSqliteDB::GetInstance().LoadMemo( g_pAVATAR->Get_NAME(), callback );
}

int CCommDlg::callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	assert( argc == 4 );
	if( argc == 4 )
	{
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY ) )
		{
			CCommDlg* pCommDlg = (CCommDlg*)pDlg;
			pCommDlg->AddMemo( atoi(argv[0]), argv[1], argv[2], atoi( argv[3] ) );
		}
	}
	return 0;
}