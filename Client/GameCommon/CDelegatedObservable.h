#ifndef _CDelegatedObservable_
#define _CDelegatedObservable_
#include "cobservable.h"

class CDelegatedObservable : public CObservable
{
public:
	CDelegatedObservable(void);
	virtual ~CDelegatedObservable(void);

	void	NotifyObservers( CTObject* pObj );
	void	SetChanged();
	void	ClearChanged();
};
#endif