
#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include "BugGuard/BugGuard.h"


static DWORD s_dwVER = 0;
static char  s_szAppNAME[ MAX_PATH ] = "unknown";
static char  s_szErrFILE[ MAX_PATH ] = "Application.ERR";

//-------------------------------------------------------------------------------------------------
LONG __stdcall MyUnhandledExceptionFilter ( EXCEPTION_POINTERS* pExPtrs)
{
//	::MessageBox ( NULL, "aaa", "bbb", MB_OK );

	FILE *fp;
	char  pStr[ 1024 ];
	DWORD	dwSize;

	fp = fopen ( s_szErrFILE , "a");

	fprintf (fp, "\n\n-------------------------------------------------------------\n");

	// Program Version:		각자 알아서...
	fprintf (fp, "Program Version: %s : %d [ %s ]\n", s_szAppNAME, s_dwVER, __TIMESTAMP__);

	dwSize = sizeof( pStr );
	fprintf (fp, "Computer Name: ");
	// Computer Name:		"
	if ( GetComputerName(pStr, &dwSize) )
		fprintf (fp, "%s\n", pStr);
	else
		fprintf (fp, "unknown\n" );

	dwSize = sizeof( pStr );
	fprintf (fp, "User Name: ");
	// Computer Name:		"
	if ( GetUserName(pStr, &dwSize) )
		fprintf (fp, "%s\n", pStr);
	else
		fprintf (fp, "unknown\n" );
	

	SYSTEMTIME sST;
	::GetLocalTime (&sST);

	// Time:				"
	fprintf (fp, "Date: %d/%d/%d    Time: %d:%d:%d\n", sST.wYear, sST.wMonth,  sST.wDay, sST.wHour, sST.wMinute, sST.wSecond);

	fprintf (fp, "%s\n\n", GetExceptionReason( pExPtrs ) );
	fprintf (fp, "Reigster:\n%s\n\n",	GetRegisters ( pExPtrs ) );

	LPCSTR lpCStr;

	lpCStr = GetFirstCallStack ( pExPtrs );
	if ( lpCStr ) {
		fprintf (fp, "%s\n", lpCStr );

		while ( lpCStr = GetNextCallStack (pExPtrs ) )
			fprintf (fp, "%s\n", lpCStr );
	}

	fprintf (fp, "\n-------------------------------------------------------------\n\n");
	fclose (fp);

	return EXCEPTION_EXECUTE_HANDLER;
}


//-------------------------------------------------------------------------------------------------
void SetExceptionReport (DWORD dwVer, char *szAppName, char *szErrFile )
{
	s_dwVER = dwVer;
	strcpy( s_szAppNAME, szAppName );
	strcpy( s_szErrFILE, szErrFile );

	SetUnhandledExceptionFilter ( MyUnhandledExceptionFilter );
}


//-------------------------------------------------------------------------------------------------
