#include "stdafx.h"
#include ".\cteventclan.h"

CTEventClan::CTEventClan(void)
{
	m_strString = "CTEventClan";
}

CTEventClan::~CTEventClan(void)
{
}

void CTEventClan::SetName( const char* pszName )
{
	assert( pszName );
	if( pszName )
		m_strName = pszName;
}

void CTEventClan::SetClass( int iClass )
{
	m_iClass = iClass;
}

int			CTEventClan::GetClass()
{
	return m_iClass;
}

const char* CTEventClan::GetName()
{
	return m_strName.c_str();
}


int CTEventClan::GetPoint()
{
	return m_iPoint;
}

int CTEventClan::GetChannelNo()
{
	return m_iChannelNo;
}

void CTEventClan::SetPoint( int iPoint )
{
	m_iPoint = iPoint;
}

void CTEventClan::SetChannelNo( int iChannelNo )
{
	m_iChannelNo = iChannelNo;
}
short CTEventClan::GetJob()
{
	return m_nJob;
}
short CTEventClan::GetLevel()
{
	return m_nLevel;
}
void CTEventClan::SetJob( short nJob )
{
	m_nJob = nJob;
}
void CTEventClan::SetLevel( short nLevel )
{
	m_nLevel = nLevel;
}
