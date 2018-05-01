#ifndef _CTEventUpgrade_
#define _CTEventUpgrade_
#include "ctevent.h"
class CItem;
class CTEventUpgrade :	public CTEvent
{
public:
	CTEventUpgrade(void);
	virtual ~CTEventUpgrade(void);
	enum{
		EID_NONE,
		EID_SET_TARGETITEM,
		EID_REMOVE_TARGETITEM,
		EID_SET_MATERIALiTEM,
		EID_REMOVE_MATERIALITEM,
		EID_RECEIVE_RESULT,
	};

	void	SetItem( CItem* pItem );
	CItem*	GetItem();

	void	SetIndex( int iIndex );
	int		GetIndex();


protected:
	CItem*	m_pItem;
	int		m_iIndex;///재료 아이템일경우 Index( 0 ~ 2 )


};
#endif