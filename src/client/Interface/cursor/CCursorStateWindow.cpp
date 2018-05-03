#include "stdafx.h"
#include ".\ccursorstatewindow.h"
#include "CCursor.h"
CCursorStateWindow::CCursorStateWindow(void)
{
	m_iState = CCursor::STATE_WINDOW;
}

CCursorStateWindow::~CCursorStateWindow(void)
{

}

int CCursorStateWindow::SetCursorType( int iType )
{ 
	//ShowCursor( TRUE );
	return 0;
}