#include "StdAfx.h"
#include ".\imeview.h"

#pragma comment (lib, "IMM32.lib")
#include "TEditBox.h"
CImeView	g_CIME;

CImeView::CImeView()
{
	//m_hKeyWnd = NULL;
	m_TextCurCount = 0;
	m_hKeyLayout = NULL;
    m_nState = 0;   // Current Ime state.
    m_nCompLen = 0;   // To save previous composition string length.
    m_property = 0;
	m_charWidth = 0;
	m_charHeight = 0;
	m_hIMC = NULL;
    memset(m_hwndCand, 0, sizeof(HWND) * MAX_LISTCAND);
    memset(m_CandList, 0, sizeof(LPCANDIDATELIST) * MAX_LISTCAND);
    memset(m_Text, 0, sizeof(char) * MAX_CHAR_100);
    memset(m_TextComp, 0, sizeof(char) * 3);

}

CImeView::~CImeView()
{
}

void CImeView::InitIme(HWND hwnd)
{
	char		ga[] = "가";
	SIZE		size;
	HFONT		hFont;
	LOGFONT		lf;
	
	if(hwnd)	m_hWnd = hwnd;
	hFont = (HFONT)SendMessage(m_hWnd,WM_GETFONT, 0, 0);
	if (hFont) {
		if (GetObject(hFont, sizeof(LOGFONT), &lf)) {
			SetCompositionFont(&lf);
		}
	}

	//m_hKeyWnd = hwnd;
	m_hKeyLayout = GetKeyboardLayout(0);	// 0 means Current thread
	m_property = ImmGetProperty(m_hKeyLayout, IGP_PROPERTY);
	for (int i = 0; i < MAX_LISTCAND; i++) {
		m_hwndCand[i] = NULL;
		m_CandList[i] = NULL;
	}
	return;
	HDC hDC = ::GetDC(m_hWnd);
    HFONT hScrFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
    HFONT hOldFont = (HFONT)SelectObject(hDC, hScrFont);
    GetTextExtentPoint(hDC, (LPSTR)&ga, 2, &size);
	SelectObject(hDC, hOldFont);
    ::ReleaseDC(m_hWnd, hDC);

	m_charWidth  = size.cx / 2;
	m_charHeight = size.cy;
}

BOOL CImeView::IsProcessEvent()
{
	if (m_nState & IME_IN_CHOSECAND ||
			(m_nState & IME_IN_COMPOSITION && GetCompCursorPos()) ) {
		return FALSE;
	} else
		return TRUE;
}

// Set IME open state 
void CImeView::SetState(BOOL fOpen)
{
	if (Enter()) {
		ImmSetOpenStatus(m_hIMC, fOpen);
		Leave();
	}
}

CImeView::SetCompositionFont(LPLOGFONT lf)
{
	BOOL ret;
	if (Enter()) {
		ret = ImmSetCompositionFont(m_hIMC, lf);
		Leave();
	} else
		ret = FALSE;
	return ret;
}

void CImeView::SetCompWndPos(POINT& pt)
{
	if (m_property & IME_PROP_SPECIAL_UI || m_property & IME_PROP_AT_CARET)
		return;
	// at_near

	if (Enter()) {
		COMPOSITIONFORM cf;

		cf.dwStyle = CFS_POINT;
		cf.ptCurrentPos = pt;
		ImmSetCompositionWindow(m_hIMC, &cf);
		Leave();
	}
}

int CImeView::GetCompCursorPos()
{
	if (!Enter())
		return 0;
    int ret = LOWORD(ImmGetCompositionString(m_hIMC, GCS_CURSORPOS, NULL, 0));
	Leave();
	return ret;
}

void CImeView::ClearData()
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

void CImeView::CandPaint(HWND hwnd)
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

BOOL CImeView::GetResultString()
{
    DWORD       len;					// Storage for length of result str.
    LPSTR       str;					// Pointer to result string.

	if (Enter()) {
		if ((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0) {
			str = new char[len + 1];
			ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, str, len);
			str[len] = 0;
			ProcessResultString(str);
			delete str;
		}
		Leave();
	}
	return TRUE;
}

BOOL CImeView::GetCompString(LONG flag)
{
    DWORD      len;              // Stogare for len. of composition str
    LPSTR      str;              // Pointer to composition str.
    LPSTR      strAttr;          // Pointer to composition str array.
    DWORD      lenAttr;
   
	if (!Enter())
		return FALSE;
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
	return TRUE;
}

///////////////////////////////////////////////////////////////
// for IME Control Message

LONG CImeView::OnInputLangChange(WPARAM dwCommand, LPARAM dwData)
{
    if (ImmIsIME(m_hKeyLayout) && m_property & IME_PROP_AT_CARET)
		ClearData();

    // Set new keyboard layout.
	InitIme();

	if(Enter()) {
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
	return (LONG)DefWindowProc(m_hWnd,WM_INPUTLANGCHANGE, dwCommand, dwData);
}

LONG CImeView::OnIMESetContext(WPARAM dwCommand, LPARAM dwData)
{
	if (m_property & IME_PROP_AT_CARET) {
		// application wants to draw UI by itself.
		dwData &= ~(ISC_SHOWUICOMPOSITIONWINDOW | ISC_SHOWUIALLCANDIDATEWINDOW);
	}
	return (LONG)DefWindowProc(m_hWnd,WM_IME_SETCONTEXT, dwCommand, dwData);
}

LONG CImeView::OnIMEStartComposition(WPARAM dwCommand, LPARAM dwData)
{
	if (Check()) { // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		m_nCompLen = 0; // length of composition string.
		m_nState |= IME_IN_COMPOSITION;
		IMEStartComposition((LONG)dwData);
		GetCompString((LONG)dwData);
		return TRUE;
	} else
		return FALSE;
}

LONG CImeView::OnIMEComposition(WPARAM dwCommand, LPARAM dwData)
{
	if (Check()) { // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		if (dwData & GCS_RESULTSTR)
			GetResultString(); // Result String
		else if (dwData & GCS_COMPSTR)
			GetCompString((LONG)dwData); // Composition string
	}
	return 0;
}

LONG CImeView::OnIMEEndComposition(WPARAM dwCommand, LPARAM dwData)
{
	if (Check()) { // !IME_PROP_SPECIAL_UI && IME_PROP_AT_CARET
		m_nCompLen = 0;
		m_nState &= ~IME_IN_COMPOSITION;
		IMEEndComposition((LONG)dwData);
		return TRUE;
	} else
		return FALSE;
}

LONG CImeView::OnIMECompositionFull(WPARAM dwCommand, LPARAM dwData)
{
	// Make sure the size for drawing the composition string.
	// Application should draw the composition string correctly.
	return 0;
}

LONG CImeView::OnIMENotify(WPARAM dwCommand, LPARAM dwData)
{
	// This class does not handle all notification message.
	// So we pass those notification messages which are not hanlded
	// by this application to the DefWindowProc.

    switch (dwCommand) {
        case IMN_OPENCANDIDATE:
			return (LONG)OpenCandidate((LONG)dwData);

        case IMN_CLOSECANDIDATE:
			return (LONG)CloseCandidate((LONG)dwData);

        case IMN_CHANGECANDIDATE:
			return (LONG)ChangeCandidate((LONG)dwData);

        case IMN_SETOPENSTATUS:
			return SetOpenStatus();
    }
    return (LONG)DefWindowProc(m_hWnd,WM_IME_NOTIFY, dwCommand, dwData);
}

LONG CImeView::OnIMEControl(WPARAM dwCommand, LPARAM dwData)
{
	// This message is not received by the application window.
	// But don't pass it to DefWindowProc().
	return 0;
}

/////////////////////////////////////////////////////////////////
// for Candidate window

// IMN_OPENCANDIDATE:
BOOL CImeView::OpenCandidate(LONG lParam)
{
    LPCANDIDATELIST lpCandList = NULL;         // Storage for LP to candidate list.
    DWORD       dwBufLen;               // Storage for candidate strings.
    LPSTR       lpStr;                  // Storage for LP to a string.
    int         max_width = 0;          // Storage for width of listCand
    int         CurNumCandList = 0;     // Storage for number of cand. lists.
    DWORD       dwPreferNumPerPage;     // Storage for PreferNumPerPage
 //   POINT       point;                  // Storage for caret position.

    if (!Enter())
        return FALSE;

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
			SetWindowLong(m_hwndCand[index], 0, (LONG)this);

            ::ShowWindow(m_hwndCand[index], SW_SHOWNOACTIVATE);
            DisplayCandStrings(m_hwndCand[index], lpCandList);
            CurNumCandList++;
        }
    }
    m_nState |= IME_IN_CHOSECAND;

exit_opencand:
    Leave();
	return TRUE;
}

// IMN_CLOSECANDIDATE:
BOOL CImeView::CloseCandidate(LONG CandList)
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
	return TRUE;
}

// IMN_CHANGECANDIDATE:
BOOL CImeView::ChangeCandidate(LONG CandList)
{
    LPCANDIDATELIST lpCandList = NULL;         
    DWORD           dwIndex;
    DWORD 			dwBufLen;
    LPSTR			lpStr;
    DWORD			i = 1;
    RECT			rect;
    int				max_width = 0;
    DWORD			dwPreferNumPerPage;


	if (!Enter())
		return FALSE;

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
	return TRUE;
}

// IMN_SETOPENSTATUS:
BOOL CImeView::SetOpenStatus()
{
    if (!Enter())
        return FALSE;

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
	return TRUE;
}

BOOL CImeView::DisplayCandStrings(HWND hwnd, LPCANDIDATELIST lpCandList)
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
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CImeView overriding functions

void CImeView::ProcessResultString(LPSTR str)
{
//	PutString(str);
}

void CImeView::ProcessCompString(LPSTR str, LPSTR strAttr)
{

	int len = lstrlen(str);
	if(len >2)
		return;
	strcpy(m_TextComp,str);
	// draw composition string
}

void CImeView::IMEStartComposition(LONG dwData)
{
}

void CImeView::IMEEndComposition(LONG dwData)
{
}

/////////////////////////////////////////////////////////////////////////////
// CImeView message handlers

bool CImeView::IsYouMessage(UINT msg,WPARAM wparam, LPARAM lparam)
{
	if(!m_hWnd)
		return FALSE;
	switch (msg) { 
	  
	  case WM_IME_STARTCOMPOSITION:     // 글자 조합의 시작. WM_IME_COMPOSITION 메시지를           // 받을 준비를 한다. 
			OnIMEStartComposition(wparam,lparam);
         return TRUE;    
	  case WM_IME_ENDCOMPOSITION:          // 글자 조합의 끝. 조합된 문자열 처리를 끝낸다          break; 
			OnIMEEndComposition(wparam,lparam);
			///버튼 클릭시 현재 조합중이던 글자가 날라가 버리는 현상을 막기 위해
			///현재 Focus를 가지진 EditBox에 조합중이던 글자를 보내주고 처리하도록 한다.
			memset(m_TextComp,0,3);
         return TRUE;    
      case WM_IME_COMPOSITION:          // 현재 조합중인 문자열이나 완성된 문자열을 얻어서 화면 출력 
			OnIMEComposition(wparam,lparam);
			InvalidateRect(m_hWnd,NULL,TRUE);
         return TRUE;    
	  case WM_IME_SETCONTEXT:    // 글자 조합 윈도우와 한자 변환 윈도우를 표시하지 않게 바꿈 
			OnIMESetContext(wparam,lparam);
         return TRUE;    
      case WM_IME_NOTIFY:        
		  OnIMENotify(wparam,lparam);
         return TRUE;             
      case WM_INPUTLANGCHANGE:       // 키보드 레이아웃이 바뀌는 경우 IME를 초기화      
		  OnInputLangChange(wparam,lparam);
           return TRUE;    
	  case WM_IME_CONTROL:
		  OnIMEControl(wparam,lparam);
         return TRUE;    
	  case WM_IME_COMPOSITIONFULL:
		  OnIMECompositionFull(wparam,lparam);
         return TRUE;    
	  case WM_CHAR:
		  OnChar((UINT)wparam);
		  InvalidateRect(m_hWnd,NULL,TRUE);
		  break;
	  case WM_KEYDOWN:
		  
		  switch(wparam)
		  {
		  case VK_RETURN:
			  memset(m_TextComp, 0, sizeof(char) * 3);	
			  break;
		  case VK_SPACE:
			  memset(m_TextComp, 0, sizeof(char) * 3);	
			  break;
		  }
		  
		  break;

    }
	
	return FALSE;
}

void CImeView::OnChar(UINT nChar)
{
	if(((nChar & 0xff) == 8 ) || ((nChar & 0xff) == 27) || ((nChar & 0xff) == 9 ))
	{
		if(!m_TextCurCount) return;
	
		if(ChkEndChar())
		{
			m_Text[m_TextCurCount- 1] = NULL;
			m_Text[m_TextCurCount- 2] = NULL;
			m_TextCurCount-=2;
		//	memset(m_TextComp,0,3);
		//	Kor--;
		}
		else
		{
			m_Text[m_TextCurCount-1] = NULL;
			m_TextCurCount--;
		//	memset(m_TextComp,0,3);
		//	Eng--;
		}
	}
	else if( (nChar & 0xff) == 13)
	{
		m_Text[m_TextCurCount]= NULL;
		if(m_TextCurCount>0) 
			m_TextCurCount-=1;
	}

	else
	{
		char str[2];
		str[0] = nChar & 0xff;
		str[1] = 0;
		PutString(str);
//		memset(m_TextComp,0,3);
	}
}

void CImeView::PutString(LPSTR str)
{
	int len = (int)strlen(str); 
	if((len+m_TextCurCount) >= MAX_CHAR_100)
		return;
	strcpy(m_Text+m_TextCurCount,str);
	m_TextCurCount += len;
}

void CImeView::Darw(HDC dc,POINT * ppo)
{
	RECT rc= {ppo->x,ppo->y,ppo->x+500,ppo->y+20};
	DrawText(dc, m_Text, (int)strlen(m_Text), &rc, DT_LEFT | DT_VCENTER);
	rc.top = ppo->y+20;
	rc.bottom = ppo->y+40;
	DrawText(dc, m_TextComp, (int)strlen(m_TextComp), &rc, DT_LEFT | DT_VCENTER);
}

BOOL CImeView::ChkEndChar()
{
	BOOL IsKor=FALSE;

	for(int i=0;i<m_TextCurCount;i++)
	{
		if(ChkKorean2(m_Text,i))
		{
			i++;
			IsKor=TRUE;
		}
		else
			IsKor=FALSE;
	}
	return IsKor;
}

BOOL CImeView::ChkKorean(LPSTR str)
{
	if( *(str) & 0x80 )
		return TRUE;
	return FALSE;
}

BOOL CImeView::ChkKorean2(LPSTR str,int nCnt)
{
	if( str[nCnt] & 0x80 )
		return TRUE;
	return FALSE;
}

void CImeView::ClearString()
{
	m_TextCurCount = 0;
	memset(m_Text, 0, sizeof(char) * MAX_CHAR_100);
//	memset(m_TextComp, 0, sizeof(char) * 3);	
}

void CImeView::ConvInputMode()
{
	ImmSetConversionStatus(m_hIMC,IME_CMODE_CHARCODE,IME_CMODE_NATIVE);
}
