#ifndef _CTEventChatRoom_
#define _CTEventChatRoom_
#include "ctevent.h"

class CTEventChatRoom :	public CTEvent
{
public:
	CTEventChatRoom(void);
	virtual ~CTEventChatRoom(void);
	enum{
		EID_STATE_ACTIVATED,
		EID_STATE_DEACTIVATED,///채팅룸에서 나간다 - UI를 빼자
		EID_JOIN_MEMBER,
		EID_LEAVE_MEMBER,
		EID_SET_MASTER,
	};

	void SetName( const char* pszName );
	void SetServerIdx( WORD wServerIdx );

	const char* GetName();
	WORD  GetServerIdx();
protected:
	WORD		m_wServerIdx;
	std::string	m_strName;
};
#endif