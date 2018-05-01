#ifndef _CTEventManufacture_
#define _CTEventManufacture_
#include "ctevent.h"

class CItem;
class CTEventManufacture : public CTEvent
{
public:
	CTEventManufacture(void);
	virtual ~CTEventManufacture(void);

	enum{
		EID_NONE,
		EID_CHANGE_TARGETITEM,
		EID_ADD_MATERIAL,
		EID_REMOVE_MATERIAL,
		EID_CHANGE_ITEMLIST,
	};

	void SetItem( CItem* pItem );
	void SetIndex( int iIndex );

	int				GetIndex();
	CItem*	GetItem();
protected:
	CItem*  m_pItem;
	int		m_iIndex;
};
#endif