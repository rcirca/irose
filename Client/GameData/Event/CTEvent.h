#ifndef _CTEvent_
#define _CTEvent_

#include "tgamectrl/tobject.h"

/**
* Observable과 Observer간의 메세지 전달에 사용되는 CTEvent의 Base Class
*	
* @Warning	CTEvent의 구분을 스트링으로 하고 있다. 이때 일정 길이를 넘을경우 이상동작할때가 있다
* @Author	최종진
* @Date		2005/9/15
*/
class CTEvent :	public CTObject
{
public:
	CTEvent(void);
	virtual ~CTEvent(void);

	void	SetID( int iID );
	int		GetID();

protected:
	int		m_iID;
};
#endif