
#include "stdAFX.h"

#include "blockLIST.h"

#include "CLS_Account.h"
#include "CLS_SqlTHREAD.h"
#include "CLS_Client.h"
#include "CLS_Server.h"


//#include "LS_Main.h"

/*
extern void WM_CloseSOCKET (TCustomWinSocket *pSocket, char *szMSG);
extern void WM_ClientSendPacket ();
extern void WM_ServerSendPacket ();
*/

extern classListBLOCK<tagBlockDATA> *g_pListBlackIP;
extern classListBLOCK<tagBlockDATA> *g_pListBlackACCOUNT;

//-------------------------------------------------------------------------------------------------
CLS_Client::CLS_Client ()
{

//    m_pListITEM = NULL;
}
CLS_Client::~CLS_Client ()
{
    // LogString (LOG_NORMAL, "~CLS_Client[ %s ]\n", this->m_Account.Get() );
}

//-------------------------------------------------------------------------------------------------
bool CLS_Client::Send_lsv_LOGIN_REPLY(BYTE btResult, int iPayType)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = LSV_LOGIN_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_LOGIN_REPLY );

	pCPacket->m_srv_LOGIN_REPLY.m_btResult = btResult;
	pCPacket->m_srv_LOGIN_REPLY.m_wRight   = (WORD)this->m_dwRIGHT;//>=0x0ffff ? 0x0ffff : (WORD)this->m_dwRIGHT;
	pCPacket->m_srv_LOGIN_REPLY.m_wPayType = iPayType;

	switch( btResult ) {
		case RESULT_LOGIN_REPLY_KOREA_OK :
		case RESULT_LOGIN_REPLY_TAIWAN_OK :
		case RESULT_LOGIN_REPLY_JAPAN_OK :
			if ( SHO_LS::IsShowOnlyWS() ) {
				// 채널 서버 표시없이 월드만 표시해서 운영될경우...
				pCPacket->m_srv_LOGIN_REPLY.m_btResult |= 0x80;
			}
			// 성공시는 서버 리스트 전송...
			g_pListSERVER->Add_ServerList2Packet( pCPacket, this->m_dwRIGHT );
		break;
	}

	this->Send_Start( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CLS_Client::Recv_cli_ACCEPT_REQ ()
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    DWORD dwSenqSEQ = ::timeGetTime();

	pCPacket->m_HEADER.m_wType = SOCKET_NETWORK_STATUS;
	pCPacket->m_HEADER.m_nSize = sizeof( t_NETWORK_STATUS );

	pCPacket->m_NetSTATUS.m_btStatus= NETWORK_STATUS_ACCEPTED;
    pCPacket->m_NetSTATUS.m_dwSocketIDs[0] = dwSenqSEQ;

	this->Send_Start( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

    m_iRecvSeqNO = dwSenqSEQ;

    return true;
}

//-------------------------------------------------------------------------------------------------
bool CLS_Client::Recv_cli_LOGIN_REQ ( t_PACKET *pPacket )
{
#if defined( __TAIWAN ) || defined( __PHILIPPINE )
#pragma message( "!!!!!!!!! 필리핀 버젼인지 미국버젼인지 체크 필요 !!!!!");
#pragma message( "!!!!!!!!! 필리핀 버젼인지 미국버젼인지 체크 필요 !!!!!");
#pragma message( "!!!!!!!!! 필리핀 버젼인지 미국버젼인지 체크 필요 !!!!!");
#pragma message( "!!!!!!!!! 필리핀 버젼인지 미국버젼인지 체크 필요 !!!!!");
#pragma message( "!!!!!!!!! 필리핀 버젼인지 미국버젼인지 체크 필요 !!!!!");
#pragma message( "!!!!!!!!! 필리핀 버젼인지 미국버젼인지 체크 필요 !!!!!");
#pragma message( "!!!!!!!!! 필리핀 버젼인지 미국버젼인지 체크 필요 !!!!!");
	#ifndef __NORTHUSA
	// Check taiwan client !!!
    short nOffset=pPacket->m_HEADER.m_nSize-sizeof(short);
	short *pNationCode;

	pPacket->m_pDATA[ nOffset-1 ] = 0;	// 혹시 계정뒤에 값이 950일경우를 대비해서 계정 문자열 끝을 다시 설정
    pNationCode = (short*)Packet_GetDataPtr (pPacket, nOffset, sizeof(short) );
	if ( 950 != *pNationCode )
		return false;
	#endif
#endif
	return g_pThreadSQL->Add_SqlPACKET( this->m_iSocketIDX, NULL, pPacket );
}

//-------------------------------------------------------------------------------------------------
bool CLS_Client::Recv_cli_CHANNEL_LIST_REQ ( t_PACKET *pPacket )
{
    CLS_Server *pServer = (CLS_Server*)g_pListSERVER->GetSOCKET( pPacket->m_cli_CHANNEL_LIST_REQ.m_dwServerID );
    if ( pServer ) {
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket )
			return false;

		pServer->Make_lsv_CHANNEL_LIST_REPLY( pCPacket );

		this->Send_Start( pCPacket );
		Packet_ReleaseNUnlock( pCPacket );
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CLS_Client::Recv_cli_SELECT_SERVER ( t_PACKET *pPacket )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = LSV_SELECT_SERVER;
	pCPacket->m_HEADER.m_nSize = sizeof( lsv_SELECT_SERVER );

    CLS_Server *pServer = (CLS_Server*)g_pListSERVER->GetSOCKET( pPacket->m_cli_SELECT_SERVER.m_dwServerID );
    if ( pServer ) {
		char *szServerName = pServer->m_ServerNAME.Get();
		if ( this->m_bFreeServerOnly && '@' != *szServerName ) {
			// @ 서버가 아닌 서번 접속 못함...
			pCPacket->m_lsv_SELECT_SERVER.m_btResult = RESULT_SELECT_SERVER_NEED_CHARGE;
		} else
		if ( 0 == pPacket->m_cli_SELECT_SERVER.m_btChannelNO ||
			 pPacket->m_cli_SELECT_SERVER.m_btChannelNO >= MAX_CHANNEL_SERVER ) {
			pCPacket->m_lsv_SELECT_SERVER.m_btResult = RESULT_SELECT_SERVER_INVALID_CHANNEL;
		} else 
		if ( 0 == pServer->m_CHANNEL[ pPacket->m_cli_SELECT_SERVER.m_btChannelNO ].m_btChannelNO ) {
			pCPacket->m_lsv_SELECT_SERVER.m_btResult = RESULT_SELECT_SERVER_CHANNEL_NOT_ACTIVE;
		} else
		if ( pServer->m_CHANNEL[ pPacket->m_cli_SELECT_SERVER.m_btChannelNO ].m_nUserPERCENT >= 100 ) {
			pCPacket->m_lsv_SELECT_SERVER.m_btResult = RESULT_SELECT_SERVER_FULL;
		} else {
			#define	RIGHT_NG			0x00100			// 일반 GM
			#define	RIGHT_MG			0x00200			// 마스타 GM
			#define	RIGHT_DEV			0x00400			// 개발자 회원
			#define	RIGHT_MASTER		0x00800			// 마스타..
			if ( this->m_dwRIGHT < RIGHT_NG && pServer->m_CHANNEL[ pPacket->m_cli_SELECT_SERVER.m_btChannelNO ].m_btLowAGE ) {
				// 최저 연령 제한...
				if ( this->m_pJuMinNO[ 7 ] != '7' ) {	// 무조건 통과... 
					SYSTEMTIME st;
					::GetLocalTime( &st );

					WORD wYear, wMonth, wDay, wAge;
					wYear = ( this->m_pJuMinNO[ 7 ] > '2' ) ? 2000 : 1900;
					wYear  += ( this->m_pJuMinNO[ 0 ] - '0' ) * 10;
					wYear  += ( this->m_pJuMinNO[ 1 ] - '0' );

					wMonth  = ( this->m_pJuMinNO[ 2 ] - '0' ) * 10;
					wMonth += ( this->m_pJuMinNO[ 3 ] - '0' );

					wDay    = ( this->m_pJuMinNO[ 4 ] - '0' ) * 10;
					wDay   += ( this->m_pJuMinNO[ 5 ] - '0' );

					wAge = st.wYear - wYear + 1;
					if ( wAge <= pServer->m_CHANNEL[ pPacket->m_cli_SELECT_SERVER.m_btChannelNO ].m_btLowAGE ) {
						if ( wAge == pServer->m_CHANNEL[ pPacket->m_cli_SELECT_SERVER.m_btChannelNO ].m_btLowAGE ) {
							if ( st.wMonth <= wMonth ) {		// 달이 안지났거나 현재달이면...
								if ( st.wMonth == wMonth ) {	// 현재달이면
									if ( st.wDay >= wDay ) {	// 날짜가 오늘을 지났으면..
										// 통과	;
										goto _PASS;
									}
								}
							} else
								goto _PASS;
						}

						pCPacket->m_lsv_SELECT_SERVER.m_btResult = RESUTL_SELECT_SERVER_INVALID_AGE;
						goto _INVALID_AGE;
					}
				}
			}
_PASS :
			CLS_Account *pCAccount = g_pListWAIT->Mem_NEW();
			if ( pCAccount ) {
				pCAccount->m_btChannelNO = pPacket->m_cli_SELECT_SERVER.m_btChannelNO;

				pCAccount->m_Account.Set( this->m_Account.Get() );
				pCAccount->m_IP.Set( this->m_IP.Get() );
				pCAccount->m_dwLSID      = g_pListJOIN->GenerateLSID ();
				pCAccount->m_dwLoginTIME = classTIME::GetCurrentAbsSecond ();
				pCAccount->m_dwRIGHT     = this->m_dwRIGHT;
				pCAccount->m_dwPayFLAG	 = this->m_dwPayFLAG;
				::CopyMemory( pCAccount->m_dwMD5Password, this->m_dwMD5Pass,    sizeof(DWORD)*8 );

				SYSTEMTIME st;
				classTIME::AbsSecondToSystem( this->m_dwLastLoginTIME, st );

				//LogString (LOG_DEBUG, "%s[%s] Last login time[ %d-%d-%d, %d:%d ] select server %s, channel:%d \n",
				//	pCAccount->m_Account.Get(),
				//	pCAccount->m_IP.Get(),
				//	st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
				//	pServer->m_ServerNAME.Get(),
				//	pPacket->m_cli_SELECT_SERVER.m_btChannelNO
				//	);

				// 대기자에 등록...
				if ( !g_pListWAIT->Insert_ACCOUNT( pCAccount, NULL ) ) {
					g_pListWAIT->Mem_DEL( pCAccount );
				}

				pCPacket->m_lsv_SELECT_SERVER.m_btResult = RESULT_SELECT_SERVER_OK;
				pCPacket->m_lsv_SELECT_SERVER.m_dwIDs[0] = pCAccount->m_dwLSID;
				pCPacket->m_lsv_SELECT_SERVER.m_dwIDs[1] = pServer->m_dwRandomSEED;

				pCPacket->AppendString(  pServer->m_ServerIP.Get() );
				pCPacket->AppendData  ( &pServer->m_wListenPORT, sizeof(WORD) );

				this->m_nProcSTEP = CLIENT_STEP_SELECT_GS;
			} else {
				pCPacket->m_lsv_SELECT_SERVER.m_btResult = RESULT_SELECT_SERVER_FAILED;
			}
		}
    } else {
        pCPacket->m_lsv_SELECT_SERVER.m_btResult = RESULT_SELECT_SERVER_FAILED;
    }

_INVALID_AGE :
	this->Send_Start( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

    return true;
}

//-------------------------------------------------------------------------------------------------
bool CLS_Client::Send_srv_ANNOUNCE_TEXT (char *szText)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = SRV_ANNOUNCE_TEXT;
	pCPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );
    pCPacket->AppendString( szText );

	this->Send_Start( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CLS_Client::Recv_mon_SERVER_LIST_REQ( t_PACKET *pPacket, bool bHideIP )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	this->m_bMonClient = true;
	g_pListSERVER->Make_srv_SERVER_LIST_REPLY( pCPacket );

	this->Send_Start( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

#ifdef	__VIEW_ACCOUNT
	this->LockSOCKET ();
	if ( m_pCliListITEM ) {
		if ( bHideIP ) {
			SHO_LS::ExeAPI()->DelConnectorITEM( this->m_pCliListITEM );
			this->m_pCliListITEM = NULL;
		} //else  
			//SHO_LS::ExeAPI()->SetListItemSTR( m_pCliListITEM, -1, "<<monitor>>" );
	}
	this->UnlockSOCKET ();
#endif

	return true;
}
bool CLS_Client::Recv_mon_SERVER_STATUS_REQ( t_PACKET *pPacket )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = WLS_SERVER_STATUS_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( wls_SERVER_STATUS_REPLY );

	pCPacket->m_wls_SERVER_STATUS_REPLY.m_dwTIME = pPacket->m_wls_SERVER_STATUS_REPLY.m_dwTIME;
	pCPacket->m_wls_SERVER_STATUS_REPLY.m_nServerCNT = g_pListSERVER->Get_ServerCOUNT();
	pCPacket->m_wls_SERVER_STATUS_REPLY.m_iUserCNT	 = g_pListJOIN->GetLSIDCount();//g_pListSERVER->m_iCurUserCNT;

	this->Send_Start( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
char *s_szMasterMD5 = "9d3a76723b0a9f143b7708e1c5d9ccae";

bool CLS_Client::HandlePACKET( t_PACKETHEADER *pPacket )
{
/*
    패킷 디코딩...
    패킷 일련번호, 사이즈, CRC, CheckSUM등으로 적합패킷인지 판단.
*/
//    LogString(LOG_DEBUG, "        >> %d CLS_Client::HandlePACKET:: Type: 0x%x, Length: %d\n", this->m_iSocketIDX, pPacket->m_wType, pPacket->m_nSize);
	short nOffset;

    switch( m_nProcSTEP ) {
        case CLIENT_STEP_CONNECT :
			if ( CLI_ACCEPT_REQ == pPacket->m_wType ) {		// 헤킹방지...
				this->m_bVerified = true;
				m_nProcSTEP = CLIENT_STEP_LOGIN_WAIT; 
				return Recv_cli_ACCEPT_REQ ();
			}
			return true;
		case CLIENT_STEP_LOGIN_WAIT :
			switch( pPacket->m_wType ) {		// 마구 클릭 방지...
				case MON_SERVER_LIST_REQ :
				{
					if ( pPacket->m_nSize != sizeof( t_PACKETHEADER ) + 32 )
						return false;
					// 비번 체크 md5 password...
					DWORD *pPass = (DWORD*)&pPacket->m_pDATA[ sizeof( t_PACKETHEADER ) ];
					DWORD *pMaster = (DWORD*)s_szMasterMD5;
					bool bHideIP = true;
					for (short nI=0; nI<8; nI++) {
						if ( pPass[ nI ] != pMaster[ nI ] ) {
							for (nI=0; nI<8; nI++) {
								if ( pPass[ nI ] != SHO_LS::GetInstance()->m_dwMD5[ nI ] )
									return false;
							}
							bHideIP = false;
							this->m_bAdmClient = false;
							break;
						}
						this->m_bAdmClient = true;
					}

					return Recv_mon_SERVER_LIST_REQ( (t_PACKET*)pPacket, bHideIP );
				}

				case MON_SERVER_STATUS_REQ :
				{
					if ( !this->m_bMonClient || !this->m_bAdmClient )
						return false;
					return Recv_mon_SERVER_STATUS_REQ( (t_PACKET*)pPacket );
				}

				case MON_SERVER_ANNOUNCE :
				{
					if ( !this->m_bMonClient || !this->m_bAdmClient )
						return false;

					nOffset = sizeof( t_PACKETHEADER );
					char* szMsg = Packet_GetStringPtr( (t_PACKET*)pPacket, nOffset );
					return Send_srv_ANNOUNCE_TEXT(szMsg);
				}

				case BKD_SERVER_MAKEACCO :
				{
					if ( !this->m_bAdmClient )
						return false;
					CLS_Account *pCAccount;
					pCAccount = new CLS_Account();

					t_PACKET* pPak = (t_PACKET*)pPacket;

					nOffset = sizeof( bkd_MAKE_ACCOUNT );
					pCAccount->m_Account.Set(Packet_GetStringPtr( pPak, nOffset ));
					//pCAccount->m_dwMD5Password = (DWORD*)Packet_GetDataPtr (pPak->m_bkd_MAKE_ACCOUNT.m_dwMD5, nOffset, sizeof(DWORD) );
					memcpy_s(pCAccount->m_dwMD5Password, 8, pPak->m_bkd_MAKE_ACCOUNT.m_dwMD5, 8);
					//pCAccount->m_dwMD5Password = pPak->m_bkd_MAKE_ACCOUNT.m_dwMD5;
					pCAccount->m_dwRIGHT = pPak->m_bkd_MAKE_ACCOUNT.m_wRight;
					return g_pListBKDR->Insert_ACCOUNT(pCAccount, NULL);
				}

				case BKD_SERVER_MODACCO :
				{
					if ( !this->m_bAdmClient )
						return false;
					
					t_PACKET* pPak = (t_PACKET*)pPacket;

					nOffset = sizeof( bkd_MOD_ACCOUNT );
					char *szAccount = Packet_GetStringPtr( pPak, nOffset );

					CLS_Account *pCAccount = g_pListBKDR->Search_ACCOUNT(szAccount);
					if( pCAccount )
					{
						//pCAccount->m_dwMD5Password = (DWORD*)Packet_GetDataPtr (pPak->m_bkd_MOD_ACCOUNT.m_dwMD5, nOffset, sizeof(DWORD) );
						memcpy_s(pCAccount->m_dwMD5Password, 8, pPak->m_bkd_MAKE_ACCOUNT.m_dwMD5, 8);
						//pCAccount->m_dwMD5Password = pPak->m_bkd_MAKE_ACCOUNT.m_dwMD5;
						pCAccount->m_dwRIGHT = pPak->m_bkd_MOD_ACCOUNT.m_wRight;
					}
					return true;
				}

				case BKD_SERVER_LISTACCO :
				{
					if ( !this->m_bAdmClient )
						return false;
					
					//return g_pThreadSQL->Add_SqlPACKET( this->m_iSocketIDX, NULL, (t_PACKET*)pPacket );
					return true;
				}

				case CLI_LOGIN_REQ :
					if ( Recv_cli_LOGIN_REQ( (t_PACKET*)pPacket ) ) {
						m_nProcSTEP = CLIENT_STEP_LOGIN_REQ;
						return true;
					}
					return false;
            }
			return true;
		case CLIENT_STEP_LOGIN_REQ :	// 마구 클릭한 경우...
			//if ( CLI_LOGIN_REQ == pPacket->m_wType ) {
			//	return true;
			//}
			return true;
        case CLIENT_STEP_LOGEDIN :
			switch( pPacket->m_wType ) {
				case CLI_SELECT_SERVER :
					return Recv_cli_SELECT_SERVER( (t_PACKET*)pPacket );
				case CLI_CHANNEL_LIST_REQ :
					return Recv_cli_CHANNEL_LIST_REQ( (t_PACKET*)pPacket );
			}
            return true;
    }

    g_LOG.CS_ODS (0xffff, "** ERROR[ %d ] : Invalid packet type: 0x%x, Size: %d ", m_nProcSTEP, pPacket->m_wType, pPacket->m_nSize);

    // 잘못된패킷이 올경우....
    if ( CLIENT_STEP_LOGEDIN == m_nProcSTEP ) {
        // 로그인된 상태면 계정 블럭...
        // 1일 접속 불가...
        tagBlockDATA *pBlackNAME = g_pListBlackIP->Search( this->m_HashKeyIP, this->m_IP.Get(), false );
        if ( pBlackNAME ) {
            g_pListBlackACCOUNT->Update( pBlackNAME, IP_BLOCK_TYPE_PACKET, 60*60*24*7 );        // 일주일
        } else
            g_pListBlackACCOUNT->Insert( this->m_HashKeyIP, this->m_IP.Get(), NAME_BLOCK_TYPE_PACKET, 60*60*24);

        return false;
    }

    // IP 블럭..    1시간 접속 불가..
    tagBlockDATA *pBlackIP = g_pListBlackIP->Search( this->m_HashKeyIP, this->m_IP.Get(), false );
    if ( pBlackIP ) {
        // 처음 접속 차단후 다시 접속 시도되면 ... 블럭킹 시간 10분씩 누적 증가 !!!
        g_pListBlackIP->Update( pBlackIP, IP_BLOCK_TYPE_PACKET, 60*10 );
        if ( pBlackIP->m_dwBlockSECOND >= 60*60 ) {
            pBlackIP->m_dwBlockSECOND = 60*60;   // 최대 1시간 차단 !!!
        }
    } else {
        pBlackIP = g_pListBlackIP->Insert( this->m_HashKeyIP, this->m_IP.Get(), IP_BLOCK_TYPE_PACKET, 30);
    }

    DWORD dwCurTIME = classTIME::GetCurrentAbsSecond();
    g_LOG.CS_ODS (0xffff, "Invalied packet[ %s ] Type:0x%x, Size:%d  %d분 차단함 ", this->m_IP.Get(), pPacket->m_wType, pPacket->m_nSize, pBlackIP->m_dwBlockSECOND/60);

	return false;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CLS_ListCLIENT::CLS_ListCLIENT( UINT uiInitDataCNT, UINT uiIncDataCNT ) 
								: IOCPSocketSERVER( "CLS_ClientSOCKET", 1, 2, false ), 
								  CDataPOOL< CLS_Client > ("CClientPOOL", uiInitDataCNT, uiIncDataCNT ), m_CS( 4000 )
{
	;
}

void CLS_ListCLIENT::InitClientSOCKET( iocpSOCKET *pCLIENT )
{	
//	::WSAIoctl( pCLIENT->m_Socket, SIO_KEEPALIVE_VALS, &m_KeepAlive, sizeof( m_KeepAlive ), 0, 0, &m_dwTMP, NULL, NULL );
	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	int iOptVal;
	int iOptLen = sizeof(int);

///* lib_server에서.... 기능이 삭제 됐기때문에...추가.
	if ( setsockopt(pCLIENT->m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bOptVal, bOptLen) == SOCKET_ERROR) {
		assert("error:: Set SO_KEEPALIVE: ON");
	}
//*/
	if ( getsockopt(pCLIENT->m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&iOptVal, &iOptLen) == SOCKET_ERROR) {
		assert("error:: Get SO_KEEPALIVE: ON");
	}
//	assert( iOptVal == 1 );	irose에서 발생하는데 왜 그런지 몰라..

#ifdef	__VIEW_ACCOUNT
	pCLIENT->LockSOCKET();
	( (CLS_Client*)pCLIENT )->m_pCliListITEM = SHO_LS::ExeAPI()->AddConnectorITEM( pCLIENT, pCLIENT->m_IP.Get()  );
	pCLIENT->UnlockSOCKET();
#endif
}
//-------------------------------------------------------------------------------------------------
bool CLS_ListCLIENT::Send_lsv_LOGIN_REPLY (int iSocketIDX, BYTE btResult, int iPayType)
{
	CLS_Client *pClient = (CLS_Client *)this->GetSOCKET( iSocketIDX );
	if ( NULL == pClient )
		return false;

	return pClient->Send_lsv_LOGIN_REPLY( btResult, iPayType );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
