#ifndef _CTEventServerList_
#define _CTEventServerList_
#include "ctevent.h"

struct CHANNELSERVER_INFO{
	DWORD			m_worldserver_id;
	BYTE			m_channel_id;
	std::string		m_channel_name;
	short			m_user_percent;
};

struct WORLDSERVER_INFO{
	DWORD							m_world_id;
	std::string						m_world_name;
	std::list< CHANNELSERVER_INFO >	m_channel_list;		
};

class CTEventServerList : public CTEvent
{
public:
	CTEventServerList(void);
	virtual ~CTEventServerList(void);

	enum{
		EID_NONE,
		EID_CLEAR_WORLDSERVER_LIST,
		EID_CLEAR_CHANNELSERVER_LIST,
		EID_ADD_WORLDSERVER,
		EID_ADD_CHANNELSERVER,
	};
	void SetWorldServerInfo( WORLDSERVER_INFO& info );
	void SetChannelServerInfo( CHANNELSERVER_INFO& info );
	WORLDSERVER_INFO& GetWorldServerInfo();
	CHANNELSERVER_INFO& GetChannelServerInfo();
private:
	WORLDSERVER_INFO	m_worldserver;
	CHANNELSERVER_INFO	m_channelserver;
};
#endif