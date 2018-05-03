#ifndef _CNotifyButtonDlg_
#define _CNotifyButtonDlg_
#include "tgamectrl/tdialog.h"
#include <list>
class CTButton;

/**
* 알림버튼들을 보여주기 위한 인터페이스
*	- CTDialog를 상속받았지만 TDialog의 기능은 거의 안쓰고 있다
*
* @Author		최종진
* @Date			2005/9/15
*/
class CNotifyButtonDlg : public CTDialog
{
public:
	CNotifyButtonDlg(void);
	virtual ~CNotifyButtonDlg(void);

	virtual unsigned Process( unsigned uiMsg, WPARAM wParam ,LPARAM lParam );
	virtual void Draw();
	virtual void Update( POINT ptMouse );

	void    Clear();
	void	AddButton( CTButton* pBtn );
	void	UpdatePosition();

protected:


	std::list< CTButton* > m_Children;
};
#endif