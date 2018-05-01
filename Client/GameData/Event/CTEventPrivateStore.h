#ifndef _CTEventPrivateStore_
#define _CTEventPrivateStore_
#include "ctevent.h"

class CItem;
//*----------------------------------------------------------------------//
/// 개인상점관련 Data와 UI간의 Event Class
//*----------------------------------------------------------------------//
class CTEventPrivateStore :	public CTEvent
{
public:
	CTEventPrivateStore(void);
	virtual ~CTEventPrivateStore(void);

	enum{
		EID_NONE,
		EID_ADD_BUYLIST,
		EID_ADD_SELLLIST,
		EID_ADD_WISHLIST,
		EID_REMOVE_BUYLIST,
		EID_REMOVE_SELLLIST,
		EID_REMOVE_WISHLIST,
		EID_CLEAR_BUYLIST,
		EID_CLEAR_SELLIST,
		EID_SORT_SELLLIST,
	};
	void	SetIndex( int iIndex );
	int		GetIndex();

	void	SetItem( CItem* pItem );
	CItem*	GetItem();

protected:
	int		m_iIndex;
	CItem*	m_pItem;
};
#endif