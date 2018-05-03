/*
	$Header: /7HeartsOnline/LIB_Util/CRawSOCKET.cpp 2     05-01-06 2:18p Icarus $
 */

#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>

#include "CRawSOCKET.h"


//-------------------------------------------------------------------------------------------------
CRawSOCKET::CRawSOCKET()
{
	m_hSocket = INVALID_SOCKET;
}


//-------------------------------------------------------------------------------------------------
CRawSOCKET::~CRawSOCKET()
{
	if ( m_hSocket != INVALID_SOCKET )
		Close();
}

/*
bool Socket(UINT uiWIndowMsg, 
		int nSocketType=SOCK_STREAM, 
		long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		int nProtocolType = 0, 
		int nAddressFormat = PF_INET);
*/
//-------------------------------------------------------------------------------------------------
bool CRawSOCKET::Socket(UINT uiWindowMsg, int nSocketType, long lEvent, int nProtocolType, int nAddressFormat)
{
	_ASSERT ( m_hSocket == INVALID_SOCKET );

//	nProtocolType = IPPROTO_UDP;
//	nProtocolType = IPPROTO_TCP;

	m_hSocket = socket(nAddressFormat, nSocketType, nProtocolType);
	if ( m_hSocket != INVALID_SOCKET )
		return AsyncSelect(uiWindowMsg, lEvent);

	Socket_Error ("CRawSOCKET::Socket");

	return false;
}


//-------------------------------------------------------------------------------------------------
bool CRawSOCKET::Bind(UINT uiSocketPort, LPCTSTR lpszSocketAddress)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons((u_short)uiSocketPort);

	if ( lpszSocketAddress == NULL ) {
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	} else
	{
		DWORD lResult = inet_addr(lpszSocketAddress);
		if ( lResult == INADDR_NONE )
		{
			Socket_Error ("CRawSOCKET::Bind - inet_addr");
			return false;
		}
		sockAddr.sin_addr.s_addr = lResult;
	}

	int iRetValue = bind (m_hSocket, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR_IN));
	if ( iRetValue == SOCKET_ERROR ) {
		Socket_Error ("CRawSOCKET::Bind");
		return false;
	}

	return true;
}

/*
bool Create(HWND hWnd, UINT uiWindowMsg, 
		UINT nSocketPort = 0, 
		int nSocketType=SOCK_STREAM,
		long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
		LPCTSTR lpszSocketAddress = NULL);
*/
//-------------------------------------------------------------------------------------------------
bool CRawSOCKET::Create(HWND hWnd, UINT uiWindowMsg, UINT uiSocketPort, int nSocketType, long lEvent, LPCTSTR lpszSocketAddress)
{
	m_hSocket		= INVALID_SOCKET;
	m_hSocketWindow = hWnd;

	if ( Socket(uiWindowMsg, nSocketType, lEvent) )
	{
		if ( Bind(uiSocketPort,lpszSocketAddress) ) {
			return true;
		}

		Close();
	} 

	Socket_Error ("CRawSOCKET::Create");

	return false;
}


//-------------------------------------------------------------------------------------------------
bool CRawSOCKET::AsyncSelect(UINT uiWindowMsg, long lEvent)
{
	_ASSERT( m_hSocket != INVALID_SOCKET );
	_ASSERT(m_hSocketWindow != NULL);

	int iRetValue;

	iRetValue = WSAAsyncSelect(m_hSocket, m_hSocketWindow, uiWindowMsg, lEvent);
	if ( iRetValue == SOCKET_ERROR ) {
		Socket_Error ("CRawSOCKET::AsyncSelect");
		return false;
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
bool CRawSOCKET::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	_ASSERT( lpszHostAddress != NULL );

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(lpszHostAddress);

	if ( sockAddr.sin_addr.s_addr == INADDR_NONE )
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(lpszHostAddress);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			Socket_Error ("CRawSOCKET::Connect - gethostbyname");
			return false;
		}
	}

	sockAddr.sin_port = htons((u_short)nHostPort);

	int iRetValue = connect (m_hSocket, (LPSOCKADDR)&sockAddr, sizeof(SOCKADDR_IN));
	if ( iRetValue == SOCKET_ERROR ) {
		iRetValue = WSAGetLastError ();
		if ( iRetValue != WSAEWOULDBLOCK ) {
			Socket_Error( "Connect" );
			return false;
		}
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
void CRawSOCKET::Close()
{
	if ( m_hSocket != INVALID_SOCKET )
	{
		int iRet = closesocket(m_hSocket);

		if ( iRet == SOCKET_ERROR ) {
			Socket_Error( "CRawSOCKET::Close" );
		} 
		/*
		else {
			OutputDebugString ("CRawSOCKET::Close () success...\n");
		}
		*/

		m_hSocket = INVALID_SOCKET;
	}
}


//-------------------------------------------------------------------------------------------------
bool CRawSOCKET::IOCtl(long lCommand, DWORD* lpArgument)
{
	if ( ioctlsocket (m_hSocket, lCommand, lpArgument) == SOCKET_ERROR ) {
		Socket_Error ("CRawSOCKET::IOCtl");
		return false;
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
int CRawSOCKET::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	return recv(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
}


//-------------------------------------------------------------------------------------------------
int CRawSOCKET::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	return send(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
}


//-------------------------------------------------------------------------------------------------
int CRawSOCKET::ReceiveFrom(void* lpBuf, int nBufLen, SOCKADDR_IN *pSockAddr, int nFlags)
{
	int iSockAddrLen = sizeof(SOCKADDR_IN);

	memset(pSockAddr, 0, sizeof(SOCKADDR_IN));
	int iResult = recvfrom(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, (SOCKADDR*)pSockAddr, &iSockAddrLen);
/*
	if ( nResult != SOCKET_ERROR ) {
		rSocketPort = ntohs(sockAddr.sin_port);
		rSocketAddress = inet_ntoa(sockAddr.sin_addr);
	}
*/

	return iResult;
}


//-------------------------------------------------------------------------------------------------
int CRawSOCKET::SendTo (const void* lpBuf, int nBufLen, SOCKADDR_IN *pSockAddr, int nFlags)
{
	return sendto(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, (SOCKADDR*)pSockAddr, sizeof(SOCKADDR_IN));
}


//-------------------------------------------------------------------------------------------------
int CRawSOCKET::SendTo (const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress, int nFlags)
{
	SOCKADDR_IN sockAddr;

	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port   = htons((u_short)nHostPort);

	if ( lpszHostAddress == NULL )
		sockAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	else
	{
		sockAddr.sin_addr.s_addr = inet_addr(lpszHostAddress);

		if ( sockAddr.sin_addr.s_addr == INADDR_NONE )	// URL ÀÌ¸י ...
		{
			LPHOSTENT lphost;
			lphost = gethostbyname( lpszHostAddress );
			if ( lphost != NULL )
				sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			else
			{
				WSASetLastError(WSAEINVAL);
				return SOCKET_ERROR;
			}
		}
	}

	return sendto(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
}


/////////////////////////////////////////////////////////////////////////////
// CRawSOCKET Overridable callbacks
void CRawSOCKET::OnReceive(int /*nErrorCode*/)
{
}
void CRawSOCKET::OnSend(int /*nErrorCode*/)
{
}
void CRawSOCKET::OnOutOfBandData(int /*nErrorCode*/)
{
}
void CRawSOCKET::OnAccept(int /*nErrorCode*/)
{
}
void CRawSOCKET::OnConnect(int /*nErrorCode*/)
{
}
void CRawSOCKET::OnClose(int /*nErrorCode*/)
{
}


//-------------------------------------------------------------------------------------------------
