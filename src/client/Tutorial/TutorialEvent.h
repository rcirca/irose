#ifndef _TUTORIAL_EVENT_
#define _TUTORIAL_EVENT_

#include "../Common/IO_STB.h"


//-----------------------------------------------------------------------------------
///
/// CTutorialEvent
/// 튜토리얼 진행을 위한 이벤트 처리 클래스
///
//-----------------------------------------------------------------------------------

class CTutorialEvent
{
public:
	CTutorialEvent(void);
	~CTutorialEvent(void);

	bool			Init();
	void			Release();

	void			CheckEvent();
};


#endif _TUTORIAL_EVENT_