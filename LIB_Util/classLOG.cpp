/*
	$History: classLOG.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 04-07-16   Time: 12:42a
 * Updated in $/7HeartsOnline/LIB_Util
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-03-30   Time: 8:16p
 * Updated in $/7HeartsOnline/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:20a
 * Created in $/7HeartsOnline/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:09a
 * Created in $/SevenHearts/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:51a
 * Created in $/SevenHearts/LIB_Util/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:47a
 * Created in $/SevenHearts/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:35a
 * Created in $/SHO/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:26a
 * Created in $/SevenHearts/LIB_Util/LIB_Util
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 04-03-23   Time: 4:41p
 * Updated in $/SevenHearts/LIB_Util
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-03-23   Time: 4:30p
 * Updated in $/SevenHearts/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 03-07-03   Time: 11:38a
 * Created in $/LIB_Util
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 03-03-17   Time: 7:48p
 * Updated in $/Client/UTIL
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 03-03-17   Time: 5:21p
 * Updated in $/Client/Util
*/
#include <windows.h>
#include <stdio.h>
#include "classLOG.h"
//-------------------------------------------------------------------------------------------------

#ifdef  __BORLANDC__
extern void WriteFILE (char *szString);
#endif
extern void WriteLOG  (char *szString);

CLOG g_LOG;

char CLOG::m_StrBUFF[ LOG_STR_SIZE ];

#ifdef	_DEBUG
WORD CLOG::m_wLogMODE[LOG_MAXOUT] = {  0x0ffff, 0x0ffff, 0x0fff };
#else
WORD CLOG::m_wLogMODE[LOG_MAXOUT] = {  LOG_NORMAL, LOG_NORMAL, LOG_NORMAL };
#endif

//-------------------------------------------------------------------------------------------------
CLOG::CLOG ()
{
    ::InitializeCriticalSection( &m_csLOCK );
}
CLOG::~CLOG ()
{
	::DeleteCriticalSection( &m_csLOCK );
}

//-------------------------------------------------------------------------------------------------
void CLOG::CS_OUT (WORD wLogMODE, char *szMSG)
{
    if ( !(wLogMODE & m_wLogMODE[ LOG_DBGOUT ] ) )
        return;

    ::EnterCriticalSection( &m_csLOCK );

    ::OutputDebugString( szMSG );

    ::LeaveCriticalSection( &m_csLOCK );
}

//-------------------------------------------------------------------------------------------------
void CLOG::CS_ODS (WORD wLogMODE, char *fmt, ...)
{
    if ( !(wLogMODE & m_wLogMODE[ LOG_DBGOUT ] ) )
        return;

    ::EnterCriticalSection( &m_csLOCK );

    try {
        va_list argptr;
        va_start(argptr, fmt);
        vsprintf(m_StrBUFF,fmt,argptr);
        va_end(argptr);
    }
    catch( ... ) {
	    ::LeaveCriticalSection( &m_csLOCK );
        return;
    }

    ::OutputDebugString( m_StrBUFF );

	if ( wLogMODE & m_wLogMODE[ LOG_SCR    ] )
		WriteLOG ( m_StrBUFF );

#ifdef  __BORLANDC__
    if ( wLogMODE & m_wLogMODE[ LOG_FILE   ] )
        WriteFILE( m_StrBUFF );
#endif

    ::LeaveCriticalSection( &m_csLOCK );
}

//-------------------------------------------------------------------------------------------------
WORD CLOG::GetLogMODE (t_LOG logType)
{
    if ( logType >= 0 || logType < LOG_MAXOUT )
        return m_wLogMODE[ logType ];
    return 0;
}

//-------------------------------------------------------------------------------------------------
void CLOG::SetLogMODE (WORD wLogMODE, t_LOG logType)
{
    if ( logType >= 0 && logType < LOG_MAXOUT ) {
        WORD wNewMODE;

        if ( wLogMODE & LOG_DEBUG )
            wNewMODE = LOG_NORMAL | LOG_VERBOSE | LOG_DEBUG;
        else
        if ( wLogMODE & LOG_VERBOSE )
            wNewMODE = LOG_NORMAL | LOG_VERBOSE;
        else
        if ( wLogMODE & LOG_NORMAL )
            wNewMODE = LOG_NORMAL;
        else
            wNewMODE = 0;

        m_wLogMODE[ logType ] = wNewMODE;
    }
}

//-------------------------------------------------------------------------------------------------
void CLOG::OutputString (WORD wLogMODE, char *fmt, ...)
{
	if ( !(m_wLogMODE[LOG_SCR] & wLogMODE) && !(m_wLogMODE[LOG_FILE] & wLogMODE) )
		return;

    try {
        va_list argptr;
        va_start(argptr, fmt);
        vsprintf(m_StrBUFF,fmt,argptr);
        va_end(argptr);
    }
    catch( ... ) {
        return;
    }

#ifdef  __BORLANDC__
    if ( wLogMODE & m_wLogMODE[ LOG_FILE   ] )
        WriteFILE( m_StrBUFF );
#endif

	::OutputDebugString( m_StrBUFF );
}

//-------------------------------------------------------------------------------------------------
