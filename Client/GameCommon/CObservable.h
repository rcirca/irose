#ifndef _CObservable_
#define _CObservable_

#include <list>
class IObserver;
class CTObject;
class CObservable
{
public:
	CObservable(void);
	~CObservable(void);
	
	void	AddObserver( IObserver* pObserver );
	void	DeleteObserver( IObserver* pObserver );
	void	DeleteObservers();
	int		CountObservers();
	bool	HasChanged();

protected:
	void	NotifyObservers( CTObject* pObj );
	void	SetChanged();
	void	ClearChanged();

protected:
	std::list< IObserver* >		m_Observers;
	bool						m_bChanged;

};
#endif