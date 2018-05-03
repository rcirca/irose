#ifndef _CTEventItem_
#define _CTEventItem_
#include "ctevent.h"

class CItem;
class CTEventItem :	public CTEvent
{
public:
	CTEventItem(void);
	virtual ~CTEventItem(void);
	enum{
		EID_NONE,
		EID_ADD_ITEM,
		EID_DEL_ITEM,
		EID_CHANGE_ITEM
	};
	
	void		SetItem( CItem* pItem );
	void		SetIndex( int iIndex );


	int			GetIndex();
	CItem*		GetItem();
protected:
	int			m_iIndex;
	CItem*		m_pItem;

};
#endif