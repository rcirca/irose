#ifndef _H_CDXHPC
#define _H_CDXHPC

#define WIN32_EXTRA_LEAN
#include <windows.h>

class CDXHPC
{
private  :
	DWORD	m_dwTmpTIME;
	DWORD	m_dwCheckTIME;
	DWORD	m_dwLastTIME;

protected:
	// counter variables
	LARGE_INTEGER m_freq, m_restart;
	LARGE_INTEGER m_current, m_excess;
	LARGE_INTEGER m_tmpcurrent;

public:
	CDXHPC();
	virtual ~CDXHPC() {};

	// starts the counter
	BOOL Start();
	// returns the current # of ms since counter was started
	DWORD GetValue();
	// waits for a specified ms since the counter was started
	// note that if its already been that long since it was started
	// the function would exit immediately
	void Wait(DWORD dwWaitms);
	// resets the counter back to zero
	DWORD Reset();

	DWORD GetCheckTime()			{	return m_dwCheckTIME;						}
	DWORD GetCurrentTIME ();
	DWORD GetPassTIME ()			{	return ( m_dwCheckTIME - m_dwLastTIME );	}
};

#endif		/* #ifndef _H_CDXHPC */
