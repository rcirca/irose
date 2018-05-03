#include "stdafx.h"
#include ".\cdragitem.h"
#include "Icon/CIcon.h"
#include "tgamectrl/tcommand.h"

CDragItem::CDragItem(void)
{
	m_pIcon  = NULL;
}

CDragItem::~CDragItem(void)
{
	std::map< int , CTCommand* >::iterator iter;
	CTCommand* pCmd = NULL;
	for( iter = m_Targets.begin(); iter != m_Targets.end();	)
	{
		pCmd = iter->second;
		iter = m_Targets.erase( iter );
		delete pCmd;
	}
}


void CDragItem::AddTarget( int iTargetType , CTCommand* pCmd )
{
	m_Targets.insert( std::map< int , CTCommand* >::value_type( iTargetType, pCmd ));
}

void CDragItem::SetIcon( CIcon* pIcon )
{
	if( m_pIcon )
	{
		delete m_pIcon;
		m_pIcon = NULL;
	}

	if( pIcon )
		m_pIcon = pIcon->Clone();
	else
		m_pIcon = NULL;
}

CIcon* CDragItem::GetIcon()
{
	return m_pIcon;
}

const std::map< int , CTCommand* >& CDragItem::GetTargets()
{
	return m_Targets;
}
