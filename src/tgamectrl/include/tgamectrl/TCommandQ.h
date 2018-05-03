#ifndef _TCOMMANDQ_
#define _TCOMMANDQ_
#include "TGameCtrl.h"
#include <list>
using namespace std;

class CTCommand;

/**
* CTCommand를 보관할 Container Class
*		- 현재(2005/8/30) TDialog가 가지고 있다
*
* @Author	최종진
*
* @Date		2005/8/30
*/
class TGAMECTRL_API CTCommandQ
{
public:
	CTCommandQ(void);
	virtual ~CTCommandQ(void);

	void		Push_Back( CTCommand* pCmd);
	void		Push_Front( CTCommand* pCmd);
	CTCommand*	GetCommand();
	void		Clear();

protected:
	list<CTCommand*>	m_listCommand;
};
#endif