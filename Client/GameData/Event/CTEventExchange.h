#ifndef _CTEventExchange_
#define _CTEventExchange_
#include "ctevent.h"
class CItem;
class CTEventExchange :	public CTEvent
{
public:
	CTEventExchange(void);
	virtual ~CTEventExchange(void);

	enum{
		EID_NONE = 0,
		EID_ADD_MYITEM,				/// 내아이템 추가
		EID_REMOVE_MYITEM,			/// 내아이템 삭제
		EID_ADD_OTHERITEM,			/// 상대방 아이템 추가
		EID_REMOVE_OTHERITEM,		/// 상대방 아이템 삭제
		EID_CHANGE_READYOTHER		/// 상대방 레디 상태 변경
	};

	void    SetSlotIndex( int iIndex );
	void	SetItem( CItem* pItem );
	void	SetReadyOther( bool bReady );

	CItem*  GetItem();
	bool	GetReadyOther();
	int		GetSlotIndex();
protected:
	int			m_iSlotIndex;
	CItem*		m_pItem;
	bool		m_bReadyOther;
};


#endif