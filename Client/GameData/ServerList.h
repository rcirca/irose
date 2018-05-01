#ifndef _CServerList_
#define _CServerList_

#include <map>
#include <list>
#include "../GameCommon/CObservable.h"
#include "event/CTEventServerList.h"

/**
* 로그인서버에서 받은 월드 서버및 채널서버 Data Class
*
* @Author		최종진
* @Date			2005/9/15
*/
class CServerList : public CObservable
{
	CServerList(void);
	~CServerList(void);
public:

	static CServerList& GetInstance();
	const std::map< int, WORLDSERVER_INFO >& GetWorldServerList();
	const std::list< CHANNELSERVER_INFO >& GetChannelServerList( int worldserver_id );

	void ClearChannelServerList( int worldserver_id );
	void ClearWorldServerList();

	void AddChannelServerList( int worldserver_id, BYTE channelserver_id, const char* channelserver_name, short user_perent );
	void AddWorldServerList( int index, DWORD worldserver_id, const char* worldserver_name );

	void ShowChannel();
	void HideChannel();

	bool IsShowChannel();
private:
	///list  월드 서버 ID, 월드서버이름,채널서버 리스트 (월드 서버 ID를 가지고 있자)
	std::map< int, WORLDSERVER_INFO >		m_worldserver_list;
	std::list< CHANNELSERVER_INFO >			m_channelserver_nulllist;
	///type - 채널보이기/감추기
	bool	m_show_channel;
	CTEventServerList		m_Event;
};
#endif