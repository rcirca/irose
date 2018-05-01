//---------------------------------------------------------------------------

#include "stdAFX.h"
#pragma hdrstop

#include "ThreadWORKER.h"
#pragma package(smart_init)
//-------------------------------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Unit1::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//-------------------------------------------------------------------------------------------------

__fastcall ThreadWORKER::ThreadWORKER(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    Priority = tpNormal;
    m_hIOCP = NULL;
}

void __fastcall ThreadWORKER::LogSTART ()
{
	Log_String (0xffff, ">>>> Running  WorkerThread ID: %d(0x%x),  WorkerNO: %d \n", ThreadID, ThreadID, m_iThreadNO);
}
void __fastcall ThreadWORKER::LogSTOP ()
{
    Log_String (0xffff, "<<<< Shutdown WorkerThread ID: %d(0x%x),  WorkerNO: %d \n", ThreadID, ThreadID, m_iThreadNO);
}
void __fastcall ThreadWORKER::LogERROR ()
{
    Log_String (0xffff, " ");
    Log_String (0xffff, "* ERROR:: WorkerThread ID: %d(0x%x),  WorkerNO: %d : lpOverlapped == NULL :: ThreadFunc - Illegal call to GetQueuedCompletionStatus", ThreadID, ThreadID, m_iThreadNO);
    Log_String (0xffff, " ");
}

//-------------------------------------------------------------------------------------------------
void __fastcall ThreadWORKER::Execute()
{
	BOOL		 bResult;
	DWORD		 dwBytesIO;
    DWORD        dwCompletionKey;
    LPOVERLAPPED lpOverlapped;

    Synchronize( LogSTART );

	while ( !Terminated )
	{
		bResult = ::GetQueuedCompletionStatus( m_hIOCP, &dwBytesIO,
		        			&dwCompletionKey, // (DWORD *) &pUSER,
				        	(LPOVERLAPPED *)&lpOverlapped,
                            INFINITE);

        /*
            if ( lpOverlapped != NULL && dwBytesIO == 0 ;
            // 소켓종료.
            ...
        */

		if ( bResult == FALSE ) {
			if ( lpOverlapped == NULL ) {
				// 시스템 오류 !!!

                g_LOG.CS_ODS(0xffff, "* ERROR:: WorkerThread ID: %d(0x%x),  WorkerNO: %d : lpOverlapped == NULL :: ThreadFunc - Illegal call to GetQueuedCompletionStatus", ThreadID, ThreadID, m_iThreadNO);

                Synchronize( LogERROR );
				break;
			} else {
				// 소켓 종료
                STATUS_ReturnFALSE( lpOverlapped, dwCompletionKey );
			}
		} else
        if ( dwBytesIO == -1 ) {
            // ExitThread(0);
            break;
        } else {
            STATUS_ReturnTRUE( lpOverlapped, dwCompletionKey, dwBytesIO );
        }
	}

    Synchronize( LogSTOP );
}


//-------------------------------------------------------------------------------------------------
bool __fastcall ThreadWORKER::Init (HANDLE hIOCP, int iThreadNO)
{
    m_hIOCP = hIOCP;
    m_iThreadNO = iThreadNO;

    return true;
}
void __fastcall ThreadWORKER::Free (void)
{
    this->Terminate ();

    if ( this->Suspended ) {
        this->Resume ();
    }
/*    
    else {
        // ** 주의 !!!
        // m_hIOCP를 공유하는 다른 워크쓰레드가 ::GetQueuedCompletionStatus에서 먼저
        // 신호를 받아서 처리되어 질수있다.
        // 본 함수가 호출되어 지더라도 this 쓰레드가 Terminate되진 않는다..
        if ( 0 == ::PostQueuedCompletionStatus( m_hIOCP, -1, 0, NULL) )
        {
            Log_String( 0xffff, "ThreadWORKER::Free() PostQueuedCompletionStatus() return 0, LastERROR: %d(0x%x)", GetLastError(), GetLastError());
        }
    }
*/
/*
    this->WaitFor ();
    m_hIOCP = NULL;
*/
}

//-------------------------------------------------------------------------------------------------
    /*
void ThreadWORKER::STATUS_ReturnTRUE  (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey, DWORD dwBytesIO)
{
    if ( dwBytesIO == 0 ) {
        // 소켓 종료
        Log_String ("WorkerThread %d: [ %s ] dwBytesIO == 0, CloseSocket () :: IONode->PoolIndex: %d \n", pParam->wThreadNO , (lpOverlapped->m_IOmode == ioREAD) ? "ioREAD" : "ioWRITE", pParam->wThreadNO, lpOverlapped->m_pPoolNode->DATA.m_iPoolIndex);

        // ioWRITE 일경우에는 pUSER->m_SendList에 노드가 이미 등록되어 있어 SubUser()에서 풀림으로 ioREAD일 때만...
        if ( lpOverlapped->m_IOmode == ioREAD ) {
            g_pIO_DATA->ReleaseIONode ( lpOverlapped->m_pPoolNode );
        }

        pUSER->LockUSER ();
        if ( pUSER->m_Socket != INVALID_SOCKET ) {
            pParam->pIOCP->_CloseSocket ( pUSER->m_Socket );
            pUSER->m_Socket = INVALID_SOCKET;
            g_CUserLIST.SubUser ( pUSER->m_pPoolNode );			// WorkerThreadFunc
        }
        pUSER->UnlockUSER ();
    } else
    if ( pUSER->m_Socket != INVALID_SOCKET ) {
        // ** pUSER의 소켓이 종료된 상태에서도 이곳으로 온다 ㅡㅡ;

        // 정상 처리 !!!
        lpOverlapped->m_dwIOBytes += dwBytesIO;

        switch ( lpOverlapped->m_IOmode ) {
            case ioREAD :
                pUSER->Recv_Complete ( lpOverlapped->m_pPoolNode );
                break;
            case ioWRITE :	// 보내기 완료 !!!
                pUSER->Send_Complete ( lpOverlapped->m_pPoolNode );
                break;
        }
    }
}
    */

    /*
void ThreadWORKER::STATUS_ReturnFALSE (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey)
{
    Log_String (0xffff, "WorkerThread %d: [ %s ] bResult == FALSE, CloseSocket () :: IONode->PoolIndex: %d \n", m_iThreadNO, (lpOverlapped->m_IOmode == ioREAD) ? "ioREAD" : "ioWRITE", lpOverlapped->m_pPoolNode->DATA.m_iPoolIndex);

    // ioWRITE 일경우에는 pUSER->m_SendList에 노드가 이미 등록되어 있어 SubUser()에서 풀림으로 ioREAD일 때만...
    if ( lpOverlapped->m_IOmode == ioREAD ) {
        g_pIO_DATA->ReleaseIONode ( lpOverlapped->m_pPoolNode );
    }

    pUSER->LockUSER ();
    if ( pUSER->m_Socket != INVALID_SOCKET ) {
        pParam->pIOCP->_CloseSocket ( pUSER->m_Socket );
        pUSER->m_Socket = INVALID_SOCKET;
        g_CUserLIST.SubUser ( pUSER->m_pPoolNode );		// WorkerThreadFunc
    }
    pUSER->UnlockUSER ();
}
    */

//-------------------------------------------------------------------------------------------------

