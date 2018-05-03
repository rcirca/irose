#ifndef _CUpgradeDlgStateNormal_
#define _CUpgradeDlgStateNormal_

#include "cupgradedlgstate.h"

class CUpgradeDlg;
/**
* 제련인터페이스 창의 State Class : 사용자가 아이템및 재료를 준비하는 상태
*
* @Author		최종진
* @Date			2005/9/15
*/
class CUpgradeDlgStateNormal : public CUpgradeDlgState
{
public:
	CUpgradeDlgStateNormal( CUpgradeDlg* pParent );
	virtual ~CUpgradeDlgStateNormal(void);

	virtual void Enter();
	virtual void Leave();
	virtual void Update( POINT ptMouse );
	virtual void Draw();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

private:
	enum{
		IID_BTN_START	= 10,
		IID_BTN_CLOSE	= 11
	};

	CUpgradeDlg*	m_pParent;
};

#endif