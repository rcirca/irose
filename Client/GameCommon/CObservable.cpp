//*-----------------------------------------------------------------------/
/// 2004/ 6/ 10 - AddObserver : 같은 Observer가 등록될시에는 무시한다.
//*-----------------------------------------------------------------------/



#include "stdafx.h"
#include ".\cobservable.h"
#include "IObserver.h"
#include <algorithm>
CObservable::CObservable(void)
{
	m_bChanged = false;
}

CObservable::~CObservable(void)
{
}
void	CObservable::AddObserver( IObserver* pObserver )
{
	std::list< IObserver* >::iterator iter;
	for( iter = m_Observers.begin(); iter != m_Observers.end(); ++iter )
	{
		if( *iter == pObserver )
		{
			LogString( LOG_NORMAL,"Duplicated Observer @CObservable::AddObserver\n" );		
			return;
		}
	}
	m_Observers.push_back( pObserver );
}

void	CObservable::DeleteObserver( IObserver* pObserver )
{
	std::list< IObserver* >::iterator iter;
	for( iter = m_Observers.begin(); iter != m_Observers.end(); ++iter )
	{
		if( *iter == pObserver )
		{
			m_Observers.erase( iter );
			break;
		}
	}
}

void	CObservable::DeleteObservers()
{
	m_Observers.clear();
}

int		CObservable::CountObservers()
{
	return m_Observers.size();
}

void	CObservable::NotifyObservers( CTObject* pObj )
{
	std::list< IObserver* >::iterator iter;
	for( iter = m_Observers.begin(); iter != m_Observers.end(); ++iter )
		(*iter)->Update( this, pObj );

	ClearChanged();

}

void	CObservable::SetChanged()
{
	m_bChanged = true;
}

void	CObservable::ClearChanged()
{
	m_bChanged = false;
}

bool	CObservable::HasChanged()
{
	return m_bChanged;
}