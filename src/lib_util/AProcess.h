#ifndef	__CPROCESS_H
#define	__CPROCESS_H
#include <psapi.h>
#include <Tlhelp32.h>
//-------------------------------------------------------------------------------------------------

typedef	bool (__stdcall *CALLBACK_FINDPROCESS) ( DWORD dwPID, char *szFileName );

class CProcess {
public :
	static DWORD	FindThread (DWORD dwProcessID);

	static DWORD    FindProcessWithCallBack ( CALLBACK_FINDPROCESS CallBackFunc );

    static DWORD	FindProcess ( char *szExeName );
    static BOOL	    KillProcess ( DWORD dwPID );
    static HWND     GetWindowHandleWithProcessID (DWORD dwPID);
	static HWND		GetWindowHandleWithFileName (char *szFileName);
} ;

//-------------------------------------------------------------------------------------------------
#endif