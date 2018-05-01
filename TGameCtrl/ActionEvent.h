#ifndef _CActionEvent_
#define _CActionEvent_

#include "TGameCtrl.h"

class CWinCtrl;

//---------------------------------------------------------------------------------------------------
/// CWinCtrl의 CActionListenerList에 추가되어 있는 IActionListener를 호출시 인자로 넘길 Class
///
///			* Client에서 CWinCtrl의 ActionListenerList에 IActionListener를 추가해서 
///				CWinCtrl::Process에서 직접 Client의 처리 루틴을 호출할수 있도록 하기 위한 System에 사용
///
/// 관련 Class : CActionListenerList, IActionListener, CActionEvent
///
/// @author	최종진
/// @date 2005/8/19
//---------------------------------------------------------------------------------------------------
class TGAMECTRL_API CActionEvent
{
public:
	CActionEvent();
	CActionEvent( CWinCtrl* pSource , unsigned uiID , unsigned wParam ,unsigned lParam );
	virtual ~CActionEvent(void);


	CWinCtrl*	GetSource();
	unsigned	GetID();
	unsigned	GetWParam();
	unsigned	GetLParam();

	void		SetSource( CWinCtrl* pSource );
	void		SetID( unsigned uiID );
	void		SetWParam( unsigned wParam );
	void		SetLParam( unsigned lParam );
protected:

	///Event를 처리한 CWinCtrl의 포인터
	CWinCtrl*		m_pSource;	
	///Event ID : Window Message일경우 Message ID
	unsigned		m_uiID;		
	///Window Message일경우WParam : 아닐경우 임의로 사용가능
	unsigned		m_uiWParam; 
	///Window Message일경우LParam : 아닐경우 임의로 사용가능
	unsigned		m_uiLParam; 
};
#endif