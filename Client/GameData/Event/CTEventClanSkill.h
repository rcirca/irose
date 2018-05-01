#ifndef _CTEventClanSkill_
#define _CTEventClanSkill_
#include "ctevent.h"

class CTEventClanSkill : public CTEvent
{
public:
	CTEventClanSkill(void);
	virtual ~CTEventClanSkill(void);
	enum{
		EID_NONE,
		EID_ADD_SKILL,
		EID_DEL_SKILL,
		EID_LEVELUP,
	};
	void	SetIndex( int iIndex );
	int		GetIndex();
protected:

	int			m_iIndex;

};
#endif