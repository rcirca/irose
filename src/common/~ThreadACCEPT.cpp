//---------------------------------------------------------------------------

#include "stdAFX.h"
#pragma hdrstop
#include <winsock2.h>
#include "ThreadACCEPT.h"

#include "GameSERVER.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------

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
//---------------------------------------------------------------------------

__fastcall ThreadACCEPT::ThreadACCEPT(bool CreateSuspended)
    : TThread(CreateSuspended)
{
/*
tpIdle	The thread executes only when the system is idle. Windows won't interrupt other threads to execute a thread with tpIdle priority.
tpLowest	The thread's priority is two points below normal.
tpLower	The thread's priority is one point below normal.
tpNormal	The thread has normal priority.
tpHigher	The thread's priority is one point above normal.
tpHighest	The thread's priority is two points above normal.
tpTimeCritical	The thread gets highest priority.
Warning:		Boosting the thread priority of a CPU intensive operation may “starve” other threads in the application. Only apply priority boosts to threads that spend most of their time waiting for external events.
*/
    Priority = tpLower;
    m_ListenSocket = NULL;
}
__fastcall ThreadACCEPT::~ThreadACCEPT()
{
}

void __fastcall ThreadACCEPT::LogSTART ()
{
	Log_String (0xffff, ">>>> Running AcceptThread ID : %d(0x%x) !!!\n", ThreadID, ThreadID);
	g_LOG.CS_ODS (0xffff, ">>>> Running AcceptThread ID : %d(0x%x) !!!\n", ThreadID, ThreadID);
}
void __fastcall ThreadACCEPT::LogSTOP ()
{
    Log_String (0xffff, "<<<< Shutdown AcceptThreadFunc %d(0x%x) !!!\n", ThreadID, ThreadID);
    g_LOG.CS_ODS (0xffff, "<<<< Shutdown AcceptThreadFunc %d(0x%x) !!!\n", ThreadID, ThreadID);
}

//---------------------------------------------------------------------------
void __fastcall ThreadACCEPT::Execute()
{
    SOCKET      ClientSocket = INVALID_SOCKET;
    sockaddr_in SockADDR;
    int         iAddrLEN;

    Synchronize( LogSTART );

    // m_ListenSocketÀÌ INVALID_SOCKETÀÌ µÇ¸é ·çÇÁ¸¦ Å»ÃâÇÑ´Ù.
    while ( 1 ) //!Terminated )
    {
        ClientSocket = ::accept (m_ListenSocket, (sockaddr*)&SockADDR, &iAddrLEN);
        if ( INVALID_SOCKET == ClientSocket ) {
            // If user hits Ctrl+C or Ctrl+Brk or console window is closed, the control
            // handler will close the g_sdListen socket. The above WSAAccept call will
            // fail and we thus break out the loop,
            g_LOG.CS_ODS (0xffff, "Accept return INVALID_SOCKET, LastERROR: %d(0x%x)\n", WSAGetLastError(), WSAGetLastError() );
            break;
        }

        if ( !this->AcceptSOCKET( ClientSocket, SockADDR ) ) {
            // ´õÀÌ»ó ¹ÞÀ»¼ö ¾ø´Ù.
            struct linger li = {0, 0};	// Default: SO_DONTLINGER

            ::shutdown   (ClientSocket, SD_BOTH);
            ::setsockopt (ClientSocket, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li));
            ::closesocket(ClientSocket);
        }
    }

    Synchronize( LogSTOP );
}

//---------------------------------------------------------------------------
bool __fastcall ThreadACCEPT::Init (int iTCPPort)
{
	//----------------------- Accept socket »ý¼º
	int     iRet;
    WSADATA	wsaData;

	m_ListenSocket  = INVALID_SOCKET;

	if ( (iRet = ::WSAStartup(MAKEWORD(2,2), &wsaData)) != 0 ) {
		Log_String(LOG_NORMAL, "WSAStartup failed: %d\n",iRet);
        return false;
    }

   // Create a listening socket
	m_ListenSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
   if ( m_ListenSocket == INVALID_SOCKET ) {
      Log_String(LOG_NORMAL, "WSASocket() failed with error %d\n", WSAGetLastError());
      return false;
   }

   SOCKADDR_IN sSockAddr;

   sSockAddr.sin_family		 = AF_INET;
   sSockAddr.sin_port		 = ::htons( iTCPPort );
   sSockAddr.sin_addr.s_addr = ::htonl( INADDR_ANY );

	iRet = ::bind (m_ListenSocket, (SOCKADDR*)&sSockAddr, sizeof(SOCKADDR_IN));
	if ( iRet == SOCKET_ERROR ) {
		Log_String (LOG_NORMAL, "bind failed with error :: %d", WSAGetLastError());
		return false;
	}

	// Prepare socket for listening
	iRet  = ::listen(m_ListenSocket, 5);
	if ( iRet == SOCKET_ERROR ) {
		Log_String(LOG_NORMAL, "listen() failed with error %d\n", WSAGetLastError());
		return false;
	}
    
    return true;
}

//---------------------------------------------------------------------------
void __fastcall ThreadACCEPT::Free (void)
{
    if ( m_ListenSocket != INVALID_SOCKET ) {
        ::closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
    }
    ::WSACleanup ();

    this->Terminate ();
    if ( this->Suspended )
        this->Resume ();

    // ThreadWORKER¿Í ´Þ¸® m_ListenSocket¸¦ °øÀ¯ÇÏÁö ¾ÊÀ¸¹Ç·Î
    // º» ÇÔ¼ö°¡ È£ÃâµÈÈÄ¿¡´Â ¾²·¹µå°¡ Á¾·áµÈ´Ù.
    this->WaitFor ();
}

//---------------------------------------------------------------------------
/*
bool ThreadACCEPT::AcceptSOCKET (SOCKET hSocket, sockaddr_in &SockADDR)
{
	classDLLNODE<classUSER> *pNode;

    pNode = g_CUserLIST.AddUser (ClientSocket);
    if ( pNode == NULL ) {
        // »ç¿ëÀÚ ¸®½ºÆ®¿¡ ´õÀÌ»ó µî·ÏÇÒ¼ö ¾ø´Ù..
        ::closesocket (ClientSocket);
    } else
    if ( !pServer->_AddSocket (ClientSocket, (DWORD)&pNode->DATA) ) {
        ::closesocket (ClientSocket);
        g_CUserLIST.SubUser (pNode);	// AcceptThreadFunc
    } else {
        pNode->DATA.Recv_Start ();		// AcceptThreadFunc
    }

    return false;
}
*/
//---------------------------------------------------------------------------

