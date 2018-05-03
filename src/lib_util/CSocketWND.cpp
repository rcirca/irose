
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>

#include "LIB_Util.h"
#include "classTHREAD.h"
#include "classLOG.h"
#include "../client/util/CClientSOCKET.h"
#include "../client/util/CSocketWND.h"

CSocketWND *CSocketWND::m_pInstance = NULL;


//-------------------------------------------------------------------------------------------------
#ifdef	__USE_MESSAGE_PUMP
class CSocketEVENT : public classTHREAD 
{
private :
	HWND	m_hWnd;

public :
	CSocketEVENT (HWND hWnd);
	void Execute ();
} ;
CSocketEVENT::CSocketEVENT( HWND hWnd ) : classTHREAD( true )
{
	m_hWnd = hWnd;
}
void CSocketEVENT::Execute ()
{
	// Window Message Pumpper !!!
	MSG		sMSG;
    BOOL	bRet; 

	g_LOG.CS_ODS( 0xffff, ">  > >> CSocketEVENT::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

	// bRet == -1 then the function fails if hWnd is an invalid window handle or lpMsg is an invalid pointer !!!
	// bRet == 0 then MSG == WM_QUIT !!!
	while( (bRet = ::GetMessage( &sMSG, m_hWnd, 0, 0 )) != 0 ) { 
		if ( bRet == -1 ) {
			// handle the error and possibly exit
			break;
		} else {
			::DispatchMessage(&sMSG); 
		}
	}

	g_LOG.CS_ODS( 0xffff, "<  < << CSocketEVENT::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}
#endif

//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK CSockWNDProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	static UINT uiSocketIDX;

	if ( CSocketWND::GetInstance() && uiMsg >= WM_SOCKETWND_MSG ) {
		uiSocketIDX = uiMsg - WM_SOCKETWND_MSG;
		if ( uiSocketIDX < CSocketWND::GetInstance()->GetMaxSocket() ) {
			if ( CSocketWND::GetInstance()->m_ppSocketBUFF[ uiSocketIDX ] ) {
				CSocketWND::GetInstance()->m_ppSocketBUFF[ uiSocketIDX ]->WndPROC( wParam, lParam );
				return 1;
			}
		}
	}

	return ::DefWindowProc(hWnd, uiMsg, wParam, lParam);
}


//-------------------------------------------------------------------------------------------------
CSocketWND::CSocketWND (HINSTANCE hInstance, UINT uiMaxSocket)
{
	const char *szWndClassName = "CSocketWND";
	m_hWND = NULL;

#ifdef	__USE_MESSAGE_PUMP
	m_pSocketEVENT = NULL;
#endif

	// if ( !hPrevInstance ) 
	{
		// 윈도우 클레스 생성
		WNDCLASS wc;

		wc.style			= 0;
		wc.lpfnWndProc		= CSockWNDProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= NULL;
		wc.hCursor			= NULL;
		wc.hbrBackground	= NULL;
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= szWndClassName;

		BOOL bResult = ::RegisterClass( &wc );
		if ( !bResult )
			return;
	}

	// 윈도우 생성, 윈도우는 디폴트에 의해 보이지 않고 ShowWindow로 절대 보여서는 안된다.
	m_hWND = ::CreateWindow(
				szWndClassName,						// 윈도우 클래스 이름
				"",									// 윈도우 타이틀 제목
				0,									// 윈도우 스타일
				CW_USEDEFAULT, CW_USEDEFAULT,		// 위치 = 디폴트
				CW_USEDEFAULT, CW_USEDEFAULT,		// 크기 = 디폴트
				NULL,								// 부모 윈도우 없음
				NULL,								// 메뉴 = 윈도우 클래스 디폴트
				hInstance,							// 윈도우 소유자
				NULL );								// 추가적인 생성 데이타 없음
	if ( m_hWND ) {
		m_uiMaxSocket = uiMaxSocket ? uiMaxSocket : 1;

		m_ppSocketBUFF = new CClientSOCKET*[ m_uiMaxSocket ];
		::ZeroMemory( m_ppSocketBUFF, sizeof(CClientSOCKET*) * m_uiMaxSocket );

#ifdef	__USE_MESSAGE_PUMP
		// message loop thread ...
		m_pSocketEVENT = new CSocketEVENT( m_hWND );
		if ( m_pSocketEVENT )
			m_pSocketEVENT->Resume ();
#endif
	}
}


//-------------------------------------------------------------------------------------------------
CSocketWND::~CSocketWND ()
{
	if ( m_hWND ) {
#ifdef	__USE_MESSAGE_PUMP
		// wait until thread terminate...
		if ( m_pSocketEVENT ) {
			while( !m_pSocketEVENT->IsFinished() ) {
				::SendMessage( m_hWND, WM_CLOSE, 0, 0 );
				::Sleep( 10 );
			}
			SAFE_DELETE( m_pSocketEVENT );
		}
#endif
		SAFE_DELETE_ARRAY( m_ppSocketBUFF );
		::DestroyWindow( m_hWND );
		m_hWND = NULL;
	}
	m_pInstance = NULL;
}


//-------------------------------------------------------------------------------------------------
void CSocketWND::DelSocket( UINT uiSocketWndMSG )
{
	UINT uiSocketIDX = uiSocketWndMSG - WM_SOCKETWND_MSG;
	if ( uiSocketIDX >= m_uiMaxSocket )
		return;

	m_ppSocketBUFF[ uiSocketIDX ] = NULL;
}

//-------------------------------------------------------------------------------------------------
bool CSocketWND::AddSocket( CClientSOCKET *pSocket, UINT uiSocketWndMSG )
{
	UINT uiSocketIDX = uiSocketWndMSG - WM_SOCKETWND_MSG;
	if ( uiSocketIDX >= m_uiMaxSocket ) {
		_ASSERT( 0 );
		return false;
	}

	m_ppSocketBUFF[ uiSocketIDX ] = pSocket;

	return true;
}
/*
#else
bool CSocketWND::AddSocketMsgProc(UINT uiSocketWndMSG, fpSockMsgPROC fpMsgProc)
{
	UINT uiSocketIDX = uiSocketWndMSG - WM_SOCKETWND_MSG;
	if ( uiSocketIDX >= m_uiMaxSocket )
		return false;

	m_fpMsgProc[ uiSocketIDX ] = fpMsgProc;

	return true;
}
bool CSocketWND::ProcSocketWndMSG( UINT uiSocketIDX, WPARAM wParam, LPARAM lParam )
{
	if ( uiSocketIDX < m_uiMaxSocket && m_fpMsgProc[ uiSocketIDX ] )
		return m_fpMsgProc[ uiSocketIDX ]( wParam, lParam );

	return false;
}
#endif
*/
//-------------------------------------------------------------------------------------------------