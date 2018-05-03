#ifndef _LIVE_CHECK_
#define _LIVE_CHECK_

#include "../Util/JSingleton.h"


class CLiveCheck : public CJSingleton< CLiveCheck >
{
private:
	DWORD		m_dwElapsedTime;
	DWORD		m_dwLastRecoveryUpdateTime;

public:
	CLiveCheck(void);
	~CLiveCheck(void);

	void		Check();
	void		ResetTime();
};


#endif