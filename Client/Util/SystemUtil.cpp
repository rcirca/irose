#include "stdafx.h"
#include ".\systemutil.h"
#include <Tlhelp32.h>


CSystemUtil::CSystemUtil(void)
{
}

CSystemUtil::~CSystemUtil(void)
{
}

DWORD CSystemUtil::FindProcess ( char *szExeName )
{
    HANDLE hSnapshot;
    PROCESSENTRY32  sEntry32 = {0};
    DWORD dwPID=0;
    /*
    typedef struct tagPROCESSENTRY32 {
        DWORD dwSize;
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

BOOL CSystemUtil::KillProcess ( DWORD dwPID )
{
    HANDLE hProcess;
    hProcess = ::OpenProcess( PROCESS_ALL_ACCESS, TRUE, dwPID );
    if ( hProcess ) {
        return ::TerminateProcess( hProcess, 0 );
    }
    return FALSE;
}
