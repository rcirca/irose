#include "stdafx.h"
#include "CSelectServer.h"
#include "ExternalUILobby.h"

#include "../../Network/CNetwork.h"
#include "Net_Prototype.h"
#include "../../GameData/Event/CTEventServerList.h"
#include "../../GameData/ServerList.h"

#include "../../Game.h"
#include "../../System/CGame.h"

#include "../Command/CTCmdHotExec.h"
#include "../../Debug.h"
#include "tgamectrl/tlistbox.h"
#include "tgamectrl/tmsgbox.h"

CSelectServer::CSelectServer()
{
	m_ServerInfoList.clear();;
	m_Channels.clear();;
	m_bWaitServerReply = false;
}

CSelectServer::~CSelectServer()
{
}

///
/// set observer class
///
void CSelectServer::SetEUIObserver( CExternalUIManager* pObserver )
{
	m_pEUIManager = pObserver;
}

///
/// Message procedure
///
UINT CSelectServer::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	UINT Ret = 0;
	if( !IsVision() || m_bWaitServerReply )
		return Ret;

	if( uiMsg == WM_KEYDOWN )
	{
		switch( wParam )
		{
		case VK_RETURN:
			{
				SendSelectChannel();
				return uiMsg;
				break;
			}
		default:
				break;
		}
	}


	unsigned iProcID;
	if( iProcID = CTDialog::Process( uiMsg, wParam,lParam ) )
	{
		Ret = uiMsg;
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			{
				OnLButtonUp( iProcID );
				break;
			}
		case WM_LBUTTONDBLCLK:
			{
				OnLButtonDblClk( iProcID );
				break;
			}
		case WM_LBUTTONDOWN:
			{
				OnLButtonDown( iProcID );
				break;
			}
		default:
			break;
		}
	}
	return Ret;
}



void CSelectServer::SendSelectServerReq( )
{
	CWinCtrl* pCtrl = Find( IID_LISTBOX_SERVER );

	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;

		if( FindSvrId(pListBox->GetSelectedItemID()) )
		{
			unsigned int uiSelectedServerID = FindSvrId(pListBox->GetSelectedItemID());
			g_pNet->Send_cli_CHANNEL_LIST_REQ( uiSelectedServerID );


			int iSelectItmeIndex = pListBox->GetSelectedItemID();

			if(m_ServerInfoList.size() > 0 && m_ServerInfoList.size() > iSelectItmeIndex )
			{
				CGame::GetInstance().SetServerInfo( m_ServerInfoList[iSelectItmeIndex] );
			}
			
			if( pListBox->GetSelectedItem() )
				g_GameDATA.m_strJoinServerName = pListBox->GetSelectedItem();
		}
	}
}
void CSelectServer::SendReqFirestServerChannelList()
{
	if( CServerList::GetInstance().IsShowChannel() )
	{
		CWinCtrl* pCtrl = Find( IID_LISTBOX_SERVER );

		if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
		{
			CTListBox* pListBox = (CTListBox*)pCtrl;
			if( pListBox->GetMaximum() > 0 )
				g_pNet->Send_cli_CHANNEL_LIST_REQ( FindSvrId(0) );

			if(m_ServerInfoList.size()>0)
			{
				CGame::GetInstance().SetServerInfo( m_ServerInfoList[0] );
			}			
		}
	}
	else
	{
		std::vector< ServerInfo >::reverse_iterator riter;
		for( riter = m_ServerInfoList.rbegin(); riter != m_ServerInfoList.rend() ; ++riter )
		{
			g_pNet->Send_cli_CHANNEL_LIST_REQ( riter->iServerID );
		}
	}
}
void CSelectServer::RecvSelectServer( t_PACKET* recvPacket )
{
	if ( recvPacket->m_lsv_SELECT_SERVER.m_btResult != RESULT_SELECT_SERVER_OK	) 
	{
		switch( recvPacket->m_lsv_SELECT_SERVER.m_btResult )
		{
		case RESULT_SELECT_SERVER_FAILED:
			g_EUILobby.ShowMsgBox( STR_RESULT_SELECT_SERVER_FAILED,CTMsgBox::BT_OK ,true ,GetDialogType()  );
			break;
		case RESULT_SELECT_SERVER_FULL:
			g_EUILobby.ShowMsgBox( STR_RESULT_SELECT_SERVER_FULL,CTMsgBox::BT_OK ,true ,GetDialogType()  );
			break;
		case RESULT_SELECT_SERVER_INVALID_CHANNEL:
			g_EUILobby.ShowMsgBox(STR_RESULT_SELECT_SERVER_INVALID_CHANNEL,CTMsgBox::BT_OK ,true ,GetDialogType()  );
			break;
		case RESULT_SELECT_SERVER_CHANNEL_NOT_ACTIVE:///점검중
			g_EUILobby.ShowMsgBox(STR_RESULT_SELECT_SERVER_CHANNEL_NOT_ACTIVE,CTMsgBox::BT_OK ,true ,GetDialogType()  );
			break;
		case RESUTL_SELECT_SERVER_INVALID_AGE://		0x05	// 연령이 맞지 않는다..
			g_EUILobby.ShowMsgBox(STR_RESUTL_SELECT_SERVER_INVALID_AGE,CTMsgBox::BT_OK ,true ,GetDialogType()  );
			break;
		case RESULT_SELECT_SERVER_NEED_CHARGE:
			g_EUILobby.ShowMsgBox( STR_RESULT_SELECT_SERVER_NEED_CHARGE, CTMsgBox::BT_OK ,true ,GetDialogType());
			break;
		default:
			break;
		}
		m_bWaitServerReply = false;
    }
}

void CSelectServer::OnLButtonUp( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_BTN_OK:
		SendSelectChannel();
		break;
	case IID_BTN_CANCEL:
		g_pCApp->SetExitGame();
		break;
	default:
		break;
	}
}

void CSelectServer::OnLButtonDblClk( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_LISTBOX_SERVER:
		{
			SendSelectServerReq();
			break;
		}
	case IID_LISTBOX_CHANNEL:
		{
			SendSelectChannel();
			break;
		}
	default:
		break;
	}
}

unsigned int CSelectServer::FindSvrId( short ItemID )
{
	if( ItemID >= (short)m_ServerInfoList.size() || ItemID < 0 )
		return 0;

	return m_ServerInfoList[ ItemID ].iServerID;
}

ServerInfo* CSelectServer::GetServerInfo( int iIndex )
{ 
	if( iIndex >= (int)m_ServerInfoList.size() )
		return NULL;

	return &m_ServerInfoList[ iIndex ]; 
}

void CSelectServer::Show()
{

	POINT pt = { g_pCApp->GetWIDTH() - GetWidth() - 30,
		100 };

	MoveWindow( pt );

	CTDialog::Show();
	CWinCtrl* pCtrl = Find( IID_LISTBOX_SERVER );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		pListBox->SetSelectedItem( 0 );
	}

	pCtrl = Find( IID_LISTBOX_CHANNEL );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		pListBox->SetValue(0);
		pListBox->SetSelectedItem( 0 );
	}
	m_bWaitServerReply = false;
}

void CSelectServer::MsgBoxProc(  unsigned int iMsgType, short int iButton )
{
	switch(iMsgType )
	{
	case MSG_TYPE_EXIT:
		g_pCApp->SetExitGame();
		break;
	case MSG_TYPE_NORMAL:
		g_EUILobby.HideMsgBox();
		break;
	default:
		break;
	}
}

int  CSelectServer::FindChannelNo( int iIndex )
{
	if( iIndex >= (short)m_Channels.size() || iIndex < 0 )
		return 0;

	return m_Channels[ iIndex ].m_btChannelNO;

}


void CSelectServer::SendSelectChannel()
{
	CWinCtrl* pCtrl = Find( IID_LISTBOX_SERVER );
	assert( pCtrl );
	if( pCtrl == NULL )
		return;
	
	CTListBox* pListBox = (CTListBox*)pCtrl;
	DWORD dwServerID = FindSvrId(pListBox->GetSelectedItemID());

	pCtrl = Find( IID_LISTBOX_CHANNEL );
	if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
	{
		CTListBox* pListBox = (CTListBox*)pCtrl;
		int iChannelNo = FindChannelNo( pListBox->GetSelectedItemID());
		assert( iChannelNo );
		if( iChannelNo )
		{
			CTCommand* pCmd = new CTCmdExit;
			g_EUILobby.ShowMsgBox( STR_RECEIVE_CHARACTER_LIST,CTMsgBox::BT_CANCEL ,true ,GetDialogType() ,NULL, pCmd);
			g_pNet->Send_cli_SELECT_SERVER( dwServerID, iChannelNo );
			CGame::GetInstance().SetJoinChannelNo( iChannelNo );
			CGame::GetInstance().SetSelectedServerID( dwServerID );
			CGame::GetInstance().ClearWndMsgQ();
			m_bWaitServerReply = true;
			if( pListBox->GetSelectedItem() )
				g_GameDATA.m_strJoinChannelName = pListBox->GetSelectedItem();
		}
	}
	else
	{
		if( !m_Channels.empty() )
		{
			int iChannelNo = m_Channels.begin()->m_btChannelNO;
			CTCommand* pCmd = new CTCmdExit;
			g_EUILobby.ShowMsgBox( STR_RECEIVE_CHARACTER_LIST,CTMsgBox::BT_CANCEL ,true ,GetDialogType() ,NULL, pCmd);
			g_pNet->Send_cli_SELECT_SERVER( dwServerID, iChannelNo );
			CGame::GetInstance().SetSelectedServerID( dwServerID );
			CGame::GetInstance().SetJoinChannelNo( iChannelNo );
			CGame::GetInstance().ClearWndMsgQ();
			m_bWaitServerReply = true;
			g_GameDATA.m_strJoinChannelName = "NOT-CHANNEL";
		}
	}
}



void CSelectServer::OnLButtonDown( unsigned iProcID )
{
	switch( iProcID )
	{
	case IID_LISTBOX_SERVER:
		{
			SendSelectServerReq();
			break;
		}
	default:
		break;
	}
}

void CSelectServer::Hide()
{
	CTDialog::Hide();
	m_bWaitServerReply = false;
}


void CSelectServer::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );

	if( pObj == NULL || strcmp( pObj->toString(), "SVR_LIST" ) )
	{
		assert( 0 && "Invalid Event Type or Null" );
		return;
	}

	CTEventServerList* pEvent = (CTEventServerList*)pObj;

	switch( pEvent->GetID() )
	{
	case CTEventServerList::EID_CLEAR_WORLDSERVER_LIST:
		{
			m_ServerInfoList.clear();
			CWinCtrl* pCtrl = Find( IID_LISTBOX_SERVER );
			if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pListBox->ClearText();
			}

		}
		break;
	case CTEventServerList::EID_CLEAR_CHANNELSERVER_LIST:
		{
			m_Channels.clear();
			CWinCtrl* pCtrl = Find( IID_LISTBOX_CHANNEL );
			if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pListBox->ClearText();
			}
		}
		break;
	case CTEventServerList::EID_ADD_WORLDSERVER:
		{
			WORLDSERVER_INFO worldserver_info = pEvent->GetWorldServerInfo();
			ServerInfo info;
			info.strServerName = std::string( worldserver_info.m_world_name.c_str() );
			info.iServerID = worldserver_info.m_world_id;

			m_ServerInfoList.push_back( info );
			
			CWinCtrl* pCtrl = Find( IID_LISTBOX_SERVER );
			if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
			{
				CTListBox* pListBox = (CTListBox*)pCtrl;
				pListBox->AppendText( info.strServerName.c_str() ,D3DCOLOR_RGBA(255,255,255,255) );
				pListBox->SetValue(0);
				pListBox->SetSelectedItem(0);
			}

		}
		break;
	case CTEventServerList::EID_ADD_CHANNELSERVER:
		{
			CHANNELSERVER_INFO info = pEvent->GetChannelServerInfo();
			ChannelInfo Channel;
			Channel.m_btChannelNO	= info.m_channel_id;
			Channel.m_btHighAGE		= 0;
			Channel.m_btLowAGE		= 0;
			m_Channels.push_back( Channel );

			CWinCtrl* pCtrl = Find( IID_LISTBOX_CHANNEL );
			if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
			{
				DWORD dwColor = 0;
				std::string	strTemp( info.m_channel_name );

				CTListBox* pListBox = (CTListBox*)pCtrl;
				if( info.m_user_percent <= 20 )
				{
					dwColor = g_dwGREEN;
					strTemp.append(STR_SERVERSTATE_SMOOTH);
				}
				else if( info.m_user_percent <= 60 )
				{
					dwColor = g_dwGREEN;
					strTemp.append(STR_SERVERSTATE_NORMAL);
				}
				else if( info.m_user_percent <= 99 )
				{
					dwColor = g_dwYELLOW;
					strTemp.append(STR_SERVERSTATE_CONFUSION);
				}
				else 
				{
					dwColor = g_dwRED;
					strTemp.append(STR_SERVERSTATE_EXCESS);
				}
				pListBox->AppendText( strTemp.c_str() , dwColor);
			}
			else
			{
				std::vector< ServerInfo >::iterator iter;
				int index = 0;
				for( iter = m_ServerInfoList.begin(); iter != m_ServerInfoList.end(); ++iter,++index )
				{
					if( iter->iServerID == info.m_worldserver_id )
					{
						pCtrl = Find( IID_LISTBOX_SERVER );
						if( pCtrl && pCtrl->GetControlType() == CTRL_LISTBOX )
						{
							DWORD dwColor = 0;
							std::string	strTemp( iter->strServerName.c_str() );

							CTListBox* pListBox = (CTListBox*)pCtrl;
							if( info.m_user_percent <= 20 )///대만 요청으로 25->20으로 수정 : 
							{
								dwColor = g_dwGREEN;
								strTemp.append(STR_SERVERSTATE_SMOOTH);
							}
							else if( info.m_user_percent <= 60 )
							{
								dwColor = g_dwGREEN;
								strTemp.append(STR_SERVERSTATE_NORMAL);
							}
							else if( info.m_user_percent <= 99 )
							{
								dwColor = g_dwYELLOW;
								strTemp.append(STR_SERVERSTATE_CONFUSION);
							}
							else 
							{
								dwColor = g_dwRED;
								strTemp.append(STR_SERVERSTATE_EXCESS);
							}
							pListBox->SetText(index, strTemp.c_str() , dwColor);
						}
						break;
					}
				}
			}
		}
		break;
	default:
		break;
	}

}