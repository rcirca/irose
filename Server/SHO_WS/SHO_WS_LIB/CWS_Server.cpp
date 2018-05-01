
#include "stdAFX.h"
#include "SHO_WS_LIB.h"
#include "WS_SocketLSV.h"
#include "CWS_Server.h"
#include "CWS_Client.h"
#include "CWS_Party.h"
#include "WS_ZoneLIST.h"
#include "CThreadGUILD.h"

enum {
	SERVER_LISTCOL_USERCNT = 3,
	SERVER_LISTCOL_MAXUSER = 4,

	SERVER_LISTCOL_CON_TIEM = 5,
	SERVER_LISTCOL_ACTIVE = 6,
} ;

//-------------------------------------------------------------------------------------------------
CWS_Server::CWS_Server () : m_TmpStr( 512, 512 ), m_csListNODE( 4000 )
{
    m_dwRandomSEED = 0;
    m_iCurUserCNT = 0;
    m_iMaxUserCNT = 0;
	m_dwLimitUserCNT = MAX_ZONE_USER_BUFF;
	m_nUserPERCENT = 0;
	m_nBeforePERCENT = 0;

    m_dwAcceptRIGHT = 0;

    m_dwCheckSEC = classTIME::GetCurrentAbsSecond ();

	m_iUserBuffSIZE = MAX_ZONE_USER_BUFF;
	m_ppChannelUSER = new CWS_Client* [ m_iUserBuffSIZE ];
	::ZeroMemory( m_ppChannelUSER, sizeof(CWS_Client*) * MAX_ZONE_USER_BUFF );
}

CWS_Server::~CWS_Server ()
{
	SAFE_DELETE_ARRAY( m_ppChannelUSER );
}

//-------------------------------------------------------------------------------------------------
void CWS_Server::Init ()
{
	m_Socket     = INVALID_SOCKET;
	m_iSocketIDX = 0;					// CUserLIST::InitData

	m_listNODE.m_VALUE = this;

	m_pSrvListITEM = NULL;

	m_btChannelNO = 0;
}
void CWS_Server::Free ()
{
	// 접속된어 있던 사용자 삭제...
	CWS_Client *pClient;
	for (short nI=0; nI<m_iUserBuffSIZE; nI++) {
		if ( g_pUserLIST && m_ppChannelUSER[ nI ] ) {
			// m_ppChannelUSER[ nI ]를 받으시 pClient로 받고 아래 작업해야한다.
			// 호출시 m_ppChannelUSER[ nI ] == NULL이 되기때문에...
			pClient = m_ppChannelUSER[ nI ];
			pClient->LockSOCKET ();
			{
				if ( 0 == g_pUserLIST->Del_ACCOUNT( pClient->Get_ACCOUNT(), BIT_LOGIN_GS, this ) )
					pClient->m_HashACCOUNT = 0;
			}
			pClient->UnlockSOCKET ();
		}
	}

	this->LockLI ();
	if ( m_pSrvListITEM ) {
		SHO_WS::ExeAPI()->DelChannelITEM( m_pSrvListITEM );
		m_pSrvListITEM = NULL;
	}
	this->UnlockLI ();

	_ASSERT( 0 == this->m_iSocketIDX );
	this->Clear_LIST ();
}

#define	DIFF_USER_PERCENT	2	// 2%이상 차이나면 로그인 서버에 전송
void CWS_Server::AddChannelCLIENT( DWORD dwGSClientIDX, CWS_Client *pClient )	
{	
	m_iCurUserCNT ++;
	m_nUserPERCENT = (int)( m_iCurUserCNT * 100.f / (1+m_dwLimitUserCNT) );
	if ( abs( m_nBeforePERCENT-m_nUserPERCENT ) >= DIFF_USER_PERCENT ) {
		m_nBeforePERCENT = m_nUserPERCENT;
		g_pSockLSV->Send_wls_CHANNEL_LIST();
	}

	if ( m_iCurUserCNT > m_iMaxUserCNT ) {
		m_iMaxUserCNT = m_iCurUserCNT;
		this->LockLI ();
		if ( m_pSrvListITEM )
			SHO_WS::ExeAPI()->SetListItemINT( m_pSrvListITEM, SERVER_LISTCOL_MAXUSER, m_iCurUserCNT);
		this->UnlockLI ();
	}

	this->LockLI ();
	if ( m_pSrvListITEM )
		SHO_WS::ExeAPI()->SetListItemINT( m_pSrvListITEM, SERVER_LISTCOL_USERCNT, m_iCurUserCNT);
	this->UnlockLI ();

	pClient->Set_GSID( dwGSClientIDX );
	m_ppChannelUSER[ dwGSClientIDX ] = pClient;	

	// 존서버에 추가 됐으므로.....
	g_pThreadGUILD->Add_ClanCMD( GCMD_LOGIN, pClient->m_iSocketIDX, NULL, pClient->Get_NAME() );
}
void CWS_Server::SubChannelCLIENT( CWS_Client *pClient ) 
{
	if ( pClient->Get_GSID() ) {
		m_iCurUserCNT --;
		m_nUserPERCENT = (int)( m_iCurUserCNT * 100.f / (1+m_dwLimitUserCNT) );
		if ( abs( m_nBeforePERCENT-m_nUserPERCENT ) >= DIFF_USER_PERCENT ) {
			m_nBeforePERCENT = m_nUserPERCENT;
			g_pSockLSV->Send_wls_CHANNEL_LIST();
		}

		m_ppChannelUSER[ pClient->Get_GSID() ] = NULL;	
		this->LockLI ();
		if ( m_pSrvListITEM )
			SHO_WS::ExeAPI()->SetListItemINT( m_pSrvListITEM, SERVER_LISTCOL_USERCNT, m_iCurUserCNT);
		this->UnlockLI ();
		pClient->Set_GSID( 0 );

		if ( pClient->GetClanID() ) {
			g_pThreadGUILD->Add_ClanCMD( GCMD_LOGOUT, pClient->GetClanID(), NULL, pClient->Get_NAME() );
		}
	}
}

//-------------------------------------------------------------------------------------------------
bool CWS_Server::Send_str_PACKET (WORD wType, char *szString)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) {
        g_LOG.CS_ODS (0xffff, "CWS_Server(%s:%s)::Send_str_PACKET(), Packet alloc falied... \n", this->m_ServerNAME.Get(), this->m_ServerIP.Get() );
		return false;
    }

	pCPacket->m_HEADER.m_wType = wType;
	pCPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );
    if ( szString )
        pCPacket->AppendString( szString );

	this->Send_Start( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

    return true;
}

//-------------------------------------------------------------------------------------------------
bool CWS_Server::Send_wsv_CHEAT_REQ( t_PACKET *pPacket, DWORD dwReqGSID, DWORD dwReplyGSID, char *szCode )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = WSV_CHEAT_REQ;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_CHEAT );

	pCPacket->m_wsv_CHEAT_REQ.m_dwReqUSER	= dwReqGSID;
	pCPacket->m_wsv_CHEAT_REQ.m_dwReplyUSER	= dwReplyGSID;

	pCPacket->m_wsv_CHEAT_REQ.m_nZoneNO	= pPacket->m_gsv_CHEAT_REQ.m_nZoneNO;
	pCPacket->m_wsv_CHEAT_REQ.m_PosCUR	= pPacket->m_gsv_CHEAT_REQ.m_PosCUR;

	pCPacket->AppendString( szCode );// pPacket->m_gsv_CHEAT_REQ.m_szCheatCODE );
	this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}


//-------------------------------------------------------------------------------------------------
bool CWS_Server::Recv_zws_SERVER_INFO (t_PACKET *pPacket)
{
	// !!! pPacket->m_zws_SERVER_INFO.m_nActiveZONE[0] != 0 !!!

    short nOffset = sizeof( zws_SERVER_INFO );
    char *szServerName, *szServerIP;

    szServerName = Packet_GetStringPtr (pPacket, nOffset);
    szServerIP   = Packet_GetStringPtr (pPacket, nOffset);

    this->m_ServerNAME.Set( szServerName );
    if ( *szServerIP )
        this->m_ServerIP.Set( szServerIP );
    else
        this->m_ServerIP.Set( this->m_IP.Get() );

    this->m_wListenPORT = pPacket->m_zws_SERVER_INFO.m_wPortNO;
    this->m_dwRandomSEED= pPacket->m_zws_SERVER_INFO.m_dwSeed;

	this->m_bActive		= false;
	this->m_btChannelNO = pPacket->m_zws_SERVER_INFO.m_Channel.m_btChannelNO;
	this->m_btLowAGE	= pPacket->m_zws_SERVER_INFO.m_Channel.m_btLowAGE;
	this->m_btHighAGE	= pPacket->m_zws_SERVER_INFO.m_Channel.m_btHighAGE;

	if ( !g_pListSERVER->Set_ChannelSERVER( this->m_btChannelNO, this ) ) {
		g_LOG.CS_ODS (0xffff, "Invalid Channel Index %d :: %s / %s / %d\n", this->m_btChannelNO, szServerName, m_ServerIP.Get(), this->m_wListenPORT );
		this->m_btChannelNO = 0;
		return false;
	}

	g_LOG.CS_ODS (0xffff, "Start Channel SERVER :: %s / %s / %d\n", szServerName, m_ServerIP.Get(), this->m_wListenPORT );

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = WSV_SET_WORLD_VAR;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_SET_WORLD_VAR ) + sizeof( tagWorldVAR );

	pCPacket->m_srv_SET_WORLD_VAR.m_nVarIDX = -1;
	::CopyMemory( pCPacket->m_srv_SET_WORLD_VAR.m_nValue, g_ZoneLIST.m_pVAR, sizeof( tagWorldVAR ) );

	this->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	this->LockLI ();
	this->m_pSrvListITEM = SHO_WS::ExeAPI()->AddChannelITEM( this, m_btChannelNO, szServerName, m_ServerIP.Get(), m_wListenPORT );
	this->UnlockLI ();

    return true;
}


//-------------------------------------------------------------------------------------------------
bool CWS_Server::Send_wls_CONFIRM_ACCOUNT_REPLY (BYTE btResult, DWORD dwWSID, DWORD dwGSID, CWS_Client *pCLIENT)// DWORD dwLoginTIME, DWORD dwRight, char *szAccount, char *szChar, bool bFirstZONE, DWORD dwLSID )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) {
        g_LOG.CS_ODS (0xffff, "CWS_Server(%s:%s)::Send_lsv_ADD_ACCOUNT_REPLY(), Packet alloc falied... \n", this->m_ServerNAME.Get(), this->m_ServerIP.Get() );
		return false;
    }

	pCPacket->m_HEADER.m_wType = WLS_CONFIRM_ACCOUNT_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( wls_CONFIRM_ACCOUNT_REPLY );
	pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_btResult	= btResult;

    pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwWSID  = dwWSID;
    pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwGSID  = dwGSID;
	if ( pCLIENT ) {
		pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwLSID		= pCLIENT->Get_LSID();
		pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwLoginTIME = pCLIENT->Get_LoginTIME();
		pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwRIGHT		= pCLIENT->m_dwRIGHT;
		pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwPayFLAG	= pCLIENT->m_dwPayFLAG;

        pCPacket->AppendString( pCLIENT->Get_ACCOUNT() );
		pCPacket->AppendString( pCLIENT->Get_NAME() );
	}

	this->Send_Start( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

    return true;
}


//-------------------------------------------------------------------------------------------------
bool CWS_Server::Recv_zws_CONFIRM_ACCOUNT_REQ( t_PACKET *pPacket )
{
	CWS_Client *pCLIENT = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID );
	if ( NULL == pCLIENT || NULL == pCLIENT->m_pAccount || 0 == pCLIENT->m_HashACCOUNT ) {
        // 못찾았다...
        return this->Send_wls_CONFIRM_ACCOUNT_REPLY( RESULT_CONFIRM_ACCOUNT_FAILED,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID, 0 );
    }

    for (short nI=0; nI<8; nI++)
        if ( pCLIENT->m_dwMD5Password[ nI ] != pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwMD5Password[ nI ] ) {
            // 비밀번호 오류 !!! 이런 경우가 ... 해킹 ???
			pCLIENT->CloseSocket ();
			g_LOG.CS_ODS(0xffff, "??? Mismatch LS<->WS password, account: %s, char: %s \n", pCLIENT->Get_ACCOUNT(), pCLIENT->Get_NAME() );

            return this->Send_wls_CONFIRM_ACCOUNT_REPLY( RESULT_CONFIRM_ACCOUNT_INVALID_PASSWORD,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID, 0 );
        }

	pCLIENT->m_pAccount->Add_GSBit( pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID );
	this->AddChannelCLIENT( pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID, pCLIENT );	// Recv_zws_CONFIRM_ACCOUNT_REQ

	// "파티 짱이면 존 서버에 현재 파티의 경치,렙을 같이 전송 시킨다"
    return this->Send_wls_CONFIRM_ACCOUNT_REPLY( RESULT_CONFIRM_ACCOUNT_OK,
                    pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
                    pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID,
					pCLIENT );
}

//-------------------------------------------------------------------------------------------------
bool CWS_Server::Recv_zws_SUB_ACCOUNT( t_PACKET *pPacket )
{
	short nOffset   = sizeof( zws_SUB_ACCOUNT );
	char *szAccount = Packet_GetStringPtr( pPacket, nOffset );

	g_pUserLIST->Del_ACCOUNT( szAccount, BIT_LOGIN_GS );

	return true;
}

//-------------------------------------------------------------------------------------------------
/*
bool CWS_Server::Recv_gsv_WARP_USER( t_PACKET *pPacket )
{
	// 서버에서 이 패킷이 오면 클라이언트의 접속은 100% 끊긴다.

	CWS_Client *pCLIENT = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_gsv_WARP_USER.m_dwWSID );
	if ( pCLIENT ) {
		// pCLIENT->m_PosSTART = pPacket->m_gsv_WARP_USER.m_PosWARP;
		if ( pPacket->m_gsv_WARP_USER.m_dwIngStatusFLAG ) {
			::CopyMemory( &pCLIENT->m_IngSTATUS, pPacket->m_gsv_WARP_USER.m_IngSTATUS, sizeof(CIngSTATUS) );
		} else
			pCLIENT->m_IngSTATUS.ClearFLAG( 0xffffffff );

		pCLIENT->m_btRunMODE  = pPacket->m_gsv_WARP_USER.m_btRunMODE;
		pCLIENT->m_btRideMODE = pPacket->m_gsv_WARP_USER.m_btRideMODE;	// 04.04.21

		pCLIENT->Send_wsv_MOVE_SERVER( pPacket->m_gsv_WARP_USER.m_nZoneNO );

		assert( pCLIENT->Get_GSID() && "Recv_gsv_WARP_USER" );

		// 현재존에서 삭제... 대기존 ㄴpPacket->m_gsv_WARP_USER.m_nZoneNO
		this->SubZoneCLIENT( pCLIENT );		// Recv_gsv_WARP_USER
		// 소속된 파티가 있다.
		//if ( pCLIENT->Get_PARTY() ) {
		//	// 파티 인원수 감소.
		//	this->SubPartyUserCNT( pCLIENT->Get_PARTY()->m_wPartyWSID );
		//}
	}

	return true;
}
*/

//-------------------------------------------------------------------------------------------------
bool CWS_Server::Recv_gsv_CHANGE_CHAR( t_PACKET *pPacket )
{
	CWS_Client *pUSER;
	pUSER = g_pUserLIST->Find_ACCOUNT( pPacket->m_gsv_CHANGE_CHAR.m_szAccount );
	if ( pUSER ) {
		this->SubChannelCLIENT( pUSER );
		pUSER->Send_wsv_CHAR_CHANGE ();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CWS_Server::Recv_gsv_SET_WORLD_VAR( t_PACKET *pPacket )
{
	g_ZoneLIST.Set_WorldVAR( pPacket->m_srv_SET_WORLD_VAR.m_nVarIDX, pPacket->m_srv_SET_WORLD_VAR.m_nValue[0] );
	return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CWS_Server::Recv_gsv_CHEAT_REQ( t_PACKET *pPacket )
{
	short nOffset=sizeof( srv_CHEAT );
	char *szCode = ::Packet_GetStringPtr( pPacket, nOffset );

	char *pToken, *pArg1, *pArg2, *pArg3;
	char *pDelimiters = " ";

	pToken = m_TmpStr.GetTokenFirst (szCode, pDelimiters);
	pArg1  = m_TmpStr.GetTokenNext  (pDelimiters);
	if ( NULL == pToken )
		return true;

	if ( pArg1 ) {
		if ( !strcmpi( pToken, "/w" ) ) {
			// 서버에서 사용자에게 귓속말 전송...
			CWS_Client *pORDER = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER );
			if ( pORDER ) {
				pORDER->Send_gsv_WHISPER( "]", &szCode[3] );
			}
		} else
		if ( !strcmpi( pToken, "/move" ) ) {
			// cli->gsv->wsv->gsv->wsv->gsv->cli
			pArg2 = m_TmpStr.GetTokenNext (pDelimiters);	// account

			CWS_Client *pUSER=NULL;
			if ( pArg2 ) {
				pUSER = g_pUserLIST->Find_ACCOUNT( pArg2 );
			} else
			if ( pArg1 ) {
				pUSER = g_pUserLIST->Find_CHAR( pArg1 );
			}

			if ( pUSER && pUSER->GetSERVER() ) {
				pUSER->GetSERVER()->Send_wsv_CHEAT_REQ( pPacket, pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER, pUSER->Get_GSID(), pToken/* szCode */ );
			}
		} else
		if ( !strcmpi( pToken, "/call" ) ) {
			// cli->gsv->wsv->gsv->cli
			pArg2 = m_TmpStr.GetTokenNext (pDelimiters);	// account

			CWS_Client *pUSER=NULL;
			if ( pArg2 ) {
				pUSER = g_pUserLIST->Find_ACCOUNT( pArg2 );
			} else
			if ( pArg1 ) {
				pUSER = g_pUserLIST->Find_CHAR( pArg1 );
			}

			if ( pUSER && pUSER->GetSERVER() ) {
				pUSER->GetSERVER()->Send_wsv_CHEAT_REQ( pPacket, pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER, pUSER->Get_GSID(), pToken/* szCode */ );
			}
		} else
		if ( !strcmpi( pToken, "/add" ) ) {
			// cli->gsv->wsv->gsv->cli
			pArg2 = m_TmpStr.GetTokenNext (pDelimiters);
			if ( pArg2 ) {
				if ( !strcmpi(pArg1, "exp") ) {
					pArg3 = m_TmpStr.GetTokenNext (pDelimiters);
					if ( pArg3 ) {
						// 경험치를 올려줄 대상이 있는가 ?
						CWS_Client *pUSER = g_pUserLIST->Find_CHAR( pArg3 );
						if ( pUSER && pUSER->GetSERVER() ) {
							pUSER->GetSERVER()->Send_wsv_CHEAT_REQ( pPacket, 0, pUSER->Get_GSID(), szCode );
						}
					}
				}
			}
		} else
		if ( !strcmpi( pToken, "/na" ) ) {
			// 존서버들에게 전체 공지
			if ( pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER ) {
				CWS_Client *pORDER = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER );
				if ( pORDER ) 
					g_pListSERVER->Send_gsv_ANNOUNCE_CHAT( &szCode[4], pORDER->Get_NAME() );
			} else {
				g_pListSERVER->Send_gsv_ANNOUNCE_CHAT( &szCode[4], NULL );
			}
		} else
		if ( !strcmpi( pToken, "/out" ) ) {
			// 강제 접속 종료...
			pArg2 = m_TmpStr.GetTokenNext (pDelimiters);	// account

			CWS_Client *pUSER=NULL;
			if ( pArg2 )	pUSER = g_pUserLIST->Find_ACCOUNT( pArg2 );
			else
			if ( pArg1 )	pUSER = g_pUserLIST->Find_CHAR( pArg1 );

			if ( pUSER ) {
				CWS_Client *pORDER = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER );
				if ( pORDER && pORDER->m_dwRIGHT >= pUSER->m_dwRIGHT )
					pUSER->Send_gsv_GM_COMMAND( pORDER->Get_NAME(), GM_CMD_LOGOUT );
			}
		} else
		if ( !strcmpi( pToken, "/shut" ) ) {
			// 말하기 금지...
			pArg2 = m_TmpStr.GetTokenNext (pDelimiters);		// block time
			pArg3 = NULL;
			WORD wBlockTIME = 1;
			if ( pArg2 ) {
				pArg3 = m_TmpStr.GetTokenNext (pDelimiters);	// account
				wBlockTIME = (WORD)atoi( pArg2 );
				if ( wBlockTIME > 20 )
					wBlockTIME = 20;
			}

			CWS_Client *pUSER=NULL;
			if ( pArg3 )	pUSER = g_pUserLIST->Find_ACCOUNT( pArg3 );
			else 
			if ( pArg1 )	pUSER = g_pUserLIST->Find_CHAR( pArg1 );

			if ( pUSER ) {
				CWS_Client *pORDER = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER );
				if ( pORDER && pORDER->m_dwRIGHT >= pUSER->m_dwRIGHT )
					pUSER->Send_gsv_GM_COMMAND( pORDER->Get_NAME(), GM_CMD_SHUT, wBlockTIME );
			}
		} else
		if ( !strcmpi( pToken, "/account" ) ) {
			// 케릭 이름으로 계정 얻기
			CWS_Client *pUSER=NULL;
			pUSER = g_pUserLIST->Find_CHAR( pArg1 );
			if ( pUSER ) {
				CWS_Client *pORDER = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER );
				if ( pORDER && pORDER->m_dwRIGHT >= pUSER->m_dwRIGHT )
					pUSER->Send_gsv_WHISPER( pUSER->Get_NAME(), pUSER->Get_ACCOUNT() );
			}
		} else 
		if ( !strcmpi( pToken, "/where" ) ) {
			// cli->gsv->wsv->cli
			pArg2 = m_TmpStr.GetTokenNext (pDelimiters);	// account

			CWS_Client *pUSER=NULL;
			if ( pArg2 ) {
				pUSER = g_pUserLIST->Find_ACCOUNT( pArg2 );
			} else
			if ( pArg1 ) {
				pUSER = g_pUserLIST->Find_CHAR( pArg1 );
			}

			if ( pUSER /* && pUSER->GetZONE() */ ) {
				// m_TmpStr.Printf ("%s:%d ", pUSER->GetZONE()->Get_NAME(), pUSER->GetZONE()->Get_ZoneNO() );

				CWS_Client *pORDER = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER );
				if ( pORDER /* && pORDER->Get_RIGHT() >= pUSER->Get_RIGHT() */ ) {
					// pORDER->Send_gsv_WHISPER( pUSER->Get_NAME(), m_TmpStr.Get() );
					pUSER->GetSERVER()->Send_wsv_CHEAT_REQ( pPacket, pORDER->Get_GSID(), pUSER->Get_GSID(), pToken/* szCode */ );
				}
			}
		} else
		if ( !strcmpi( pToken, "/get" ) ) {
			// /get hp xxx  :: cli->gsv->wsv->gsv->wsv->cli
			pArg2 = m_TmpStr.GetTokenNext (pDelimiters);	// char
			if ( pArg2 ) {
				CWS_Client *pUSER  = g_pUserLIST->Find_CHAR( pArg2 );
				if ( pUSER ) {
					CWS_Client *pORDER = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER );
					if ( pORDER && pORDER->Get_RIGHT() >= pUSER->Get_RIGHT() ) {
						pUSER->GetSERVER()->Send_wsv_CHEAT_REQ( pPacket, pORDER->Get_GSID(), pUSER->Get_GSID(), szCode );
					}
				}
			}
		}
	} else
	if ( !strcmpi( pToken, "/call_r" ) ) {
		// /move에 대한 GSV의 응답 :: 요청가 있는 서버로 ..
		CWS_Client *pUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_gsv_CHEAT_REQ.m_dwReqUSER );
		if ( pUSER ) {
			pUSER->GetSERVER()->Send_wsv_CHEAT_REQ( pPacket, 0, pUSER->Get_GSID(), "/call" );
		}
	}


	return true;
}


//-------------------------------------------------------------------------------------------------
bool CWS_Server::Recv_srv_ACTIVE_MODE( t_PACKET* pPacket )
{
	g_pListSERVER->Set_ChannelACTIVE( this->m_btChannelNO, pPacket->m_srv_ACTIVE_MODE.m_bActive );

	g_LOG.CS_ODS(0xffff, "Server %s Active: %d\n", this->m_ServerNAME.Get(), this->m_bActive);
	return true;
}
bool CWS_Server::Recv_srv_USER_LIMIT( t_PACKET *pPacket )
{
	m_dwLimitUserCNT = pPacket->m_srv_USER_LIMIT.m_dwUserLIMIT;
	if ( m_dwLimitUserCNT < 128 )
		m_dwLimitUserCNT = 128;
	else
	if ( m_dwLimitUserCNT >= MAX_ZONE_USER_BUFF ) 
		m_dwLimitUserCNT = MAX_ZONE_USER_BUFF;

	g_LOG.CS_ODS(0xffff, "Server %s user limit count : %d\n", this->m_ServerNAME.Get(), m_dwLimitUserCNT);
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CWS_Server::Recv_zws_ACCOUNT_LIST( t_PACKET* pPacket )
{
/*
	short nI, nOffset=sizeof( zws_ACCOUNT_LIST );
	tag_ZWS_ACCOUNT *pInfo;
	char *szAccount, *szCharName;

	for (nI=0; nI<pPacket->m_zws_ACCOUNT_LIST.m_nAccountCNT; nI++) {
		pInfo = (tag_ZWS_ACCOUNT*)Packet_GetDataPtr( pPacket, nOffset, sizeof(tag_ZWS_ACCOUNT) );
		szAccount  = Packet_GetStringPtr( pPacket, nOffset );
		szCharName = Packet_GetStringPtr( pPacket, nOffset );

		xxx
	}
*/
	return true;
}


//-------------------------------------------------------------------------------------------------
bool CWS_Server::Recv_zws_CREATE_CLAN( t_PACKET *pPacket )
{
	CWS_Client *pCLIENT = (CWS_Client*)g_pUserLIST->GetSOCKET( pPacket->m_zws_CREATE_CLAN.m_dwGSID );
	if ( pCLIENT && pCLIENT->m_HashCHAR == pPacket->m_zws_CREATE_CLAN.m_HashCHAR ) {
		g_pThreadGUILD->Add_ClanCMD( GCMD_LOGIN, pCLIENT->m_iSocketIDX, NULL, pCLIENT->Get_NAME() );
	}
	return true;
}
bool CWS_Server::Recv_gsv_ADJ_CLAN_VAR( t_PACKET *pPacket )
{
	g_pThreadGUILD->Add_ClanCMD( GCMD_ADJ_VAR, 0, pPacket, "null" );
	return true;
}

//-------------------------------------------------------------------------------------------------
// 존 서버로 부터 받은 패킷...
bool CWS_Server::HandlePACKET (t_PACKETHEADER *pPacket )
{
//    LogString (LOG_DEBUG, "        >> %d CWS_Server::HandlePACKET:: Type: 0x%x, Length: %d\n", this->m_iSocketIDX, pPacket->m_wType, pPacket->m_nSize);

    switch ( pPacket->m_wType ) {
		case ZWS_SERVER_INFO :		// 존서버 정보.
			return Recv_zws_SERVER_INFO( (t_PACKET*)pPacket );

		case ZWS_CONFIRM_ACCOUNT_REQ :
			return Recv_zws_CONFIRM_ACCOUNT_REQ( (t_PACKET*)pPacket );

		case ZWS_SUB_ACCOUNT :		// 존서버에서 끊겼다.
			return Recv_zws_SUB_ACCOUNT( (t_PACKET*)pPacket );	

		case ZWS_ACCOUNT_LIST :
			return Recv_zws_ACCOUNT_LIST( (t_PACKET*)pPacket );

	/*
		case GSV_WARP_USER :
			return Recv_gsv_WARP_USER( (t_PACKET*)pPacket );
	*/

		case GSV_CHEAT_REQ :
			return Recv_gsv_CHEAT_REQ( (t_PACKET*)pPacket );

		case GSV_CHANGE_CHAR :
			return Recv_gsv_CHANGE_CHAR( (t_PACKET*)pPacket );

		case ZWS_CREATE_CLAN :
			return Recv_zws_CREATE_CLAN( (t_PACKET*)pPacket );
		case GSV_ADJ_CLAN_VAR :
			return Recv_gsv_ADJ_CLAN_VAR( (t_PACKET*)pPacket );

		case GSV_SET_WORLD_VAR :
			return Recv_gsv_SET_WORLD_VAR( (t_PACKET*)pPacket );

		case SRV_ACTIVE_MODE :
			return Recv_srv_ACTIVE_MODE( (t_PACKET*)pPacket );
		case SRV_USER_LIMIT :
			return Recv_srv_USER_LIMIT( (t_PACKET*)pPacket );

        default :
	            g_LOG.CS_ODS (0xffff, "** ERROR:: CLS_Server: Invalid packet type: 0x%x, Size: %d ", pPacket->m_wType, pPacket->m_nSize);
    }

	return true;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CWS_ListSERVER::CWS_ListSERVER( UINT uiInitDataCNT, UINT uiIncDataCNT )
							: IOCPSocketSERVER( "CWS_ServerSOCKET", 1, 1, false ),
								CDataPOOL< CWS_Server > ("CServerPOOL", uiInitDataCNT, uiIncDataCNT ),
								m_csLIST( 4000 )
{
	m_nChannelCNT = 0;
	::ZeroMemory( m_ppChannelSERVER, sizeof(CWS_Server*) * MAX_CHANNEL_SERVER );
}
CWS_ListSERVER::~CWS_ListSERVER ()
{
	;
}

void CWS_ListSERVER::Send_ToWORLD( classPACKET *pCPacket )
{
	this->LockLIST ();

	for (short nI=0; nI<MAX_CHANNEL_SERVER; nI++) {
		if ( m_ppChannelSERVER[nI] )
			m_ppChannelSERVER[nI]->SendPacket( pCPacket );
	}
	//CDLList< CWS_Server* >::tagNODE *pServerNODE;
	//pServerNODE = this->m_listSERVER.GetHeadNode();
	//while( pServerNODE ) {
	//	pServerNODE->m_VALUE->SendPacket( pCPacket );

	//	pServerNODE = pServerNODE->GetNext ();
	//}

	this->UnlockLIST ();
}


//-------------------------------------------------------------------------------------------------
void CWS_ListSERVER::Set_ChannelACTIVE( int iChannelNo, bool bActive, bool bToggle )
{
	if ( iChannelNo <= 0 || iChannelNo >= MAX_CHANNEL_SERVER )
		return;

	bool bMode;
	this->LockLIST ();
		if ( NULL == m_ppChannelSERVER[ iChannelNo ] ) {
			this->UnlockLIST ();
			return;
		}

		if ( bToggle ) {
			m_ppChannelSERVER[ iChannelNo ]->m_bActive = !m_ppChannelSERVER[ iChannelNo ]->m_bActive;
		} else
			m_ppChannelSERVER[ iChannelNo ]->m_bActive = bActive;

		bMode = m_ppChannelSERVER[ iChannelNo ]->m_bActive;

		m_ppChannelSERVER[ iChannelNo ]->LockLI ();
		void *pListItem = m_ppChannelSERVER[ iChannelNo ]->m_pSrvListITEM;
		if ( pListItem )
			SHO_WS::ExeAPI()->SetListItemINT( pListItem, SERVER_LISTCOL_ACTIVE, bMode);
		m_ppChannelSERVER[ iChannelNo ]->UnlockLI ();
	this->UnlockLIST ();

	// 로그인 서버에 채널리스트 전송.
	g_pSockLSV->Send_wls_CHANNEL_LIST ();
}

//-------------------------------------------------------------------------------------------------
bool CWS_ListSERVER::Set_ChannelSERVER( BYTE btChannelNO, CWS_Server *pServer )
{
	if ( btChannelNO <= 0 || btChannelNO >= MAX_CHANNEL_SERVER )
		return false;

	this->LockLIST ();
		m_ppChannelSERVER[ btChannelNO ] = pServer;
		if ( pServer )
			m_nChannelCNT ++;
		else
			m_nChannelCNT --;
	this->UnlockLIST ();

	// 로그인 서버에 채널리스트 전송.
	g_pSockLSV->Send_wls_CHANNEL_LIST ();

	return true;
}

void CWS_ListSERVER::Del_ChannelSERVER (int iChannelNo)
{
	if ( iChannelNo <= 0 || iChannelNo >= MAX_CHANNEL_SERVER )
		return;
	this->LockLIST ();
		if ( m_ppChannelSERVER[ iChannelNo ] )
			m_ppChannelSERVER[ iChannelNo ]->CloseSocket();
	this->UnlockLIST ();
}

//-------------------------------------------------------------------------------------------------
void CWS_ListSERVER::Send_gsv_ANNOUNCE_CHAT( char *szMessage, char *szAccount )
{
	if ( NULL == szMessage )
		return;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = LSV_ANNOUNCE_CHAT;
	pCPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );
	pCPacket->AppendString( szMessage );

	if ( szAccount ) {
		g_LOG.CS_ODS( 0xffff, "[Announce:%s] %s\n", szAccount, szMessage );
		pCPacket->AppendString( szAccount );
	} else {
		g_LOG.CS_ODS( 0xffff, "[Announce] %s\n", szMessage );
	}
	this->Send_ToWORLD( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void CWS_ListSERVER::Make_wls_CHANNEL_LIST( classPACKET *pCPacket )
{
//	CDLList< CWS_Server* >::tagNODE *pNode;

	pCPacket->m_HEADER.m_wType = WLS_CHANNEL_LIST;
	pCPacket->m_HEADER.m_nSize = sizeof( wls_CHANNEL_LIST );

	pCPacket->m_wls_CHANNEL_LIST.m_btChannelCNT = 0;

	tagCHANNEL_SRV	sChannel;
	this->LockLIST ();
	{
		for (short nI=1; nI<MAX_CHANNEL_SERVER;nI++) {
			if ( m_ppChannelSERVER[nI] && m_ppChannelSERVER[nI]->m_bActive ) {
				sChannel.m_btChannelNO	= m_ppChannelSERVER[nI]->m_btChannelNO;
				sChannel.m_btLowAGE		= m_ppChannelSERVER[nI]->m_btLowAGE;
				sChannel.m_btHighAGE	= m_ppChannelSERVER[nI]->m_btHighAGE;
				sChannel.m_nUserPERCENT	= m_ppChannelSERVER[nI]->m_nUserPERCENT;

				pCPacket->AppendData( &sChannel, sizeof( tagCHANNEL_SRV ) );
				pCPacket->AppendString( m_ppChannelSERVER[nI]->m_ServerNAME.Get() );
	
				pCPacket->m_wls_CHANNEL_LIST.m_btChannelCNT ++;
			}
		}
	//	pNode = m_listSERVER.GetHeadNode ();
	//	while( pNode ) {
	//		if ( pNode->m_VALUE->m_bActive ) {
	//			sChannel.m_btChannelNO	= pNode->m_VALUE->m_btChannelNO;
	//			sChannel.m_btLowAGE		= pNode->m_VALUE->m_btLowAGE;
	//			sChannel.m_btHighAGE	= pNode->m_VALUE->m_btHighAGE;
	//			pCPacket->AppendData( &sChannel, sizeof( tagCHANNEL_SRV ) );
	//			pCPacket->AppendString( pNode->m_VALUE->m_ServerNAME.Get() );
	//
	//			pCPacket->m_wls_CHANNEL_LIST.m_btChannelCNT ++;
	//		}
	//		pNode = pNode->GetNext();
	//	}
	}
	this->UnlockLIST ();
}

//-------------------------------------------------------------------------------------------------
void CWS_ListSERVER::Make_srv_SERVER_LIST_REPLY( classPACKET *pCPacket )
{
	pCPacket->m_HEADER.m_wType = WLS_SERVER_LIST_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_SERVER_LIST_REPLY );

	pCPacket->m_srv_SERVER_LIST_REPLY.m_dwBuildNO   = GetServerBuildNO();
	pCPacket->m_srv_SERVER_LIST_REPLY.m_dwStartTIME = GetServerStartTIME();

	this->LockLIST ();
	{
		m_nChannelCNT = 0;
		for (short nI=1; nI<MAX_CHANNEL_SERVER;nI++) {
			if ( m_ppChannelSERVER[nI] ) {
				pCPacket->AppendData  (&m_ppChannelSERVER[nI]->m_wListenPORT,	sizeof(WORD) );
				pCPacket->AppendData  (&m_ppChannelSERVER[nI]->m_dwRandomSEED,	sizeof(DWORD) );

				pCPacket->AppendString( m_ppChannelSERVER[nI]->m_ServerNAME.Get() );
				pCPacket->AppendString( m_ppChannelSERVER[nI]->m_ServerIP.Get() );
				m_nChannelCNT ++;
			}
		}
	}
	this->UnlockLIST ();

	pCPacket->m_srv_SERVER_LIST_REPLY.m_nServerCNT  = m_nChannelCNT;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CWS_ListSERVER::Send_zws_SET_USER_CLAN( CWS_Client *pClient )
{
	CWS_Server *pChannel = this->Get_ChannelSERVER( pClient->Get_ChannelNO() );
	if ( pChannel ) {
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return false;

		pCPacket->m_HEADER.m_wType = ZWS_SET_USER_CLAN;
		pCPacket->m_HEADER.m_nSize = sizeof( zws_SET_USER_CLAN );

		pCPacket->m_zws_SET_USER_CLAN.m_dwGSID   = pClient->Get_GSID();
		pCPacket->m_zws_SET_USER_CLAN.m_HashCHAR = pClient->m_HashCHAR;
		pCPacket->m_zws_SET_USER_CLAN.m_ClanINFO = pClient->m_CLAN;
		pCPacket->AppendString( pClient->GetClanNAME() );

		pChannel->Send_Start( pCPacket );

		Packet_ReleaseNUnlock( pCPacket );
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CWS_ListSERVER::Send_zws_DEL_USER_CLAN( CWS_Client *pClient )
{
	CWS_Server *pChannel = this->Get_ChannelSERVER( pClient->Get_ChannelNO() );
	if ( pChannel ) {
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return false;

		pCPacket->m_HEADER.m_wType = ZWS_DEL_USER_CLAN;
		pCPacket->m_HEADER.m_nSize = sizeof( zws_DEL_USER_CLAN );

		pCPacket->m_zws_DEL_USER_CLAN.m_dwGSID   = pClient->Get_GSID();
		pCPacket->m_zws_DEL_USER_CLAN.m_HashCHAR = pClient->m_HashCHAR;

		pChannel->Send_Start( pCPacket );

		Packet_ReleaseNUnlock( pCPacket );
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

