/*
	$Header: /Client/Util/WndMsgQ.cpp 3     03-09-26 1:26p Icarus $
*/
#include "StdAfx.h"
#include "WndMsgQ.h"

//-------------------------------------------------------------------------------------------------
bool CWndMsgQ::_Init (short nMaxQSize)
{
    m_sCurMsg.uiMsg = 0;
    m_sCurMsg.wParam = 0;
    m_sCurMsg.lParam = 0;

	m_nMaxQSize = nMaxQSize;

	if ( m_pQueue )
		delete[] m_pQueue;
	m_pQueue = new struct tagWNDMSG[ m_nMaxQSize ];

	Del_AllMSG ();

    return true;
}


//-------------------------------------------------------------------------------------------------
void CWndMsgQ::_Free (void)
{
	if ( m_pQueue ) {
		delete[] m_pQueue;
		m_pQueue = NULL;
	}
}


//-------------------------------------------------------------------------------------------------
void CWndMsgQ::Del_AllMSG (void)
{
    m_nQCount = 0;
    m_nQHead  = m_nQTail = 0;

    m_sCurMsg.uiMsg  = 0;
    m_sCurMsg.wParam = 0;
    m_sCurMsg.lParam = 0;
}


//-------------------------------------------------------------------------------------------------
void CWndMsgQ::Add_WndMSG2Q (UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    if ( m_nQCount < m_nMaxQSize ) {
        m_nQTail %= m_nMaxQSize;
        m_pQueue[ m_nQTail ].uiMsg	= uiMsg;
        m_pQueue[ m_nQTail ].wParam	= wParam;
        m_pQueue[ m_nQTail ].lParam = lParam;
        m_nQTail  ++;
        m_nQCount ++;
    } else {
        m_sCurMsg.uiMsg	 = uiMsg;
        m_sCurMsg.wParam = wParam;
        m_sCurMsg.lParam = lParam;
    }
}


//-------------------------------------------------------------------------------------------------
bool CWndMsgQ::Get_WndMSG (void)
{
    if ( m_nQCount == 0 ) 
        return false;

    m_nQCount --;
    m_nQHead %= m_nMaxQSize;

    m_sCurMsg.uiMsg  = m_pQueue[ m_nQHead ].uiMsg;
    m_sCurMsg.wParam = m_pQueue[ m_nQHead ].wParam;
    m_sCurMsg.lParam = m_pQueue[ m_nQHead ].lParam;

    m_nQHead ++;

    return true;
}


//-------------------------------------------------------------------------------------------------
/*
bool CWndMsgQ::Add_Keybd_MSG (UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uiMsg ) {
		case WM_IME_STARTCOMPOSITION:	
		case WM_IME_ENDCOMPOSITION:     
		case WM_IME_COMPOSITION:        
		case WM_IME_SETCONTEXT:			
		case WM_IME_NOTIFY:        
		case WM_INPUTLANGCHANGE:		
		case WM_IME_CONTROL:
		case WM_IME_COMPOSITIONFULL:

			;
		case WM_KEYDOWN :
		case WM_KEYUP   :
		case WM_CHAR    :
			Add_WndMSG2Q (uiMsg, wParam, lParam);
			return true;
		case WM_SYSKEYDOWN :
			if ( wParam == VK_F10 ) {
				Add_WndMSG2Q (uiMsg, wParam, lParam);
				return true;
			}
			break;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool CWndMsgQ::Add_Mouse_MSG (UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uiMsg ) {
		case WM_MOUSEMOVE   :
			m_sCurMsg.sPos.x  = (short)LOWORD(lParam);
			m_sCurMsg.sPos.y  = (short)HIWORD(lParam);
			m_sCurMsg.nEvent |= EVN_MOVE;
			return true;

		case WM_LBUTTONDOWN :
		case WM_LBUTTONUP   :
		case WM_LBUTTONDBLCLK :
		case WM_RBUTTONDBLCLK :
		case WM_RBUTTONDOWN :
		case WM_RBUTTONUP   :
			Add_WndMSG2Q (uiMsg, wParam, lParam);
			return true;
	}

	return false;
}
*/
//-------------------------------------------------------------------------------------------------
