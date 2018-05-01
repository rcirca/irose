#include "stdafx.h"
#include ".\serverlist.h"
#include <utility>

CServerList::CServerList(void)
{
	m_show_channel = true;
}

CServerList::~CServerList(void)
{

}

CServerList& CServerList::GetInstance()
{
	static CServerList s_Instance;
	return s_Instance;
}

const std::map< int, WORLDSERVER_INFO >& CServerList::GetWorldServerList()
{
	return m_worldserver_list;
}

const std::list< CHANNELSERVER_INFO >& CServerList::GetChannelServerList( int worldserver_id )
{
	std::map< int, WORLDSERVER_INFO >::iterator iter;
	for( iter = m_worldserver_list.begin(); iter != m_worldserver_list.end(); ++iter )
	{
		if( iter->second.m_world_id == worldserver_id )
			return iter->second.m_channel_list;
	}
	return m_channelserver_nulllist;
}

void CServerList::ClearChannelServerList( int worldserver_id )
{
	std::map< int, WORLDSERVER_INFO >::iterator iter;
	for( iter = m_worldserver_list.begin(); iter != m_worldserver_list.end(); ++iter )
	{
		if( iter->second.m_world_id == worldserver_id )
		{
			iter->second.m_channel_list.clear();

			m_Event.SetID( CTEventServerList::EID_CLEAR_CHANNELSERVER_LIST );
			SetChanged();
			NotifyObservers( &m_Event );

			return;
		}
	}
}

void CServerList::ClearWorldServerList()
{
	m_worldserver_list.clear();
	m_Event.SetID( CTEventServerList::EID_CLEAR_WORLDSERVER_LIST );
	SetChanged();
	NotifyObservers( &m_Event );
}

void CServerList::AddChannelServerList( int worldserver_id, BYTE channelserver_id, const char* channelserver_name, short user_perent )
{
	assert( channelserver_name );
	if( channelserver_name )
	{
		std::map< int, WORLDSERVER_INFO >::iterator iter;
		for( iter = m_worldserver_list.begin(); iter != m_worldserver_list.end(); ++iter )
		{
			if( iter->second.m_world_id == worldserver_id )
			{

				CHANNELSERVER_INFO channel;
				channel.m_worldserver_id = worldserver_id;
				channel.m_channel_id   = channelserver_id;
				channel.m_channel_name = channelserver_name;
				channel.m_user_percent = user_perent;
				iter->second.m_channel_list.push_back( channel );

				m_Event.SetID( CTEventServerList::EID_ADD_CHANNELSERVER );
				m_Event.SetChannelServerInfo( channel );
				SetChanged();
				NotifyObservers( &m_Event );

				return;
			}
		}
	}
}

void CServerList::AddWorldServerList( int index, DWORD worldserver_id, const char* worldserver_name )
{
	assert( worldserver_name );
	if( worldserver_name )
	{
		std::map< int, WORLDSERVER_INFO >::iterator iter;
		for( iter = m_worldserver_list.begin(); iter != m_worldserver_list.end(); ++iter )
		{
			if( iter->second.m_world_id == worldserver_id )
			{
				_RPT0( _CRT_ASSERT, "중복된 월드서버 ID를 받았습니다" );			
				return;
			}
		}

		WORLDSERVER_INFO world;
		world.m_world_id   = worldserver_id;
		world.m_world_name = worldserver_name;
		m_worldserver_list.insert( std::make_pair( index, world) );

		m_Event.SetID( CTEventServerList::EID_ADD_WORLDSERVER );
		m_Event.SetWorldServerInfo( world );
		SetChanged();
		NotifyObservers( &m_Event );
	}
}

void CServerList::ShowChannel()
{
	m_show_channel = true;
}

void CServerList::HideChannel()
{
	m_show_channel = false;
}

bool CServerList::IsShowChannel()
{
	return m_show_channel;
}
