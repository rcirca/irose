#ifndef _DROPITEMMANAGER_
#define _DROPITEMMANAGER_

#include <list>


struct stDROPITEM
{
	int		m_iDamageOwner;
	int		m_iDropItemIndex;
};

///
/// 정확한 타이밍에 아이템을 드랍하기 위한 클래스..
///

class CDropItemManager
{
private:
	std::list< stDROPITEM >			m_DropItemList;


public:
	CDropItemManager(void);
	~CDropItemManager(void);

	void	PushDropItem( stDROPITEM& DropItem ){ m_DropItemList.push_back( DropItem ); }
	/// Drop item 정보가 수신되었다.
	int	UpdateDropItemList( int iDropItemIndex );
};

extern CDropItemManager	g_DropItemManager;

#endif // _DROPITEMMANAGER_	