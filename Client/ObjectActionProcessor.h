#ifndef _OBJECTACTIONPROCESS_
#define _OBJECTACTIONPROCESS_

#include "Util/JSingleton.h"


class CActionProcessChain;

//------------------------------------------------------------------------------------------------
/// Managing for CActionProcessChain
//------------------------------------------------------------------------------------------------
class CObjectActionProcessor : public CJSingleton< CObjectActionProcessor >
{
private:
	CActionProcessChain*			m_pRootActionChain;

public:
	CObjectActionProcessor(void);
	~CObjectActionProcessor(void);

	void		MakeChain();
	void		ClearChain();

	void		SetNextChain( CActionProcessChain* pChain );
	void		ProcessChain( HNODE hNode );
};




//------------------------------------------------------------------------------------------------
/// Interface for "getUserdata function"
//------------------------------------------------------------------------------------------------
class CActionProcessChain
{
protected:
	CActionProcessChain*			m_pNextActionChain;

public:
	CActionProcessChain(void);
	virtual ~CActionProcessChain(void);

	virtual void					DeleteNextChian();
	virtual void					AddNextChain( CActionProcessChain* pNext );
	virtual void					ProcessChain( HNODE hNode ) = 0{ *(int*)0 = 10; };
};


//------------------------------------------------------------------------------------------------
/// Warp object process
//------------------------------------------------------------------------------------------------
class CWarpObjectActionProcessor : public CActionProcessChain
{
public:
	CWarpObjectActionProcessor(void);
	~CWarpObjectActionProcessor(void);
	
	virtual void					ProcessChain( HNODE hNode );
};


//------------------------------------------------------------------------------------------------
/// Warp object process
//------------------------------------------------------------------------------------------------
class CEventObjectActionProcessor : public CActionProcessChain
{
public:
	CEventObjectActionProcessor(void);
	~CEventObjectActionProcessor(void);
	
	virtual void					ProcessChain( HNODE hNode );
};


#endif //_OBJECTACTIONPROCESS_
