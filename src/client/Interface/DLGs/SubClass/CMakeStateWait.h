#ifndef _CMAKESTATEWAIT_
#define _CMAKESTATEWAIT_
#include "cmakestate.h"

/**
* 제조인터페이스 창의 State Class : 제조시작을 서버에 요청하고 결과를 대기하는 상태
*
* @Warning		이 상태의 경우는 유저의 입력에 대한 처리를 하지 않도록 한다.
* @Author		최종진
* @Date			2005/9/15
*/
class CMakeStateWait :public CMakeState
{
public:

	CMakeStateWait( CMakeDLG* pParent);
	virtual ~CMakeStateWait();

	virtual void Draw();
	virtual void Update( POINT ptMouse);
	virtual void Show();
	virtual void Hide();
	virtual void Init();
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void MoveWindow(POINT ptPos ){}
	virtual bool IsModal();
};
#endif