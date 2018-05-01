//////////////////////////////////////////////////////////////////////////////////////
//
// Written by Zoltan Csizmadia, zoltan_csizmadia@yahoo.com
// For companies(Austin,TX): If you would like to get my resume, send an email.
//
// The source is free, but if you want to use it, mention my name and e-mail address
//
// History:
//    1.0      Initial version                  Zoltan Csizmadia
//
//////////////////////////////////////////////////////////////////////////////////////
//
// ExtendedTrace.h
//

#ifndef EXTENDEDTRACE_H_INCLUDED
#define EXTENDEDTRACE_H_INCLUDED
//-------------------------------------------------------------------------------------------------

#if defined(_DEBUG) && defined(WIN32)
//-------------------------------------------------------------------------------------------------
	#include <windows.h>
	#include <tchar.h>

	#pragma comment( lib, "imagehlp.lib" )

	#if defined(_AFX) || defined(_AFXDLL)
	#define TRACEF									         TRACE
	#else
	#define TRACEF									         OutputDebugStringFormat
	void OutputDebugStringFormat( LPCTSTR, ... );
	#endif

	#define TRACE_INIT( IniSymbolPath )						Trace_INIT( IniSymbolPath )
	#define TRACE_FREE()									Trace_FREE()
	#define TRACE_SOURCECUSTOM( Msg, File, Line)			Trace_SOURCE( Msg, File, Line )
	#define TRACE_SOURCE( Msg )								Trace_SOURCE( Msg, __FILE__, __LINE__ )
	#define TRACE_PARAM()							        Trace_PARAM()
	#define TRACE_STACKMSG( Msg )					        Trace_STACK( GetCurrentThread(), Msg )
	#define TRACE_STACK()							        Trace_STACK( GetCurrentThread(), _T("") )
	#define TRACE_THREADSTACKMSG( hThread, Msg )			Trace_STACK( hThread, Msg )
	#define TRACE_THREADSTACK( hThread )					Trace_STACK( hThread, _T("") )

	BOOL Trace_INIT( PCSTR );
	BOOL Trace_FREE();
	void Trace_SOURCE( LPCTSTR, LPCTSTR, ULONG );
	void Trace_STACK( HANDLE, LPCTSTR );
	void Trace_PARAM();
//-------------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------------
	#define TRACE_INIT( IniSymbolPath )						((void)0)
	#define TRACE_FREE()									((void)0)
	#define TRACEF									        ((void)0)
	#define TRACE_SOURCECUSTOM( Msg, File, Line)			((void)0)
	#define TRACE_SOURCE( Msg )								((void)0)
	#define TRACE_PARAM()							        ((void)0)
	#define STACKTRACEMSG( Msg )					        ((void)0)
	#define TRACE_STACKMSG( Msg )					        ((void)0)
	#define TRACE_STACK()						         	((void)0)
	#define TRACE_THREADSTACKMSG( hThread, Msg )			((void)0)
	#define TRACE_THREADSTACK( hThread )					((void)0)
//-------------------------------------------------------------------------------------------------
#endif

//-------------------------------------------------------------------------------------------------
#endif


/*
#include "ExtendedTrace.h"

void g ( LPTSTR )
{
	// Dumps the actual call stack
	TRACE_STACK();
}

void f( int, int, int )
{
	// What parameters was this function called with?
	TRACE_PARAM();
	g( NULL );
}

int main( int, char** )
{
	// Just like the TRACE(...) in MFC
	TRACEF( _T("Application started at %d\n"), clock() );

	// Initializes the symbol files
	TRACE_INIT( NULL );

	// Trace message with a link to this line in the source code
	TRACE_SOURCE( _T("I'm calling f(...)\n") );

	f( 1, 2, 3);

	// Cleaning up
	TRACE_FREE();

	TRACEF( _T("Application ended at %d\n"), clock() );

	return 0;
}

The output in the Output Window... 
Application started at 578
Loaded 'D:\WINNT\system32\dbghelp.dll', no matching symbolic information found.
c:\temp\magictrace\main.cpp(28) : I'm calling f(...)
Function info(thread=0x36C) : void f(int=0x00000001,int=0x00000002,int=0x00000003)
Call stack info(thread=0x36C) : 
     c:\temp\magictrace\main.cpp(12) : void g(char *=0x00000000)
     c:\temp\magictrace\main.cpp(19) : void f(int=0x00000001,int=0x00000002,int=0x00000003)
     c:\temp\magictrace\main.cpp(30) : main(int=0x00000001,TCHAR * *=0x00522C88)
     crtexe.c(338) : mainCRTStartup()
     KERNEL32!0x77E87903 : SetUnhandledExceptionFilter
Application ended at 1171
The thread 0x36C has exited with code 0 (0x0).

*/