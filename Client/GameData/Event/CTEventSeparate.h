#ifndef _CTEventSeparate_
#define _CTEventSeparate_
#include "ctevent.h"

class CItem;
class CTEventSeparate :	public CTEvent
{
public:
	CTEventSeparate(void);
	virtual ~CTEventSeparate(void);

	enum{
		EID_NONE,
		EID_SET_MATERIAL_ITEM,
		EID_REMOVE_MATERIAL_ITEM,
		EID_SET_OUTPUT_ITEM,
		EID_REMOVE_OUTPUT_ITEM,

	};
	void SetItem( CItem* pItem );
	CItem* GetItem();

	void SetIndex( int iIndex );
	int  GetIndex();
protected:
	CItem*	m_pItem;
	int		m_iIndex;
};
#endif