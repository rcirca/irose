/*
$Header: /Client/Network/RecvPACKET.cpp 690   05-10-27 10:18a Choo0219 $
*/
#include "stdAFX.h"
#include "CNetwork.h"
#include "Game.h"
#include "CCamera.h"
#include "IO_Terrain.h"
#include "Game_FUNC.h"
#include "JCommandState.h"	

#include "../CClientStorage.h"
#include "../CJustModelAVT.h"
#include "../SqliteDB.h"
#include "../Country.h"

#include "../System/CGame.h"
#include "../GameData/ServerList.h"
#include "../GameData/CClan.h"
#include "../GameData/CBank.h"
#include "../GameData/CParty.h"
#include "../GameData/CSeparate.h"
#include "../GameData/CGameDataCreateAvatar.h"
#include "../GameData/CUpgrade.h"
#include "../GameCommon/Skill.h"
#include "../Common/IO_STB.h"
#include "../Common/IO_Quest.h"
#include "../common/Calculation.h"
#include "../Util/CCheat.h"
#include "../GameProc/CDayNNightProc.h"

#include "../Interface/it_MGR.h"
#include "../Interface/TypeResource.h"	
#include "../Interface/CHelpMgr.h"
#include "../Interface/CUIMediator.h"
#include "../Interface/ExternalUI/CExternalUI.h"
#include "../Interface/ExternalUI/CLogin.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../Interface/ExternalUI/CSelectChannel.h"
#include "../Interface/ClanMarkTransfer.h"
#include "../Interface/ClanMarkManager.h"

#include "../Interface/Dlgs/DeliveryStoreDlg.h"
#include "../Interface/Dlgs/ChattingDlg.h"
#include "../interface/dlgs/CQuestDlg.h"
#include "../interface/dlgs/CPartyDlg.h"
#include "../interface/dlgs/DealDlg.h"
#include "../interface/dlgs/MakeDlg.h"
#include "../interface/dlgs/CSystemMsgDlg.h"
#include "../interface/dlgs/ExchangeDlg.h"
#include "../interface/dlgs/CCommDlg.h"
#include "../interface/icon/ciconitem.h"


#include "../interface/command/CTCmdHotExec.h"
#include "../interface/command/CTCmdNumberInput.h"
#include "../interface/command/UICommand.h"

#include "../Util/LogWnd.h"

#include "../Event/QuestRewardQueue.h"
#include "../ObjFixedEvent.h"

#include "../Event/Quest_Func.h"
#include "GameProc/LiveCheck.h"
#include "GameProc/DelayedExp.h"

#include "../Tutorial/TutorialEventManager.h"
//#include "../nProtect/NProtect.h"
#include "../GameProc/UseItemDelay.h"

#include "triggerinfo/triggerinfo.h"

// ¼­¹ö¿¡¼­ ¹ÞÀº Àü¿ª ¹ø¼öµé ¼³Á¤...
void SetServerVAR( tagVAR_GLOBAL *pVAR )
{
	// short	m_nWorld_PRODUCT;					// Á¦Á¶ °ü·Ã
	// DWORD	m_dwUpdateTIME;						// °»½ÅµÈ ½Ã°£.
	// short	m_nWorld_RATE;						// °æÁ¦ °ü·Ã :: ¼¼°è¹°°¡ 80~140
	// BYTE		m_btTOWN_RATE;						// ¸¶À» ¹°°¡					80~140
	// BYTE		m_btItemRATE[ MAX_PRICE_TYPE ];		// ¾ÆÀÌÅÛº° ¹°°¡				1~127

	::Set_WorldPROD( pVAR->m_nWorld_PRODUCT );
	::Set_WorldRATE( pVAR->m_nWorld_RATE );

	if ( g_pTerrain ) {
		g_pTerrain->m_Economy.m_dwUpdateTIME = pVAR->m_dwUpdateTIME;
		g_pTerrain->m_Economy.m_btTOWN_RATE = pVAR->m_btTOWN_RATE;
		::CopyMemory( g_pTerrain->m_Economy.m_btItemRATE, pVAR->m_btItemRATE, sizeof(BYTE)*MAX_PRICE_TYPE);
	}

	g_GameDATA.m_iPvPState  = pVAR->m_dwGlobalFLAGS & ZONE_FLAG_PK_ALLOWED;
}



//-------------------------------------------------------------------------------------------------
CRecvPACKET::CRecvPACKET ()
{
	m_pRecvPacket = (t_PACKET*) new char[ MAX_PACKET_SIZE ];
}
CRecvPACKET::~CRecvPACKET()
{
	SAFE_DELETE_ARRAY( m_pRecvPacket );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_srv_ERROR ()
{
	switch( m_pRecvPacket->m_lsv_ERROR.m_wErrorCODE ) 
	{
	case 0:
		break;
	default :
		;
	} 
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_GM_COMMAND ()
{
	switch( m_pRecvPacket->m_gsv_GM_COMMAND.m_btCMD ) 
	{
	case GM_CMD_SHUT   :
		{
			short nOffset=sizeof( gsv_GM_COMMAND );

			DWORD dwBlockTime = m_pRecvPacket->m_gsv_GM_COMMAND.m_wBlockTIME;
			DWORD dwBlockTimeMilSecond = dwBlockTime * 60 * 1000;

			char *szName = Packet_GetStringPtr ( m_pRecvPacket, nOffset );
			char szTemp[128];
			///Ã¤ÆÃ ºí·Ï ÇØÁ¦
			if( dwBlockTime <= 0 )
			{
				if( szName )
					sprintf( szTemp,FORMAT_STR_CANCEL_BLOCK_CHAT, szName);
				else
					sprintf( szTemp,STR_CANCEL_BLOCK_CHAT);

				g_itMGR.OpenMsgBox(szTemp);
				CChatDLG* pDlg = g_itMGR.GetChatDLG();
				if( pDlg )
					pDlg->SetChatUnBlock();

			}
			else///Ã¤ÆÃ ºí·Ï
			{
				if( szName )
					sprintf( szTemp,F_STR2_CHAT_BLOCKED_BY_GM, szName ,dwBlockTime );
				else
					sprintf( szTemp,FORMAT_STR_BLOCK_CHAT,dwBlockTime );

				g_itMGR.OpenMsgBox(szTemp);
				CChatDLG* pDlg = g_itMGR.GetChatDLG();
				if( pDlg )
					pDlg->SetChatBlock( dwBlockTimeMilSecond );
			}					
			break;
		}
	case GM_CMD_LOGOUT :
		{
			CTCmdExit Command;
			Command.Exec( NULL );
			//CTCommand* pCmd = new CTCmdExit;
			//short nOffset=sizeof( gsv_GM_COMMAND );
			//char *szName = Packet_GetStringPtr ( m_pRecvPacket, nOffset );
			//char szTemp[128];
			//if( szName )
			//	sprintf( szTemp,FORMAT_STR_GM_BAN_USER, szName );
			//else
			//	sprintf( szTemp,STR_GM_BAN_USER);
			//
			//g_itMGR.OpenMsgBox(szTemp,CMsgBox::BT_OK, true, 0, pCmd ,NULL);
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_GLOBAL_VAR ()
{
	SetServerVAR( (tagVAR_GLOBAL*)&m_pRecvPacket->m_pDATA[ sizeof(t_PACKETHEADER) ] );

	g_itMGR.AppendChatMsg(STR_CHANGE_PRICES, IT_MGR::CHAT_TYPE_SYSTEM );

	if( CDealData::GetInstance().GetTradeItemCnt() )
	{
		CTCmdCloseStore* pCmd = new CTCmdCloseStore;
		char szTemp[256];
		sprintf( szTemp,"%s %s",STR_CHANGE_PRICES,STR_QUERY_STOP_TRADE );
		g_itMGR.OpenMsgBox(szTemp , CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd , NULL );
	}

}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_GLOVAL_FLAG ()
{
	g_GameDATA.m_iPvPState = m_pRecvPacket->m_gsv_SET_GLOBAL_FLAG.m_dwGlobalFLAGS & ZONE_FLAG_PK_ALLOWED;
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_srv_ANNOUNCE_TEXT ()
{
#pragma message ( "TODO:: ¹ÞÀº °øÁö »çÇ× Ã³¸®..." __FILE__ )
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ANNOUNCE_CHAT ()
{
	short nOffset=sizeof( t_PACKETHEADER );
	char *szMSG = NULL;
	szMSG = CStr::Printf("%s", Packet_GetStringPtr( m_pRecvPacket, nOffset ) );
	char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );			

	if( g_Cheat.DoSpecialCheat( szMSG ) )
		return;

	if( szMSG[ 0 ] == '/' )
		return;

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_SYSTEMMSG );
	if( pDlg )
	{
		std::string Title = STR_NOTIFY_03;
		if( szName )
		{
			Title.append(":");				
			Title.append( szName );
		}
		CSystemMsgDlg* pSysMsgDlg = (CSystemMsgDlg*) pDlg;
		pSysMsgDlg->SetMessage( Title.c_str(),szMSG, CSystemMsgDlg::MSG_TYPE_NOTICE, g_dwWHITE, 15000 , g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ]);

		std::string ChatMsg = STR_NOTIFY_03;
		if( szName )///Ãß°¡
		{
			ChatMsg.append(":");
			ChatMsg.append( szName );
			ChatMsg.append(" ");
		}			

		ChatMsg.append( szMSG );
		g_itMGR.AppendChatMsg( ChatMsg.c_str(), IT_MGR::CHAT_TYPE_NOTICE );
	}
}

//-------------------------------------------------------------------------------------------------
bool CRecvPACKET::Recv_lsv_LOGIN_REPLY ()
{
	CLogin* pLogin = (CLogin*)g_EUILobby.GetEUI( EUI_LOGIN );

	if( pLogin == NULL )
		return false;

	g_EUILobby.HideMsgBox();


	CServerList& ServerList = CServerList::GetInstance();
	///Ã¤³Î º¸ÀÌ±â, ¼û±â±â ±¸ºÐ
	if( m_pRecvPacket->m_srv_LOGIN_REPLY.m_btResult & 0x80 )
		ServerList.HideChannel();
	else
		ServerList.ShowChannel();

	///Ã¤³Î º¸ÀÌ±â, ¼û±â±â Á¤º¸¸¦ »«»óÅÂ¿¡¼­ 
	BYTE btResult = m_pRecvPacket->m_srv_LOGIN_REPLY.m_btResult & ~0x80;

	if( RESULT_LOGIN_REPLY_OK != btResult 
		&& RESULT_LOGIN_REPLY_TAIWAN_OK != btResult
		&& RESULT_LOGIN_REPLY_JAPAN_OK != btResult )
	{
		switch( btResult ) 
		{
		case RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT:
			g_EUILobby.ShowMsgBox(STR_LOGIN_REPLY_NO_RIGHT_TO_CONNECT, CTMsgBox::BT_OK ,true , pLogin->GetDialogType() );
			return false;
		case RESULT_LOGIN_REPLY_FAILED               :// ¿À·ù
			g_EUILobby.ShowMsgBox(STR_COMMON_ERROR,CTMsgBox::BT_OK ,true , pLogin->GetDialogType() );
			return false;
		case RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT    :// °èÁ¤ ¾ø´Ù.
			g_EUILobby.ShowMsgBox(STR_NOT_FOUND_ACCOUNT,CTMsgBox::BT_OK ,true , pLogin->GetDialogType() );
			return false;
		case RESULT_LOGIN_REPLY_INVALID_PASSWORD     :// ºñ¹ø ¿À·ù
			g_EUILobby.ShowMsgBox(STR_INVALID_PASSWORD,CTMsgBox::BT_OK ,true ,pLogin->GetDialogType());
			return false;
		case RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN     :// ÀÌ¹Ì ·Î±×ÀÎ ÁßÀÌ´Ù
			g_EUILobby.ShowMsgBox(STR_ALREADY_LOGGEDIN,CTMsgBox::BT_OK ,true , pLogin->GetDialogType() );
			return false;
		case RESULT_LOGIN_REPLY_REFUSED_ACCOUNT      :// ¼­¹ö¿¡¼­ °ÅºÎµÈ °èÁ¤ÀÌ´Ù.È¤Àº ºí·°µÈ °èÁ¤ÀÔ´Ï´Ù.
			g_EUILobby.ShowMsgBox(STR_REFUSED_ACCOUNT,CTMsgBox::BT_OK ,true , pLogin->GetDialogType());
			return false;
		case RESULT_LOGIN_REPLY_NEED_CHARGE          :// ÃæÀüÀÌ ÇÊ¿äÇÑ´Ù
			g_EUILobby.ShowMsgBox( STR_BILL_AT_ROSEONLINE_HOMEPAGE	,CTMsgBox::BT_OK ,true ,pLogin->GetDialogType()); //·ÎÁî¿Â¶óÀÎ È¨ÆäÀÌÁö¿¡¼­ °áÁ¦ ½ÅÃ»À» ÇØÁÖ¼¼¿ä~
			return false;		   			
		case RESULT_LOGIN_REPLY_TOO_MANY_USER:
			g_EUILobby.ShowMsgBox(STR_LOGIN_REPLY_TOO_MANY_USER,CTMsgBox::BT_OK ,true ,pLogin->GetDialogType());
			return false;
		case RESULT_LOGIN_REPLY_NO_REAL_NAME:
			g_EUILobby.ShowMsgBox(STR_RESULT_LOGIN_REPLY_NO_REAL_NAME, CTMsgBox::BT_OK ,true ,pLogin->GetDialogType());
			return false;
		case RESULT_LOGIN_REPLY_OUT_OF_IP:
			g_EUILobby.ShowMsgBox( STR_INSUFFICIENCY_IP, CTMsgBox::BT_OK ,true ,pLogin->GetDialogType()); //"Á¢¼Ó °¡´ÉÇÑ ip ¼ö°¡ ÃÊ°ú µÇ¾ú½À´Ï´Ù"
			return false;
		case RESULT_LOGIN_REPLY_TAIWAN_FAILED:
			switch( m_pRecvPacket->m_srv_LOGIN_REPLY.m_wPayType )
			{
			case 7:
				g_EUILobby.ShowMsgBox(STR_NOT_FOUND_ACCOUNT,CTMsgBox::BT_OK ,true , pLogin->GetDialogType() );
				break;
			default:
				g_EUILobby.ShowMsgBox(STR_COMMON_ERROR,CTMsgBox::BT_OK ,true , pLogin->GetDialogType() );
				break;
			}

			return false;
		default:
			g_EUILobby.ShowMsgBox("Login Failed", CTMsgBox::BT_OK ,true ,pLogin->GetDialogType());
			return false;
		}
	}
	short nOffset=sizeof(srv_LOGIN_REPLY);
	char *szServerName;
	DWORD *pServerID;

	szServerName = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	pServerID = (DWORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(DWORD) );

	if ( !szServerName || !pServerID ) 
	{
		CTCommand* pCmd = new CTCmdExit;
		g_EUILobby.ShowMsgBox(STR_INSPECT_ALL_SEVER,CTMsgBox::BT_OK ,true , pLogin->GetDialogType() ,pCmd);
		return false;
	}


	///
	/// Get server list
	///

	CExternalUIManager& refEUIManager = g_EUILobby.GetExternalUIManager();

	//	bool	bSetFirstServerID = false;
	//	int		iFirstServerID  = 0;

	DWORD dwServerID = 0;
	short nServerID=0;
	DWORD dwRight = m_pRecvPacket->m_srv_LOGIN_REPLY.m_wRight ;
	CGame::GetInstance().SetRight( dwRight );
	CGame::GetInstance().SetPayType( m_pRecvPacket->m_srv_LOGIN_REPLY.m_wPayType );

	std::map< BYTE, pair< DWORD, std::string> > TempServerList;




	///ÀÌÀü¿¡ »ç¿ëÇÏ´ø ÀÎÅÍÆäÀÌ½ºÃ¢ÀÌ ÀÖ´Ù¸é Observer¿¡¼­ »èÁ¦ÇÑ´Ù.
	if( CTDialog* pUI = refEUIManager.GetEUI( EUI_SELECT_SERVER ) )
	{
		CSelectServer* pSelectServer = (CSelectServer*)pUI;
		ServerList.DeleteObserver( (IObserver*)pSelectServer );
	}


	//EUI_SELECT_SERVER,
	CSelectServer* pSelectServer = new CSelectServer;
	refEUIManager.AddEUI( EUI_SELECT_SERVER, pSelectServer );
	pSelectServer->SetEUIObserver( &refEUIManager );

	if( CServerList::GetInstance().IsShowChannel() )
		pSelectServer->Create("DlgSelSvr");
	else
		pSelectServer->Create("DlgSelOnlySvr");///Ã¤³ÎÀ» °¨Ãá´Ù.

	POINT	ptNew;
	ptNew.x = g_pCApp->GetWIDTH() / 2 - pSelectServer->GetWidth()/2;
	ptNew.y = g_pCApp->GetHEIGHT() / 3;
	pSelectServer->MoveWindow( ptNew );

	ServerList.AddObserver( pSelectServer );
	ServerList.ClearWorldServerList();

	//CTDialog* pUI = refEUIManager.GetEUI( EUI_SELECT_SERVER );
	//CSelectServer* pSelectServer = (CSelectServer*)pUI;
	//
	//pSelectServer->ClearServerList();
	//pSelectServer->ClearChannelList();

	while( szServerName && pServerID ) {
		if ( !dwServerID )
			dwServerID = *pServerID;

		nServerID ++;

		LogString (LOG_DEBUG, "\n\n[[ Server: %s, ID: %d ]]\n\n\n", szServerName, *pServerID );

		///¼­¹ö³×ÀÓ¿¡@¿¡ ºÙÀº°æ¿ì °³¹ßÀÚ ±ÇÇÑ¿¡¼­¸¸ ¸®½ºÆ®¿¡ º¸¿©ÁØ´Ù.
		if( g_GameDATA.m_bForOpenTestServer )
		{
			if( szServerName[0] == '@' && strlen(szServerName) >= 2 )
				TempServerList.insert( std::map< BYTE, pair< DWORD, std::string> >::value_type( (BYTE)( 128 + (*pServerID) ), make_pair( *pServerID, &szServerName[1] ) ) );
		}
		else
		{
			if( szServerName[0] == '@')
			{  
				if( dwRight >= CHEAT_MM) 
				{
					if( strlen(szServerName) >= 2 )
						TempServerList.insert( std::map< BYTE, pair< DWORD, std::string> >::value_type( (BYTE)( 128 + (*pServerID) ) , make_pair( *pServerID, &szServerName[0] ) ) );
				}
			}
			else
			{
				if( strlen(szServerName) >= 2 )
					TempServerList.insert( std::map< BYTE, pair< DWORD, std::string> >::value_type( szServerName[0], make_pair( *pServerID, &szServerName[1] ) ) );

			}
		}

		szServerName = Packet_GetStringPtr( m_pRecvPacket, nOffset);
		pServerID = (DWORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(DWORD) );
	}

	std::map< BYTE, pair< DWORD, std::string> >::iterator iter;
	int iCount = 0;
	for( iter = TempServerList.begin(); iter != TempServerList.end(); ++iter, ++iCount )
		ServerList.AddWorldServerList( iCount, iter->second.first, (char*)(iter->second.second.c_str()) );


	g_EUILobby.HideMsgBox();

	///ÇÑÇÁ·¹ÀÓÀ» ±×·ÁÁÖ¾î¾ß LoadingÀÌ ³ª¿Â´Ù.
	g_EUILobby.ShowMsgBox( "Loading...",CTMsgBox::BT_NONE, true, pSelectServer->GetDialogType() );
	//CGame::GetInstance().UpdateCurrentState();
	//->

	CGame::GetInstance().ChangeState( CGame::GS_SELECTSVR );
	g_EUILobby.HideMsgBox();

	g_EUILobby.CreateServerListDlg();
	g_EUILobby.SendReqFirstServerChannelList();

	///Á¢¼Ó¿¡ ¼º°øÇß´Ù

	if( CTDialog* pDlg = g_EUILobby.GetEUI( EUI_LOGIN ) )
	{
		CLogin* p = (CLogin*)pDlg;
		if( g_ClientStorage.IsSaveLastConnectID() )
			g_ClientStorage.SaveLastConnectID( p->GetID() );
		else
			g_ClientStorage.SaveLastConnectID( NULL );

		g_ClientStorage.SaveOptionLastConnectID();
	}

	///nProtect  ¹Ú ÁöÈ£
	//m_nProtectSys.Set_UserID(g_GameDATA.m_Account.Get());


	return true;
}

//-------------------------------------------------------------------------------------------------
int CRecvPACKET::Recv_lsv_SELECT_SERVER ()
{
	LogString (LOG_DEBUG, "Recv_lsv_SELECT_SERVER:: Result: %d ", m_pRecvPacket->m_lsv_SELECT_SERVER.m_btResult);

	///
	/// Recv proc
	/// ³»ºÎ¿¡¼­ Á¤»ó µ¿ÀÛ ¿©ºÎ È®ÀÎ...
	///
	CSelectServer* pSelectServer = (CSelectServer*)g_EUILobby.GetEUI( EUI_SELECT_SERVER );

	if( pSelectServer == NULL )
		return 0;

	pSelectServer->RecvSelectServer( m_pRecvPacket );

	if ( m_pRecvPacket->m_lsv_SELECT_SERVER.m_btResult != RESULT_SELECT_SERVER_OK	) {
		return 0;
	}

	short nOffset = sizeof( lsv_SELECT_SERVER );
	char *szServerIP;
	WORD *pServerPort;

	szServerIP = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	pServerPort= (WORD*)Packet_GetDataPtr  ( m_pRecvPacket, nOffset, sizeof(WORD) );

	// ¼ÒÄÏ µ¿ÀÛ Áß¿¡´Â ÁÖ¼Ò³ª Æ÷Æ®¸¦ ¹Ù²Ü¼ö ¾ø´Ù.
	this->m_WSV_IP.Set( szServerIP );
	this->m_wWSV_PORT  = *pServerPort;
	this->m_dwWSV_ID   = m_pRecvPacket->m_lsv_SELECT_SERVER.m_dwIDs[0];

	LogString (LOG_DEBUG, "Recv_lsv_SELECT_SERVER:: Result: %d, IP: %s, Port: %d ", m_pRecvPacket->m_lsv_SELECT_SERVER.m_btResult, szServerIP, *pServerPort);

	this->DisconnectFromServer( NS_DIS_FORM_LSV );	

	// ÀÓ½Ã
	//DestroyWaitDlg();

	return m_pRecvPacket->m_lsv_SELECT_SERVER.m_dwIDs[1];
}

//-------------------------------------------------------------------------------------------------
int CRecvPACKET::Recv_srv_JOIN_SERVER_REPLY ()
{
	char *szResult[] = {
		"RESULT_JOIN_SERVER_OK",
			"RESULT_JOIN_SERVER_FAILED",
			"RESULT_JOIN_SERVER_TIME_OUT",
			"RESULT_JOIN_SERVER_INVALID_PASSWORD",
			"RESULT_JOIN_SERVER_ALREADY_LOGGEDIN"
	} ;

	LogString (LOG_DEBUG, "Recv_srv_JOIN_SERVER_REPLY:: Result: %d [ %s ]", m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_btResult, szResult[ m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_btResult ]);

	if ( RESULT_JOIN_SERVER_OK != m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_btResult ) {
		return 0;
	}

	return m_pRecvPacket->m_srv_JOIN_SERVER_REPLY.m_dwID;
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_INIT_DATA ()
{
	m_pRecvPacket->m_gsv_INIT_DATA.m_iRandomSEED;
	m_pRecvPacket->m_gsv_INIT_DATA.m_wRandomINDEX;
#ifdef FRAROSE
	g_itMGR.AppendChatMsg("Bienvenu sur FraRose Online.",IT_MGR::CHAT_TYPE_SYSTEM);
	g_itMGR.AppendChatMsg("Ne divulgez jamais votre nom de compte ou mot de passe à qui que ce soit, nos admin ne vous le demanderons jamais.",IT_MGR::CHAT_TYPE_SYSTEM);
	g_itMGR.AppendChatMsg("Bon jeu!.",IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 255, 255, 255));
#endif
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_wsv_CHAR_LIST ()
{
	CSelectAvata* pSelectAvata = (CSelectAvata*)g_EUILobby.GetEUI( EUI_SELECT_AVATA );

	pSelectAvata->RecvAvataList( m_pRecvPacket );

	g_EUILobby.CloseWaitAvataListDlg();	

	if( CGame::GetInstance().GetCurrStateID() == CGame::GS_SELECTSVR )
		CGame::GetInstance().ChangeState( CGame::GS_PREPARESELECTAVATAR );
	else
		CGame::GetInstance().ChangeState( CGame::GS_SELECTAVATAR );
}

bool CRecvPACKET::Recv_wsv_CREATE_CHAR ()
{

	CCreateAvata* pCreateAvata = (CCreateAvata*)g_EUILobby.GetEUI( EUI_CREATE_AVATA );
	if( pCreateAvata == NULL )
		return false;

	if( pCreateAvata->RecvCreateAvata( m_pRecvPacket ) )
	{
		g_EUILobby.CloseWaitJoinServerDlg();		
		g_pNet->Send_cli_CHAR_LIST();
		return true;
	}

	return false;
}

void CRecvPACKET::Recv_wsv_MOVE_SERVER ()
{
	short nOffset = sizeof(wsv_MOVE_SERVER);
	char *szServerIP  =Packet_GetStringPtr( m_pRecvPacket, nOffset);

	this->m_GSV_IP.Set( szServerIP );
	this->m_wGSV_PORT    = m_pRecvPacket->m_wsv_MOVE_SERVER.m_wPortNO;
	this->m_dwGSV_IDs[0] = m_pRecvPacket->m_wsv_MOVE_SERVER.m_dwIDs[0];
	this->m_dwGSV_IDs[1] = m_pRecvPacket->m_wsv_MOVE_SERVER.m_dwIDs[1];
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_JOIN_ZONE ()
{
	if( !g_pAVATAR )
		return;

	// ³» ¾Æ¹ÙÅ¸ÀÇ ÀÎµ¦½º...
 	g_pObjMGR->Set_ServerObjectIndex (g_pAVATAR->Get_INDEX(), m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex );

	/// Á×¾úÀ»°æ¿ì.. ÇöÀç HP¿Í ÆÐ³ÎÆ¼ °æÇèÄ¡ ¼¼ÆÃ..
	/// ÇöÀç °æÇèÄ¡ sETTING
	g_pAVATAR->SetCur_EXP( m_pRecvPacket->m_gsv_JOIN_ZONE.m_lCurEXP );
	g_pAVATAR->Set_HP( m_pRecvPacket->m_gsv_JOIN_ZONE.m_nCurHP );
	g_pAVATAR->Set_MP( m_pRecvPacket->m_gsv_JOIN_ZONE.m_nCurMP );
	g_pAVATAR->m_GrowAbility.m_lPenalEXP = m_pRecvPacket->m_gsv_JOIN_ZONE.m_lPenalEXP;

	/// È¸º¹À» À§ÇÑ ½Ã°£ ¸®¼Â..
	g_pAVATAR->ClearTimer();

	// set server var ÇÔ¼ö ¸¸µé¾î¾îÇÔ...
	SetServerVAR( &m_pRecvPacket->m_gsv_JOIN_ZONE.m_VAR );

	g_pObjMGR->ResetTime();
	//	g_pCMouse->SetEnable();
	//	g_pCKeybd->SetEnable();

	g_DayNNightProc.SetWorldTime( g_pTerrain->GetZoneNO(), m_pRecvPacket->m_gsv_JOIN_ZONE.m_dwAccWorldTIME );	

	/// ¼ÒÈ¯¸÷ ¸®½ºÆ® Å¬¸®¾î..
	g_pAVATAR->ClearSummonedMob();



	//----------------------------------------------------------------------------------------------------	
	/// @brief Á¸µ¥ÀÌÅÍ¿¡ ¼³Á¤µÈ Æ®¸®°Å½ÇÇà
	//----------------------------------------------------------------------------------------------------
	g_pAVATAR->SetTeamInfo( m_pRecvPacket->m_gsv_JOIN_ZONE.m_iTeamNO );
	/// Äù½ºÆ® º¸»ó ½ÇÇà.	
	/// Å¬¶óÀÌ¾ðÆ®¿¡¼­ ½ÇÇàÇÏ´Â°ÍÀÌ ¾Æ´Ï¶ó.. Quest_Reply°¡ ¿À°Ô ¼öÁ¤ 7/28
	/*char* pTriggerName = ZONE_JOIN_TRIGGER( g_pTerrain->GetZoneNO() );
	t_HASHKEY HashKEY = ::StrToHashKey( pTriggerName );
	eQST_RESULT bResult = g_QuestList.CheckQUEST( g_pAVATAR, HashKEY, true );*/

	//----------------------------------------------------------------------------------------------------	
	/// @brief Á¸¿¡ Á¢¼ÓÇÏ¸ç ³ª¿ÃÈ¿°ú
	//----------------------------------------------------------------------------------------------------
	SE_CharJoinZone( g_pAVATAR->Get_INDEX() );

	//----------------------------------------------------------------------------------------
	/// ³¯¾¾ ÀÌÆåÆ®
	//----------------------------------------------------------------------------------------
	int iZoneWeather = ZONE_WEATHER_TYPE( g_pTerrain->GetZoneNO() );
	SE_WeatherEffect( g_pAVATAR->Get_INDEX(), iZoneWeather );


	//----------------------------------------------------------------------------------------------------	
	/// @brief ÆÄÆ¼ÁßÀÏ°æ¿ì¿¡´Â SvrIndex¸¦ ¹Ù²Ù¾îÁØ´Ù.
	//----------------------------------------------------------------------------------------------------
	if( CParty::GetInstance().HasParty() )
	{
		tag_PARTY_MEMBER MemberInfo;
		MemberInfo.m_dwUserTAG  = g_pAVATAR->GetUniqueTag();
		MemberInfo.m_wObjectIDX = m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex	;
		/* ³ªÀÏ°æ¿ì´Â ¹«½ÃÇÑ´Ù.
		MemberInfo.m_nHP		= g_pAVATAR->Get_HP();
		MemberInfo.m_nMaxHP		= g_pAVATAR->Get_MaxHP();
		MemberInfo.m_btRecoverHP = g_pAVATAR->Get_RecoverHP();
		MemberInfo.m_btRecoverMP = g_pAVATAR->Get_RecoverMP();
		MemberInfo.m_btCON		 = g_pAVATAR->Get_CON();
		MemberInfo.m_dwStatusFALG = g_pAVATAR->m_EndurancePack.GetStateFlag()
		*/
		CParty::GetInstance().ChangeMemberInfoByUserTag( MemberInfo );
	}


	//	CLiveCheck::GetSingleton().ResetTime();		// 2004.11.11, icarus: SRV_JOIN_SERVER_REPLY¹Þ¾ÒÀ» ¶§·Î ÀÌµ¿..

	g_GameDATA.m_iReceivedAvatarEXP = g_pAVATAR->Get_EXP();

	::setModelBlinkCloseMode( g_pAVATAR->GetZMODEL(), false );

	//------------------------------------------------------------------------------------
	///¹Ú ÁöÈ£: ¿©½Å¼ÒÈ¯ »óÅÂÀÎ À¯Àú´Â ¼ÒÈ¯ÁßÀÎ ÀÌÆåÆ®¸¦ ¼³Á¤ÇÑ´Ù. 
	//------------------------------------------------------------------------------------
	goddessMgr.SetProcess(goddessMgr.IsAvataState(),m_pRecvPacket->m_gsv_JOIN_ZONE.m_wServerObjectIndex,TRUE);

}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_REVIVE_REPLY ()
{
	if ( m_pRecvPacket->m_gsv_REVIVE_REPLY.m_nZoneNO > 0 )
	{
		if( g_pAVATAR )
			g_pAVATAR->m_nReviveZoneNO = m_pRecvPacket->m_gsv_REVIVE_REPLY.m_nZoneNO;
	}
}
void CRecvPACKET::Recv_gsv_SET_VAR_REPLY ()
{
	if( !g_pAVATAR )
		return;

	if ( m_pRecvPacket->m_gsv_SET_VAR_REPLY.m_btVarTYPE & REPLY_GSV_SET_VAR_FAIL_BIT ) {
		// TODO:: º¯¼ö ¼³Á¤ ¿À·ù !!!
		return;
	}

	int iValue = m_pRecvPacket->m_gsv_SET_VAR_REPLY.m_iValue;
	switch( m_pRecvPacket->m_gsv_SET_VAR_REPLY.m_btVarTYPE ) 
	{
	case SV_SEX		:	return g_pAVATAR->Set_SEX ( iValue );		
	case SV_CLASS	:	return g_pAVATAR->Set_JOB ( iValue );
	case SV_UNION	:	return g_pAVATAR->Set_UNION ( iValue );
	case SV_RANK	:	return g_pAVATAR->Set_RANK ( iValue );
	case SV_FAME	:	return g_pAVATAR->Set_FAME ( iValue );

	case SV_STR		:	return g_pAVATAR->Set_STR ( iValue );
	case SV_DEX		:	return g_pAVATAR->Set_DEX ( iValue );
	case SV_INT		:	return g_pAVATAR->Set_INT ( iValue );
	case SV_CON		:	return g_pAVATAR->Set_CON ( iValue );
	case SV_CHA		:	return g_pAVATAR->Set_CHARM ( iValue );
	case SV_SEN		:	return g_pAVATAR->Set_SENSE ( iValue );

	case SV_LEVEL	:	return g_pAVATAR->Set_LEVEL ( iValue );
	case SV_EXP		:	return g_pAVATAR->Set_EXP ( iValue );

	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SELECT_CHAR ()
{
	/*
	struct gsv_SELECT_CHAR : public t_PACKETHEADER {
	WORD				m_wObjectIDX;
	short				m_nZoneNO;
	tPOINTF				m_PosSTART;

	tagBasicINFO		m_BasicINFO;	
	tagBasicAbility		m_BasicAbility;	
	tagGrowAbility		m_GrowAbility;	
	tagSkillAbility		m_Skill;
	//	tagQuestData		m_Quests;
	//	char				szName[];
	} 
	;*/
	short nOffset=sizeof( gsv_SELECT_CHAR );
	char *szName;
	szName = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	//Dagnarus
	g_pCApp->SetCaption( CStr::Printf("Tsuki Online [ %s ]", szName));

	LogString (LOG_NORMAL, "\n\n\n>>> AVATER( %s ) : Zone: %d, Pos: %f, %f <<<\n\n\n\n",
		szName, 
		m_pRecvPacket->m_gsv_SELECT_CHAR.m_nZoneNO,
		m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.x, m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.y);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ¼±ÅÃµÈ ¾Æ¹ÙÅ¸ µ¥ÀÌÅÍ ¼¼ÆÃ..
	CGame& refGame = CGame::GetInstance();
	refGame.m_strAvataName = std::string( ( szName == NULL )? "NULL":szName );


	//	::CopyMemory( refGame.m_SelectedAvataInfo.m_nPartItemIDX, m_pRecvPacket->m_gsv_SELECT_CHAR.m_nPartItemIDX, sizeof( refGame.m_SelectedAvataInfo.m_nPartItemIDX ) );
	::CopyMemory( refGame.m_SelectedAvataInfo.m_PartITEM, m_pRecvPacket->m_gsv_SELECT_CHAR.m_PartITEM, sizeof( refGame.m_SelectedAvataInfo.m_PartITEM ) );

	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_BasicINFO,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_BasicINFO,		sizeof(tagBasicINFO)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_BasicAbility,	&m_pRecvPacket->m_gsv_SELECT_CHAR.m_BasicAbility,	sizeof(tagBasicAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_BasicAbility,	&m_pRecvPacket->m_gsv_SELECT_CHAR.m_BasicAbility,	sizeof(tagBasicAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_GrowAbility,	&m_pRecvPacket->m_gsv_SELECT_CHAR.m_GrowAbility,	sizeof(tagGrowAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_Skill,			&m_pRecvPacket->m_gsv_SELECT_CHAR.m_Skill,			sizeof(tagSkillAbility)	);
	::CopyMemory ( &refGame.m_SelectedAvataInfo.m_HotICONS,		&m_pRecvPacket->m_gsv_SELECT_CHAR.m_HotICONS,		sizeof(CHotICONS)		);

	refGame.m_SelectedAvataInfo.m_btCharRACE     = m_pRecvPacket->m_gsv_SELECT_CHAR.m_btCharRACE;
	refGame.m_SelectedAvataInfo.m_nZoneNO		= m_pRecvPacket->m_gsv_SELECT_CHAR.m_nZoneNO;
	refGame.m_SelectedAvataInfo.m_PosSTART.x		= m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.x;
	refGame.m_SelectedAvataInfo.m_PosSTART.y		= m_pRecvPacket->m_gsv_SELECT_CHAR.m_PosSTART.y;
	refGame.m_SelectedAvataInfo.m_nReviveZoneNO	= m_pRecvPacket->m_gsv_SELECT_CHAR.m_nReviveZoneNO;
	refGame.m_SelectedAvataInfo.m_dwUniqueTAG	= m_pRecvPacket->m_gsv_SELECT_CHAR.m_dwUniqueTAG;


}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_INVENTORY_DATA ()
{
	CGame& refGame = CGame::GetInstance();
	refGame.m_SelectedAvataINV.m_INV.m_i64Money = m_pRecvPacket->m_gsv_INVENTORY_DATA.m_INV.m_i64Money;

	memcpy( &refGame.m_SelectedAvataINV.m_INV.m_ItemLIST, 
		&m_pRecvPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST, 
		sizeof( m_pRecvPacket->m_gsv_INVENTORY_DATA.m_INV.m_ItemLIST ) );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_QUEST_DATA()
{ 
	CGame& refGame = CGame::GetInstance();

	memcpy( &refGame.m_QuestData,
		&m_pRecvPacket->m_gsv_QUEST_DATA,
		sizeof( m_pRecvPacket->m_gsv_QUEST_DATA ) );

	g_EUILobby.CloseAvataListDlg();	

	CGame::GetInstance().CreateSelectedAvata();


	gsv_TELEPORT_REPLY data;
	data.m_nZoneNO = refGame.m_SelectedAvataInfo.m_nZoneNO;
	data.m_PosWARP.x = refGame.m_SelectedAvataInfo.m_PosSTART.x;
	data.m_PosWARP.y = refGame.m_SelectedAvataInfo.m_PosSTART.y;


	refGame.SetLoadingData(  data );
	refGame.ChangeState( CGame::GS_MOVEMAIN );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHEAT_CODE ()
{
	char *szCheatCode = m_pRecvPacket->m_gsv_CHEAT_CODE.m_szCheatCODE;
	g_Cheat.DoCheat( szCheatCode );
}


//-------------------------------------------------------------------------------------------------
/// ÀÏ´Ü Á¤Áö»óÅÂ¿¡¼­¸¸ µ¿ÀÛÇÑ´Ù..
void CRecvPACKET::Recv_gsv_SET_MOTION ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_SET_MOTION.m_wObjectIDX, false );
	if ( pCHAR ) {
		// 2003. 11. 27 Ãß°¡..
		// ¸ó½ºÅÍ, ¾Æ¹ÙÅ¸¸¦ ±¸ºÐÇÏ¿© ¹ÞÀº µ¿ÀÛÀ» Àû¿ëÇÒ°÷...
#pragma message ( "TODO:: ÄÉ¸¯ÅÍ¿¡ µ¿ÀÛ Àû¿ëÇÒ ºÎºÐ Å×½ºÆ® ¾ÈµÈ°÷.." __FILE__ )

		if( m_pRecvPacket->m_gsv_SET_MOTION.m_wIsSTOP )
		{
			pCHAR->SetCMD_STOP();
		}

		// ÇöÀç ÀÌµ¿¼ÓµµÀ¯Áö(?) È¤½Ã ÀÌµ¿ ÁßÀÏÁö ¸ô¶ó¼­... ¾Ö´Ï ¼Óµµ´Â µðÆúÆ®... 
		pCHAR->Set_MOTION( m_pRecvPacket->m_gsv_SET_MOTION.m_nMotionNO, pCHAR->m_fCurMoveSpeed );
		::setRepeatCount( pCHAR->GetZMODEL(), 1 );

		pCHAR->SetUpdateMotionFlag( true );


		/*pCHAR->Set_STATE( CS_STOP );
		pCHAR->Set_COMMAND( CMD_STOP );
		pCHAR-> m_fCurMoveSpeed = 0;*/
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_TOGGLE ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_TOGGLE.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		pCHAR->SetCMD_TOGGLE( m_pRecvPacket->m_gsv_TOGGLE.m_btTYPE );				

		/// ¼Óµµ°¡ º¯Çß´Ù¸é ¼Óµµ ¼¼ÆÃ
		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_TOGGLE ) + sizeof(short) ) )
		{
			if( pCHAR->IsUSER() )
			{
				((CObjAVT*)pCHAR)->SetOri_RunSPEED( m_pRecvPacket->m_gsv_TOGGLE.m_nRunSPEED[0] );
			}
		}	
	}
}

//-------------------------------------------------------------------------------------------------
char* __stdcall Translate(char* text, char* lang = "en") {
    /*char szPath[256], szRead[0xF00], szRet[256], *Token, szSite[256], *next_token;
    GetTempPath(256, szPath);
    strcat(szPath, "translation.txt");

	sprintf_s(szSite, 255, "http://aeongames.net/~support/translate.php?dest=%s&msg=%s", lang, text);

    if (URLDownloadToFile(0, szSite, szPath, 0, 0) == S_OK)
	{
        FILE *fIpHtm = fopen(szPath, "r");
        if (fIpHtm)
		{
			ZeroMemory(szRead, sizeof(szRead));
			fseek (fIpHtm , 0 , SEEK_END);
			long lSize = ftell (fIpHtm);
			rewind (fIpHtm);

			fseek(fIpHtm, 0, SEEK_SET);
			fread (szRead,1,lSize,fIpHtm);
			fclose(fIpHtm);

            sprintf_s(szRet, 250, "%s", szRead);
            return szRead;
        }
    }*/
    return text;
}

void CRecvPACKET::Recv_gsv_CHAT ()
{
	short nOffset = sizeof( gsv_CHAT );
	char *szMsg;

	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		szMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);

		if( szMsg[ 0 ] == '/' )
			return;

		if(g_GameDATA.m_bTranslate)
		{
			///*
			char* tMsg;
			tMsg = Translate(szMsg, g_GameDATA.m_cLang.Get());
			szMsg = tMsg;
			//*/
		}

		g_itMGR.AppendChatMsg( CStr::Printf("%s>%s", pCHAR->Get_NAME(), szMsg), IT_MGR::CHAT_TYPE_ALL );

		g_UIMed.AddChatMsg( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, szMsg );

#ifdef __VIRTUAL_SERVER
		g_Cheat.DoCheat( szMsg );
#endif

	}
}

void CRecvPACKET::Recv_gsv_PARTY_CHAT()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_CHAT_1(%d)\n" ,g_GameDATA.GetGameTime() );
	short nOffset = sizeof( gsv_CHAT );
	char *szMsg;

	PartyMember member;
	if( CParty::GetInstance().GetMemberInfoByObjSvrIdx( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, member ))
	{
		szMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);

		if(g_GameDATA.m_bTranslate)
		{
			///*
			char* tMsg;
			tMsg = Translate(szMsg, g_GameDATA.m_cLang.Get());
			szMsg = tMsg;
			//*/
		}
		g_itMGR.AppendChatMsg( CStr::Printf("%s>%s", member.m_strName.c_str(), szMsg), IT_MGR::CHAT_TYPE_PARTY );
		g_UIMed.AddChatMsg( m_pRecvPacket->m_gsv_CHAT.m_wObjectIDX, szMsg );
	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_CHAT_2(%d)\n" ,g_GameDATA.GetGameTime() );
}

void CRecvPACKET::Recv_gsv_WHISPER ()
{
	short nOffset = sizeof( gsv_WHISPER );
	char *szAccount, *szMsg;
	szAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	szMsg     = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	if( szMsg[ 0 ] == '/' )
		return;
	char buffer[1024];

	if ( szMsg && szMsg[ 0 ] ) 
	{
		if(g_GameDATA.m_bTranslate)
		{
			///*
			char* tMsg;
			tMsg = Translate(szMsg, g_GameDATA.m_cLang.Get());
			szMsg = tMsg;
			//*/
		}
		///¼­¹ö°¡ º¸³»´Â ±Ó¼Ó¸»Àº Ç×»ó º¸¿©ÁØ´Ù.
		///GMÀÌ³ª ½Ã½ºÅÛ ±Ó¼Ó¸»ÀÇ°æ¿ìµµ ±×·¸°Ô ÇØ¾ß ÇÒÅÙµ¥...
		if( strcmpi( szAccount, "<SERVER>::" ) == 0 )
		{
			sprintf( buffer, "[%s]%s>%s",STR_WHISPER,szAccount, szMsg );
			g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
		}
		else
		{
			if( g_ClientStorage.IsApproveWhisper() )
			{
				sprintf( buffer,"[%s]%s>%s", STR_WHISPER,szAccount, szMsg );
				g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
			}
			else
			{
				g_pNet->Send_cli_WHISPER(  szAccount, STR_OTHER_WHISPER_REJECT_STATE );
			}
		}
	}
	else
		g_itMGR.AppendChatMsg ( CStr::Printf( FORMAT_STR_FAIL_WHISPER, szAccount ), IT_MGR::CHAT_TYPE_WHISPER );
}

void CRecvPACKET::Recv_gsv_SHOUT ()
{
	short nOffset = sizeof( gsv_SHOUT );
	char *szAccount, *szMsg;
	szAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	szMsg     = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	if(g_GameDATA.m_bTranslate)
	{
		///*
		char* tMsg;
		tMsg = Translate(szMsg, g_GameDATA.m_cLang.Get());
		szMsg = tMsg;
		//*/
	}

	g_itMGR.AppendChatMsg( CStr::Printf("[%s]%s>%s",STR_SHOUT, szAccount, szMsg), IT_MGR::CHAT_TYPE_SHOUT);
}

//-------------------------------------------------------------------------------------------------
bool CRecvPACKET::Recv_tag_ADD_CHAR (short nCliObjIDX, tag_ADD_CHAR* tagAddChar )
{
	//struct tag_ADD_CHAR : public t_PACKETHEADER {
	//    WORD		m_wObjectIDX;
	//	tPOINTF		m_PosCUR;
	//	tPOINTF		m_PosTO;
	//	WORD		m_wCommand;
	//	WORD		m_wTargetOBJ;
	//	BYTE		m_btMoveMODE;					// 0:°È±â, 1:¶Ù±â, 2:³» ½Â¿ë¾ÆÀÌÅÛ¿¡ Å¸°íÀÖ´Ù, 3: m_wTargetOBJÀÇ ½Â¿ë¾ÆÀÌÅÛ¿¡ Å¸°íÀÖ´Ù.
	//	int			m_iHP;
	//#ifdef	__APPLY_04_10_15_TEAMNO
	//	int			m_iTeamNO;						//	char m_cTeamNO => int·Î º¯°æ 04.10.15 ¼öÁ¤...
	//#else
	//	char		m_cTeamNO;
	//#endif
	//	DWORD		m_dwStatusFALG;			
	//} ;


	CObjCHAR *pSourCHAR = g_pObjMGR->Get_CharOBJ( nCliObjIDX, false );
	if ( pSourCHAR ) 
	{
		switch( tagAddChar->m_btMoveMODE )
		{
		case 0: // °È±â
			pSourCHAR->m_bRunMODE = false;
			break;
		case 1: // ¶Ù±â
			pSourCHAR->m_bRunMODE = true;
			break;
		case 2: // Ä«Æ®¿¡ Åº »óÅÂ
			{
				//pSourCHAR->m_bRunMODE = false;
				if( pSourCHAR->Is_AVATAR() )
				{
					CObjAVT* pAVT = ( CObjAVT* )pSourCHAR;
					pAVT->CreateCartFromMyData();
				}
			}
			break;
		}		

		pSourCHAR->m_btMoveMODE = tagAddChar->m_btMoveMODE;

		pSourCHAR->Set_HP( tagAddChar->m_iHP );

		/// Team info
		pSourCHAR->SetTeamInfo( tagAddChar->m_iTeamNO );

		if ( pSourCHAR->Get_HP() > 0 ) 
		{
			// pSourCHAR->m_PosGOTO.x = m_pRecvPacket->m_tag_ADD_CHAR.m_PosTO.x;
			// pSourCHAR->m_PosGOTO.y = m_pRecvPacket->m_tag_ADD_CHAR.m_PosTO.y;
			// ;
			switch( tagAddChar->m_wCommand ) {
		case CMD_DIE		:
			pSourCHAR->SetCMD_DIE();
			return false;
		case CMD_SIT		:
			pSourCHAR->SetCMD_SIT ();
			break;

		case CMD_STOP		:
			break;

		case CMD_ATTACK		:
			{
				WORD wServeDist = CD3DUtil::distance ((int)tagAddChar->m_PosCUR.x, (int)tagAddChar->m_PosCUR.y, 
					(int)tagAddChar->m_PosTO.x, (int)tagAddChar->m_PosTO.y);

				D3DVECTOR PosTO;
				PosTO.x = tagAddChar->m_PosTO.x;
				PosTO.y = tagAddChar->m_PosTO.y;
				PosTO.z = 0; // »õ·Î µé¾î¿À½Ã´Â ºÐµéÀº ³ôÀÌ¸¦ ¾Ë ¼ö ¾øÀ¸¹Ç·Î ÇöÀç ÀÚ±â ¾Æ¹ÙÅ¸ÀÇ ³ôÀÌ·Î ÀÏ´Ü ¼³Á¤

				pSourCHAR->SetCMD_ATTACK( 
					tagAddChar->m_wTargetOBJ, 
					wServeDist, 
					PosTO );						
			}
			break;

		case CMD_RUNAWAY    :
		case CMD_MOVE		:
		case CMD_PICK_ITEM	:
			{
				WORD wServeDist = CD3DUtil::distance ((int)tagAddChar->m_PosCUR.x, (int)tagAddChar->m_PosCUR.y, 
					(int)tagAddChar->m_PosTO.x, (int)tagAddChar->m_PosTO.y);
				D3DVECTOR PosTO;
				PosTO.x = tagAddChar->m_PosTO.x;
				PosTO.y = tagAddChar->m_PosTO.y;
				PosTO.z = 0; // »õ·Î µé¾î¿À½Ã´Â ºÐµéÀº ³ôÀÌ¸¦ ¾Ë ¼ö ¾øÀ¸¹Ç·Î ÇöÀç ÀÚ±â ¾Æ¹ÙÅ¸ÀÇ ³ôÀÌ·Î ÀÏ´Ü ¼³Á¤

				pSourCHAR->SetCMD_MOVE ( 
					wServeDist, 
					PosTO, 
					tagAddChar->m_wTargetOBJ );						
			}
			break;

		case CMD_SKILL2SELF :
			// TODO:: // pSourCHAR->SetCMD_Skill2SELF
			break;
		case CMD_SKILL2OBJ :
			// TODO:: // pSourCHAR->SetCMD_Skill2OBJ
			// Å¸°ÙÀ» ¸øÃ£À¸¸é ¸í·É°ú Å¸°ÙÀ» À¯ÁöÇÏ°í
			// ÃÖÁ¾ ÁÂÇ¥·Î ÀÌµ¿..
			break;
		case CMD_SKILL2POS :
			// TODO:: // pSourCHAR->SetCMD_Skill2POS
			break;
			}

			return true;
		}else
		{
			pSourCHAR->Set_HP( DEAD_HP + 1 );
			pSourCHAR->Dead();
		}
	}
	return false;
}

void CRecvPACKET::Recv_tag_ADJ_STATUS ( short& nPacketOffset, tag_ADD_CHAR* ptagAddChar )
{
	int	  iStateSTBIDX = 0;
	DWORD dwTemp = 0;
	DWORD dwResult = ptagAddChar->m_dwStatusFALG;

	CObjCHAR* pChar = g_pObjMGR->Get_ClientCharOBJ( ptagAddChar->m_wObjectIDX, true );
	if( pChar == NULL )
		return;

	// //Á¶¼ºÇö Ä³¸¯ÅÍ º¯½ÅÇÒ¶§...
	if ( ptagAddChar->m_dwStatusFALG &	FLAG_ING_DUMMY_DAMAGE ) {
		pChar->m_bDisguise = true;
	}

	/// ¸ðµç »óÅÂ ºñÆ®µé Ã¼Å©.
	for( int i = 0; i < sizeof( DWORD )*8; i++ )
	{
		dwTemp = 0x00000001 & ( dwResult >> i );

		if( dwTemp == 0 )
			continue;

		int iIngType = 0;
		if( CEndurancePack::m_StateFlagTable.find( ( dwTemp << i ) ) != CEndurancePack::m_StateFlagTable.end() )
		{
			iIngType = CEndurancePack::m_StateFlagTable[ ( dwTemp << i ) ];
		}

		if( CEndurancePack::m_StateSTBIDXTable.find( iIngType ) != CEndurancePack::m_StateSTBIDXTable.end() )
		{
			iStateSTBIDX = CEndurancePack::m_StateSTBIDXTable[ iIngType ];

			/// ¾ÆÀÌÅÛ »ç¿ë¿¡ ÀÇÇÑ Áö¼ÓÀÌ ¾Æ´Ï¶ó¸é...
			if( ING_INC_HP != iStateSTBIDX && ING_INC_MP != iStateSTBIDX )
			{
				if( iStateSTBIDX == ING_DEC_LIFE_TIME )
					pChar->AddEnduranceEntity( 0, 43, 100, ENDURANCE_TYPE_SKILL );
				else
					pChar->AddEnduranceEntity( 0, iStateSTBIDX, 100, ENDURANCE_TYPE_SKILL );
			}
		}/*else
		 {
		 AddMsgToChatWND( STR_UNKNOWN_STATE_FLAG, g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
		 }*/
	}


	if ( ptagAddChar->m_dwStatusFALG & ( FLAG_ING_MAX_HP | FLAG_ING_INC_MOV_SPEED | FLAG_ING_DEC_MOV_SPEED | FLAG_ING_INC_ATK_SPEED | FLAG_ING_DEC_ATK_SPEED ) ) 
	{
		///pChar->m_EndurancePack.ClearEntityPack();
		BYTE* pDATA = reinterpret_cast< BYTE* >( ptagAddChar );
		short *nSTATUS = ( short* )( &(pDATA[ nPacketOffset ]) );

		/// ¼³Á¤µÈ ÇÃ·¡±×¸¸Å­ µ¥ÀÌÅÍ´Â ³Ö´Â´Ù.
		int iIndex = 0;
		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_MAX_HP )		
			pChar->m_EndurancePack.SetStateValue( ING_INC_MAX_HP, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_INC_MOV_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_INC_MOV_SPD, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_DEC_MOV_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_DEC_MOV_SPD, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_INC_ATK_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_INC_ATK_SPD, nSTATUS[ iIndex++ ] );

		if( ptagAddChar->m_dwStatusFALG & FLAG_ING_DEC_ATK_SPEED )		
			pChar->m_EndurancePack.SetStateValue( ING_DEC_ATK_SPD, nSTATUS[ iIndex++ ] );		

		nPacketOffset += iIndex * sizeof( short );

	}
	//else if( ptagAddChar->m_dwStatusFALG & FLAG_ING_STORE_MODE )
	//{
	//	if( pChar->IsUSER() )///¾Æ¹ÙÅ¸ÀÏ°æ¿ì
	//	{
	//		CObjAVT* pAvt=( CObjAVT* )pChar;		


	//		char *pszTitle = Packet_GetStringPtr ( m_pRecvPacket, nPacketOffset );

	//		pAvt->SetPersonalStoreTitle( pszTitle );
	//		g_UIMed.AddPersonalStoreIndex( pAvt->Get_INDEX() );
	//	}
	//}
}

// MOB & NPC Ãß°¡.
void CRecvPACKET::Recv_gsv_SET_NPC_SHOW ()
{
	CObjCHAR *pCHAR = (CObjCHAR*)g_pObjMGR->Get_ClientOBJECT( m_pRecvPacket->m_gsv_SET_NPC_SHOW.m_wObjectIDX );

	if ( pCHAR && pCHAR->IsA(OBJ_NPC ) ) {
		if ( m_pRecvPacket->m_gsv_SET_NPC_SHOW.m_bShow )
			pCHAR->SHOW ();
		else
			pCHAR->HIDE ();
	}
}

void CRecvPACKET::Recv_gsv_NPC_CHAR ()
{
	D3DVECTOR PosCUR;

	PosCUR.x = m_pRecvPacket->m_gsv_NPC_CHAR.m_PosCUR.x;
	PosCUR.y = m_pRecvPacket->m_gsv_NPC_CHAR.m_PosCUR.y;
	PosCUR.z = 0.0f;

	short nCObj, nCharIdx = abs( m_pRecvPacket->m_gsv_NPC_CHAR.m_nCharIdx );

	if ( NPC_TYPE( nCharIdx ) == 999 ) 
	{

		nCObj = g_pObjMGR->Add_NpcCHAR( m_pRecvPacket->m_gsv_NPC_CHAR.m_wObjectIDX, nCharIdx, PosCUR, m_pRecvPacket->m_gsv_NPC_CHAR.m_nQuestIDX, m_pRecvPacket->m_gsv_NPC_CHAR.m_fModelDIR);


		if ( this->Recv_tag_ADD_CHAR( nCObj, &(m_pRecvPacket->m_gsv_NPC_CHAR) ) ) 
		{
			short nOffset = sizeof( gsv_NPC_CHAR );
			this->Recv_tag_ADJ_STATUS( nOffset, &(m_pRecvPacket->m_gsv_NPC_CHAR) );			
		}

		CObjCHAR *pSourCHAR = g_pObjMGR->Get_CharOBJ( nCObj, false );
		if( pSourCHAR )///2004/2/27:Ãß°¡ nAvy
		{
			if ( m_pRecvPacket->m_gsv_NPC_CHAR.m_nCharIdx < 0 ) {
				( (CObjNPC*)pSourCHAR )->HIDE ();
			}

			/// 04/5/18 Æ¯Á¤ NPC¿¡ ÀÌº¥Æ® º¯¼ö Ãß°¡ - jeddli
			((CObjNPC*)pSourCHAR)->SetEventValue( m_pRecvPacket->m_gsv_NPC_CHAR.m_nEventSTATUS );
			/*char Buf[255];
			sprintf( Buf, " ¼­¹ö·Î ºÎÅÍÀÌº¥Æ® º¯¼ö ¹ÞÀ½ %d\n", m_pRecvPacket->m_gsv_NPC_CHAR.m_nEventSTATUS );
			MessageBox( NULL, Buf, "...", MB_OK );*/

			LogString (LOG_NORMAL, "Add NPC : [%s] CObj: %d, SObj: %d \n", 
				pSourCHAR->Get_NAME(),
				nCObj, g_pObjMGR->Get_ServerObjectIndex( nCObj ) );
		}
		else
		{
			LogString (LOG_NORMAL, "Fail Add NPC : CharIdx[%d], QuestIdx[%d]\n", 
				m_pRecvPacket->m_gsv_NPC_CHAR.m_nCharIdx,
				m_pRecvPacket->m_gsv_NPC_CHAR.m_nQuestIDX
				);


		}
	} else {
		_ASSERT( 0 );
	}
}

void CRecvPACKET::Recv_gsv_MOB_CHAR ()
{
	CObjCHAR* pChar = NULL;
	int iSkillOwner = 0;
	int iDoingSkillIDX = 0;

	/// ¼ÒÈ¯¼öÀÎ°¡?
	if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_DEC_LIFE_TIME )
	{
		short nOffset = sizeof( gsv_MOB_CHAR );

		short *nSTATUS = ( short* )( &( m_pRecvPacket->m_pDATA[ nOffset ] ) );

		/// ¼³Á¤µÈ ÇÃ·¡±×¸¸Å­ µ¥ÀÌÅÍ´Â ³Ö´Â´Ù.
		int iIndex = 0;
		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_MAX_HP )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_INC_MOV_SPEED )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_DEC_MOV_SPEED )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_INC_ATK_SPEED )		
			iIndex++;

		if( m_pRecvPacket->m_gsv_MOB_CHAR.m_dwStatusFALG & FLAG_ING_DEC_ATK_SPEED )		
			iIndex++;

		iSkillOwner = nSTATUS[ iIndex++ ];
		pChar = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );

		/// ÇöÀç Ä³½ºÆÃ ÁßÀÌ ¾Æ´Ï¸é ¹Ù·Î ¼ÒÈ¯		
		if( pChar && pChar->m_nActiveSkillIDX && !iDoingSkillIDX )
			iDoingSkillIDX = pChar->m_nActiveSkillIDX;

		if( pChar && pChar->m_nToDoSkillIDX && !iDoingSkillIDX )
			iDoingSkillIDX = pChar->m_nToDoSkillIDX;

		if( pChar && pChar->m_nDoingSkillIDX && iDoingSkillIDX )
			iDoingSkillIDX = pChar->m_nDoingSkillIDX;



		/// ÀÏ´Ü ¹Ù·Î¼ÒÈ¯
		//if( ( pChar != NULL ) && 		
		//	/// ÇöÀç Ä³½ºÆÃ ÁßÀÌÁö ¾ÊÀº°ÍÀº.. ÀÌ¹Ì ¾×¼ÇÀÌ ÁøÇàµÇ¹ö·È´Ù.. ³Ê¹« ´Ê°Ô µµÂøÇÑ ÆÐÅ¶..
		//	iDoingSkillIDX )		
		//{
		//	pChar->SetSummonMobInfo( m_pRecvPacket->m_gsv_MOB_CHAR );
		//	return;
		//}

		/// ½ÃÀüÀÚ°¡ ¾ø´Ù¸é ±×³É ¼ÒÈ¯..
	}

	/// ÀÏ¹Ý ¸÷ µîÀå..
	{

		D3DVECTOR PosCUR;

		PosCUR.x = m_pRecvPacket->m_gsv_MOB_CHAR.m_PosCUR.x;
		PosCUR.y = m_pRecvPacket->m_gsv_MOB_CHAR.m_PosCUR.y;
		PosCUR.z = 0.0f;


		short nCObj;

		if ( NPC_TYPE( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx ) != 999 ) 
		{
			nCObj = g_pObjMGR->Add_MobCHAR( m_pRecvPacket->m_gsv_MOB_CHAR.m_wObjectIDX, 
				m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx, 
				PosCUR, 
				m_pRecvPacket->m_gsv_MOB_CHAR.m_nQuestIDX, 
				m_pRecvPacket->m_gsv_MOB_CHAR.m_btMoveMODE );
			if ( this->Recv_tag_ADD_CHAR( nCObj, &(m_pRecvPacket->m_gsv_MOB_CHAR) ) ) 
			{
				short nOffset = sizeof( gsv_MOB_CHAR );
				this->Recv_tag_ADJ_STATUS( nOffset, &(m_pRecvPacket->m_gsv_MOB_CHAR) );
			}

			/*LogString( LOG_NORMAL, " =====ADD MOB[ %s, %f, %f, %f ]=====\n", 
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_NAME(),
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_CurPOS().x,
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_CurPOS().y,
			g_pObjMGR->Get_CharOBJ( nCObj, false )->Get_CurPOS().z );*/

			//-------------------------------------------------------------------------------------
			/// ¼ÒÈ¯¸÷ÀÏ°æ¿ì.. ¼ÒÈ¯¸÷ÀÓÀ» Ç¥½ÃÇÔ.			
			//-------------------------------------------------------------------------------------
			if( pChar )
			{
				;
			}

			//-------------------------------------------------------------------------------------
			/// ³» ¼ÒÈ¯¸÷ÀÏ°æ¿ì¿¡ ¼ÒÈ¯¸÷ ¸®½ºÆ®¿¡ Ãß°¡
			//-------------------------------------------------------------------------------------
			if( (pChar != NULL) && ( pChar->IsA( OBJ_USER ) ) )
			{
				SummonMobInfo mobInfo;
				bool bIsThere = ((CObjUSER*)pChar)->GetSummonedMobInfo( m_pRecvPacket->m_gsv_MOB_CHAR.m_wObjectIDX, mobInfo );
				if( bIsThere )
					iDoingSkillIDX = mobInfo.iSkillIDX;

				((CObjUSER*)pChar)->AddSummonedMob( m_pRecvPacket->m_gsv_MOB_CHAR.m_wObjectIDX, iDoingSkillIDX );

				if( iDoingSkillIDX )
				{					

					int iSkillLevel = SKILL_LEVEL( iDoingSkillIDX );
					int iMaxHP = 0;

					///2005.06.29 ´ë¸¸ Àû¿ë½Ä ºÐ¸®
					if( CCountry::GetSingleton().IsApplyNewVersion() )
					{
						/// ¼ÒÈ¯¼ö Å¸ÀÔ¿¡ µû¶ó HP°è»ê½Ä ºÐ¸® 2005/8/26 ÃÖÁ¾Áø 
						switch( NPC_SUMMONMOB_TYPE( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx ) )
						{
						case 0:
							iMaxHP = (int)( NPC_HP( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx	) + g_pAVATAR->Get_MaxHP() * 8 / 10 );
							break;
						case 1:
							iMaxHP = (int)( NPC_HP( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx	));
							break;
						default:
							_RPTF0( _CRT_ASSERT,"¾Ë¼ö ¾ø´Â Å¸ÀÔ(¼ÒÈ¯¼ö HP°è»ê½Ä±¸ºÐ)-LIST_NPC.STBÀÇ 29¹øÄÃ·³" );
							break;
						}
					}
					else
					{
						iMaxHP = (int)( NPC_HP( m_pRecvPacket->m_gsv_MOB_CHAR.m_nCharIdx	) * ( iSkillLevel + 16 ) 
							* ( g_pAVATAR->Get_LEVEL() + 85  ) / 2600.f );
					}

					CObjCHAR* pMobChar = g_pObjMGR->Get_CharOBJ( nCObj, true );
					if( pMobChar )
					{
						((CObjMOB*)pMobChar)->Set_MaxHP( iMaxHP );
					}
				}else
				{
					assert( 0 && "This is summoned mob, but don't have owner" );
				}
			}
		} else 
		{
			Recv_gsv_NPC_CHAR ();
			return;
			//		_ASSERT( 0 );
		}
	}
}

//-------------------------------------------------------------------------------------------------
/// »ç¿ëÀÚ Ãß°¡ 
/// ÆÄÆ¼ÁßÀÏ¶§ ÀÌ Ä³¸¯ÅÍ°¡ ÆÄÆ¼¿øÀÌ¶ó¸é ¹Ì¸® ¹Þ¾Æ ÀúÀåÁßÀÌ´ø Data¸¦ °¡Á®¿Í¼­ SettingÇØÁØ´Ù.
/// »óÅÂ´Â ¾îÄÉ ÇÏ³ª?ÇöÀç Recv_gsv_AVT_CHAR¿¡ »óÅÂ°¡ ³Ñ¾î¿À¹Ç·Î º°µµ·Î Ã³¸®°¡ ÇÊ¿ä¾ø°í Object List¿¡ ³Ö±â¸¸ÇÏ¸é µÈ´Ù.
void CRecvPACKET::Recv_gsv_AVT_CHAR ()
{
	/*
	WORD		m_wObjectIDX;
	tPOINTF		m_PosCUR;
	tPOINTF		m_PosTO;
	WORD		m_wCommand;
	WORD		m_wTargetOBJ;
	short		m_nHP;

	bool		m_bIsFemale;
	short		m_nPartItemIDX[ MAX_BODY_PART ];
	// char		szUserID[]
	*/
	short nOffset=sizeof( gsv_AVT_CHAR );
	char *szName = Packet_GetStringPtr ( m_pRecvPacket, nOffset );

	CObjAVT *pNewAVT = g_pObjMGR->New_AvtCHAR( m_pRecvPacket->m_gsv_AVT_CHAR.m_wObjectIDX, szName ); 
	if ( !pNewAVT ) 
	{
		LogString (LOG_NORMAL, "ADD_USER[ %s ] !!!!!!!!FAILED!!!!!!!!!!! \n", szName );
		return;
	}

	/// Bullet Á¤º¸ ÀúÀå
	for( int i = 0;  i < MAX_SHOT_TYPE; ++i )
		pNewAVT->SetShotData( i, m_pRecvPacket->m_gsv_AVT_CHAR.m_sShotItem[i].m_nItemNo );
	//------------------------------------------------------------------------------------------


	// ¾Æ·¡ 3ÁÙÀº ¼ø¼­ ÁÖÀÇ
	//	pNewAVT->SetAllPARTS    ( m_pRecvPacket->m_gsv_AVT_CHAR.m_nPartItemIDX );
	pNewAVT->SetAllPARTS    ( m_pRecvPacket->m_gsv_AVT_CHAR.m_PartITEM );
	//	pNewAVT->SetAllPetPARTS ( m_pRecvPacket->m_gsv_AVT_CHAR.m_nRidingITEM );
	pNewAVT->SetAllPetPARTS ( m_pRecvPacket->m_gsv_AVT_CHAR.m_RidingITEM );
	pNewAVT->SetOri_RunSPEED( m_pRecvPacket->m_gsv_AVT_CHAR.m_nRunSpeed );
	pNewAVT->SetPsv_AtkSPEED( m_pRecvPacket->m_gsv_AVT_CHAR.m_nPsvAtkSpeed );

	pNewAVT->Set_HP( m_pRecvPacket->m_gsv_AVT_CHAR.m_iHP );
	//	assert( m_pRecvPacket->m_gsv_AVT_CHAR.m_nHP > 0 );

	pNewAVT->SetTeamInfo( m_pRecvPacket->m_gsv_AVT_CHAR.m_iTeamNO );

	pNewAVT->SetAvtLevel( m_pRecvPacket->m_gsv_AVT_CHAR.m_btLEVEL );
	/// Job .. Ãß°¡..

	//------------------------------------------------------------------------------------------
	///ÆÄÆ¼¿øÀÏ°æ¿ì
	///CParty¿¡¼­ Á¤º¸¸¦ ¾ò¾î¿Â´Ù.
	PartyMember member;
	if( CParty::GetInstance().GetMemberInfoByObjSvrIdx( m_pRecvPacket->m_gsv_AVT_CHAR.m_wObjectIDX , member ) )
	{
		pNewAVT->Set_MaxHP( member.m_Info.m_nMaxHP );
		pNewAVT->Set_CON( member.m_Info.m_nCON );
		pNewAVT->Set_AddRecoverHP( member.m_Info.m_btRecoverHP );
		pNewAVT->Set_AddRecoverMP( member.m_Info.m_btRecoverMP );
		pNewAVT->SetStamina( member.m_Info.m_nSTAMINA );
	}

	//------------------------------------------------------------------------------------------
	/// TODO:: ¹«°Ô ºñÀ²¿¡ µû¶ó ¶Ù±â/°È±â Á¦ÇÑÀ» °É¾î¾ß ÇÔ...
	m_pRecvPacket->m_gsv_AVT_CHAR.m_btWeightRate;


	pNewAVT->Update_SPEED ();

	LogString (LOG_NORMAL, "ADD_USER[ %s ], MoveSpeed: %f \n", szName, pNewAVT->GetOri_RunSPEED() );

	D3DVECTOR PosCUR;
	PosCUR.x = m_pRecvPacket->m_gsv_AVT_CHAR.m_PosCUR.x;
	PosCUR.y = m_pRecvPacket->m_gsv_AVT_CHAR.m_PosCUR.y;
	PosCUR.z = 0.0f;

	if ( g_pObjMGR->Add_AvtCHAR( pNewAVT, PosCUR, m_pRecvPacket->m_gsv_AVT_CHAR.m_btCharRACE) ) 
	{
		if ( this->Recv_tag_ADD_CHAR( pNewAVT->Get_INDEX(), &(m_pRecvPacket->m_gsv_AVT_CHAR) ) ) 
		{
			this->Recv_tag_ADJ_STATUS( nOffset, &(m_pRecvPacket->m_gsv_AVT_CHAR) );
		}
	} 
	else 
	{
		g_pObjMGR->Del_Object( pNewAVT );
		return;
	}

	pNewAVT->SetAvataName( Packet_GetStringPtr( m_pRecvPacket, nOffset ) );

	// °³ÀÎ »óÁ¡ Ã¼Å©¸¦ ¿Å±è... 2004. 11. 25
	// Process For Ment string... 
	switch( m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG )
	{
	case FLAG_SUB_STORE_MODE:
		{			
			short* nType = (short*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( short ) );
			char* pszTitle = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );			

			pNewAVT->SetPersonalStoreTitle( pszTitle, *nType );
			g_UIMed.AddPersonalStoreIndex( pNewAVT->Get_INDEX() );
		}
		break;
	case FLAG_SUB_INTRO_CHAT:
		{
			char *pszMent = Packet_GetStringPtr ( m_pRecvPacket, nOffset );
		}
		break;
	default:
		break;
	}



	/// clan information
	tag_CLAN_ID* pClanInfo = (tag_CLAN_ID*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_CLAN_ID) ) ;
	if( pClanInfo && pNewAVT )
	{
		pNewAVT->SetClan( 
			pClanInfo->m_dwClanID, 
			pClanInfo->m_wClanMARK[0],
			pClanInfo->m_wClanMARK[1],
			(char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ),
			pClanInfo->m_btClanLEVEL,
			pClanInfo->m_btClanPOS
			);
	}

	//------------------------------------------------------------------------------------
	/// Æ¯¼öÇÑ »óÅÂ ÇÃ·¡±×
	//------------------------------------------------------------------------------------
	pNewAVT->ChangeSpecialState( m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG );

	//------------------------------------------------------------------------------------
	///¹Ú ÁöÈ£: ¿©½Å¼ÒÈ¯ »óÅÂÀÎ À¯Àú´Â ¼ÒÈ¯ ÀÌÆåÆ®¸¦ ¼³Á¤ÇÑ´Ù. 
	//------------------------------------------------------------------------------------
	goddessMgr.SetProcess((m_pRecvPacket->m_gsv_AVT_CHAR.m_dwSubFLAG & FLAG_SUB_ARUA_FAIRY),
		m_pRecvPacket->m_gsv_AVT_CHAR.m_wObjectIDX,TRUE);
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SUB_OBJECT ()
{
	short nObjCNT= ( m_pRecvPacket->m_HEADER.m_nSize - sizeof( t_PACKETHEADER ) ) / 2;

	CGameOBJ *pObj;
	for (short nI=0; nI<nObjCNT; nI++) 
	{
		pObj = g_pObjMGR->Get_ClientOBJECT( m_pRecvPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ nI ] );
		if ( pObj ) 
		{			 
			switch( pObj->Get_TYPE() )
			{
			case OBJ_EVENTOBJECT:					
				break; 

			case OBJ_ITEM:					
				/*	LogString( LOG_NORMAL, "SubItem [ %d ] \n", iDropItem );
				iDropItem--;
				*/	 
    
			default:  
				{
					//---------------------------------------------------------------------------
					//¹ÚÁöÈ£::2ÀÎ½Â ÇØÁ¦ 
						CObjAVT* pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ nI ], false );
						if(pAVT)
						{	pAVT->Process_JOIN_RIDEUSER(); }
					//---------------------------------------------------------------------------

					g_UIMed.SubPersonalStoreIndex( pObj->Get_INDEX() );

					LogString ( LOG_NORMAL, "Del_Object: CObj: %s, SObj: %d \n", pObj->Get_NAME(), m_pRecvPacket->m_gsv_SUB_OBJECT.m_wObjectIDX[ nI ] );
					g_pObjMGR->Del_Object( pObj );		
				}
				break;
			} 
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_WEIGHT_RATE ()
{
	// ¹«°Ô ºñÀ²¿¡ µû¶ó ¶Ù±â, °È±â Á¦ÇÑµîÀÌ Àû¿ëµÈ´Ù.
	CObjAVT* pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_wObjectIDX, false );
	if( pAVT )
	{
		pAVT->SetWeightRate( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate );

		if( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate >= 110 )
		{
			if( pAVT->GetPetMode() < 0 )
				pAVT->SetCMD_TOGGLE( MOVE_MODE_WALK + TOGGLE_TYPE_DRIVE );

			if( pAVT->IsA(OBJ_USER) )
				g_itMGR.AppendChatMsg( CStr::Printf("%s >= 110%%, %s",STR_WEIGHT_RATE,STR_CANT_ATTACK), IT_MGR::CHAT_TYPE_SYSTEM );
		}
		else if( m_pRecvPacket->m_gsv_SET_WEIGHT_RATE.m_btWeightRate >= 100  )
		{
			if( pAVT->GetPetMode() < 0 )
				pAVT->SetCMD_TOGGLE( MOVE_MODE_WALK + TOGGLE_TYPE_DRIVE );

			if( pAVT->IsA( OBJ_USER ) )
				g_itMGR.AppendChatMsg(CStr::Printf("100%% <= %s < 110%%, %s", STR_WEIGHT_RATE, STR_CANT_RUN ), IT_MGR::CHAT_TYPE_SYSTEM );
		}
		else
		{
			if( pAVT->IsA( OBJ_USER ) )
				g_itMGR.AppendChatMsg( CStr::Printf("%s < 100%%, %s", STR_WEIGHT_RATE ,STR_NO_PENALTY), IT_MGR::CHAT_TYPE_SYSTEM );
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ADJUST_POS ()
{
	m_pRecvPacket->m_gsv_ADJUST_POS.m_wObjectIDX;
	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_ADJUST_POS.m_wObjectIDX, true );
	if ( pSourCHAR ) 
	{
		// »ç¿ëÀÚÀÇ ÁÂÇ¥¸¦ º¸Á¤ÇÒ°÷ !!!
		// Ãâ¹ß À§Ä¡´Â °°°í ÃÖÁ¾ À§Ä¡°¡ ¹Ù²î´Â°æ¿ì·Î...
		// Ãâ¹ß À§Ä¡ º¸´Ù ¸Ö¸® °£ °æ¿ì´Â »ª½ÃÅ°°í
		// ´ú °£ °æ¿ì´Â ÃÖÁ¾ À§Ä¡¸¦ ¹Ù²Ù¾î ÁÖ¸é µÉµí...

		// TODO:: pSourCHARÀÇ ÁÂÇ¥¸¦ m_pRecvPacket->m_gsv_ADJUST_POS.m_PosCUR·Î ÃÖÁ¾ À§Ä¡·Î ºü¸£°Ô ÀÌµ¿ º¸Á¤ÇÑ´Ù.
		// ÀÏ´Ü °­Á¦·Î..
		D3DVECTOR PosCUR;
		PosCUR.x = m_pRecvPacket->m_gsv_ADJUST_POS.m_PosCUR.x;
		PosCUR.y = m_pRecvPacket->m_gsv_ADJUST_POS.m_PosCUR.y;
		PosCUR.z = m_pRecvPacket->m_gsv_ADJUST_POS.m_nPosZ;

		pSourCHAR->SetCMD_MOVE(	PosCUR, pSourCHAR->m_bRunMODE );

		// Á¤Áö ¸í·ÉÀ¸·Î..
		//pSourCHAR->SetCMD_STOP ();
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_STOP ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_STOP.m_wObjectIDX, true );
	if ( pCHAR ) {
		pCHAR->SetCMD_STOP ();
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ATTACK ()
{
	
	
	//Á¶¼ºÇö 10 - 27 chatting
	//--------------------------------------------------------------
	/*
	char buffer[1024];
	sprintf( buffer, "*******************");
	g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );

    sprintf( buffer, "Receive Attack");
	g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
	//*/
	//-------------------------------------------------------------
	
	
	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_ATTACK.m_wAtkObjIDX, true );

	if ( pSourCHAR ) 
	{
		if( pSourCHAR->IsA( OBJ_MOB ) )
			pSourCHAR->m_bRunMODE = true;

		D3DVECTOR PosTO;
		PosTO.x = m_pRecvPacket->m_gsv_ATTACK.m_PosTO.x;
		PosTO.y = m_pRecvPacket->m_gsv_ATTACK.m_PosTO.y;
		PosTO.z = 0;

		pSourCHAR->SetCMD_ATTACK( m_pRecvPacket->m_gsv_ATTACK.m_wDefObjIDX, 
			m_pRecvPacket->m_gsv_ATTACK.m_wSrvDIST, 
			PosTO );

#ifdef	_DEBUG
		if ( g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->m_nIndex ) == m_pRecvPacket->m_gsv_ATTACK.m_wDefObjIDX ) {
			LogString (LOG_NORMAL, "°ø°Ý¸í·É:: %d:%s[ %d ]  ==> %s[ %f, %f, %f ] \n", 
				m_pRecvPacket->m_gsv_ATTACK.m_wAtkObjIDX,
				pSourCHAR->Get_NAME(),

				m_pRecvPacket->m_gsv_ATTACK.m_wSrvDIST, 					

				g_pAVATAR->Get_NAME(),

				m_pRecvPacket->m_gsv_ATTACK.m_PosTO.x, 
				m_pRecvPacket->m_gsv_ATTACK.m_PosTO.y,
				PosTO.z );
		}
#endif
	}
}

void CRecvPACKET::Recv_gsv_ATTACK_START ()
{
	_ASSERT( 0 );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief z°ªÀÌ Ãß°¡µÊ..
//----------------------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_MOUSECMD ()
{
	/*
	struct gsv_MOUSECMD : public t_PACKETHEADER {
	WORD	m_wSourObjIDX;
	WORD	m_wDestObjIDX;
	tPOINTF	m_PosCUR;
	tPOINTF m_PosTO;
	} ;
	*/
	
	//Á¶¼ºÇö 10 - 27 chatting 
	//-----------------------------------------------------------------
	/*
	char buffer[1024];
	sprintf( buffer, "*******************");
	g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );

    sprintf( buffer, "Receive Mouse Command");
	g_itMGR.AppendChatMsg ( buffer, IT_MGR::CHAT_TYPE_WHISPER );
	//*/
    //------------------------------------------------------------------
	

	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_MOUSECMD.m_wSourObjIDX, true );
	if ( !pSourCHAR )
		return;

	D3DVECTOR PosTO;

	PosTO.x = m_pRecvPacket->m_gsv_MOUSECMD.m_PosTO.x;
	PosTO.y = m_pRecvPacket->m_gsv_MOUSECMD.m_PosTO.y;
	PosTO.z = 0;

	if (0 == pSourCHAR->Get_ModelSPEED()) {
		// ÇöÀç Á¦ÀÚ¸®¿¡ ¸ØÃß¾î ¼­ ÀÖ´ø °æ¿ì¿¡¸¸ ³ôÀÌ º¸Á¤.
		// Update_Height ¿¡¼­ °»½ÅÇÒ ÈùÆ® ³ôÀÌ°ª ¼¼ÆÃ.
		pSourCHAR->SetResetPosZ( true, m_pRecvPacket->m_gsv_MOUSECMD.m_nPosZ );
	}

	//D3DXVECTOR3 PosCur = pSourCHAR->Get_CurPOS();
	//PosCur.z = m_pRecvPacket->m_gsv_MOUSECMD.m_nPosZ;
	//pSourCHAR->ResetCUR_POS( PosCur );

	pSourCHAR->SetCMD_MOVE ( m_pRecvPacket->m_gsv_MOUSECMD.m_wSrvDIST,
		PosTO, 
		m_pRecvPacket->m_gsv_MOUSECMD.m_wDestObjIDX );


}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
/// @bug ÀÌÆÐÅ¶Àº ¸ó½ºÅÍ ÀÌµ¿¿¡¸¸ µé¾î¿Â´Ù. ±×·¡¼­m_pRecvPacket->m_gsv_MOVE.m_nPosZ ÀÌ ¾²·¹±â°ª 
///		 ³ôÀÌ º¸Á¤À» ÇÏÁö¸¶¶ó.
//----------------------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_MOVE ()
{
	/// Recv_gsv_MOUSECMD¿¡ ÀÌµ¿¼Óµµ Ãß°¡µÈ ÆÐÅ¶...
	CObjCHAR *pSourCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_MOVE.m_wSourObjIDX, true );
	if ( !pSourCHAR )
		return;	

	pSourCHAR->m_bRunMODE = m_pRecvPacket->m_gsv_MOVE.m_btMoveMODE;
	pSourCHAR->m_btMoveMODE = m_pRecvPacket->m_gsv_MOVE.m_btMoveMODE;

	D3DVECTOR PosTO;
	PosTO.x = m_pRecvPacket->m_gsv_MOVE.m_PosTO.x;
	PosTO.y = m_pRecvPacket->m_gsv_MOVE.m_PosTO.y;	
	PosTO.z = 0;

	/*if( pSourCHAR->Is_AVATAR() )
	{
	D3DXVECTOR3 PosCur = pSourCHAR->Get_CurPOS();
	if( abs( PosCur.z - m_pRecvPacket->m_gsv_MOVE.m_nPosZ )  )
	{
	PosCur.z = m_pRecvPacket->m_gsv_MOVE.m_nPosZ;
	pSourCHAR->ResetCUR_POS( PosCur );
	}
	}*/

	pSourCHAR->SetCMD_MOVE ( 
		m_pRecvPacket->m_gsv_MOVE.m_wSrvDIST, 
		PosTO, 
		m_pRecvPacket->m_gsv_MOVE.m_wDestObjIDX );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_DAMAGE ()
{
	//------------------------------------------------------------------------------------
	/// µ¥¹ÌÁö·Î´Â ¼ÒÈ¯¸÷ÀÎÁö ¸ð¸¥´Ù. ¸®½ºÆ®¿¡¼­ Ã£¾Æ¼­ ÀÖÀ¸¸é Áö¿ö¶ó. ¼ÒÈ¯¸÷ ¸®½ºÆ®¿¡¼­ »©¶ó..
	//------------------------------------------------------------------------------------			
	if ( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD )
	{
		if( g_pAVATAR )
			g_pAVATAR->SubSummonedMob( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX );
	}


	CObjCHAR *pAtkOBJ, *pDefOBJ;

	pDefOBJ = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX, true );
	pAtkOBJ = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX, false );


	/////<-2004 / 2/ 11 : ¼öÁ¤ - Object List¿¡´Â ¾øÁö¸¸ ´Ù¸¥Á¸¿¡ ÀÖ´Â MemberÀÇ Damage°¡ ¿Ã¼ö ÀÖ´Ù.( ÆÄÆ¼ ) - nAvy
	//CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_PARTY );
	//if( pDlg && pDlg->IsVision() )
	//{
	//	CPartyDlg* pPartyDlg = (CPartyDlg*)pDlg;
	//	if ( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) 
	//		pPartyDlg->MemberDead( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX );
	//	else
	//		pPartyDlg->MemberDamaged( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX,m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage );
	//}

	if ( NULL == pDefOBJ ) 
		return;


	//if( pAtkOBJ ) {
	//	LogString (LOG_NORMAL, "damage: %x,  %s(%f, %f) => %s(%f, %f) \n", m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage,
	//						pAtkOBJ->Get_NAME(), pAtkOBJ->m_PosCUR.x, pAtkOBJ->m_PosCUR.y,
	//						pDefOBJ->Get_NAME(), pDefOBJ->m_PosCUR.x, pDefOBJ->m_PosCUR.y );
	//}


	if ( pAtkOBJ ) 
	{
		// 2004. 01. 06 ¾ÆÀÌÅÛ µå·Ó ¾Å±× ¸ÂÃß±â..
		if ( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) 
		{
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_DAMAGE ) + sizeof( tag_DROPITEM ) ) ) 
			{
				// Á×À»¶§ µå·ÓµÈ ¾ÆÀÌÅÛÀÌ ÀÖ´Ù...

				short nOffset = sizeof( gsv_DAMAGE );
				tag_DROPITEM* pFieldItem = ( tag_DROPITEM* )Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_DROPITEM ) );


				/// ¸÷ÀÌ Á×À¸¸é¼­ µå¶øÇÑ°ÍÀÌ´Ù.
				//CObjCHAR* pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX, false );
				if( pDefOBJ )
				{
					assert( ( pFieldItem->m_ITEM.GetTYPE() < 15 ) || ( pFieldItem->m_ITEM.GetTYPE() == ITEM_TYPE_MONEY ) );
					pDefOBJ->PushFieldItemToList( *pFieldItem );

					pDefOBJ->m_bDead = true;
				}			
			}
		}

		if ( (m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) )
			//&&  pAtkOBJ->m_iServerTarget == m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX ) 
		{
			// ÀÌ µ¥¹ÌÁö ¸Â°í Á×¾î¾ß ÇÑ´Ù...
			// ¹Ì¸® Á×´Â°ÍÀ» ¸·±âÀ§ÇØ¼­ Á×Àº ³ÑÀ» ÀÏÁ¤ ½Ã°£ ´ë±â½ÃÄÑ ³õ´Â´Ù.
			pDefOBJ->m_DeadDAMAGE.m_nTargetObjIDX = pAtkOBJ->Get_INDEX();	// ¶§¸°³Ñ
			pDefOBJ->m_DeadDAMAGE.m_wDamage = m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage;
			pDefOBJ->m_lDeadTIME = g_GameDATA.GetGameTime();

			pDefOBJ->m_bDead = true;

			/*if( g_pAVATAR )
			{
			if( g_pAVATAR->Get_INDEX() == pAtkOBJ->Get_INDEX() )
			{				
			switch( pDefOBJ->Get_TYPE() )
			{
			case OBJ_MOB:
			sprintf( g_MsgBuf, F_STR_SUCCESS_HUNT, pDefOBJ->Get_NAME() );
			break;

			case OBJ_AVATAR:
			sprintf( g_MsgBuf, F_STR_WIN_PVP, pDefOBJ->Get_NAME() );
			break;
			}				
			g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM );
			}
			}*/

			return;
		}

		// Áï½Ã µ¥¹ÌÁö Àû¿ë..
		if( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_IMMEDIATE )
		{			
			pDefOBJ->Apply_DAMAGE( pAtkOBJ, m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage );
			if( pDefOBJ != pAtkOBJ )
				pDefOBJ->CreateImmediateDigitEffect( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage );


			g_itMGR.AppendChatMsg( CStr::Printf(  "%s[ %d ]", STR_SHIELD_COUNTERATTACK, m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage ) ,
				IT_MGR::CHAT_TYPE_SYSTEM );
			return;
		}

		//----------------------------------------------------------------------------------------------------
		/// @param
		/// @brief Damage µî·Ï..
		///			Pet Mode ÀÏ °æ¿ì¿¡´Â Pet ÀÇ ÀÎµ¦½º¸¦ ³Ö´Â´Ù.( ¼­¹ö´Â Å¾½ÂÀÚ¸¸ Á¸ÀçÇÏ¹Ç·Î Å¾½ÂÀÚ·Î Attacker °¡ ³¯¶ó¿Â´Ù. )
		//----------------------------------------------------------------------------------------------------		
		/// Pet Å¾½ÂÁßÀÏ¶§´Â
		if( pAtkOBJ->GetPetMode() >= 0 )
		{
			if( pAtkOBJ->IsUSER() )
			{
				if( ((CObjAVT*)pAtkOBJ)->m_pObjCART != NULL )
				{
					int iIndex = ((CObjAVT*)pAtkOBJ)->m_pObjCART->Get_INDEX();
					pDefOBJ->PushDamageToList( iIndex, m_pRecvPacket->m_gsv_DAMAGE.m_Damage );		
				}else
				{
					assert( 0 && "PETMODE is valid, but ChildCART is NULL" );
				}
			}
		}else
		{
			pDefOBJ->PushDamageToList( g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_DAMAGE.m_wAtkObjIDX ), m_pRecvPacket->m_gsv_DAMAGE.m_Damage );		
		}
		return;
	}

	if( !pAtkOBJ )
		Log_String( LOG_NORMAL, "°ø°ÝÀÚ°¡ ¾ø´Ù~~~~!! \n" );


	/// °ø°ÝÀÚ°¡ ¾ø´Âµ¥ Á×´Â ÆÐÅ¶ÀÌ ¿ÔÀ»°æ¿ì...
	if ( (m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage & DMG_BIT_DEAD ) ) 
	{
		// ÀÌ µ¥¹ÌÁö ¸Â°í Á×¾î¾ß ÇÑ´Ù...
		// ¹Ì¸® Á×´Â°ÍÀ» ¸·±âÀ§ÇØ¼­ Á×Àº ³ÑÀ» ÀÏÁ¤ ½Ã°£ ´ë±â½ÃÄÑ ³õ´Â´Ù.
		pDefOBJ->m_DeadDAMAGE.m_nTargetObjIDX = 0;	// ¶§¸°³Ñ
		pDefOBJ->m_DeadDAMAGE.m_wDamage = m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage;
		pDefOBJ->m_lDeadTIME = g_GameDATA.GetGameTime();
		pDefOBJ->m_bDead = true;

		//------------------------------------------------------------------------------------
		/// µ¥¹ÌÁö·Î´Â ¼ÒÈ¯¸÷ÀÎÁö ¸ð¸¥´Ù. ¸®½ºÆ®¿¡¼­ Ã£¾Æ¼­ ÀÖÀ¸¸é Áö¿ö¶ó. ¼ÒÈ¯¸÷ ¸®½ºÆ®¿¡¼­ »©¶ó..
		//------------------------------------------------------------------------------------
		if( g_pAVATAR )
			g_pAVATAR->SubSummonedMob( m_pRecvPacket->m_gsv_DAMAGE.m_wDefObjIDX );

		return;
	}

	// °ø°ÝÀÚ°¡ ¾ø°Å³ª, °ø°ÝÀÚ°¡ ÇöÀç ´Ù¸¥ Å¸°ÙÀ» °ø°ÝÁßÀÌÁö ¾ÊÀ»°æ¿ì ¹Ù·Î µ¥¹ÌÁö Àû¿ë...
	pDefOBJ->Apply_DAMAGE( pAtkOBJ, m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage );
	if( pDefOBJ != pAtkOBJ )
		pDefOBJ->CreateImmediateDigitEffect( m_pRecvPacket->m_gsv_DAMAGE.m_Damage.m_wDamage );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHANGE_NPC ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_CHANGE_NPC.m_wObjectIDX, true );
	if ( pCHAR && pCHAR->IsNPC() ) {
		pCHAR->Change_CHAR( m_pRecvPacket->m_gsv_CHANGE_NPC.m_nNPCNo );
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SETEXP ()
{
	if( !g_pAVATAR )
		return;

	/// ÇØ´ç ¸÷ÀÌ Á×´Â Å¸ÀÌ¹Ö¿¡ °æÇèÄ¡ È¹µæ ¸Þ¼¼Áö¸¦ Ãâ·ÂÇÏ±â À§ÇØ¼­
	if( m_pRecvPacket->m_gsv_SETEXP.m_wFromObjIDX == 0 )
	{
		/// °»½ÅµÈ ³» °æÇèÄ¡´Ù...
		long lDiff = m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP - g_GameDATA.m_iReceivedAvatarEXP;

		/// ÀÌ°Ç ¹º°¡ ¼­¹ö¿¡¼­ ¼ø¼­°¡ Àß¸øµÇ¼­ ³¯¶ó¿Â°ÍÀÌ´Ù.
		if( lDiff < 0 )
		{
			char buf[ 255 ];
			sprintf( buf, "Received exp is invalid[ %d/%d ]", m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP, g_GameDATA.m_iReceivedAvatarEXP );
			assert( 0 && buf );

			lDiff = 1;

			/// °æÇèÄ¡ ±ïÀÏ °æ¿ì¿¡´Â Ãâ·Â¾ÈÇÔ...
			/// ¸®Á°·º¼Ç µîÀÇ ½ºÅ³·Î ºÎÈ°ÇÒ°æ¿ì °æÇèÄ¡ ÆÐ³ÎÆ¼ ¶§¹®¿¡.. ÀÌÀü°æÇèÄ¡°¡ ´õ Å¬¼ö ÀÖ´Ù
			/// °á±¹ lDiff °¡ À½¼ö°¡ µÉ¼ö ÀÖ´Ù...( ¹ö±×·Î ÀÎÇÑ - ´Â ¾ø´Ù´Â °¡Á¤.. )
		}else
		{
			/// lDiffÀÇ °æÇèÄ¡¸¦ È¹µæÇß´Ù... ¸Þ¼¼ÁöÃâ·Â..
			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, lDiff );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );	

			g_pAVATAR->SetCur_EXP( m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP );				
			/// ½ºÅÂ¹Ì³Êµµ Å¸ÀÌ¹Ö¶§¹®¿¡.. µû·Î Ã³¸®ÇÏ±â·Î Çß´Ù.. 2005/1/19
			g_pAVATAR->SetCur_STAMINA( m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );
		}
	}else
	{
		/// ÇöÀç °æÇèÄ¡¸¦ ³Ö¾ú´õ´Ï.. Ã³¸® ¼ø¼­°¡ Æ²·ÁÁö¸¸ - °æÇèÄ¡¸¦ ¾òÀ» °æ¿ì°¡ ÀÖ´Ù..
		long lDiff = m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP - g_GameDATA.m_iReceivedAvatarEXP;
		/// ÀÌ°Ç ¹º°¡ ¼­¹ö¿¡¼­ ¼ø¼­°¡ Àß¸øµÇ¼­ ³¯¶ó¿Â°ÍÀÌ´Ù.
		if( lDiff < 0 )
		{
			char buf[ 255 ];
			sprintf( buf, "Received exp is invalid[ %d/%d ]", m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP, g_GameDATA.m_iReceivedAvatarEXP );
			assert( 0 && buf );

			lDiff = 1;

			/// °æÇèÄ¡ ±ïÀÏ °æ¿ì¿¡´Â Ãâ·Â¾ÈÇÔ...
			/// ¸®Á°·º¼Ç µîÀÇ ½ºÅ³·Î ºÎÈ°ÇÒ°æ¿ì °æÇèÄ¡ ÆÐ³ÎÆ¼ ¶§¹®¿¡.. ÀÌÀü°æÇèÄ¡°¡ ´õ Å¬¼ö ÀÖ´Ù
			/// °á±¹ lDiff °¡ À½¼ö°¡ µÉ¼ö ÀÖ´Ù...( ¹ö±×·Î ÀÎÇÑ - ´Â ¾ø´Ù´Â °¡Á¤.. )
		}else
		{
			int iClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_SETEXP.m_wFromObjIDX );
			CDelayedExp::GetSingleton().PushEXPData( iClientIndex, lDiff, m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP, 
				m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );
		}
	}

	g_GameDATA.m_iReceivedAvatarEXP = m_pRecvPacket->m_gsv_SETEXP.m_lCurEXP;

	/// ½ºÅÂ¹Ì³Ê´Â ±×³É...
	/// ½ºÅÂ¹Ì³Êµµ Å¸ÀÌ¹Ö¶§¹®¿¡.. µû·Î Ã³¸®ÇÏ±â·Î Çß´Ù.. 2005/1/19
	///g_pAVATAR->SetCur_STAMINA( m_pRecvPacket->m_gsv_SETEXP.m_nCurSTAMINA );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_LEVELUP ()
{
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_LEVELUP.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		if ( pCHAR->IsA( OBJ_USER ) ) 
		{		
			if ( m_pRecvPacket->m_gsv_LEVELUP.m_nSize != sizeof( gsv_LEVELUP ) ) 
				return;


			__int64 lDiffExp = g_pAVATAR->Get_NeedEXP( g_pAVATAR->Get_LEVEL() )+ m_pRecvPacket->m_gsv_LEVELUP.m_lCurEXP - g_GameDATA.m_iReceivedAvatarEXP ;

			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, lDiffExp );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );


			// ³»°¡ ·¹º§¾÷ Çß´Ù.
			g_pAVATAR->m_GrowAbility.m_lPenalEXP = 0;
			g_pAVATAR->Set_LEVEL( m_pRecvPacket->m_gsv_LEVELUP.m_nCurLevel );
			g_pAVATAR->Set_EXP  ( m_pRecvPacket->m_gsv_LEVELUP.m_lCurEXP );			
			g_pAVATAR->Set_BonusPOINT( m_pRecvPacket->m_gsv_LEVELUP.m_nBonusPoint );

			int old_skillpoint = g_pAVATAR->Get_SkillPOINT();
			int new_skillpoint = m_pRecvPacket->m_gsv_LEVELUP.m_nSkillPoint;
			if( old_skillpoint != new_skillpoint )
				g_itMGR.AppendChatMsg( CStr::Printf(STR_EARN_SP, new_skillpoint - old_skillpoint ), IT_MGR::CHAT_TYPE_SYSTEM ,D3DCOLOR_ARGB( 255,255,0,0 ));

			g_pAVATAR->Set_SkillPOINT( m_pRecvPacket->m_gsv_LEVELUP.m_nSkillPoint );


			g_pAVATAR->LevelUP ();

			///»õ·Î¿î ·¹º§¿¡ ¸Â´Â TipÀÇ Index List¸¦ Àç±¸¼ºÇÑ´Ù.
			g_HelpMgr.Update();
			if( g_pAVATAR->GetClanID() )
				g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV( m_pRecvPacket->m_gsv_LEVELUP.m_nCurLevel, g_pAVATAR->Get_JOB() );


			/// Notify levelup event
			CTutorialEventManager::GetSingleton().CheckLevelUpEvent( g_pAVATAR->Get_LEVEL() );

			g_GameDATA.m_iReceivedAvatarEXP = m_pRecvPacket->m_gsv_LEVELUP.m_lCurEXP;
		}
		else
		{
			pCHAR->SetAvtLevel( pCHAR->Get_LEVEL() + 1 );
		}

		//----------------------------------------------------------------------------------------------------		
		/// @brief ·¹º§¾÷ È¿°ú Ãâ·Â
		//----------------------------------------------------------------------------------------------------
		SE_CharLevelUp( pCHAR->Get_INDEX() );
	}
}

void CRecvPACKET::Recv_gsv_HP_REPLY()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_HP_REPLY.m_wObjectIDX, false );
	if ( pCHAR && g_pAVATAR ) 
	{
		short client_obj_index = g_pObjMGR->Get_ClientObjectIndex ( m_pRecvPacket->m_gsv_HP_REPLY.m_wObjectIDX );

		if( client_obj_index == g_pAVATAR->Get_INDEX() )
			pCHAR->SetReviseHP( m_pRecvPacket->m_gsv_HP_REPLY.m_iHP - pCHAR->Get_HP() );
		else
			pCHAR->Set_HP( m_pRecvPacket->m_gsv_HP_REPLY.m_iHP );
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CHANGE_SKIN ()
{
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHANGE_SKIN.m_wObjectIDX, false );
	if ( pCHAR ) 
	{
		// 2003. 11. 27 Ãß°¡..
		// ¾Æ¹ÙÅ¸ ¾ó±¼, ¸Ó¸®ÅÐ º¯°æÇÒ ºÎºÐ...


		/// Object ¹øÈ£°¡ ³ª¶ó¸é ÆÐ½º~
		if( pCHAR->Get_INDEX() == g_pAVATAR->Get_INDEX() )
			return;

		pCHAR->SetPartITEM( m_pRecvPacket->m_gsv_CHANGE_SKIN.m_wAbilityTYPE,
			m_pRecvPacket->m_gsv_CHANGE_SKIN.m_iValue );

		if( !pCHAR->IsPersonalStoreMode() )
			pCHAR->Update ();	
	}
}

//-------------------------------------------------------------------------------------------------
/// m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nEquipIndex °¡ ÀÎº¥Åä¸® ½½·Ô ³Ñ¹ö°¡ ³Ñ¾î¿Â´Ù..
/// @bug ÀÌµ¿¼Óµµ Ã¼Å©ÇÏ¶ó..
void CRecvPACKET::Recv_gsv_EQUIP_ITEM ()
{
	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_wObjectIDX, false );
	if ( pCHAR ) 
	{	
		short nBodyPart = CInventory::GetBodyPartByEquipSlot( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nEquipIndex );

		if( nBodyPart == MAX_EQUIP_IDX )
		{
			_ASSERT( 0 && "nBodyPart == MAX_EQUIP_IDX" );
			return;
		}

		short nEquipItemNO = m_pRecvPacket->m_gsv_EQUIP_ITEM.m_PartITEM.m_nItemNo;


		if( nBodyPart == BODY_PART_WEAPON_R || nBodyPart == BODY_PART_WEAPON_L )
		{			
			if( nBodyPart == BODY_PART_WEAPON_R ) 
			{
				//	short nWeaponTYPE = WEAPON_TYPE( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nItemNO );
				short nWeaponTYPE = WEAPON_TYPE( nEquipItemNO );

				if ( nWeaponTYPE >= 221 && nWeaponTYPE <= 255 ) {
					// ¾ç¼Õ°Ë : 221 ~
					// ¿ø°Å¸® : 231 ~
					// ¸¶¹ý¹«±â : 241 ~
					// Ä«Æ®¸£°è¿­ : 251 ~
					if ( nWeaponTYPE != 242 ) 
					{
						// ¾ç¼Õ ¹«±â¸é ¿Þ¼ÕÀÇ ¹«±â¸¦ »èÁ¦...
						if( pCHAR->Get_L_WEAPON() != 0 )
						{
							pCHAR->SetPartITEM( BODY_PART_WEAPON_L, 0 );
							pCHAR->ClearLWeaponSkillEffect();
						}
					} // else ( 242 ÇÑ¼Õ ¸¶¹ý µµ±¸ )					
				}

				pCHAR->SetChangeWeaponR( nEquipItemNO );
				pCHAR->ClearRWeaponSkillEffect();
			} else
				if( nBodyPart == BODY_PART_WEAPON_L )
				{
					pCHAR->SetChangeWeaponL( nEquipItemNO );	
					pCHAR->ClearLWeaponSkillEffect();
				}

				pCHAR->SetUpdateMotionFlag( true );
		}

		if( nBodyPart < MAX_BODY_PART )///2004 / 2 / 2 :nAvyÃß°¡ - ¹ÝÁö,¸ñ°ÉÀÌ, ±Í°ÉÀÌÀÇ °æ¿ì´Â ¾ø´Ù.
		{
			pCHAR->SetPartITEM( nBodyPart, nEquipItemNO );			
		}

		// µî±Þ, º¸¼®¹øÈ£ Æ÷ÇÔ...
		pCHAR->SetPartITEM( nBodyPart, m_pRecvPacket->m_gsv_EQUIP_ITEM.m_PartITEM );

		if( !pCHAR->IsPersonalStoreMode() )
			pCHAR->Update ();		


		/// ¼Óµµ°¡ º¯Çß´Ù¸é ¼Óµµ ¼¼ÆÃ
		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_EQUIP_ITEM ) + sizeof(short) ) )
		{
			if( pCHAR->IsUSER() )
			{
				((CObjAVT*)pCHAR)->SetOri_RunSPEED( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nRunSPEED[0] );
			}
		}


		/// if pCHAR is my avatar, update ability
		if( pCHAR->IsA( OBJ_USER ) )
		{
			((CObjUSER*)pCHAR)->UpdateAbility();
		}

#ifndef	__VIRTUAL_SERVER
		// ¼­¹ö¿¡¼­ ¹ÞÀº ÀÌµ¿ ¼Óµµ Àû¿ë..
		pCHAR->SetOri_RunSPEED( m_pRecvPacket->m_gsv_EQUIP_ITEM.m_nRunSPEED[0] );
		pCHAR->Update_SPEED();
#endif


	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_STORE_TRADE_REPLY ()
{
	// TODO:: »óÁ¡°Å·¡ ¿äÃ»½Ã ½ÇÆÐÇÒ°æ¿ì ÀÀ´ä ¸Þ¼¼Áö ¶ç¿ì±â ...

	switch( m_pRecvPacket->m_gsv_STORE_TRADE_REPLY.m_btRESULT ) 
	{
	case STORE_TRADE_RESULT_PRICE_DIFF:	//°æÁ¦ µ¥ÀÌÅ¸°¡ °»½ÅµÇ¾î °¡°ÝÀÌ Æ²·Á Á³´Ù, °¡°Ý ´Ù½Ã °è»êÇØ¼­ ÆÈ¾Æ¾ßÇÔ
		g_itMGR.OpenMsgBox(STR_CHANGE_PRICES);
		break;
	case STORE_TRADE_RESULT_NPC_NOT_FOUND:// »óÁ¡ ÁÖÀÎÀÌ ¾ø´Ù.
		break;
	case STORE_TRADE_RESULT_TOO_FAR:// »óÁ¡ ÁÖÀÎÀÌ ¸Ö´Ù.
		break;
	case STORE_TRADE_RESULT_OUT_OF_MONEY:
		g_itMGR.OpenMsgBox(STR_NOT_ENOUGH_MONEY);
		break;
	case STORE_TRADE_RESULT_NOT_UNION_USER:// °°Àº Á¶ÇÕ¿øÀÌ ¾Æ´Ï´Ù
		g_itMGR.OpenMsgBox( STR_CANT_OPEN_UNIONSTORE );
		break;
	case STORE_TRADE_RESULT_OUT_OF_POINT://		0x06	// Á¶ÇÕ Æ÷ÀÎÆ®°¡ ¸ðÀÚ¸£´Ù
		g_itMGR.OpenMsgBox( STR_WANT_UNIONPOINT );
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_P_STORE_MONEYnINV()
{
	Recv_gsv_SET_MONEYnINV();
}
//-------------------------------------------------------------------------------------------------
/// 2004 / 2 / 19 : ¼öÁ¤ nAvy ( Add_ITEM => Set_ITEM );
void CRecvPACKET::Recv_gsv_SET_MONEYnINV ()
{
	_ASSERT( m_pRecvPacket->m_HEADER.m_nSize == sizeof( gsv_SET_MONEYnINV ) + sizeof( tag_SET_INVITEM ) * m_pRecvPacket->m_gsv_SET_MONEYnINV.m_btItemCNT );

	if( g_pAVATAR )
	{
		g_pAVATAR->SetCur_MONEY( m_pRecvPacket->m_gsv_SET_MONEYnINV.m_i64Money );

		g_pAVATAR->SetWaitUpdateInventory( true );
		for (short nI=0; nI<m_pRecvPacket->m_gsv_SET_MONEYnINV.m_btItemCNT; nI++) {
			g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ nI ].m_btInvIDX, 
				m_pRecvPacket->m_gsv_SET_MONEYnINV.m_sInvITEM[ nI ].m_ITEM );
		}
		g_pAVATAR->SetWaitUpdateInventory( false );
		g_pAVATAR->UpdateInventory();

	}

	//g_pAVATAR->RefreshLookUpTable();
}

//-------------------------------------------------------------------------------------------------
/// 2004 / 2 / 19 : ¼öÁ¤ nAvy ( Add_ITEM => Set_ITEM );
void CRecvPACKET::Recv_gsv_SET_INV_ONLY ()
{
	_ASSERT( m_pRecvPacket->m_HEADER.m_nSize == sizeof( gsv_SET_INV_ONLY ) + sizeof( tag_SET_INVITEM ) * m_pRecvPacket->m_gsv_SET_INV_ONLY.m_btItemCNT );

	if( g_pAVATAR )
	{
		g_pAVATAR->SetWaitUpdateInventory( true );
		for (short nI=0; nI<m_pRecvPacket->m_gsv_SET_INV_ONLY.m_btItemCNT; nI++) {
			g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ nI ].m_btInvIDX,
				m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[ nI ].m_ITEM );
		}
		g_pAVATAR->SetWaitUpdateInventory( false );

		g_pAVATAR->UpdateAbility();///ÀåÂøµÈ ¾ÆÀÌÅÛ¿¡ µû¸¥ ´É·ÂÄ¡ Àç°è»ê

		if( g_pAVATAR->Get_COMMAND() & CMD_ATTACK )///°ø°ÝÁßÀÏ°æ¿ì
		{
			///ÇöÀç ÀåÂøµÈ ¹«±â¿Í ÇÊ¿ä¼Ò¸ðÅºÀÇ °³¼ö¸¦ ºñ±³ÇØ¼­ 0ÀÏ°æ¿ì STOPÀ» ³¯¸®°í ¸Þ¼¼Áö¸¦ ÇÑ¹ø »Ñ·ÁÁØ´Ù.
			if( int iItemNo = g_pAVATAR->Get_R_WEAPON() )
			{

			}
		}
	}
}


//-------------------------------------------------------------------------------------------------
/// ¾ÆÀÌÅÛÀ» »ç¿ëÇß´Ù.
///¾ÆÀÌÅÛÀÌ »ç¿ëµÇ¾úÀ»¶§ ¼­¹ö·ÎºÎÅÍ ¹Þ´Â ÆÐÅ¶: 2004/4/29:nAvy
/// 1. ³»°¡ ¾´°æ¿ì ( ÇÑ¹ø »ç¿ë¿¡ 1~2¹ø ¿Â´Ù )
///		1) µÚ¿¡ ÀÎº¥Åä¸® ÀÎµ¦½º°¡ ºÙÀº°æ¿ì´Â ¼Ò¸ð¼º ¾ÆÀÌÅÛÀÇ °¹¼ö¸¦ »©±â À§ÇÏ¿© ¹ÞÀº°ªÀÌ´Ù . ÀÌ¶§ ´Ù¸¥ À¯Àú¿¡°Ô ¾Ë·ÁÁÖ±â À§ÇÑ BroadCast¿¡ ³»°¡ Æ÷ÇÔµÇ¾î ÇÑ¹ø´õ ¿Ã¼ö ÀÖ´Ù.
///		2) ¸¸¾à ¾ÆÀÌÅÛÀÌ ´Ù »ç¿ëµÇ¾úÀ»°æ¿ì¿¡´Â ÀÎº¥Åä¸® ÀÎµ¦½º°¡ ºÙÀº ÆÐÅ¶À» ¿ÀÁö ¾Ê°í gsv_SET_INV_ONLY°¡ ¿À°Ô µÈ´Ù.
/// 2. ´Ù¸¥ Ä³¸¯ÅÍ°¡ ¾´°æ¿ì( ¿ÀÁ÷ 1¹ø¸¸ ¿Â´Ù )
///     1) È¿°ú¿Í
void CRecvPACKET::Recv_gsv_USE_ITEM ()
{

	CObjAVT *pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_USE_ITEM.m_wObjectIDX, true );

	/// ¾ÆÀÌÅÛÀ» »ç¿ëÇÑ³ðÀÌ ÀÖ´Ù¸é..
	if ( pAVT ) 
	{

		//--------------------------------------------------------------------------------------------------------------
			//¹ÚÁöÈ£::USEITEM CoolTime Àû¿ë, ¼­¹öÀÇ °á°ú¸¦ ¹Þ°í ÄðÅ¸ÀÓÀ» Àû¿ëÇÑ´Ù. 
			g_pAVATAR->SetCoolTimeUseItem(pAVT->Get_NAME(),m_pRecvPacket->m_gsv_USE_ITEM.m_nUseItemNO);
		//--------------------------------------------------------------------------------------------------------------

		if( pAVT->IsA( OBJ_USER ))
		{
			///Size°¡ °°Àº °æ¿ì¿¡´Â BroadCast ½Ã¿¡ Áßº¹µÇ´Â ÆÐÅ¶ÀÌ´Ù
			if( m_pRecvPacket->m_HEADER.m_nSize != sizeof( gsv_USE_ITEM ) )
			{
				tagITEM UsedItem = g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_USE_ITEM.m_btInvIDX[0] ];
				UsedItem.m_uiQuantity = 1;
				g_pAVATAR->Sub_ITEM( m_pRecvPacket->m_gsv_USE_ITEM.m_btInvIDX[0], UsedItem );
			}
			else///¸¶Áö¸· ¹°¾àÀÏ°æ¿ì¿¡´Â BroadCast½Ã¿¡¸¸ ¿À°Ô µÈ´Ù.
			{
				///¾ÆÀÌÅÛÀ¸·Î »óÅÂ°¡ º¯È­ÇÏ´Â °æ¿ì
				WORD wUseItemNO = m_pRecvPacket->m_gsv_USE_ITEM.m_nUseItemNO;

				/// ÀÌº¥Æ® ¾ÆÀÌÅÛÀº È¿°ú¹øÈ£¿Í con ÆÄÀÏ ¹øÈ£¸¦ °°ÀÌ »ç¿ëÇÑ´Ù.
				if( ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO  ) != USE_ITEM_EVENT_ITEM )
				{
					// È¿°ú Ãâ·Â..
					int iUseItemEffect = USEITEM_USE_EFFECT( wUseItemNO );
					int iUseItemSound = USEITEM_USE_SOUND( wUseItemNO );

					pAVT->ShowEffectOnCharByIndex( iUseItemEffect, iUseItemSound );
				}


				g_pAVATAR->Use_ITEM( wUseItemNO );



				if ( USEITME_STATUS_STB( wUseItemNO ) && ( USE_ITEM_SKILL_DOING != ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO ) ) ) /// Áö¼ÓÇü ¾ÆÀÌÅÛÀÌ¶ó¸é..
				{		
					for( int i = 0; i < STATE_APPLY_ABILITY_CNT; i++ )
					{
						int	 iStateNO1 = STATE_APPLY_ING_STB( USEITME_STATUS_STB( wUseItemNO ), i );

						if( iStateNO1 )
						{
							int iStepValue = STATE_APPLY_ABILITY_VALUE( USEITME_STATUS_STB( wUseItemNO ), i );
							if( iStepValue )
							{
								int iApplyValue = USEITEM_ADD_DATA_VALUE( wUseItemNO );

								pAVT->AddEnduranceEntity( wUseItemNO,
									iStateNO1, 
									( iApplyValue * 1000 ) / iStepValue,
									ENDURANCE_TYPE_USEITEM );
							}					
						}	
					}
				}
			}
		}else
		{
			///¾ÆÀÌÅÛÀ¸·Î »óÅÂ°¡ º¯È­ÇÏ´Â °æ¿ì
			WORD wUseItemNO = m_pRecvPacket->m_gsv_USE_ITEM.m_nUseItemNO;

			/// ÀÌº¥Æ® ¾ÆÀÌÅÛÀº È¿°ú¹øÈ£¿Í con ÆÄÀÏ ¹øÈ£¸¦ °°ÀÌ »ç¿ëÇÑ´Ù.
			if( ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO  ) != USE_ITEM_EVENT_ITEM )
			{
				// È¿°ú Ãâ·Â..
				int iUseItemEffect = USEITEM_USE_EFFECT( wUseItemNO );
				int iUseItemSound = USEITEM_USE_SOUND( wUseItemNO );
				pAVT->ShowEffectOnCharByIndex( iUseItemEffect, iUseItemSound );
			}

			if ( USEITME_STATUS_STB( wUseItemNO ) && ( USE_ITEM_SKILL_DOING != ITEM_TYPE( ITEM_TYPE_USE, wUseItemNO ) ) ) /// Áö¼ÓÇü ¾ÆÀÌÅÛÀÌ¶ó¸é..
			{		
				for( int i = 0; i < STATE_APPLY_ABILITY_CNT; i++ )
				{
					int	 iStateNO1 = STATE_APPLY_ING_STB( USEITME_STATUS_STB( wUseItemNO ), i );

					if( iStateNO1 )
					{
						int iStepValue = STATE_APPLY_ABILITY_VALUE( USEITME_STATUS_STB( wUseItemNO ), i );
						if( iStepValue )
						{
							int iApplyValue = USEITEM_ADD_DATA_VALUE( wUseItemNO );

							pAVT->AddEnduranceEntity( wUseItemNO,
								iStateNO1, 
								( iApplyValue * 1000 ) / iStepValue,
								ENDURANCE_TYPE_USEITEM );
						}					
					}	
				}
			}
			/// »óÅÂ°¡ ¾øÀ»°æ¿ì ¹Ù·Î Àû¿ë..
			/// HP, MP¸¸..
			else
			{
				switch( USEITEM_ADD_DATA_TYPE( wUseItemNO ) )
				{
				case AT_HP:
					pAVT->Add_HP( USEITEM_ADD_DATA_VALUE( wUseItemNO ) );
					if( pAVT->Get_HP() > pAVT->Get_MaxHP() )
						pAVT->Set_HP( pAVT->Get_MaxHP() );

					break;
				case AT_MP:
					pAVT->Add_MP( USEITEM_ADD_DATA_VALUE( wUseItemNO ) );
					if( pAVT->Get_MP() > pAVT->Get_MaxMP() )
						pAVT->Set_MP( pAVT->Get_MaxMP() );

					break;
				case AT_STAMINA:
					break;
				default:
					assert( 0 && "Invalid property is modified" );
					break;
				}				 
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
/// 1.7 ¸÷ÀÌ µå¶øÇÏ´Â ¾ÆÀÌÅÛÀº µ¥¹ÌÁö¿¡ ºÙ¾î¼­ ¿Â´Ù. ÀÌ ÆÐÅ¶Àº ³»°¡ ¶³¾î¶ß¸®°Å³ª, ´Ù¸¥ ¾Æ¹ÙÅ¸°¡ ¶³¾î¶ß·ÈÀ»¶§ ¿È.
void CRecvPACKET::Recv_gsv_ADD_FIELDITEM ()
{	
	/// ³»°¡ ¶³¾î¶ß¸°°ÍÀÌ´Ù.
	{
		int iItemOBJ = g_pObjMGR->Add_GndITEM( 
			m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wServerItemIDX, 
			m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_ITEM, 
			m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_PosCUR );
		if ( iItemOBJ ) 
		{
			CObjITEM *pITEM = (CObjITEM *)g_pObjMGR->Get_OBJECT( iItemOBJ );
			pITEM->m_wOwnerServerObjIDX = m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX;
			pITEM->m_wRemainTIME		= m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME;

			pITEM->InsertToScene ();

			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wOwnerObjIDX;
			// m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_wRemainTIME;

			if( g_pAVATAR )
			{
				WORD wClientIdx = g_pAVATAR->Get_INDEX();
				WORD wSvrIdx = g_pObjMGR->Get_ServerObjectIndex( wClientIdx );

				if( pITEM->m_wOwnerServerObjIDX == g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX()))
				{
					tagITEM sITEM = m_pRecvPacket->m_gsv_ADD_FIELDITEM.m_ITEM;

					/*if ( sITEM.m_cType != ITEM_TYPE_MONEY )
					g_itMGR.AppendChatMsg(CStr::Printf ("%s %s Type: %d, NO: %d ", 
					STR_DROP_ITEM,
					ITEM_NAME( sITEM.m_cType, sITEM.m_nItemNo ),
					sITEM.m_cType, sITEM.m_nItemNo ), IT_MGR::CHAT_TYPE_SYSTEM);
					else 
					g_itMGR.AppendChatMsg(CStr::Printf ("%s %d",  STR_DROP_MONEY ,sITEM.m_uiMoney), IT_MGR::CHAT_TYPE_SYSTEM);*/
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_GET_FIELDITEM_REPLY ()
{
	// ³»°¡ ¾ÆÀÌÅÛ ¸Ô¾ú´Ù.
	CObjITEM *pITEM = g_pObjMGR->Get_ClientItemOBJ( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_wServerItemIDX );

	switch( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_btResult ) 
	{
	case REPLY_GET_FIELDITEM_REPLY_OK	 :
		// ÀÎº¥Åä¸®¿¡ Ãß°¡...
		g_itMGR.AppendChatMsg( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_ITEM.GettingMESSAGE(m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO), IT_MGR::CHAT_TYPE_SYSTEM );

		if( g_pAVATAR )
			g_pAVATAR->Add_CatchITEM( m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_nInventoryListNO, m_pRecvPacket->m_gsv_GET_FIELDITEM_REPLY.m_ITEM );
	case REPLY_GET_FIELDITEM_REPLY_NONE :
		if ( pITEM ) 
		{
			pITEM->RemoveFromScene ();
			g_pObjMGR->Del_Object( pITEM );
		}
		break;
	case REPLY_GET_FIELDITEM_REPLY_TOO_MANY :
		g_itMGR.AppendChatMsg( STR_NOTIFY_01, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case REPLY_GET_FIELDITEM_REPLY_NO_RIGHT :
		g_itMGR.AppendChatMsg( STR_NOTIFY_02, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	}

}

#include "../System/GameStateMovePlanet.h"
//-------------------------------------------------------------------------------------------------
/// @todo °°Àº Á¸ÀÏ°æ¿ì Á¸ Ç®°í ·ÎµùÇÏ´Â°Å °íÃÄ¶ó...
void CRecvPACKET::Recv_gsv_TELEPORT_REPLY ()
{		
	CGame::GetInstance().SetLoadingData( m_pRecvPacket->m_gsv_TELEPORT_REPLY );

	//---------------------------------------------------------------------------
		//¹ÚÁöÈ£::2ÀÎ½Â ÇØÁ¦ 
		CObjAVT* pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_wObjectIDX,false);
		if(pAVT)
		{	pAVT->Process_JOIN_RIDEUSER(); }
	//---------------------------------------------------------------------------
	
	/// Çà¼º¹øÈ£°¡ °°´Ù¸é..
	if( ZONE_PLANET_NO( m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO ) == 
		ZONE_PLANET_NO( g_pTerrain->GetZoneNO() ) )		
	{
		CGame::GetInstance().ChangeState( CGame::GS_WARP );
	}
	else
	{
		/// Çà¼º¹øÈ£°¡ Æ²¸®´Ù¸é..
		if( ZONE_PLANET_NO( m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO ) != 8 &&
			ZONE_PLANET_NO( g_pTerrain->GetZoneNO() ) != 8 )
		{
			CGameStateMovePlanet::m_iTargetPlanet = m_pRecvPacket->m_gsv_TELEPORT_REPLY.m_nZoneNO;
			CGame::GetInstance().ChangeState( CGame::GS_MOVEPLANET );
		}else			
		{
			/// ¾ÆÁöÆ®·Î È¤Àº ¾ÆÁöÆ®·ÎºÎÅÍÀÇ ÀÌµ¿¿¡´Â ÄÆ½Å ¾øÀ½..
			CGame::GetInstance().ChangeState( CGame::GS_WARP );
		}
	}

}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SET_HOTICON ()
{
	if ( g_pAVATAR ) 
	{
		///g_pAVATAR->m_HotICONS.m_IconLIST[ m_pRecvPacket->m_gsv_SET_HOTICON.m_btListIDX ] = m_pRecvPacket->m_gsv_SET_HOTICON.m_HotICON;
		CHotIconSlot* pHotIcon = g_pAVATAR->GetHotIconSlot();
		pHotIcon->AddQuickItem( m_pRecvPacket->m_gsv_SET_HOTICON.m_btListIDX, 
			m_pRecvPacket->m_gsv_SET_HOTICON.m_HotICON.m_cType,
			m_pRecvPacket->m_gsv_SET_HOTICON.m_HotICON.m_nSlotNo );
	}
}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_USE_BPOINT_REPLY ()
{
	if ( g_pAVATAR ) 
	{
		// ±âº» ´É·ÂÄ¡°¡ ¾÷µ¥ÀÌÆ® µÆ´Ù..
		short nNeedPoint = g_pAVATAR->Get_NeedPoint2AbilityUP( m_pRecvPacket->m_gsv_USE_BPOINT_REPLY.m_btAbilityNO );
		g_pAVATAR->Set_BonusPOINT( g_pAVATAR->Get_BonusPOINT() - nNeedPoint );

		g_pAVATAR->m_BasicAbility.m_nBasicA[ m_pRecvPacket->m_gsv_USE_BPOINT_REPLY.m_btAbilityNO ] = m_pRecvPacket->m_gsv_USE_BPOINT_REPLY.m_nAbilityValue;
		g_pAVATAR->UpdateAbility();
	}
}


//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SKILL_LEARN_REPLY ()
{
	if ( !g_pAVATAR ) 
		return;

	switch( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_btResult ) 
	{
	case RESULT_SKILL_LEARN_SUCCESS	 :		// ¹è¿ì±â ¼º°ø.		


		g_pAVATAR->AddNewSkill( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX, m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_btSkillSLOT );

		g_itMGR.OpenMsgBox( CStr::Printf( F_STR_SKILL_LEARNED,SKILL_NAME( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX )), 
			CMsgBox::BT_OK, false );

		g_pAVATAR->UpdateAbility();

		g_pAVATAR->GetSkillSlot()->SetActiveSkillEnableByRideState( g_pAVATAR->GetPetMode()>=0 );

		break;
	case RESULT_SKILL_LEARN_FAILED :		// ¹è¿ì±â ½ÇÆÐ.	
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_NEED_JOB : 		// Á÷¾÷ÀÌ ÀÏÄ¡ÇÏÁö ¾Ê´Â´Ù.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_JOB_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_NEED_SKILL :	// º¸À¯ÇÒ ½ºÅ³ÀÌ ÇÊ¿äÇÑ´Ù.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_PRESKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_NEED_ABILITY :	// ´É·ÂÄ¡°¡ ºÎÁ·ÇÏ´Ù		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_ABILITY_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_OUTOFSLOT :		// ´õÀÌ»ó ½ºÅ³À» ¹è¿ï¼ö ¾ø´Ù.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_SLOT_FULL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_INVALID_SKILL :		// Àß¸øµÈ ½ºÅ³¹øÈ£ ÀÔ´Ï´Ù.		
		g_itMGR.AppendChatMsg( STR_LEARN_SKILL_INVALID_SKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_OUTOFPOINT ://	0x07		// ½ºÅ³ Æ÷ÀÎÆ® ºÎÁ·
		g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_SKILLPOINT, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_SKILL_LEARN_DELETE:///	0x08		// m_btSkillSLOTÀÇ m_nSkillIDX¹ø ½ºÅ³À» »èÁ¦ÇØ¶ó..
		///½ºÅ³»èÁ¦
		break;
	default:
		break;

	}

	g_pAVATAR->Set_SkillPOINT( m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillPOINT );

}
void CRecvPACKET::Recv_gsv_SKILL_LEVELUP_REPLY ()
{
	if ( !g_pAVATAR ) 
		return;

	switch( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_btResult ) {
		case RESULT_SKILL_LEVELUP_SUCCESS		:
			{
				///g_pAVATAR->AddNewSkill( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillIDX, m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_btSkillSLOT );
				g_pAVATAR->Skill_LEARN( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_btSkillSLOT, m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillINDEX , false );
				g_pAVATAR->Skill_LevelUp( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_btSkillSLOT, m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillINDEX );
				SE_SkillLevelUp( g_pAVATAR->Get_INDEX() );

				if( CCountry::GetSingleton().IsApplyNewVersion() )
				{
					int skill_index = m_pRecvPacket->m_gsv_SKILL_LEARN_REPLY.m_nSkillIDX;			

					int consume_july = SKILL_LEVELUP_NEED_ZULY( skill_index ) * 100;
					int consume_sp   = SKILL_NEED_LEVELUPPOINT( skill_index );

					std::string message;
					if( consume_july )
					{
						message.append( CStr::Printf( STR_CONSUME_JULY, consume_july ) );
						if( consume_sp )
							message.append( "{BR}");

						g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - consume_july );

					}

					if( consume_sp )
						message.append( CStr::Printf( STR_CONSUME_SP , consume_sp ) );

					if( consume_july || consume_sp )
						g_itMGR.OpenMsgBox( message.c_str() , CMsgBox::BT_OK, false );
				}
				g_pAVATAR->UpdateAbility();
			}
			break;
		case RESULT_SKILL_LEVELUP_FAILED		:
			g_itMGR.AppendChatMsg( STR_FAIL_SKILL_LEVELUP, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		case RESULT_SKILL_LEVELUP_OUTOFPOINT	:
			g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_SKILLPOINT, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		case RESULT_SKILL_LEVELUP_NEED_ABILITY	:
			g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_ABILITY_FAILED, IT_MGR::CHAT_TYPE_SYSTEM);
			break;
		case RESULT_SKILL_LEVELUP_NEED_JOB :
			g_itMGR.AppendChatMsg( STR_LEARN_SKILL_JOB_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
		case RESULT_SKILL_LEVELUP_NEED_SKILL :
			g_itMGR.AppendChatMsg( STR_LEARN_SKILL_NEED_PRESKILL_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
			break;
	}
	g_pAVATAR->Set_SkillPOINT( m_pRecvPacket->m_gsv_SKILL_LEVELUP_REPLY.m_nSkillPOINT );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SELF_SKILL ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_SELF_SKILL.m_wSourObjIDX, true );
	if ( pCHAR ) 
	{
		/// ¸¶Áö¸·À¸·Î Ä³½ºÆÃÇÑ ½ºÅ³( cancel½Ã¿¡ ¸¶³ª¸¦ ±ï±â À§ÇØ¼­ )
		pCHAR->SetLastCastingSkill( m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX );

		/// Mob ÀÌ¶ó¸é ½ºÅ³¸ð¼ÇÀ» ¼¼ÆÃÇØÁØ´Ù.
		if( pCHAR->IsA( OBJ_MOB ) == true )
		{			
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_SELF_SKILL ) + sizeof(char) ) )
			{
				((CObjMOB*)pCHAR)->SetMobAniSkill( m_pRecvPacket->m_gsv_SELF_SKILL.cNpcSkillMOTION[0] );						
				Log_String( LOG_NORMAL, "SELF SKILL : cNpcSkillMOTION[0][ %d ]\n", m_pRecvPacket->m_gsv_SELF_SKILL.cNpcSkillMOTION[0] );			
			}
		}	

		pCHAR->SetCMD_Skill2SELF ( m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX );

		//Á¶¼ºÇö Ä³¸¯ÅÍ º¯½ÅÇÒ¶§...
		if((m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX >= 1286)  &&  (m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX <= 1290) )
		{
			pCHAR->m_bDisguise = true;
		}

		//--------------------------------------------------------------------------------------------------------------
			//¹ÚÁöÈ£::Skill CoolTime Àû¿ë, ¼­¹öÀÇ °á°ú¸¦ ¹Þ°í ÄðÅ¸ÀÓÀ» Àû¿ëÇÑ´Ù. 
			g_pAVATAR->SetCoolTimeSkill(pCHAR->Get_NAME(),m_pRecvPacket->m_gsv_SELF_SKILL.m_nSkillIDX);
		//--------------------------------------------------------------------------------------------------------------
	}
}

void CRecvPACKET::Recv_gsv_TARGET_SKILL ()
{
	Log_String( LOG_NORMAL, "Recv Target Skill[%d]\n", m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX );
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_TARGET_SKILL.m_wSourObjIDX, true );
	if ( pCHAR ) 
	{

		/// ¸¶Áö¸·À¸·Î Ä³½ºÆÃÇÑ ½ºÅ³( cancel½Ã¿¡ ¸¶³ª¸¦ ±ï±â À§ÇØ¼­ )
		pCHAR->SetLastCastingSkill( m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX );

		/// Mob ÀÌ¶ó¸é ½ºÅ³¸ð¼ÇÀ» ¼¼ÆÃÇØÁØ´Ù.
		if( pCHAR->IsA( OBJ_MOB ) == true )
		{			
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_TARGET_SKILL ) + sizeof(char) ) )
			{
				((CObjMOB*)pCHAR)->SetMobAniSkill( m_pRecvPacket->m_gsv_TARGET_SKILL.cNpcSkillMOTION[0] );
				Log_String( LOG_NORMAL, "TARGET SKILL : cNpcSkillMOTION[0][ %d ]\n", m_pRecvPacket->m_gsv_TARGET_SKILL.cNpcSkillMOTION[0] );
			}
		}

		D3DVECTOR PosTO;
		PosTO.x = m_pRecvPacket->m_gsv_TARGET_SKILL.m_PosTO.x;
		PosTO.y = m_pRecvPacket->m_gsv_TARGET_SKILL.m_PosTO.y;
		PosTO.z = pCHAR->Get_CurZPOS();

		pCHAR->SetCMD_Skill2OBJ( m_pRecvPacket->m_gsv_TARGET_SKILL.m_wSrvDIST,
			PosTO,
			m_pRecvPacket->m_gsv_TARGET_SKILL.m_wDestObjIDX, 
			m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX );

		//--------------------------------------------------------------------------------------------------------------
			//¹ÚÁöÈ£::Skill CoolTime Àû¿ë, ¼­¹öÀÇ °á°ú¸¦ ¹Þ°í ÄðÅ¸ÀÓÀ» Àû¿ëÇÑ´Ù. 
			g_pAVATAR->SetCoolTimeSkill(pCHAR->Get_NAME(),m_pRecvPacket->m_gsv_TARGET_SKILL.m_nSkillIDX);
		//--------------------------------------------------------------------------------------------------------------

	}
}

void CRecvPACKET::Recv_gsv_POSITION_SKILL ()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_POSITION_SKILL.m_wObjectIDX, true );
	if ( pCHAR ) 
	{
		/// ¸¶Áö¸·À¸·Î Ä³½ºÆÃÇÑ ½ºÅ³( cancel½Ã¿¡ ¸¶³ª¸¦ ±ï±â À§ÇØ¼­ )
		pCHAR->SetLastCastingSkill( m_pRecvPacket->m_gsv_POSITION_SKILL.m_nSkillIDX );

		/// Mob ÀÌ¶ó¸é ½ºÅ³¸ð¼ÇÀ» ¼¼ÆÃÇØÁØ´Ù.
		if( pCHAR->IsA( OBJ_MOB ) == true )
		{			
			if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_POSITION_SKILL ) + sizeof(char) ) )
			{
				((CObjMOB*)pCHAR)->SetMobAniSkill( m_pRecvPacket->m_gsv_POSITION_SKILL.cNpcSkillMOTION[0] );
				Log_String( LOG_NORMAL, "TARGET SKILL : cNpcSkillMOTION[0][ %d ]\n", m_pRecvPacket->m_gsv_POSITION_SKILL.cNpcSkillMOTION[0] );
			}
		}

		D3DVECTOR PosTarget;
		PosTarget.x = m_pRecvPacket->m_gsv_POSITION_SKILL.m_PosTARGET.x;
		PosTarget.y = m_pRecvPacket->m_gsv_POSITION_SKILL.m_PosTARGET.y;
		PosTarget.z = pCHAR->Get_CurZPOS();

		pCHAR->SetCMD_Skill2POS ( PosTarget,
			m_pRecvPacket->m_gsv_POSITION_SKILL.m_nSkillIDX );

		//--------------------------------------------------------------------------------------------------------------
			//¹ÚÁöÈ£::Skill CoolTime Àû¿ë, ¼­¹öÀÇ °á°ú¸¦ ¹Þ°í ÄðÅ¸ÀÓÀ» Àû¿ëÇÑ´Ù. 
			g_pAVATAR->SetCoolTimeSkill(pCHAR->Get_NAME(),m_pRecvPacket->m_gsv_POSITION_SKILL.m_nSkillIDX);
		//--------------------------------------------------------------------------------------------------------------

	} 
}


//------------------------------------------------------------------------------------
/// CRecvPACKET::
/// @param 
/// @brief : ½ÇÁ¦ ¼­¹ö¿¡¼­ Ä³½ºÆÃÀ» ½ÃÀÛÇß´Ù.
//------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_SKILL_START()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_SKILL_START.m_wObjectIDX, true );
	if ( pCHAR ) 
	{
#ifdef _DEBUG		
		Log_String( LOG_NORMAL, "½ºÅ³ ½ÃÀÛ[%d]\n", m_pRecvPacket->m_gsv_SKILL_START.m_wObjectIDX );
#endif		

		/// ÇöÀç ½ÃÀüÇÒ·Á´Â ½ºÅ³Àº ½ºÅ¸Æ®¸¦ ¹Þ¾Ò´Ù.
		if( pCHAR->bCanStartSkill() )
		{
			///assert( 0 && "Recv_gsv_SKILL_START" );
			/// ±×·¸´Ù¸é ¸í·ÉÅ¥¸¦ µÚÁ®¶ó..
			pCHAR->m_CommandQueue.SetValidFlag();

		}else
			pCHAR->SetStartSkill( true );
	}else
	{
		assert( 0 && "SKILL_START[ Not exist owner ]" );
	}
}

/// @bug /// ÀÏ´Ü À¯Á®ÀÏ°æ¿ì¸¸ ¼Ó¼º°´Ã¼ Ãß°¡..  ÀÌ¶óÀÎÀÌ ¿Ö ÀÖÁö? 2004/3/23
void CRecvPACKET::Recv_gsv_EFFECT_OF_SKILL ()
{
	CObjCHAR*			pObjCHAR		= NULL;
	int					iObjIDX			= 0;

	/// ½ºÅ³ ½ÃÀüÀÚ¿¡°Ô.. µî·Ï..
	short iSkillOwner = m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_wSpellObjIDX;
	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );

	/// ÇöÀç »ç¿ëÁßÀÎ ½ºÅ³ÀÌ ¹ß»çÇüÀÌ¶ó¸é.. ¹Ù·Î Àû¿ëÇÑ´Ù.
	int iDoingSkillIDX = 0;

	if( pChar && pChar->m_nActiveSkillIDX )
		iDoingSkillIDX = pChar->m_nActiveSkillIDX;

	if( pChar && pChar->m_nToDoSkillIDX )
		iDoingSkillIDX = pChar->m_nToDoSkillIDX;    	

	if( ( pChar != NULL ) && 		
		/// ÇöÀç Ä³½ºÆÃ ÁßÀÌÁö ¾ÊÀº°ÍÀº.. ÀÌ¹Ì ¾×¼ÇÀÌ ÁøÇàµÇ¹ö·È´Ù.. ³Ê¹« ´Ê°Ô µµÂøÇÑ ÆÐÅ¶..
		iDoingSkillIDX )//&&
		/// ÃÑ¾Ë ¹ß»çÇü ½ºÅ³À» ½ÃÀüÁß ¿Â ½ºÅ³ Àû¿ë°á°ú´Â ( ´Ù Áö¼ÓÀÌ°ÚÁö? ) ¹Ù·Î Àû¿ë..
		//( !( SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_FIRE_BULLET ) || 
		//  (	SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_ENFORCE_BULLET ) ) )	
	{
		gsv_DAMAGE_OF_SKILL damageOfSkill;
		memcpy( &damageOfSkill, &(m_pRecvPacket->m_gsv_EFFECT_OF_SKILL ), sizeof( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL ) );
		pChar->PushEffectedSkillToList( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX, 										
			damageOfSkill,
			m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT );		
	}
	else
	{
		/// ¼ÒÀ¯ÁÖ°¡ ¾øÀ»¶§´Â ¹Ù·Î Àû¿ë...
		///CObjCHAR *pEffectedChar = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_wObjectIDX, true );
		CObjCHAR *pEffectedChar = CSkillManager::GetSkillTarget( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_wObjectIDX, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX );
		if( pEffectedChar == NULL )
		{
			/// ±×³ðÀÌ Á×¾ú°Å³ª.. ³ªÀÇ °ü¸®¿¡¼­ ºüÁ³´Ù..
			/// g_itMGR.OpenMsgBox( "½ºÅ³ Àû¿ë´ë»ó ¾øÀ½" );
			return;
		}

		int iSkillIDX = m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX;

		if( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS == 0 )/// Àû¿ë È¿°úÈÄ ¹Ù·Î »èÁ¦..Áï ½ºÅ³ Àû¿ë ½ÇÆÐ´Ù
		{
			///AddMsgToChatWND( STR_SKILL_APPLY_FAILED, g_dwRED, true );
			///LogString( LOG
			;
		}else
		{
			///½ºÅ³ÀÌ Áö¼ÓÇüÀÏ °æ¿ì ÃÖ´ë 2°³ÀÇ »óÅÂ±îÁö ¹Ù²ð¼ö ÀÖÀ¸¹Ç·Î
			for( int i = 0 ; i < 2 ; i++ ) 
			{			
				if ( ( 0x01 << i ) & m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS ) 
				{
					int iStateIndex = 0;

					/// Áö¼ÓÇüÀÌ ¾Æ´Ñ ´Ü¼ø ´É·ÂÄ¡ »ó½ÂÇü..
					/*if( SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_SELF_BOUND &&
					SKILL_TYPE( iSkillIDX ) != SKILL_ACTION_TARGET_BOUND )*/
					{
						iStateIndex = SKILL_STATE_STB( iSkillIDX, i );

						/// À¯¸®»óÅÂ ÇØÁö, ºÒ¸®»óÅÂ ÇØÁö µîÀÇ »óÅÂ ÇØÁ¦ ½ºÅ³µé
						if( iStateIndex && STATE_TYPE( iStateIndex ) > ING_CHECK_END )
						{
							pEffectedChar->ProcFlushStateSkill( iStateIndex );
							continue;
						}
					}

					if( iStateIndex != 0 )
						/// Áö¼ÓÇü ½ºÅ³ÀÌ¶ó¸é..
						/*if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND_DURATION ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND_DURATION ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_STATE_DURATION ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_STATE_DURATION )*/
					{
						/// ÀÏ´Ü À¯Á®ÀÏ°æ¿ì¸¸ ¼Ó¼º°´Ã¼ Ãß°¡..
						//if( pChar->IsA( OBJ_USER ) )
						pEffectedChar->AddEnduranceEntity( iSkillIDX,	iStateIndex, SKILL_DURATION( iSkillIDX ), ENDURANCE_TYPE_SKILL ) ;

						/// »óÅÂ Å¸ÀÔ..
						int iStateType = STATE_TYPE( iStateIndex );
						/// »óÅÂ ¹øÈ£°¡ 1,2 ¹øÀÎ°æ¿ì¿¡´Â LIST_STATUS.STB ÀÇ °ªÀ» Âü°íÇÏ°í
						if( iStateType == ING_INC_HP || iStateType == ING_INC_MP || iStateType == ING_POISONED )
							pEffectedChar->m_EndurancePack.SetStateValue( iStateType, STATE_APPLY_ABILITY_VALUE( iStateIndex, i ) );
						else
						{
							int iIncValue = 0;							

							/// 04/4/24
							if( pEffectedChar->IsA( OBJ_USER ) )
								iIncValue = CCal::Get_SkillAdjustVALUE( (CObjUSER*)pEffectedChar, iSkillIDX, i, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT );
							else
							{
								iIncValue = 1;

								/// À¯Á®°¡ ¾Æ´Ò°æ¿ì ¾Ë¼ö°¡ ¾ø´Ù. ¸ó½ºÅÍÀÏ°æ¿ì¿¡´Â °ø¼Ó, ÀÌ¼Ó¸¸ °è»êÇØº»´Ù.
								if( pEffectedChar->IsA( OBJ_MOB ) )
								{
									int iAbilityValue = 0;
									switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
									{
									case AT_SPEED:
										iAbilityValue = pEffectedChar->GetOri_RunSPEED();
										break;
									case AT_ATK_SPD:
										iAbilityValue = pEffectedChar->GetOri_ATKSPEED();
										break;
									}

									iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i ) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
								}

								//--------------------------------------------------------------------------------------------
								/// ´Ù¸¥ ¾Æ¹ÙÅ¸ÀÏ°æ¿ì.. MAX_HP´Â °í·Á¸¦ ÇÑ´Ù..
								if( pEffectedChar->IsA( OBJ_AVATAR ) )
								{
									int iAbilityValue = 0;
									switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
									{
									case AT_MAX_HP:
										iAbilityValue = pEffectedChar->Get_MaxHP();
										break;								
									}

									iIncValue = (short)( iAbilityValue * SKILL_CHANGE_ABILITY_RATE( iSkillIDX, i) / 100.f + SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i) * ( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT + 300 ) / 315.f );								
								}
								//--------------------------------------------------------------------------------------------

							}

							pEffectedChar->m_EndurancePack.SetStateValue( STATE_TYPE( iStateIndex ), iIncValue );

							/// »óÅÂ½ºÅ³ÀÌ °É¸±°æ¿ì AVATAR ÀÇ °æ¿ì´Â ´É·ÂÄ¡ ¾÷µ¥ÀÌÆ®..
							if( pEffectedChar->IsA( OBJ_USER ) )
							{
								(( CObjUSER* )pEffectedChar)->UpdateAbility();
							}
						}

					}else if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND ||
						SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND )
					{
						/// hp ´Â °è»ê½Ä Àû¿ë..
						int iIncValue = CCal::Get_SkillAdjustVALUE( (CObjUSER*)pEffectedChar, iSkillIDX, i, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nINT );

						switch( SKILL_INCREASE_ABILITY( iSkillIDX, i ) )
						{
						case AT_HP:
							pEffectedChar->Add_HP( iIncValue );
							break;
						case AT_MP:						
							pEffectedChar->Add_MP( iIncValue );
							break;
						case AT_STAMINA:
							{
								/// »óÅÂ½ºÅ³ÀÌ °É¸±°æ¿ì AVATAR ÀÇ °æ¿ì´Â ´É·ÂÄ¡ ¾÷µ¥ÀÌÆ®..
								if( pEffectedChar->IsA( OBJ_USER ) )
								{
									(( CObjUSER* )pEffectedChar)->AddCur_STAMINA( SKILL_INCREASE_ABILITY_VALUE( iSkillIDX, i ) );
								}								
							}
							break;
						default:
							g_itMGR.AppendChatMsg( "Unknown property..", IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						}
					}			
				} 				
			}
		}		
	}	
}

///
/// 7,17¹ø Å¸ÀÔÀÇ °æ¿ì. Damage °¡ Àû¿ëµÈ´Ù.
///
void CRecvPACKET::Recv_gsv_DAMAGE_OF_SKILL ()			
{
	Log_String( LOG_NORMAL, "½ºÅ³ µ¥¹ÌÁö ¹ÞÀ½[%d]\n", m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wDamage );


	if( m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS )
	{
		Recv_gsv_EFFECT_OF_SKILL();
	}


	CObjCHAR*			pObjCHAR		= NULL;
	int					iObjIDX			= 0;

	/// ½ºÅ³ ½ÃÀüÀÚ¿¡°Ô.. µî·Ï..
	short iSkillOwner = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wSpellObjIDX;
	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );


	/// Á×´Â µ¥¹ÌÁöÀÏ°æ¿ì.. ¾ÆÀÌÅÛ µå¶ø¸®½ºÆ® µî·Ï..	
	if ( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wDamage & DMG_BIT_DEAD ) 
	{

		//------------------------------------------------------------------------------------
		/// µ¥¹ÌÁö·Î´Â ¼ÒÈ¯¸÷ÀÎÁö ¸ð¸¥´Ù. ¸®½ºÆ®¿¡¼­ Ã£¾Æ¼­ ÀÖÀ¸¸é Áö¿ö¶ó. ¼ÒÈ¯¸÷ ¸®½ºÆ®¿¡¼­ »©¶ó..
		//------------------------------------------------------------------------------------			
		if( g_pAVATAR )
			g_pAVATAR->SubSummonedMob( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX );



		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_DAMAGE_OF_SKILL ) + sizeof( tag_DROPITEM ) ) ) 
		{
			// Á×À»¶§ µå·ÓµÈ ¾ÆÀÌÅÛÀÌ ÀÖ´Ù...

			short nOffset = sizeof( gsv_DAMAGE_OF_SKILL );
			tag_DROPITEM* pFieldItem = ( tag_DROPITEM* )Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_DROPITEM ) );

			CObjCHAR* pObjDead = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX, true );
			/// ¸÷ÀÌ Á×À¸¸é¼­ µå¶øÇÑ°ÍÀÌ´Ù.			
			if( pObjDead )
			{
				assert( ( pFieldItem->m_ITEM.GetTYPE() >= ITEM_TYPE_FACE_ITEM &&
					pFieldItem->m_ITEM.GetTYPE() <= ITEM_TYPE_RIDE_PART ) || 
					pFieldItem->m_ITEM.GetTYPE() == ITEM_TYPE_MONEY );

				pObjDead->PushFieldItemToList( *pFieldItem );
				pObjDead->m_bDead = true;
			}			
		}
	}


	/// Àå°Å¸® °ø°Ý ½ºÅ³ÀÇ°æ¿ì¿¡´Â µ¥¹ÌÁö·Î ÀüÈ¯ÇØ¼­ µî·ÏÇØÁØ´Ù.
	int iSkillType = SKILL_TYPE( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_nSkillIDX );
	if( iSkillType == SKILL_ACTION_ENFORCE_BULLET || 
		iSkillType == SKILL_ACTION_FIRE_BULLET )
	{
		iObjIDX = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX;
		CObjCHAR *pEffectedChar = g_pObjMGR->Get_ClientCharOBJ( iObjIDX, true );

		if(  pEffectedChar )
			pEffectedChar->ConvertDamageOfSkillToDamage( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL );
		else
		{
			/// ¹Ù·Î Àû¿ë..			

			if( pEffectedChar == NULL )
				return;

			pEffectedChar->Apply_DAMAGE( NULL, m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wDamage );
			pEffectedChar->CreateImmediateDigitEffect( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage.m_wVALUE );
		}

		return;
	}



	/// ÇöÀç »ç¿ëÁßÀÎ ½ºÅ³ÀÌ ¹ß»çÇüÀÌ¶ó¸é.. ¹Ù·Î Àû¿ëÇÑ´Ù.
	int iDoingSkillIDX = 0;

	if( pChar && pChar->m_nActiveSkillIDX )
		iDoingSkillIDX = pChar->m_nActiveSkillIDX;

	if( pChar && pChar->m_nToDoSkillIDX )
		iDoingSkillIDX = pChar->m_nToDoSkillIDX;    

	/*if( pChar && pChar->m_nToDoSkillIDX )
	iDoingSkillIDX = pChar->m_nDoingSkillIDX;   */ 


	if( ( pChar != NULL ) && 		
		/// ÇöÀç Ä³½ºÆÃ ÁßÀÌÁö ¾ÊÀº°ÍÀº.. ÀÌ¹Ì ¾×¼ÇÀÌ ÁøÇàµÇ¹ö·È´Ù.. ³Ê¹« ´Ê°Ô µµÂøÇÑ ÆÐÅ¶..
		iDoingSkillIDX )//&&
		/// ÃÑ¾Ë ¹ß»çÇü ½ºÅ³À» ½ÃÀüÁß ¿Â ½ºÅ³ Àû¿ë°á°ú´Â ( ´Ù Áö¼ÓÀÌ°ÚÁö? ) ¹Ù·Î Àû¿ë..
		//( !( SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_FIRE_BULLET ) || 
		//  (	SKILL_TYPE( iDoingSkillIDX ) == SKILL_ACTION_ENFORCE_BULLET ) ) )	
	{
		pChar->PushEffectedSkillToList( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_nSkillIDX,
			m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL, 
			m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_nINT,
			true );		
	}else
	{
		/// ¹Ù·Î Àû¿ë..	

		iObjIDX = m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX;
		///CObjCHAR *pEffectedChar = g_pObjMGR->Get_ClientCharOBJ( iObjIDX, true );
		CObjCHAR *pEffectedChar = CSkillManager::GetSkillTarget( iObjIDX, m_pRecvPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX );

		if( pEffectedChar == NULL )
			return;

		pEffectedChar->Apply_DAMAGE( NULL, m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_wDamage );
		pEffectedChar->CreateImmediateDigitEffect( m_pRecvPacket->m_gsv_DAMAGE_OF_SKILL.m_Damage.m_wVALUE );		
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief ¼­¹ö¿¡¼­ MP¸¦ ±ïÀºÈÄ ³¯¶ó¿À´Â ½ºÅ³
//----------------------------------------------------------------------------------------------------

void CRecvPACKET::Recv_gsv_SKILL_CANCEL()
{
	short iSkillOwner = m_pRecvPacket->m_gsv_SKILL_CANCEL.m_wObjectIDX;
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( iSkillOwner, true );

	if( pObjCHAR )
	{
		switch( m_pRecvPacket->m_gsv_SKILL_CANCEL.m_btWHY )
		{
		case SKILL_CANCEL_NEED_ABILITY:
			{					
				pObjCHAR->SetCMD_STOP();
			}
			break;
		case SKILL_CANCEL_NEED_TARGET:
			{
				pObjCHAR->SetCMD_STOP();
			}
			break;
			// ÄÉ½ºÆÃÈÄ ½ÇÁ¦µ¿ÀÛÀü Å¸°ÙÀÌ ¾ø¾îÁ³´Ù... MP¼Ò¸ð ÀÖÀ½
		case SKILL_CANCEL_TARGET_NOT_FOUND:
			{
				pObjCHAR->SetEffectedSkillFlag( true );		

				CSkillManager::UpdateUseProperty( pObjCHAR, pObjCHAR->GetLastCastingSkill() );

				if( pObjCHAR->IsA( OBJ_USER ) )
				{
					if( !CGame::GetInstance().GetRight() )
						g_itMGR.AppendChatMsg( "Å¸°Ù »ç¸Á ½ºÅ³Ãë¼Ò", IT_MGR::CHAT_TYPE_SYSTEM );
				}

				/// °á°ú°¡ ¿Â°Í¿¡ ´ëÇØ¼­¸¸ ¸¶³ª¸¦ ¼Ò¸ðÇÑ´Ù..
				/*if( pObjCHAR->m_nActiveSkillIDX )
				CSkillManager::UpdateUseProperty( pObjCHAR, pObjCHAR->m_nActiveSkillIDX );
				else
				k						assert( 0 && "Skill cancel[ Invalid skill index ]" );*/
			}
			break;
		}
	}
}


void CRecvPACKET::Recv_gsv_RESULT_OF_SKILL ()	
{
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_RESULT_OF_SKILL.m_wObjectIDX, false );

	if( pObjCHAR != NULL )
	{
		/*if( pObjCHAR->IsA( OBJ_USER ) )
		g_itMGR.AppendChatMsg( "½ºÅ³°á°ú ¹Þ¾Ò´Ù", g_dwRED );*/


		/// Å¥¿¡ ÀúÀåµÈ ½ºÅ³ ¸í·ÉÀ» µÚÁø´Ù.( start ¸¦ ¹ÞÀº ½ºÅ³ÀÌ ÀÖ´Ù¸é ±× ½ºÅ³¿¡ °á°ú¸¦ ¼¼ÆÃ.. )
		pObjCHAR->m_CommandQueue.SetValidResultOfSkill();
		pObjCHAR->SetEffectedSkillFlag( true );			


		/// °á°ú°¡ ¿Â°Í¿¡ ´ëÇØ¼­¸¸ ¸¶³ª¸¦ ¼Ò¸ðÇÑ´Ù..
		CSkillManager::UpdateUseProperty( pObjCHAR, m_pRecvPacket->m_gsv_RESULT_OF_SKILL.m_nSkillIDX );		

		Log_String( LOG_NORMAL, "½ºÅ³ °á°ú¹ÞÀ½[%d][%d]\n",pObjCHAR->bCanActionActiveSkill(), m_pRecvPacket->m_gsv_SKILL_START.m_wObjectIDX );
	}
	else
	{		
		assert( 0 && "recv result of skill" );
		return;
	}

	/// ÇöÀç »ç¿ëÁßÀÎ ½ºÅ³ÀÌ ¹ß»çÇüÀÌ¶ó¸é.. ¹Ù·Î Àû¿ëÇÑ´Ù.
	int iDoingSkillIDX = 0;

	if( pObjCHAR && pObjCHAR->m_nActiveSkillIDX )
		iDoingSkillIDX = pObjCHAR->m_nActiveSkillIDX;

	if( pObjCHAR && pObjCHAR->m_nToDoSkillIDX )
		iDoingSkillIDX = pObjCHAR->m_nToDoSkillIDX;    	

	/// ÀÌ¹Ì ¾×¼Çµ¿ÀÛÀÌ ½ÃÀÛÈÄ¿¡ ¿Â°ÍÀÌ¶ó¸é( ±×·²¸®´Â¾øÁö? ) ¹Ù·Î ¼öÇà..
	///if( pObjCHAR && pObjCHAR->m_nToDoSkillIDX )
	///	iDoingSkillIDX = pObjCHAR->m_nDoingSkillIDX;    	

	/// ½ºÅ³ ¾×¼ÇÀÌ ³¡³µ´Ù.. ( Ä³½ºÆÃ µ¿ÀÛÀÌ ¾øÀ»°æ¿ì ¹ß»ý°¡´É )
	if( iDoingSkillIDX == 0 )
	{
		pObjCHAR->ProcEffectedSkill();
	}
}


/// È¿°ú°¡ Á¾·áµÇ¸é¼­ ³­¶ó ¿À´Â°Å´Ï Client ¿¡¼­´Â »©ÁÖ¸é µÈ´Ù.
void CRecvPACKET::Recv_gsv_CLEAR_STATUS ()
{
	//TODO:: /// »óÅÂ°¡ ¹Ù²î¾ú´Ù...
	CObjCHAR *pObjCHAR = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_wObjectIDX, false );

	if ( pObjCHAR ) 
	{
		DWORD dwResult = pObjCHAR->m_EndurancePack.GetStateFlag() & (~m_pRecvPacket->m_gsv_CLEAR_STATUS.m_dwStatusFLAG);

		int iIndexA = 0;
		if( dwResult & FLAG_ING_HP )
		{			
			pObjCHAR->Set_HP( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ iIndexA ] );
			if( pObjCHAR->Get_HP() > pObjCHAR->Get_MaxHP() )
				pObjCHAR->Set_HP( pObjCHAR->Get_MaxHP() );

			iIndexA++;
		}
		if( dwResult & FLAG_ING_MP )
		{
			pObjCHAR->Set_MP( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ iIndexA ] );			
			if( pObjCHAR->Get_MP() > pObjCHAR->Get_MaxMP() )
				pObjCHAR->Set_MP( pObjCHAR->Get_MaxMP() );

			iIndexA++;
		}

		//Á¶¼ºÇö Ä³¸¯ÅÍ º¯½ÅÇÒ¶§...
		if ( (pObjCHAR->m_EndurancePack.GetStateFlag() & FLAG_ING_DUMMY_DAMAGE) && !(m_pRecvPacket->m_gsv_CLEAR_STATUS.m_dwStatusFLAG & FLAG_ING_DUMMY_DAMAGE) ) {
			pObjCHAR->m_bDisguise = false;   

			::setLightingRecursive(pObjCHAR->GetZMODEL(), g_GameDATA.m_hCharacterLight);
			::setVisibilityRecursive(pObjCHAR->GetZMODEL(), 1.0f);
			::setScale(pObjCHAR->GetZMODEL(), 1.0f, 1.0f, 1.0f);
			::setVisibleGlowRecursive( pObjCHAR->GetZMODEL(), 0, 1.0f, 1.0f, 1.0f );

		}

		///»õ·Î¿î Áö¼Ó»óÅÂ ÇÃ·¡±×·Î ±âÁ¸ »óÅÂ Á¾·á..
		pObjCHAR->m_EndurancePack.DeleteEnduranceState( m_pRecvPacket->m_gsv_CLEAR_STATUS.m_dwStatusFLAG );


	}
}

/// ¼Óµµ°¡ º¯°æµÆ´Ù.
/// ÀÚ½ÅÀÏ°æ¿ì ´É·ÂÄ¡¸¦ Àç°è»êÇØÁØ´Ù.			2005/07/12 - nAvy
void CRecvPACKET::Recv_gsv_SPEED_CHANGED ()
{
	CObjAVT *pAVTChar = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_SPEED_CHANGED.m_wObjectIDX, false);

	if ( pAVTChar ) {
		pAVTChar->SetOri_RunSPEED( m_pRecvPacket->m_gsv_SPEED_CHANGED.m_nRunSPEED );			// ÆÐ½Ãºê »óÅÂ¸¦ Æ÷ÇÔ, Áö¼Ó »óÅÂ Á¦¿Ü
		pAVTChar->SetPsv_AtkSPEED( m_pRecvPacket->m_gsv_SPEED_CHANGED.m_nPsvAtkSPEED );			// ÆÐ½Ãºê °ª¸¸...

		if( g_pAVATAR && pAVTChar->IsA( OBJ_USER ))
			g_pAVATAR->UpdateAbility();

		///TODO::
		m_pRecvPacket->m_gsv_SPEED_CHANGED.m_btWeightRate;		// ÇöÀç¼ÒÁö·®/ÃÖ´ë¼ÒÁö·®*100
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_P_STORE_OPENED	()
{
	CObjAVT *pAVTChar = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_P_STORE_OPENED.m_wObjectIDX, true );
	if( pAVTChar )
	{
		if ( m_pRecvPacket->m_HEADER.m_nSize > ( sizeof( gsv_P_STORE_OPENED ) ) ) 
		{
			short nOffset = sizeof( gsv_P_STORE_OPENED );
			short* pPersonalStoreType = (short*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(short) );
			char* pszTitle = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

			pAVTChar->SetPersonalStoreTitle( pszTitle, 
				*pPersonalStoreType );
			g_UIMed.AddPersonalStoreIndex( pAVTChar->Get_INDEX() );
		}else
			assert( 0 && " Recv_gsv_P_STORE_OPENED.. but strTitle is null" );
	}	
}

//-------------------------------------------------------------------------------------------------
#include "../interface/dlgs/CAvatarStoreDlg.h"
void CRecvPACKET::Recv_gsv_P_STORE_CLOSED	()
{
	CObjAVT *pAVTChar = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_P_STORE_CLOSED.m_wObjectIDX, true );
	if( pAVTChar )
	{
		pAVTChar->SetPersonalStoreTitle( NULL );
		g_UIMed.SubPersonalStoreIndex( pAVTChar->Get_INDEX() );

		if( g_itMGR.IsDlgOpened( DLG_TYPE_AVATARSTORE ) )
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
			if( pDlg )
			{
				CAvatarStoreDlg* pAvtStoreDlg = ( CAvatarStoreDlg*)pDlg;
				if( pAvtStoreDlg->GetMasterSvrObjIdx() == m_pRecvPacket->m_gsv_P_STORE_CLOSED.m_wObjectIDX )
				{
					g_itMGR.CloseDialog( DLG_TYPE_AVATARSTORE );
					g_itMGR.AppendChatMsg(STR_STORE_IS_CLOSED, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}
		}

	}
}

//-------------------------------------------------------------------------------------------------
#include "../interface/dlgs/CAvatarStoreDlg.h"
void CRecvPACKET::Recv_gsv_P_STORE_LIST_REPLY ()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
	if( pDlg )
	{
		CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
		///ÆÇ¸ÅÈñ¸Á ¸ñ·Ï
		for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT; ++i )
			pStoreDlg->AddItem2SellList( m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[i] );

		///±¸ÀÔÈñ¸Á ¸ñ·Ï
		for( int i = m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT; 
			i < m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btSellItemCNT + m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_btWishItemCNT;
			++i)
		{
			pStoreDlg->AddItem2BuyList( m_pRecvPacket->m_gsv_P_STORE_LIST_REPLY.m_SlotITEMs[i] );
		}

		g_itMGR.OpenDialog( DLG_TYPE_AVATARSTORE );
	}	


	///CAvatarStoreDlg¿¡ ¾ÆÀÌÅÛÀ» Ãß°¡ÇÏ°í
	///CAvatarStoreDlg¸¦ ¿¬´Ù.
}
//-------------------------------------------------------------------------------------------------
#include "../GameData/CPrivateStore.h"
void CRecvPACKET::Recv_gsv_P_STORE_RESULT()
{
	switch(m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btResult)	
	{

	case RESULT_P_STORE_BOUGHT_PART:// ±¸ÀÔ ¿Ï·á...´Ü, ÀÏºÎ Ç°¸ñÀº Ç°ÀýµÇ¾î ±¸ÀÔ ½ÇÆÐ...
	case RESULT_P_STORE_BOUGHT_ALL:	//			0x05	// ¿äÃ»ÇÑ Ç°¸ñ ÀüÃ¼ ±¸ÀÔ ¿Ï·á 
		{
			if( g_pAVATAR )
			{
				///³»°¡ °³¼³ÇÑ »óÁ¡¿¡¼­ °Å·¡°¡ ¿Ï·áµÇ¾úÀ»¶§
				if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_wStoreObjectIDX 
					== g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) )
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
							CPrivateStore::GetInstance().UpdateSellList(  m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					}
				}
				else///°³ÀÎ»óÁ¡ÀÌ¿ëÀÌ ¼º°øÇßÀ»¶§
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
						if( pDlg )
						{
							CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
							for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
								pStoreDlg->UpdateSellList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );
						}
					}
				}
			}
			break;
		}
	case RESULT_P_STORE_SOLD_PART:	// ÆÇ¸Å ¿Ï·á...´Ü, ÀÏºÎ Ç°¸ñÀº ±¸ÀÔ¿Ï·áµÇ¾î ÆÇ¸Å ½ÇÆÐ...
	case RESULT_P_STORE_SOLD_ALL:	//				0x07	// ¿äÃ»ÇÑ Ç°¸ñ ÀüÃ¼ ÆÇ¸Å ¿Ï·á
		{
			if( g_pAVATAR )
			{
				///³»°¡ °³¼³ÇÑ »óÁ¡¿¡¼­ °Å·¡°¡ ¿Ï·áµÇ¾úÀ»¶§
				if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_wStoreObjectIDX 
					== g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) )
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
							CPrivateStore::GetInstance().UpdateBuyList(  m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					}
				}
				else///°³ÀÎ»óÁ¡ÀÌ¿ëÀÌ ¼º°øÇßÀ»¶§
				{
					if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
					{
						CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
						if( pDlg )
						{
							CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
							for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
								pStoreDlg->UpdateBuyList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );
						}
					}
				}
			}
			break;
		}
		break;
	case RESULT_P_STORE_CANCLED:
		g_itMGR.AppendChatMsg( STR_TRADE_IS_CANCEL, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case RESULT_P_STORE_TRADE_SOLD_OUT:
		{
			if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
			{
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
				if( pDlg )
				{
					CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
					for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
						pStoreDlg->UpdateSellList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					g_itMGR.AppendChatMsg( STR_ITEM_IS_SOLD, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}
			break;
		}
	case RESULT_P_STORE_TRADE_NO_MORE_NEED:
		{
			if( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT > 0 )
			{
				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
				if( pDlg )
				{
					CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
					for( int i = 0; i < m_pRecvPacket->m_gsv_P_STORE_RESULT.m_btItemCNT; ++i )
						pStoreDlg->UpdateBuyList( m_pRecvPacket->m_gsv_P_STORE_RESULT.m_UpdatedITEM[i] );

					g_itMGR.AppendChatMsg( STR_ITEM_IS_BUY_ALL, IT_MGR::CHAT_TYPE_SYSTEM );
				}
			}
			break;
		}
	default:
		break;
	}


}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_QUEST_REPLY ()
{
	if( !g_pAVATAR )
		return;

	g_pAVATAR->m_bQuestUpdate = true;

	switch( m_pRecvPacket->m_gsv_QUEST_REPLY.m_btResult ) 
	{
	case RESULT_QUEST_REPLY_ADD_SUCCESS	:
		/// Äù½ºÆ® µî·Ï
		if ( !g_pAVATAR->Quest_Append(	m_pRecvPacket->m_gsv_QUEST_REPLY.m_btQuestSLOT,
			m_pRecvPacket->m_gsv_QUEST_REPLY.m_iQuestID ) ) {
				/// ¹¹Áö?? ¼­¹ö¿¡¼± µî·ÏµÆ´Âµ¥ ???
				break;
			}

#ifndef	__SERVER
			//--------------------------------------------------------------------------------
			LOGOUT( "!!!Äù½ºÆ® µî·Ï!!!" );
			//--------------------------------------------------------------------------------
#endif

			break;
	case RESULT_QUEST_REPLY_ADD_FAILED	:
		break;
	case RESULT_QUEST_REPLY_DEL_SUCCESS	:
		/// Äù½ºÆ® »èÁ¦
		if ( !g_pAVATAR->Quest_Delete(	m_pRecvPacket->m_gsv_QUEST_REPLY.m_btQuestSLOT,
			m_pRecvPacket->m_gsv_QUEST_REPLY.m_iQuestID ) ) 
		{
			/// ÀÌ°Ç¶Ç ¹¹¾ß?? ¼­¹ö¿¡¼± »èÁ¦µÆ´Âµ¥...
			break;
		}

#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGOUT( "!!!Äù½ºÆ® »èÁ¦!!!" );
		//--------------------------------------------------------------------------------
#endif

		break;
	case RESULT_QUEST_REPLY_DEL_FAILED	:
		break;
	case RESULT_QUEST_REPLY_TRIGGER_SUCCESS :

#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGOUT( "!!!Äù½ºÆ® º¸»ó ½ÇÇà!!!" );
		//--------------------------------------------------------------------------------
#endif

		/// Äù½ºÆ® º¸»ó ½ÇÇà.
		g_QuestList.CheckQUEST( g_pAVATAR, m_pRecvPacket->m_gsv_QUEST_REPLY.m_TriggerHash, true );
		break;
	case RESULT_QUEST_REPLY_TRIGGER_FAILED	:
#ifndef	__SERVER
		//--------------------------------------------------------------------------------
		LOGOUT( "!!!Æ®¸®°Å ½ÇÆÐ!!!" );
		//--------------------------------------------------------------------------------
#endif		
		break;
	}

	///Äù½ºÆ® ¸®½ºÆ® °»½Å
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUEST );
	if( pDlg && pDlg->IsVision() )
	{
		CQuestDlg* pQuestDlg = ( CQuestDlg* ) pDlg;
		pQuestDlg->UpdateQuestList();
	}
}

///Æ®·¹ÀÌµå °ü·Ã 
void CRecvPACKET::Recv_gsv_TRADE_P2P()
{
	switch( m_pRecvPacket->m_gsv_TRADE_P2P.m_btRESULT )
	{ 

		///1.³»°¡ ¿ä±¸¸¦ ¹ÞÀº°æ¿ì
	case RESULT_TRADE_REQUEST:
		{
			///Á×Àº³ÑÀº °Å·¡ÇÏÁö¸¶!!!!
			CObjCHAR* pObjChar = g_pObjMGR->Get_ClientCharOBJ ( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX , true );
			if( pObjChar )
			{
				if(CCountry::GetSingleton().IsJapan())
				{
					if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_TRADE) )
					{
						g_pNet->Send_cli_TRADE_P2P(  m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX  , RESULT_TRADE_NO_CHARGE_TARGET  );
						return;
					}        
				}

				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_EXCHANGE );
				if( pDlg && !pDlg->IsVision() && !pDlg->IsInValidShow() && g_itMGR.FindMsgBox(CMsgBox::MSGTYPE_RECV_TRADE_REQ) == NULL && g_ClientStorage.IsApproveExchange() )
				{
					char szTemp[128];
					sprintf( szTemp,FORMAT_STR_RECEIVE_TRADE_REQ, pObjChar->Get_NAME() );


					CTCommand* pCmdOk     = new CTCmdAcceptTradeReq( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX );
					CTCommand* pCmdCancel = new CTCmdRejectTradeReq( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX );

					g_itMGR.OpenMsgBox( szTemp, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false , 0, pCmdOk, pCmdCancel ,CMsgBox::MSGTYPE_RECV_TRADE_REQ);
				}
				else
				{

					g_pNet->Send_cli_TRADE_P2P(  m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX  , RESULT_TRADE_BUSY  );
				}
			}
			break;
		}
		///»ó´ë¹æÀÌ °Å·¡¸¦ µµÁß¿¡ Áß´ÜµÇ¾ú´Ù
	case RESULT_TRADE_CANCEL:
		{
			g_itMGR.CloseDialog( DLG_TYPE_EXCHANGE );				
			g_itMGR.OpenMsgBox( STR_OTHER_CANCEL_TRADE, CMsgBox::BT_OK , true );
			break;
		}
		///»ó´ë¹æÀÌ ¹°°ÇÀ» ´Ù ¿Ã¸®°í ÁØºñ»óÅÂÀÌ´Ù.
	case RESULT_TRADE_CHECK_READY:
		{   
			CExchange::GetInstance().SetReadyOther( true );
			break;
		}
	case RESULT_TRADE_UNCHECK_READY:
		{	
			CExchange::GetInstance().SetReadyOther( false );
			break;
		}
		///»ó´ë¹æÀÇ ÀÎº¥Åä¸®°¡ ºÎÁ·ÇÏ´Ù, ³»°¡ ¿Ã¸° ¾ÆÀÌÅÛÀ» ´Ù½Ã »©ÀÚ
	case RESULT_TRADE_OUT_OF_INV:
		{
			CExchange::GetInstance().RemoveMyItemBySlotIndex( m_pRecvPacket->m_cli_TRADE_P2P.m_cTradeSLOT );
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_OTHER_INVENTORY_SPACE );
			break;
		}
		///°Å·¡ ¿Ï·á
	case RESULT_TRADE_DONE:
		{
			g_itMGR.CloseDialog( DLG_TYPE_EXCHANGE );
			CExchange::GetInstance().EndExchange();
			g_itMGR.OpenMsgBox( STR_SUCCESS_EXCHANGE, CMsgBox::BT_OK, false );
			break;
		}
		///»ó´ë¹æÀÌ ±³È¯¹öÆ°À» ´©¸£°í ´ë±âÁßÀÌ´Ù.
		///*-------------------------------------------------------------------------------------*/
		///2.³»¿ä±¸ÀÇ ÀÀ´ä
	case RESULT_TRADE_BUSY:
		{
			g_itMGR.OpenMsgBox( STR_OTHER_CANT_RECEIVE_TRADE_REQ, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_NO_CHARGE_TARGET:
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_TRADE_DONT_PAY_TARGET, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_ACCEPT:
		{
			CObjCHAR* pObjChar = g_pObjMGR->Get_ClientCharOBJ( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX, true );
			if( pObjChar == NULL )///»ó´ë¹æÀÌ ¾øÀ»°æ¿ì ¹«½Ã
				break;

			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_EXCHANGE );
			if( pDlg )
			{
				CExchange::GetInstance().StartExchange( m_pRecvPacket->m_gsv_TRADE_P2P.m_wObjectIDX , pObjChar->Get_NAME() );

				int iPosX = g_pCApp->GetWIDTH() / 2 - pDlg->GetWidth();
				int iPosY = g_pCApp->GetHEIGHT() / 2 - pDlg->GetHeight() / 2;

				g_itMGR.OpenDialog( DLG_TYPE_EXCHANGE ,  true, iPosX , iPosY );	
				iPosX = g_pCApp->GetWIDTH() / 2;
				g_itMGR.OpenDialog( DLG_TYPE_ITEM , false , iPosX, iPosY );
			}
			break;
		}
	case RESULT_TRADE_REJECT:
		{
			g_itMGR.OpenMsgBox( STR_OTHER_REJECT_TRADE_REQ, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_TOO_FAR:
		{
			g_itMGR.OpenMsgBox( STR_OTHER_IS_TOO_FAR_TO_TRADE, CMsgBox::BT_OK , true );
			break;
		}
	case RESULT_TRADE_NOT_TARGET:
		{
			g_itMGR.OpenMsgBox( STR_NOT_FOUND_TRADE_OTHER, CMsgBox::BT_OK , true );
			break;
		}
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_TRADE_P2P_ITEM()
{
	if( CExchange::GetInstance().IsReadyMe() )
		g_itMGR.OpenMsgBox( STR_CHANGE_TRADEITEM_IN_MYREADY );

	CExchange::GetInstance().UpdateOtherItem( m_pRecvPacket->m_gsv_TRADE_P2P_ITEM.m_cTradeSLOT, m_pRecvPacket->m_gsv_TRADE_P2P_ITEM.m_ITEM );
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_PARTY_REQ()
{
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REQ_1 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REQ.m_btREQUEST, g_GameDATA.GetGameTime() );

	WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REQ.m_dwFromIDXorTAG;
	CObjAVT* pObjChar		= g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), true );
	CParty& Party = CParty::GetInstance();


	switch( m_pRecvPacket->m_gsv_PARTY_REQ.m_btREQUEST )
	{
	case PARTY_REQ_JOIN:///ÆÄÆ¼ÀåÀÌ ÆÄÆ¼¿¡ µé¾î¿Ã°ÍÀ» ¿äÃ»Çß´Ù.
		{
			if(CCountry::GetSingleton().IsJapan())
			{
				if( !(CGame::GetInstance().GetPayType() & PAY_FLAG_JP_BATTLE) )
				{
					g_pNet->Send_cli_PARTY_REPLY( PAATY_REPLY_NO_CHARGE_TARGET, wObjSvrIdx );
					return;
				}        
			}

			if( !Party.IsValidJoinParty()  || g_itMGR.IsOpenedMsgBox( CMsgBox::MSGTYPE_RECV_PARTY_REQ ) || !g_ClientStorage.IsApproveParty() )
				g_pNet->Send_cli_PARTY_REPLY( PARTY_REPLY_BUSY, wObjSvrIdx );
			else
			{
				if( pObjChar )
				{

					sprintf( g_MsgBuf,FORMAT_STR_PARTY_INVITE,pObjChar->Get_NAME() );
					CTCmdAcceptPartyJoin* pCmdOk		= new CTCmdAcceptPartyJoin( wObjSvrIdx );
					CTCmdRejectPartyJoin* pCmdCancel	= new CTCmdRejectPartyJoin( wObjSvrIdx );

					g_itMGR.OpenMsgBox( g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0, pCmdOk, pCmdCancel , CMsgBox::MSGTYPE_RECV_PARTY_REQ );
				}
			}
			break;
		}
	case PARTY_REQ_MAKE:///ÆÄÆ¼ÁßÀÌ ¾Æ´Ñ ´Ù¸¥ ¾Æ¹ÙÅ¸°¡ ÆÄÆ¼ °á¼ºÀ» ¿äÃ»Çß´Ù.
		{
			if(CCountry::GetSingleton().IsJapan())
			{
				if( !(CGame::GetInstance().GetPayType() & PAY_FLAG_JP_BATTLE) )
				{
					g_pNet->Send_cli_PARTY_REPLY( PAATY_REPLY_NO_CHARGE_TARGET, wObjSvrIdx );
					return;
				}        
			}

			if( !Party.IsValidJoinParty() || g_itMGR.IsOpenedMsgBox( CMsgBox::MSGTYPE_RECV_PARTY_REQ ) || !g_ClientStorage.IsApproveParty())
				g_pNet->Send_cli_PARTY_REPLY( PARTY_REPLY_BUSY, wObjSvrIdx );
			else
			{
				if( pObjChar )
				{

					sprintf( g_MsgBuf,FORMAT_STR_PARTY_MAKE_REQ,pObjChar->Get_NAME() );

					CTCmdAcceptPartyJoin* pCmdOk		= new CTCmdAcceptPartyJoin( wObjSvrIdx );
					CTCmdRejectPartyJoin* pCmdCancel	= new CTCmdRejectPartyJoin( wObjSvrIdx );

					g_itMGR.OpenMsgBox( g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0, pCmdOk, pCmdCancel , CMsgBox::MSGTYPE_RECV_PARTY_REQ );
				}
			}
			break;
		}

	default:
		break;
	}
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REQ_2 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REQ.m_btREQUEST, g_GameDATA.GetGameTime() );

}
void CRecvPACKET::Recv_gsv_PARTY_REPLY()
{
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REPLY_1 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REPLY.m_btREPLY,g_GameDATA.GetGameTime() );
	CParty& Party = CParty::GetInstance();

	switch( m_pRecvPacket->m_gsv_PARTY_REPLY.m_btREPLY )
	{
	case PARTY_REPLY_INVALID_LEVEL:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			CObjAVT* pObjAvt = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), true);
			if( pObjAvt )
			{

				sprintf( g_MsgBuf,"%s%s", pObjAvt->Get_NAME(),STR_CANT_JOIN_PARTY_REASON_LEVEL);
				g_itMGR.AppendChatMsg( g_MsgBuf, IT_MGR::CHAT_TYPE_SYSTEM);
			}
			break;
		}
	case PARTY_REPLY_CHANGE_OWNERnDISCONN:
		{
			DWORD dwLeaderTag = Party.GetLeaderObjectTAG();
			CParty::GetInstance().MemberDisconnected( dwLeaderTag );

			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			Party.ChangeLeaderByObjSvrIdx( wObjSvrIdx );
			break;
		}
	case PARTY_REPLY_CHANGE_OWNER:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			Party.ChangeLeaderByObjSvrIdx( wObjSvrIdx );
			break;
		}
	case PARTY_REPLY_BAN:
		{
			DWORD dwObjectTag = m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			Party.BanMember( dwObjectTag );
			break;
		}
	case PARTY_REPLY_NOT_FOUND:
		g_itMGR.AppendChatMsg(STR_PARTY_NOTFOUND_DEST, IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case PARTY_REPLY_BUSY:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), false );
			if( pObjChar )
				g_itMGR.AppendChatMsg( CStr::Printf( FORMAT_STR_PARTY_BUSY,pObjChar->Get_NAME() ), IT_MGR::CHAT_TYPE_SYSTEM);

			break;
		}
	case PAATY_REPLY_NO_CHARGE_TARGET:
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_INVITED_PARTY );
		}
		break;

	case PARTY_REPLY_ACCEPT_MAKE:///ÀÚ½ÅÀÌ ÆÄÆ¼°á¼ºÀ» ¿äÃ»ÇØ¼­ »ó´ë¹æÀÌ ÀÀ´äÇßÀ»°æ¿ì
		{
			if( g_pAVATAR )
			{
				g_itMGR.AppendChatMsg(STR_PARTY_MADE, IT_MGR::CHAT_TYPE_SYSTEM);
				CParty::GetInstance().SetLeaderObjSvrIdx( g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX()));
				CParty::GetInstance().SetLeaderObjectTag( g_pAVATAR->GetUniqueTag() );
			}
			break;
		}
	case PARTY_REPLY_ACCEPT_JOIN:
		{
			break;
		}
	case PARTY_REPLY_REJECT_JOIN:
		{
			WORD wObjSvrIdx = (WORD)m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG;
			CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( g_pObjMGR->Get_ClientObjectIndex( wObjSvrIdx ), false );
			if( pObjChar )
			{

				sprintf( g_MsgBuf ,FORMAT_STR_PARTY_REJECT_INVITE,pObjChar->Get_NAME() );
				g_itMGR.OpenMsgBox( g_MsgBuf,CMsgBox::BT_OK, false  );
			}
			break;
		}
	case PARTY_REPLY_DESTROY:
		{
			CParty::GetInstance().Destory();
			break;
		}
	case PARTY_REPLY_REJOIN:
		{
			//CParty::GetInstance().Join();
			break;
		}
	case PARTY_REPLY_DISCONNECT:
		{
			CParty::GetInstance().MemberDisconnected( m_pRecvPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG );
			///ºñÁ¤»ó Á¾·áµÇ¾ú´Ù.±×·¯¸é Object Index¸¦ 0À¸·Î ¸¸µé±î? ±×¸®°í À§ÀÓÀº ¾ÈµÇ°Ô ÇÏÀÚ.
			break;
		}
	default:
		break;
	}
	_RPT2( _CRT_WARN,"Recv_gsv_PARTY_REPLY_2 %d(%d)\n",m_pRecvPacket->m_gsv_PARTY_REPLY.m_btREPLY,g_GameDATA.GetGameTime() );
}
void CRecvPACKET::Recv_gsv_PARTY_MEMBER()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_MEMBER_1 %d\n",g_GameDATA.GetGameTime() );
	short  nOffset = sizeof( gsv_PARTY_MEMBER );

	tag_PARTY_MEMBER* pMember = NULL;
	char*	pszName	= NULL;

	CParty& Party = CParty::GetInstance();

	if( Party.HasParty() )
	{
		if( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_cUserCNT == PARTY_MEMBER_SUB )///Leave
		{
			Party.LeaveMember( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[0] );

			if( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[0] == Party.GetLeaderObjectTAG() )
				Party.GetInstance().ChangeLeaderByTag( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[1] );

		}
		else///Join
		{
			pMember = (tag_PARTY_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_PARTY_MEMBER ) );
			pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pMember && pszName );
			if( pMember && pszName )
				Party.JoinMember( *pMember, pszName );
		}
	}
	else///»õ·Î¿î ÆÄÆ¼°¡ °á¼ºµÇ¾úÀ»¶§
	{
		///»õ·Ó°Ô ÆÄÆ¼¸¦ ¸¸µé°í ÀÌ°÷¿¡ °¡ÀÔÇÑ »ç¶÷ÀÌ´Ù.
		if( Party.IsPartyLeader() )///³ª´Â ³»·Á¿ÀÁö ¾Ê´Â´Ù.
		{
			CParty::GetInstance().Make();
			pMember = (tag_PARTY_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_PARTY_MEMBER ) );
			pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pMember && pszName );
			if( pMember && pszName )
				Party.JoinMember( *pMember, pszName );

			Party.RecvPartyRule( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_btPartyRULE );
		}
		else///Ã³À½À¸·Î ³»·Á¿Â »ç¶÷ÀÌ ÆÄÆ¼ÀåÀÌ´Ù.///³ªµµ ³»·Á¿À³ª?
		{
			CParty::GetInstance().Join();
			for( char i = 0; i < m_pRecvPacket->m_gsv_PARTY_MEMBER.m_cUserCNT; ++i )
			{
				pMember = (tag_PARTY_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_PARTY_MEMBER ) );
				pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				assert( pMember && pszName );
				if( pMember && pszName )
				{
					Party.JoinMember( *pMember, pszName );
					if( i == 0 )///ÆÄÆ¼ÀåÀÌ¸é
					{
						Party.ChangeLeaderByObjSvrIdx( pMember->m_wObjectIDX );
					}
				}
			}
			Party.RecvPartyRule( m_pRecvPacket->m_gsv_PARTY_MEMBER.m_btPartyRULE );
		}
	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_MEMBER_2 %d\n",g_GameDATA.GetGameTime() );
} 

void CRecvPACKET::Recv_gsv_PARTY_LEVnEXP()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_LEVnEXP_1(%d)\n" ,g_GameDATA.GetGameTime() );

	CParty::GetInstance().SetExp( m_pRecvPacket->m_gsv_PARTY_LEVnEXP.m_iEXP );
	CParty::GetInstance().SetLevel( m_pRecvPacket->m_gsv_PARTY_LEVnEXP.m_btLEVEL );
	///·¹º§¾÷½Ã¿¡
	if( m_pRecvPacket->m_gsv_PARTY_LEVnEXP.m_bitLevelUP )
	{
		CParty::GetInstance().LevelUp();

		//----------------------------------------------------------------------------------------------------			
		/// @brief ÆÄÆ¼¿øµé¿¡°Ô ÀÌÆåÆ®¸¦ ºÙÀÎ´Ù.
		//----------------------------------------------------------------------------------------------------
		if( g_pAVATAR )
			SE_PartyLevelUp( g_pAVATAR->Get_INDEX() );	

	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_LEVnEXP_2(%d)\n" ,g_GameDATA.GetGameTime() );
}

///ÀÏ´Ü ÀúÀåÇÏ°í Object List¿¡ ÀÖ´Â AVATAR¶ó¸é ±×°÷¿¡µµ Data¸¦ ³Ö¾îÁØ´Ù.
///¼­¹ö ±¸Á¶°¡ ¹Ù²ñ¿¡ µû¶ó ¾Æ¸¶µµ ¼­¹ö¿¡¼­´Â ÀÌÁ¦ ¾È¿À´Â ÆÐÅ¶ÀÏ°Í °°Àºµ¥
///2004/7/15 - ÆÄÆ¼Áß ³»°¡ Á×¾úÀ»°æ¿ì ObjIndex°¡ º¯ÇØµµ ¿ÀÁö¾Ê´Â´Ù.JOIN_ZONE¿¡¼­ Ã³¸®ÇÑ´Ù.
void CRecvPACKET::Recv_gsv_CHANGE_OBJIDX()
{
	_RPT1( _CRT_WARN,"Recv_gsv_CHANGE_OBJIDX-1(%d)\n", g_GameDATA.GetGameTime() );
	if( CParty::GetInstance().ChangeMemberInfoByUserTag( m_pRecvPacket->m_gsv_CHANGE_OBJIDX.m_Member ) )
	{
		CObjAVT *pObjAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_wObjectIDX , false ); 
		if ( pObjAVT ) 
		{
			tag_PARTY_MEMBER* pMemberInfo = &(m_pRecvPacket->m_gsv_CHANGE_OBJIDX.m_Member);
			if( pMemberInfo )///ÆÄÆ¼¿øÀÏ°æ¿ì
			{
				pObjAVT->Set_HP( pMemberInfo->m_nHP );
				pObjAVT->Set_MaxHP( pMemberInfo->m_nMaxHP );
				pObjAVT->Set_CON( pMemberInfo->m_nCON );
				pObjAVT->Set_AddRecoverHP( pMemberInfo->m_btRecoverHP );
				pObjAVT->Set_AddRecoverMP( pMemberInfo->m_btRecoverMP );
				pObjAVT->SetStamina( pMemberInfo->m_nSTAMINA );
			}
		}
	}
	_RPT1( _CRT_WARN,"Recv_gsv_CHANGE_OBJIDX-2(%d)\n", g_GameDATA.GetGameTime() );

}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CREATE_ITEM_REPLY()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_MAKE );
	if( pDlg && pDlg->IsVision() )
	{
		CMakeDLG* pMakeDlg = ( CMakeDLG* ) pDlg;
		pMakeDlg->RecvResult( m_pRecvPacket );
	}
}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_BANK_LIST_REPLY()
{
	if( !g_pAVATAR )
		return;

	switch( m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btREPLY )
	{
	case BANK_REPLY_INIT_DATA:
		{
			g_pAVATAR->InitBank();///
			for( BYTE bt = 0; bt < m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;++bt )
				g_pAVATAR->SetBankItem(  m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_btInvIDX, 
				m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_ITEM);

			///ÀºÇà¿¡ º¸°üµÈ µ·Á¤º¸°¡ ºÙ¾î ÀÖÀ»¶§
			short Not_IncludeMoneyPacketSize = sizeof( gsv_BANK_LIST_REPLY ) + sizeof(tag_SET_INVITEM) * m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;

			if( m_pRecvPacket->m_HEADER.m_nSize  == Not_IncludeMoneyPacketSize + 8 )
			{
				__int64* money = (__int64*)Packet_GetDataPtr( m_pRecvPacket, Not_IncludeMoneyPacketSize, sizeof(__int64) );
				CBank::GetInstance().SetMoney( *money );
			}
		}
	case BANK_REPLY_SHOW_DATA:
		g_itMGR.OpenDialog( DLG_TYPE_ITEM, false );
		g_itMGR.OpenDialog( DLG_TYPE_BANK ,false );
		break;
	case BANK_REPLY_PLATINUM:///INIT_DATAº¸´Ù ³ªÁß¿¡ ¿Í¾ßµÈ´Ù.
		{
			for( BYTE bt = 0; bt < m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_btItemCNT;++bt )
				g_pAVATAR->SetBankItem(  m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_btInvIDX, 
				m_pRecvPacket->m_gsv_BANK_LIST_REPLY.m_sInvITEM[bt].m_ITEM);
		}
		break;
	case BANK_REPLY_NEED_PASSWORD:// Ã¢°í ºñ¹øÀÌ ÇÊ¿ä..
		g_itMGR.OpenMsgBox(STR_BANK_REPLY_NEED_PASSWORD);
		break;
	case BANK_REPLY_CHANGED_PASSWORD:// Ã¢°í ºñ¹øÀÌ º¯°æµÆÀ½
		g_itMGR.OpenMsgBox(STR_BANK_REPLY_CHANGED_PASSWORD);
		break;
	case BANK_REPLY_INVALID_PASSWORD:// Ã¢°í ºñ¹øÀÌ Æ²¸²
		g_itMGR.OpenMsgBox(STR_BANK_REPLY_INVALID_PASSWORD);
		break;
	case BANK_REPLY_CLEARED_PASSWORD:// Ã¢°í ºñ¹ø »èÁ¦µÊ
		g_itMGR.OpenMsgBox( STR_BANK_REPLY_CLEARED_PASSWORD);
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_MOVE_ITEM()
{
	if( !g_pAVATAR )
		return;

	if( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nInvIDX >= 0 )
		g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nInvIDX, m_pRecvPacket->m_gsv_MOVE_ITEM.m_InvItem );

	if( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nBankIDX >= 0 )
		g_pAVATAR->SetBankItem( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nBankIDX, m_pRecvPacket->m_gsv_MOVE_ITEM.m_BankITEM );

	// ÆÐÅ¶ »çÀÌÁî == gsv_MOVE_ITEM ÀÌ¸é				Ã¢°í=>ÀÎº¥Åä¸® ÀÌµ¿
	// ÆÐÅ¶ »çÀÌÁî == gsv_MOVE_ITEM+sizeof(__int64)¸é	ÀÎº¥=>Ã¢°í, m_iCurMoney[0]¿¡ µ·µé¾î ÀÖÀ½
	if( m_pRecvPacket->m_HEADER.m_nSize > sizeof( gsv_MOVE_ITEM) )
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_MOVE_ITEM.m_iCurMoney[0] );

	if( m_pRecvPacket->m_gsv_MOVE_ITEM.m_nInvIDX < 0 && m_pRecvPacket->m_gsv_MOVE_ITEM.m_nBankIDX < 0 )
		g_itMGR.AppendChatMsg(STR_BANK_MOVEITEM_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );

}
//-------------------------------------------------------------------------------------------------
///2004/ 2 / 18 : Create : nAvy : ¼Ò¸ðÅº °ü·Ã
///2004/ 2 / 19 : Modify : nAvy : ÇöÀç ¼Ò¸ðÅºÀÌ ´Ù »ç¿ëµÈ°æ¿ì Inventory¿¡¼­ »èÁ¦ÇØÁØ´Ù.
void CRecvPACKET::Recv_gsv_SET_BULLET()
{
	CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex(m_pRecvPacket->m_gsv_SET_BULLET.m_wObjectIDX ), false);
	if( pObjAVT )
	{
		tagShotDATA ShotData = m_pRecvPacket->m_gsv_SET_BULLET.m_sShot;
		pObjAVT->SetShotData( ShotData.m_cType, ShotData.m_nItemNo );
	}
}
//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_SERVER_DATA()
{
	switch( m_pRecvPacket->m_gsv_SERVER_DATA.m_btDataTYPE )
	{
	case SERVER_DATA_ECONOMY:
		{
			struct EconomyData{
				DWORD	m_nTown_COUNTER;					// Ä«¿îÅÍ 1ºÐ¿¡ 1¾¿ °¨¼Ò.		50~100

				short	m_nTown_POP_BASE;					// ±âÁØ ÀÎ±¸¼ö.					100~8000
				short	m_nTown_DEV_BASE;					// ±âÁØ ¹ßÀüµµ					10~100
				short	m_nTown_CONSUM[ MAX_PRICE_TYPE ];	// ¾ÆÀÌÅÛº° ¼Òºñ·®

				short	m_nTownDEV;							// ¸¶À» ¹ßÀüµµ
				int		m_iTownPOP;							// ¸¶À» ÀÎ±¸.
				int		m_iTownITEM[ MAX_PRICE_TYPE ];		// ¾ÆÀÌÅÛº° º¸À¯·®				1000~100000
			};

			EconomyData Data;
			memcpy( &Data, m_pRecvPacket->m_gsv_SERVER_DATA.m_pDATA, sizeof( EconomyData ));


			sprintf( g_MsgBuf,"±âÁØÀÎ±¸¼ö:%d", Data.m_nTown_POP_BASE );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );

			sprintf( g_MsgBuf,"±âÁØ ¹ßÀüµµ:%d", Data.m_nTown_DEV_BASE );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );

			for( int i = 1; i < MAX_PRICE_TYPE; ++i )
			{
				sprintf( g_MsgBuf,"¾ÆÀÌÅÛº° ¼Òºñ·®(%d):%d", i,Data.m_nTown_CONSUM[i] );
				g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );
			}

			sprintf( g_MsgBuf,"¸¶À» ¹ßÀüµµ:%d", Data.m_nTownDEV );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );

			sprintf( g_MsgBuf,"¸¶À» ÀÎ±¸:%d", Data.m_iTownPOP );
			g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );


			for( int i = 1; i < MAX_PRICE_TYPE; ++i )
			{
				sprintf( g_MsgBuf,"¾ÆÀÌÅÛº° º¸À¯·®(%d):%d", i,Data.m_iTownITEM[i] );
				g_itMGR.AppendChatMsg(g_MsgBuf, g_dwYELLOW );
			}


			break;
		}
	case SERVER_DATA_NPC:
		{
#define	MAX_OBJ_VAR_CNT		20
			struct tagObjVAR 
			{
				union 
				{
					struct 
					{
						int			m_iNextCheckTIME;
						t_HASHKEY	m_HashNextTrigger;
						int			m_iPassTIME;
						union {
							short	m_nEventSTATUS;
							short	m_nAI_VAR[ MAX_OBJ_VAR_CNT ];
						} ;
					} ;
					BYTE m_pVAR[ 1 ];
				};		
			};

			tagObjVAR* pObjVAR = (tagObjVAR*)m_pRecvPacket->m_gsv_SERVER_DATA.m_pDATA;

			g_itMGR.AppendChatMsg( CStr::Printf("NextCheckTIME: %d", pObjVAR->m_iNextCheckTIME ),g_dwYELLOW );
			g_itMGR.AppendChatMsg( CStr::Printf("HashNextTrigger: %d", pObjVAR->m_HashNextTrigger ), g_dwYELLOW );
			g_itMGR.AppendChatMsg( CStr::Printf("PassTime: %d", pObjVAR->m_iPassTIME ), g_dwYELLOW );
			g_itMGR.AppendChatMsg( CStr::Printf("EventStatus: %d", pObjVAR->m_nEventSTATUS), g_dwYELLOW );
			for( int i = 0 ; i < MAX_OBJ_VAR_CNT; ++i )
				g_itMGR.AppendChatMsg( CStr::Printf("AI_VAR(%d):%d", i+1, pObjVAR->m_nAI_VAR[ i ] ), g_dwYELLOW );

			break;
		}
	default:

		break;
	}
}

//-------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_ASSEMBLE_RIDE_ITEM()
{
	/*struct gsv_ASSEMBLE_RIDE_ITEM : public t_PACKETHEADER 
	{
	WORD	m_wObjectIDX;
	short	m_nRidingPartIDX;
	short	m_nItemNO;
	} ;*/

	CObjAVT *pAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_wObjectIDX, false );
	if ( pAVT ) 
	{
		pAVT->SetPetParts( m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nRidingPartIDX,
			m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_RideITEM.m_nItemNo );
		//m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nItemNO );

		pAVT->UpdatePet();

		/// ¼Óµµ°¡ º¯Çß´Ù¸é ¼Óµµ ¼¼ÆÃ
		if ( m_pRecvPacket->m_HEADER.m_nSize == ( sizeof( gsv_ASSEMBLE_RIDE_ITEM ) + sizeof(short) ) )
		{
			pAVT->SetOri_RunSPEED( m_pRecvPacket->m_gsv_ASSEMBLE_RIDE_ITEM.m_nRunSPEED[0] );
		}

	}else
	{
		;
		// ERROR
	}
}


void CRecvPACKET::Recv_GSV_SET_EVENT_STATUS()
{
	CObjCHAR *pCHAR = g_pObjMGR->Get_CharOBJ( g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_SET_EVENT_STATUS.m_wObjectIDX ), false );
	if ( pCHAR ) 
	{
		switch( pCHAR->Get_TYPE() )
		{
		case OBJ_NPC:
			{
				((CObjNPC*)pCHAR)->SetEventValue( m_pRecvPacket->m_gsv_SET_EVENT_STATUS.m_nEventSTATUS );
			}
			break;

		case OBJ_EVENTOBJECT:
			{
				CObjFixedEvent* pObj = (CObjFixedEvent*)pCHAR;					
				pObj->ExecEventScript( m_pRecvPacket->m_gsv_SET_EVENT_STATUS.m_nEventSTATUS );
			}
			break;			
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_GSV_SET_ITEM_LIFE()
{	
	if( !g_pAVATAR )
		return;

	CItemSlot* pItemSlot =  g_pAVATAR->GetItemSlot();
	CItem* pItem = pItemSlot->GetItem( m_pRecvPacket->m_gsv_SET_ITEM_LIFE.m_nInventoryIDX );
	if( pItem )
	{
		pItem->SetLife( m_pRecvPacket->m_gsv_SET_ITEM_LIFE.m_nLife );


		
		if( pItem->GetItem().GetLife() <= 0 &&
			(ITEM_TYPE( pItem->GetItem().GetTYPE(), pItem->GetItem().GetItemNO() ) == TUNING_PART_ENGINE_CART ||
			ITEM_TYPE( pItem->GetItem().GetTYPE(), pItem->GetItem().GetItemNO() ) == TUNING_PART_ENGINE_CASTLEGEAR) )
		{						
			g_itMGR.AppendChatMsg( STR_CANT_REFAIR_PAT_ENGINE, IT_MGR::CHAT_TYPE_SYSTEM );
		}


		g_pAVATAR->UpdateAbility();
	}
}


void CRecvPACKET::Recv_lsv_CHANNEL_LIST_REPLY ()
{

	DWORD worldserver_id = m_pRecvPacket->m_lsv_CHANNEL_LIST_REPLY.m_dwServerID;
	CServerList::GetInstance().ClearChannelServerList( worldserver_id );

	short nOffset = sizeof( lsv_CHANNEL_LIST_REPLY );

	if( m_pRecvPacket->m_lsv_CHANNEL_LIST_REPLY.m_btChannelCNT > 0 )
	{
		tagCHANNEL_SRV* pChannel = NULL;
		char* pszChannelName = NULL;
		int iFirstChannelNo = 0;
		for( int i = 0; i < m_pRecvPacket->m_lsv_CHANNEL_LIST_REPLY.m_btChannelCNT; ++i )
		{
			pChannel = (tagCHANNEL_SRV*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tagCHANNEL_SRV) );
			pszChannelName = Packet_GetStringPtr( m_pRecvPacket, nOffset);
			if( !pszChannelName )
				pszChannelName = CStr::Printf("Channel-%d", pChannel->m_btChannelNO );

			CServerList::GetInstance().AddChannelServerList( worldserver_id, pChannel->m_btChannelNO, pszChannelName, pChannel->m_nUserPERCENT );

			if( i == 0 )
				iFirstChannelNo = pChannel->m_btChannelNO;

		}
		g_EUILobby.HideMsgBox();
	}
}


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "../GameData/CChatRoom.h"
void CRecvPACKET::Recv_wsv_CHATROOM ()
{
	switch( m_pRecvPacket->m_tag_CHAT_HEADER.m_btCMD ) {
		case CHAT_REPLY_ROOM_MAKED		:
			{
				short nOffset = sizeof( tag_CHAT_HEADER );
				WORD* pwMyID = (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(WORD) );
				//char* szRoomName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

				CChatRoom::GetInstance().Made( *pwMyID );
				CChatRoom::GetInstance().SetMaster( *pwMyID );

				break;
			}
		case CHAT_REPLY_ROOM_JOINED		:
			{
				short nOffset = sizeof( tag_CHAT_HEADER );
				WORD* pwMyID = (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(WORD) );
				char* szRoomName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

				CChatRoom::GetInstance().SetTitle( szRoomName );
				CChatRoom::GetInstance().Join();			

				WORD* pwUserID;
				char* szUserName;
				bool bSetMaster = false;

				for( ; nOffset < m_pRecvPacket->m_HEADER.m_nSize ; )
				{
					pwUserID = (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( WORD ));
					szUserName = Packet_GetStringPtr(m_pRecvPacket, nOffset);
					CChatRoom::GetInstance().JoinMember( *pwUserID, szUserName );
					if( !bSetMaster )
					{
						CChatRoom::GetInstance().SetMaster( *pwUserID );
						bSetMaster = true;
					}
				}

				if( g_pAVATAR )
				{
					CChatRoom::GetInstance().JoinMember( 
						*pwMyID,
						g_pAVATAR->Get_NAME() );
				}

				break;
			}

		case CHAT_REPLY_MAKE_FAILED		:	// ¹æ¸¸µé±â ½ÇÆÐ...
			g_itMGR.OpenMsgBox(STR_CHAT_REPLY_MAKE_FAILED);
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			break;
		case CHAT_REPLY_INVALIED_PASS	:	// Âü°¡ ¿äÃ»ÇÑ ¹æÀÇ ºñ¹øÀÌ Æ²¸².
			g_itMGR.OpenMsgBox( STR_CHAT_REPLY_INVALIED_PASS );
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			break;
		case CHAT_REPLY_ROOM_NOT_FOUND	:	// ¿äÃ»ÇÑ ¹æÀÌ »Ç°³Á³À½.
			g_itMGR.OpenMsgBox(STR_CHAT_REPLY_ROOM_NOT_FOUND);
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			///Ã¤ÆÃ¸®½ºÆ®¿¡¼­ »èÁ¦ÇÒ°Í
			break;
		case CHAT_REPLY_FULL_USERS		:	// ¿äÃ»ÇÑ ¹æÀÌ ¸¸¶¥ÀÓ..
			g_itMGR.OpenMsgBox(STR_CHAT_REPLY_FULL_USERS);
			CChatRoom::GetInstance().SetState( CChatRoom::STATE_DEACTIVATED );
			break;

		case CHAT_REPLY_KICKED			:	
			{
				if( g_pAVATAR )
				{
					// CHAT_REPLY_USER_LEFT¿Í °°Àº ±¸Á¶... ÂÏ°Ü³­ »ç¶÷ÇÑÅ×¸¸ ¿Â´Ù.. ´Ù¸¥ »ç¶÷ÇÑµ¥´Â CHAT_REPLY_USER_LEFT·Î Àü¼ÛµÊ 
					if( g_pObjMGR->Get_ServerObjectIndex(g_pAVATAR->Get_INDEX()) 
						== m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID )
					{
						CChatRoom::GetInstance().Leave();
						g_itMGR.AppendChatMsg(STR_CHAT_REPLY_KICKED,IT_MGR::CHAT_TYPE_SYSTEM );
					}
				}
				break;
			}
		case CHAT_REPLY_USER_LEFT		:
			{
				CChatRoom::GetInstance().LeaveMember( m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID );
				break;
			}
		case CHAT_REPLY_USER_LEFT_NAME:
			{
				short nOffset = sizeof( tag_CHAT_HEADER );
				char* pszName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				CChatRoom::GetInstance().LeaveMember( pszName );
				break;
			}
		case CHAT_REPLY_USER_ADD		:	
			{
				char* pszUserName;
				short nOffset = sizeof( wsv_CHAT_ROOM_USER );			
				pszUserName = Packet_GetStringPtr(m_pRecvPacket, nOffset);

				CChatRoom::GetInstance().JoinMember( 
					m_pRecvPacket->m_wsv_CHAT_ROOM_USER.m_wUserID,
					pszUserName );
				break;
			}
		case CHAT_REPLY_ROOM_LIST_END:
		case CHAT_REPLY_ROOM_LIST:
			{
				tag_CHAT_ROOM *pRoom;
				short nOffset = sizeof( wsv_CHAT_ROOM_LIST );
				char *szRoomName;

				CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
				assert( pDlg );
				if( pDlg == NULL ) break;

				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				for (short nI=0; nI<m_pRecvPacket->m_wsv_CHAT_ROOM_LIST.m_cRoomCNT; nI++) 
				{
					pRoom = (tag_CHAT_ROOM*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_CHAT_ROOM) );
					szRoomName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

					pCommDlg->AddChatRoom( szRoomName, pRoom->m_btRoomTYPE, pRoom->m_nRoomIDX, pRoom->m_cUserCNT );
				}
				break;
			}
	}
}
#include "../interface/dlgs/subclass/CFriendListItem.h"
void CRecvPACKET::Recv_tag_MCMD_HEADER ()
{
	switch( m_pRecvPacket->m_tag_MCMD_HEADER.m_btCMD ) 
	{
	case MSGR_CMD_APPEND_REQ	:
		{
			if( g_ClientStorage.IsApproveAddFriend() )
			{
				CTCmdAcceptAddFriend* pAcceptCmd = new CTCmdAcceptAddFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX,
					0,
					m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName);

				CTCmdRejectAddFriend* pRejectCmd = new CTCmdRejectAddFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX ,
					m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName );

				g_itMGR.OpenMsgBox( CStr::Printf( F_STR_QUERY_APPEND_FRIEND_REQ, m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName ),
					CMsgBox::BT_OK | CMsgBox::BT_CANCEL, 
					false,
					0,
					pAcceptCmd,
					pRejectCmd );
			}
			else
			{
				CTCmdRejectAddFriend Command( m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_wUserIDX ,	m_pRecvPacket->m_wsv_MCMD_APPEND_REQ.m_szName );
				Command.Exec( NULL );
			}
			break;
		}
	case MSGR_CMD_APPEND_ACCEPT	:
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				//±âÁ¸¿¡ 
				if( CFriendListItem* pItem = pCommDlg->FindFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_dwUserTAG ) )
				{
					pItem->SetStatus( m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btStatus );
				}
				else
				{
					pCommDlg->AddFriend( m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_dwUserTAG,
						m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btStatus,
						m_pRecvPacket->m_wsv_MCMD_APPEND_ACCEPT.m_szName );
				}
			}
			break;
		}
	case MSGR_CMD_APPEND_REJECT	:
		{
			// Ä£±¸ Ãß°¡ ¿äÃ»ÇÑ »ó´ë(szName)°¡ °ÅÀýÇß´Ù.
			short nOffset = sizeof( tag_MCMD_HEADER );
			char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			g_itMGR.OpenMsgBox(CStr::Printf(F_STR_REJECT_APPEND_FRIEND_REQ, szName) );
			break;
		}
	case MSGR_CMD_NOT_FOUND		:
		{
			// Ä£±¸ Ãß°¡ ¿äÃ»ÇÑ »ó´ë(szName)°¡ ¾ø´Ù.
			short nOffset = sizeof( tag_MCMD_HEADER );
			char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );
			g_itMGR.OpenMsgBox(CStr::Printf(F_STR_NOT_FOUNDORCONNECT_FRIEND, szName) );
			break;
		}
	case MSGR_CMD_CHANGE_STATUS	:
		{
			// »ç¿ëÀÚ m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAGÀÇ »óÅÂ°¡
			// m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus·Î ¹Ù²ã¾ú´Ù.
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				pCommDlg->ChangeFriendStatus( m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG,
					m_pRecvPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus );
			}		
			break;
		}
	case MSGR_CMD_LIST:
		{
			// Ä£±¸ ¸®½ºÆ®...
#pragma pack (push, 1)
			struct tagFriend {
				DWORD	m_dwUserTAG;
				BYTE	m_btStatus;
			} *pFR;
#pragma pack (pop)
			char *szName;
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;
				pCommDlg->ClearFriendList();

				short nOffset = sizeof( wsv_MCMD_LIST );
				for (short nI=0; nI<m_pRecvPacket->m_wsv_MCMD_LIST.m_btFriendCNT; nI++) 
				{
					pFR = (tagFriend*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tagFriend ) );
					szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

					pCommDlg->AddFriend( pFR->m_dwUserTAG, pFR->m_btStatus, szName );
				}
			}
			break;
		}
	default:
		break;
		/*
		case MSGR_CMD_DELETE		:	
		{
		break;
		}
		case MSGR_CMD_REFUSE		:	
		{
		break;
		}
		*/
	}
}

//----------------------------------------------------------------------------------------------------
#include "../interface/dlgs/subclass/CFriendListItem.h"
#include "../interface/dlgs/CPrivateChatDlg.h"
void CRecvPACKET::Recv_wsv_MESSENGER_CHAT ()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
	assert( pDlg );
	if( pDlg == NULL ) return;

	CCommDlg* pCommDlg = (CCommDlg*)pDlg;
	CFriendListItem* pItem = pCommDlg->FindFriend( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG );
	if( pItem )
	{
		if(CCountry::GetSingleton().IsJapan())
		{
			if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
			{					
				g_pNet->Send_cli_WHISPER(  (char*)pItem->GetName(), STR_JP_BILL_CANT_SEND_MSG );
				return;
			}        
		}			

		if( g_ClientStorage.IsApproveMessanger() )
		{
			CPrivateChatDlg* pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG );
			if( pPrivateChatDlg  == NULL )
				g_itMGR.OpenPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG, pItem->GetStatus(), pItem->GetName() );

			if( pPrivateChatDlg = g_itMGR.GetPrivateChatDlg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ) )
			{
				pPrivateChatDlg->RecvChatMsg( m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG ,
					m_pRecvPacket->m_wsv_MESSENGER_CHAT.m_szMSG );
			}
			else
			{
				assert( pPrivateChatDlg );
			}
		}
		else
		{			
			g_pNet->Send_cli_WHISPER(  (char*)pItem->GetName(), STR_OTHERSTATE_IS_REJCTMESSANGER );
		}
	}
}
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_wsv_MEMO()
{
	switch( m_pRecvPacket->m_wsv_MEMO.m_btTYPE )
	{
	case MEMO_REPLY_RECEIVED_CNT:
		{
			if( g_pAVATAR )
			{
				g_pAVATAR->SetServerSavedMemoCount( m_pRecvPacket->m_wsv_MEMO.m_nRecvCNT[0] );
				if( g_pAVATAR->GetServerSavedMemoCount() > 0 )
					g_pNet->Send_cli_MEMO_REQ();
			}
			break;
		}
	case MEMO_REPLY_CONTENTS:
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
			assert( pDlg );
			if( pDlg )
			{
				CCommDlg* pCommDlg = (CCommDlg*)pDlg;

				DWORD*	pdwRecvTime;
				char*	pszFromName;
				char*	pszMemo;

				for( short	nOffset = sizeof( wsv_MEMO ) ; nOffset < m_pRecvPacket->m_HEADER.m_nSize ; )
				{
					pdwRecvTime = (DWORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( DWORD ) );
					pszFromName	= Packet_GetStringPtr( m_pRecvPacket, nOffset );
					pszMemo		= Packet_GetStringPtr( m_pRecvPacket, nOffset );

					///ÆÄÀÏ¿¡ ÀúÀå
					//pCommDlg->AddMemo( pszFromName, pszMemo, *pdwRecvTime );
					CSqliteDB::GetInstance().AddMemo( g_pAVATAR->Get_NAME(), pszFromName, pszMemo, *pdwRecvTime );

					if( g_pAVATAR )
						g_pAVATAR->DecreaseServerSavedMemoCount();
				}

				if( g_pAVATAR )
				{
					if( g_pAVATAR->GetServerSavedMemoCount() > 0 )
						g_pNet->Send_cli_MEMO_REQ();
				}
			}
			break;
		}
	case MEMO_REPLY_SEND_OK:
		break;
	case MEMO_REPLY_SEND_INVALID_TARGET:
	case MEMO_REPLY_SEND_NOT_EXIST:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_NOT_EXISTORINVALID);
		break;
	case MEMO_REPLY_SEND_REFUSED:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_REFUSED);
		break;
	case MEMO_REPLY_SEND_FULL_MEMO:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_FULL_MEMO);
		break;
	case MEMO_REPLY_SEND_INVALID_CONTENT:
		g_itMGR.OpenMsgBox(STR_MEMO_REPLY_SEND_INVALID_CONTENT);
		break;
	default:
		break;
	}
}
#include "../interface/dlgs/CChatRoomDlg.h"
void CRecvPACKET::Recv_wsv_CHATROOM_MSG ()
{
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CHATROOM );
	assert( pDlg );
	if( pDlg )
	{
		CChatRoomDlg* pChatRoomDlg = ( CChatRoomDlg*) pDlg;
		pChatRoomDlg->RecvChatMsg( m_pRecvPacket->m_wsv_CHATROOM_MSG.m_wObjectID,
			m_pRecvPacket->m_wsv_CHATROOM_MSG.m_szMSG
			);
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_CRAFT_ITEM_REPLY()
{
	//struct gsv_CRAFT_ITEM_REPLY : public t_PACKETHEADER {
	//BYTE			m_btRESULT;
	//BYTE			m_btOutCNT;			// º¯°æµÈ ¾ÆÀÌÅÛ °¹¼ö
	//tag_SET_INVITEM	m_sInvITEM[ 0 ];	// º¯°æµÈ °¹¼ö ¸¸Å­ µé¾î ÀÖ´Ù... 
	//									// ¿¹¿Ü) CRAFT_UPGRADE_SUCCESS, CRAFT_UPGRADE_FAILED ÀÏ°æ¿ì
	//									// m_sInvITEM[ m_btOutCNT-1 ].m_uiQuantity¿¡ ¼º°øµµ °è»êµÈ°ªÀÌ µé¾îÀÖÀ½
	//} ;

	switch( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT )
	{
	case	CRAFT_GEMMING_SUCCESS:		//	0x01
		{


			g_pAVATAR->SetWaitUpdateInventory( true );
			for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
			{
				int iPartIdx = CInventory::GetBodyPartByEquipSlot( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX );

				if( g_pAVATAR )
				{
					g_pAVATAR->SetPartITEM( iPartIdx,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
					/// Equip ÆÐÅ¶ÀÌ ³¯¶ó¿À±â¶§¹®¿¡ ¼Ó¼º ¾÷µ¥ÀÌÆ®´Â ºÒÇÊ¿äÇØ º¸ÀÎ´Ù.
					/// ¾ÆÀÌÅÛ ½½·Ô¸¸ °»½Å½ÃÅ°¸é µÉµí..
					g_pAVATAR->Update();
				}
			}
			g_pAVATAR->SetWaitUpdateInventory( false );
			g_pAVATAR->UpdateInventory();


			g_itMGR.AppendChatMsg( STR_GEMMING_SUCCESS, IT_MGR::CHAT_TYPE_SYSTEM );					
		}
		break;
	case	CRAFT_GEMMING_NEED_SOCKET:	//	0x02	// ¼ÒÄÏ¾ø´Ù.
		{
			g_itMGR.AppendChatMsg( STR_GEMMING_NEED_SOCKET, IT_MGR::CHAT_TYPE_SYSTEM );					
		}
		break;
	case	CRAFT_GEMMING_USED_SOCKET:	//	0x03	// Àç¹ÖµÈ °æ¿ìµî....
		break;

	case	CRAFT_BREAKUP_SUCCESS_GEM:	//	0x04	// º¸¼® ºÐ¸® ¼º°ø
		{
			if( g_pAVATAR )
			{
				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				g_itMGR.OpenMsgBox(STR_CRAFT_BREAKUP_SUCCESS_GEM);

				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}			

				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
			}

			break;
		}
	case	CRAFT_BREAKUP_DEGRADE_GEM:	//	0x05	// º¸¼® ºÐ¸® ¼º°ø, º¸¼®µî±Þ °­µî
		{
			if( g_pAVATAR )
			{
				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}

				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
				g_itMGR.OpenMsgBox(STR_CRAFT_BREAKUP_DEGRADE_GEM);
			}
			break;
		}
	case	CRAFT_BREAKUP_CLEARED_GEM:	//	0x06	// º¸¼® ºÐ¸® ¼º°ø, º¸¼® »èÁ¦µÊ
		{
			if( g_pAVATAR )
			{
				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();


				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}

				g_itMGR.OpenMsgBox(STR_CRAFT_BREAKUP_CLEARED_GEM);
				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
			}
			break;
		}

	case	CRAFE_BREAKUP_SUCCESS:		//	0x07	// ¾ÆÀÌÅÛ ºÐÇØ ¼º°ø
		{
			if( g_pAVATAR )
			{
				CSeparate& Separate = CSeparate::GetInstance();
				switch( Separate.GetType() )
				{
				case CSeparate::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Separate.GetRequireMp() );
					break;
				case CSeparate::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Separate.GetRequireMoney() );
					break;
				default:
					break;
				}

				tagITEM		ItemData;
				tagITEM*	pItem = NULL;
				//CIcon*		pIcon = NULL;
				CreateMsgBoxData MsgBoxData;
				MsgBoxData.strMsg = STR_CRAFE_BREAKUP_SUCCESS;
				CIconItem* pItemIcon = NULL;

				g_pAVATAR->SetWaitUpdateInventory( true );
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT; i++ )
				{
					pItem = &m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM;
					if( !pItem->IsEmpty() )
					{
						if( !pItem->IsEnableDupCNT() )
						{
							pItemIcon = new CIconItem(pItem);		
							MsgBoxData.m_Icons.push_back( pItemIcon );
						}
						else
						{
							ItemData = *pItem;
							if( ItemData.m_uiQuantity > g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX ].m_uiQuantity )
							{
								ItemData.m_uiQuantity -= g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX ].m_uiQuantity;
								pItemIcon = new CIconItem(&ItemData);
								MsgBoxData.m_Icons.push_back( pItemIcon );
							}
						}
					}

					g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_btInvIDX,
						m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ].m_ITEM );
				}
				g_pAVATAR->SetWaitUpdateInventory( false );
				g_pAVATAR->UpdateInventory();

				g_itMGR.OpenMsgBox2(MsgBoxData);
				SE_SuccessSeparate( g_pAVATAR->Get_INDEX() );
			}
			break;
		}
	case	CRAFT_UPGRADE_SUCCESS:		//	0x10	// Àç·Ã ¼º°ø
		{
			if( g_pAVATAR )
			{
				CUpgrade& Upgrade = CUpgrade::GetInstance();
				Upgrade.ClearResultItemSet();

				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1; i++ )
					Upgrade.AddResultItemSet( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ] );

				Upgrade.SetResultSuccessProb( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1].m_ITEM.m_uiQuantity );
				Upgrade.SetResult( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT  );

				switch( Upgrade.GetType() )
				{
				case CUpgrade::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Upgrade.GetRequireMp() );
					break;
				case CUpgrade::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Upgrade.GetRequireMoney() );
					break;
				default:
					break;
				}
			}
			break;
		}
	case	CRAFT_UPGRADE_FAILED:		//	0x11	// Àç·Ã ½ÇÆÐ
		{
			if( g_pAVATAR )
			{
				CUpgrade& Upgrade = CUpgrade::GetInstance();
				Upgrade.ClearResultItemSet();
				for( int i = 0; i < m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1; i++ )
					Upgrade.AddResultItemSet( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ i ] );

				Upgrade.SetResultSuccessProb( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_sInvITEM[ m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btOutCNT - 1].m_ITEM.m_uiQuantity );
				Upgrade.SetResult( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT  );

				switch( Upgrade.GetType() )
				{
				case CUpgrade::TYPE_SKILL:
					g_pAVATAR->Sub_MP( Upgrade.GetRequireMp() );
					break;
				case CUpgrade::TYPE_NPC:
					g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - Upgrade.GetRequireMoney() );
					break;
				default:
					break;
				}
			}
		}
		break;
	case	CRAFT_UPGRADE_INVALID_MAT:	//	0x12	// Àç·á ¾ÆÀÌÅÛÀÌ Àß¸øµÆ´Ù.
		{
			CUpgrade::GetInstance().SetResult( m_pRecvPacket->m_gsv_CRAFT_ITEM_REPLY.m_btRESULT  );
			break;
		}
	default:
		break;
	}
}

void CRecvPACKET::Recv_gsv_USED_ITEM_TO_REPAIR()
{
	Recv_gsv_SET_INV_ONLY();

	if( g_pAVATAR )
		g_pAVATAR->UpdateAbility();
}
void CRecvPACKET::Recv_gsv_REPAIRED_FROM_NPC()
{
	Recv_gsv_SET_MONEYnINV();

	if( g_pAVATAR )
		g_pAVATAR->UpdateAbility();
}

void CRecvPACKET::Recv_gsv_SET_MONEY_ONLY()
{
	__int64 i64Diff = m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money - g_pAVATAR->Get_MONEY();

	if( i64Diff > 0 )
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_SET_MONEY_INC, i64Diff) ,IT_MGR::CHAT_TYPE_SYSTEM );
	else
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_SET_MONEY_DEC, -i64Diff),IT_MGR::CHAT_TYPE_SYSTEM );

	if( g_pAVATAR )
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money );
}

void CRecvPACKET::Recv_gsv_REWARD_MONEY()
{
	__int64 i64Diff = m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money - g_pAVATAR->Get_MONEY();

	if( i64Diff > 0 )
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_REWARD_MONEY_INC, i64Diff) , IT_MGR::CHAT_TYPE_QUESTREWARD );
	else
		g_itMGR.AppendChatMsg( CStr::Printf(F_STR_REWARD_MONEY_DEC, -i64Diff), IT_MGR::CHAT_TYPE_QUESTREWARD );

	if( g_pAVATAR )
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_SET_MONEY_ONLY.m_i64Money );
}


//----------------------------------------------------------------------------------------------------	
///
/// @brief Äù½ºÆ® º¸»ó°ü·Ã
///
//----------------------------------------------------------------------------------------------------
void CRecvPACKET::Recv_gsv_REWARD_ITEM()
{
	if( !g_pAVATAR )
		return;

	for ( int i = 0; i < m_pRecvPacket->m_gsv_SET_INV_ONLY.m_btItemCNT ; i++ ) 
	{
		char *szMsg = NULL;

		tagITEM *pInvITEM = NULL;
		tagITEM *pSrvITEM = NULL;

		pSrvITEM = &m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[i].m_ITEM;
		if( !pSrvITEM->IsEmpty() )
		{
			if ( pSrvITEM->IsEnableDupCNT() ) 
			{
				pInvITEM = &g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_SET_INV_ONLY.m_sInvITEM[i].m_btInvIDX ];
				szMsg = CStr::Printf(F_STR_REWARD_ITEMS, ITEM_NAME( pSrvITEM->GetTYPE(), pSrvITEM->GetItemNO() ), pSrvITEM->GetQuantity() - pInvITEM->GetQuantity() );
			} 
			else 
			{
				szMsg = CStr::Printf(F_STR_REWARD_ITEM, ITEM_NAME( pSrvITEM->GetTYPE(), pSrvITEM->GetItemNO() ) );
			}

			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_QUESTREWARD );
		}
	}
}

void CRecvPACKET::Recv_gsv_REWARD_ADD_ABILITY()
{
	g_QuestRewardQueue.PushAddAbility( m_pRecvPacket->m_gsv_SET_ABILITY );
}

void CRecvPACKET::Recv_gsv_REWARD_SET_ABILITY()
{
	g_QuestRewardQueue.PushSetAbility( m_pRecvPacket->m_gsv_SET_ABILITY );
}

void CRecvPACKET::Recv_gsv_PARTY_ITEM()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_ITEM_1%d\n",g_GameDATA.GetGameTime() );
	CParty& refParty = CParty::GetInstance();
	assert( refParty.HasParty() );
	if( refParty.HasParty() )
	{
		PartyMember Member;
		if( refParty.GetMemberInfoByObjSvrIdx( m_pRecvPacket->m_gsv_PARTY_ITEM.m_wObjectIDX , Member ) )
		{
			//20050727 È«±Ù ÆÄÆ¼½Ã¿¡ ¾ÆÀÌÅÆ ½Àµæ ¸Þ¼¼Áö.
			if( CCountry::GetSingleton().IsJapan() )
			{				
				g_itMGR.AppendChatMsg( 				
					CStr::Printf( m_pRecvPacket->m_gsv_PARTY_ITEM.m_ITEM.GettingMESSAGE_Party( Member.m_strName.c_str() ) ),
					IT_MGR::CHAT_TYPE_SYSTEM );
			}
			else
			{
				g_itMGR.AppendChatMsg( 
					CStr::Printf("%s´ÔÀÌ %s", Member.m_strName.c_str(), m_pRecvPacket->m_gsv_PARTY_ITEM.m_ITEM.GettingMESSAGE() ),
					IT_MGR::CHAT_TYPE_SYSTEM );
			}
		}		
	}
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_ITEM_2%d\n",g_GameDATA.GetGameTime() );
}

void CRecvPACKET::Recv_gsv_PARTY_RULE()
{
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_RULE_1(%d)\n",g_GameDATA.GetGameTime() );
	CParty::GetInstance().RecvPartyRule( m_pRecvPacket->m_gsv_PARTY_RULE.m_btPartyRULE );
	_RPT1( _CRT_WARN,"Recv_gsv_PARTY_RULE_2(%d)\n",g_GameDATA.GetGameTime() );
}

void CRecvPACKET::Recv_wsv_CHAR_CHANGE()
{
	CGame::GetInstance().ChangeState( CGame::GS_EXITMAIN );
}

void CRecvPACKET::Recv_gsv_LOGOUT_REPLY()
{
	g_itMGR.SetWaitDisconnectTime( m_pRecvPacket->m_gsv_LOGOUT_REPLY.m_wWaitSec );
	g_itMGR.ChangeState( IT_MGR::STATE_WAITDISCONNECT );
}

void CRecvPACKET::Recv_wsv_DELETE_CHAR ()
{
	short nOffset = (short)sizeof( wsv_DELETE_CHAR);
	char *szName = Packet_GetStringPtr( m_pRecvPacket, nOffset );

	CJustModelAVT* pAVT = CGameDataCreateAvatar::GetInstance().FindAvatar( szName );
	assert( pAVT );
	if( pAVT )
	{
		if( m_pRecvPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME == 0xFFFFFFFF )
		{
			g_EUILobby.ShowMsgBox( STR_CLAN_CANT_DELETE_CLANMASTER, CTMsgBox::BT_OK ,true , EUI_SELECT_AVATA );
		}
		else
		{
			pAVT->SetDeleteRemainSEC( m_pRecvPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME );
			if( m_pRecvPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME )
			{
				pAVT->SwitchState( CJustModelAVT::STATE_SITDOWN );
			}
			else
			{
				pAVT->SwitchState( CJustModelAVT::STATE_STANDUP );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------	
///
/// @brief ÀÌº¥Æ® ¿ÀºêÁ§Æ® Ã³¸® °ü·Ã
///
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_ADD_EVENTOBJ()
{
	/*struct gsv_ADD_EVENTOBJ : public t_PACKETHEADER {
	WORD	m_wObjectIDX;
	char	m_cMapX;
	char	m_cMapY;
	WORD	m_wMapEventID;
	short	m_nEventSTATUS;
	} ;*/

	CObjFixedEvent* pObj = g_pObjMGR->GetEventObject( m_pRecvPacket->m_gsv_ADD_EVENTOBJ.m_wMapEventID );
	if( pObj )
	{		
		g_pObjMGR->Set_ServerObjectIndex( pObj->Get_INDEX(), m_pRecvPacket->m_gsv_ADD_EVENTOBJ.m_wObjectIDX );

		pObj->ExecEventScript( m_pRecvPacket->m_gsv_ADD_EVENTOBJ.m_nEventSTATUS, true, false, true );
	}else
	{
		assert( 0 && "Recv invalid event object" );
	}


}

void CRecvPACKET::Recv_gsv_APPRAISAL_REPLY()
{
	if( m_pRecvPacket->m_gsv_APPRAISAL_REPLY.m_btResult == RESULT_APPRAISAL_REPLY_OK )
	{
		if( g_pAVATAR )
		{
			g_pAVATAR->m_Inventory.m_ItemLIST[ m_pRecvPacket->m_gsv_APPRAISAL_REPLY.m_wInventoryIndex ].m_bIsAppraisal = 1;
			g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - CGame::GetInstance().GetAppraisalCost() );


		}
	}
	else
	{
		g_itMGR.OpenMsgBox(STR_FAIL_IDENTIFY );
	}
	CGame::GetInstance().EndAppraisal();
}


//----------------------------------------------------------------------------------------------------	
///
/// @brief HP, MP ¹°¾à Á¾·á ÆÐÅ¶.
///			ÀÌÆÐÅ¶ÀÇ ³»¿ëÀ¸·Î ÇöÀç »óÅÂ¸¦ °»½ÅÇÏ°í, ¶ÇÇÑ ÀÌÀü¿¡ ´©ÀûµÇ¾îÀÖ´ø µ¥¹ÌÁöµµ ´Ù ¼ÒÁøÇØ¾ßµÈ´Ù.
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_SET_HPnMP()
{
	short iTarget = m_pRecvPacket->m_gsv_SET_HPnMP.m_wObjectIDX;
	CObjCHAR *pChar = g_pObjMGR->Get_ClientCharOBJ( iTarget, true );

	if( pChar && g_pAVATAR )
	{
		short client_obj_index = g_pObjMGR->Get_ClientObjectIndex ( m_pRecvPacket->m_gsv_SET_HPnMP.m_wObjectIDX );

		if( m_pRecvPacket->m_gsv_SET_HPnMP.m_nHP >= 0 )
		{
			/// ¸ðµç µ¥¹ÌÁö Å¬¸®¾î..
			pChar->ClearAllDamage();

			///ÀÚ±â ÀÚ½ÅÀÏ°æ¿ì¿¡¸¸ Revise HP(¼­¼­È÷ º¸Á¤)¸¦ Àû¿ëÇÑ´Ù.
			if( client_obj_index == g_pAVATAR->Get_INDEX() )
				pChar->SetReviseHP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nHP - pChar->Get_HP() );
			else
				pChar->Set_HP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nHP );
		}

		if( m_pRecvPacket->m_gsv_SET_HPnMP.m_nMP >= 0 )
		{
			///ÀÚ±â ÀÚ½ÅÀÏ°æ¿ì¿¡¸¸ Revise MP(¼­¼­È÷ º¸Á¤)¸¦ Àû¿ëÇÑ´Ù.
			if( client_obj_index == g_pAVATAR->Get_INDEX() )
				pChar->SetReviseMP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nMP - pChar->Get_MP() );
			else
				pChar->Set_MP( m_pRecvPacket->m_gsv_SET_HPnMP.m_nMP );
		}
	}	
}

//----------------------------------------------------------------------------------------------------	
///
/// @brief Do_DeadEvent ¿¡¼­ È£ÃâÇÏ´ø°ÍÀ» ¼­¹ö¿¡¼­ ¿äÃ»½Ã Ã³¸®ÇÏ°Ô º¯°æ..
///
//----------------------------------------------------------------------------------------------------	
void CRecvPACKET::Recv_gsv_CHECK_NPC_EVENT()
{	
	LOGOUT( " ¼­¹ö·ÎºÎÅÍ Æ®¸®¼­ ½ÇÇà ¸í·É ¹ÞÀ½[ %s ] ", NPC_DESC( m_pRecvPacket->m_gsv_CHECK_NPC_EVENT.m_nNpcIDX ) );
	QF_doQuestTrigger( NPC_DESC( m_pRecvPacket->m_gsv_CHECK_NPC_EVENT.m_nNpcIDX ) );
}

void CRecvPACKET::Recv_wsv_CLAN_COMMAND()
{
	switch( m_pRecvPacket->m_wsv_CLAN_COMMAND.m_btRESULT )
	{
	case RESULT_CLAN_MEMBER_JOBnLEV:
		{
			short nOffset = sizeof( wsv_CLAN_MEMBER_JOBnLEV );
			if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CClan::GetInstance().SetMemberJob( pszName, m_pRecvPacket->m_wsv_CLAN_MEMBER_JOBnLEV.m_nJOB );
				CClan::GetInstance().SetMemberLevel( pszName, m_pRecvPacket->m_wsv_CLAN_MEMBER_JOBnLEV.m_nLEVEL );
			}
		}
		break;
	case RESULT_CLAN_CREATE_NO_CONDITION:
		g_itMGR.OpenMsgBox( STR_CLAN_RESULT_CLAN_CREATE_NO_CONDITION );
		break;
	case RESULT_CLAN_CREATE_OK:/// Ã¢¼³ÇÑ À¯Àú¿¡°Ô ¿Â´Ù.
		{
			/// 100¸¸À» »«´Ù.
			g_pAVATAR->Set_MONEY( g_pAVATAR->Get_MONEY() - 1000000 );
			g_itMGR.CloseDialog( DLG_TYPE_CLAN_ORGANIZE );
			g_itMGR.OpenMsgBox(STR_CLAN_ORGANIZED );
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN ) ) )
			{
				if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					CClan& Clan = CClan::GetInstance();

					Clan.SetName( pszName );
					Clan.SetClanNo( pInfo->m_dwClanID );
					Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
					Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
					Clan.SetPoint( pInfo->m_iClanSCORE );
					Clan.SetClass( pInfo->m_btClanPOS );
					Clan.SetMoney( pInfo->m_biClanMONEY );
					Clan.SetStoreRate( pInfo->m_iClanRATE );
					Clan.SetLevel( pInfo->m_btClanLEVEL );
					Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );

					if( char* pszSlogan = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetSlogan( pszSlogan );

					Clan.AddMember( g_pAVATAR->Get_NAME(), CClan::CLAN_MASTER , pInfo->m_iClanCONT, CGame::GetInstance().GetJoinChannelNo() , g_pAVATAR->Get_LEVEL(), g_pAVATAR->Get_JOB() );

					g_pAVATAR->SetClan( pInfo->m_dwClanID, pInfo->m_wClanMARK[0], pInfo->m_wClanMARK[1], pszName , 1, CClan::CLAN_MASTER );

					Clan.SetMemberJob( g_pAVATAR->Get_NAME(), g_pAVATAR->Get_JOB() );
					Clan.SetMemberLevel( g_pAVATAR->Get_NAME(), g_pAVATAR->Get_LEVEL() );

					g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV(  g_pAVATAR->Get_LEVEL() , g_pAVATAR->Get_JOB());
				}
			}
		}
		break;
	case RESULT_CLAN_JOIN_OK:///Å¬·£¿¡ »õ·Î °¡ÀÔµÈ À¯Àú¿¡°Ô¸¸ ¿Â´Ù.	
		{
			g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_OK);
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN ) ) )
			{
				if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					CClan& Clan = CClan::GetInstance();

					Clan.SetName( pszName );
					Clan.SetClanNo( pInfo->m_dwClanID );
					Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
					Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
					Clan.SetPoint( pInfo->m_iClanSCORE );
					Clan.SetClass( pInfo->m_btClanPOS );
					Clan.SetMoney( pInfo->m_biClanMONEY );
					Clan.SetStoreRate( pInfo->m_iClanRATE );
					Clan.SetLevel( pInfo->m_btClanLEVEL );
					Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );

					if( char* pszSlogan = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetSlogan( pszSlogan );

					if( char* pszNotice = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetNotice( pszNotice );

					g_pAVATAR->SetClan( pInfo->m_dwClanID, pInfo->m_wClanMARK[0], pInfo->m_wClanMARK[1], pszName , pInfo->m_btClanLEVEL, CClan::CLAN_JUNIOR );

					Clan.RequestMemberList();
					//Clan.RequestClanInfo();

					g_pNet->Send_cli_CLAN_MEMBER_JOBnLEV(  g_pAVATAR->Get_LEVEL() , g_pAVATAR->Get_JOB());
				}
			}
		}
		break;
	case RESULT_CLAN_DESTROY_OK://¾È¾²ÀÌ´Â ÆÐÅ¶
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROY_OK);
		break;
	case RESULT_CLAN_CREATE_FAILED://		
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_CREATE_FAILED );
		break;
	case RESULT_CLAN_CREATE_DUP_NAME://		
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_CREATE_DUP_NAME);
		break;
	case RESULT_CLAN_CREATE_NO_RIGHT://			
		g_itMGR.CloseDialog( DLG_TYPE_CLAN_ORGANIZE );
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_CREATE_NO_RIGHT);
		break;
	case RESULT_CLAN_DESTROYED://			0x051		// Å¬·£ »Ç°³ Á³´Ù..
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROYED);
		CClan::GetInstance().Clear();
		if( g_pAVATAR )
			g_pAVATAR->ResetClan();
		break;
	case RESULT_CLAN_DESTROY_FAILED://		
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROY_FAILED);
		break;
	case RESULT_CLAN_DESTROY_NO_RIGHT://			// »Ç°¶ ±ÇÇÑ ¾ø´Ù.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_DESTROY_NO_RIGHT);
		break;
	case RESULT_CLAN_JOIN_MEMBER:/// »õ·Î °¡ÀÔµÈ Å¬·£¿øÀÌ ÀÖÀ»¶§ ¿Â´Ù.
		{
			/*strujct {
			char *m_szMember;	½Å±Ô °¡ÀÔÀÚ
			char *m_szMaster;	°¡ÀÔ½ÃÅ²ÀÚ
			*/
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMember = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pszMember && pszMaster );
			if( pszMember && pszMaster )
			{
				if( strcmpi( g_pAVATAR->Get_NAME(), pszMember ) )///³»°¡ ¾Æ´Ò °æ¿ì¿¡¸¸ º¸¿©ÁÖÀÚ.
				{
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_JOIN, pszMember, pszMaster));

					CObjCHAR* pChar = g_pObjMGR->Get_ClientCharOBJByName( pszMember );
					if( pChar && pChar->Is_AVATAR() )
					{
						CObjAVT* pAvt = (CObjAVT*)pChar;				
						pAvt->SetClan( g_pAVATAR->GetClanID(),
							g_pAVATAR->GetClanMarkBack(),
							g_pAVATAR->GetClanMarkCenter(),
							g_pAVATAR->GetClanName() ,
							g_pAVATAR->GetClanLevel() , CClan::CLAN_JUNIOR );
					}
					CClan::GetInstance().AddMember( pszMember, CClan::CLAN_JUNIOR , 0, 0 ,0, 0);
				}
			}
		}
		break;
	case RESULT_CLAN_JOIN_FAILED:			
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_FAILED);
		break;
	case RESULT_CLAN_JOIN_NO_RIGHT://				// °¡ÀÔ ½ÃÅ³ ±ÇÇÑÀÌ ¾ø´Ù.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_NO_RIGHT);
		break;
	case RESULT_CLAN_JOIN_HAS_CLAN://				// ´Ù¸¥ ±æµå¿¡ °¡ÀÔÇØ ÀÖ´Ù.
		g_itMGR.OpenMsgBox(STR_CLAN_RESULT_CLAN_JOIN_HAS_CLAN);
		break;
	case RESULT_CLAN_MY_DATA:///Ã³À½ ·Î±×ÀÎ½Ã Å¬·£Á¤º¸
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN ) ) )
			{
				if( char* pszName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					CClan& Clan = CClan::GetInstance();
					Clan.SetName( pszName );
					Clan.SetClanNo( pInfo->m_dwClanID );
					Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
					Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
					Clan.SetPoint( pInfo->m_iClanSCORE );
					Clan.SetClass( pInfo->m_btClanPOS );
					Clan.SetLevel( pInfo->m_btClanLEVEL );
					Clan.SetMoney( pInfo->m_biClanMONEY );
					Clan.SetStoreRate( pInfo->m_iClanRATE );
					Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );

					if( char* pszSlogan = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetSlogan( pszSlogan );

					if( char* pszNotice = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						Clan.SetNotice( pszNotice );

					if( g_pAVATAR )///È¤½Ã ¾Æ¹ÙÅ¸°¡ ¸¸µé¾îÁø ÈÄ¿¡ ¿Ã¼ö ÀÖ´Ù.
					{
						g_pAVATAR->SetClan( pInfo->m_dwClanID, pInfo->m_wClanMARK[0], pInfo->m_wClanMARK[1], pszName , pInfo->m_btClanLEVEL , pInfo->m_btClanPOS );
					}

				}
			}
		}
		break;
	case RESULT_CLAN_INFO://				0x031		// ±æµå Á¤º¸ /// 2004 / 11/ 23ÀÏ »èÁ¦
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			tag_MY_CLAN* pInfo = (tag_MY_CLAN*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tag_MY_CLAN) );

			assert( pInfo );
			if( pInfo )
			{
				CClan& Clan = CClan::GetInstance();

				if( Clan.GetLevel() < pInfo->m_btClanLEVEL )
					g_itMGR.OpenMsgBox( STR_CLAN_LEVELUP,CMsgBox::BT_OK, false );

				Clan.SetClanNo( pInfo->m_dwClanID );
				Clan.SetMarkBack( pInfo->m_wClanMARK[0] );
				Clan.SetMarkCenter( pInfo->m_wClanMARK[1] );
				Clan.SetPoint( pInfo->m_iClanSCORE );
				//Clan.SetClass( pInfo->m_btClanPOS );
				Clan.SetLevel( pInfo->m_btClanLEVEL );
				Clan.SetMoney( pInfo->m_biClanMONEY );
				Clan.SetStoreRate( pInfo->m_iClanRATE );
				Clan.SetSkill( pInfo->m_ClanBIN.m_SKILL, MAX_CLAN_SKILL_SLOT );
			}
		}
		break;
	case RESULT_CLAN_ROSTER://				0x032		// ±æµå ¸®½ºÆ®
		{
			CClan& Clan = CClan::GetInstance();
			Clan.ClearMemberlist();

			short nOffset = sizeof( wsv_CLAN_COMMAND );
			tag_CLAN_MEMBER* pMember = NULL;
			char*  pszCharName	= NULL;


			//CStringManager& StringMgr = CStringManager::GetSingleton();
			while( pMember = (tag_CLAN_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_MEMBER ) ) )
			{
				if( pszCharName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
					Clan.AddMember( pszCharName, pMember->m_btClanPOS, pMember->m_iClanCONTRIBUTE, pMember->m_btChannelNO , pMember->m_nLEVEL ,pMember->m_nJOB);

			}


		}
		break;
	case RESULT_CLAN_KICK://				0x081		// Å¬·£¿¡¼­ Ãß¹æµÊ
		{
			//char *m_szMember;	Å»ÅðÀÚ
			//char *m_szMaster;	Å»Åð½ÃÅ²ÀÚ

			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMember = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

			assert( pszMember && pszMaster );
			if( pszMember && pszMaster )
			{
				if( strcmpi( pszMember, g_pAVATAR->Get_NAME() ) )
				{
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_KICK, pszMember, pszMaster ) );
					CClan::GetInstance().RemoveMember( pszMember );
				}
				else
				{
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_KICK_ME, pszMaster ));
					CClan::GetInstance().Clear();
					if( g_pAVATAR )
						g_pAVATAR->ResetClan();
				}
			}
		}
		break;
	case RESULT_CLAN_QUIT://				0x082		// ¼Ò¼Ó Å¬·£¿¡¼­ »èÁ¦µÊ( Å¬·£º¯¼ö ÃÊ±âÈ­ )
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMember = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pszMember );
			if( pszMember )
			{
				if( strcmpi( pszMember, g_pAVATAR->Get_NAME() )== 0)			
				{
					CClan::GetInstance().Clear();
					if( g_pAVATAR )
						g_pAVATAR->ResetClan();
				}
				else
				{
					CClan::GetInstance().RemoveMember( pszMember );
					g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_RESULT_CLAN_QUIT,  pszMember ) );
				}
			}
		}
		break;
	case GCMD_INVITE_REQ://³ª¿¡°Ô Å¬·£ °¡ÀÔÀ» ¿äÃ»Çß´Ù. <¿äÃ»ÀÚÀÌ¸§>
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			assert( pszMaster );
			if( pszMaster )
			{

				if(CCountry::GetSingleton().IsJapan())
				{
					if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_BATTLE) )
					{
						g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_JOIN_CLAN );

						CTCmdRejectReqJoinClan pCmdNo( pszMaster );
						pCmdNo.Exec(NULL);

						return;
					}        
				}

				//MSGTYPE_RECV_CLANJOIN_REQ
				if( g_itMGR.FindMsgBox( CMsgBox::MSGTYPE_RECV_CLANJOIN_REQ ) == NULL )
				{
					CTCommand* pCmdYes  = new CTCmdAcceptReqJoinClan( pszMaster );
					CTCommand* pCmdNo	= new CTCmdRejectReqJoinClan( pszMaster );
					g_itMGR.OpenMsgBox(
						CStr::Printf( STR_CLAN_GCMD_INVITE_REQ, pszMaster ),
						CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0,
						pCmdYes, pCmdNo ,CMsgBox::MSGTYPE_RECV_CLANJOIN_REQ);
				}
			}
		}
		break;
		//#define	GCMD_INVITE_REPLY_YES	0x00c		//Å¬·£°¡ÀÔ ½Â³« <¿äÃ»ÀÚÀÌ¸§>
	case GCMD_INVITE_REPLY_NO://	0x00d		//Å¬·£°¡ÀÔ °ÅÀý <¿äÃ»ÀÚÀÌ¸§>
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszTargetName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

			g_itMGR.OpenMsgBox( CStr::Printf(STR_CLAN_GCMD_INVITE_REPLY_NO, pszTargetName ) );
		}
		break;
	case RESULT_CLAN_MOTD:
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			CClan::GetInstance().SetNotice( (const char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) );
		}
		break;
	case RESULT_CLAN_SLOGAN:
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			CClan::GetInstance().SetSlogan( (const char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) );
		}
		break;
	case RESULT_CLAN_POSITION://			0x083		// Å¬·£¿¡¼­ÀÇ ³ªÀÇ µî±ÞÀÌ Á¶Á¤µÇ¾úÀ»¶§ 
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			char* pszMaster = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset );
			BYTE* pbtClass  = (BYTE*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( BYTE ) );
			CClan::GetInstance().SetClass( *pbtClass );
			if( g_pAVATAR && g_pAVATAR->GetClanMarkBack() == 0 && *pbtClass > CClan::CLAN_MASTER )
				g_pNet->Send_cli_CLANMARK_REG_TIME();


			if( pszMaster )
			{
				std::string strMsg = CStr::Printf(STR_CLAN_RESULT_CLAN_POSITION,CStringManager::GetSingleton().GetClanClass( *pbtClass ) );
				strMsg.append(":");
				strMsg.append( pszMaster );

				g_itMGR.OpenMsgBox( strMsg.c_str() ); 
			}
		}
		break;
	case RESULT_CLAN_SET:
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			WORD* pwObjIDX			= (WORD*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( WORD ));
			tag_CLAN_ID* pClanID	= (tag_CLAN_ID*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_ID) );
			assert( pwObjIDX && pClanID );
			if( pwObjIDX && pClanID )
			{
				CObjAVT* pAvt = g_pObjMGR->Get_ClientCharAVT ( *pwObjIDX, false );
				if( pAvt )
				{
					if( pClanID->m_dwClanID == 0 )
					{
						pAvt->ResetClan();		
					}
					else
					{
						if( char* pszClanName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
						{
							if( pClanID->m_wClanMARK[0] == 0 && pAvt->GetClanMarkCenter() != pClanID->m_wClanMARK[1] )
								g_pNet->Send_cli_CLANMARK_REQ( pClanID->m_dwClanID );

							pAvt->SetClan( pClanID->m_dwClanID, pClanID->m_wClanMARK[0], pClanID->m_wClanMARK[1], pszClanName , pClanID->m_btClanLEVEL , pClanID->m_btClanPOS );
						}
					}
				}
			}
		}
		break;
	case RESULT_CLAN_MEMBER_LOGIN:// ÀüÃ¼¿¡°Ô º¸³»Áö´Â ¸â¹ö ·Î±×ÀÎ
	case RESULT_CLAN_MEMBER_LOGOUT:// ÀüÃ¼¿¡°Ô º¸³»Áö´Â ¸â¹ö ·Î±×¾Æ¿ô
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_CLAN_MEMBER* pMember = (tag_CLAN_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_MEMBER ) ) )
			{
				if( char* pszCharName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
					CClan::GetInstance().ChangeConnectStatus( pszCharName, pMember->m_btChannelNO );
			}
		}
		break;
	case RESULT_CLAN_MEMBER_POSITION://		0x075		// ÀüÃ¼¿¡°Ô º¸³»Áö´Â ¸â¹ö Á÷À§Á¶Àý
		{
			short nOffset = sizeof( wsv_CLAN_COMMAND );
			if( tag_CLAN_MEMBER* pMember = (tag_CLAN_MEMBER*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof( tag_CLAN_MEMBER ) ) )
			{
				if( char* pszCharName = (char*)Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
					CClan::GetInstance().ChangeClass(pszCharName, pMember->m_btClanPOS );
			}
		}
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_wsv_CLAN_CHAT()
{
	short nOffset = sizeof( gsv_SHOUT );
	char *szAccount, *szMsg;
	szAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	szMsg     = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	g_itMGR.AppendChatMsg( CStr::Printf("[%s]%s>%s","CLAN", szAccount, szMsg), IT_MGR::CHAT_TYPE_CLAN);
}

/// ¼­¹ö·Î ºÎÅÍ Àü¼Û¹ÞÀº Å¬·£ ¸¶Å© ÀúÀå..
void CRecvPACKET::Recv_wsv_CLANMARK_REPLY()
{
	int iClanID = (int)( m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_dwClanID );
	if( iClanID )//¼º°ø¹× º¯°æµÇ¾úÀ½
	{
		WORD wCRC16 = m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_wMarkCRC16;	

		int iSize = m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_nSize - sizeof( wsv_CLANMARK_REPLY );	
		short nOffset = sizeof( wsv_CLANMARK_REPLY );
		BYTE* pDATA = (BYTE*)Packet_GetStringPtr( m_pRecvPacket, nOffset );

		///È­ÀÏ·Î ÀúÀå
		CClanMarkTransfer::GetSingleton().ReceiveMarkFromServer( iClanID, wCRC16, pDATA, iSize );

		///ÇöÀç µî·ÏµÇ¾î¼­ »ç¿ëÁßÀÎ Å¬·£¸¶Å©°¡ º¯°æµÇ¾úÀ»°æ¿ì Texture¸¦ º¯°æÇÑ´Ù.
		///»ç¿ëÁßÀÌÁö ¾Ê´Ù¸é ¾Æ¹« µ¿ÀÛµµ ÇÏÁö ¾Ê´Â´Ù.

		std::string file_name;
		CClanMarkUserDefined::GetClanMarkFileName( CGame::GetInstance().GetSelectedServerID(), iClanID, file_name );

		CClanMarkManager::GetSingleton().ReloadTexture( file_name.c_str(), wCRC16 );

		if( g_pAVATAR && g_pAVATAR->GetClanID() == iClanID && g_pAVATAR->GetClanPos() >= CClan::CLAN_MASTER )
		{
			///ÇöÀç 2¹ø ³»·Á¿Â´Ù.
			if( g_pAVATAR->GetClanMarkCenter() != wCRC16 )
			{
				g_pNet->Send_cli_CLANMARK_REG_TIME();
				g_itMGR.OpenMsgBox( STR_CLANMARK_REGISTER_SUCCESS );
			}
		}

		/// ÇöÀç ÇØ´ç Å¬·£¿øµé(³ªÆ÷ÇÔ)ÀÇ ¸ðµç Å¬·£¸¶Å©Á¤º¸¸¦ °»½ÅÇØÁÖ¾î¾ß ÇÑ´Ù.
		/// Å¬·£¸¶Å©°¡ »õ·Î µî·ÏµÈ °æ¿ì¿¡´Â CNameBox¿¡¼­ »õ·Î CClanMarkUserDefined¸¦ Ã£À»°ÍÀÌ°í
		/// µî·ÏµÇ¾î ÀÖ´ø ¸¶Å©°¡ º¯°æ½Ã¿¡´Â À§¿¡¼­ Texture°¡ ¹Ù²î¹Ç·Î º°µµ·Î ÇØÁÙ ÀÛ¾÷ÀÌ ¾ø´Ù.
		g_pObjMGR->ResetClanMarkInfo( iClanID, wCRC16 );
		//g_pAVATAR->SetClanMark( 0, wCRC16 );
	}
	else//½ÇÆÐ
	{
		switch( m_pRecvPacket->m_wsv_CLANMARK_REPLY.m_wFailedReason )
		{
		case RESULT_CLANMARK_TOO_MANY_UPDATE:// ¸¶Å© °»½ÅÀº ÀÏÁ¤½Ã°£ ÈÄ¿¡ ÇØ¾ß ÇÑ´Ù...
			g_itMGR.OpenMsgBox( STR_CLANMARK_UPDATE_ERROR );
			break;
		case RESULT_CLANMARK_DB_ERROR:// µðºñ °»½Å ¿À·ù
			g_itMGR.OpenMsgBox("Register ClanMark Error:DB");
			break;
		case RESULT_CLANMAKR_SP_ERROR://			0x0003	// µðºñ SP ¿À·ù
			g_itMGR.OpenMsgBox("Register ClanMark Error:SP");
			break;
		default:
			g_itMGR.OpenMsgBox("Register ClanMark Error");
			break;
		}
	}
}

void CRecvPACKET::Recv_gsv_ALLIED_CHAT()
{

	short nOffset = sizeof( gsv_ALLIED_CHAT );

	CObjAVT *pCHAR = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_ALLIED_CHAT.m_wObjectIDX, false );
	if ( pCHAR && g_pAVATAR->Get_TeamNO() == m_pRecvPacket->m_gsv_ALLIED_CHAT.m_iTeamNO ) 
	{
		char* pszMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);
		g_itMGR.AppendChatMsg( CStr::Printf("[%s]%s>%s", STR_ALLIED, pCHAR->Get_NAME(), pszMsg), IT_MGR::CHAT_TYPE_ALLIED );
	}
}

void CRecvPACKET::Recv_gsv_ALLIED_SHOUT()
{
	short nOffset = sizeof( gsv_ALLIED_SHOUT );
	char* pszAccount = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	char* szMsg		 = Packet_GetStringPtr( m_pRecvPacket, nOffset);
	if ( g_pAVATAR && pszAccount && szMsg && g_pAVATAR->Get_TeamNO() == m_pRecvPacket->m_gsv_ALLIED_SHOUT.m_iTeamNO ) 
		g_itMGR.AppendChatMsg ( CStr::Printf("[%s]%s>%s", STR_ALLIED, pszAccount, szMsg), IT_MGR::CHAT_TYPE_ALLIED );
}

void CRecvPACKET::Recv_gsv_ITEM_RESULT_REPORT()
{
	WORD wObjectIdx = m_pRecvPacket->m_gsv_ITEM_RESULT_REPORT.m_wObjectIDX;
	if( CObjAVT* pAvt = g_pObjMGR->Get_ClientCharAVT( wObjectIdx, false ) )
	{
		switch( m_pRecvPacket->m_gsv_ITEM_RESULT_REPORT.m_btREPORT )
		{
		case REPORT_ITEM_CREATE_START://		0x00	// ¾ÆÀÌÅÛ Á¦Á¶ ½ÃÀÛ
			SE_StartMake ( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_CREATE_SUCCESS://		0x01	// ¾ÆÀÌÅÛ Á¦Á¶ ¼º°ø
			SE_SuccessMake( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_CREATE_FAILED://		0x02	// ¾ÆÀÌÅÛ Á¦Á¶ ½ÇÆÐ
			SE_FailMake( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_UPGRADE_START://		0x03	// ¾ÆÀÌÅÛ Á¦·Ã ½ÃÀÛ
			SE_StartUpgrade( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_UPGRADE_SUCCESS://		0x04	// ¾ÆÀÌÅÛ Á¦·Ã ¼º°ø
			SE_SuccessUpgrade( pAvt->Get_INDEX() );
			break;
		case REPORT_ITEM_UPGRADE_FAILED://		0x05	// ¾ÆÀÌÅÛ Á¦·Ã ½ÇÆÐ
			SE_FailUpgrade( pAvt->Get_INDEX() );
			break;
		default:
			break;
		}
	}
}

void CRecvPACKET::Recv_gsv_MALL_ITEM_REPLY()
{
	switch( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE )
	{
	case REPLY_MALL_ITEM_CHECK_CHAR_FOUND:
		///¼±¹°ÇÒ ´ë»óÀ» Ã£¾Ò´Ù.
		/// ÀúÀåÇØµÎ¾ú´ø Ä³¸¯ÅÍ¸í°ú ¾ÆÀÌÅÛÀÌ¸§À» °¡Á®¿Í¼­ ¸Þ¼¼Áö ¹Ú½º¿¡ È®ÀÎ ¹öÆ°¿¡ ÇÒ´çµÉ Command ¸¦ »ý¼ºÇØ¼­ OpenÇÑ´Ù.
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{

			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			assert( p->get_selecteditem_slotindex() >= 1 );
			if( p->get_selecteditem_slotindex() >= 1 )
			{
				CTCommand* pCmdOk = new CTCmdGiftMallItem( (BYTE)(p->get_selecteditem_slotindex() - 1 ), p->get_receiver_name() );
				g_itMGR.OpenMsgBox( CStr::Printf("%s:%s{BR}%s:%s", STR_RECEIVER, p->get_receiver_name(), STR_SENDING_ITEM ,p->GetSelectedItemName() ), CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmdOk );
			}
		}	

		break;
	case REPLY_MALL_ITEM_CHECK_CHAR_NONE:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_CHECK_CHAR_NONE);
		break;
	case REPLY_MALL_ITEM_CHECK_CHAR_INVALID:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_CHECK_CHAR_INVALID);
		break;
	case REPLY_MALL_ITEM_BRING_SUCCESS:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			g_pAVATAR->Set_ITEM( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_btInvIDX,
				m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_ITEM );

			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			p->SetItem( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[1].m_btInvIDX,
				m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[1].m_ITEM	);///DeliveryStore¿¡¼­ Áö¿î´Ù.
		}
		break;
	case REPLY_MALL_ITEM_BRING_FAILED:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_BRING_FAILED);
		break;
	case REPLY_MALL_ITME_GIVE_SUCCESS:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			p->SetItem( m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_btInvIDX,
				m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_BringITEM[0].m_ITEM );

			g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITME_GIVE_SUCCESS);
		}
		break;
	case REPLY_MALL_ITEM_GIVE_FAILED:
		g_itMGR.OpenMsgBox(STR_REPLY_MALL_ITEM_GIVE_FAILED);
		break;
	case REPLY_MALL_ITEM_LIST_START:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
			p->ClearItem();
		}
		break;
	case REPLY_MALL_ITEM_LIST_DATA:
		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
		{
			CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;

			short nOffset = sizeof( gsv_MALL_ITEM_REPLY );

			tagITEM* pItem = NULL;
			char* pszFrom	= NULL;
			char* pszDesc	= NULL;
			char* pszTo		= NULL;

			for( int count = 0; count < m_pRecvPacket->m_gsv_MALL_ITEM_REPLY.m_btCntOrIdx; ++count )
			{
				pItem = (tagITEM*)Packet_GetDataPtr( m_pRecvPacket, nOffset, sizeof(tagITEM) );
				pszFrom = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				if( pszFrom[0] != '\0' )
				{
					pszDesc = Packet_GetStringPtr( m_pRecvPacket, nOffset );
					pszTo   = Packet_GetStringPtr( m_pRecvPacket, nOffset );
				}
				p->AddItem( pItem , pszFrom , pszDesc, pszTo );

				pszFrom = NULL;
				pszDesc = NULL;
				pszTo   = NULL;
			}
		}
		break;
	case REPLY_MALL_ITEM_LIST_END:
		///?????
		break;
	default:
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//	ÀÏº»ÂÊ ºô¸µ Ã³¸®
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_JPN()
{
	CGame::GetInstance().SetPayType( 0 );

	g_itMGR.AppendChatMsg( STR_JP_PAY_TYPE, IT_MGR::CHAT_TYPE_SYSTEM );				

	if(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0])
	{
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_BATTLE )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_ENTRY, IT_MGR::CHAT_TYPE_SYSTEM );				
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_COMMUNITY )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_COMMUNITY, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_TRADE )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_TRADE, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STOCK_SPACE )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_STOCKSPACE, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_STOCK )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_EXTRASTOCK, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STARSHIP_PASS )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_STARSHIPPASS, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_DUNGEON_ADV )
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_DUNGEONADVENTURE, IT_MGR::CHAT_TYPE_SYSTEM );			
		}
		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_CHAR)
		{
			g_itMGR.AppendChatMsg( STR_JP_PAY_EXTRACHARACTER, IT_MGR::CHAT_TYPE_SYSTEM );			
		}		
	}	
	else
	{
		g_itMGR.AppendChatMsg( STR_JP_PAY_FREEPLAY, IT_MGR::CHAT_TYPE_SYSTEM );
	}

	CGame::GetInstance().SetPayType( (WORD)m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] );
}


//¿ì¼± TW ³Ö¾îµÐ´Ù... ¾ÆÁ÷ ¾²Áö´Â ¾Ê´Â´Ù.
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_TW()
{
	switch(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType)
	{
	case 0:	//±ÝÁö
		break;
	case 1: //½Ã°£Á¦, ¸Å 1 ½Ã°£ 6Æ÷ÀÎÆ® »è°¨, 1½Ã°£ÀÌ Ã¤¿ìÁö ¸øÇßÀ» °æ¿ì 1½Ã°£À¸·Î °è»ê.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_1HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 2:	//¿ùÁ¤¾×Á¦,  30ÀÏ ±âÁØ 350 Æ÷ÀÎÆ® »è°¨.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 3:	//½Ã°£´ÜÀ§ Á¤¾×Á¦,  12½Ã°£À» ±âÁØÀ¸·Î 20Æ÷ÀÎÆ® »è°¨.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_12HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 4:	//3°³¿ù Á¤¾×Á¦, 90ÀÏÀ» ±âÁØÀ¸·Î 888Æ÷ÀÎÆ® »è°¨.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_3MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 5:	//È«Äá ¿ù´ÜÀ§ Á¤¾×Á¦.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 6:	//È«Äá 3°³¿ù Á¤¾×Á¦.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_3MONTH, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 7:	//È«Äá ½Ã°£Á¦
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 8:	//È«Äá ½Ã°£´ÜÀ§ Á¤¾×Á¦.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_HONGKONG_12HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 9:	//ºÐ Á¤¾×Á¦, ¸Å ºÐ ´ÜÀ§·Î 0.25Æ÷ÀÎÆ® »è°¨.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_MINUTE, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 10:	//2½Ã°£ ´ÜÀ§ Á¤¾×Á¦, ¸Å 2½Ã°£ ±âÁØÀ¸·Î 12Æ÷ÀÎÆ® »è°¨.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_2HOUR, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 11:	//10ÀÏ Á¤¾×Á¦, ¸Å 10ÀÏ ±âÁØÀ¸·Î 150Æ÷ÀÎÆ® »è°¨.
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_10DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 12:	//¹«·á Ã¼ÇèÁ¦
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_FREE, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 101:	//ºí·Ï 3ÀÏ
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_BLOCK_3DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 102:	//ºí·Ï7ÀÏ
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_BLOCK_7DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	case 103:	//ºí·Ï10ÀÏ
		g_itMGR.AppendChatMsg( STR_TW_BILL_TYPE_BLOCK_10DAY, IT_MGR::CHAT_TYPE_SYSTEM );
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//	ÇÑ±¹, ÇÊ¸®ÇÉÂÊ ºô¸µ Ã³¸®
//
//	°³ÀÎÁ¤¾×(ÇÃ·¹Æ¼³Ñ)ÀÌ ¾Æ´Ñ °æ¿ì´Â ¸ðµÎ ÇÁ¸®¹Ì¾ö ÀÔ´Ï´Ù.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_KOR_And_PH()
{
	switch(m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType)
	{
	case PAY_FLAG_KR_FU:
		g_itMGR.AppendChatMsg( STR_FREE_ACCOUNT_USER, IT_MGR::CHAT_TYPE_SYSTEM );			
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_CHAR | PLAY_FLAG_EXTRA_STOCK );
		break;

	case PAY_FLAG_KR_FA:
		{
			g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );				
			CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
			break;
		}			

	case PAY_FLAG_KR_FAP:
		g_itMGR.AppendChatMsg( STR_PAID_DAYBASED_PLAN_PLATINUM, IT_MGR::CHAT_TYPE_SYSTEM );			
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_CHAR | PLAY_FLAG_EXTRA_STOCK );
		break;
	case PAY_FLAG_KR_FQ:
		g_itMGR.AppendChatMsg( STR_PAID_TIMEBASED_PLAN, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;
	case PAY_FLAG_KR_GU:
		g_itMGR.AppendChatMsg( STR_PAID_INTERNET_CAFE, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;
	case PAY_FLAG_KR_GQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_TIME_END, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;
	case PAY_FLAG_KR_IQ:
		g_itMGR.AppendChatMsg( STR_PAID_USER_PLAN_IC_IP_FULL, IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( PLAY_FLAG_KOREA_DEFAULT );
		break;		
	}	
}	

///////////////////////////////////////////////////////////////////////////////////////////////////////
// BILLING_MESSAGE ¿¡¼­ Ã³¸® ÇØÁÖ´ø ºÎºÐ.
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Set_BILLING_MESSAGE_EXT_Other()
{
	switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE )
	{
	case BILLING_MSG_KICKOUT_CHEAT_DETECT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_CHEAT_DETECT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_ALREADY_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_ALREADY_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_TIME_EXPIRED:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_TIME_EXPIRED ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_DUP_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_DUP_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_NO_RIGHT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_NO_RIGHT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_OUT_OF_IP:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_OUT_OF_IP ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_EXIST_ACCOUNT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_EXIST_ACCOUNT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_TYPE_SET_DATE:///¸¸·á ÀÏÀÚ
		{
			if( CGame::GetInstance().GetDetailPayType() != PAY_FLAG_KR_FU )
			{
				short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );

				if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					std::string strTime = pszRemainTime;
					CGame::GetInstance().SetExpireTime( pszRemainTime );
					CGame::GetInstance().SetExpireTimeType( 1 );

					if( strTime.size() >= 12 )
					{
						int year  = atoi( strTime.substr( 0, 4 ).c_str() );
						int month = atoi( strTime.substr( 4, 2 ).c_str() );
						int day   = atoi( strTime.substr( 6, 2 ).c_str() );
						int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
						int min   = atoi( strTime.substr( 10, 2 ).c_str() );

						g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_EXPIRED_TIME, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
					}
				}
			}
		}
		break;
	case BILLING_MSG_TYPE_SET_TIME:///³²Àº ºÐ
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );
				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf(STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_SYSTEM );
			}
		}
		break;
	case BILLING_MSG_TYPE_TIME_ALERT:///³²Àº ºÐ
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );

				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf( STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_ERROR );
			}
		}
		break;
	case BILLING_MSG_TYPE_TAIWAN:
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );
			if( char* pszMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				g_itMGR.AppendChatMsg( pszMsg, IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	case BILLING_MSG_KICKOUT_TAIWAN:
		{
			//g_itMGR.AppendChatMsg( "Disconnect From Billing Server", IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// È®ÀåµÈ ºô¸µÃ³¸®.
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecvPACKET::Recv_gsv_BILLING_MESSAGE_EXT()
{
	///Àû´çÇÑ ¸Þ¼¼Áö Ãâ·Â
	//CGame::GetInstance().SetExpireTimeType( 0 );
	//CGame::GetInstance().SetPayType( 0 );

	std::string strBuf;

	switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE )
	{
	case EXT_BILLING_MSG_PAY_KOREA:	//korea, Philippines.
		Set_BILLING_MESSAGE_EXT_KOR_And_PH();
		break;

	case EXT_BILLING_MSG_PAY_TAIWAN:
		Set_BILLING_MESSAGE_EXT_TW();
		break;

	case EXT_BILLING_MSG_PAY_JAPAN:		
		Set_BILLING_MESSAGE_EXT_JPN();					
		break;

	case EXT_BILLING_MSG_TYPE_SET_DATE: //¸¸·áÀÏÀÚ

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType==0 
			&& m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] == 0)
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE_EXT );

			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{

				//ExpireTime
				g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_JP_PAY_EXPIRE_DATE), IT_MGR::CHAT_TYPE_SYSTEM );

				char* p = strtok( pszRemainTime, "\t" );
				while( p != NULL )
				{
					//type
					std::string strType = p;

					//time
					p = strtok( NULL,"\t" );					

					std::string strTime;
					if(p)
					{
						strTime = p;
					}

					//ÇÔ¼ö ÀÛ¼º.

					int iType = atoi( strType.c_str() );

					CGame::GetInstance().SetExpireTimeExt( iType-1, strTime.c_str() );
					CGame::GetInstance().SetExpireTimeTypeExt( iType-1, 1 );

					if( strTime.size() >= 12 )
					{
						int year  = atoi( strTime.substr( 0, 4 ).c_str() );
						int month = atoi( strTime.substr( 4, 2 ).c_str() );
						int day   = atoi( strTime.substr( 6, 2 ).c_str() );
						int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
						int min   = atoi( strTime.substr( 10, 2 ).c_str() );

						switch(iType-1)
						{
						case BILL_FLAG_JP_BATTLE:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_ENTRY, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_COMMUNITY:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_COMMUNITY, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_TRADE:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_TRADE, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_STOCK_SPACE:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_STOCKSPACE, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_EXTRA_STOCK:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_EXTRASTOCK, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_STARSHIP_PA:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_STARSHIPPASS, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_DUNGEON_ADV:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_DUNGEONADVENTURE, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						case BILL_FLAG_JP_EXTRA_CHAR:
							g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_JP_PAY_EXTRACHARACTER, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
							break;
						}											
					}
					p = strtok( NULL,"\t" );
				}
			}
		}

		break;

	case EXT_BILLING_MSG_TYPE_SET_TIME:	//³²ÀººÐ

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType==0 
			&& m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] == 0)
		{
			short nOffset = sizeof( GSV_BILLING_MESSAGE_EXT );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{

				//³²Àº ½Ã°£
				g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_JP_PAY_REMAIN_TIME), IT_MGR::CHAT_TYPE_SYSTEM );


				char* p = strtok( pszRemainTime, "\t" );
				while( p != NULL )
				{
					//type
					std::string strType = p;

					//time
					p = strtok( NULL,"\t" );
					std::string strTime = p;

					int iType = atoi( strType.c_str() );

					CGame::GetInstance().SetExpireTimeExt( iType-1, strTime.c_str() );					
					CGame::GetInstance().SetExpireTimeTypeExt( iType-1, 2 );

					DWORD time = atoi( strTime.c_str() );					

					switch(iType-1)
					{
					case BILL_FLAG_JP_BATTLE:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_ENTRY , strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_COMMUNITY:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_COMMUNITY, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_TRADE:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_TRADE, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_STOCK_SPACE:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_STOCKSPACE, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_EXTRA_STOCK:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_EXTRASTOCK, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_STARSHIP_PA:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_STARSHIPPASS, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_DUNGEON_ADV:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_DUNGEONADVENTURE, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					case BILL_FLAG_JP_EXTRA_CHAR:
						g_itMGR.AppendChatMsg( CStr::Printf(STR_JP_PAY_REMAIN_TIME_MESSAGE, STR_JP_PAY_EXTRACHARACTER, strTime.c_str() ), IT_MGR::CHAT_TYPE_SYSTEM );
						break;
					}	


					p = strtok( NULL,"\t" );
				}			
			}
		}

		break;

	case EXT_BILLING_MSG_TYPE_TIME_ALERT:
	case EXT_BILLING_MSG_TYPE_TIME_EXPIRED:

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE == EXT_BILLING_MSG_TYPE_TIME_ALERT )
		{
			//Expired Alert
			strBuf = STR_JP_PAY_EXPIRE_ALERT;
		}
		else
		{
			//Expired.
			strBuf = STR_JP_PAY_EXPIRE;			
		}

		if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPayType==0 )
		{
			//ÀÏº»
			if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] )
			{                
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_BATTLE )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_ENTRY), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_COMMUNITY )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_COMMUNITY), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_TRADE )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_TRADE), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STOCK_SPACE )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_STOCKSPACE), IT_MGR::CHAT_TYPE_SYSTEM );					
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_STOCK )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_EXTRASTOCK), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_STARSHIP_PASS )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_STARSHIPPASS), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_DUNGEON_ADV )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_DUNGEONADVENTURE), IT_MGR::CHAT_TYPE_SYSTEM );
				}
				if( m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0] & PAY_FLAG_JP_EXTRA_CHAR)
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s : %s", strBuf.c_str(), STR_JP_PAY_EXTRACHARACTER), IT_MGR::CHAT_TYPE_SYSTEM );
				}

				if(  m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_wMsgTYPE == EXT_BILLING_MSG_TYPE_TIME_EXPIRED )
				{
					DWORD dwCurPayType = CGame::GetInstance().GetPayType();

					dwCurPayType &= ~m_pRecvPacket->m_gsv_BILLING_MESSAGE_EXT.m_dwPlayingFlag[0];

					CGame::GetInstance().SetPayType( (WORD)dwCurPayType );
				}				
			}			
		}
		break;

	default:
		//BILLING_MSG µéµµ ¿Ã°Å °°¾Æ¼­ ÇØ ³ù´Âµ¥ ¾È¾µ°Í °°´Ù.
		Set_BILLING_MESSAGE_EXT_Other();

	}	   
}

void CRecvPACKET::Recv_gsv_BILLING_MESSAGE()
{
	///Àû´çÇÑ ¸Þ¼¼Áö Ãâ·Â
	CGame::GetInstance().SetExpireTimeType( 0 );
	switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE )
	{
		//case BILLING_MSG_FREE_USER:
		//	break;

	case BILLING_MSG_JPN_NEED_CHARGE:

		switch( m_pRecvPacket->m_gsv_BILLING_MESSAGE2.m_cFunctionTYPE )
		{
		case 'H':	//clan skill				
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_CLANSKILL );
			break;

		case 'I':	//basic field		
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_MOVE_PLANET );
			break;

		case 'P':	// space taravel
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_MOVE_PLANET2 );
			break;

		case 'Q':	// Dungeon Adventure
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_ENTER_DUNGEON );
			break;
		}

		break;

	case BILLING_MSG_PAY_FU:
		g_itMGR.AppendChatMsg( "Æò»ý¹«·á »ç¿ëÀÚÀÔ´Ï´Ù", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_FA:
		if( CGame::GetInstance().GetDetailPayType() != BILLING_MSG_FREE_USER )
		{
			g_itMGR.AppendChatMsg( "°³ÀÎÁ¤¾× »ç¿ëÀÚÀÔ´Ï´Ù", IT_MGR::CHAT_TYPE_SYSTEM );
			CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		}
		else
		{
			g_itMGR.AppendChatMsg( "¹«·á »ç¿ëÀÚÀÔ´Ï´Ù", IT_MGR::CHAT_TYPE_SYSTEM );
			CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		}
		break;
	case BILLING_MSG_PAY_FAP:
		g_itMGR.AppendChatMsg( "°³ÀÎÁ¤¾×(ÇÃ·¹Æ¼³Ñ) »ç¿ëÀÚÀÔ´Ï´Ù", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_FQ:
		g_itMGR.AppendChatMsg( "°³ÀÎÁ¤·® »ç¿ëÀÚÀÔ´Ï´Ù",IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_GU:
		g_itMGR.AppendChatMsg( "°ÔÀÓ¹æ »ç¿ëÀÚ·Î Á¢¼ÓµÇ¾ú½À´Ï´Ù", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_GQ:
		g_itMGR.AppendChatMsg( "°ÔÀÓ¹æÀÇ °ú±Ý±â°£(½Ã°£)ÀÌ ¸¸·áµÇ¾î °³ÀÎÁ¤·®À¸·Î Á¢¼ÓµÇ¾ú½À´Ï´Ù", IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_PAY_IQ:
		g_itMGR.AppendChatMsg( "°ÔÀÓ¹æÀÇ Á¢¼Ó °¡´É IP¼ýÀÚ¸¦ ³Ñ¾î °³ÀÎÁ¤·®À¸·Î Á¢¼ÓµÇ¾ú½À´Ï´Ù",IT_MGR::CHAT_TYPE_SYSTEM );
		CGame::GetInstance().SetPayType( m_pRecvPacket->m_gsv_BILLING_MESSAGE.m_btTYPE );
		break;
	case BILLING_MSG_KICKOUT_CHEAT_DETECT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_CHEAT_DETECT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_ALREADY_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_ALREADY_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_TIME_EXPIRED:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_TIME_EXPIRED ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_DUP_LOGIN:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_DUP_LOGIN ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_NO_RIGHT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_NO_RIGHT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_OUT_OF_IP:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_OUT_OF_IP ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_KICKOUT_EXIST_ACCOUNT:
		g_itMGR.AppendChatMsg( STR_BILLING_MSG_KICKOUT_EXIST_ACCOUNT ,IT_MGR::CHAT_TYPE_SYSTEM);
		break;
	case BILLING_MSG_TYPE_SET_DATE:///¸¸·á ÀÏÀÚ
		{
			if( CGame::GetInstance().GetDetailPayType() != BILLING_MSG_PAY_FU )
			{
				short nOffset = sizeof( gsv_BILLING_MESSAGE );

				if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				{
					std::string strTime = pszRemainTime;
					CGame::GetInstance().SetExpireTime( pszRemainTime );
					CGame::GetInstance().SetExpireTimeType( 1 );

					if( strTime.size() >= 12 )
					{
						int year  = atoi( strTime.substr( 0, 4 ).c_str() );
						int month = atoi( strTime.substr( 4, 2 ).c_str() );
						int day   = atoi( strTime.substr( 6, 2 ).c_str() );
						int hour  = atoi( strTime.substr( 8, 2 ).c_str() );
						int min   = atoi( strTime.substr( 10, 2 ).c_str() );

						g_itMGR.AppendChatMsg( CStr::Printf("%s:%d/%d/%d %d:%02d", STR_EXPIRED_TIME, year, month, day, hour, min ), IT_MGR::CHAT_TYPE_SYSTEM );
					}
				}
			}
		}
		break;
	case BILLING_MSG_TYPE_SET_TIME:///³²Àº ºÐ
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );
				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf(STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_SYSTEM );
			}
		}
		break;
	case BILLING_MSG_TYPE_TIME_ALERT:///³²Àº ºÐ
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE );
			if( char* pszRemainTime = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
			{
				CGame::GetInstance().SetExpireTime( pszRemainTime );
				CGame::GetInstance().SetExpireTimeType( 2 );

				DWORD time = atoi( pszRemainTime );
				g_itMGR.AppendChatMsg( CStr::Printf( STR_GAME_TIME_LEFT_MINS, time ), IT_MGR::CHAT_TYPE_ERROR );
			}
		}
		break;
	case BILLING_MSG_TYPE_TAIWAN:
		{
			short nOffset = sizeof( gsv_BILLING_MESSAGE );
			if( char* pszMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset ) )
				g_itMGR.AppendChatMsg( pszMsg, IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	case BILLING_MSG_KICKOUT_TAIWAN:
		{
			//g_itMGR.AppendChatMsg( "Disconnect From Billing Server", IT_MGR::CHAT_TYPE_SYSTEM );
		}
		break;
	default:
		break;
	}
}

void CRecvPACKET::Recv_wsv_CLANMARK_REG_TIME()
{
	SYSTEMTIME clanmark_regtime;

	clanmark_regtime.wYear  = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_wYear;
	clanmark_regtime.wMonth = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btMon;
	clanmark_regtime.wDay   = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btDay;
	clanmark_regtime.wHour  = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btHour;
	clanmark_regtime.wMinute = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btMin;
	clanmark_regtime.wSecond = m_pRecvPacket->m_wsv_CLANMARK_REG_TIME.m_btSec;

	CClan::GetInstance().SetClanMarkRegTime( clanmark_regtime );
}

void CRecvPACKET::Recv_gsv_MOVE_ZULY()
{
	if( g_pAVATAR )
	{
		CBank::GetInstance().SetMoney( m_pRecvPacket->m_gsv_MOVE_ZULY.m_i64BankZuly );
		g_pAVATAR->Set_MONEY( m_pRecvPacket->m_gsv_MOVE_ZULY.m_i64InvZuly );
	}	
}


//----------------------------------------------------------------------------------------------------	
///¹Ú ÁöÈ£ :: ¿©½Å ¼ÒÈ¯ ÆÐÅ¶Ã³¸® 
///ÃÖ Á¾Áø :: - ´É·ÂÄ¡ º¯°æ Àç°è»êÈÄ ÀúÀå : 2005/7/12 : nAvy
///           - ´º¹öÁ¯¿¡¼­¸¸ ÀÌÆÐÅ¶ÀÌ ¿Â´Ù°í °¡Á¤ 
void CRecvPACKET::Recv_gsv_GODDNESS_MODE()
{

	LOGOUT( "¿©½Å¼ÒÈ¯ ¸í·É ¹ÞÀ½ On/Off [ %d ]",m_pRecvPacket->m_gsv_GODDNESS_MODE.m_btOnOff);
	goddessMgr.SetProcess(m_pRecvPacket->m_gsv_GODDNESS_MODE.m_btOnOff,
		m_pRecvPacket->m_gsv_GODDNESS_MODE.m_wObjectIDX);


	//short	client_object_index = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_GODDNESS_MODE.m_wObjectIDX);
	//if( client_object_index == g_pAVATAR->Get_INDEX() )
	//	g_pAVATAR->Calc_AruaAddAbility();


}

void CRecvPACKET::Recv_gsv_PATSTATE_CHANGE()
{
	_RPT1( _CRT_WARN,"m_gsv_PATSTATE_CHANGE.m_btOnOff is %d\n", m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_btOnOff );

	if( g_pAVATAR && g_pObjMGR && g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) == m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_wObjectIDX )
	{
		g_pAVATAR->SetCur_PatHP( m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_nMaxPatHP );
		g_pAVATAR->SetCur_PatCoolTIME( m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_dwCoolTIME );

		///¹ÚÁöÈ£::Å¾½Â½ºÅ³¿¡ °üÇÑ Å¸ÀÓÀ» ¼³Á¤ÇÑ´Ù. 	
		if( 0 == m_pRecvPacket->m_gsv_PATSTATE_CHANGE.m_btOnOff )
			g_SoloSkillDelayTick.SetUseItemDelay(17, g_SkillList.GetDelayTickCount(17));

	}
}


void CRecvPACKET::Recv_gsv_CHARSTATE_CHANGE()
{
	CObjAVT *pNewAVT = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CHARSTATE_CHANGE.m_wObjectIDX, true ); 
	if ( pNewAVT ) 
	{
		//------------------------------------------------------------------------------------
		/// Æ¯¼öÇÑ »óÅÂ ÇÃ·¡±×
		//------------------------------------------------------------------------------------
		pNewAVT->ChangeSpecialState( m_pRecvPacket->m_gsv_CHARSTATE_CHANGE.m_dwFLAG );		
	}
}

// 2ÀÎ½Â Ä«Æ®.
void CRecvPACKET::Recv_gsv_CART_RIDE()
{


	switch( m_pRecvPacket->m_gsv_CART_RIDE.m_btType )
	{
	case CART_RIDE_REQ:				// Å»·¡ ?	m_wGuestObjIDX¿¡°Ô¸¸ Àü¼ÛµÊ
		{ 
			//Find Index Owner.
			int iOwnerObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX );
			CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iOwnerObjClientIndex, true );

			if( pAVT == NULL)
			{
				return;
			}	

			// %s´ÔÀÌ ½ÂÂ÷¸¦ ¿ä±¸ ÇÏ¿´½À´Ï´Ù. Å¾½Â ÇÏ½Ã°Ú½À´Ï±î?
			sprintf( g_MsgBuf, STR_WANT_BOARDS, pAVT->Get_NAME() );

			CTCmdAcceptCartRide * pCmdOk		= new CTCmdAcceptCartRide( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX, m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
			CTCmdRejectCartRide * pCmdCancel	= new CTCmdRejectCartRide( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX, m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );

			g_itMGR.OpenMsgBox_CartRide( g_MsgBuf, CMsgBox::BT_OK | CMsgBox::BT_CANCEL, false, 0, pCmdOk, pCmdCancel , CMsgBox::MSGTYPE_RECV_CART_RIDE_REQ,
				m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX,
				m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );

		}
		break;	

	case CART_RIDE_ACCEPT:			// Å»²²		ÁÖº¯ ¸ðµç ÄÉ¸¯¿¡ Àü¼ÛµÊ
		{
			// Å¾½Â ÁßÀÌ¶ó »ç¿ë ÇÒ ¼ö ¾ø½À´Ï´Ù.
			// STR_BOARDING_CANT_USE

			// Å¾½ÂÇÒ ¼ö ¾ø½À´Ï´Ù.
			// STR_CANT_BOARD
			//-------------------------------------------------------------------------------------------------------------------
				//¹ÚÁöÈ£::2ÀÎ½Â Å¾½Â 
				CObjAVT *oSrc = g_pObjMGR->Get_ClientCharAVT( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX, true ); 
					if(oSrc == NULL)
						return;
			
				oSrc->SetRideUser(m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX);
			//-------------------------------------------------------------------------------------------------------------------

			//20050901 È«±Ù 2ÀÎ½Â Ä«Æ® º¸Á¶¼®¿¡ Åº »ç¶÷¸¸.
			int iGuestObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
			CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iGuestObjClientIndex, true );
			if( pAVT && pAVT->Get_INDEX() == g_pAVATAR->Get_INDEX() )
			{				
				g_pAVATAR->Set_Block_CartRide( true );
			}
		}

		break;

	case CART_RIDE_REFUSE:			// ¾ÈÅ»·¡	m_wOwnerObjIDX¿¡°Ô¸¸ Àü¼ÛµÊ
		{
			//Find Index Owner.
			int iGuestObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wGuestObjIDX );
			int iOwnerObjClientIndex = g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_CART_RIDE.m_wOwnerObjIDX );

			CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( iGuestObjClientIndex, true );

			if( pAVT == NULL )
			{
				return;
			}	

			// Owner¿¡°Ô ¸¸ ¸Þ¼¼Áö¸¦ º¸³½´Ù.
			if( iOwnerObjClientIndex == g_pAVATAR->Get_INDEX() )
			{
				// %s´ÔÀÌ ½ÂÂ÷¸¦ °ÅºÎÇß½À´Ï´Ù.			
				g_itMGR.AppendChatMsg(	CStr::Printf( STR_REFUSE_BOARD, pAVT->Get_NAME() ),
					IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
			}
			
			//-------------------------------------------------------------------------------------------------------------------
				//¹ÚÁöÈ£::Å¾½Â°ÅºÎ ¾Ö´Ï¸ÞÀÌ¼Ç 
				pAVT->Ride_Cansel_Motion();	
			//-------------------------------------------------------------------------------------------------------------------
		}
		break;

	case CART_RIDE_OWNER_NOT_FOUND:// ÅÂ¿ö ÁØ´Ù´ø ÄÉ¸¯ÀÌ »ç¶óÁ³´Ù
		// Å¾½ÂÇÒ ¼ö ¾ø½À´Ï´Ù.
		g_itMGR.AppendChatMsg(STR_CANT_BOARD, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		break;

	case CART_RIDE_GUEST_NOT_FOUND:// ÅÂ¿ì·Á´ø ÄÉ¸¯ÀÌ »ç¶óÁ³´Ù
		// Å¾½ÂÇÒ ¼ö ¾ø½À´Ï´Ù.
		g_itMGR.AppendChatMsg(STR_CANT_BOARD, IT_MGR::CHAT_TYPE_SYSTEM, D3DCOLOR_ARGB( 255, 206, 223, 136) );
		break;	
	}
}

void  CRecvPACKET::Recv_gsv_SCREEN_SHOT_TIME()
{
	SYSTEMTIME SysTime;
	SysTime.wYear = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.wYear;
	SysTime.wMonth = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btMonth;
	SysTime.wDay = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btDay;
	SysTime.wHour = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btHour;
	SysTime.wMinute = m_pRecvPacket->m_gsv_SCREEN_SHOT_TIME.btMin;

	CGame::GetInstance().ScreenCAPTURE(SysTime);

}

void CRecvPACKET::Recv_gsv_UPDATE_NAME()
{
	short nOffset = sizeof( gsv_UPDATE_NAME );
	char *szMsg;

	CObjAVT* pAVT = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( m_pRecvPacket->m_gsv_UPDATE_NAME.m_nCharID ), false );
	
	if( pAVT == NULL)
	{
		return;
	}

	szMsg = Packet_GetStringPtr( m_pRecvPacket, nOffset);

	pAVT->SetAvataName( szMsg );
}