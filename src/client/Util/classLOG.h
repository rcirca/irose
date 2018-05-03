#ifndef	__CLOG_H
#define	__CLOG_H
//-------------------------------------------------------------------------------------------------

enum t_LOG {
    LOG_SCR=0,
    LOG_FILE,
    LOG_DBGOUT,
    LOG_MAXOUT
} ;

#define LOG_NONE            0
#define LOG_NORMAL			0x01
#define LOG_VERBOSE			0x02
#define LOG_DEBUG			0x04

#define	LOG_STR_SIZE		1024
class CLOG {
private :
    static WORD	m_wLogMODE[ LOG_MAXOUT ];
	static char m_StrBUFF [ LOG_STR_SIZE ];

	CRITICAL_SECTION	m_csLOCK;

public  :
    static WORD GetLogMODE   (t_LOG logType);
	static void SetLogMODE   (WORD wLogMODE, t_LOG logType=LOG_SCR);
	static void OutputString (WORD wLogMODE, char *fmt, ...);

    CLOG ();
    ~CLOG ();

    void CS_OUT (WORD wLogMODE, char *szMSG);
    void CS_ODS (WORD wLogMODE, char *fmt, ...);         // critical OutputDebugString ...
} ;

#if	defined( _DEBUG ) || defined( _D_RELEASE )
	#define	LogString		CLOG::OutputString
	// #define	LogString		g_LOG.CS_ODS	//CLOG::OutputString
#else
    #define	LogString
#endif

#define	Log_String		LogString

extern CLOG g_LOG;

//-------------------------------------------------------------------------------------------------
#endif
