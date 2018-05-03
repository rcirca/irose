
//#define UNICODE
//#define _UNICODE

#include <windows.h>
#include <stdio.h>
//#include <tchar.h>
//#include <crtdbg.h>
#include <assert.h>
#include <psapi.h>
#include "classSYS.h"


//#pragma comment ( lib, "Pdh.lib" )
//#pragma comment ( lib, "psapi.lib" )

//---------------------------------------------------------------------------
CPDH::CPDH ()
{
	m_hQuery = NULL;
//	m_hCounter = NULL;
}
CPDH::~CPDH ()
{
        /*
	if ( m_hCounter )
		::PdhRemoveCounter( m_hCounter ); 
        */
	if ( m_hQuery )
		::PdhCloseQuery( m_hQuery );
}

//---------------------------------------------------------------------------
bool CPDH::Init( int iCpuOrder )
{
	if ( m_hQuery )
		return false;

	m_pdhStatus = ::PdhOpenQuery (0, 0, &m_hQuery);
    if ( m_pdhStatus == ERROR_SUCCESS ) {
        char szCounterPath[45] = "\\Processor(_TOTAL)\\% Processor Time";
        if ( iCpuOrder >= 0 ) {
            sprintf( szCounterPath, "\\Processor(%d)\\% Processor Time", iCpuOrder );
        }
        m_pdhStatus = ::PdhAddCounter (m_hQuery,
                                  szCounterPath,
                                  0,
                                  &m_hCounter);
    }
    
	return m_pdhStatus == ERROR_SUCCESS;
}
long CPDH::GetValue ()
{
	m_pdhStatus = ::PdhCollectQueryData( m_hQuery );
    if ( m_pdhStatus == ERROR_SUCCESS ) {
        m_pdhStatus = ::PdhGetFormattedCounterValue (m_hCounter,
                                                PDH_FMT_LONG,//DOUBLE,
                                                (LPDWORD)NULL,
                                                &m_fmtValue);
        if ( m_pdhStatus == ERROR_SUCCESS )
            return m_fmtValue.longValue;//doubleValue;
    }
    return -1;
}

//---------------------------------------------------------------------------
/*
CSystem* CSystem::m_pInstance = NULL;
CSystem::CSystem ()
{
	m_pPDHs = NULL;
}
CSystem::~CSystem ()
{
	SAFE_DELETE_ARRAY( m_pPDHs );
}
CSystem* CSystem::Instance ()
{
	if ( NULL == m_pInstance ) {
		::GetSystemInfo (&m_SystemInfo);

		m_pPDHs = new CPDH[ GetCPUCount()+1 ];
		for (int iC=0; iC<GetCPUCount();iC++)
			m_pPDSs[ iC ].Init( iC );

		m_pDSs[ GetCPUCount() ].Init( -1 );
	}

	return m_pInstance;
}
*/

//-------------------------------------------------------------------------------------------------
DWORD CSystem::GetCPUCount ()
{	
	SYSTEM_INFO	 SystemInfo;
	::GetSystemInfo (&SystemInfo);
	return SystemInfo.dwNumberOfProcessors;
}

//-------------------------------------------------------------------------------------------------
DWORD CSystem::GetOSPlatfrom ()
{
	OSVERSIONINFO	sVI;

	memset( &sVI, 0, sizeof(OSVERSIONINFO) );
	sVI.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if ( FALSE ==  ::GetVersionEx( &sVI ) ) {
		return 0;
	}

	//switch( sVI.dwPlatformId ) {
	//	case VER_PLATFORM_WIN32_NT			//  Windows NT, Windows 2000, Windows XP, or Windows Server 2003 family. 
	//	case VER_PLATFORM_WIN32s		:	//  Win32s on Windows 3.1. 
	//	case VER_PLATFORM_WIN32_WINDOWS	:	//  Windows 95, Windows 98, or Windows Me. 
	//		break;
	//}

	return sVI.dwPlatformId;
}
bool CSystem::IsNTPlatfrom ()
{
	DWORD dwVersion = ::GetVersion();
	return ( dwVersion < 0x80000000 );
	 
	//// Get the Windows version.
	//DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	//DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	//// Get the build number.
	//if (dwVersion < 0x80000000)              // Windows NT
	//	dwBuild = (DWORD)(HIWORD(dwVersion));
	//else if (dwWindowsMajorVersion < 4)      // Win32s
	//	dwBuild = (DWORD)(HIWORD(dwVersion) & ~0x8000);
	//else                                     // Windows Me/98/95
	//	dwBuild =  0;
}

//-------------------------------------------------------------------------------------------------
DWORD CSystem::GetProcessMemory (DWORD dwPID)
{
	PPROCESS_MEMORY_COUNTERS pmc;
	int cb;
	DWORD dwSize;
	cb = sizeof(_PROCESS_MEMORY_COUNTERS);
//	GetMem(pmc, cb);
	pmc = new _PROCESS_MEMORY_COUNTERS;
	pmc->cb = cb;
	if ( ::GetProcessMemoryInfo( (HANDLE)dwPID/* GetCurrentProcess() */, pmc, cb) ) {
		dwSize = pmc->WorkingSetSize;
		//Label1.Caption := IntToStr(pmc^.WorkingSetSize) + ' Bytes'
	} else {
		//Label1.Caption := 'Unable to retrieve memory usage structure';
		dwSize = 0;
	}
	delete pmc;
//	FreeMem(pmc);
/*
	// Works only on Windows NT systems (WinNT, Win2000, WinXP)
	uses psAPI;

	procedure TForm1.Button1Click(Sender: TObject);
	var
	pmc: PPROCESS_MEMORY_COUNTERS;
	cb: Integer;
	begin
	cb := SizeOf(_PROCESS_MEMORY_COUNTERS);
	GetMem(pmc, cb);
	pmc^.cb := cb;
	if GetProcessMemoryInfo(GetCurrentProcess(), pmc, cb) then
		Label1.Caption := IntToStr(pmc^.WorkingSetSize) + ' Bytes'
	else
		Label1.Caption := 'Unable to retrieve memory usage structure';

	FreeMem(pmc);
	end;
*/
	return dwSize;
}

//-------------------------------------------------------------------------------------------------
DWORD CSystem::ShowLastError( LPSTR lpszFunction ) 
{ 
    CHAR szBuf[80]; 
    DWORD dw = GetLastError(); 
 
    sprintf(szBuf, "%s failed: GetLastError returned %u\n", 
        lpszFunction, dw); 
 
    MessageBox(NULL, szBuf, "Error", MB_OK); 
    ExitProcess(dw); 

	return dw;
}

//-------------------------------------------------------------------------------------------------
