#ifndef _DELAYED_EXP_
#define _DELAYED_EXP_

#include <list>
#include "../Util/JSingleton.h"


struct EXP_DATA
{
	int iOwnerIndex;
	int iAddExp;
	int iExp;
	int iGetTime;			/// 서버로 부터 받은시간. 

	int iStamina;			/// 스태미너도 현재 스태미너가 서버에서 날라오기때문에..
};


//------------------------------------------------------------------------------------------
///
/// class CDelayedExp
/// Synchronizing display time of getting exp with real mob dead time
///
//------------------------------------------------------------------------------------------

class CDelayedExp : public CJSingleton< CDelayedExp >
{
private:
	std::list< EXP_DATA >		m_ExpData;


public:
	CDelayedExp(void);
	~CDelayedExp(void);

	void						PushEXPData( int iOwnerIndex, int iAddExp, int iEXP, int iStamina );
	void						GetExp( int iOwnerIndex );

	void						Proc();
};

#endif //_DELAYED_EXP_
