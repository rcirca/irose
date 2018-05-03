#include "stdafx.h"
#include ".\appmsgq.h"

CAppMsgQ::CAppMsgQ(void)
{
}

CAppMsgQ::~CAppMsgQ(void)
{
}

///
/// Add msg to suitable
///

bool CAppMsgQ::AddMsgToMouseQ( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{

	tagWNDMSG msg;
	msg.lParam = lParam;
	msg.uiMsg = uiMsg;
	msg.wParam = wParam;
	m_MouseMsgQ.push_back( msg );
	return true;
}

bool CAppMsgQ::AddMsgToKeyboardQ( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	tagWNDMSG msg;
	msg.lParam = lParam;
	msg.uiMsg = uiMsg;
	msg.wParam = wParam;

	m_KeyboardMsgQ.push_back( msg );

	return true;
}

bool CAppMsgQ::AddMsgToQ( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uiMsg ) 
	{
		/// 
		/// Mouse msg
		///

		case WM_MOUSEMOVE   :
		case WM_MBUTTONDOWN :
		case WM_MBUTTONUP   :
		case WM_LBUTTONDOWN :
		case WM_LBUTTONUP   :
		case WM_RBUTTONDOWN :			
		case WM_RBUTTONUP   :
		case WM_LBUTTONDBLCLK :
		case WM_MBUTTONDBLCLK :
		case WM_RBUTTONDBLCLK :				
		case WM_MOUSEWHEEL	: 
			AddMsgToMouseQ (uiMsg, wParam, lParam);
			return true;

		/// 
		/// Keyboard msg
		///

		case WM_KEYDOWN :
		case WM_KEYUP   :
		case WM_CHAR    :
		case WM_SYSKEYDOWN :
			AddMsgToKeyboardQ (uiMsg, wParam, lParam);
			return true;		
	}

	return false;
}


///
/// get specific msg from queue
///
bool CAppMsgQ::GetMouseMsgFromQ( tagWNDMSG& WndMsg )
{
	if( m_MouseMsgQ.empty() )
	{
		return false;
	}

	WndMsg = m_MouseMsgQ.front();
	m_MouseMsgQ.pop_front();

	return true;
}

bool CAppMsgQ::GetKeyboardMsgFromQ( tagWNDMSG& WndMsg )
{
	if( m_KeyboardMsgQ.empty() )
	{
		return false;
	}

	WndMsg = m_KeyboardMsgQ.front();
	m_KeyboardMsgQ.pop_front();

	return true;
}
void CAppMsgQ::Clear()
{
	m_KeyboardMsgQ.clear();
	m_MouseMsgQ.clear();
}