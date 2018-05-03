#include "stdafx.h"
#include <windows.h>
#include "classTIME.h"

//-------------------------------------------------------------------------------------------------
void classTIME::InitStartTime ()
{
	m_i64StartTIME = GetCurrentAbsMilliSecond ();
}

//-------------------------------------------------------------------------------------------------
DWORD classTIME::GetPassAbsMilliSecond (void)
{
	INT64 i64 = GetCurrentAbsMilliSecond();

	return (DWORD)( i64-m_i64StartTIME );
}

//-------------------------------------------------------------------------------------------------
// st날짜의 절대 날짜를 구한다. 1601년 1월 1일을 기준(0)으로 경과한 날짜 수를 세준다.
DWORD classTIME::GetAbsDay (SYSTEMTIME &st)
{
	INT64 i64;
	FILETIME fst;

	st.wHour=st.wMinute=st.wSecond=st.wMilliseconds=st.wDayOfWeek=0;
	::SystemTimeToFileTime (&st, &fst);
	i64 = (((INT64)fst.dwHighDateTime) << 32 ) + fst.dwLowDateTime;
	i64 = i64 / ( 864000000000 );
	return (DWORD)i64;
}
// 절대 날짜를 시스템 타임으로 바꾼다.
void classTIME::AbsDayToSystem (DWORD dwABS, SYSTEMTIME &st)
{
	INT64 i64;
	FILETIME fst;

	i64 = dwABS * 864000000000;
	fst.dwHighDateTime = (DWORD)( i64 >> 32 );
	fst.dwLowDateTime  = (DWORD)( i64 & 0xffffffff );
	::FileTimeToSystemTime (&fst, &st);
}


//-------------------------------------------------------------------------------------------------
/*
	32비트 정수로 절대 초를 표현하기 때문에 표현 가능한 년수가 
	136년 밖에 되지 않아 부득이 기준 날짜를 FILETIME의 1601년으로
	잡지 못하고 2000년 1월 1일로 잡았다. 
	2136년 까지는 잘 써먹을수 있다...
*/
// 1day = 60sec * 60min * 24hour = 86400 sec
// st 날짜의 절대 초를 구한다. 2000년 1월 1일 자정기준
DWORD classTIME::GetAbsSecond (SYSTEMTIME &st)
{
	INT64 i64;
	FILETIME fst;

	::SystemTimeToFileTime (&st, &fst);
	i64 = (((INT64)fst.dwHighDateTime) << 32 ) + fst.dwLowDateTime;
	// 초단위로 환산 2000년 1월 1일 자정 기준으로 바꾼다.
	i64 = i64 / 10000000 - (INT64)145731 * 86400;

	return (DWORD)i64;
}

// 절대초를 시스템 타임으로 바꾼다
void classTIME::AbsSecondToSystem (DWORD dwABS, SYSTEMTIME &st)
{
	INT64 i64;
	FILETIME fst;

	i64 = (dwABS + (INT64)145731 * 86400) * 10000000;
	fst.dwHighDateTime = (DWORD)( i64 >> 32 );
	fst.dwLowDateTime  = (DWORD)( i64 & 0xffffffff );

	::FileTimeToSystemTime (&fst, &st);
}

//-------------------------------------------------------------------------------------------------
DWORD classTIME::GetCurrentAbsSecond (void)
{
	SYSTEMTIME st;

	::GetLocalTime( &st );
	return GetAbsSecond( st );
}

//-------------------------------------------------------------------------------------------------
DWORD classTIME::GetPassAbsSecond (SYSTEMTIME &st)
{
	return ( GetCurrentAbsSecond() - GetAbsSecond( st ) );
}


//-------------------------------------------------------------------------------------------------
// 1day = 60sec * 60min * 24hour = 86400 sec
// st 날짜의 절대 초를 구한다. 1601년 1월 1일을 기준(0)으로 경과한 
// 1/100초를 구한다.
INT64 classTIME::GetAbsMilliSecond (SYSTEMTIME &st)
{
	INT64 i64;
	FILETIME fst;

	::SystemTimeToFileTime (&st, &fst);
	i64 = (((INT64)fst.dwHighDateTime) << 32 ) + fst.dwLowDateTime;
	// 1/100초단위로 환산 2000년 1월 1일 자정 기준으로 바꾼다.

//	i64 = i64 / 10000000 - (INT64)145731 * 86400;
	i64 = i64 / 100000;

//	printf (" %f \n", (float)i64);

	return i64;
}
INT64 classTIME::GetCurrentAbsMilliSecond (void)
{
	SYSTEMTIME st;

	::GetLocalTime( &st );
	return GetAbsMilliSecond( st );
}

//-------------------------------------------------------------------------------------------------
CTimer::CTimer (HWND hWND, UINT_PTR uiEventID, UINT uiElapse, TIMERPROC lpTimerFunc)
{
	m_hWND = hWND;
	m_uiTimerID = 0;
	m_uiEventID   = uiEventID  ;
	m_uiElapse    = uiElapse   ;
	m_lpTimerFunc = lpTimerFunc;
}
CTimer::~CTimer ()
{
	this->Stop ();
}

//-------------------------------------------------------------------------------------------------
bool CTimer::Start ()
{
	if ( !m_uiTimerID ) {
		m_uiTimerID = ::SetTimer( m_hWND, m_uiEventID, m_uiElapse, m_lpTimerFunc );
	}

	return ( m_uiTimerID != 0 );
}

void CTimer::Stop ()
{
	if ( m_uiTimerID ) {
		::KillTimer( m_hWND, m_uiTimerID );
		m_uiTimerID = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
