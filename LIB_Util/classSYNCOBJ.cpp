
#define	_WIN32_WINNT	0x0500

#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "classSYNCOBJ.h"


//-------------------------------------------------------------------------------------------------
classEVENT::classEVENT( LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName )
{
/*
HANDLE CreateEvent(
  LPSECURITY_ATTRIBUTES lpEventAttributes, // SD
  BOOL bManualReset,                       // reset type
  BOOL bInitialState,                      // initial state
  LPCTSTR lpName                           // object name
);*/
	m_Event = ::CreateEvent( lpEventAttributes, bManualReset, bInitialState, lpName );
	if ( NULL == m_Event ) {
		DWORD dwErrCode;
		dwErrCode = ::GetLastError ();
	} else {
		_ASSERT( ::GetLastError() != ERROR_ALREADY_EXISTS );
	}
}

classEVENT::~classEVENT ()
{
	if ( m_Event ) {
		::CloseHandle( m_Event );
	}
}

/*
function TEvent.WaitFor(Timeout: DWORD): TWaitResult;
begin
  case WaitForSingleObject(Handle, Timeout) of
    WAIT_ABANDONED: Result := wrAbandoned;
    WAIT_OBJECT_0: Result := wrSignaled;
    WAIT_TIMEOUT: Result := wrTimeout;
    WAIT_FAILED:
      begin
        Result := wrError;
        FLastError := GetLastError;
      end;
  else
    Result := wrError;    
  end;
end;
*/
//-------------------------------------------------------------------------------------------------
DWORD classEVENT::WaitFor ( DWORD dwMilliseconds )
{
	DWORD dwResult = ::WaitForSingleObject( m_Event, dwMilliseconds );
	return dwResult;
}
void classEVENT::SetEvent(void)
{
	::SetEvent( m_Event );
}
void classEVENT::ResetEvent(void)
{
	::ResetEvent( m_Event );
}


//-------------------------------------------------------------------------------------------------
CCriticalSection::CCriticalSection( DWORD dwSpinCount )
{
	if ( dwSpinCount ) {
		#if !defined(_WIN32_WINNT) || ( _WIN32_WINNT < 0x0500 )
			#error >>> ERROR :: _WIN32_WINNT >= 0x0500 !!!
		#endif
		::InitializeCriticalSectionAndSpinCount( &m_CS, dwSpinCount );
	} else
		::InitializeCriticalSection( &m_CS );		
}
//-------------------------------------------------------------------------------------------------
