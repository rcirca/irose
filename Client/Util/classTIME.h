#ifndef	__CLASSTIME_H
#define	__CLASSTIME_H
//-------------------------------------------------------------------------------------------------

class classTIME {
private:
	INT64	m_i64StartTIME;

public :
	void	InitStartTime ();
	// InitStartTime() 호출후 현재까지 경과된 1/100 초를 구한다.
	// InitStartTime() 호출후 최대 497 Day를 경과하면 안된다.
	DWORD	GetPassAbsMilliSecond (void);

	// st날짜의 절대 날짜를 구한다. 1601년 1월 1일을 기준(0)으로 경과한 날짜 수를 세준다.
	static DWORD	GetAbsDay (SYSTEMTIME &st);
	// 절대 날짜를 시스템 타임으로 바꾼다.
	static void		AbsDayToSystem (DWORD dwABS, SYSTEMTIME &st);

	// st 날짜의 절대 초를 구한다. 2000년 1월 1일 자정기준
	static DWORD	GetAbsSecond (SYSTEMTIME &st);

	// 절대초를 시스템 타임으로 바꾼다
	static void		AbsSecondToSystem (DWORD dwABS, SYSTEMTIME &st);

	static DWORD	GetCurrentAbsSecond (void);
	static DWORD	GetPassAbsSecond (SYSTEMTIME &st);

	static INT64	GetAbsMilliSecond (SYSTEMTIME &st);
	static INT64	GetCurrentAbsMilliSecond (void);
} ;

class CTimer {
protected:
	HWND		m_hWND;
	UINT_PTR	m_uiTimerID;
	UINT_PTR	m_uiEventID;
	UINT		m_uiElapse;
	TIMERPROC	m_lpTimerFunc;

public :
	CTimer (HWND hWND, UINT_PTR uiEventID, UINT uiElapse, TIMERPROC lpTimerFunc);
	virtual ~CTimer ();

	bool Start ();
	void Stop ();

	UINT_PTR GetEventID ()	{	return m_uiEventID;		}
} ;

//-------------------------------------------------------------------------------------------------
#endif
