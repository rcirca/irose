/*
	$Header: /7HeartsOnline/Server/SHO_GS/Sho_gs_lib/srv_COMMON/CDB_Socket.cpp 2     04-07-19 2:42p Icarus $
	$History: CDB_Socket.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-07-19   Time: 2:42p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib/srv_COMMON
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:20a
 * Created in $/7HeartsOnline/Server/Common
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:47a
 * Created in $/SevenHearts/Server/Common
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:35a
 * Created in $/SHO/Server/Common
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:26a
 * Created in $/SevenHearts/Server/SHO_WS_LIB/Server/Common
*/
#include "stdAFX.h"
#include "CDB_Socket.h"

//-------------------------------------------------------------------------------------------------
CDB_Socket::CDB_Socket (bool bUseMySQL)
{
	m_pReconnectTimer = NULL;
	m_pRecvPket = (t_PACKET*) new char[ MAX_PACKET_SIZE ];

	if ( bUseMySQL )
		m_pSQL = new classMYSQL(32, MAX_PACKET_SIZE);
	else
		m_pSQL = new classODBC(32, MAX_PACKET_SIZE);
}
CDB_Socket::~CDB_Socket ()
{
	SAFE_DELETE( m_pSQL );
	SAFE_DELETE( m_pReconnectTimer );
	SAFE_DELETE_ARRAY( m_pRecvPket );
}

//-------------------------------------------------------------------------------------------------
void CDB_Socket::Init (HWND hWND, char *szLogServerIP, int iLogServerPort, UINT uiSocketMSG, char *szDBName, char *szDBAccount ,char *szDBPassword, 
						UINT_PTR uiTimerEventID, UINT uiTimerElapse, TIMERPROC lpTimerFunc)
{
	m_hMainWND = hWND;
	m_LogServerIP.Set( szLogServerIP );
	m_LogServerPORT = iLogServerPort;
	m_uiSocketMSG = uiSocketMSG;

	m_LogDBName.Set( szDBName );
	m_LogDBAccount.Set( szDBAccount );
	m_LogDBPassword.Set( szDBPassword );

	if ( NULL == m_pReconnectTimer )
		m_pReconnectTimer = new CTimer( m_hMainWND, uiTimerEventID, uiTimerElapse, lpTimerFunc );
}

//-------------------------------------------------------------------------------------------------
bool CDB_Socket::Connect ()
{
	return CClientSOCKET::Connect( m_hMainWND, m_LogServerIP.Get(), m_LogServerPORT, m_uiSocketMSG);
}

void CDB_Socket::Disconnect ()
{
	if ( m_pReconnectTimer )
		m_pReconnectTimer->Stop ();

	this->Close ();
}


//-------------------------------------------------------------------------------------------------
void CDB_Socket::Send_gsv_LOG_SQL()
{
	m_pQueryBUFF = this->m_pSQL->GetQueryBuffPTR ();

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = GSV_LOG_DB_SQL;
	pCPacket->m_HEADER.m_nSize = sizeof( t_PACKETHEADER ) + m_dwBuffLEN;

	::CopyMemory( pCPacket->m_gsv_LOG_SQL.m_btQUERY, m_pQueryBUFF, m_dwBuffLEN );
	this->Packet_Register2SendQ( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
bool CDB_Socket::Send_gsv_LOG_DB_INFO ()
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return true;
	m_csSQL.Lock ();
	{
		pCPacket->m_HEADER.m_wType = GSV_LOG_DB_INFO;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_LOG_DB_INFO );

		pCPacket->AppendString( this->GetServerNAME() );	// CLIB_GameSRV::GetInstance()->GetServerName() );
		pCPacket->AppendString( m_LogServerIP.Get()	);
//		pCPacket->AppendString( "192.168.0.112" ); // m_LogServerIP.Get()	);
		pCPacket->AppendString( m_LogDBName.Get()		);
		pCPacket->AppendString( m_LogDBAccount.Get()	);
		pCPacket->AppendString( m_LogDBPassword.Get() );

		this->Packet_Register2SendQ( pCPacket );
	}
	m_csSQL.Unlock ();
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CDB_Socket::WndPROC ( WPARAM wParam, LPARAM lParam )
{
	int nErrorCode = WSAGETSELECTERROR(lParam);
	switch ( WSAGETSELECTEVENT(lParam) ) {
		case FD_READ:
		{
			this->OnReceive( nErrorCode );

			// 받은 패킷 처리..
			while( this->Peek_Packet( m_pRecvPket, true ) ) {
				LogString( LOG_DEBUG, "Handle LS Packet: Type[ 0x%x ], Size[ %d ]\n", m_pRecvPket->m_HEADER.m_wType, m_pRecvPket->m_HEADER.m_nSize);
			}
			break;
		}

		case FD_WRITE:
			this->OnSend ( nErrorCode );
			break;

		case FD_OOB:
			this->OnOutOfBandData ( nErrorCode );
			break;

		case FD_CONNECT: 
		{
			this->OnConnect ( nErrorCode );
			if ( !nErrorCode ) {
				g_LOG.CS_ODS( 0xffff, "Connected to db_agent server \n");

				this->Send_gsv_LOG_DB_INFO ();

				if ( m_pReconnectTimer )
					m_pReconnectTimer->Stop ();
			} else {
				if ( m_pReconnectTimer )
					m_pReconnectTimer->Start ();
				g_LOG.CS_ODS( 0xffff, "Connect failed to db_agent server \n");
			}
			break;
		}
		case FD_CLOSE:		// Close()함수를 호출해서 종료될때는 발생 안한다.
		{
			this->OnClose ( nErrorCode );

			if ( m_pReconnectTimer )
				m_pReconnectTimer->Start ();
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
