
#include <windows.h>
#include <process.h>
#include "classLOG.h"
#include "classTHREAD.h"


//-------------------------------------------------------------------------------------------------
/*
function ThreadProc(Thread: TThread): Integer;
var
  FreeThread: Boolean;
begin
  try
    Thread.Execute;
  finally
    FreeThread := Thread.FFreeOnTerminate;
    Result := Thread.FReturnValue;
    Thread.FFinished := True;
    Thread.DoTerminate;
    if FreeThread then Thread.Free;
    EndThread(Result);
  end;
end;
*/
#ifdef  __BORLANDC__
#include <tchar.h>
#include <crtdbg.h>
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
classTHREAD::classTHREAD(bool CreateSuspended) : TThread(CreateSuspended)
{
    ;
}
BOOL classTHREAD::SetPriority( int iPriority)
{
/*
tpIdle	        The thread executes only when the system is idle. Windows won't interrupt other threads to execute a thread with tpIdle priority.
tpLowest	    The thread's priority is two points below normal.
tpLower	        The thread's priority is one point below normal.
tpNormal	    The thread has normal priority.
tpHigher	    The thread's priority is one point above normal.
tpHighest	    The thread's priority is two points above normal.
tpTimeCritical	The thread gets highest priority.
Warning:		Boosting the thread priority of a CPU intensive operation may 뱒tarve?other threads in the application. Only apply priority boosts to threads that spend most of their time waiting for external events.
*/

/*
THREAD_PRIORITY_ABOVE_NORMAL    Indicates 1 point above normal priority for the priority class.
THREAD_PRIORITY_BELOW_NORMAL    Indicates 1 point below normal priority for the priority class.
THREAD_PRIORITY_HIGHEST         Indicates 2 points above normal priority for the priority class.
THREAD_PRIORITY_IDLE            Indicates a base priority level of 1 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority level of 16 for REALTIME_PRIORITY_CLASS processes.
THREAD_PRIORITY_LOWEST          Indicates 2 points below normal priority for the priority class.
THREAD_PRIORITY_NORMAL          Indicates normal priority for the priority class.
THREAD_PRIORITY_TIME_CRITICAL   Indicates a base priority level of 15 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority level of 31 for REALTIME_PRIORITY_CLASS processes.
*/
    switch( iPriority ) {
        case THREAD_PRIORITY_ABOVE_NORMAL :
            Priority = tpHigher;            break;
        case THREAD_PRIORITY_BELOW_NORMAL :
            Priority = tpLower;             break;
        case THREAD_PRIORITY_HIGHEST :
            Priority = tpHighest;           break;
        case THREAD_PRIORITY_IDLE :
            Priority = tpIdle;              break;
        case THREAD_PRIORITY_LOWEST :
            Priority = tpLowest;            break;
        case THREAD_PRIORITY_NORMAL :
            Priority = tpNormal;            break;
        case THREAD_PRIORITY_TIME_CRITICAL :
            Priority = tpTimeCritical;      break;
        default :
            return FALSE;
    }
    return TRUE;
}
#else
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//DWORD WINAPI ThreadFunc (LPVOID lpParameter)
unsigned __stdcall ThreadFunc( void* lpParameter )
{
	classTHREAD *pCThread = (classTHREAD*)lpParameter;

	if ( !pCThread->Terminated )
		pCThread->Execute ();

	pCThread->m_bFinished = true;
	return 0;

//	DWORD dwResult;		// 아래 함수들을 사용해서 삽질을 -_-;
//	::GetExitCodeThread( pCThread->m_hThread, &dwResult );
//	::ExitThread ( dwResult );
//	return dwResult;
}


//-------------------------------------------------------------------------------------------------
classTHREAD::classTHREAD (bool bCreateSuspended)
{
	m_hThread = INVALID_HANDLE_VALUE;

	this->m_bFinished = false;
	this->Terminated  = true;

	DWORD dwCreationFlags=0;
	m_bSuspended = bCreateSuspended;
	if ( bCreateSuspended )
		dwCreationFlags = CREATE_SUSPENDED;

//	m_hThread = ::CreateThread(NULL, 0, ThreadFunc, this, dwCreationFlags, &this->ThreadID);
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, dwCreationFlags, &this->ThreadID);
	if ( m_hThread != NULL ) {
        ::SetThreadPriority (m_hThread, THREAD_PRIORITY_NORMAL);
        this->Terminated = false;
        return;
	}

        // Log_String ("CreateThread failed to create socket accept thread: ERROR_CODE[ %d ]\n", GetLastError());
	if ( m_hThread != INVALID_HANDLE_VALUE ) 
		::CloseHandle( m_hThread ); 

	m_hThread = INVALID_HANDLE_VALUE;
}
classTHREAD::~classTHREAD ()
{
	if ( !m_bFinished && !m_bSuspended ) {
		this->Terminate ();
		this->WaitFor ();
	}

	if ( m_hThread ) {
		::CloseHandle( m_hThread );
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

//-------------------------------------------------------------------------------------------------
void classTHREAD::Terminate ()
{
	this->Terminated = true;
	if ( m_bSuspended ) {
		this->Resume ();
	}
}
/*
function TThread.WaitFor: LongWord;
var
  Msg: TMsg;
  H: THandle;
begin
  H := FHandle;
  if GetCurrentThreadID = MainThreadID then
    while MsgWaitForMultipleObjects(1, H, False, INFINITE,
      QS_SENDMESSAGE) = WAIT_OBJECT_0 + 1 do PeekMessage(Msg, 0, 0, 0, PM_NOREMOVE)
  else WaitForSingleObject(H, INFINITE);
  GetExitCodeThread(H, Result);
end;
*/
int	classTHREAD::WaitFor ()
{
	DWORD dwExitCode;

	::WaitForSingleObject( m_hThread, INFINITE );
	::GetExitCodeThread( m_hThread, &dwExitCode );
	return 0;
}

DWORD classTHREAD::Suspend ()
{	
	m_bSuspended = true;
	return	::SuspendThread( m_hThread );
}
DWORD classTHREAD::Resume ()
{	
	DWORD dwReturn = ::ResumeThread( m_hThread );

	if ( dwReturn == 1 )
		m_bSuspended = false;

	return dwReturn;
}
void classTHREAD::SetSuspended (bool bValue)
{
	if ( bValue != m_bSuspended ) {
	    if ( bValue )
			this->Suspend ();
		else
			this->Resume ();
	}
}
#endif      // __BORLANDC__
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
