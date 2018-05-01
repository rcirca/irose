#include "StdAfx.h"
#include ".\tcommand.h"
#include <crtdbg.h>
CTCommand::CTCommand(void)
{
	m_bWaitExec = false;
}

CTCommand::~CTCommand(void)
{
}

bool CTCommand::Exec( CTObject* pObj )
{
	_ASSERT( 0 && "CTCommand::Exec" );
	return true;
}

CTMacroCommand::~CTMacroCommand( void )
{
	CTCommand* pCmd;
	std::list<CTCommand*>::iterator iter;
	for( iter = m_listCommand.begin(); iter != m_listCommand.end(); )
	{
		pCmd = (*iter);
		iter = m_listCommand.erase( iter );
		delete pCmd;
	}
}

bool CTMacroCommand::Exec( CTObject* pObj )
{
	std::list<CTCommand*>::iterator iter;
	for( iter = m_listCommand.begin(); iter != m_listCommand.end(); ++iter )
		(*iter)->Exec( pObj );

	return true;
}

void CTMacroCommand::AddSubCommand( CTCommand* pCmd )
{
	m_listCommand.push_back( pCmd );
}
