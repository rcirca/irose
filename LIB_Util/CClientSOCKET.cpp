/*
	$Header: /7HeartsOnline/LIB_Util/CClientSOCKET.cpp 4     04-10-22 11:14a Icarus $
*/

#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include <process.h>
#include "classLOG.h"
#include "CClientSOCKET.h"

#define CLIENTSOCKET_SERVERDEAD		(-1)
#define CLIENTSOCKET_DISCONNECTED	0x000
#define CLIENTSOCKET_CONNECTED		0x001
#define CLIENTSOCKET_CLOSING		0x002


struct t_PACKET {
	union {
		t_PACKETHEADER				m_HEADER;
		BYTE						m_pDATA[ 1 ];
	}  ;
} ;

struct t_SendPACKET {
	WORD			m_wSize;
	union {
		t_PACKET	m_Packet;
		BYTE		m_pDATA[ MAX_PACKET_SIZE ];
	} ;
} ;

//-------------------------------------------------------------------------------------------------
//DWORD WINAPI ClientSOCKET_SendTHREAD (LPVOID lpParameter)
unsigned __stdcall ClientSOCKET_SendTHREAD( void* lpParameter )
{
	CClientSOCKET *pClientSocket = (CClientSOCKET *) lpParameter;

	LogString (LOG_DEBUG, "\n\n [ 0x%x ] >>>>> ClientSocket_Thread::0x%x\n\n\n", pClientSocket, pClientSocket->m_dwThreadID);

	while ( 1 ) 
	{
		WaitForSingleObject( pClientSocket->m_hThreadEvent, INFINITE ); 

		if ( pClientSocket->m_cStatus != CLIENTSOCKET_CONNECTED ) 
			break;

		::EnterCriticalSection( &pClientSocket->m_csThread );
		pClientSocket->m_SendPacketQ.AppendNodeList( &pClientSocket->m_WaitPacketQ );
		pClientSocket->m_WaitPacketQ.Init ();
        ::LeaveCriticalSection( &pClientSocket->m_csThread );

		if ( pClientSocket->m_bWritable && pClientSocket->m_SendPacketQ.GetNodeCount() > 0 ) {
			if ( !pClientSocket->Packet_Send () ) {
				// 소켓 오류 발생...
				#pragma message ( "!!!!!! >>>> 소켓 오류 발생시 대처...." )
			}
		} 
#ifdef	_DEBUG
		else
			g_LOG.OutputString( LOG_DEBUG, " >>>>>>>> Send Failed[ %d Packet(s) ] :: Writable == false \n", pClientSocket->m_SendPacketQ.GetNodeCount() );
#endif
    }

    pClientSocket->m_cStatus = CLIENTSOCKET_DISCONNECTED;

    return (0);
}

//-------------------------------------------------------------------------------------------------
void  CClientSOCKET::mF_Init (DWORD dwInit)					
{	
	/* nop */	
}
WORD  CClientSOCKET::mF_ESP	(t_PACKETHEADER *pPacket)		
{	
	return pPacket->m_nSize;	
}
WORD  CClientSOCKET::mF_DRH	(t_PACKETHEADER *pPacket)		
{	
	return pPacket->m_nSize;	
}
short CClientSOCKET::mF_DRB	(t_PACKETHEADER *pPacket)		
{	
	return pPacket->m_nSize;	
}

//-------------------------------------------------------------------------------------------------
bool CClientSOCKET::_Init (void)
{
    m_pRecvPacket  = (t_PACKET *) new char [ MAX_PACKET_SIZE ];
	m_nPacketSize  = 0;
	m_nRecvBytes   = 0;
	m_nSendBytes   = 0;
	m_bWritable	   = false;
//	m_bRecvBlocking= false;
	m_hThread	   = NULL;
	m_hThreadEvent = NULL;

	m_cStatus	  = CLIENTSOCKET_DISCONNECTED;

    ::InitializeCriticalSection( &m_csThread );

	return true;
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::_Free (void)
{
    classDLLNODE <t_SendPACKET *> *pSendNode;

    pSendNode = m_WaitPacketQ.GetHeadNode ();
    while ( pSendNode ) {
        m_WaitPacketQ.DeleteNode (pSendNode);

        delete[] pSendNode->DATA;
        delete   pSendNode;

        pSendNode = m_WaitPacketQ.GetHeadNode ();
    }

	// Clear Send Queue.
    pSendNode = m_SendPacketQ.GetHeadNode ();
    while ( pSendNode ) {
        m_SendPacketQ.DeleteNode (pSendNode);

        delete[] pSendNode->DATA;
        delete   pSendNode;

        pSendNode = m_SendPacketQ.GetHeadNode ();
    }

	// Clear Receive Qeueue.
    classDLLNODE <t_PACKET *> *pRecvNode;
    pRecvNode = m_RecvPacketQ.GetHeadNode ();
    while ( pRecvNode ) {
        delete[] pRecvNode->DATA;
        m_RecvPacketQ.DeleteNFree( pRecvNode );

        pRecvNode = m_RecvPacketQ.GetHeadNode ();
    }

	if ( m_pRecvPacket ) {
		delete[] m_pRecvPacket;
		m_pRecvPacket = NULL;
	}

	m_cStatus = CLIENTSOCKET_DISCONNECTED;

    ::DeleteCriticalSection( &m_csThread );
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnConnect(int nErrorCode)
{
	if ( !nErrorCode ) {
	    classDLLNODE <t_SendPACKET *> *pSendNode;

		// Clear Send Queue.
		pSendNode = m_SendPacketQ.GetHeadNode ();
		while ( pSendNode ) {
			m_SendPacketQ.DeleteNode (pSendNode);

			delete[] pSendNode->DATA;
			delete   pSendNode;

			pSendNode = m_SendPacketQ.GetHeadNode ();
		}

		// Clear Receive Qeueue.
		classDLLNODE <t_PACKET *> *pRecvNode;
		pRecvNode = m_RecvPacketQ.GetHeadNode ();
		while ( pRecvNode ) {
			delete[] pRecvNode->DATA;
			m_RecvPacketQ.DeleteNFree( pRecvNode );

			pRecvNode = m_RecvPacketQ.GetHeadNode ();
		}


		m_cStatus = CLIENTSOCKET_CONNECTED;

		m_hThreadEvent = ::CreateEvent  (NULL, false, false, NULL);
		//m_hThread      = ::CreateThread (NULL, 0, ClientSOCKET_SendTHREAD, this, 0, &m_dwThreadID);
		m_hThread      = (HANDLE)_beginthreadex (NULL, 0, ClientSOCKET_SendTHREAD, this, 0, &m_dwThreadID);
		SetThreadPriority (m_hThread, THREAD_PRIORITY_NORMAL);	// Indicates 1 point  below normal priority for the priority class.
	} else {
		m_cStatus = CLIENTSOCKET_SERVERDEAD;
		this->Close ();
	}
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnAccepted(int *pCode)
{
	;
}

//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnClose(int nErrorCode)
{
	Close ();		// Close Socket ...
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnReceive(int nErrorCode)
{
	DWORD dwBytes;

	if ( !this->IOCtl(FIONREAD, &dwBytes) ) {
		nErrorCode = WSAGetLastError();
		return;
	} 

	Packet_Recv( dwBytes );

	/*
	if ( dwBytes != 0 || nErrorCode != 0 ) {
		if ( !nErrorCode )
			Packet_Recv ();
	} else {
		this->m_bRecvBlocking = true;
	}
	*/
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::OnSend (int nErrorCode)
{
	if ( !nErrorCode ) {
		m_bWritable = true;
		::SetEvent( m_hThreadEvent );			// 쓰레드에 통보 !!!
	}
}

//-------------------------------------------------------------------------------------------------
/*
void CClientSOCKET::Packet_Register2Q (classDLLIST <t_SendPACKET *> &PacketQ, t_SendPACKET *pRegPacket)
{
    classDLLNODE <t_PACKET *> *pNewNode;
    t_PACKET                  *pPacket;

    pNewNode = new classDLLNODE <t_PACKET *> (NULL);
    pPacket  = (t_PACKET *) new char[ pRegPacket->m_wSize ];
	if ( pPacket == NULL ) {
		::MessageBox (NULL, "Out of memeory...", "ERROR", MB_OK);
		return;
	}

    ::CopyMemory (pPacket, pRegPacket, wSize);
    pNewNode->DATA = pPacket;
	PacketQ.AppendNode (pNewNode);
}
*/

//-------------------------------------------------------------------------------------------------
void CClientSOCKET::Packet_Register2RecvQ( t_PACKET *pRegPacket )
{
	t_PACKET *pNewPacket;
	pNewPacket  = (t_PACKET *) new char[ pRegPacket->m_HEADER.m_nSize ];
	if ( pNewPacket ) {
		::CopyMemory (pNewPacket, pRegPacket, pRegPacket->m_HEADER.m_nSize);
		m_RecvPacketQ.AllocNAppend( pNewPacket );
	}
}

void CClientSOCKET::Set_NetSTATUS (BYTE btStatus)
{	
	m_pRecvPacket->m_HEADER.m_wType		= SOCKET_NETWORK_STATUS;
	m_pRecvPacket->m_HEADER.m_nSize		= sizeof( t_NETWORK_STATUS );
	( (t_NETWORK_STATUS*)m_pRecvPacket)->m_btStatus	= btStatus;

	this->Packet_Register2RecvQ( m_pRecvPacket );
}

//-------------------------------------------------------------------------------------------------
void CClientSOCKET::Packet_Register2SendQ (t_PACKET *pRegPacket)
{
	if ( m_cStatus != CLIENTSOCKET_CONNECTED ) 
		return;

	t_SendPACKET *pSendPacket = new t_SendPACKET;
	if ( NULL == pSendPacket )
		return;
	
	::CopyMemory( &pSendPacket->m_Packet, pRegPacket, pRegPacket->m_HEADER.m_nSize );

//	if ( m_cStatus == CLIENTSOCKET_CONNECTED ) 
	{
		::EnterCriticalSection( &m_csThread );
		{
			// Encoding pSendPacket->m_Packet ... 위에 있던것을 안으로... 인코딩된 수서와 보내는 순서가 멀티쓰레드에 의해
			// 틀려질수 있기때문에...
			pSendPacket->m_wSize = this->mF_ESP( &pSendPacket->m_Packet.m_HEADER );

			m_WaitPacketQ.AllocNAppend( pSendPacket );
		}
		::LeaveCriticalSection( &m_csThread );

		::SetEvent( m_hThreadEvent );			// 쓰레드에 통보 !!!
	}
}


//-------------------------------------------------------------------------------------------------
void CClientSOCKET::Packet_Recv( int iToRecvBytes )
{
    int iRecvBytes;

	do {
		if ( this->m_nRecvBytes < sizeof(t_PACKETHEADER) ) {
			iRecvBytes = this->Receive ((char*)m_pRecvPacket + this->m_nRecvBytes, sizeof(t_PACKETHEADER)-this->m_nRecvBytes, 0);
		} else {
			//iRecvBytes = this->Receive ((char*)m_pRecvPacket + m_nRecvBytes, m_pRecvPacket->m_HEADER.m_nSize - m_nRecvBytes, 0);
			iRecvBytes = this->Receive ((char*)m_pRecvPacket + this->m_nRecvBytes, this->m_nPacketSize - this->m_nRecvBytes, 0);
		}

		if ( iRecvBytes == SOCKET_ERROR ) {
			int WSAErr = WSAGetLastError ();
			if ( WSAErr != WSAEWOULDBLOCK ) {
				// LOST  Connection !!!
				// CLOSE Connection ...

				Socket_Error ("Packet_Recv ...");
				break;
			}
			break;
		} else
		if ( iRecvBytes <= 0 )
			return;

		this->m_nRecvBytes += iRecvBytes;

		if ( this->m_nRecvBytes >= sizeof(t_PACKETHEADER) ) {
			if ( this->m_nRecvBytes == sizeof(t_PACKETHEADER) ) {
				// Decoding Packet Header ...
				this->m_nPacketSize = this->mF_DRH( &m_pRecvPacket->m_HEADER );
				if ( !this->m_nPacketSize ) {
					// 패킷 오류 !!!
					this->Close ();
					return;
				}
			}

			_ASSERT( this->m_nPacketSize );
			_ASSERT( this->m_nRecvBytes <= this->m_nPacketSize );

			// if ( m_nRecvBytes >= m_pRecvPacket->m_HEADER.m_nSize ) {
			if ( this->m_nRecvBytes >= this->m_nPacketSize ) {
				// this->Packet_Register2RecvQ (m_pRecvPacket);

				t_PACKET *pNewPacket;
				// pNewPacket  = (t_PACKET *) new char[ m_pRecvPacket->m_HEADER.m_nSize ];
				pNewPacket  = (t_PACKET *) new char[ this->m_nPacketSize ];
				if ( pNewPacket ) {
					// ::CopyMemory (pNewPacket, m_pRecvPacket, m_pRecvPacket->m_HEADER.m_nSize);
					::CopyMemory (pNewPacket, m_pRecvPacket, this->m_nPacketSize);

					// Decoing Packet Body ...
					if ( !this->mF_DRB( &pNewPacket->m_HEADER ) ) {
						_ASSERT( 0 );
					}

					m_RecvPacketQ.AllocNAppend( pNewPacket );
				}

				this->m_nPacketSize = 0;
				this->m_nRecvBytes = 0;
			}
		}

		iToRecvBytes -= iRecvBytes;
	} while( iToRecvBytes > 0 );
}


//-------------------------------------------------------------------------------------------------
bool CClientSOCKET::Packet_Send (void)
{
    classDLLNODE <t_SendPACKET *> *pNode;
    int iRetValue;

    while ( m_SendPacketQ.GetNodeCount() > 0 ) {
        pNode = m_SendPacketQ.GetHeadNode();

		iRetValue = this->Send ((char*)pNode->DATA->m_pDATA + m_nSendBytes, pNode->DATA->m_wSize - m_nSendBytes, 0);
		if ( iRetValue == SOCKET_ERROR ) {
			int WSAErr = WSAGetLastError ();

			if ( WSAErr != WSAEWOULDBLOCK ) {
				// LOST  Connection !!!
				// CLOSE Connection ...
				Socket_Error ("Packet_Send ...");
				return false;
			}

			m_bWritable = false;
			return true;
		}

		m_nSendBytes += iRetValue;
		if ( m_nSendBytes == pNode->DATA->m_wSize ) {
			classDLLNODE <t_SendPACKET *> *pDelNode;

			pDelNode = pNode;
			pNode = m_SendPacketQ.GetNextNode (pDelNode);

			delete pDelNode->DATA;
			m_SendPacketQ.DeleteNFree( pDelNode );

			m_nSendBytes = 0;
        }
    }

	return true;
}


//-------------------------------------------------------------------------------------------------
//
//	UDP Socket
//
void CClientSOCKET::Packet_Register2RecvUDPQ (u_long ulFromIP, WORD wFromPort, int iPacketSize)
{
	/*
    classDLLNODE <struct tagUDPPACKET *> *pNewNode;
	struct tagUDPPACKET	*pUDPPacket;

    pNewNode = new classDLLNODE <struct tagUDPPACKET *> (NULL);
    pUDPPacket = (struct tagUDPPACKET*) new struct tagUDPPACKET;
	pUDPPacket->m_pPacket   = (t_PACKET *) new char[ iPacketSize ];
	
	pUDPPacket->m_ulFromIP    = ulFromIP;
	pUDPPacket->m_wFromPort	  = wFromPort;
	pUDPPacket->m_iPacketSize = iPacketSize;
    CopyMemory (pUDPPacket->m_pPacket, m_pRecvPacket, iPacketSize);

    pNewNode->DATA = pUDPPacket;
	m_RecvUDPPacketQ.AppendNode (pNewNode);
	*/
}


//-------------------------------------------------------------------------------------------------
//
//	UDP Socket
//
void CClientSOCKET::Packet_RecvFrom (void)
{
/*
struct in_addr {
        union {
                struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { u_short s_w1,s_w2; } S_un_w;
                u_long S_addr;
        } S_un;
#define s_addr  S_un.S_addr				// can be used for most tcp & ip code 
#define s_host  S_un.S_un_b.s_b2		// host on imp 
#define s_net   S_un.S_un_b.s_b1		// network 
#define s_imp   S_un.S_un_w.s_w2		// imp 
#define s_impno S_un.S_un_b.s_b4		// imp # 
#define s_lh    S_un.S_un_b.s_b3		// logical host 
};
 
struct sockaddr_in{
    short				sin_family;
    unsigned short      sin_port;
    struct   in_addr    sin_addr;
    char				sin_zero[8];
};
*/
	SOCKADDR_IN sSockAddr;
	int		    iRet;

	iRet = ReceiveFrom ((void*) m_pRecvPacket, MAX_PACKET_SIZE, &sSockAddr, 0);
	if ( iRet != SOCKET_ERROR ) {
		if ( iRet > 0 && iRet < MAX_PACKET_SIZE ) 
			Packet_Register2RecvUDPQ (sSockAddr.sin_addr.s_addr, sSockAddr.sin_port, iRet);
	} 
}


//-------------------------------------------------------------------------------------------------
bool CClientSOCKET::Peek_Packet (t_PACKET *pPacket, bool bRemoveFromQ)
{
	if ( this->m_RecvPacketQ.GetNodeCount() > 0 ) {
        classDLLNODE <t_PACKET*> *pNode;

		pNode   = this->m_RecvPacketQ.GetHeadNode();
		// pPacket = pNode->DATA;
		CopyMemory (pPacket, pNode->DATA, pNode->DATA->m_HEADER.m_nSize);

        // 패킷 삭제.
		if ( bRemoveFromQ ) {
			this->m_RecvPacketQ.DeleteNode ( pNode );
			delete[] pNode->DATA;
			delete   pNode;
		}

		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool CClientSOCKET::Connect (HWND hWND, char *szServerIP, int iTCPPort, UINT uiWindowMsg)
{
	bool bReturn;

	if ( INVALID_SOCKET == this->m_hSocket ) {
		bReturn = this->Create(hWND, uiWindowMsg);
		if ( !bReturn ) {
			OutputDebugString ("Connect :: Create() Failure !!!\n");
			return false;
		}
	}

	bReturn = CRawSOCKET::Connect (szServerIP, iTCPPort);
	if ( !bReturn ) {
		OutputDebugString ("Connect :: Connect() Failure !!!\n");
		return false;
	}

//	_AppendSocketList (pClientSocket);

	return true;
}

//-------------------------------------------------------------------------------------------------
void CClientSOCKET::Close ()
{
	if ( m_hThread != NULL ) {
	    m_cStatus = CLIENTSOCKET_CLOSING;

		do {
			::SetEvent( m_hThreadEvent );				// 쓰레드 죽으라고 통보...
			Sleep (100);
		} while ( m_cStatus == CLIENTSOCKET_CLOSING ) ;

		::CloseHandle (m_hThread);
		m_dwThreadID = 0;
		m_hThread    = NULL;
	}

	if ( m_hThreadEvent ) {
		::CloseHandle( m_hThreadEvent );
		m_hThreadEvent = NULL;
	}

	CRawSOCKET::Close();
}


//-------------------------------------------------------------------------------------------------
