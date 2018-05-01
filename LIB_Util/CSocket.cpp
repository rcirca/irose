/*
	$Header: /7HeartsOnline/LIB_Util/CSocket.cpp 1     04-03-25 11:20a Icarus $
*/

#include "winsock2.h"
#include "CSocket.h"
#include "classLOG.h"

#pragma	comment	(lib, "WSock32.lib")

//-------------------------------------------------------------------------------------------------

bool CSOCKET::Init (void)
{
    int err;

    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 1, 1 );

    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        Socket_Error ("SYSERR : WSAStartup");
        return false;
    }

    if ( LOBYTE( wsaData.wVersion ) != LOBYTE( wVersionRequested ) ||
         HIBYTE( wsaData.wVersion ) != HIBYTE( wVersionRequested ) ) {
        g_LOG.OutputString (LOG_DEBUG, "SYSERR : Invalied Socket version .......\n");
        WSACleanup( );
        return false;
    }

//	_ClearSocketList ();

    return true;
}
void CSOCKET::Free (void)
{
//	_ClearSocketList ();

    int err = WSACleanup ();
    if ( err != 0 ) {
        Socket_Error ("SYSERR : WSACleanup");
    }
}


//-------------------------------------------------------------------------------------------------
int Socket_Error (char *szMsg)
{
    int iErr;

    g_LOG.OutputString (LOG_DEBUG, szMsg);
    g_LOG.OutputString (LOG_DEBUG, " : ");

    iErr = WSAGetLastError ();

    switch ( iErr ) {
        case WSAEAFNOSUPPORT	:	g_LOG.OutputString (LOG_DEBUG, "The specified address family is not supported.");  break;
        case WSAEFAULT			:   g_LOG.OutputString (LOG_DEBUG, "The lpWSAData is not a valid pointer.\n");
									g_LOG.OutputString (LOG_DEBUG, "The buf or from parameters are not part of the user address space, or the fromlen parameter is too small to accommodate the peer address. ");	break;
        case WSAEINPROGRESS		:   g_LOG.OutputString (LOG_DEBUG, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");    break;
        case WSAEMFILE			:   g_LOG.OutputString (LOG_DEBUG, "No more socket descriptors are available.");   break;
        case WSAENETDOWN		:   g_LOG.OutputString (LOG_DEBUG, "The network subsystem or the associated service provider has failed.");    break;
        case WSAENOBUFS			:   g_LOG.OutputString (LOG_DEBUG, "No buffer space is available. The socket cannot be created."); break;
        case WSAEPROCLIM		:   g_LOG.OutputString (LOG_DEBUG, "Limit on the number of tasks supported by the Windows Sockets implementation has been reached.");  break;
        case WSAEPROTONOSUPPORT :   g_LOG.OutputString (LOG_DEBUG, "The specified protocol is not supported.");    break;
        case WSAEPROTOTYPE		:   g_LOG.OutputString (LOG_DEBUG, "The specified protocol is the wrong type for this socket.");   break;
        case WSAESOCKTNOSUPPORT :   g_LOG.OutputString (LOG_DEBUG, "The specified socket type is not supported in this address family.");  break;
        case WSANOTINITIALISED	:   g_LOG.OutputString (LOG_DEBUG, "A successful WSAStartup must occur before using this function.");  break;
        case WSASYSNOTREADY		:   g_LOG.OutputString (LOG_DEBUG, "Indicates that the underlying network subsystem is not ready for network communication."); break;
        case WSAVERNOTSUPPORTED :   g_LOG.OutputString (LOG_DEBUG, "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation."); break;

		case WSAEINTR			:	g_LOG.OutputString (LOG_DEBUG, "The (blocking) call was canceled through WSACancelBlockingCall. ");	break;
		case WSAEINVAL			:	g_LOG.OutputString (LOG_DEBUG, "The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled, or (for byte stream-style sockets only) len was zero or negative. ");	break;
		case WSAEISCONN			:	g_LOG.OutputString (LOG_DEBUG, "The socket is connected. This function is not permitted with a connected socket, whether the socket is connection-oriented or connectionless. ");	break;
		case WSAENETRESET		:	g_LOG.OutputString (LOG_DEBUG, "The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress. ");	break;
		case WSAENOTSOCK		:	g_LOG.OutputString (LOG_DEBUG, "The descriptor is not a socket. ");	break;
		case WSAEOPNOTSUPP		:	g_LOG.OutputString (LOG_DEBUG, "MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations. ");	break;
		case WSAESHUTDOWN		:	g_LOG.OutputString (LOG_DEBUG, "The socket has been shut down; it is not possible to recvfrom on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. ");	break;
		case WSAEWOULDBLOCK		:	g_LOG.OutputString (LOG_DEBUG, "The socket is marked as nonblocking and the recvfrom operation would block. ");	break;
		case WSAEMSGSIZE		:	g_LOG.OutputString (LOG_DEBUG, "The	message was too large to fit into the specified buffer and was truncated. ");	break;
		case WSAETIMEDOUT		:	g_LOG.OutputString (LOG_DEBUG, "The connection has been dropped, because of a network failure or because the system on the other end went down without notice. ");	break;
		case WSAECONNRESET		:	g_LOG.OutputString (LOG_DEBUG, "The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. On a UPD-datagram socket this error would indicate that a previous send operation resulted in an ICMP \"Port Unreachable\" message. ");	break;
        default :
            ;//LogString (LOG_DEBUG, "Unknown socket iError. CODE[ 0x%X, %d ]\n", iErr, iErr);
    }

    g_LOG.OutputString (LOG_DEBUG, "\n");

	return iErr;
}


//-------------------------------------------------------------------------------------------------
