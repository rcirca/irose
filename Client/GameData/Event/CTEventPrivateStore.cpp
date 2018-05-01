#include "stdafx.h"
#include ".\cteventprivatestore.h"

CTEventPrivateStore::CTEventPrivateStore(void)
{
	m_strString = "PrivateStore";
}


CTEventPrivateStore::~CTEventPrivateStore(void)
{
}

void CTEventPrivateStore::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

int CTEventPrivateStore::GetIndex()
{
	return m_iIndex;
}

void CTEventPrivateStore::SetItem( CItem* pItem )
{
	m_pItem = pItem;
}

CItem* CTEventPrivateStore::GetItem()
{
	return m_pItem;
}
