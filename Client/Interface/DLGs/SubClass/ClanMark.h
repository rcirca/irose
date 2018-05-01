#ifndef _CClanMark_
#define _CClanMark_
#include "winctrl.h"

/**
* 클랜 창설시 유저가 선택할수 있는 클랜마크표시 class
*
* @Author		최종진
* @Date			2005/9/15
*/
class CClanMark : public CWinCtrl
{
public:
	CClanMark(void);
	virtual ~CClanMark(void);
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	void SetGraphicID( int iID );
	void SetModuleID( int iID );

protected:
	int	m_iGraphicID;
	int m_iModuleID;
};
#endif