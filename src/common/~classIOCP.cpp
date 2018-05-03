//---------------------------------------------------------------------------

#include "stdAFX.h"
#pragma hdrstop

#include <winsock2.h>
#include "classIOCP.h"
#pragma package(smart_init)

classIOCP *classIOCP::m_pInstance = NULL;


//-------------------------------------------------------------------------------------------------
DWORD classIOCP::GetWorkerThreadCount ()
{
    if ( m_pInstance )
        return m_pInstance->m_dwWorkerThreadCnt;
        
	SYSTEM_INFO	sSystemInfo;
	::GetSystemInfo (&sSystemInfo);

	return sSystemInfo.dwNumberOfProcessors * 2 + 2;
}


//-------------------------------------------------------------------------------------------------
classIOCP::classIOCP ()
{
    m_hIOCP = NULL;

	SYSTEM_INFO	sSystemInfo;
	::GetSystemInfo (&sSystemInfo);

	m_dwWorkerThreadCnt = this->GetWorkerThreadCount ();

    //----------------------- i/o complete 포트 생성.
    m_hIOCP = ::CreateIoCompletionPort(
                INVALID_HANDLE_VALUE,
                NULL,					// No prior port
                0,						// No key
                m_dwWorkerThreadCnt);	// Use default  # of threads
}
classIOCP::~classIOCP ()
{
	if ( m_hIOCP != NULL ) {
        for (DWORD dwI=0; dwI<10*m_dwWorkerThreadCnt; dwI++)
        {
            if ( 0 == ::PostQueuedCompletionStatus( m_hIOCP, -1, 0, NULL) )
            {
                Log_String( 0xffff, "ThreadWORKER::Free() PostQueuedCompletionStatus() return 0, LastERROR: %d(0x%x)", GetLastError(), GetLastError());
                break;
            }
        }

		::CloseHandle (m_hIOCP);
		m_hIOCP = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
classIOCP *classIOCP::Instance (void)
{
    if ( m_pInstance == NULL ) {
        m_pInstance = new classIOCP;

        if ( m_pInstance->m_hIOCP == NULL ) {
            Log_String (LOG_NORMAL, "CreateIoCompletionPort failed.\n");
            SAFE_DELETE( m_pInstance );
            return NULL;
        }
    }

	return m_pInstance;
}
void classIOCP::Free (void)
{
    if ( m_pInstance ) {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}


//-------------------------------------------------------------------------------------------------
bool classIOCP::AddClientSocket (SOCKET hSocket, DWORD dwCompKey)
{
    if ( m_pInstance ) {
        HANDLE hIOCP;
        hIOCP = ::CreateIoCompletionPort (
                    (HANDLE)hSocket,	    // HANDLE FileHandle,              // handle to file
                    m_pInstance->m_hIOCP,	// HANDLE ExistingCompletionPort,  // handle to I/O completion port
                    dwCompKey,			    // ULONG_PTR CompletionKey,        // completion key
                    0 );				    // DWORD NumberOfConcurrentThreads // number of threads to execute concurrently
        if ( NULL == hIOCP ) {
            g_LOG.CS_ODS (0xffff, "ERROR:: IOCP AddSocket failed...\n");
            return false;
        }

        return true;
    }

    return false;
}


//-------------------------------------------------------------------------------------------------
void classIOCP::CloseClientSocket (SOCKET hSocket)
{
	struct linger li = {0, 0};	// Default: SO_DONTLINGER

	::shutdown   (hSocket, SD_BOTH);
	::setsockopt (hSocket, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li));
	::closesocket(hSocket);
}


//-------------------------------------------------------------------------------------------------

