#ifndef	__classSYNCOBJ_H
#define	__classSYNCOBJ_H
//-------------------------------------------------------------------------------------------------

class CSyncOBJ
{
protected:
public :
	virtual	void Lock ()=0;
	virtual void Unlock ()=0;
} ;


//-------------------------------------------------------------------------------------------------
class classEVENT
{
protected:
	HANDLE	m_Event;

public :
	classEVENT( LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName );
	virtual ~classEVENT ();

	DWORD WaitFor( DWORD dwMilliseconds );
	void  SetEvent(void);
	void  ResetEvent(void);
} ;


//-------------------------------------------------------------------------------------------------
class CCriticalSection
{
protected:
	CRITICAL_SECTION	m_CS;

public :
	CCriticalSection( DWORD dwSpinCount=0 );
	virtual ~CCriticalSection ()	{	::DeleteCriticalSection( &m_CS );			}

	void	Lock ()					{	::EnterCriticalSection( &m_CS );			}
	void	Unlock ()				{	::LeaveCriticalSection( &m_CS );			}

	HANDLE	GetOwningThread ()		{	return m_CS.OwningThread;					}

#if ( _WIN32_WINNT >= 0x0400 )
	BOOL TryLock ()					{	return ::TryEnterCriticalSection( &m_CS );	}
#endif /* _WIN32_WINNT >= 0x0400 */
} ;

//-------------------------------------------------------------------------------------------------
#endif

