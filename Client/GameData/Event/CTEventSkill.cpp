#include "stdafx.h"
#include ".\cteventskill.h"

CTEventSkill::CTEventSkill(void)
{
	m_strString = "CTEventSkill";
}

CTEventSkill::~CTEventSkill(void)
{

}

void	CTEventSkill::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

int		CTEventSkill::GetIndex()
{
	return m_iIndex;
}

