#include "stdafx.h"
#include ".\cdelegatedobservable.h"

CDelegatedObservable::CDelegatedObservable(void)
{
}

CDelegatedObservable::~CDelegatedObservable(void)
{
}
void	CDelegatedObservable::NotifyObservers( CTObject* pObj )
{
	CObservable::NotifyObservers( pObj );
}
void	CDelegatedObservable::SetChanged()
{
	CObservable::SetChanged();
}

void	CDelegatedObservable::ClearChanged()
{
	CObservable::ClearChanged();
}
