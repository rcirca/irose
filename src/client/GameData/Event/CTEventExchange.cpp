#include "stdafx.h"
#include ".\cteventexchange.h"

CTEventExchange::CTEventExchange(void)
{
	m_strString = "CTEventExchange";
}

CTEventExchange::~CTEventExchange(void)
{
	m_strString.clear();
}

void CTEventExchange::SetItem( CItem* pItem )
{
	m_pItem = pItem;
}

void CTEventExchange::SetReadyOther( bool bReady )
{
	m_bReadyOther = bReady;
}

CItem* CTEventExchange::GetItem()
{
	return m_pItem;
}

bool CTEventExchange::GetReadyOther()
{
	return m_bReadyOther;
}

int CTEventExchange::GetSlotIndex()
{
	return m_iSlotIndex;
}

void CTEventExchange::SetSlotIndex( int iIndex )
{
	m_iSlotIndex = iIndex;
}