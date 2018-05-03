#include "stdafx.h"
#include ".\cteventchatroom.h"

CTEventChatRoom::CTEventChatRoom(void)
{
	m_strString = "ChatRoom";
}

CTEventChatRoom::~CTEventChatRoom(void)
{
}
void CTEventChatRoom::SetName( const char* pszName )
{
	assert( pszName );
	if( pszName )
		m_strName = pszName;
}
void CTEventChatRoom::SetServerIdx( WORD wServerIdx )
{
	m_wServerIdx = wServerIdx;
}

const char* CTEventChatRoom::GetName()
{
	return m_strName.c_str();
}
WORD  CTEventChatRoom::GetServerIdx()
{
	return m_wServerIdx;
}
