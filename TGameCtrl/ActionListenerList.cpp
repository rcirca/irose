#include "StdAfx.h"
#include ".\actionlistenerlist.h"

CActionListenerList::CActionListenerList(void)
{

}

CActionListenerList::~CActionListenerList(void)
{

}

void	CActionListenerList::Add( IActionListener* pListener )
{

	if( pListener )
		m_list.push_back( pListener );
}

void	CActionListenerList::Remove( IActionListener* pListener )
{
	std::list< IActionListener* >::iterator iter;
	for( iter = m_list.begin(); iter != m_list.end(); ++iter )
	{
		if( *iter == pListener )
		{
			m_list.erase( iter );
			return;
		}
	}
}

std::list< IActionListener* >& CActionListenerList::GetListenerList( )
{
	return m_list;
}

int	CActionListenerList::GetCount()
{
	return m_list.size();
}
bool CActionListenerList::IsEmpty()
{
	return m_list.empty();
}
