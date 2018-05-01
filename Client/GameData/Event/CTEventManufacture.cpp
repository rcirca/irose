#include "stdafx.h"
#include ".\cteventmanufacture.h"
#include "../../gamecommon/Item.h"
CTEventManufacture::CTEventManufacture(void)
{
	m_strString = "CTEventMake";
}

CTEventManufacture::~CTEventManufacture(void)
{
	m_strString.clear();
}

void CTEventManufacture::SetItem( CItem* pItem )
{
	m_pItem = pItem;
}

CItem* CTEventManufacture::GetItem()
{
	return m_pItem;
}

void CTEventManufacture::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

int	CTEventManufacture::GetIndex()
{
	return m_iIndex;
}
