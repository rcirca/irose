#ifndef _CActionListenerList_
#define _CActionListenerList_

#include "TGameCtrl.h"
#include <list>

class IActionListener;

//---------------------------------------------------------------------------------------------------
/// Client에서 CWinCtrl에 IActionListener를 추가시 보관하는 Class
///
///	* Client에서 CWinCtrl의 ActionListenerList에 IActionListener를 추가해서 
///		CWinCtrl::Process에서 직접 Client의 처리 루틴을 호출할수 있도록 하기 위한 System에 사용
///
/// * 관련 Class : CActionListenerList, IActionListener, CActionEvent
///
/// @Author 최종진
/// @Date 2005/8/19 
//---------------------------------------------------------------------------------------------------
class TGAMECTRL_API CActionListenerList
{
public:
	CActionListenerList(void);
	~CActionListenerList(void);
	
	void	Add( IActionListener* pListener );
	void	Remove( IActionListener* pListener );
	int		GetCount();
	bool	IsEmpty();
	std::list< IActionListener* >& GetListenerList( );
protected:
	std::list< IActionListener* >	m_list;
};
#endif