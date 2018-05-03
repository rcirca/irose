
#include "LIB_gsMAIN.h"
#include "GS_SocketLSV.h"
#include "GS_ListUSER.h"
#include "ZoneLIST.h"

bool GSLSV_Socket::WndPROC( WPARAM wParam, LPARAM lParam )
{
	return g_pSockLSV->Proc_SocketMSG( wParam, lParam );
}

//-------------------------------------------------------------------------------------------------
GS_lsvSOCKET::GS_lsvSOCKET() : m_TmpSTR(1024) //: CCriticalSection( 4000 )
{
	m_pRecvPket = (t_PACKET*) new char[ MAX_PACKET_SIZE ];
//	m_pSendPket = (t_PACKET*) new char[ MAX_PACKET_SIZE ];

	m_pReconnectTimer = NULL;
	m_bTryCONN = false;
}
GS_lsvSOCKET::~GS_lsvSOCKET ()
{
	SAFE_DELETE( m_pReconnectTimer );

	SAFE_DELETE_ARRAY( m_pRecvPket );
//	SAFE_DELETE_ARRAY( m_pSendPket );
}

//-------------------------------------------------------------------------------------------------
void GS_lsvSOCKET::Init (HWND hWND, char *szLoginServerIP, int iLoginServerPort, UINT uiSocketMSG)
{
	m_hMainWND = hWND;
	m_LoginServerIP.Set( szLoginServerIP );
	m_LoginServerPORT = iLoginServerPort;
	m_uiSocketMSG = uiSocketMSG;

	if ( NULL == m_pReconnectTimer )
		m_pReconnectTimer = new CTimer( m_hMainWND, GS_TIMER_LSV, RECONNECT_TIME_TICK, (TIMERPROC)GS_TimerProc );
}

//-------------------------------------------------------------------------------------------------
bool GS_lsvSOCKET::Connect()
{
	return m_SockLSV.Connect( m_hMainWND, m_LoginServerIP.Get(), m_LoginServerPORT, m_uiSocketMSG);
}
void GS_lsvSOCKET::Disconnect ()
{
	if ( m_pReconnectTimer )
		m_pReconnectTimer->Stop ();

//	this->Lock ();
	{
		m_SockLSV.Close ();
	}
//	this->Unlock ();
}


//-------------------------------------------------------------------------------------------------
void GS_lsvSOCKET::Send_zws_SERVER_INFO ()
{
	CLIB_GameSRV *pGSV = CLIB_GameSRV::GetInstance();

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
	pCPacket->m_HEADER.m_wType = ZWS_SERVER_INFO;
	pCPacket->m_HEADER.m_nSize = sizeof( zws_SERVER_INFO );

	pCPacket->m_zws_SERVER_INFO.m_wPortNO = (short)pGSV->GetListenPort();
	pCPacket->m_zws_SERVER_INFO.m_dwSeed  = pGSV->GetRandomSeed();

	pCPacket->m_zws_SERVER_INFO.m_Channel.m_btChannelNO	= (short)pGSV->GetChannelNO();
	pCPacket->m_zws_SERVER_INFO.m_Channel.m_btLowAGE	= pGSV->GetLowAGE ();
	pCPacket->m_zws_SERVER_INFO.m_Channel.m_btHighAGE	= pGSV->GetHighAGE ();

	pCPacket->AppendString( pGSV->GetServerName() );	// 채널이름이다!!!
	pCPacket->AppendString( pGSV->GetServerIP() );

	m_SockLSV.Packet_Register2SendQ( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	// 로그인 서버가 도중에 뻣얻을경우 대비...
	// 현재 접속되어 있는 사용자 리스트를 전송...
	g_pUserLIST->Send_zws_ACCOUNT_LIST( &this->m_SockLSV, false );
}


//-------------------------------------------------------------------------------------------------
void GS_lsvSOCKET::Send_srv_SET_WORLD_VAR (short nVarIDX, short nValue)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
	pCPacket->m_HEADER.m_wType = GSV_SET_WORLD_VAR;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_SET_WORLD_VAR );

	pCPacket->m_srv_SET_WORLD_VAR.m_nVarIDX = nVarIDX;
	pCPacket->m_srv_SET_WORLD_VAR.m_nValue[ 0 ] = nValue;

	m_SockLSV.Packet_Register2SendQ( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	
}
void GS_lsvSOCKET::Recv_srv_SET_WORLD_VAR ()
{
	if ( m_pRecvPket->m_srv_SET_WORLD_VAR.m_nVarIDX < 0 ) {
		// 전체 변수 업데이트...
		g_pZoneLIST->Reset_WorldVAR( (BYTE*)( m_pRecvPket->m_srv_SET_WORLD_VAR.m_nValue ) );
	} else {
		g_pZoneLIST->CWorldVAR::Set_WorldVAR( m_pRecvPket->m_srv_SET_WORLD_VAR.m_nVarIDX, m_pRecvPket->m_srv_SET_WORLD_VAR.m_nValue[ 0 ] );
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// 로그인/월드 서버로 계정 인증 요청...
void GS_lsvSOCKET::Send_zws_CONFIRM_ACCOUNT_REQ (DWORD dwSocketID, t_PACKET *pPacket)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
	// 로그인 서버에 GSV_ADD_USER_REQ 전송 한다...
	pCPacket->m_HEADER.m_wType = ZWS_CONFIRM_ACCOUNT_REQ;
	pCPacket->m_HEADER.m_nSize = sizeof( zws_CONFIRM_ACCOUNT_REQ );

	pCPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID	= pPacket->m_cli_JOIN_SERVER_REQ.m_dwID;
	pCPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID	= dwSocketID;

	::CopyMemory(pCPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwMD5Password, pPacket->m_cli_JOIN_SERVER_REQ.m_MD5Password, sizeof(DWORD)*8);
	// pCPacket->AppendString ( szAccount )

	m_SockLSV.Packet_Register2SendQ( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void GS_lsvSOCKET::Send_zws_SUB_ACCOUNT (DWORD dwLSID, char *szAccount)
{
	if ( !dwLSID )
		return;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
	pCPacket->m_HEADER.m_wType	= ZWS_SUB_ACCOUNT;
	pCPacket->m_HEADER.m_nSize	= sizeof( zws_SUB_ACCOUNT );
	pCPacket->AppendString( szAccount );

	m_SockLSV.Packet_Register2SendQ( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void GS_lsvSOCKET::Recv_lsv_CHECK_ALIVE ()
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
	pCPacket->m_HEADER.m_wType = GSV_CHECK_ALIVE;
	pCPacket->m_HEADER.m_nSize = sizeof( lsv_CHECK_GSV );

	m_SockLSV.Packet_Register2SendQ( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void GS_lsvSOCKET::Recv_wls_CONFIRM_ACCOUNT_REPLY ()
{
	short nOffset = sizeof( wls_CONFIRM_ACCOUNT_REPLY );

#ifndef	__INC_WORLD
	/*
	if ( m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwIngStatusFLAG ) {
		nOffset += ( sizeof(CIngSTATUS) - sizeof(DWORD) );
	}

	#ifndef	__SKIP_WS_PARTY
	if ( m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_wPartyWSID ) {
		// 소속 파티가 있다.
		if ( m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_HasPartyINFO ) {
			// 로컬 파티를 생성한다.
			tagWSPartyINFO *pParty;

			pParty = (tagWSPartyINFO*)Packet_GetDataPtr( m_pRecvPket, nOffset, sizeof( tagWSPartyINFO ) );

			pParty->m_cMemberCNT;
			pParty->m_btPartyLEV;
			pParty->m_iPartyEXP;

			nOffset += sizeof( tagWSPartyINFO );
		} else {
			// 로컬 파티에 추가...
		}
	}
	#endif
	*/
#else
	// 아래 두 라인 순서 주의 !!
	m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwGSID = m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwWSID;
	m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwWSID = m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwLSID;
#endif

	char *szAccount = Packet_GetStringPtr( m_pRecvPket, nOffset );
	if ( !g_pUserLIST->Add_ACCOUNT( m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwGSID, m_pRecvPket, szAccount ) ) {
		// 그새 접속이 끊겼는가 ???
		this->Send_zws_SUB_ACCOUNT( m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwWSID, szAccount );
	}
}

void GS_lsvSOCKET::Recv_lsv_ANNOUNCE_CHAT ()
{
	m_pRecvPket->m_HEADER.m_wType = GSV_ANNOUNCE_CHAT;
	g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( m_pRecvPket );
}

//-------------------------------------------------------------------------------------------------
// 다른 사용자에 의해 로그인됐다... 강제 종료 시킴...
void GS_lsvSOCKET::Recv_wls_KICK_ACCOUNT ()
{
	short nOffset   = sizeof( t_PACKETHEADER );
	char *szAccount = Packet_GetStringPtr (m_pRecvPket, nOffset);

	g_pUserLIST->Kick_ACCOUNT( szAccount );
}


/*
void GS_lsvSOCKET::Recv_lsv_CHECK_ACCOUNT ()
{
	short nOffset=sizeof( lsv_CHECK_ACCOUNT );
	char *szAccount = Packet_GetStringPtr( m_pRecvPket, nOffset );

	if ( !g_pUserLIST->Find_ACCOUNT( szAccount ) ) {
		// 못찾았으면 서버에 삭제 전송...
		this->Send_gsv_SUB_ACCOUNT( szAccount );
	}
}
*/

//-------------------------------------------------------------------------------------------------
void GS_lsvSOCKET::Send_gsv_CHEAT_REQ( classUSER *pUSER, DWORD dwReqWSID, DWORD dwReplyWSID, char *szCheatCode)
{
	// 월드 서버에 치트코드 요청
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
//	this->Lock ();
	{
		pCPacket->m_HEADER.m_wType = GSV_CHEAT_REQ;
		pCPacket->m_HEADER.m_nSize = sizeof( srv_CHEAT );

		pCPacket->m_gsv_CHEAT_REQ.m_dwReqUSER   = dwReqWSID;
		pCPacket->m_gsv_CHEAT_REQ.m_dwReplyUSER = dwReplyWSID;

		pCPacket->m_gsv_CHEAT_REQ.m_nZoneNO = pUSER->m_nZoneNO;
		pCPacket->m_gsv_CHEAT_REQ.m_PosCUR  = pUSER->m_PosCUR;

		pCPacket->AppendString( szCheatCode );

		m_SockLSV.Packet_Register2SendQ( pCPacket );
	}
//	this->Unlock ();
	Packet_ReleaseNUnlock( pCPacket );
	return;
}

//-------------------------------------------------------------------------------------------------
// 월드 서버가 다른 존 서버로 부터 받은 치트 코드를 요청했다.
void GS_lsvSOCKET::Recv_wsv_CHEAT_REQ ()
{
	short nOffset=sizeof( srv_CHEAT );
	char *szCode = ::Packet_GetStringPtr( m_pRecvPket, nOffset );

	char *pToken, *pArg1, *pArg2;
	char *pDelimiters = " ";

	pToken = m_TmpSTR.GetTokenFirst (szCode, pDelimiters);
	pArg1  = m_TmpSTR.GetTokenNext  (pDelimiters);
	if ( NULL == pToken )
		return;

	if ( pArg1 ) {
		if ( !strcmpi( pToken, "/add" ) ) {
			pArg2 = m_TmpSTR.GetTokenNext (pDelimiters);
			if ( !pArg2 )
				return;

			// 올려줄 대상이 있는가 ?
			classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReplyUSER );
			// pArg3 = m_TmpSTR.GetTokenNext (pDelimiters);
			// if ( !pArg3 ) 
			//	return;
			// classUSER *pUSER = g_pUserLIST->Find_CHAR( pArg3 );
			if ( !pUSER || !pUSER->GetZONE() )
				return;

			int iValue = atoi( pArg2 );

			if ( !strcmpi(pArg1, "exp") ) {
				if ( iValue < 0 ) iValue = 1;
				pUSER->Add_EXP( iValue, true, 0 );
			} else
			if ( !strcmpi(pArg1, "BP") ) {
				pUSER->AddCur_BonusPOINT( iValue );

				if ( pUSER->GetCur_BonusPOINT() < 0	   ) pUSER->SetCur_BonusPOINT( 0 );
				else
				if ( pUSER->GetCur_BonusPOINT() > 9999 ) pUSER->SetCur_BonusPOINT( 9999 );
			} else
			if ( !strcmpi(pArg1, "SP") ) {
				pUSER->AddCur_SkillPOINT( iValue );

				if ( pUSER->GetCur_SkillPOINT() < 0	   ) pUSER->SetCur_BonusPOINT( 0 );
				else
				if ( pUSER->GetCur_SkillPOINT() > 9999 ) pUSER->SetCur_BonusPOINT( 9999 );
			} else
			if ( !strcmpi(pArg1, "MONEY") ) {
				pUSER->Add_CurMONEY( iValue );
			} else
			if ( !strcmpi(pArg1, "SKILL") ) {	// 스킬을 배운것으로...
				if ( iValue >= 1 && iValue < g_SkillList.Get_SkillCNT() ) {
					if ( SKILL_ICON_NO( iValue ) ) {
						pUSER->Send_gsv_SKILL_LEARN_REPLY( iValue );
					}
				}
			}
		} else
		if ( !strcmpi( pToken, "/get" ) ) {
			classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReplyUSER );
			if ( pUSER ) {
				short nMovSpeed = (short)pUSER->Get_MoveSPEED();
				short nAtkSpeed = pUSER->Get_nAttackSPEED();
				m_TmpSTR.Printf ("/w %s>HP:%d/%d, MP:%d/%d, LEV:%d, EXP:%d, JOB:%d, SPD(M:%d,A:%d), Hit:%d, Crt:%d, AP:%d, DP:%d, AVD:%d, ATTR:%d, bCST:%d, SKL:%d, FLG:%x, Summon:%d/%d, BP:%d, SP:%d, \\:%I64d",
						pUSER->Get_NAME(), pUSER->Get_HP(), pUSER->Get_MaxHP(), pUSER->Get_MP(), pUSER->Get_MaxMP(),
						pUSER->Get_LEVEL(), pUSER->Get_EXP(), pUSER->Get_JOB(), nMovSpeed, nAtkSpeed,
						pUSER->Get_HIT(), pUSER->Get_CRITICAL(), pUSER->Get_ATK(), pUSER->Get_DEF(), pUSER->Get_AVOID(), 
						( pUSER->GetZONE() ) ? pUSER->GetZONE()->IsMovablePOS(pUSER->m_PosCUR.x, pUSER->m_PosCUR.y) : 0,
						pUSER->m_bCastingSTART, pUSER->Get_ActiveSKILL(), pUSER->m_IngSTATUS.GetFLAGs(),
						pUSER->Get_SummonCNT(),  pUSER->Max_SummonCNT(), pUSER->GetCur_BonusPOINT(), pUSER->GetCur_SkillPOINT(),  pUSER->Get_MONEY() );
				this->Send_gsv_CHEAT_REQ( pUSER, m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReqUSER, m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReplyUSER, m_TmpSTR.Get() );
			}
		}
	} else {
		if ( !strcmpi( pToken, "/call" ) ) {
			classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReplyUSER );
			if ( pUSER ) {
				pUSER->Send_gsv_RELAY_REQ( RELAY_TYPE_RECALL, 
							m_pRecvPket->m_wsv_CHEAT_REQ.m_nZoneNO,
							m_pRecvPket->m_wsv_CHEAT_REQ.m_PosCUR );
			}
		} else
		if ( !strcmpi( pToken, "/move" ) ) {
			classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReplyUSER );
			if ( pUSER && pUSER->GetZONE() ) {
				// pUSER의 좌표로 m_pRecvPket->m_wsv_CHEAT_REQ.m_dwWSID가 이동하려 한다.
				// 월드 서버에 pUSER의 존과 서버를 통보..
				this->Send_gsv_CHEAT_REQ( pUSER, m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReqUSER, m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReplyUSER, "/call_r" );
			}
		} else
		if ( !strcmpi( pToken, "/where" ) ) {
			classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReplyUSER );
			if ( pUSER && pUSER->GetZONE() ) {
				m_TmpSTR.Printf ("/w %s>%s:%d ( %.0f, %.0f )", 
						pUSER->Get_NAME(), 
						pUSER->GetZONE()->Get_NAME(), 
						pUSER->GetZONE()->Get_ZoneNO(), pUSER->m_PosCUR.x, pUSER->m_PosCUR.y );

				this->Send_gsv_CHEAT_REQ( pUSER, m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReqUSER, m_pRecvPket->m_wsv_CHEAT_REQ.m_dwReplyUSER, m_TmpSTR.Get() );
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void GS_lsvSOCKET::Send_srv_ACTIVE_MODE (bool bActive)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = SRV_ACTIVE_MODE;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_ACTIVE_MODE );

	pCPacket->m_srv_ACTIVE_MODE.m_bActive = bActive;

	m_SockLSV.Packet_Register2SendQ( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
}

void GS_lsvSOCKET::Send_srv_USER_LIMIT (DWORD dwLimit)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = SRV_USER_LIMIT;
	pCPacket->m_HEADER.m_nSize = sizeof( srv_USER_LIMIT);

	pCPacket->m_srv_USER_LIMIT.m_dwUserLIMIT = dwLimit;

	m_SockLSV.Packet_Register2SendQ( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void GS_lsvSOCKET::Send_wls_CHANNEL_LIST ()
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	CLIB_GameSRV *pGSV = CLIB_GameSRV::GetInstance();

	pCPacket->m_HEADER.m_wType = WLS_CHANNEL_LIST;
	pCPacket->m_HEADER.m_nSize = sizeof( wls_CHANNEL_LIST );

	pCPacket->m_wls_CHANNEL_LIST.m_btChannelCNT = 1;

	tagCHANNEL_SRV	sChannel = { 1, 0, 0, 0 };
	pCPacket->AppendData  ( &sChannel, sizeof( tagCHANNEL_SRV ) );
	pCPacket->AppendString( pGSV->GetServerName() );

	m_SockLSV.Packet_Register2SendQ( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
/*
bool GS_lsvSOCKET::Send_gsv_WARP_USER( classUSER *pUSER )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return true;
	{
		pCPacket->m_HEADER.m_wType = GSV_WARP_USER;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_WARP_USER );

		pCPacket->m_gsv_WARP_USER.m_dwWSID     = pUSER->m_dwWSID;
		pCPacket->m_gsv_WARP_USER.m_nZoneNO    = pUSER->m_nZoneNO;
		pCPacket->m_gsv_WARP_USER.m_btRunMODE  = pUSER->m_bRunMODE;
		pCPacket->m_gsv_WARP_USER.m_btRideMODE = pUSER->m_btRideMODE;		// 04.04.21

//		pCPacket->m_gsv_WARP_USER.m_PosWARP = pUSER->m_PosCUR;
		if ( pUSER->m_IngSTATUS.GetFLAGs() ) {
			::CopyMemory( pCPacket->m_gsv_WARP_USER.m_IngSTATUS, &pUSER->m_IngSTATUS, sizeof(CIngSTATUS) );
			pCPacket->m_HEADER.m_nSize += ( sizeof(CIngSTATUS)-sizeof(DWORD) );
		} else
			pCPacket->m_gsv_WARP_USER.m_dwIngStatusFLAG = 0;

		m_SockLSV.Packet_Register2SendQ( pCPacket );
	}
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}
*/

//-------------------------------------------------------------------------------------------------
bool GS_lsvSOCKET::Send_gsv_CHANGE_CHAR( classUSER *pUSER )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return true;
#ifdef	__INC_WORLD
	pCPacket->m_HEADER.m_wType = WSV_CHAR_CHANGE;
	pCPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER );

	pUSER->SendPacket( pCPacket);
#else
	pCPacket->m_HEADER.m_wType = GSV_CHANGE_CHAR;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHANGE_CHAR );

	pCPacket->AppendString( pUSER->Get_ACCOUNT() );
	//pCPacket->m_gsv_CHANGE_CHAR.m_dwWSID		= pUSER->m_dwWSID;
	//pCPacket->m_gsv_CHANGE_CHAR.m_HashACCOUNT	= pUSER->m_HashACCOUNT;

	m_SockLSV.Packet_Register2SendQ( pCPacket );
#endif

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_lsvSOCKET::Send_zws_CREATE_CLAN( DWORD dwWSID, t_HASHKEY HashCHAR )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = ZWS_CREATE_CLAN;
	pCPacket->m_HEADER.m_nSize = sizeof( zws_CREATE_CLAN );

	pCPacket->m_zws_CREATE_CLAN.m_dwGSID = dwWSID;
	pCPacket->m_zws_CREATE_CLAN.m_HashCHAR = HashCHAR;

	m_SockLSV.Packet_Register2SendQ( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_lsvSOCKET::Send_gsv_ADJ_CLAN_VAR( t_PACKET *pPacket )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	::CopyMemory(pCPacket->m_pDATA, pPacket->m_pDATA, pPacket->m_HEADER.m_nSize );
	m_SockLSV.Packet_Register2SendQ( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_lsvSOCKET::Proc_SocketMSG (WPARAM wParam, LPARAM lParam)
{
	int nErrorCode = WSAGETSELECTERROR(lParam);
	switch ( WSAGETSELECTEVENT(lParam) ) {
		case FD_READ:
		{
			m_SockLSV.OnReceive( nErrorCode );

			// 받은 패킷 처리..
			while( m_SockLSV.Peek_Packet( m_pRecvPket, true ) ) {
				// LogString( LOG_DEBUG, "Handle LS Packet: Type[ 0x%x ], Size[ %d ]\n", m_pRecvPket->m_HEADER.m_wType, m_pRecvPket->m_HEADER.m_nSize);
				switch( m_pRecvPket->m_HEADER.m_wType ) {
					case LSV_CHECK_ALIVE :
						Recv_lsv_CHECK_ALIVE ();
						break;

					case WLS_CONFIRM_ACCOUNT_REPLY :
						Recv_wls_CONFIRM_ACCOUNT_REPLY();
						break;

					case LSV_ANNOUNCE_CHAT :
						Recv_lsv_ANNOUNCE_CHAT ();
						break;

					case WLS_KICK_ACCOUNT :
						Recv_wls_KICK_ACCOUNT ();
						break;

					case WSV_SET_WORLD_VAR :
						Recv_srv_SET_WORLD_VAR ();
						break;

					case WSV_CHEAT_REQ :		// 치트 코드 파싱해서 결과 리턴...
						Recv_wsv_CHEAT_REQ ();
						break;

					case ZWS_CREATE_CLAN   :
					case ZWS_DEL_USER_CLAN :
					case ZWS_SET_USER_CLAN :
					{
						classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( m_pRecvPket->m_zws_DEL_USER_CLAN.m_dwGSID );
						if ( pUSER && ( 0 == pUSER->m_HashCHAR || pUSER->m_HashCHAR == m_pRecvPket->m_zws_DEL_USER_CLAN.m_HashCHAR ) ) {
							// 0 == pUSER->m_HashCHAR 일경우는 케릭이 디비에서 올라오는 중이다...
							pUSER->Add_SrvRecvPacket( m_pRecvPket );
						}
						break;
					}

				/*
					case WSV_DEL_ZONE :
						// TODO:: 현재 실행중인 존을 삭제하란다...
						// 삭제하지 않고 걍두면 서버 속도 딸리겠지...
						break;

					case WSV_PARTY_CMD :
						Recv_wsv_PARTY_CMD ();
						break;
				*/
					default :
						g_LOG.CS_ODS( 0xffff, "Undefined World server packet... Type[ 0x%x ], Size[ %d ]\n", m_pRecvPket->m_HEADER.m_wType, m_pRecvPket->m_HEADER.m_nSize);
				}
			}
			break;
		}
		case FD_WRITE:
			m_SockLSV.OnSend ( nErrorCode );
			break;
		/*
		case FD_OOB:
			m_SockLSV.OnOutOfBandData ( nErrorCode );
			break;
		case FD_ACCEPT:
			m_SockLSV.OnAccept ( nErrorCode );
			break;
		*/
		case FD_CONNECT: 
		{
			m_SockLSV.OnConnect ( nErrorCode );
			if ( !nErrorCode ) {
				#ifdef	__INC_WORLD
					g_LOG.CS_ODS( 0xffff, "Connected to LOGIN server \n");
				#else
					g_LOG.CS_ODS( 0xffff, "Connected to WORLD server \n");
				#endif
				m_bTryCONN = false;

				if ( m_pReconnectTimer )
					m_pReconnectTimer->Stop ();
				this->Send_zws_SERVER_INFO ();

				#ifdef	__INC_WORLD
				// 월드 서버인양 로그인 서버에 채널 리스트 전송...
				this->Send_wls_CHANNEL_LIST ();
				#endif

				this->Send_srv_ACTIVE_MODE( true );
				CLIB_GameSRV *pGSV = CLIB_GameSRV::GetInstance();
				if ( pGSV ) 
					this->Send_srv_USER_LIMIT( pGSV->Get_UserLIMIT() );
 			} else {
				if ( m_pReconnectTimer )
					m_pReconnectTimer->Start ();
				if ( !m_bTryCONN ) {
					m_bTryCONN = true;
					#ifdef	__INC_WORLD
						g_LOG.CS_ODS( 0xffff, "Connect failed to LOGIN server \n");
					#else
						g_LOG.CS_ODS( 0xffff, "Connect failed to WORLD server \n");
					#endif
				}
			}
			break;
		}
		case FD_CLOSE:		// Close()함수를 호출해서 종료될때는 발생 안한다.
		{
			m_SockLSV.OnClose ( nErrorCode );

		#ifdef	__INC_WORLD
			g_LOG.CS_ODS( 0xffff, "Disconnected form LOGIN server \n");
		#else
			g_LOG.CS_ODS( 0xffff, "Disconnected from WORLD server \n");
		#endif

			if ( m_pReconnectTimer )
				m_pReconnectTimer->Start ();
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
