
#include "stdAFX.h"
#include "SHO_WS_LIB.h"
#include "WS_SocketLSV.h"
#include "CWS_Client.h"
#include "CWS_Server.h"


//-------------------------------------------------------------------------------------------------
bool WSLSV_Socket::WndPROC( WPARAM wParam, LPARAM lParam )
{
	return g_pSockLSV->Proc_SocketMSG( wParam, lParam );
}

//-------------------------------------------------------------------------------------------------
WS_lsvSOCKET::WS_lsvSOCKET() //: CCriticalSection( 4000 )
{
	m_pRecvPket = (t_PACKET*) new char[ MAX_PACKET_SIZE ];

	m_bActiveMODE = false;
	m_pReconnectTimer = NULL;
	m_bTryCONN = false;
}
WS_lsvSOCKET::~WS_lsvSOCKET ()
{
	SAFE_DELETE( m_pReconnectTimer );

	SAFE_DELETE_ARRAY( m_pRecvPket );
}

//-------------------------------------------------------------------------------------------------
void WS_lsvSOCKET::Init (HWND hWND, char *szLoginServerIP, int iLoginServerPort, UINT uiSocketMSG, bool bAutoActive)
{
	m_bActiveMODE = bAutoActive;
	m_hMainWND = hWND;
	m_LoginServerIP.Set( szLoginServerIP );
	m_LoginServerPORT = iLoginServerPort;
	m_uiSocketMSG = uiSocketMSG;

	if ( NULL == m_pReconnectTimer )
		m_pReconnectTimer = new CTimer( m_hMainWND, WS_TIMER_LSV, RECONNECT_TIME_TICK, (TIMERPROC)WS_TimerProc );
}

//-------------------------------------------------------------------------------------------------
bool WS_lsvSOCKET::Connect()
{
	return m_SockLSV.Connect( m_hMainWND, m_LoginServerIP.Get(), m_LoginServerPORT, m_uiSocketMSG);
}
void WS_lsvSOCKET::Disconnect ()
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
void WS_lsvSOCKET::Send_srv_ACTIVE_MODE (bool bActive)
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

//-------------------------------------------------------------------------------------------------
void WS_lsvSOCKET::Send_zws_SERVER_INFO ()
{
	SHO_WS *pGSV = SHO_WS::GetInstance();

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
//	this->Lock ();
	{
		pCPacket->m_HEADER.m_wType = ZWS_SERVER_INFO;
		pCPacket->m_HEADER.m_nSize = sizeof( zws_SERVER_INFO );

		pCPacket->m_zws_SERVER_INFO.m_wPortNO = (short)pGSV->GetListenPort();
		pCPacket->m_zws_SERVER_INFO.m_dwSeed  = pGSV->GetRandomSeed();
/*
		로그인 서버에는 전송할 필요 없다.
		pCPacket->m_zws_SERVER_INFO.m_Channel.m_btChannelNO	= (short)pGSV->GetChannelNO();
		pCPacket->m_zws_SERVER_INFO.m_Channel.m_btLowAGE	= pGSV->GetLowAGE ();
		pCPacket->m_zws_SERVER_INFO.m_Channel.m_btHighAGE	= pGSV->SetHighAGE ();
*/
		pCPacket->AppendString( pGSV->GetServerName() );
		pCPacket->AppendString( pGSV->GetServerIP() );

		m_SockLSV.Packet_Register2SendQ( pCPacket );

		// 로그인 서버가 도중에 뻣얻을경우 대비...
		// 현재 접속되어 있는 사용자 리스트를 전송...
	}
//	this->Unlock ();
	Packet_ReleaseNUnlock( pCPacket );

	this->Send_wls_ACCOUNT_LIST ();
	this->Send_wls_CHANNEL_LIST ();
}

//-------------------------------------------------------------------------------------------------
void WS_lsvSOCKET::Send_wls_CHANNEL_LIST ()
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	// 로그인 서버에 채널 서버 정보 전송.
	g_pListSERVER->Make_wls_CHANNEL_LIST( pCPacket );
	m_SockLSV.Packet_Register2SendQ( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
}



//-------------------------------------------------------------------------------------------------
void WS_lsvSOCKET::Send_zws_CONFIRM_ACCOUNT_REQ (DWORD dwSocketID, t_PACKET *pPacket)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
//	this->Lock ();
	{
		// 로그인 서버에 GSV_ADD_USER_REQ 전송 한다...
		pCPacket->m_HEADER.m_wType = ZWS_CONFIRM_ACCOUNT_REQ;
		pCPacket->m_HEADER.m_nSize = sizeof( zws_CONFIRM_ACCOUNT_REQ );

		pCPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwServerID	= pPacket->m_cli_JOIN_SERVER_REQ.m_dwID;
		pCPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwClientID	= dwSocketID;

		::CopyMemory(pCPacket->m_zws_CONFIRM_ACCOUNT_REQ.m_dwMD5Password, pPacket->m_cli_JOIN_SERVER_REQ.m_MD5Password, sizeof(DWORD)*8);
		// pCPacket->AppendString ( szAccount ) szAccont 구할수 없다.

		m_SockLSV.Packet_Register2SendQ( pCPacket );
	}
//	this->Unlock ();
	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void WS_lsvSOCKET::Send_zws_SUB_ACCOUNT (DWORD dwLSID, char *szAccount)
{
	//if ( !dwLSID )	LS에서 삭제요청된 계정을 WS가 못찾을경우 dwLSID = 0이다.
	//	return;

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
void WS_lsvSOCKET::Send_wls_ACCOUNT_LIST ()
{
	// 현재 접속되어 있는 모든 계정을 로그인 서버에 보낸다.
	g_pUserLIST->Send_wls_ACCOUNT_LIST ();
}

//-------------------------------------------------------------------------------------------------
void WS_lsvSOCKET::Recv_lsv_CHECK_ALIVE ()
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
//	this->Lock ();
	{
		pCPacket->m_HEADER.m_wType = GSV_CHECK_ALIVE;
		pCPacket->m_HEADER.m_nSize = sizeof( lsv_CHECK_GSV );

		m_SockLSV.Packet_Register2SendQ( pCPacket );
	}
//	this->Unlock ();
	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void WS_lsvSOCKET::Recv_wls_CONFIRM_ACCOUNT_REPLY ()
{
	// 로그인 서버로 부터 응답을 받았다.
	short nOffset=sizeof( wls_CONFIRM_ACCOUNT_REPLY );
	char *szAccount = Packet_GetStringPtr( m_pRecvPket, nOffset );

	g_pUserLIST->Add_ACCOUNT( m_pRecvPket->m_wls_CONFIRM_ACCOUNT_REPLY.m_dwWSID, m_pRecvPket, szAccount );
}

void WS_lsvSOCKET::Recv_lsv_ANNOUNCE_CHAT ()
{
/*
	m_pRecvPket->m_HEADER.m_wType = GSV_ANNOUNCE_CHAT;
	g_pZoneLIST->Send_wsv_ANNOUNCE_CHAT( m_pRecvPket );
*/
}

//-------------------------------------------------------------------------------------------------
// 다른 사용자에 의해 로그인됐다... 강제 종료 시킴...
void WS_lsvSOCKET::Recv_wls_KICK_ACCOUNT ()
{
	short nOffset   = sizeof( t_PACKETHEADER );
	char *szAccount = Packet_GetStringPtr (m_pRecvPket, nOffset);

	CWS_Client *pUSER = (CWS_Client*)g_pUserLIST->Find_ACCOUNT( szAccount );
	if ( pUSER ) {
		// 월드서버에 소켓이 살아 있다..
		pUSER->CloseSocket ();
	} else {
		// 모든 채널 서버에 짤라라 전송...
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			pCPacket->m_HEADER.m_wType = WLS_KICK_ACCOUNT;
			pCPacket->m_HEADER.m_nSize = sizeof( wls_KICK_ACCOUNT );
			pCPacket->AppendString( szAccount );

			g_pListSERVER->Send_ToWORLD( pCPacket );

			Packet_ReleaseNUnlock( pCPacket );
		}

		// 채널서버가 정보전송없이 죽었을경우... 월드서버 비트는 삭제되고 채널서버 비트는 살아있을수 있다.
		// 계정이 없거나 계정의 pAccount->m_pCLIENT==NULL이다...즉 채널서버에서 소켓 종료됐다.
		g_pUserLIST->Del_ACCOUNT( szAccount, BIT_LOGIN_GS );
	}
}

//-------------------------------------------------------------------------------------------------
bool WS_lsvSOCKET::Proc_SocketMSG (WPARAM wParam, LPARAM lParam)
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
						Recv_wls_CONFIRM_ACCOUNT_REPLY ();
						break;

					case LSV_ANNOUNCE_CHAT :
						Recv_lsv_ANNOUNCE_CHAT ();
						break;
					case WLS_KICK_ACCOUNT :
						Recv_wls_KICK_ACCOUNT ();
						break;

					default :
						g_LOG.CS_ODS( 0xffff, "Undefined Login server packet... Type[ 0x%x ], Size[ %d ]\n", m_pRecvPket->m_HEADER.m_wType, m_pRecvPket->m_HEADER.m_nSize);
				}
			}
			break;
		}
		case FD_WRITE:
			m_SockLSV.OnSend ( nErrorCode );
			break;
		case FD_OOB:
			m_SockLSV.OnOutOfBandData ( nErrorCode );
			break;
		/*
		case FD_ACCEPT:
			m_SockLSV.OnAccept ( nErrorCode );
			break;
		*/
		case FD_CONNECT: 
		{
			m_SockLSV.OnConnect ( nErrorCode );
			if ( !nErrorCode ) {
				g_LOG.CS_ODS( 0xffff, "Connected to login server \n");
				m_bTryCONN = false;

				if ( m_pReconnectTimer )
					m_pReconnectTimer->Stop ();
				this->Send_zws_SERVER_INFO ();

				if ( m_bActiveMODE )
					this->Send_srv_ACTIVE_MODE( true );
			} else {
				if ( m_pReconnectTimer )
					m_pReconnectTimer->Start ();

				if ( !m_bTryCONN ) {
					m_bTryCONN = true;
					g_LOG.CS_ODS( 0xffff, "Connect failed to login server\n");
				}
			}
			break;
		}
		case FD_CLOSE:		// Close()함수를 호출해서 종료될때는 발생 안한다.
		{
			m_SockLSV.OnClose ( nErrorCode );

			g_LOG.CS_ODS( 0xffff, "Disconnected from login server\n");

			if ( m_pReconnectTimer )
				m_pReconnectTimer->Start ();
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
