
#include <winsock2.h>
#include <windows.h>
#include <assert.h>
#include <mstcpip.h>
#include "CSocket.h"
#include "classLOG.h"
#include "CAcceptTHREAD.h"

//-------------------------------------------------------------------------------------------------
CAcceptTHREAD::CAcceptTHREAD (bool bCreateSuspended) : classTHREAD( bCreateSuspended )
{
	;
}

//-------------------------------------------------------------------------------------------------
void CAcceptTHREAD::SocketERROR ()
{
    Socket_Error("CAcceptTHREAD::Execute");
}

void CAcceptTHREAD::Execute ()
{
    SOCKET      ClientSocket = INVALID_SOCKET;
    SOCKADDR_IN SockADDR;
    int         iAddrLEN;

	g_LOG.CS_ODS( 0xffff, ">  > >> CAcceptTHREAD::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

//    Synchronize( LogSTART );

    // m_ListenSocket이 INVALID_SOCKET이 되면 루프를 탈출한다.
    while ( INVALID_SOCKET != m_ListenSocket && false == this->Terminated )
    {
		iAddrLEN = sizeof( SockADDR );
        ClientSocket = ::accept (m_ListenSocket, (sockaddr*)&SockADDR, &iAddrLEN);
        if ( INVALID_SOCKET == ClientSocket ) {
            // If user hits Ctrl+C or Ctrl+Brk or console window is closed, the control
            // handler will close the g_sdListen socket. The above WSAAccept call will
            // fail and we thus break out the loop,
            g_LOG.CS_ODS (0xffff, "Accept return INVALID_SOCKET, LastERROR: %d(0x%x)\n", WSAGetLastError(), WSAGetLastError() );

#ifdef  __BORLANDC__
            // BCB 경우 WriteLOG에서 DeadLock 걸린다.
            Synchronize( SocketERROR );
#else
			Socket_Error("CAcceptTHREAD::Execute");
#endif
			break; //continue;	// break;
        }

        if ( !this->AcceptSOCKET( ClientSocket, SockADDR ) ) {
            // 더이상 받을수 없거나 블럭된 IP다.
            struct linger li = {0, 0};	// Default: SO_DONTLINGER

            ::shutdown   (ClientSocket, SD_BOTH);
            ::setsockopt (ClientSocket, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li));
            ::closesocket(ClientSocket);
        }
    }

	g_LOG.CS_ODS( 0xffff, "<  < << CAcceptTHREAD::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

//    Synchronize( LogSTOP );
}

//---------------------------------------------------------------------------
bool CAcceptTHREAD::Init (int iTCPPort, int iKeepAliveSec)
{
	//----------------------- Accept socket 생성
	int     iRet;
    WSADATA	wsaData;

	m_ListenSocket  = INVALID_SOCKET;

	if ( (iRet = ::WSAStartup(MAKEWORD(2,2), &wsaData)) != 0 ) {
		Log_String(LOG_NORMAL, "WSAStartup failed: %d\n",iRet);
        return false;
    }

   // Create a listening socket
	m_ListenSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
   if ( m_ListenSocket == INVALID_SOCKET ) {
      Log_String(LOG_NORMAL, "WSASocket() failed with error %d\n", WSAGetLastError());
      return false;
   }

   SOCKADDR_IN sSockAddr;

   sSockAddr.sin_family		 = AF_INET;
   sSockAddr.sin_port		 = ::htons( iTCPPort );
   sSockAddr.sin_addr.s_addr = ::htonl( INADDR_ANY );

	iRet = ::bind (m_ListenSocket, (SOCKADDR*)&sSockAddr, sizeof(SOCKADDR_IN));
	if ( iRet == SOCKET_ERROR ) {
		Log_String (LOG_NORMAL, "bind failed with error :: %d", WSAGetLastError());
		return false;
	}
/*
	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	int iOptVal;
	int iOptLen = sizeof(int);
	if ( setsockopt(m_ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bOptVal, bOptLen) == SOCKET_ERROR) {
		assert("error:: Set SO_KEEPALIVE: ON");
	}
	if ( getsockopt(m_ListenSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&iOptVal, &iOptLen) == SOCKET_ERROR) {
		assert("error:: Get SO_KEEPALIVE: ON");
	}

	//*KeepAliveTime = milliseconds 
	//	Data Type: DWORD 
	//	For Windows 98, the Data Type is a String Value. 
	//	Specifies the connection idle time in milliseconds before TCP will begin sending keepalives, if keepalives are enabled on a connection. The default is 2 hours (7,200,000).

	//*KeepAliveInterval = 32-bit number 
	//	Data Type: DWORD
	//	For Windows 98, the Data Type is a String value. 
	//	Specifies the time in milliseconds between retransmissions of keepalives, once the KeepAliveTime has expired. Once KeepAliveTime has expired, keepalives are sent every KeepAliveInterval milliseconds until a response is received, up to a maximum of MaxDataRetries before the connection is terminated. The default is 1 second (1000).

	//*MaxDataRetries = 32-bit number 
	//	Data Type: String 
	//	Specifies the maximum number of times a segment carrying data or an FIN will be retransmitted before the connection is terminated. The retransmission timeout itself is adaptive and will vary according to link conditions. The default is 5.

	// MS사는 300,000(5분)으로 설정하도록 권장하고 있다.
	#define IOCP_KEEPALIVE_TIME			300000
	#define IOCP_KEEPALIVE_INTERVAL		3000	//응답이 없을시 다시 보낼 시간 간격 3초(기본 5회 시도후 짤름)

	tcp_keepalive	KeepAlive;
	KeepAlive.onoff				= TRUE;
	KeepAlive.keepalivetime		= iKeepAliveSec * 1000;//IOCP_KEEPALIVE_TIME;
	KeepAlive.keepaliveinterval	= IOCP_KEEPALIVE_INTERVAL;

	DWORD			dwBytesRecvd;
	::WSAIoctl( m_ListenSocket, SIO_KEEPALIVE_VALS, &KeepAlive, sizeof( KeepAlive ), 0, 0, &dwBytesRecvd, NULL, NULL );
*/
	// Prepare socket for listening
	iRet  = ::listen(m_ListenSocket, 5);
	if ( iRet == SOCKET_ERROR ) {
		m_ListenSocket = INVALID_SOCKET;
		Log_String(LOG_NORMAL, "listen() failed with error %d\n", WSAGetLastError());
		return false;
	}
    
    return true;
}

//---------------------------------------------------------------------------
void CAcceptTHREAD::Free (void)
{
    this->Terminate ();

    if ( m_ListenSocket != INVALID_SOCKET ) {
        ::closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
    }

	// 2003. 11. 05
	// 종료시 wait기능 추가... wait없이 종료한후
	// 바로 delete thread되면 socket list와 꼬임...
	while ( !this->IsFinished () ) {
		::Sleep( 10 );
	}

    ::WSACleanup ();

    // ThreadWORKER와 달리 m_ListenSocket를 공유하지 않으므로
    // 본 함수가 호출된후에는 쓰레드가 종료된다.
}


//-------------------------------------------------------------------------------------------------
/*
bool CAcceptTHREAD::AcceptSOCKET (SOCKET hSocket, sockaddr_in &SockADDR)
{
	classDLLNODE<classUSER> *pNode;

    pNode = g_CUserLIST.AddUser (ClientSocket);
    if ( pNode == NULL ) {
        // 사용자 리스트에 더이상 등록할수 없다..
        ::closesocket (ClientSocket);
    } else
    if ( !pServer->_AddSocket (ClientSocket, (DWORD)&pNode->DATA) ) {
        ::closesocket (ClientSocket);
        g_CUserLIST.SubUser (pNode);	// AcceptThreadFunc
    } else {
        pNode->DATA.Recv_Start ();		// AcceptThreadFunc
    }

    return false;
}
*/
//-------------------------------------------------------------------------------------------------
