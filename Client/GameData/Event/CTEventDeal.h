#ifndef _CTEventDeal_
#define _CTEventDeal_

#include "CTEvent.h"
class CItemFragment;
class CTEventDeal : public CTEvent
{
public :
	CTEventDeal(void);
	virtual ~CTEventDeal(void);
	enum{
		EID_NONE,
		EID_ADD_BUYITEM,
		EID_REMOVE_BUYITEM,
		EID_ADD_SELLITEM,
		EID_REMOVE_SELLITEM,
		EID_CLEAR
	};

	void		SetItem( CItemFragment* pItem );
	
	CItemFragment*		GetItem();
protected:
	CItemFragment*		m_pItem;

};

#endif