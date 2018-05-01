#ifndef _CITStateRepair_
#define _CITStateRepair_
#include "citstate.h"

/**
* 수리중(item사용 or npc이용) Interface(IT_MGR) State
*
* @Author		최종진
* @Date			2005/9/6
*
*/
class CITStateRepair : public CITState
{
public:
	CITStateRepair(void);
	virtual ~CITStateRepair(void);

	virtual void Enter();
	virtual void Leave();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void Update( POINT ptMouse ){}
};
#endif