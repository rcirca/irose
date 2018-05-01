#include "stdafx.h"
#include ".\cteventavatarstore.h"

CTEventAvatarStore::CTEventAvatarStore(void)
{
	m_strString = "AvatarStore";
}

CTEventAvatarStore::~CTEventAvatarStore(void)
{

}

void CTEventAvatarStore::SetIndex( int iIndex )
{
	m_iIndex = iIndex;
}

int CTEventAvatarStore::GetIndex()
{
	return m_iIndex;
}
