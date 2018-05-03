#ifndef _CUpgradeDlgStateWait_
#define _CUpgradeDlgStateWait_

#include "cupgradedlgstate.h"

/**
* 제련인터페이스 창의 State Class : 사용자가 서버에게 제련을 요청하고 응답을 대기하는 상태
*	
* @Warning		이상태의 경우 모든 유저의 입력은 무시한다.
* @Author		최종진
* @Date			2005/9/15
*/
class CUpgradeDlgStateWait : public CUpgradeDlgState
{
public:
	CUpgradeDlgStateWait(void);
	virtual ~CUpgradeDlgStateWait(void);

	virtual void Enter(){}
	virtual void Leave(){}
	virtual void Update( POINT ptMouse ){}
	virtual void Draw(){}
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam ){ return 1; }

};
#endif