#include "stdafx.h"
#include ".\cteventserverlist.h"

CTEventServerList::CTEventServerList(void)
{
	m_strString = "SVR_LIST";
}

CTEventServerList::~CTEventServerList(void)
{
}
void CTEventServerList::SetWorldServerInfo( WORLDSERVER_INFO& info )
{
	m_worldserver = info;
}
void CTEventServerList::SetChannelServerInfo( CHANNELSERVER_INFO& info )
{
	m_channelserver = info;
}
WORLDSERVER_INFO& CTEventServerList::GetWorldServerInfo()
{
	return m_worldserver;
}
CHANNELSERVER_INFO& CTEventServerList::GetChannelServerInfo()
{
	return m_channelserver;
}
