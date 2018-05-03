#ifndef _TARGET_MANAGER_
#define _TARGET_MANAGER_

#include "../Util/JSingleton.h"


class CTargetManager  : public CJSingleton< CTargetManager >
{
private:
	int					m_iCurrentMouseTargetObject;			/// 마우스 이동중 현재 마우스가 가르키는 오브젝트	

public:
	CTargetManager(void);
	~CTargetManager(void);


	void		SetMouseTargetObject( int iObjectIDX );


	void		Proc();
	void		Draw();
};


#endif //_TARGET_MANAGER_