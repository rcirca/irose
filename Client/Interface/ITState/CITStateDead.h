#ifndef _CITStateDead_
#define _CITStateDead_
#include "citstate.h"

/**
* 죽었을때의 인터페이스(IT_MGR) 상태
*
* @Author		최종진
*
* @Date			2005/9/6
*/
class CITStateDead : public CITState
{
public:
	CITStateDead(void);
	virtual ~CITStateDead(void);

	virtual void Enter();
	virtual void Leave();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void Update( POINT ptMouse ){}

};
#endif