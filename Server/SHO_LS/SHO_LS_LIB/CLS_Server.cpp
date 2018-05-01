
#include "stdAFX.h"

#include "CLS_Server.h"
#include "CLS_Account.h"
#include "CLS_SqlTHREAD.h"
/*
#include "LS_Main.h"

extern void WM_CloseSOCKET (TCustomWinSocket *pSocket, char *szMSG);
extern void WM_ClientSendPacket ();
extern void WM_ServerSendPacket ();
*/

#define	LIST_COL_CHANNELS	2
#define	LIST_COL_CUR_USERS	3
#define	LIST_COL_MAX_USERS	4

//-------------------------------------------------------------------------------------------------
CLS_Server::CLS_Server () : m_csLIST( 4000 )
{
    m_dwRandomSEED = 0;
    m_iCurUserCNT = 0;
    m_iMaxUserCNT = 0;
//    m_pListITEM = NULL;
    m_dwAcceptRIGHT = 0;

    m_dwCheckSEC = classTIME::GetCurrentAbsSecond ();

	for (int iC=0; iC<MAX_CHANNEL_SERVER; iC++)
		m_CHANNEL[ iC ].Init ();
}
CLS_Server::~CLS_Server ()
{
//    this->_Free ();
}

void CLS_Server::Inc_UserCNT ()
{
    m_iCurUserCNT ++;
    if ( m_iCurUserCNT > m_iMaxUserCNT ) {
        m_iMaxUserCNT = m_iCurUserCNT;
		this->LockSOCKET ();
		if ( this->m_pSrvListITEM ) {
			SHO_LS::GetInstance()->ExeAPI()->SetListItemINT( this->m_pSrvListITEM, LIST_COL_MAX_USERS, m_iMaxUserCNT );
		}
		this->UnlockSOCKET ();
    }

	this->LockSOCKET ();
	if ( this->m_pSrvListITEM ) {
		SHO_LS::GetInstance()->ExeAPI()->SetListItemINT( this->m_pSrvListITEM, LIST_COL_CUR_USERS, m_iCurUserCNT );
	}
	this->UnlockSOCKET ();

	g_pListSERVER->m_iCurUserCNT ++;
	if ( g_pListSERVER->m_iCurUserCNT > g_pListSERVER->m_iMaxUserCNT ) {
		// 최대 동접 갱신
		g_pListSERVER->m_iMaxUserCNT = g_pListSERVER->m_iCurUserCNT;
	}

	sprintf( g_pListSERVER->m_szStatusSTR, "U:%d/%d, W:%d, J:%d/%d\0", g_pListSERVER->m_iCurUserCNT, g_pListSERVER->m_iMaxUserCNT, g_pListWAIT->GetLSIDCount(), g_pListJOIN->GetAcntCount(), g_pListJOIN->GetLSIDCount() );
	SHO_LS::GetInstance()->ExeAPI()->SetStatusBarTEXT( 0, g_pListSERVER->m_szStatusSTR );
}

void CLS_Server::Dec_UserCNT ()
{
	g_pListSERVER->m_iCurUserCNT --;
	sprintf( g_pListSERVER->m_szStatusSTR, "U:%d/%d, W:%d, J:%d/%d\0", g_pListSERVER->m_iCurUserCNT, g_pListSERVER->m_iMaxUserCNT, g_pListWAIT->GetLSIDCount(), g_pListJOIN->GetAcntCount(), g_pListJOIN->GetLSIDCount() );
	SHO_LS::GetInstance()->ExeAPI()->SetStatusBarTEXT( 0, g_pListSERVER->m_szStatusSTR );

	m_iCurUserCNT --;
	this->LockSOCKET ();
	if ( this->m_pSrvListITEM ) {
		SHO_LS::GetInstance()->ExeAPI()->SetListItemINT( this->m_pSrvListITEM, LIST_COL_CUR_USERS, m_iCurUserCNT );
	}
	this->UnlockSOCKET ();
}


void CLS_Server::ClearChannels ()
{
	for (int iC=0; iC<MAX_CHANNEL_SERVER; iC++)
		m_CHANNEL[ iC ].Init ();

	CDLList< tagChannelSRV* >::tagNODE *pNode;

	this->m_csLIST.Lock ();
		pNode = this->m_ListCHANNEL.GetHeadNode();
		while( pNode ) {
			this->m_ListCHANNEL.DeleteNFree( pNode );
			pNode = this->m_ListCHANNEL.GetHeadNode();
		}
	this->m_csLIST.Unlock ();
}
//-------------------------------------------------------------------------------------------------
/*
bool  CLS_Server::_Init (TCustomWinSocket *pSocket, DWORD dwSocketID)
{
    SYSTEMTIME st;
    ::GetLocalTime( &st );
    String stTime;
    stTime.sprintf ("%d-%d-%d %d:%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute );

    m_pListITEM = BCBUtil::AddListITEM (FormMAIN->ListViewSERVER, "unknown",
                    pSocket->RemoteAddress,
                    "unknown",
                    "unknown",
                    "unknown",
                    stTime,         //
                    "0",            // 접속 권한
                    NULL);
    m_pListITEM->Data = (void*) this;

    return BCB_Socket::_Init( pSocket, dwSocketID );
}
*/
void  CLS_Server::Free (void)
{
	/*
    if ( m_pListITEM ) {
        BCBUtil::DelListITEM (FormMAIN->ListViewSERVER, m_pListITEM);
        m_pListITEM = NULL;
    }

    BCB_Socket::_Free ();
    */
    // 이 섭에 접속했던 모든 계정을 삭제한다...
    classDLLNODE< CLS_Account* > *pNODE;
    CLS_Account *pCAccount;

	g_pListSERVER->m_iCurUserCNT -= m_iCurUserCNT;

    pNODE = m_AccountLIST.GetHeadNode ();
    while( pNODE ) {
        pCAccount = pNODE->DATA;

        // 아래 순서 틀리면 뻑~~ !!!
        m_AccountLIST.DeleteNode( pNODE );
        g_pThreadSQL->Add_LogOutUSER( pCAccount );

        pNODE = m_AccountLIST.GetHeadNode ();
    }

	_ASSERT( 0 == this->m_iSocketIDX );
	this->Clear_LIST ();
}

//-------------------------------------------------------------------------------------------------
bool CLS_Server::Recv_zws_SERVER_INFO (t_PACKET *pPacket)
{
	/********
	/  2015 Garnet: Attempting to force value on World Server IP. 
	/               It fix WorldServer WAN IP that is wrong because of some routers.
	/******/

	bool forceWSIP(false); // will be used to know if we will use the forcedIP from the ini or not


	char *forceWorldServerIP  = "";
	char forceWorldServerIP2[255];
	GetPrivateProfileString("FormLSCFG","ForceWorldServerIP","",forceWorldServerIP2,sizeof(forceWorldServerIP2),"./SHO_LS.ini");
	if(strcmp(forceWorldServerIP2,"")!= 0) 
	{
		forceWorldServerIP = forceWorldServerIP2;
		forceWSIP = true; // set bool to true
	}




    short nOffset = sizeof( zws_SERVER_INFO );
    char *szServerName, *szServerIP;

	// !!! pPacket->m_zws_SERVER_INFO.m_nActiveZONE[0] = 0 !!!
    szServerName = Packet_GetStringPtr (pPacket, nOffset);
    szServerIP   = Packet_GetStringPtr (pPacket, nOffset);

    this->m_ServerNAME.Set( szServerName );

	/**if ( *szServerIP ) Original Code
        this->m_ServerIP.Set( szServerIP );
    else
	this->m_ServerIP.Set( this->m_IP.Get() );
	**/


		//Garnet: Now let's check if we force the IP or not		

		if (forceWSIP) // if true
		{
			g_LOG.CS_ODS( 0xffff, "Casse les burnes \n");
			m_ServerIP.Set( forceWorldServerIP ); // force the IP from the INI
		}
		else
		{
			if ( szServerIP && *szServerIP )
				this->m_ServerIP.Set( szServerIP );
			else
                this->m_ServerIP.Set( this->m_IP.Get() ); // load the normal IP received from WorldServer
		}
	//}

	//////////////////////////////////////////////////////////////////////////

    this->m_wListenPORT = pPacket->m_zws_SERVER_INFO.m_wPortNO;
    this->m_dwRandomSEED= pPacket->m_zws_SERVER_INFO.m_dwSeed;

    g_LOG.CS_ODS (0xffff, "Start World SERVER :: %s / %s / %d\n", szServerName, m_ServerIP.Get(), this->m_wListenPORT );

	DWORD dwRight = 0;	// TODO::
	this->LockSOCKET();
	//this->m_pSrvListITEM = SHO_LS::ExeAPI()->AddWorldITEM( this, szServerName, this->m_IP.Get(), this->m_wListenPORT, dwRight ); // Original Code
	this->m_pSrvListITEM = SHO_LS::ExeAPI()->AddWorldITEM( this, szServerName, this->m_ServerIP.Get(), this->m_wListenPORT, dwRight );
	this->UnlockSOCKET();
    return true;
	
}

//-------------------------------------------------------------------------------------------------
bool CLS_Server::Send_str_PACKET (WORD wType, char *szString)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) {
        g_LOG.CS_ODS (0xffff, "CLS_Server(%s:%s)::Send_str_PACKET(), Paclet allock falied... \n", this->m_ServerNAME.Get(), this->m_ServerIP.Get() );
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
void CLS_Server::Make_lsv_CHANNEL_LIST_REPLY( classPACKET *pCPacket )
{
	pCPacket->m_HEADER.m_wType = LSV_CHANNEL_LIST_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( lsv_CHANNEL_LIST_REPLY );

	this->m_csLIST.Lock ();
	{
		pCPacket->m_lsv_CHANNEL_LIST_REPLY.m_dwServerID   = this->m_iSocketIDX;
		pCPacket->m_lsv_CHANNEL_LIST_REPLY.m_btChannelCNT = this->m_ListCHANNEL.GetNodeCount();

		CDLList< tagChannelSRV* >::tagNODE *pNode;
		pNode = this->m_ListCHANNEL.GetHeadNode ();
		tagCHANNEL_SRV sChannel;
		while( pNode ) {
			sChannel.m_btChannelNO	= pNode->m_VALUE->m_btChannelNO;	
			sChannel.m_btLowAGE		= pNode->m_VALUE->m_btLowAGE;
			sChannel.m_btHighAGE	= pNode->m_VALUE->m_btHighAGE;
			sChannel.m_nUserPERCENT = pNode->m_VALUE->m_nUserPERCENT;

			pCPacket->AppendData  ( &sChannel, sizeof( tagCHANNEL_SRV ) );
			pCPacket->AppendString( pNode->m_VALUE->m_ChannelNAME.Get () );

			pNode = pNode->GetNext ();
		}
	}
	this->m_csLIST.Unlock ();
}

//-------------------------------------------------------------------------------------------------
bool CLS_Server::Recv_wls_CHANNEL_LIST (t_PACKET *pPacket)
{
	short nOffset = sizeof( wls_CHANNEL_LIST );
	tagCHANNEL_SRV *pChannel;
	char *pName;

	this->m_csLIST.Lock();
	{
		this->ClearChannels ();

		CDLList< tagChannelSRV* >::tagNODE *pNode;
		for (short nI=0; nI<pPacket->m_wls_CHANNEL_LIST.m_btChannelCNT; nI++) {
			pChannel = (tagCHANNEL_SRV*) Packet_GetDataPtr( pPacket, nOffset, sizeof(tagCHANNEL_SRV) );
			pName = Packet_GetStringPtr( pPacket, nOffset );

			if ( pChannel->m_btChannelNO && pChannel->m_btChannelNO < MAX_CHANNEL_SERVER ) {
				pNode = this->m_ListCHANNEL.AllocNAppend ();

				m_CHANNEL[ pChannel->m_btChannelNO ].m_btChannelNO = pChannel->m_btChannelNO;
				m_CHANNEL[ pChannel->m_btChannelNO ].m_btLowAGE	 = pChannel->m_btLowAGE;
				m_CHANNEL[ pChannel->m_btChannelNO ].m_btHighAGE	 = pChannel->m_btHighAGE;
				m_CHANNEL[ pChannel->m_btChannelNO ].m_nUserPERCENT= pChannel->m_nUserPERCENT;
				m_CHANNEL[ pChannel->m_btChannelNO ].m_ChannelNAME.Set( pName );

				pNode->m_VALUE = &m_CHANNEL[ pChannel->m_btChannelNO ];
			}
		}
	}
	this->m_csLIST.Unlock ();

	// 채널 갯수...
	this->LockSOCKET ();
	if ( this->m_pSrvListITEM ) {
		SHO_LS::GetInstance()->ExeAPI()->SetListItemINT( this->m_pSrvListITEM, LIST_COL_CHANNELS, pPacket->m_wls_CHANNEL_LIST.m_btChannelCNT );
	}
	this->UnlockSOCKET ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CLS_Server::Send_wls_CONFIRM_ACCOUNT_REPLY (BYTE btResult, DWORD dwLSID, DWORD dwWSID, DWORD dwLoginTIME, DWORD dwRight, DWORD dwPayFLAG, BYTE btChannelNO, char *szAccount)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) {
        g_LOG.CS_ODS (0xffff, "CLS_Server(%s:%s)::Send_lsv_ADD_ACCOUNT_REPLY(), Paclet allock falied... \n", this->m_ServerNAME.Get(), this->m_ServerIP.Get() );
		return false;
    }

	pCPacket->m_HEADER.m_wType = WLS_CONFIRM_ACCOUNT_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( wls_CONFIRM_ACCOUNT_REPLY );
	pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_btResult	= btResult;
    pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwLSID		= dwLSID;
    pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwWSID		= dwWSID;
    pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwLoginTIME = dwLoginTIME;
    pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwRIGHT		= dwRight;
	pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwPayFLAG   = dwPayFLAG;
	pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_btChannelNO = btChannelNO;
/*
	pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_bFirstZONE  = false;		// 존서버에서만 사용함...
	pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwIngStatusFLAG = 0;		// 존서버에서만 사용함...

#ifndef	__SKIP_WS_PARTY
	pCPacket->m_wls_CONFIRM_ACCOUNT_REPLY.m_wPartyWSID  = 0;			// 존서버에서만 사용함...
#endif
*/
    if ( szAccount )
        pCPacket->AppendString( szAccount );

	this->Send_Start( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

    return true;
}

//-------------------------------------------------------------------------------------------------
bool CLS_Server::Recv_gsv_CHECK_ALIVE (t_PACKET *pPacket)
{
	// LogString (LOG_DEBUG, "CLS_Server::Recv_gsv_CHECK_ALIVE[ %s:%s ] \n", m_ServerNAME.Get(), m_ServerIP.Get() );

    this->m_dwCheckSEC = classTIME::GetCurrentAbsSecond ();

    return true;
}

//-------------------------------------------------------------------------------------------------
bool CLS_Server::Recv_zws_CONFIRM_ACCOUNT_REQ (t_PACKET *pPacket)
{
    CLS_Account *pCAccount;

    pCAccount = g_pListWAIT->Remove_FromLIST( pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID );
    if ( NULL == pCAccount ) {
		LogString(0xffff, "LSID[ %d ] not found\n", pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID );
        // 못찾았다...
        return this->Send_wls_CONFIRM_ACCOUNT_REPLY( RESULT_CONFIRM_ACCOUNT_TIME_OUT,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID, 0 );
    }

    if ( pCAccount->m_dwLSID != pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID ) {
        // 이상한 현상이다...
		g_LOG.CS_ODS(0xffff, "??? Mismatch LSID  account: %s \n", pCAccount->m_Account.Get() );
		g_pListWAIT->Mem_DEL( pCAccount );

        return this->Send_wls_CONFIRM_ACCOUNT_REPLY( RESULT_CONFIRM_ACCOUNT_FAILED,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID, 0 );
    }

    for (short nI=0; nI<8; nI++)
        if ( pCAccount->m_dwMD5Password[ nI ] != pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwMD5Password[ nI ] ) {
			short nOffset = sizeof( zws_CONFIRM_ACCOUNT_REQ );
			char szOriPass[33], szRecvPass[33];

			::CopyMemory( szOriPass,	pCAccount->m_dwMD5Password,							32 );
			::CopyMemory( szRecvPass,	pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwMD5Password,	32 );
			szOriPass [ 32 ] = 0;
			szRecvPass[ 32 ] = 0;

            // 비밀번호 오류 !!! 이런 경우가 ... 해킹 ???
			g_LOG.CS_ODS(0xffff, "??? Mismatch LS<->WS password, account: %sLSID:%d, %s, %s\n", 
					pCAccount->m_Account.Get(),
					pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
					szOriPass, szRecvPass );

			g_pListWAIT->Mem_DEL( pCAccount );

            return this->Send_wls_CONFIRM_ACCOUNT_REPLY( RESULT_CONFIRM_ACCOUNT_INVALID_PASSWORD,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID, 0 );
        }

    if ( g_pListJOIN->Search_ACCOUNT( pCAccount->m_Account.Get() ) ) {
		g_LOG.CS_ODS(0xffff, "!!! Duplicated login account: %s \n", pCAccount->m_Account.Get() );
        // 이미 로그인된 아뒤다...
        // 대기 목록에서 삭제...
		g_pListWAIT->Mem_DEL( pCAccount );

        return this->Send_wls_CONFIRM_ACCOUNT_REPLY( RESULT_CONFIRM_ACCOUNT_ALREADY_LOGGEDIN,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID, 0 );
    }

    pCAccount->m_HashACCOUNT = CStr::GetHASH( pCAccount->m_Account.Get() );
    if ( !g_pListJOIN->Insert_ACCOUNT( pCAccount, this ) ) {
		g_pListWAIT->Mem_DEL( pCAccount );

        return this->Send_wls_CONFIRM_ACCOUNT_REPLY( RESULT_CONFIRM_ACCOUNT_ALREADY_LOGGEDIN,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
                        pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID, 0 );
	}

	m_AccountLIST.AppendNode( pCAccount->m_pListNODE );

//    FormMAIN->Update_AccountCNT( g_pListJOIN->GetCount() );

	this->Inc_UserCNT ();

    return this->Send_wls_CONFIRM_ACCOUNT_REPLY( RESULT_CONFIRM_ACCOUNT_OK,
                    pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID,
                    pPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID,
                    pCAccount->m_dwLoginTIME,
                    pCAccount->m_dwRIGHT,
					pCAccount->m_dwPayFLAG,
					pCAccount->m_btChannelNO,
                    pCAccount->m_Account.Get() );
}

//-------------------------------------------------------------------------------------------------
/*
    로그인 서버가 뻗어서 다시 시작될경우
    각 게임 서버에서 접속되어 있는 사용자 리스트를 보내는 패킷이다...
*/
bool CLS_Server::Recv_zws_ADD_ACCOUNT (t_PACKET *pPacket)
{
//    LogString (LOG_DEBUG, "CLS_Server::Recv_gsv_ADD_ACCOUNT:: Type: 0x%x, Length: %d\n", pPacket->m_HEADER.m_wType, pPacket->m_HEADER.m_nSize);

    short nOffset=sizeof(zws_ADD_ACCOUNT);
    char *szAccount = Packet_GetStringPtr (pPacket, nOffset);
    DWORD*pLoginTIME = (DWORD*)Packet_GetDataPtr (pPacket, nOffset, sizeof(DWORD) );

    while( szAccount && pLoginTIME ) {
        CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );

        if ( pCAccount ) {
            // 게임 서버에는 남아 있는데 벌써 접속해 버렸나 ??
            // 게임 서버에 있는 사용자를 짤라라 !!!
            if ( pCAccount->m_pWorldServer ) {
                pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
            }
            this->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
        } else {
            CLS_Account *pCAccount = g_pListJOIN->Mem_NEW();

            pCAccount->m_Account.Set( szAccount );
            pCAccount->m_dwLoginTIME = *pLoginTIME;

            if ( !g_pListJOIN->Insert_ACCOUNT( pCAccount, this ) ) {
				g_pListJOIN->Mem_DEL( pCAccount );
            } else
                m_AccountLIST.AppendNode( pCAccount->m_pListNODE );
        }

        szAccount  = Packet_GetStringPtr (pPacket, nOffset);
        pLoginTIME = (DWORD*)Packet_GetDataPtr (pPacket, nOffset, sizeof(DWORD) );
    }

    return true;
}

bool CLS_Server::Recv_zws_SUB_ACCOUNT( t_PACKET *pPacket )
{
    short nOffset   = sizeof( zws_SUB_ACCOUNT );
    char *szAccount = Packet_GetStringPtr( pPacket, nOffset );

    CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
    if ( pCAccount && pCAccount->m_pWorldServer ) {
        pCAccount->m_pWorldServer = NULL;
		this->Dec_UserCNT();

        // 아래 순서 틀리면 뻑~~ !!!
        m_AccountLIST.DeleteNode( pCAccount->m_pListNODE );	// 뻑 :: 같은 계정에 대해 여러번 호출되는듯...
        g_pThreadSQL->Add_LogOutUSER( pCAccount );
	} else {
		LogString( 0xffff, "Recv_gsv_SUB_ACCOUNT:: Account[ %s ] not found in JOIN list\n", szAccount );
	}

    return true;
}

bool CLS_Server::Recv_srv_ACTIVE_MODE( t_PACKET *pPacket )
{
	this->m_bActive = pPacket->m_srv_ACTIVE_MODE.m_bActive;

	g_LOG.CS_ODS(0xffff, "Server %s Active: %d\n", this->m_ServerNAME.Get(), this->m_bActive);
	return true;
}

bool CLS_Server::Recv_wls_ACCOUNT_LIST( t_PACKET *pPacket )
{
	CLS_Account *pCAccount;

	short nI, nOffset=sizeof( wls_ACCOUNT_LIST );
	tag_WLS_ACCOUNT *pInfo;
	char *szAccount;

	for (nI=0; nI<pPacket->m_wls_ACCOUNT_LIST.m_nAccountCNT; nI++) {
		pInfo = (tag_WLS_ACCOUNT*)Packet_GetDataPtr( pPacket, nOffset, sizeof(tag_WLS_ACCOUNT) );
		szAccount = Packet_GetStringPtr( pPacket, nOffset );

		pCAccount = g_pListJOIN->Search_ACCOUNT( szAccount );
		if ( pCAccount ) {
			// 중복된 계정이 있네~~~ 오랜된 넘 짤라!!!
			if ( pCAccount->m_dwLSID < pInfo->m_dwLSID && pCAccount->m_pWorldServer ) {
                pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
			} else 
			if ( pCAccount->m_dwLSID != pInfo->m_dwLSID ) {
	            this->Send_str_PACKET( WLS_KICK_ACCOUNT, szAccount );
				continue;
			} else {
				// 같은넘이야 ???
				continue;
			}
		}

        pCAccount = g_pListJOIN->Mem_NEW();

		pCAccount->m_HashACCOUNT = CStr::GetHASH( szAccount );
		pCAccount->m_Account.Set( szAccount );
		pCAccount->m_dwLoginTIME = pInfo->m_dwLoginTIME;
		pCAccount->m_dwLSID = pInfo->m_dwLSID;

		if ( !g_pListJOIN->Insert_ACCOUNT( pCAccount, this ) ) {
			g_pListJOIN->Mem_DEL( pCAccount );
			// false면 메모리가 없다는것...
			break;
		}
		m_AccountLIST.AppendNode( pCAccount->m_pListNODE );
		this->Inc_UserCNT ();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CLS_Server::HandlePACKET (t_PACKETHEADER *pPacket )
{
/*
    패킷 디코딩...
    패킷 일련번호, 사이즈, CRC, CheckSUM등으로 적합패킷인지 판단.
*/
//    LogString (LOG_DEBUG, "        >> %d CLS_Server::HandlePACKET:: Type: 0x%x, Length: %d\n", this->m_iSocketIDX, pPacket->m_wType, pPacket->m_nSize);

    switch ( pPacket->m_wType ) {
		case SRV_ACTIVE_MODE :
			return Recv_srv_ACTIVE_MODE( (t_PACKET*)pPacket );

        case GSV_CHECK_ALIVE :
            return Recv_gsv_CHECK_ALIVE( (t_PACKET*)pPacket );

        case ZWS_SERVER_INFO :
            return Recv_zws_SERVER_INFO( (t_PACKET*)pPacket );

        case ZWS_CONFIRM_ACCOUNT_REQ :
            return Recv_zws_CONFIRM_ACCOUNT_REQ( (t_PACKET*)pPacket );

        case ZWS_ADD_ACCOUNT :
            return Recv_zws_ADD_ACCOUNT( (t_PACKET*)pPacket );

        case ZWS_SUB_ACCOUNT :
            return Recv_zws_SUB_ACCOUNT( (t_PACKET*)pPacket );

		case WLS_CHANNEL_LIST :
			return Recv_wls_CHANNEL_LIST( (t_PACKET*)pPacket );

		case WLS_ACCOUNT_LIST :
			return Recv_wls_ACCOUNT_LIST( (t_PACKET*)pPacket );

        default :
            g_LOG.CS_ODS (0xffff, "** ERROR:: CLS_Server: Invalid packet type: 0x%x, Size: %d\n", pPacket->m_wType, pPacket->m_nSize);
    }

	return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CLS_ListSERVER::Add_ServerList2Packet( classPACKET *pCPacket, DWORD dwRight )
{
    classDLLNODE< CLS_Server* > *pConnNODE;

    this->LockLIST ();
    pConnNODE = m_LIST.GetHeadNode ();
    while ( pConnNODE ) {
        if ( dwRight >= pConnNODE->DATA->m_dwAcceptRIGHT &&
			 pConnNODE->DATA->m_bActive ) {
            pCPacket->AppendString ( pConnNODE->DATA->m_ServerNAME.Get() );
            pCPacket->AppendData   (&pConnNODE->DATA->m_iSocketIDX, sizeof(int) );
        }

        pConnNODE = m_LIST.GetNextNode (pConnNODE);
    }

    this->UnlockLIST ();

    return true;
}

//-------------------------------------------------------------------------------------------------
void CLS_ListSERVER::Send_lsv_ANNOUNCE_CHAT (void *pServer, char *szChatMSG)
{
    classDLLNODE< CLS_Server* > *pConnNODE, *pNextNODE;

    this->LockLIST ();
    pConnNODE = m_LIST.GetHeadNode ();
    while ( pConnNODE ) {
		pNextNODE = m_LIST.GetNextNode (pConnNODE);

		if ( pConnNODE->DATA == pServer ) {
            pConnNODE->DATA->Send_str_PACKET( LSV_ANNOUNCE_CHAT, szChatMSG );
		}
            
        pConnNODE = pNextNODE;
    }

    this->UnlockLIST ();
}

//-------------------------------------------------------------------------------------------------
void CLS_ListSERVER::Send_lsv_CHECK_ALIVE (void)
{
    DWORD dwCurTIME = classTIME::GetCurrentAbsSecond ();

    classDLLNODE< CLS_Server* > *pConnNODE;

    this->LockLIST ();
    pConnNODE = m_LIST.GetHeadNode ();
    while ( pConnNODE ) {
        if ( !pConnNODE->DATA->m_dwCheckSEC ) {
            // 현 서버 ip에 새로운 서버 리붓패킷 전송 ???
            
            // 이전 패킷에 대해 응답이 안왔다.
            /*
            classDLLNODE< CLS_Server > *pDelNODE;
            pDelNODE = pConnNODE;
            pConnNODE = m_LIST.GetNextNode (pConnNODE);

            // 이서버에 물린 모든 사용자 접속 끊기 :: Disconnect 호출하면 자동 삭제된다..
            // this->Delete( pDelNODE );    <--- 이걸로 삭제하면 소켓은 끊기지 않네그려 ㅡ,.ㅡ
            pDelNODE->DATA.CloseSOCKET( 99 );
            */

            pConnNODE->DATA->CloseSocket ();// "srv:: CHECK_ALIVE" );

            pConnNODE = m_LIST.GetNextNode (pConnNODE);

            continue;
        }

        pConnNODE->DATA->m_dwCheckSEC = 0;
        pConnNODE->DATA->Send_str_PACKET( LSV_CHECK_ALIVE, NULL );

        pConnNODE = m_LIST.GetNextNode (pConnNODE);
    }
    this->UnlockLIST ();
}

//-------------------------------------------------------------------------------------------------
void CLS_ListSERVER::Make_srv_SERVER_LIST_REPLY (classPACKET *pCPacket)
{
	pCPacket->m_HEADER.m_wType = WLS_SERVER_LIST_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_SERVER_LIST_REPLY );

	pCPacket->m_srv_SERVER_LIST_REPLY.m_dwBuildNO   = GetServerBuildNO();
	pCPacket->m_srv_SERVER_LIST_REPLY.m_dwStartTIME = GetServerStartTIME();

	classDLLNODE< CLS_Server* > *pConnNODE;
    this->LockLIST ();
    pConnNODE = m_LIST.GetHeadNode ();
    while ( pConnNODE ) {
        pCPacket->AppendData   (&pConnNODE->DATA->m_wListenPORT,	sizeof(WORD)  );
		pCPacket->AppendData   (&pConnNODE->DATA->m_dwRandomSEED,	sizeof(DWORD) );
		pCPacket->AppendString ( pConnNODE->DATA->m_ServerNAME.Get() );
		pCPacket->AppendString ( pConnNODE->DATA->m_ServerIP.Get() );
        pConnNODE = m_LIST.GetNextNode (pConnNODE);
    }
	pCPacket->m_srv_SERVER_LIST_REPLY.m_nServerCNT = m_LIST.GetNodeCount();
    this->UnlockLIST ();
}

//-------------------------------------------------------------------------------------------------
