#ifndef _CTCandidateWindowList_
#define _CTCandidateWindowList_

#include "tcandidatewindow.h"
#include "TListBox.h"

/**
* 한국어이외의 일어, 중국어일경우 사용되는 Candidate Window class 
*	* 리스트 형태
*	* 연관 Class	: CTIme
*
* @Author	최종진
* @Date		2005/8/30
*/
class CTCandidateWindowList : public CTCandidateWindow
{
public:
	CTCandidateWindowList(void);
	virtual ~CTCandidateWindowList(void);
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void AddCandidate( const char* pszCandidate );
	virtual void Clear();
	virtual void MoveWindow( POINT pt );
	virtual void Show();
	virtual void Hide();
	virtual void SetSelection( short n );
protected:

	CTListBox	m_Candidats;
	int			m_iMaxWidth;
};
#endif