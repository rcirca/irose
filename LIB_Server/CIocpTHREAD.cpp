
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "classLOG.h"
#include "CIocpTHREAD.h"

//-------------------------------------------------------------------------------------------------
CIocpTHREAD::CIocpTHREAD (bool bCreateSuspended) : classTHREAD( bCreateSuspended )
{
	;
}

//-------------------------------------------------------------------------------------------------
void CIocpTHREAD::Execute ()
{
	DWORD		 dwBytesIO;
    DWORD        dwCompletionKey;
    LPOVERLAPPED lpOverlapped;

	g_LOG.CS_ODS(0xffff, ">  > >> CIocpTHREAD::Execute() ThreadID: %d(0x%x),  WorkerNO: %d \n", this->ThreadID, this->ThreadID, m_iThreadNO);

	while ( !this->Terminated )
	{
		if ( 0 == ::GetQueuedCompletionStatus( m_hIOCP, &dwBytesIO, &dwCompletionKey, (LPOVERLAPPED *)&lpOverlapped, INFINITE) ) {
			/*
			If *lpOverlapped is NULL and the function does not dequeue a completion packet from the completion port, 
			the RETURN VALUE IS ZERO.
			The function does not store information in the variables pointed to by 
			the lpNumberOfBytesTransferred and lpCompletionKey parameters.

			If *lpOverlapped is not NULL and the function dequeues a completion packet 
			for a failed I/O operation from the completion port, the RETURN VALUSE IS ZERO.
			return value is zero.
			The function stores information in the variables pointed to by 
			lpNumberOfBytesTransferred, lpCompletionKey, and lpOverlapped. 
			*/
			if ( NULL != lpOverlapped ) {
				// 소켓 종료
				DWORD dwErrCODE = ::GetLastError();
				
				//
				// ErrorCode == 64 :: 지정된 네트웍크 이름을 더이상 사용할수 없습니다 == 클라이언트 끊어진 상태.
				//
				/* if ( dwErrCODE != 64 ) {
					LPVOID lpMsgBuf;
					if ( ::FormatMessage( 
						FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dwErrCODE,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &lpMsgBuf,
						0,
						NULL ))
					{
						// Display the string.
						// MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
						g_LOG.CS_ODS( 0xffff, "$$$$ ERROR[%d]:: socketIdx: %d, %s\n", dwErrCODE, dwCompletionKey, lpMsgBuf );
						::LocalFree( lpMsgBuf );
					}
				} */
				
				STATUS_ReturnFALSE( lpOverlapped, dwCompletionKey );
				continue;
			}

			// 시스템 오류 !!!
            g_LOG.CS_ODS(0xffff, "* ERROR:: WorkerThread ID: %d(0x%x),  WorkerNO: %d : lpOverlapped == NULL \n", this->ThreadID, this->ThreadID, m_iThreadNO);
			break;
		}

		if ( 0 == dwBytesIO ) {
			STATUS_ReturnFALSE( lpOverlapped, dwCompletionKey );
			continue;
		}
        if ( dwBytesIO == -1 ) {
			// 0 == dwCompletionKey
			// IOCP is freed ?
			// g_LOG.CS_ODS(0xffff, "* Break:: WorkerThread ID: %d(0x%x),  WorkerNO: %d : dwBytesIO == -1 \n", this->ThreadID, this->ThreadID, m_iThreadNO);
            break;
        } 
		
		STATUS_ReturnTRUE( lpOverlapped, dwCompletionKey, dwBytesIO );
	}

	g_LOG.CS_ODS(0xffff, "<  < << CIocpTHREAD::Execute() ThreadID: %d(0x%x),  WorkerNO: %d \n", this->ThreadID, this->ThreadID, m_iThreadNO);
}


//-------------------------------------------------------------------------------------------------
bool CIocpTHREAD::Init (HANDLE hIOCP, int iThreadNO)
{
    m_hIOCP = hIOCP;
    m_iThreadNO = iThreadNO;

    return true;
}
void CIocpTHREAD::Free (void)
{
    this->Terminate ();

	g_LOG.CS_ODS(0xffff, ">>> CIocpTHREAD::Free ThreadID: %d(0x%x),  WorkerNO: %d \n", this->ThreadID, this->ThreadID, m_iThreadNO);
	do {
        if ( 0 == ::PostQueuedCompletionStatus( m_hIOCP, -1, 0, NULL) ) {
            g_LOG.CS_ODS(0xffff, "ThreadWORKER::Free() PostQueuedCompletionStatus() return 0, LastERROR: %d(0x%x)", GetLastError(), GetLastError());
			break;
        }
	} while( !this->IsFinished() ) ;
	g_LOG.CS_ODS(0xffff, "<<< CIocpTHREAD::Free ThreadID: %d(0x%x),  WorkerNO: %d \n", this->ThreadID, this->ThreadID, m_iThreadNO);

/*    
    // ** 주의 !!!
    // m_hIOCP를 공유하는 다른 워크쓰레드가 ::GetQueuedCompletionStatus에서 먼저
    // 신호를 받아서 처리되어 질수있다.
    // 본 함수가 호출되어 지더라도 this 쓰레드가 Terminate되진 않는다..
    if ( 0 == ::PostQueuedCompletionStatus( m_hIOCP, -1, 0, NULL) )
    {
        Log_String( 0xffff, "ThreadWORKER::Free() PostQueuedCompletionStatus() return 0, LastERROR: %d(0x%x)", GetLastError(), GetLastError());
    }
*/
/*
    this->WaitFor ();
    m_hIOCP = NULL;
*/
}


//-------------------------------------------------------------------------------------------------
/*
//void ThreadWORKER::STATUS_ReturnTRUE  (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey, DWORD dwBytesIO)
//{
//    if ( dwBytesIO == 0 ) {
//        // 소켓 종료
//        Log_String ("WorkerThread %d: [ %s ] dwBytesIO == 0, Close Socket () :: IONode->PoolIndex: %d \n", pParam->wThreadNO , (lpOverlapped->m_IOmode == ioREAD) ? "ioREAD" : "ioWRITE", pParam->wThreadNO, lpOverlapped->m_pPoolNode->DATA.m_iPoolIndex);
//
//        // ioWRITE 일경우에는 pUSER->m_SendList에 노드가 이미 등록되어 있어 SubUser()에서 풀림으로 ioREAD일 때만...
//        if ( lpOverlapped->m_IOmode == ioREAD ) {
//            g_pIO_DATA->ReleaseIONode ( lpOverlapped->m_pPoolNode );
//        }
//
//        pUSER->LockUSER ();
//        if ( pUSER->m_Socket != INVALID_SOCKET ) {
//            pParam->pIOCP->_CloseSocket ( pUSER->m_Socket );
//            pUSER->m_Socket = INVALID_SOCKET;
//            g_CUserLIST.SubUser ( pUSER->m_pPoolNode );			// WorkerThreadFunc
//        }
//        pUSER->UnlockUSER ();
//    } else
//    if ( pUSER->m_Socket != INVALID_SOCKET ) {
//        // ** pUSER의 소켓이 종료된 상태에서도 이곳으로 온다 ㅡㅡ;
//
//        // 정상 처리 !!!
//        lpOverlapped->m_dwIOBytes += dwBytesIO;
//
//        switch ( lpOverlapped->m_IOmode ) {
//            case ioREAD :
//                pUSER->Recv_Complete ( lpOverlapped->m_pPoolNode );
//                break;
//            case ioWRITE :	// 보내기 완료 !!!
//                pUSER->Send_Complete ( lpOverlapped->m_pPoolNode );
//                break;
//        }
//    }
//}
//-------------------------------------------------------------------------------------------------
//void ThreadWORKER::STATUS_ReturnFALSE (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey)
//{
//    Log_String (0xffff, "WorkerThread %d: [ %s ] bResult == FALSE, Close Socket () :: IONode->PoolIndex: %d \n", m_iThreadNO, (lpOverlapped->m_IOmode == ioREAD) ? "ioREAD" : "ioWRITE", lpOverlapped->m_pPoolNode->DATA.m_iPoolIndex);
//
//    // ioWRITE 일경우에는 pUSER->m_SendList에 노드가 이미 등록되어 있어 SubUser()에서 풀림으로 ioREAD일 때만...
//    if ( lpOverlapped->m_IOmode == ioREAD ) {
//        g_pIO_DATA->ReleaseIONode ( lpOverlapped->m_pPoolNode );
//    }
//
//    pUSER->LockUSER ();
//    if ( pUSER->m_Socket != INVALID_SOCKET ) {
//        pParam->pIOCP->_CloseSocket ( pUSER->m_Socket );
//        pUSER->m_Socket = INVALID_SOCKET;
//        g_CUserLIST.SubUser ( pUSER->m_pPoolNode );		// WorkerThreadFunc
//    }
//    pUSER->UnlockUSER ();
//}
*/

//-------------------------------------------------------------------------------------------------
