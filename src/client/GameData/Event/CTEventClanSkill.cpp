#include "stdafx.h"
#include ".\cteventclanskill.h"

CTEventClanSkill::CTEventClanSkill(void)
{
	m_strString = std::string("ETClanSkill");
}

CTEventClanSkill::~CTEventClanSkill(void)
{

}

void	CTEventClanSkill::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

int		CTEventClanSkill::GetIndex()
{
	return m_iIndex;
}

