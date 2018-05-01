#ifndef _ITEM_INFO_VIEW_
#define _ITEM_INFO_VIEW_

#include <list>
#include "../Interface/CInfo.h"



class CObjITEM;


//----------------------------------------------------------------------------------------------------
/// class CDropItemInfo
/// 드랍아이템의 정보를 출력할 심플 컨터롤
//----------------------------------------------------------------------------------------------------

class CDropItemInfo
{
private:
	CObjITEM*							m_pDropItem;

	CInfo								m_Info;
	

public:
	CDropItemInfo();
	~CDropItemInfo();

	void			Reset();

	void			SetDropItemInfo( CObjITEM* pItem );
	void			Draw();
};



//----------------------------------------------------------------------------------------------------
/// class CItemInfoView
/// 특정 상황에서( alt 키를 눌렀을경우등 ) 아이템의 정보를 다 보여줌
//----------------------------------------------------------------------------------------------------

class CItemInfoView
{
private:
	std::list< CDropItemInfo* >			m_ViewedItemList;
	std::list< CDropItemInfo* >			m_DropItemInfoPool;

public:
	CItemInfoView(void);
	~CItemInfoView(void);

	void			ReserverDropItemPool( int iPoolSize = 20 );
	void			Clear();

	void			Reset();
    bool			PushDropItemInfo( CObjITEM* pItem );
	void			Draw();
};





#endif //_ITEM_INFO_VIEW_