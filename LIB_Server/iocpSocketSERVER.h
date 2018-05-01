#ifndef	__IOCPSOCKETSERVER_H
#define	__IOCPSOCKETSERVER_H
#include "classIOCP.h"
#include "iocpSOCKET.h"
#include "blockLIST.h"
#include "classINDEX.h"
#include "CAcceptTHREAD.h"
#include "CIocpTHREAD.h"
#include "classTIME.h"
#include "CSLList.h"
#include "IP_Addr.h"
//-------------------------------------------------------------------------------------------------

struct tagAtkIP {
	INT64	m_biTIME;
	UINT	m_uiIP;
} ;

class IOCPSocketAcceptTHREAD;
class IOCPSocketWorkerTHREAD;
class IOCPSocketSERVER : public CCriticalSection
{
protected:
	CIOCP			m_IOCP;
	DWORD			m_dwWorkerThreadCNT;
	DWORD			m_dwMaxSocketCNT;
	CStrVAR			m_ServerName;
	CIPv4Addr		m_RefuseIP;

	IOCPSocketAcceptTHREAD *m_pAcceptTHREAD;
	IOCPSocketWorkerTHREAD**m_ppWorkerTHREAD;

	CIndexARRAY< iocpSOCKET* >		*m_pSocketIDX;
	classListBLOCK< tagBlockDATA >	*m_pBlockingIP;

	CSLList< tagAtkIP >				 m_AttackLIST;
	CDLList<iocpSOCKET*>			 m_SocketLIST;
	bool							 m_bManageSocketVerify;

public	:
	// worker thread 갯수 = CPU갯수 * btMulCPUT + cAddCPUT
	IOCPSocketSERVER (char *szName, BYTE btMulCPU, char cAddCPU, bool bManageSocketVerify);
	~IOCPSocketSERVER ();

	bool Add_RefuseIP (char *szIPFrom, char *szIPTo )	{	return m_RefuseIP.Add( szIPFrom, szIPTo );	}

	char *GetServerNAME()		{	return this->m_ServerName.Get();	}

	virtual iocpSOCKET*	AllocClientSOCKET()=0;					// 메모리할당
	virtual void InitClientSOCKET( iocpSOCKET *pCLIENT )		{	/* nop */	}	// 접속 완료.. 초기화 할거 있음 해라..
	virtual void FreeClientSOCKET( iocpSOCKET *pCLIENT )=0;		// 검증없이 메모리 해제
	virtual void ClosedClientSOCKET( iocpSOCKET *pCLIENT )=0;	// 소켓이 삭제됐다.. 알아서 메모리 해제할것...

	void LockLIST ()		{	this->Lock ();		}
	void UnlockLIST ()		{	this->Unlock ();	}

	inline iocpSOCKET* GetSOCKET( int iSocketIDX )		
	{	
		if ( m_pSocketIDX )
			return m_pSocketIDX->GetData( iSocketIDX );
		return NULL;
	}

	bool New_SOCKET (SOCKET hSocket, sockaddr_in &SockADDR);
	iocpSOCKET *Del_SOCKET ( int iSocketIDX );

	void On_TRUE (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey, DWORD dwBytesIO);
	void On_FALSE(LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey);

	int  GetUsedSocketCNT()		
	{	
		if ( m_pSocketIDX )
			return m_pSocketIDX->GetUsedSlotCNT();
		return 0;
	}
	// 모든 쓰레드 동작...
	bool Active (int iListenTCPPortNO, DWORD dwMaxSocketCount, int iKeepAliveSec);
	void Shutdown ()
	{
		this->ShutdownACCEPT ();
		this->ShutdownWORKER ();
		this->ShutdownSOCKET ();
	}

	// 접속된 소켓 모두 종료
	void ShutdownSOCKET ();

	void StartACCEPT (int iListenTCPPortNO, int iKeepAliveSec);
	void ShutdownACCEPT ();

	void StartWORKER ();
	void ShutdownWORKER ();

	DWORD GetMaxSocketCOUNT()	{	return m_dwMaxSocketCNT;	}
	void  CloseIdleSCOKET (DWORD dwIdelMilliSec);
} ;


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class IOCPSocketAcceptTHREAD : public CAcceptTHREAD
{
private :
	IOCPSocketSERVER   *m_pIOCPSocketSERVER;

    inline bool AcceptSOCKET (SOCKET hSocket, sockaddr_in &SockADDR)
	{
		return m_pIOCPSocketSERVER->New_SOCKET( hSocket, SockADDR );
	}

public  :
    IOCPSocketAcceptTHREAD(IOCPSocketSERVER *pServer) : CAcceptTHREAD ( true )				
	{	
		m_pIOCPSocketSERVER = pServer;
	}
} ;


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class IOCPSocketWorkerTHREAD : public CIocpTHREAD
{
private :
	IOCPSocketSERVER   *m_pIOCPSocketSERVER;

    inline void STATUS_ReturnTRUE  (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey, DWORD dwBytesIO)
	{
		m_pIOCPSocketSERVER->On_TRUE( lpOverlapped, dwCompletionKey, dwBytesIO );
	}
    inline void STATUS_ReturnFALSE (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey)
	{
		m_pIOCPSocketSERVER->On_FALSE( lpOverlapped, dwCompletionKey );
	}

public  :
    IOCPSocketWorkerTHREAD(IOCPSocketSERVER *pServer) : CIocpTHREAD ( true )				
	{	
		m_pIOCPSocketSERVER = pServer;
	}
} ;


//-------------------------------------------------------------------------------------------------
#endif
