#include "stdafx.h"
#include ".\tcmdsystem.h"
#include "CGame.h"

CTCmdChangeGameState::CTCmdChangeGameState( int iState )
{
	m_iState = iState;
}
bool CTCmdChangeGameState::Exec( CTObject* )
{
	CGame::GetInstance().ChangeState( m_iState );
	return true;
}
