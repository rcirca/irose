
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "classUTIL.h"
#include "classLOG.h"
#include "classSTR.h"
#include "ioDataPOOL.h"
#include "CRandom.h"

#include "iocpSocketSERVER.h"


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// spin lock 4000으로 설정...
IOCPSocketSERVER::IOCPSocketSERVER (char *szName, BYTE btMulCPU, char cAddCPU, bool bManageSocketVerify) 
						: CCriticalSection( 4000 )
{
	m_bManageSocketVerify = bManageSocketVerify;
	m_ServerName.Set( szName );

	SYSTEM_INFO	sSystemInfo;
	::GetSystemInfo (&sSystemInfo);
/*
	CPoolPACKET::Instance ();
	CPoolRECVIO::Instance ();
	CPoolSENDIO::Instance ();
	CPacketCODEC::Instance ();
*/
	m_pSocketIDX = NULL;
	m_pAcceptTHREAD = NULL;
	m_ppWorkerTHREAD = NULL;

	m_pBlockingIP = new classListBLOCK< tagBlockDATA >( DEF_BLACK_IP_HASHTABLE_SIZE );

	m_dwWorkerThreadCNT = sSystemInfo.dwNumberOfProcessors * btMulCPU + cAddCPU;
	if ( m_dwWorkerThreadCNT > 32 )
		m_dwWorkerThreadCNT = 32;
	m_IOCP.OpenPort( m_dwWorkerThreadCNT );
}
IOCPSocketSERVER::~IOCPSocketSERVER ()
{
	this->Shutdown ();
	m_IOCP.ClosePort ();
	SAFE_DELETE( m_pBlockingIP );
/*
	CPacketCODEC::Destroy ();
	CPoolRECVIO::Destroy ();
	CPoolSENDIO::Destroy ();
	CPoolPACKET::Destroy ();
	CRandom::Destroy ();
*/
	CSLList< tagAtkIP >::tagNODE *pNode;
	pNode = m_AttackLIST.GetHeadNode();
	while( pNode ) {
		pNode = m_AttackLIST.DeleteHead ();
		SAFE_DELETE( pNode );
		pNode = m_AttackLIST.GetHeadNode();
	} 
}

//-------------------------------------------------------------------------------------------------
bool IOCPSocketSERVER::Active (int iListenTCPPortNO, DWORD dwMaxSocketCount, int iKeepAliveSec)
{
	if ( !m_IOCP.GetHANDLE() || m_pSocketIDX )
		return false;

	m_pSocketIDX = new CIndexARRAY< iocpSOCKET* >( "IOCPSocketIDX", dwMaxSocketCount );
	m_pSocketIDX->ClearAll ();
	m_dwMaxSocketCNT = dwMaxSocketCount;

	this->StartWORKER ();
	this->StartACCEPT (iListenTCPPortNO, iKeepAliveSec);

	return true;
}

//-------------------------------------------------------------------------------------------------
void IOCPSocketSERVER::ShutdownSOCKET ()
{
	// 모든 소켓 종료...
	if ( m_pSocketIDX ) {
		for (DWORD dwI=0; dwI<m_dwMaxSocketCNT; dwI++)
			this->Del_SOCKET( dwI );

		SAFE_DELETE( m_pSocketIDX );
	}
}


//-------------------------------------------------------------------------------------------------
void IOCPSocketSERVER::StartACCEPT (int iListenTCPPortNO, int iKeepAliveSec)
{
	if ( NULL == m_pAcceptTHREAD ) {
		m_pAcceptTHREAD = new IOCPSocketAcceptTHREAD( this );                // suspend mode
		m_pAcceptTHREAD->Init( iListenTCPPortNO, iKeepAliveSec );
		m_pAcceptTHREAD->Resume ();
	}
}
void IOCPSocketSERVER::ShutdownACCEPT ()
{
	if ( m_pAcceptTHREAD ) {
		m_pAcceptTHREAD->Free ();
		// ShutdownACCEPT()를 호출한 쓰레드와 m_pAcceptTHREAD가 틀린 쓰래드 인데.. Free에서 종료되기 전에는 리턴 안됨
		SAFE_DELETE( m_pAcceptTHREAD );
	}
}

//-------------------------------------------------------------------------------------------------
void IOCPSocketSERVER::StartWORKER ()
{
	if ( NULL == m_ppWorkerTHREAD ) {
		m_ppWorkerTHREAD = new IOCPSocketWorkerTHREAD*[ m_dwWorkerThreadCNT ];
		for (DWORD dwI=0; dwI<m_dwWorkerThreadCNT; dwI++) {
			m_ppWorkerTHREAD[ dwI ] = new IOCPSocketWorkerTHREAD( this );
			m_ppWorkerTHREAD[ dwI ]->Init( m_IOCP.GetHANDLE(), dwI );
			m_ppWorkerTHREAD[ dwI ]->Resume ();
		}
	}
}
void IOCPSocketSERVER::ShutdownWORKER ()
{
	if ( m_ppWorkerTHREAD ) {
		for (DWORD dwI=0; dwI<m_dwWorkerThreadCNT; dwI++) {
			m_ppWorkerTHREAD[ dwI ]->Terminate();

			if ( 0 == ::PostQueuedCompletionStatus( m_IOCP.GetHANDLE(), -1/*dwNumberOfBytesTransferred*/, 0/*dwCompletionKey*/, NULL/*lpOverlapped*/) ) {
				g_LOG.CS_ODS(0xffff, "ThreadWORKER::Free() PostQueuedCompletionStatus() return 0, LastERROR: %d(0x%x)", GetLastError(), GetLastError());
			}
		}

		DWORD dwDead;
		do {
			dwDead = 0;
			for (DWORD dwI=0; dwI<m_dwWorkerThreadCNT; dwI++) {
				dwDead ++;
				if ( m_ppWorkerTHREAD[ dwI ] ) {
					if ( m_ppWorkerTHREAD[ dwI ]->IsFinished () ) {
						SAFE_DELETE( m_ppWorkerTHREAD[ dwI ] );
					} else {
						dwDead --;
						::Sleep( 10 );
					}
				}
			}
		} while( dwDead < m_dwWorkerThreadCNT );

		SAFE_DELETE_ARRAY( m_ppWorkerTHREAD );
	}
}


//-------------------------------------------------------------------------------------------------
void IOCPSocketSERVER::CloseIdleSCOKET (DWORD dwIdleMilliSec)
{
	DWORD dwCurTime = ::timeGetTime();	// classTIME::GetCurrentAbsMilliSecond ();
	CDLList<iocpSOCKET*>::tagNODE *pNode, *pDelNode;

	this->Lock ();
	{
		pNode = this->m_SocketLIST.GetHeadNode ();
		while( pNode ) {
			if ( pNode->m_VALUE->m_bVerified ) {
				// 검증된 클라이언트 소켓이다.
				pNode->m_VALUE->m_pSockNODE = NULL;
				pDelNode = pNode;
				pNode = pDelNode->GetNext ();
				this->m_SocketLIST.DeleteNFree( pDelNode );
				continue;
			}
			if ( dwCurTime - pNode->m_VALUE->m_dwConnTIME >= dwIdleMilliSec ) {
				// 짤라 ??? ...
				// if ( pNode->m_VALUE->OnIdelCLOSE() ) 
				{
					g_LOG.CS_ODS( 0xffff, "Close Idle SOCK:: %s:%d=%d-%d, %s\n", this->GetServerNAME(), 
						dwCurTime - pNode->m_VALUE->m_dwConnTIME, dwCurTime, pNode->m_VALUE->m_dwConnTIME,
						pNode->m_VALUE->Get_IP() );

					pNode->m_VALUE->CloseSocket();
					pNode->m_VALUE->m_pSockNODE = NULL;
					pDelNode = pNode;
					pNode = pDelNode->GetNext ();
					this->m_SocketLIST.DeleteNFree( pDelNode );
					continue;
				}
			}
			pNode = pNode->GetNext ();
		}
	}
	this->Unlock ();
}


//-------------------------------------------------------------------------------------------------
bool IOCPSocketSERVER::New_SOCKET( SOCKET hSocket, sockaddr_in &SockADDR )
{
	if ( m_RefuseIP.Find( SockADDR.sin_addr ) ) {
		// 접근 거부된 IP영역이다.
		return false;
	}


	char *szIP = ::inet_ntoa( SockADDR.sin_addr );
	t_HASHKEY ipHashKEY = CStrVAR::GetHASH( szIP );

	// Check block ip address !!!
	tagBlockDATA *pBlockIP = m_pBlockingIP->Search( ipHashKEY, szIP, true);
	if ( pBlockIP ) {
        // m_pBlockingIP->Update( pBlockIP, IP_BLOCK_TYPE_ACCEPT, 1 );
		// if ( pBlockIP->m_dwBlockSECOND >= 0x0ff000000 ) {
        //    pBlockIP->m_dwExpireTIME = 0;   // 영구 차단 !!!
        //}
		switch( pBlockIP->m_wAttackTYPE ) {
			case IP_BLOCK_TYPE_ACCEPT :
				if ( pBlockIP->m_dwBlockSECOND >= 5 ) {
					pBlockIP->m_dwBlockSECOND = 5;			// 최대 5초 차단...
				}
				break;
			case IP_BLOCK_TYPE_PACKET :
				if ( pBlockIP->m_dwBlockSECOND >= 120 ) {
					pBlockIP->m_dwBlockSECOND = 120;		// 최대 2분 차단...
				}
				break;
		}
		return false;
	}

	// 접속 공격 ip 체크...
	INT64 biTIME = classTIME::GetCurrentAbsMilliSecond ();
	ULONG ulIP   = SockADDR.sin_addr.S_un.S_addr;

	CSLList< tagAtkIP >::tagNODE *pNode;
	pNode = m_AttackLIST.GetHeadNode();
	int iFindCnt=0;
	while( pNode ) {
		if ( biTIME - pNode->m_VALUE.m_biTIME >= 50 ) {	// 0.5초
			pNode = m_AttackLIST.DeleteHead ();
			SAFE_DELETE( pNode );
			pNode = m_AttackLIST.GetHeadNode();
			continue;
		}
		if ( pNode->m_VALUE.m_uiIP == ulIP ) {
			if ( ++iFindCnt >= 3 ) {
				m_pBlockingIP->Insert (ipHashKEY, szIP, IP_BLOCK_TYPE_ACCEPT, 1 );
				LogString( 0xffff, ">>>> Add Block IP :: %s\n", szIP );
				return false;
			}
		}
		pNode = pNode->GetNext ();
	} 
	pNode = new CSLList< tagAtkIP >::tagNODE;
	pNode->m_VALUE.m_biTIME = biTIME;
	pNode->m_VALUE.m_uiIP = ulIP;
	m_AttackLIST.AppendNode (pNode);


	iocpSOCKET *pSOCKET;
	pSOCKET = this->AllocClientSOCKET();
	if ( NULL == pSOCKET )
		return false;

	pSOCKET->Init_SCOKET ();
	this->Lock ();
		int iSocketIDX = this->m_pSocketIDX->AddData( pSOCKET );
	this->Unlock ();
	if ( 0 == iSocketIDX ) {
		this->FreeClientSOCKET( pSOCKET );
		return false;
	}

	pSOCKET->m_Socket = hSocket;
	pSOCKET->m_IP.Set( szIP );
	pSOCKET->m_HashKeyIP = ipHashKEY;

	if ( !m_IOCP.LinkPort( (HANDLE)hSocket, iSocketIDX ) || 
		 eRESULT_PACKET_OK != pSOCKET->Recv_Start () ) {
		this->Lock ();
			// @버그 수정 : 2004. 7. 16 iSocketIDX대신 pSOCKET->m_iSocketIDX로 사용했던 실수가 있었음.
			// 아미 메모리 풀이 중복 해제되는 원인으루 추정됨...
			m_pSocketIDX->DelData( iSocketIDX );
			// lock 외부에 있던거 안으로...
			// Shutdown함수호출시에서 Accept, Worker쓰레드가 종료되어 Free..함수가 호출될경우
			// ShutdownSocket과 충돌...
			this->FreeClientSOCKET( pSOCKET );
		this->Unlock ();

		return false;
	} 

	if ( m_bManageSocketVerify ) {
		pSOCKET->m_dwConnTIME = ::timeGetTime();	// classTIME::GetCurrentAbsMilliSecond ();
		this->Lock ();
			pSOCKET->m_pSockNODE = this->m_SocketLIST.AllocNAppend( pSOCKET );
		this->Unlock ();
	}

	pSOCKET->m_iSocketIDX = iSocketIDX;
	this->InitClientSOCKET( pSOCKET );

    return true;
}


//-------------------------------------------------------------------------------------------------
// IOCPSocketSERVER::ShutdownSOCKET()
// IOCPSocketSERVER::On_FALSE()
// IOCPSocketSERVER::On_TRUE() 에서 호출됨
iocpSOCKET *IOCPSocketSERVER::Del_SOCKET( int iSocketIDX )
{
	iocpSOCKET *pSOCKET;

	this->Lock ();
		pSOCKET = this->GetSOCKET( iSocketIDX );
		if ( pSOCKET ) {
			if ( pSOCKET->m_pSockNODE ) {
				this->m_SocketLIST.DeleteNFree( pSOCKET->m_pSockNODE );
				pSOCKET->m_pSockNODE = NULL;
			}
			m_pSocketIDX->DelData( iSocketIDX );
			pSOCKET->m_iSocketIDX = 0;
			pSOCKET->CloseSocket ();

			// 위에 소켓 버퍼를 먼저 0으루 만들고 호출되어함..순서 주의 !!!!
			this->ClosedClientSOCKET( pSOCKET );
		}
	this->Unlock ();
/*
	//if ( pSOCKET ) {
	//	this->ClosedClientSOCKET( pSOCKET );
	//}
*/

	return pSOCKET;
}

//-------------------------------------------------------------------------------------------------
// 소켓 종료..
void IOCPSocketSERVER::On_FALSE (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey)
{
    tagIO_DATA *pIOData = (tagIO_DATA*) lpOverlapped;

	switch ( pIOData->m_IOmode ) {
		case ioREAD :
			// ioWRITE 일경우에는 pUSER->m_SendList에 노드가 이미 등록되어 있어 SubUser()에서 풀림으로 ioREAD일 때만...
			iocpSOCKET::Free_RecvIODATA( pIOData );
			break;
		case ioWRITE :
			break;
		default :
			_ASSERT( 0 );
	}

	this->Del_SOCKET( dwCompletionKey );
}

//-------------------------------------------------------------------------------------------------
void IOCPSocketSERVER::On_TRUE  (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey, DWORD dwBytesIO)
{
	iocpSOCKET *pSOCKET;
    tagIO_DATA *pIOData = (tagIO_DATA*) lpOverlapped;

	pSOCKET = this->GetSOCKET( dwCompletionKey );
	if ( pSOCKET ) {
		// ** pUSER의 소켓이 종료된 상태에서도 이곳으로 온다 ㅡㅡ;
		pIOData->m_dwIOBytes += dwBytesIO;
		switch ( pIOData->m_IOmode ) {
			case ioREAD :
				switch( pSOCKET->Recv_Complete( pIOData ) ) {
					case eRESULT_PACKET_BLOCK :
						// 30초 블럭...
						m_pBlockingIP->Insert (pSOCKET->m_HashKeyIP, pSOCKET->Get_IP(), IP_BLOCK_TYPE_PACKET, 30 );

					case eRESULT_PACKET_DISCONNECT :
						// 짤러라 !!!
						this->Del_SOCKET( dwCompletionKey );
						break;
				}
				//if ( !pSOCKET->Recv_Complete ( pIOData ) ) {
				//	// 짤러라 !!!
				//	this->Del_SOCKET( dwCompletionKey );
				//}
				break;
			case ioWRITE :	// 보내기 완료 !!!
				if ( !pSOCKET->Send_Complete ( pIOData ) ) {
					// 짤러라 !!!
					this->Del_SOCKET( dwCompletionKey );
				}
				break;
			default :
				_ASSERT( 0 );
		}
	}
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
