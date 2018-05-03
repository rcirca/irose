#ifndef _CITStateNpcDialog_
#define _CITStateNpcDialog_
#include "citstate.h"



/**
* NPC대화시 인터페이스(IT_MGR)상태
* - 2005/9/6일 현재 대만 버젼에서만 사용한다.
* - DLG_TYPE_DIALOG,  DLG_TYPE_SELECTEVENT , DLG_TYPE_EVENTDIALOG  셋중에 한개만 Show되어 진다
*
* @Author		최종진
*
* @Date			2005/9/6
*/
class CITStateNpcDialog : public CITState
{
public:
	CITStateNpcDialog(void);
	virtual ~CITStateNpcDialog(void);

	virtual void Enter();
	virtual void Leave();

	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void Update( POINT ptMouse );
	virtual bool Draw();
};
#endif