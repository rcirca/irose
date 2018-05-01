#include "StdAfx.h"
#include ".\tcandidatewindow.h"

CTCandidateWindow::CTCandidateWindow(void)
{
}

CTCandidateWindow::~CTCandidateWindow(void)
{
}

unsigned CTCandidateWindow::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

void CTCandidateWindow::AddCandidate( const char* pszCandidate )
{
	_ASSERT( 0 && "Pure virtual Function Call");
}

void CTCandidateWindow::Clear()
{
	_ASSERT( 0 && "Pure virtual Function Call");
}
