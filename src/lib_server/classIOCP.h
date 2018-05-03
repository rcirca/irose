#ifndef classIOCPH
#define classIOCPH
// #include <ScktComp.hpp>
/*
#include "DLLIST.h"
#include "classHASH.h"
#include "classINDEX.h"
#include "classSYNCOBJ.h"
#include "iocpSOCKET.h"
*/
//---------------------------------------------------------------------------

class CIOCP
{
private:
	HANDLE	m_hIOCP;

public :
	CIOCP ()				{	m_hIOCP=NULL;		}
	~CIOCP ()				{	this->ClosePort();	}

	HANDLE	GetHANDLE()		{	return m_hIOCP;		}

	bool OpenPort (DWORD dwWorkerThreadCNT=0)	// 0 is default 
	{		
		this->ClosePort ();
		m_hIOCP = ::CreateIoCompletionPort(	INVALID_HANDLE_VALUE,
											NULL,					// No prior port
											0,						// No key
											dwWorkerThreadCNT);		// Use default  # of threads
		return ( NULL != m_hIOCP );
	}

	void ClosePort ()
	{
		if ( m_hIOCP ) {
			::CloseHandle (m_hIOCP);
			m_hIOCP = NULL;
		}
	}

	bool LinkPort (HANDLE hFileHandle, ULONG_PTR ulCompletionKey)
	{
        return ( NULL != ::CreateIoCompletionPort (
							(HANDLE)hFileHandle,	// HANDLE FileHandle,              // handle to file
							this->m_hIOCP,			// HANDLE ExistingCompletionPort,  // handle to I/O completion port
							ulCompletionKey,		// ULONG_PTR CompletionKey,        // completion key
							0 ) );				    // DWORD NumberOfConcurrentThreads // number of threads to execute concurrently
	}
} ;


/*
#define	DEFAULT_CLIENT_SOCKET		( 1024 * 32 )

class classIOCP	: private CCriticalSection
{
private:
	HANDLE						m_hIOCP;
	DWORD						m_dwWorkerThreadCnt;

	DWORD						m_dwMaxSocket;
	CIndexARRAY< iocpSOCKET* > *m_pClientIDX;

    static  classIOCP		   *m_pInstance;

protected:
    classIOCP (DWORD dwMaxClientCount);
    ~classIOCP ();

public :
	static  classIOCP *Instance (DWORD dwMaxClientCount=DEFAULT_CLIENT_SOCKET);
    static  void       Free ();

    static  DWORD	GetWorkerThreadCount ();
            HANDLE	GetHANDLE ()							{   return m_hIOCP;				}
			void	CloseIOCP ();

	static	void	LockSocketIDX ()						{	m_pInstance->Lock ();		}
	static	void	UnlockSocketIDX ()						{	m_pInstance->Unlock ();		}

	static  iocpSOCKET* GetSocket (int iSocketIDX)			{	return m_pInstance->m_pClientIDX->GetData(iSocketIDX);	}
	static	DWORD		GetMaxSocket ()						{	return m_pInstance->m_dwMaxSocket;						}

	static  int			AddClientSocket (iocpSOCKET *pSOCKET, SOCKET hSocket, char *szIP, t_HASHKEY ipHashKEY);
	static	iocpSOCKET* DelClientSocket (int iSocketIDX);
} ;
*/

//---------------------------------------------------------------------------
#endif
