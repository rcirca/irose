/*
	$Header: /7HeartsOnline/Client/Util/CRawSOCKET.h 2     04-05-27 8:32p Icarus $
*/
#ifndef	__CRawSOCKET_H
#define	__CRawSOCKET_H
#include "CSocket.h"
//-------------------------------------------------------------------------------------------------

class CRawSOCKET
{
// Construction
public:
	CRawSOCKET();
	~CRawSOCKET();
	bool Create(HWND hWnd, UINT uiWindowMsg, UINT nSocketPort = 0, int nSocketType=SOCK_STREAM,
		long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		LPCTSTR lpszSocketAddress = NULL);

// Attributes
public:
	HWND	m_hSocketWindow;
	SOCKET	m_hSocket;

	operator SOCKET() const;

	bool Socket(UINT uiWIndowMsg, int nSocketType=SOCK_STREAM, long lEvent =
		FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		int nProtocolType = 0, int nAddressFormat = PF_INET);

	BOOL SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel = SOL_SOCKET);
	BOOL GetSockOpt(int nOptionName, void* lpOptionValue, int* lpOptionLen, int nLevel = SOL_SOCKET);

	static int PASCAL GetLastError();

// Operations
public:
	virtual void Close();

	bool Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
	bool Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress);

	int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	int Send (const void* lpBuf, int nBufLen, int nFlags = 0);

	int ReceiveFrom (void* lpBuf, int nBufLen, SOCKADDR_IN *pSockAddr, int nFlags);
	int SendTo(const void* lpBuf, int nBufLen, SOCKADDR_IN *pSockAddr, int nFlags = 0);
	int SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress = NULL, int nFlags = 0);

	enum { receives = 0, sends = 1, both = 2 };
	BOOL ShutDown(int nHow = sends);

	bool IOCtl(long lCommand, DWORD* lpArgument);
	bool AsyncSelect(UINT uiWindowMsg, long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);

// Overridable callbacks
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
} ;


//-------------------------------------------------------------------------------------------------
#endif