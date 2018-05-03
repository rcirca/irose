#include "stdafx.h"
#include "ErrorEtc.h"
#include <stdio.h>
#include <stdarg.h>

/***********************************************************************************
 * OuputDebugString Printf ¹öÁ¯
 */
void ODS (char *fmt, ...)
{
    char m_StrBUFF[ 2048 ];
	
    try {
        va_list argptr;
        va_start(argptr, fmt);
        vsprintf(m_StrBUFF,fmt,argptr);
        va_end(argptr);
    }
    catch( ... ) {
        return;
    }

    ::OutputDebugString( m_StrBUFF );
}

DWORD CErrorInfo::m_dwLastErrorCode = VERROR_NONE;


void CErrorInfo::Refresh ( void )
{
	m_dwLastErrorCode = ::GetLastError ();
}

DWORD CErrorInfo::GetError ( void )
{
	return m_dwLastErrorCode;
}
