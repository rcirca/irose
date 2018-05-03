#ifndef	__CLASSCONSOLE_H
#define __CLASSCONSOLE_H
//-------------------------------------------------------------------------------------------------

/*
FOREGROUND_BLUE, 
FOREGROUND_GREEN, 
FOREGROUND_RED, 
FOREGROUND_INTENSITY, 

BACKGROUND_BLUE, 
BACKGROUND_GREEN, 
BACKGROUND_RED, 
BACKGROUND_INTENSITY
*/
#define CONSOLE_BRIGHT		FOREGROUND_INTENSITY
#define CONSOLE_WHITE	  ( FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE )
#define CONSOLE_RED			FOREGROUND_RED
#define CONSOLE_GREEN		FOREGROUND_GREEN
#define CONSOLE_BLUE		FOREGROUND_BLUE
#define	CONSOLE_YELLOW	  ( FOREGROUND_RED|FOREGROUND_GREEN )

class classCONSOLE {
private :
	HANDLE	            m_hConsole;
	char	            m_cBuffer[ 1024 ];

	CRITICAL_SECTION	m_csLOCK;
    void    Lock ()     {   ::EnterCriticalSection( &m_csLOCK );    }
    void    Unlock ()   {   ::LeaveCriticalSection( &m_csLOCK );    }

public  :
	classCONSOLE ();
	~classCONSOLE ();

	void  SetColor (WORD wColor);		// ex) wColor = FOREGROUND_BLUE | BACKGROUND_GREEN;
	char *Printf (char *fmt, ...);
	char *Printf (WORD wColor, char *fmt, ...);
	void  Puts   (char *pStr);
	void  Puts   (WORD wColor, char *pStr);
} ;

//-------------------------------------------------------------------------------------------------
#endif
