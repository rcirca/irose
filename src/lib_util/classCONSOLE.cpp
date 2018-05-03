
// #include "stdAFX.h"
#include <VCL.h>
#include <stdio.h>
#include "classCONSOLE.h"

//-------------------------------------------------------------------------------------------------
classCONSOLE::classCONSOLE ()
{
	_COORD SBSize; 

	::InitializeCriticalSection( &m_csLOCK );
	::AllocConsole();

    m_hConsole = ::CreateConsoleScreenBuffer(GENERIC_WRITE,
                                      FILE_SHARE_WRITE,
                                      NULL,
                                      CONSOLE_TEXTMODE_BUFFER,
                                      NULL );

    SBSize.X = 256;
    SBSize.Y = 1000;

    ::SetConsoleScreenBufferSize (m_hConsole, SBSize);
    ::SetConsoleActiveScreenBuffer (m_hConsole);

	WORD wAttributes;

	wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    ::SetConsoleTextAttribute(m_hConsole , wAttributes);
}
classCONSOLE::~classCONSOLE ()
{
	::FreeConsole();
	::DeleteCriticalSection( &m_csLOCK );
}

//-------------------------------------------------------------------------------------------------
void classCONSOLE::SetColor (WORD wColor)
{
    ::SetConsoleTextAttribute(m_hConsole , wColor);
}

//-------------------------------------------------------------------------------------------------
char *classCONSOLE::Printf (WORD wColor, char *fmt, ...)
{
    this->Lock ();

    va_list	argptr;

    va_start (argptr, fmt);
    vsprintf (m_cBuffer,fmt,argptr);
    va_end(argptr);

	DWORD Chars;

    ::SetConsoleTextAttribute(m_hConsole , wColor);
    ::WriteConsole(m_hConsole, m_cBuffer, strlen( m_cBuffer ), &Chars, NULL);

    this->Unlock ();

	return m_cBuffer;
}

//-------------------------------------------------------------------------------------------------
void classCONSOLE::Puts (WORD wColor, char *pStr)
{
	DWORD Chars;

    this->Lock ();

    ::SetConsoleTextAttribute(m_hConsole , wColor);
    ::WriteConsole(m_hConsole, pStr, strlen( pStr ), &Chars, NULL);

    this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
char *classCONSOLE::Printf (char *fmt, ...)
{
    this->Lock ();

    va_list	argptr;

    va_start (argptr, fmt);
    vsprintf (m_cBuffer,fmt,argptr);
    va_end(argptr);

	DWORD Chars;

    ::WriteConsole(m_hConsole, m_cBuffer, strlen( m_cBuffer ), &Chars, NULL);
    
    this->Unlock ();

	return m_cBuffer;
}

//-------------------------------------------------------------------------------------------------
void classCONSOLE::Puts (char *pStr)
{
	DWORD Chars;

    this->Lock ();
    ::WriteConsole(m_hConsole, pStr, strlen( pStr ), &Chars, NULL);
    this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
