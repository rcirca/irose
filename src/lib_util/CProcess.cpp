#include <windows.h>
#include <stdio.h>
//#include <tchar.h>
//#include <crtdbg.h>
#include <assert.h>
#include "CProcess.h"

//#pragma comment ( lib, "Pdh.lib" )
//#pragma comment ( lib, "psapi.lib" )

//---------------------------------------------------------------------------
DWORD CProcess::FindThread (DWORD dwProcessID)
{
    HANDLE hSnapshot;
    THREADENTRY32  sEntry32 = {0};
    DWORD dwTID=0;
/*
	typedef struct tagTHREADENTRY32 {  
		DWORD dwSize;  
		DWORD cntUsage;  
		DWORD th32ThreadID;  
		DWORD th32OwnerProcessID;  
		LONG tpBasePri;  
		LONG tpDeltaPri;  
		DWORD dwFlags;
	} THREADENTRY32, *PTHREADENTRY32;
*/
    hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessID);
	sEntry32.dwSize = sizeof(THREADENTRY32);
	if ( ::Thread32First( hSnapshot, &sEntry32 ) ) {
		do {
            if ( dwProcessID == sEntry32.th32OwnerProcessID ) {
            ///*
			char szTmp[ 256 ];
			sprintf( szTmp, ">>> %x == %x : %x ]]] \n", dwProcessID, sEntry32.th32OwnerProcessID, sEntry32.th32ThreadID );
			OutputDebugString ( szTmp );
            //*/
                dwTID = sEntry32.th32ThreadID;
                break;
            }

		} while( ::Thread32Next( hSnapshot, &sEntry32 ) ) ;
	}
    ::CloseHandle( hSnapshot );

	return dwTID;
}

DWORD CProcess::FindProcessWithCallBack ( CALLBACK_FINDPROCESS CallBackFunc )
{
    HANDLE hSnapshot;
    PROCESSENTRY32  sEntry32 = {0};
    DWORD dwPID=0;

    hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    sEntry32.dwSize = sizeof(PROCESSENTRY32); 

    if ( ::Process32First( hSnapshot, &sEntry32 ) ) {
        do {
			if ( !CallBackFunc( sEntry32.th32ProcessID, sEntry32.szExeFile ) )
				break;
        } while( ::Process32Next( hSnapshot, &sEntry32 ) );
    }

    ::CloseHandle( hSnapshot );
    return dwPID;
}

DWORD CProcess::FindProcess ( char *szExeName )
{
    HANDLE hSnapshot;
    PROCESSENTRY32  sEntry32 = {0};
    DWORD dwPID=0;
    /*
    typedef struct tagPROCESSENTRY32 {
        DWORD dwSize;
        DWORD cntUsage;
        DWORD th32ProcessID;
        ULONG_PTR th32DefaultHeapID;
        DWORD th32ModuleID;
        DWORD cntThreads;
        DWORD th32ParentProcessID;
        LONG pcPriClassBase;
        DWORD dwFlags;
        TCHAR szExeFile[MAX_PATH];
    } PROCESSENTRY32, *PPROCESSENTRY32;
    */
    hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    sEntry32.dwSize = sizeof(PROCESSENTRY32); 

    if ( ::Process32First( hSnapshot, &sEntry32 ) ) {
        do {
            if ( 0 == strcmpi(szExeName, sEntry32.szExeFile) ) {
                dwPID = sEntry32.th32ProcessID;
                break;
            }
        } while( ::Process32Next( hSnapshot, &sEntry32 ) );
    }

    ::CloseHandle( hSnapshot );
    return dwPID;
}


BOOL CProcess::KillProcess ( DWORD dwPID )
{
    HANDLE hProcess;

    hProcess = ::OpenProcess( PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION/* PROCESS_ALL_ACCESS */, FALSE, dwPID );
    if ( hProcess ) {
        return ::TerminateProcess( hProcess, 0 );
    }
    return FALSE;
}

//-------------------------------------------------------------------------------------------------
struct tagFindWND {
	HWND	m_hWND;
	DWORD	m_dwPID;
} ;

BOOL CALLBACK CSystem_EnumWindowsProc ( HWND hwnd, LPARAM lParam )
{
	tagFindWND *pFindWND = (tagFindWND*)lParam;
	DWORD dwTID, dwPID;
	
	dwTID= ::GetWindowThreadProcessId(hwnd, &dwPID);
	if ( dwPID == pFindWND->m_dwPID ) {
        if ( NULL == ::GetWindowLong(hwnd, GWL_HWNDPARENT) &&
            (::IsWindowVisible(hwnd) || ::IsIconic(hwnd) ) &&
            (::GetWindowLong(hwnd, GWL_EXSTYLE) & (WS_EX_WINDOWEDGE | WS_EX_APPWINDOW) ) ) {
            /*
            char szTmp[ MAX_PATH ];
            ::GetClassName( hwnd, szTmp, MAX_PATH );
            OutputDebugString( szTmp );
            */
            pFindWND->m_hWND = hwnd;
	        return FALSE;
        }
	}
	return TRUE;
}

HWND CProcess::GetWindowHandleWithProcessID (DWORD dwPID)
{
    if ( 0 == dwPID )
        return NULL;

    tagFindWND sFindWND;
    sFindWND.m_hWND = NULL;
    sFindWND.m_dwPID = dwPID;

    if ( ::EnumWindows( (WNDENUMPROC)CSystem_EnumWindowsProc, (LPARAM)( &sFindWND ) ) ) {
        ;   
    }
    
    return sFindWND.m_hWND;
}
HWND CProcess::GetWindowHandleWithFileName (char *szFileName)
{
	DWORD dwPID = FindProcess( szFileName );
    if ( 0 == dwPID )
        return NULL;
    return GetWindowHandleWithProcessID( dwPID );
}

//-------------------------------------------------------------------------------------------------
