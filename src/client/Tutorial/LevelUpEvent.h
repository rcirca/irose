#ifndef _LEVELUP_EVENT_
#define _LEVELUP_EVENT_

#include "../Common/IO_STB.h"

#include <map>

//--------------------------------------------------------------------------------------
/// 
/// class CLevelUpEvent
/// @brief check the levelup event
/// 
//--------------------------------------------------------------------------------------

class CLevelUpEvent
{
private:
	STBDATA					m_TblLevelEvent;	/// 레벨업 알림버튼 이벤트

	std::map< int, int >	m_LevelUpEventTbl;	/// < 레벨, 알림버튼번호 >

public:
	CLevelUpEvent(void);
	~CLevelUpEvent(void);

	bool		Init();
	void		Release();

	void		CheckLevelEvent( int iLevel );

	void		Proc();
};


#endif //_LEVELUP_EVENT_