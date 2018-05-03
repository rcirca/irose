#ifndef _TUTORIAL_EVENT_UTILITY_
#define _TUTORIAL_EVENT_UTILITY_

#include "../Common/IO_STB.h"
#include "../Util/JSingleton.h"


//---------------------------------------------------------------------------------------
///
/// class CTutorialEventUtility
///
///	이벤트 진행에 필요한 유틸리티 클래스입니다.
///
//---------------------------------------------------------------------------------------

class CTutorialEventUtility : public CJSingleton< CTutorialEventUtility >
{
private:
	STBDATA					m_TblNotifyButtonEvent;	/// 알림버튼 이벤트

public:
	CTutorialEventUtility(void);
	~CTutorialEventUtility(void);

	bool			Init();
	void			Release();

	/// 알림 버튼 이벤트를 생성한다.
	bool			CreateNotifyEventButton( int iEventIdx );
	/// EventButton.STB에 설정된 알림 버튼 이벤트를 실행한다.
	bool			ExecNotifyEvent( int iEventIdx );
};

#endif //_TUTORIAL_EVENT_UTILITY_