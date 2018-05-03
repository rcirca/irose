#ifndef _IActionListener_
#define _IActionListener_
//---------------------------------------------------------------------------------------------------
/// Client에서 CWinCtrl의 CActionListenerList에 처리루틴 추가시 상속받아야할 인터페이스
///
///	* Client에서 CWinCtrl의 ActionListenerList에 IActionListener를 추가해서 
///		CWinCtrl::Process에서 직접 Client의 처리 루틴을 호출할수 있도록 하기 위한 System에 사용
///
/// * 관련 Class : CActionListenerList, IActionListener, CActionEvent
///
/// @Author 최종진
/// @Date	2005/8/19 
//---------------------------------------------------------------------------------------------------


#include "TGameCtrl.h"
class CActionEvent;
class TGAMECTRL_API IActionListener
{
public:
	virtual unsigned ActionPerformed (CActionEvent*  e) = 0;
};
#endif