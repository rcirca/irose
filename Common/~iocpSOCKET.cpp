//---------------------------------------------------------------------------

#include "stdAFX.h"
#pragma hdrstop

#include "classIOCP.h"
#include "iocpSOCKET.h"

#include "GameSERVER.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//-------------------------------------------------------------------------------------------------
//
//  IOCP Client socket class
//
//-------------------------------------------------------------------------------------------------
classDLLNODE<tagIO_DATA> *iocpSOCKET::Alloc_RecvIODATA (void)
{
    classDLLNODE<tagIO_DATA> *pRecvNODE;

    pRecvNODE = new classDLLNODE< tagIO_DATA >;
    if ( pRecvNODE ) {
        ::ZeroMemory (&pRecvNODE->DATA.m_Overlapped, sizeof(OVERLAPPED));
        pRecvNODE->DATA.m_IOmode    = ioREAD;
        pRecvNODE->DATA.m_pNODE     = pRecvNODE;
        pRecvNODE->DATA.m_pCPacket  = Packet_AllocOnly ();
        pRecvNODE->DATA.m_dwIOBytes = 0;

//        g_LOG.CS_ODS( 0xffff, ">> Alloc_RecvIODATA :: %d \n", pRecvNODE->DATA.m_pNODE );
    }

    return pRecvNODE;
}
void iocpSOCKET::Free_RecvIODATA (tagIO_DATA *pRecvDATA)
{
//    g_LOG.CS_ODS( 0xffff, "<< Free_RecvIODATA :: %d \n", pRecvDATA->m_pNODE );

    SAFE_DELETE( pRecvDATA->m_pCPacket );
    delete pRecvDATA->m_pNODE;
    // pRecvDATA->m_pNODE = NULL;  <-- ERROR !!!  so can't use SAFE_DELETE( pRecvDATA->m_pNODE );
}

//-------------------------------------------------------------------------------------------------
classDLLNODE<tagIO_DATA> *iocpSOCKET::Alloc_SendIODATA (CPacket *pCPacket)
{
    classDLLNODE<tagIO_DATA> *pSendNODE;

    pSendNODE = new classDLLNODE< tagIO_DATA >;
    if ( pSendNODE ) {
        ::ZeroMemory (&pSendNODE->DATA.m_Overlapped, sizeof(OVERLAPPED));
        pSendNODE->DATA.m_IOmode    = ioWRITE;
        pSendNODE->DATA.m_pNODE     = pSendNODE;
        pSendNODE->DATA.m_pCPacket  = pCPacket;
        pSendNODE->DATA.m_dwIOBytes = 0;

//        g_LOG.CS_ODS( 0xffff, ">> Alloc_SendIODATA :: %d \n", pSendNODE );
    }

    return pSendNODE;
}
void iocpSOCKET::Free_SendIODATA (tagIO_DATA *pSendDATA)
{
//    g_LOG.CS_ODS( 0xffff, "<< Free_SendIODATA :: %d \n", pSendDATA->m_pNODE );

	// 다른 사용자에게 전송중에 있을경우 LockPacket이 걸려 있어 대기상태에 빠지도록...
    Packet_DecRefCount( pSendDATA->m_pCPacket );
    delete pSendDATA->m_pNODE;
    // pSendDATA->m_pNODE = NULL;  <-- ERROR !!!  so can't use SAFE_DELETE( pSendDATA->m_pNODE );
}


//-------------------------------------------------------------------------------------------------
iocpSOCKET::iocpSOCKET ()
{
    m_Socket    = INVALID_SOCKET;
    m_bSending  = false;

    ::InitializeCriticalSection( &m_csSELF );
	::InitializeCriticalSection( &m_csSendList );
/*
    ::InitializeCriticalSection( &m_csRecvList );
*/
}
iocpSOCKET::~iocpSOCKET ()
{
    CloseSocket ();

//	Clear_IOList ();
/*
    ::DeleteCriticalSection( &m_csRecvList );
*/    
	::DeleteCriticalSection( &m_csSendList );
    ::DeleteCriticalSection( &m_csSELF );
}

//-------------------------------------------------------------------------------------------------
void iocpSOCKET::CloseSocket (void)
{
    if ( m_Socket != INVALID_SOCKET ) {
        classIOCP::CloseClientSocket( m_Socket );
        m_Socket = INVALID_SOCKET;
    }
}

//-------------------------------------------------------------------------------------------------
void iocpSOCKET::Clear_LIST (void)
{
    classDLLNODE<tagIO_DATA> *pNode;
    
/*
    ::EnterCriticalSection( &m_csRecvList );
    {
//		Log_String ("Clear %d RecvList items\n", m_RecvList.GetNodeCount ());
        while( pNode = m_RecvList.GetHeadNode() ) {
            m_RecvList.DeleteNode( pNode );
            g_pIO_DATA->ReleaseIONode ( pNode );
        }
    }
    ::LeaveCriticalSection( &m_csRecvList );
*/

    ::EnterCriticalSection( &m_csSendList );
    {
        while( pNode = m_SendList.GetHeadNode() ) {
            m_SendList.DeleteNode( pNode );
            this->Free_SendIODATA ( &pNode->DATA );
        }
    }
    ::LeaveCriticalSection( &m_csSendList );
}

//-------------------------------------------------------------------------------------------------
// pRecvNode에 이어 받기.
void iocpSOCKET::Recv_Continue (tagIO_DATA *pRecvDATA)
{
    pRecvDATA->m_pCPacket->m_iRefCnt = 1;

    if ( 0 == ::ReadFile( (HANDLE)m_Socket,										// HANDLE hFile,                // handle to file
                    &pRecvDATA->m_pCPacket->m_pDATA[ pRecvDATA->m_dwIOBytes ],  // LPVOID lpBuffer,             // data buffer
                    MAX_PACKET_SIZE-pRecvDATA->m_dwIOBytes,						// DWORD nNumberOfBytesToRead,  // number of bytes to read
                    NULL,														// LPDWORD lpNumberOfBytesRead, // number of bytes read
                    (LPOVERLAPPED)pRecvDATA ) ) 								// LPOVERLAPPED lpOverlapped    // overlapped buffer
    {
        // Function failed ..
        if ( ERROR_IO_PENDING != ::GetLastError () ) {
            DWORD dwCode = ::GetLastError ();

            this->CloseSocket ();
            ;
            g_LOG.CS_ODS(0xffff, "$$$$$$$      ERROR[ %d:%s ] ::: ReadFile \n", dwCode, CUtil::GetLastErrorMsg( dwCode ) );
            ;
        }
    }
}


//-------------------------------------------------------------------------------------------------
// 새로 받기.
bool iocpSOCKET::Recv_Start (void)
{
    classDLLNODE<tagIO_DATA> *pRecvNODE;

    pRecvNODE = this->Alloc_RecvIODATA ();
    if ( NULL == pRecvNODE )
        return false;

    this->Recv_Continue( &pRecvNODE->DATA );

    return true;
}


//-------------------------------------------------------------------------------------------------
void iocpSOCKET::Recv_Complete (tagIO_DATA *pRecvDATA)
{
    if ( pRecvDATA->m_dwIOBytes < sizeof(t_PACKETHEADER) ||
         (short)pRecvDATA->m_dwIOBytes < pRecvDATA->m_pCPacket->m_HEADER.m_nSize ) {
        this->Recv_Continue( pRecvDATA );	// 이어 받기.
        return;
    } else
    if ( (short)pRecvDATA->m_dwIOBytes == pRecvDATA->m_pCPacket->m_HEADER.m_nSize ) {
        // AddPacketToRecvList( pRecvNode );
        this->Recv_Done( pRecvDATA );
        this->Free_RecvIODATA( pRecvDATA );

        this->Recv_Start ();	// RecvComplete
        return;
    }

    // 뭉쳐온 패킷 분리.
    t_PACKET *pPacket;
    short	  nRemainBytes;

    nRemainBytes = (short)pRecvDATA->m_dwIOBytes;
    pPacket		 = (t_PACKET*) &pRecvDATA->m_pCPacket->m_pDATA;
    do {
        nRemainBytes -= pPacket->m_HEADER.m_nSize;
        if ( 0 == nRemainBytes ) {
            // AddPacketToRecvList( pRecvNode );			// 완성 패킷 추가
            this->Recv_Done( pRecvDATA );
            this->Free_RecvIODATA( pRecvDATA );

            this->Recv_Start ();						    // 새로 받기. :: RecvComplete
            return;
        }

        pPacket = (t_PACKET*) ( pPacket->m_pDATA + pPacket->m_HEADER.m_nSize );
    } while ( nRemainBytes >= sizeof(t_PACKETHEADER) && nRemainBytes >= pPacket->m_HEADER.m_nSize ) ;


    // 모자란 부분을 다시 읽어들일 데이타 생성.
    classDLLNODE<tagIO_DATA> *pNewNODE;
    pNewNODE = this->Alloc_RecvIODATA ();
    if ( NULL == pNewNODE )
        return;
    pNewNODE->DATA.m_dwIOBytes = nRemainBytes;
    ::CopyMemory (pNewNODE->DATA.m_pCPacket->m_pDATA, pPacket, nRemainBytes);

    // 앞부분의 완성 패킷등록.
    pRecvDATA->m_dwIOBytes -= nRemainBytes;
    // AddPacketToRecvList( pRecvNode );
    this->Recv_Done( pRecvDATA );
    this->Free_RecvIODATA( pRecvDATA );

    this->Recv_Continue( &pNewNODE->DATA );	    			// 이어 받기.
}


//-------------------------------------------------------------------------------------------------
// pSendNode에 이어 보내기.
bool iocpSOCKET::Send_Continue (tagIO_DATA *pSendDATA)
{
//	Log_String ("     Send Continue[ %d:%d ] !!!\n", m_iIndex, pSendNode->DATA.m_pCPacket->m_iPoolIndex);

    if ( 0 == ::WriteFile((HANDLE)m_Socket,											// HANDLE hFile,                    // handle to file
                    &pSendDATA->m_pCPacket->m_pDATA[ pSendDATA->m_dwIOBytes ],	    // LPCVOID lpBuffer,                // data buffer
                    pSendDATA->m_pCPacket->m_HEADER.m_nSize-pSendDATA->m_dwIOBytes,	// DWORD nNumberOfBytesToWrite,     // number of bytes to write
                    NULL,															// LPDWORD lpNumberOfBytesWritten,  // number of bytes written
                    (LPOVERLAPPED)pSendDATA )) {									// LPOVERLAPPED lpOverlapped        // overlapped buffer
        // Function failed ..
        if ( ERROR_IO_PENDING != GetLastError () ) {
            DWORD dwCode = ::GetLastError ();
            
            // 접속 해제~
            this->CloseSocket ();
            ;
            g_LOG.CS_ODS(0xffff, "$$$$$$$      ERROR[ %d:%s ] ::: WriteFile \n", dwCode, CUtil::GetLastErrorMsg( dwCode ));
            ;
        } else
            return false;
    } else
        m_bSending = true;

    return true;
}


//-------------------------------------------------------------------------------------------------
void iocpSOCKET::Send_Start (CPacket *pCPacket)
{
    if ( m_Socket == INVALID_SOCKET )
        return;

    classDLLNODE<tagIO_DATA> *pSendNODE;

    pSendNODE = this->Alloc_SendIODATA( pCPacket );
    if ( NULL == pSendNODE )
        return;
    pCPacket->m_iRefCnt ++;

    ::EnterCriticalSection( &m_csSendList );
    {
        m_SendList.AppendNode( pSendNODE );

        if ( !m_bSending ) {
            pSendNODE = m_SendList.GetHeadNode ();
            if ( !this->Send_Continue( &pSendNODE->DATA ) )
                m_bSending = true;
        }
    }
    ::LeaveCriticalSection( &m_csSendList );
}


//-------------------------------------------------------------------------------------------------
void iocpSOCKET::Send_Complete (tagIO_DATA *pSendDATA)
{
    ::EnterCriticalSection( &m_csSendList );
    {
        m_bSending = false;

        if ( (short)pSendDATA->m_dwIOBytes < pSendDATA->m_pCPacket->m_HEADER.m_nSize ) {
            this->Send_Continue (pSendDATA);
        } else {
/*
#ifdef	_DEBUG
            if ( pSendNode != m_SendList.GetHeadNode() ) {
                Dbg_String ("&&&& ERROR :: SendLIST  %s, %d\n", __FILE__, __LINE__);
            }
#endif
*/
            // ** 아래 라인에서 m_SendList에서 pSendNode를 삭제하는 과정에서
            //    오류가 난것은 pUSER가 이미 접속종료되어 ClearIOList() 함수를
            //    실행하여 m_SendList가 이미 비어있기 때문이다.
            m_SendList.DeleteNode( pSendDATA->m_pNODE );
            this->Free_SendIODATA( pSendDATA );

            classDLLNODE<tagIO_DATA> *pSendNODE = m_SendList.GetHeadNode ();
            if ( pSendNODE ) {
                this->Send_Continue( &pSendNODE->DATA );
            }
        }
    }
    ::LeaveCriticalSection( &m_csSendList );
}


//-------------------------------------------------------------------------------------------------
bool iocpSOCKET::Recv_Done (tagIO_DATA *pRecvDATA)
{
	t_PACKET *pPacket = (t_PACKET*)&pRecvDATA->m_pCPacket->m_pDATA;
    do {
        if ( !this->HandlePACKET( pPacket ) ) {
            ;
        }

        pRecvDATA->m_dwIOBytes -= pPacket->m_HEADER.m_nSize;
        pPacket = (t_PACKET*)( pPacket->m_pDATA + pPacket->m_HEADER.m_nSize );
    } while ( pRecvDATA->m_dwIOBytes );

    return true;
}


//-------------------------------------------------------------------------------------------------
