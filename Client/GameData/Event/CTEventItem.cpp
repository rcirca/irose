#include "stdafx.h"
#include ".\cteventitem.h"
#include "../../GameCommon/Item.h"
CTEventItem::CTEventItem(void)
{
	m_strString = "CTEventItem";
}

CTEventItem::~CTEventItem(void)
{
	m_strString.clear();
}

void CTEventItem::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

int CTEventItem::GetIndex()
{
	return m_iIndex;
}

void CTEventItem::SetItem( CItem* pItem )
{
	assert( pItem );
	m_pItem = pItem;
}

CItem* CTEventItem::GetItem()
{
	return m_pItem;
}
