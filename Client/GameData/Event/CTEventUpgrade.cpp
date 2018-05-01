#include "stdafx.h"
#include ".\cteventupgrade.h"
#include "../../gamecommon/Item.h"

CTEventUpgrade::CTEventUpgrade(void)
{
	m_strString = "Upgrade";
}

CTEventUpgrade::~CTEventUpgrade(void)
{
}

void CTEventUpgrade::SetItem( CItem* pItem )
{
	m_pItem = pItem;
}

CItem* CTEventUpgrade::GetItem()
{
	return m_pItem;
}

void CTEventUpgrade::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

int CTEventUpgrade::GetIndex()
{
	return m_iIndex;
}
