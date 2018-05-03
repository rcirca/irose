#ifndef _IObserver
#define _IObserver

class CObservable;
class CTObject;
class IObserver
{
public:
	virtual void Update( CObservable* pObservable, CTObject* pObj ) = 0;
	
};
#endif