
#include <windows.h>
#include "classIME.h"

#pragma comment (lib, "IMM32.lib")				// For IME

//-------------------------------------------------------------------------------------------------
classIME::classIME()
{	
	m_bActive		= false;

	//m_hKeyWnd = NULL;
	m_TextCurCount	= 0;
	m_hKeyLayout	= NULL;
    m_nState		= 0;   // Current Ime state.
    m_nCompLen		= 0;   // To save previous composition string length.
    m_property		= 0;
	m_charWidth		= 0;
	m_charHeight	= 0;
	m_hIMC			= NULL;

    ::ZeroMemory (m_hwndCand,	sizeof(HWND) * MAX_LISTCAND);
    ::ZeroMemory (m_CandList,	sizeof(LPCANDIDATELIST) * MAX_LISTCAND);
    ::ZeroMemory (m_TextComp,	sizeof(char) * 3);

	m_nMaxTextLength = 0;
	m_pText			 = NULL;
}

classIME::~classIME()
{
	if ( m_pText )
		delete[] m_pText;
}


//-------------------------------------------------------------------------------------------------
bool classIME::Enter() 
{
	if ( m_hWnd != NULL ) {
		return ( (bool)(m_hIMC = ::ImmGetContext(m_hWnd)) );
	}

	return false;
}


//-------------------------------------------------------------------------------------------------
void classIME::Leave()
{	
	::ImmReleaseContext(m_hWnd, m_hIMC);	
}


//-------------------------------------------------------------------------------------------------
bool classIME::Check()
{
	return (!(m_property & IME_PROP_SPECIAL_UI) && (m_property & IME_PROP_AT_CARET));
}


//-------------------------------------------------------------------------------------------------
void classIME::Clear ()
{
	m_bActive		= false;

	//m_hKeyWnd = NULL;
	m_TextCurCount	= 0;
	m_hKeyLayout	= NULL;
    m_nState		= 0;   // Current Ime state.
    m_nCompLen		= 0;   // To save previous composition string length.
    m_property		= 0;
	m_charWidth		= 0;
	m_charHeight	= 0;
	m_hIMC			= NULL;

    ::ZeroMemory (m_hwndCand,	sizeof(HWND) * MAX_LISTCAND);
    ::ZeroMemory (m_CandList,	sizeof(LPCANDIDATELIST) * MAX_LISTCAND);
    ::ZeroMemory (m_TextComp,	sizeof(char) * 3);

	if ( m_pText )
		::ZeroMemory (m_pText, sizeof(char) * m_nMaxTextLength);
}


//-------------------------------------------------------------------------------------------------
bool classIME::Create (HWND hWnd, short nMaxTextLen)
{
	char		ga[] = "가";
	SIZE		size;
	HFONT		hFont;
	LOGFONT		lf;

	if ( !m_pText ) {
		m_nMaxTextLength = nMaxTextLen;
		m_pText = new char [ nMaxTextLen ];
		::ZeroMemory (m_pText, sizeof(char) * nMaxTextLen);
	}


	if ( hWnd )	m_hWnd = hWnd;

	hFont = (HFONT)::SendMessage(m_hWnd,WM_GETFONT, 0, 0);
	if ( hFont ) {
		if ( GetObject(hFont, sizeof(LOGFONT), &lf) ) {
			SetCompositionFont(&lf);
		}
	}

	//m_hKeyWnd = hwnd;
	m_hKeyLayout = ::GetKeyboardLayout(0);	// 0 means Current thread
	m_property   = ::ImmGetProperty(m_hKeyLayout, IGP_PROPERTY);
	for (int i = 0; i < MAX_LISTCAND; i++) {
		m_hwndCand[i] = NULL;
		m_CandList[i] = NULL;
	}

	HDC hDC = ::GetDC(m_hWnd);
    HFONT hScrFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
    HFONT hOldFont = (HFONT)SelectObject(hDC, hScrFont);
    GetTextExtentPoint(hDC, (LPSTR)&ga, 2, &size);
	SelectObject(hDC, hOldFont);
    ::ReleaseDC(m_hWnd, hDC);

	m_charWidth  = size.cx / 2;
	m_charHeight = size.cy;

	return true;
}

//-------------------------------------------------------------------------------------------------
bool classIME::IsProcessEvent()
{
	if ( m_nState & IME_IN_CHOSECAND ||
		(m_nState & IME_IN_COMPOSITION && GetCompCursorPos()) ) {
		return false;
	} else
		return true;
}


//-------------------------------------------------------------------------------------------------
// Set IME open state 
void classIME::SetState(bool fOpen)
{
	if ( Enter() ) {
		::ImmSetOpenStatus(m_hIMC, fOpen);
		Leave();
	}
}

//-------------------------------------------------------------------------------------------------
bool classIME::SetCompositionFont(LPLOGFONT lf)
{
	bool bRetValue;

	if ( Enter() ) {
		bRetValue = ::ImmSetCompositionFont(m_hIMC, lf) ? true : false;
		Leave();
	} else
		bRetValue = false;

	return bRetValue;
}

//-------------------------------------------------------------------------------------------------
void classIME::SetCompWndPos(POINT& pt)
{
	if (m_property & IME_PROP_SPECIAL_UI || m_property & IME_PROP_AT_CARET)
		return;
	// at_near

	if ( Enter() ) {
		COMPOSITIONFORM cf;

		cf.dwStyle = CFS_POINT;
		cf.ptCurrentPos = pt;
		ImmSetCompositionWindow(m_hIMC, &cf);
		Leave();
	}
}

//-------------------------------------------------------------------------------------------------
int classIME::GetCompCursorPos()
{
	if ( !Enter() )
		return 0;

    int ret = LOWORD(ImmGetCompositionString(m_hIMC, GCS_CURSORPOS, NULL, 0));

	Leave();
	return ret;
}

//-------------------------------------------------------------------------------------------------
void classIME::ClearData()
{
    for(int i = 0; i < MAX_LISTCAND; i++) {
        if (m_hwndCand[i]) {
            ::DestroyWindow(m_hwndCand[i]);
			m_hwndCand[i] = NULL;
            delete m_CandList[i];
			m_CandList[i] = NULL;
        }
    }
    m_nCompLen = m_nState = 0;
}

//-------------------------------------------------------------------------------------------------
void classIME::CandPaint(HWND hwnd)
{
    int         index;
    HDC         hdc;
    PAINTSTRUCT ps;

    hdc = ::BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
    
    for (index = 0; index < MAX_LISTCAND; index ++ )
        if (m_hwndCand[index] == hwnd)
            break;

	if ( index != MAX_LISTCAND && m_CandList[index]) {
	    DisplayCandStrings(hwnd, m_CandList[index]);
	}
    ::EndPaint( hwnd, (LPPAINTSTRUCT)&ps );
}

//-------------------------------------------------------------------------------------------------
bool classIME::GetResultString()
{
    DWORD       len;					// Storage for length of result str.
    LPSTR       str;					// Pointer to result string.

	if ( Enter() ) {
		if ((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0) {
			str = new char[len + 1];
			ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, str, len);
			str[len] = 0;
			ProcessResultString(str);
			delete str;
		}
		Leave();
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
bool classIME::GetCompString(long flag)
{
    DWORD      len;              // Stogare for len. of composition str
    LPSTR      str;              // Pointer to composition str.
    LPSTR      strAttr;          // Pointer to composition str array.
    DWORD      lenAttr;
   
	if ( !Enter() ) 
		return false;

    if ((len = ImmGetCompositionString(m_hIMC, GCS_COMPSTR, NULL, 0)) > 0) {
		str = new char[len+1];
		ImmGetCompositionString(m_hIMC, GCS_COMPSTR, str, len);
		str[len] = 0;
		strAttr = NULL;
		if (flag & GCS_COMPATTR) {
			if ((lenAttr = ImmGetCompositionString(m_hIMC, GCS_COMPATTR, NULL, 0)) > 0) {
				strAttr = new char[lenAttr+1];
				ImmGetCompositionString(m_hIMC, GCS_COMPATTR, strAttr, lenAttr);
				strAttr[lenAttr] = 0;
			}
		} 
		// Override function
		ProcessCompString(str, strAttr);
		m_nCompLen = (UINT)len;
		if (strAttr)
			delete strAttr;
		if (str)
			delete str;
	}

	Leave();
	return true;
}

//-------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////
// for IME Control Message
//
long classIME::OnInputLangChange(WPARAM dwCommand, LPARAM dwData)
{
    if (ImmIsIME(m_hKeyLayout) && m_property & IME_PROP_AT_CARET)
		ClearData();

    // Set new keyboard layout.
	InitIme();

	if ( Enter() ) {
		for (int i = 0; i < MAX_LISTCAND; i++) {
			CANDIDATEFORM cf;

			if (m_property & IME_PROP_AT_CARET) {
	/*
				// This application do not want to set candidate window to
				// any position. Anyway, if an application need to set the
				// candiadet position, it should remove the if 0 code
				// the position you want to set
				cf.dwIndex = i;
				cf.dwStyle = CFS_CANDIDATEPOS;
				cf.ptCurrentPos.x = ptAppWantPosition[i].x;
				cf.ptCurrentPos.y = ptAppWantPosition[i].y;
				ImmSetCandidateWindow(m_hIMC, &CandForm );
	*/
			} else {
				if (!ImmGetCandidateWindow(m_hIMC, i, &cf))
					continue;
				if (cf.dwStyle == CFS_DEFAULT)
					continue;
				cf.dwStyle = CFS_DEFAULT;
				ImmSetCandidateWindow(m_hIMC, &cf);
			}
		}
		Leave();
	}

	return DefWindowProc(m_hWnd, WM_INPUTLANGCHANGE, dwCommand, dwData);
}

//-------------------------------------------------------------------------------------------------
long classIME::OnIMESetContext(WPARAM dwCommand, LPARAM dwData)
{
	if (m_property & IME_PROP_AT_CARET) {
		// application wants to draw UI by itself.
		dwData &= ~(ISC_SHOWUICOMPOSITIONWINDOW | ISC_SHOWUIALLCANDIDATEWINDOW);
	}

	return DefWindowProc(m_hWnd, WM_IME_SETCONTEXT, dwCommand, dwData);
}

//-------------------------------------------------------------------------------------------------
long classIME::OnIMEStartComposition(WPARAM dwCommand, LPARAM dwData)
{
	if (Check()) { // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		m_nCompLen = 0; // length of composition string.
		m_nState |= IME_IN_COMPOSITION;
		IMEStartComposition(dwData);
		return true;
	} else
		return false;
}

//-------------------------------------------------------------------------------------------------
long classIME::OnIMEComposition(WPARAM dwCommand, LPARAM dwData)
{
	if ( Check() )	{ // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		if ( dwData & GCS_RESULTSTR )
			GetResultString();		// Result String
		else 
		if (dwData & GCS_COMPSTR)
			GetCompString(dwData);	// Composition string
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
long classIME::OnIMEEndComposition(WPARAM dwCommand, LPARAM dwData)
{
	if (Check()) { // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		m_nCompLen = 0;
		m_nState &= ~IME_IN_COMPOSITION;
		IMEEndComposition(dwData);
		return true;
	} else
		return false;
}

//-------------------------------------------------------------------------------------------------
long classIME::OnIMECompositionFull(WPARAM dwCommand, LPARAM dwData)
{
	// Make sure the size for drawing the composition string.
	// Application should draw the composition string correctly.
	return 0;
}

//-------------------------------------------------------------------------------------------------
long classIME::OnIMENotify(WPARAM dwCommand, LPARAM dwData)
{
	// This class does not handle all notification message.
	// So we pass those notification messages which are not hanlded
	// by this application to the DefWindowProc.

    switch (dwCommand) {
        case IMN_OPENCANDIDATE:
			return OpenCandidate(dwData);

        case IMN_CLOSECANDIDATE:
			return CloseCandidate(dwData);

        case IMN_CHANGECANDIDATE:
			return ChangeCandidate(dwData);

        case IMN_SETOPENSTATUS:
			return SetOpenStatus();
    }

    return DefWindowProc(m_hWnd, WM_IME_NOTIFY, dwCommand, dwData);
}

//-------------------------------------------------------------------------------------------------
long classIME::OnIMEControl(WPARAM dwCommand, LPARAM dwData)
{
	// This message is not received by the application window.
	// But don't pass it to DefWindowProc().
	return 0;
}

//-------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////
// for Candidate window
// IMN_OPENCANDIDATE:
//
bool classIME::OpenCandidate(long lParam)
{
    LPCANDIDATELIST lpCandList;         // Storage for LP to candidate list.
    DWORD       dwBufLen;               // Storage for candidate strings.
    LPSTR       lpStr;                  // Storage for LP to a string.
    int         max_width = 0;          // Storage for width of listCand
    int         CurNumCandList = 0;     // Storage for number of cand. lists.
    DWORD       dwPreferNumPerPage;     // Storage for PreferNumPerPage
 //   POINT       point;                  // Storage for caret position.

    if ( !Enter() )
        return false;

    // SetWindowText(hwnd, (LPSTR)TITLE_CAND);
    for (int i = 0; i < MAX_LISTCAND; i++ ) {
        if (m_hwndCand[i])
            CurNumCandList++;
    }
    for (int index = 0; index < MAX_LISTCAND; index++ ) {
        if (lParam & (1 << index)) {   
            if (!(dwBufLen = ImmGetCandidateList(m_hIMC, index, lpCandList, 0)))                      
                goto exit_opencand;
            if( !(m_CandList[index] = (LPCANDIDATELIST)new char[dwBufLen]))
                goto exit_opencand;
            lpCandList = m_CandList[index];
            ImmGetCandidateList(m_hIMC, index, lpCandList, dwBufLen);

            POINT pt ;
			GetCaretPos(&pt);
            ClientToScreen(m_hWnd,&pt);

            dwPreferNumPerPage = ( !lpCandList->dwPageSize ) ?
                                 DEFAULT_CAND_NUM_PER_PAGE :
                                 lpCandList->dwPageSize;

			// get the longest string length
            for (int i = 0; i < (int)lpCandList->dwCount; i++ ) {
                lpStr = (LPSTR)lpCandList + lpCandList->dwOffset[i];
                max_width = (max_width < lstrlen(lpStr)) ? lstrlen(lpStr) : max_width;
            }

            m_hwndCand[index] = CreateWindow(          
                                 "HanjaCandidate", "CandWindow",
                                 WS_BORDER | WS_POPUP | WS_DISABLED,
                                 CurNumCandList * m_charWidth + pt.x, pt.y + m_charHeight + 2,
                                 (max_width + 3) * m_charWidth + 4,
                                 (int)dwPreferNumPerPage * m_charHeight + 5,
                                 m_hWnd,
                                 (HMENU)NULL,
                                 (HINSTANCE)GetWindowLong(m_hWnd, GWL_HINSTANCE ),
                                 (LPVOID)NULL
                                 );

            if (m_hwndCand[index] == NULL) {
				delete m_CandList[index];
				m_CandList[index] = NULL;
                goto exit_opencand;
            }
			SetWindowLong(m_hwndCand[index], 0, (long)this);

            ::ShowWindow(m_hwndCand[index], SW_SHOWNOACTIVATE);
            DisplayCandStrings(m_hwndCand[index], lpCandList);
            CurNumCandList++;
        }
    }
    m_nState |= IME_IN_CHOSECAND;

exit_opencand:
    Leave();
	return true;
}

//-------------------------------------------------------------------------------------------------
// IMN_CLOSECANDIDATE:
bool classIME::CloseCandidate(long CandList)
{
    for (int i = 0; i < MAX_LISTCAND; i++) {
        if ((CandList & ( 1 << i ) ) && m_CandList[i]) {
            ::DestroyWindow(m_hwndCand[i]);
            m_hwndCand[i] = NULL;
			delete m_CandList[i];
			m_CandList[i] = NULL;
        }
    }
    m_nState &= ~IME_IN_CHOSECAND;
	return true;
}

//-------------------------------------------------------------------------------------------------
// IMN_CHANGECANDIDATE:
bool classIME::ChangeCandidate(long CandList)
{
    LPCANDIDATELIST lpCandList = NULL;         
    DWORD           dwIndex;
    DWORD 			dwBufLen;
    LPSTR			lpStr;
    DWORD			i = 1;
    RECT			rect;
    int				max_width = 0;
    DWORD			dwPreferNumPerPage;


	if ( !Enter() )
		return false;

    for (dwIndex = 0; dwIndex < MAX_LISTCAND; dwIndex++) {
        if (CandList & i)
            break;
		else
			i <<= 1;
	}

    if (dwIndex == MAX_LISTCAND)
        goto exit_changecand;

    if (!(dwBufLen = ImmGetCandidateList(m_hIMC, dwIndex, lpCandList, 0))) {
		goto exit_changecand;
	}
	delete m_CandList[dwIndex];
	m_CandList[dwIndex] = lpCandList = (LPCANDIDATELIST)new char[dwBufLen];

    ImmGetCandidateList(m_hIMC, dwIndex, lpCandList, dwBufLen);
    dwPreferNumPerPage = (!lpCandList->dwPageSize ) ?
                         DEFAULT_CAND_NUM_PER_PAGE : lpCandList->dwPageSize;

    for( i = 0; i < lpCandList->dwCount; i++ ) {
        lpStr = (LPSTR)lpCandList + lpCandList->dwOffset[i];
        max_width = (max_width < lstrlen(lpStr)) ? lstrlen(lpStr) : max_width;
    }

    ::GetWindowRect(m_hwndCand[dwIndex], (LPRECT) &rect);
    ::SetWindowPos(m_hwndCand[dwIndex], m_hWnd, rect.left, rect.top,
		  (max_width + 3) * m_charWidth + 4,
		  (int)(dwPreferNumPerPage) * m_charHeight + 5,
		  SWP_NOZORDER | SWP_NOACTIVATE );
		  
	DisplayCandStrings(m_hwndCand[dwIndex], lpCandList);

exit_changecand:
	Leave();
	return true;
}

//-------------------------------------------------------------------------------------------------
// IMN_SETOPENSTATUS:
bool classIME::SetOpenStatus()
{
    if ( !Enter() )
        return false;

    if (ImmGetOpenStatus(m_hIMC)) {
        // the IME conversion engine is open
    } else {
        // Here we close and destroy all of candidate windows
        // if IME conversion engine is closed.

        for (int i = 0; i < MAX_LISTCAND; i++ ) {
            if (m_CandList[i]) {
                ::DestroyWindow(m_hwndCand[i]);
                m_hwndCand[i] = NULL;
				delete m_CandList[i];
				m_CandList[i] = NULL;
            }
        }
		m_nCompLen = 0;
		m_nState = 0;
    }

	Leave();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool classIME::DisplayCandStrings(HWND hwnd, LPCANDIDATELIST lpCandList)
{
    HDC         hdc;            // Storage for device context handle.
    LPSTR       lpStr;          // Storage for LP to a string.
    DWORD       dwNumPerPage;   // Storage for num per page
    DWORD       dwStartIndex;   // Storage for candidate index
    DWORD       dwEndIndex;     // Storage for candidate index
    RECT        rect;           // Storage for client rect.
    int         y;				// 
    DWORD       dwBackColor;    // Storage for background color value
    DWORD       dwTextColor;
	char		buf[255];

    dwNumPerPage = (!lpCandList->dwPageSize ) ? 
						DEFAULT_CAND_NUM_PER_PAGE : 
						lpCandList->dwPageSize;
    dwStartIndex = lpCandList->dwPageStart;

    dwEndIndex = dwStartIndex + dwNumPerPage;
    dwEndIndex = ( dwEndIndex >= lpCandList->dwCount ) ?
                 lpCandList->dwCount : dwEndIndex;
                 
    hdc = ::GetDC(hwnd);
	// Draw Background
    dwBackColor = SetBkColor(hdc, RGB(0xc0 ,0xc0, 0xc0));
    ::GetClientRect(hwnd, &rect);
    ExtTextOut( hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, 0);

	// Draw Selection
    rect.top = rect.top + ( lpCandList->dwSelection - dwStartIndex ) *
               m_charHeight;
    rect.bottom = rect.top + m_charHeight;
    SetBkColor(hdc, RGB(0x00, 0x00, 0x80));
    ExtTextOut( hdc, 0, rect.top, ETO_OPAQUE, &rect, NULL, 0, 0);

    SetBkMode( hdc, TRANSPARENT );
    dwTextColor = GetTextColor(hdc); 

    for (y = 0 ; dwStartIndex < dwEndIndex; dwStartIndex++, y++ ) {
        lpStr = (LPSTR) lpCandList + lpCandList->dwOffset[dwStartIndex];
        if ( dwStartIndex == lpCandList->dwSelection )
            SetTextColor(hdc, RGB(255, 255, 255));
        else
            SetTextColor(hdc, dwTextColor);
		wsprintf(buf, "%d :%s", y + 1, lpStr);
        TextOut(hdc, 2, y * m_charHeight, buf, lstrlen(buf));
    }

    SetTextColor( hdc, dwTextColor );
    SetBkColor( hdc, dwBackColor );
    ::ReleaseDC( hwnd, hdc );
	return true;
}

//-------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// classIME overriding functions
//
void classIME::ProcessResultString(LPSTR str)
{
	PutString(str);
}

//-------------------------------------------------------------------------------------------------
void classIME::ProcessCompString(LPSTR str, LPSTR strAttr)
{
	int len = lstrlen(str);
	if ( len > 2 )
		return;

	strcpy(m_TextComp, str);
	// draw composition string
}

//-------------------------------------------------------------------------------------------------
void classIME::IMEStartComposition(long dwData)
{
}

//-------------------------------------------------------------------------------------------------
void classIME::IMEEndComposition(long dwData)
{
	ZeroMemory (m_TextComp, sizeof(char) * 3);
}

//-------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// classIME message handlers
//
int classIME::ProcIMEMessage (UINT msg, WPARAM wparam, LPARAM lparam)
{
	if ( !m_hWnd )
		return CIME_SKIP;

	switch ( msg ) { 
		case WM_IME_STARTCOMPOSITION:	// 글자 조합의 시작. WM_IME_COMPOSITION 메시지를 받을 준비를 한다. 
			OnIMEStartComposition(wparam,lparam);
			return CIME_HANDLED;

		case WM_IME_ENDCOMPOSITION:     // 글자 조합의 끝. 조합된 문자열 처리를 끝낸다          break; 
			OnIMEEndComposition(wparam,lparam);
			return CIME_HANDLED;

		case WM_IME_COMPOSITION:        // 현재 조합중인 문자열이나 완성된 문자열을 얻어서 화면 출력 
			OnIMEComposition(wparam,lparam);
			return CIME_HANDLED;

		case WM_IME_SETCONTEXT:			// 글자 조합 윈도우와 한자 변환 윈도우를 표시하지 않게 바꿈 
			OnIMESetContext(wparam,lparam);
			return CIME_HANDLED;

		case WM_IME_NOTIFY:        
			OnIMENotify(wparam,lparam);
			return CIME_HANDLED;

		case WM_INPUTLANGCHANGE:		// 키보드 레이아웃이 바뀌는 경우 IME를 초기화      
			OnInputLangChange(wparam,lparam);
			return CIME_HANDLED;

		case WM_IME_CONTROL:
			OnIMEControl(wparam,lparam);
			return CIME_HANDLED;

		case WM_IME_COMPOSITIONFULL:
			OnIMECompositionFull(wparam,lparam);
			return CIME_HANDLED;

		case WM_CHAR:
			return OnChar(wparam);
    }

	return CIME_SKIP;
}

//-------------------------------------------------------------------------------------------------
int classIME::OnChar(UINT nChar)
{
	switch ( nChar & 0x0ff ) {
		case VK_ESCAPE :
			return CIME_CANCEL;

		case VK_BACK   :
			if ( !m_TextCurCount ) break;
		
			if( ChkEndChar() )
			{
				m_pText[ m_TextCurCount-1 ] = NULL;
				m_pText[ m_TextCurCount-2 ] = NULL;
				m_TextCurCount-=2;
				::ZeroMemory (m_TextComp, sizeof(char) * 3);
			} else {
				m_pText[ m_TextCurCount-1 ] = NULL;
				m_TextCurCount--;
				::ZeroMemory (m_TextComp, sizeof(char) * 3);
			}
			break;

		case VK_RETURN :
			m_pText[ m_TextCurCount ] = NULL;
			// m_TextCurCount -= 1;
			return CIME_RETURN;

		default :
			char str[ 2 ];
			str[ 0 ] = nChar & 0xff;
			str[ 1 ] = 0;
			PutString(str);
			::ZeroMemory (m_TextComp, sizeof(char) * 3);
	}

	return CIME_WMCHAR;
}


//-------------------------------------------------------------------------------------------------
void classIME::PutString(LPSTR str)
{
	int len = strlen(str); 
	if ( (len+m_TextCurCount) >= m_nMaxTextLength )
		return;

	strcpy(m_pText+m_TextCurCount,str);
	m_TextCurCount += len;
}


//-------------------------------------------------------------------------------------------------
bool classIME::ChkEndChar()
{
	bool IsKor=false;

	for (int i=0;i<m_TextCurCount;i++) {
		if ( ChkKorean2(m_pText,i) ) {
			i++;
			IsKor=true;
		} else
			IsKor=false;
	}

	return IsKor;
}

//-------------------------------------------------------------------------------------------------
bool classIME::ChkKorean(LPSTR str)
{
	if ( *(str) & 0x80 )
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------
bool classIME::ChkKorean2(LPSTR str,int nCnt)
{
	if ( str[ nCnt ] & 0x80 )
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------
void classIME::ClearString()
{
	m_TextCurCount = 0;

	if ( m_pText )	
		ZeroMemory (m_pText, sizeof(char) * m_nMaxTextLength);
}

//-------------------------------------------------------------------------------------------------
void classIME::ToggleInputMode(bool bHangul)
{
    HIMC        hIMC;
    DWORD       dwConversion;
    DWORD       dwSentence;

    if ( (hIMC = ::ImmGetContext( m_hWnd )) ) {
        ::ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence);

        if ( dwConversion & IME_CMODE_HANGEUL ) {	//한글 모드면
			if ( !bHangul ) {
				// dwConversion -= IME_CMODE_HANGEUL;
				dwConversion &= ~IME_CMODE_HANGEUL;
			}
			// iCARUS_LogString (LOG_DEBUG, "Hangul Input Mode !!!!!!\n");
		} else {
			if ( bHangul ) {
				dwConversion = dwConversion | IME_CMODE_HANGEUL;
			}
			// iCARUS_LogString (LOG_DEBUG, "English Input Mode !!!!!!\n");
		}

        ::ImmSetConversionStatus( hIMC, dwConversion, dwSentence);
        ::ImmReleaseContext( m_hWnd, hIMC );
    }
}

/*
//-------------------------------------------------------------------------------------------------
void classIME::Draw (short nX, short nY, HDC hDC)
{
	char szStr[ 128 ];

    SetBkMode (hDC, TRANSPARENT);
    SetTextColor (hDC, RGB( 255, 255, 255 ) );

	strcpy (szStr, m_pText);
	strcat (szStr, m_TextComp);

    TextOut (hDC, nX, nY, szStr, lstrlen(szStr) );
}


//-------------------------------------------------------------------------------------------------
void classIME::Draw (short nX, short nY, LPDIRECTDRAWSURFACE7 pSurface)
{
    HDC         hDC;

    if ( pSurface->GetDC(&hDC) != DD_OK ) return;

	Draw (nX, nY, hDC);

    pSurface->ReleaseDC(hDC);
}
*/

//-------------------------------------------------------------------------------------------------
