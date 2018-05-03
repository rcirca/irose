#include "stdafx.h"
#include ".\levelupevent.h"
#include "TutorialEventUtility.h"




CLevelUpEvent::CLevelUpEvent(void)
{
}

CLevelUpEvent::~CLevelUpEvent(void)
{
}

bool CLevelUpEvent::Init()
{
	if( m_TblLevelEvent.Load ( "3DDATA\\STB\\LevelUpEvent.STB",		false, false ) == false )
		return false;

	/// Make event table from stb that loaded above.
	for( int i = 1; i < m_TblLevelEvent.m_nDataCnt; i++ )
	{
		m_LevelUpEventTbl.insert( std::make_pair< int, int >( m_TblLevelEvent.m_ppDATA[ i ][ 0 ], m_TblLevelEvent.m_ppDATA[ i ][ 1 ] ) );
	}

	return true;
}

void CLevelUpEvent::Release()
{
	m_TblLevelEvent.Free();
}

void CLevelUpEvent::CheckLevelEvent( int iLevel )
{
	std::map< int, int >::iterator itor = m_LevelUpEventTbl.find( iLevel );

	/// if find level up event
	if( itor != m_LevelUpEventTbl.end() )
	{
		CTutorialEventUtility::GetSingleton().CreateNotifyEventButton( itor->second );
	}
}

void CLevelUpEvent::Proc()
{
	
}
