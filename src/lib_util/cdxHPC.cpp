/////////////////////////////////////////////////////////////////////
// Project Name:   [ CDX Class Library - CDX.lib ]
// Source File:    [ CDX High Performance Counter Implementation]
// Author:         [ Bil Simser - bsimser@home.com ]
// Contributions:  [ Michael Rich ]
// Revision:       [ 1.99b ]
/////////////////////////////////////////////////////////////////////

#include "cdxhpc.h"


/////////////////////////////////////////////////////////////////////
// constructor
/////////////////////////////////////////////////////////////////////
CDXHPC::CDXHPC()
{
	// init to zero
	m_freq.QuadPart = 0;
	m_restart.QuadPart = 0;
}

/////////////////////////////////////////////////////////////////////
// starts the counter
/////////////////////////////////////////////////////////////////////
BOOL CDXHPC::Start()
{
	QueryPerformanceCounter(&m_restart);
	if (!QueryPerformanceFrequency(&m_freq)) return FALSE;
	// convert it down to milliseconds
	m_freq.QuadPart /= 1000;

	m_dwLastTIME = m_dwCheckTIME = (DWORD)( m_restart.QuadPart / m_freq.QuadPart );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// returns the current # of ms since counter was started
/////////////////////////////////////////////////////////////////////
DWORD CDXHPC::GetValue()
{
    QueryPerformanceCounter(&m_current);
    return (DWORD)((m_current.QuadPart - m_restart.QuadPart) / m_freq.QuadPart);
}

/////////////////////////////////////////////////////////////////////
// waits for a specified ms since the counter was started
// note that if its already been that long since it was started
// the function would exit immediately
/////////////////////////////////////////////////////////////////////
void CDXHPC::Wait(DWORD dwMilliSecs)
{
    while (TRUE)
	{
		if (GetValue()>=dwMilliSecs) break;
	}
}

/////////////////////////////////////////////////////////////////////
// resets the counter back to zero
/////////////////////////////////////////////////////////////////////
DWORD CDXHPC::Reset()
{
    QueryPerformanceCounter(&m_current);
    m_excess.QuadPart = (m_current.QuadPart - m_restart.QuadPart) % m_freq.QuadPart;

    QueryPerformanceCounter(&m_restart);
	m_dwTmpTIME = (DWORD)( m_restart.QuadPart / m_freq.QuadPart );

    m_restart.QuadPart -= m_excess.QuadPart;

	return m_dwTmpTIME;
}

DWORD CDXHPC::GetCurrentTIME ()
{
	m_dwLastTIME = m_dwCheckTIME;

    QueryPerformanceCounter(&m_tmpcurrent);
	m_dwCheckTIME = (DWORD)( m_tmpcurrent.QuadPart / m_freq.QuadPart );

	return m_dwCheckTIME;
}

