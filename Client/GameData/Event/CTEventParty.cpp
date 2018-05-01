#include "stdafx.h"
#include ".\cteventparty.h"

CTEventParty::CTEventParty(void)
{
	m_strString = "Party";
}

CTEventParty::~CTEventParty(void)
{

}

void CTEventParty::SetObjSvrIdx( WORD iObjSvrIdx )
{
	m_wObjSvrIdx = iObjSvrIdx;
}

WORD CTEventParty::GetObjSvrIdx()
{
	return m_wObjSvrIdx;
}

void CTEventParty::SetName( const char* pszName )
{
	assert( pszName );
	if( pszName )
		m_strName = pszName;
}


const char* CTEventParty::GetName()
{
	return m_strName.c_str();
}

void	CTEventParty::SetObjectTag( DWORD dwObjectTag )
{
	m_dwObjectTag = dwObjectTag;
}

DWORD   CTEventParty::GetObjectTag()
{
	return m_dwObjectTag;
}
void	CTEventParty::SetRule( BYTE btRule )
{
	m_btRule = btRule;
}
BYTE	CTEventParty::GetRule()
{
	return m_btRule;
}
