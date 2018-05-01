#include "StdAfx.h"
#include "tcommandq.h"
#include "tcommand.h"

CTCommandQ::CTCommandQ(void)
{
}

CTCommandQ::~CTCommandQ(void)
{
	Clear();
}

void CTCommandQ::Push_Back( CTCommand* pCmd )
{
	m_listCommand.push_back( pCmd );
}

void CTCommandQ::Push_Front( CTCommand* pCmd )
{
	m_listCommand.push_front( pCmd );
}

CTCommand* CTCommandQ::GetCommand()
{
	list<CTCommand*>::iterator Iter;
	if( m_listCommand.empty() )
		return NULL;


	Iter = m_listCommand.begin();
	CTCommand* Ret = *Iter;
	m_listCommand.erase( Iter );
	return Ret;
}

void CTCommandQ::Clear()
{
	list<CTCommand*>::iterator Iter;
	for( Iter = m_listCommand.begin(); Iter != m_listCommand.end(); )
	{
		delete *Iter;
		Iter = m_listCommand.erase( Iter );
	}
}