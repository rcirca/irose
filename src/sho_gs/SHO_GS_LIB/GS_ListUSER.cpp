
#include "LIB_gsMAIN.h"
#include "GS_ListUSER.h"
#include "GS_ThreadSQL.h"
#include "GS_Party.h"
#include "GS_SocketLSV.h"
#include "GS_SocketASV.h"
#include "IO_Quest.h"

#ifdef	__INC_WORLD
	#include "CChatROOM.h"
	#include "CThreadGUILD.h"
#endif

extern void IncUserCNT( int iUserCNT, classUSER *pUSER );
extern void DecUserCNT( int iUserCNT, classUSER *pUSER );

//-------------------------------------------------------------------------------------------------
// Worker Thread 갯수 = CPU * 2 + 2
CUserLIST::CUserLIST ( UINT uiInitDataCNT, UINT uiIncDataCNT ) 
						      : CDataPOOL< classUSER > ("CUserPOOL", uiInitDataCNT, uiIncDataCNT ),
								m_csHashACCOUNT( 4000 ),
								m_csHashCHAR( 4000 ),
								m_csNullZONE( 4000 ),
#ifdef	__INC_WORLD
							  IOCPSocketSERVER( "GS_SocketSERVER", 0, 2, true )
#else
							  IOCPSocketSERVER( "GS_SocketSERVER", 2, 2, true )
#endif
{
	m_pHashACCOUNT	= new classHASH< classUSER* >( 1024 * 2 );
	m_pHashCHAR		= new classHASH< classUSER* >( 1024 * 2 );

	char szTmp[ MAX_PATH ];
	t_HASHKEY HashKey;
	m_HashLevelUpTRIGGER = new t_HASHKEY[MAX_LEVEL];
	::ZeroMemory( m_HashLevelUpTRIGGER, sizeof(t_HASHKEY)*MAX_LEVEL );

	for (int iL=1;iL<MAX_LEVEL; iL++) {
		sprintf( szTmp, "levelup_%d", iL );
		HashKey = ::StrToHashKey( szTmp );
		if ( g_QuestList.GetQuest( HashKey ) ) 
			m_HashLevelUpTRIGGER[ iL ] = HashKey;
	}
}
CUserLIST::~CUserLIST ()
{
	SAFE_DELETE( m_pHashCHAR    );
	SAFE_DELETE( m_pHashACCOUNT );
}

//-------------------------------------------------------------------------------------------------
void CUserLIST::InitData (classUSER *pData)
{
    pData->Init_GameOBJ ();
	pData->InitUSER ();
}
void CUserLIST::FreeData (classUSER *pData)
{
	// assert( 0 == pData->m_iSocketIDX );
	pData->Clear_LIST ();
	pData->FreeUSER ();
}

//-------------------------------------------------------------------------------------------------
// @@ 서로 다른 쓰레드에의해 중복 호출될수 있다...
// @@ 반드시 소켓이 종료되어 있어야 한다.
void CUserLIST::DeleteUSER (classUSER *pUSER, BYTE btLogOutMODE)
{
	// 같은 코드 여러번 실행되지 않도록..
	pUSER->LockSOCKET ();
	{
		if ( pUSER->m_btLogOutMODE ) {
			pUSER->UnlockSOCKET ();
			return;
		}

		pUSER->m_btLogOutMODE = btLogOutMODE;

		if ( pUSER->m_iClanCreateMoney )
			pUSER->CheckClanCreateCondition( 2 );	// 클랜 생성하다 종료 :: 감소된 조건 복원
	}
	pUSER->UnlockSOCKET ();

	if ( pUSER->Is_CartDriver() || pUSER->Is_CartGuest() ) {
		classUSER *pUser2 = (classUSER*)g_pUserLIST->GetSOCKET( pUSER->m_iLinkedCartUsrIDX );
		pUser2->m_iLinkedCartUsrIDX = 0;

		if ( pUSER->Is_CartDriver() ) {
			// 나가는 케릭이 운전자다.. 손님을 걷기 상태로 변경
			pUser2->m_btRideMODE = 0;
			pUser2->m_btRideATTR = RIDE_ATTR_NORMAL;
		}

		pUser2->m_iLinkedCartObjIDX = 0;
		pUser2->m_iLinkedCartUsrIDX = 0;
	}

	//if ( pUSER->m_pPartyBUFF ) {
	//	pUSER->m_pPartyBUFF->OnDisconnect( pUSER );	
	//}

#ifdef	__INC_WORLD
	g_pChatROOMs->LeftUSER ( pUSER );
#endif

	// ZoneThread에서 바로 호출되어 올경우 소켓 닫고 삭제하기..
	this->Del_SOCKET( pUSER->m_iSocketIDX );

	// delete Character from Hash table...
	m_csHashCHAR.Lock ();
	if ( pUSER->m_HashCHAR ) {
	#ifdef	__INC_WORLD
		LogString( 0xffff, "%d Sub_CHAR[ %s:%s:%s ] ..\n", this->GetUsedSocketCNT(), pUSER->Get_ACCOUNT(), pUSER->Get_NAME(), pUSER->Get_IP() );

		g_pThreadMSGR->Add_MessengerCMD( pUSER->Get_NAME(), MSGR_CMD_LOGOUT, NULL, 0 );
		if ( pUSER->GetClanID() )
			g_pThreadGUILD->Add_ClanCMD( GCMD_LOGOUT, pUSER->GetClanID(), NULL, pUSER->Get_NAME() );
	#endif
		if ( !m_pHashCHAR->Delete( pUSER->m_HashCHAR, pUSER ) ) {
			g_LOG.CS_ODS( 0xffff, "ERROR *** Sub_CHAR( %s : %s) not found \n", pUSER->Get_ACCOUNT(), pUSER->Get_IP() );
		}
		pUSER->m_HashCHAR = 0;
	}
	m_csHashCHAR.Unlock ();
	
	// delete Account from Hash table...
	m_csHashACCOUNT.Lock ();
	if ( pUSER->m_HashACCOUNT ) {
		this->Sub_NullZONE( &pUSER->m_ZoneNODE );

		if ( !m_pHashACCOUNT->Delete( pUSER->m_HashACCOUNT, pUSER ) ) {
			g_LOG.CS_ODS( 0xffff, "ERROR *** Sub_ACCOUNT( %s : %s) not found \n", pUSER->Get_ACCOUNT(), pUSER->Get_IP() );
		}
		pUSER->m_HashACCOUNT = 0;

		::DecUserCNT( m_pHashACCOUNT->GetCount(), pUSER );
	}
	m_csHashACCOUNT.Unlock ();

#ifdef	__INC_WORLD
	LogString( 0xffff, "Delete Char:: %s, Account: %s [ %s ]\n", 
			pUSER->Get_NAME(),
			pUSER->Get_ACCOUNT(),
			pUSER->m_IP.Get() );
#endif
	g_pThreadSQL->Add_BackUpUSER( pUSER, btLogOutMODE );
}

//-------------------------------------------------------------------------------------------------
bool CUserLIST::SendPacketToSocketIDX (int iClientSocketIDX, classPACKET *pCPacket)
{
	// 반드시 pCPacket은 1 유저에게 보내는 패킷이어야 한다.
	classUSER *pFindUSER = (classUSER*)this->GetSOCKET( iClientSocketIDX );
	if ( pFindUSER ) {
		if ( !pFindUSER->SendPacket( pCPacket ) ) {
			// 보내기 실패...
		}

		return true;
	} 

	return false;
}

//-------------------------------------------------------------------------------------------------
void CUserLIST::Check_SocketALIVE ()
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = SRV_ERROR;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_ERROR );
	pCPacket->m_gsv_ERROR.m_wErrorCODE = 0;
	
	DWORD dwCurTIME = ::timeGetTime();
	classDLLNODE< CGameOBJ* > *pUserNODE;

	m_csNullZONE.Lock ();
		pUserNODE = m_NullZoneLIST.GetHeadNode();
		while( pUserNODE ) {
			// 마지막으로 패킷을 보낸후 30초가 넘었으면...
			if ( dwCurTIME - ( (classUSER*)pUserNODE->DATA )->Get_CheckTIME() >= 30*1000 ) {
				pUserNODE->DATA->SendPacket( pCPacket );
				// 5분동안 암것도 보내지 못했으면... 짤러~~~~
				if ( dwCurTIME - SOCKET_KEEP_ALIVE_TIME > ( (classUSER*)pUserNODE->DATA )->Get_CheckTIME() )
					( (classUSER*)pUserNODE->DATA )->CloseSocket ();
			}
			pUserNODE = m_NullZoneLIST.GetNextNode( pUserNODE );
		}
	m_csNullZONE.Unlock ();

	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void CUserLIST::Send_wsv_CREATE_CHAR (int iSocketIDX, BYTE btResult)
{
#ifndef	__INC_WORLD
	assert( 0 && "Send_wsv_CREATE_CHAR" );
	return;
#endif
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = WSV_CREATE_CHAR;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_CREATE_CHAR );
	pCPacket->m_wsv_CREATE_CHAR.m_btResult = btResult;

	this->SendPacketToSocketIDX( iSocketIDX, pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void CUserLIST::Send_wsv_MEMO (int iSocketIDX, BYTE btTYPE, short nMemoCNT)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = WSV_MEMO;
	pCPacket->m_wsv_MEMO.m_btTYPE = btTYPE;

	if ( nMemoCNT >= 0 ) {
		pCPacket->m_HEADER.m_nSize = sizeof( wsv_MEMO ) + sizeof(short);
		pCPacket->m_wsv_MEMO.m_nRecvCNT[ 0 ] = nMemoCNT;
	} else
		pCPacket->m_HEADER.m_nSize = sizeof( wsv_MEMO );

	this->SendPacketToSocketIDX( iSocketIDX, pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void CUserLIST::Send_wsv_GUILD_COMMAND (int iSocketIDX, BYTE btResult, char *szStr)
{
	assert( 0 );
	//classPACKET *pCPacket = Packet_AllocNLock ();
	//if ( !pCPacket )
	//	return;

	//pCPacket->m_HEADER.m_wType = WSV_GUILD_COMMAND;
	//pCPacket->m_HEADER.m_nSize = sizeof( wsv_GUILD_COMMAND );
	//pCPacket->m_wsv_GUILD_COMMAND.m_btRESULT = btResult;

	//if ( szStr )
	//	pCPacket->AppendString( szStr );

	//this->SendPacketToSocketIDX( iSocketIDX, pCPacket );

	//Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
bool CUserLIST::Add_CHAR (classUSER *pUSER)
{
#ifdef	__INC_WORLD
	LogString( LOG_DEBUG, "Add_CHAR[ iSockIDX:%d, %s:%s:%s ] ..\n", pUSER->m_iSocketIDX, pUSER->Get_ACCOUNT(), pUSER->Get_NAME(), pUSER->Get_IP() );
#endif
	// GS_CThreadSQL::Run_SqlPACKET()에서 호출되는 함수로 Lock 필요 없다.
	pUSER->m_HashCHAR = CStrVAR::GetHASH( pUSER->Get_NAME() );
	m_csHashCHAR.Lock ();
	m_pHashCHAR->Insert( pUSER->m_HashCHAR, pUSER );
	m_csHashCHAR.Unlock ();

#ifdef	__INC_WORLD
	g_pThreadMSGR->Add_MessengerCMD( pUSER->Get_NAME(), MSGR_CMD_LOGIN, NULL, pUSER->m_iSocketIDX, pUSER->m_dwDBID );
	g_pThreadGUILD->Add_ClanCMD( GCMD_LOGIN, pUSER->m_iSocketIDX, NULL, pUSER->Get_NAME() );
#endif

	return true;
}

classUSER* CUserLIST::Find_CHAR (char *szCharName)
{
	t_HASHKEY HashKEY = CStrVAR::GetHASH( szCharName );

	classUSER *pUSER;
	tagHASH< classUSER* > *pHashNode;

	m_csHashCHAR.Lock ();

	pHashNode = m_pHashCHAR->Search( HashKEY );
	pUSER = pHashNode ? pHashNode->m_DATA : NULL;
	while( pUSER ) {
		if ( !strcmpi( szCharName, pUSER->Get_NAME() ) ) {
			// Find !!!
			break;
		}

		pHashNode = m_pHashCHAR->SearchContinue( pHashNode, HashKEY );
		pUSER = pHashNode ? pHashNode->m_DATA : NULL;
	} ;
	m_csHashCHAR.Unlock ();

	return pUSER;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CUserLIST::Add_ACCOUNT (int iSocketIDX, t_PACKET *pRecvPket, char *szAccount)
{
	classUSER *pUSER = (classUSER*)this->GetSOCKET( iSocketIDX );
	if ( NULL == pUSER ) 
		return false;
	classUSER *pFIND = this->Find_ACCOUNT( szAccount );
	if ( pFIND ) {
		pFIND->CloseSocket ();
		pUSER->CloseSocket ();
		return false;
	}

	if ( pUSER->Send_srv_JOIN_SERVER_REPLY( pRecvPket, szAccount ) ) {
		if ( RESULT_CONFIRM_ACCOUNT_OK == pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_btResult ) {
			// 우선 이곳에서 계정 서버에 계정 추가 전송...
			g_pSockASV->Send_zws_ADD_ACCOUNT( szAccount, pUSER->Get_MD5PW(), pUSER->Get_IP() );

			// 존에서 빠진 사용자 리스트에 등록...
			g_pUserLIST->Add_NullZONE( &pUSER->m_ZoneNODE );	// CUserLIST::Add_ACCOUNT

			pUSER->m_dwLSID     = pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwLSID;
			pUSER->m_dwWSID		= pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwWSID;
			pUSER->m_dwRIGHT	= pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwRIGHT;
#ifndef	__INC_WORLD
			pUSER->m_dwPayFLAG  = pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwPayFLAG;
#else
			pUSER->m_dwPayFLAG	= 0x0ffffffff;
#endif
			// this->m_dwLoginTIME = pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwLoginTIME;

			// insert hash table ...
			m_csHashACCOUNT.Lock ();
			m_pHashACCOUNT->Insert( pUSER->m_HashACCOUNT, pUSER );
			::IncUserCNT( m_pHashACCOUNT->GetCount(), pUSER );
			m_csHashACCOUNT.Unlock ();

		#ifndef	__INC_WORLD
			// 케릭터 선택해라...
			t_PACKET *pSelCharPket = (t_PACKET*)new BYTE [ 256 ];
			{
				short nOffset = sizeof( wls_CONFIRM_ACCOUNT_REPLY ) + (short)strlen(szAccount)+1;	// + '\0'
			/*
				if ( pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwIngStatusFLAG ) {
					::CopyMemory( &pUSER->m_IngSTATUS, pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_IngSTATUS, sizeof(CIngSTATUS) );
					nOffset += ( sizeof(CIngSTATUS)-sizeof(DWORD) );
				}
			*/
				char *szCharName = Packet_GetStringPtr( pRecvPket, nOffset );

				pSelCharPket->m_HEADER.m_wType = CLI_SELECT_CHAR;
				pSelCharPket->m_HEADER.m_nSize = sizeof( cli_SELECT_CHAR );

				Packet_AppendString( pSelCharPket, szCharName );

				// SQL쓰레드에 등록..
				pUSER->Recv_cli_SELECT_CHAR( pSelCharPket, 
							true			/* pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_bFirstZONE */,
							MOVE_MODE_RUN	/* pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_btRunMODE  */,
							0 /* pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_btRideMODE */ 
							);
			}
			SAFE_DELETE_ARRAY( pSelCharPket );
		#endif
		}
		return true;
	}
	
	pUSER->CloseSocket ();
	return false;
}


//-------------------------------------------------------------------------------------------------
classUSER* CUserLIST::Find_ACCOUNT (char *szAccount)
{
	t_HASHKEY HashKEY = CStrVAR::GetHASH( szAccount );

	tagHASH< classUSER* > *pHashNode;

	m_csHashACCOUNT.Lock ();
	pHashNode = m_pHashACCOUNT->Search( HashKEY );
	
	classUSER *pUSER = pHashNode ? pHashNode->m_DATA : NULL;
	while( pUSER ) {
		if ( !strcmpi( szAccount, pUSER->Get_ACCOUNT() ) ) {
			// Find !!!
			break;
		}

		pHashNode = m_pHashACCOUNT->SearchContinue( pHashNode, HashKEY );
		pUSER = pHashNode ? pHashNode->m_DATA : NULL;
	} ;
	m_csHashACCOUNT.Unlock ();

	return pUSER;
}

classUSER * CUserLIST::Find_IP( char * szIP )
{
	tagHASH< classUSER* > *pAccNode = NULL;

	for(int iT = 0; iT < m_pHashACCOUNT->GetTableCount(); iT++)
	{
		pAccNode = m_pHashACCOUNT->GetEntryNode( iT );
		
		while( pAccNode )
		{
			if( pAccNode->m_DATA->Get_IP() && strcmp( szIP, pAccNode->m_DATA->Get_IP() ) == 0 )
				return pAccNode->m_DATA;
			pAccNode = pAccNode->m_NEXT;
		}
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------
bool CUserLIST::Kick_ACCOUNT( char *szAccount )
{
	classUSER *pUSER = g_pUserLIST->Find_ACCOUNT( szAccount );
	if ( pUSER ) {
		pUSER->CloseSocket ();
		return true;
	}
	return false;
}


//-------------------------------------------------------------------------------------------------
void CUserLIST::Send_zws_ACCOUNT_LIST (CClientSOCKET *pSrvSocket, bool bSendToGUMS )
{
	classPACKET *pCPacket = Packet_AllocNLock ();

	pCPacket->m_HEADER.m_wType = ZWS_ACCOUNT_LIST;
	pCPacket->m_HEADER.m_nSize = sizeof( zws_ACCOUNT_LIST );
	pCPacket->m_zws_ACCOUNT_LIST.m_nAccountCNT = 0;

	tagHASH<classUSER*> *pAccNODE;
#ifdef	__INC_WORLD
	tag_WLS_ACCOUNT	sInfo;
#else
	tag_ZWS_ACCOUNT sInfo;
#endif
	m_csHashACCOUNT.Lock ();
	for (int iL=0; iL<m_pHashACCOUNT->GetTableCount(); iL++) {
		pAccNODE = m_pHashACCOUNT->GetEntryNode( iL );
		while( pAccNODE ) {
			sInfo.m_dwLSID		= pAccNODE->m_DATA->m_dwLSID;
	#ifndef	__INC_WORLD
			sInfo.m_dwGSID		= pAccNODE->m_DATA->m_iSocketIDX;
	#endif
			sInfo.m_dwLoginTIME = pAccNODE->m_DATA->m_dwLoginTIME;
			pCPacket->AppendData( &sInfo, sizeof(tag_ZWS_ACCOUNT) );		// 12
			pCPacket->AppendString( pAccNODE->m_DATA->Get_ACCOUNT() );		// 31
			if ( bSendToGUMS ) {
				pCPacket->AppendString( pAccNODE->m_DATA->Get_MD5PW() );	// 33
				pCPacket->AppendString( pAccNODE->m_DATA->Get_IP() );		// 16
			} else
				pCPacket->AppendString( pAccNODE->m_DATA->Get_NAME() );

			pCPacket->m_wls_ACCOUNT_LIST.m_nAccountCNT ++;

			if ( pCPacket->m_HEADER.m_nSize >= MAX_PACKET_SIZE - 100 ) {
				pSrvSocket->Packet_Register2SendQ( pCPacket );
				Packet_ReleaseNUnlock( pCPacket );

				pCPacket = Packet_AllocNLock ();
				pCPacket->m_HEADER.m_wType = ZWS_ACCOUNT_LIST;
				pCPacket->m_HEADER.m_nSize = sizeof( zws_ACCOUNT_LIST );
				pCPacket->m_zws_ACCOUNT_LIST.m_nAccountCNT = 0;
			}

			pAccNODE = pAccNODE->m_NEXT;
		}
	}
	m_csHashACCOUNT.Unlock ();

	if ( 0 != pCPacket->m_zws_ACCOUNT_LIST.m_nAccountCNT ) {
		pSrvSocket->Packet_Register2SendQ( pCPacket );
	}

	Packet_ReleaseNUnlock( pCPacket );
}


//-------------------------------------------------------------------------------------------------
void CUserLIST::Send_cli_STRESS_TEST ( classPACKET *pCPacket )
{
	iocpSOCKET *pSocket;

	for (DWORD dwI=0; dwI<this->GetMaxSocketCOUNT(); dwI++) {
		pSocket = this->GetSOCKET( dwI );
		if ( pSocket ) {
			((classUSER*)pSocket)->SendPacket( pCPacket );
		}
	}
}

void CUserLIST::Save_AllUSER ()
{
	iocpSOCKET *pSocket;

	for (DWORD dwI=0; dwI<this->GetMaxSocketCOUNT(); dwI++) {
		pSocket = this->GetSOCKET( dwI );
		if ( pSocket ) {
			g_pThreadSQL->UpdateUserRECORD( (classUSER*)pSocket );
		}
	}
}

//-------------------------------------------------------------------------------------------------
void Save_AllUSER ()
{
	g_LOG.CS_ODS( 0xfff, ">>>>>>>>>>>>>>>>> Start Save_AllUSER()\n");
	g_pUserLIST->Save_AllUSER ();
	g_LOG.CS_ODS( 0xfff, "<<<<<<<<<<<<<<<<< End Save_AllUSER()\n");
}

