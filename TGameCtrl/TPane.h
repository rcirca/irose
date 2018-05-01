#ifndef _CTPane_
#define _CTPane_
#include "winctrl.h"
#include "JContainer.h"

/**
* Dialog내에서의 영역별로 child control들을 구분하기 위한 Container class
*   - 클라이언트의 CChatRoomDlg에서 위/아래 영역이 이 Container class로 구분되어 있다.
*
* @Author		최종진
* @Data			2005/8/30
*/
class TGAMECTRL_API CTPane : public CWinCtrl
{
public:
	CTPane(void);
	virtual ~CTPane(void);

	virtual void Update( POINT ptMouse );
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void MoveWindow( POINT pt );
	virtual void Show();
	virtual void Hide();

	void		AddChild( CWinCtrl* pCtrl );
	CWinCtrl*	FindChild( unsigned iID );
	CWinCtrl*	Find( const char * szName );

protected:
	CJContainer		m_Children;
};
#endif