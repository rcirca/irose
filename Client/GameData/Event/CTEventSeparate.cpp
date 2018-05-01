#include "stdafx.h"
#include ".\cteventseparate.h"

CTEventSeparate::CTEventSeparate(void)
{
	m_strString = "Separate";
}

CTEventSeparate::~CTEventSeparate(void)
{
}

void CTEventSeparate::SetItem( CItem* pItem )
{
	assert( pItem );
	m_pItem = pItem;
}
CItem* CTEventSeparate::GetItem()
{
	return m_pItem;
}

void CTEventSeparate::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

int  CTEventSeparate::GetIndex()
{
	return m_iIndex;
}
