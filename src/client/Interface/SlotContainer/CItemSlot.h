#ifndef _CItemSlot_
#define _CItemSlot_
#include <vector>
#include "../../common/citem.h"
#include "../../gamecommon/cobservable.h"
class CItem;
class CTCommand;
class CTEventItem;


/**
* CItem 컨테이너 클래스
*	- 기존에 서버와 소스를 공유해서 쓰던( 현재:2005/9/15는 분리됨) tagITEM을 가지는 CInventory는 게임인터페이스 구현에 많은 부분에서 제약이 있어
*		tagITEM은 CItem으로 CInventory는 CItemSlot으로 wrapping하여 사용한다.
*	- Observable로써 보유한 아이템의 장/탈착, 습득, 버림에 따라 Observer들에게 알려준다.
*
* @Author		최종진
* @Date			2005/9/15
*/
class CItemSlot : public CObservable
{
public:
	CItemSlot(void);
	~CItemSlot(void);

	CItem*	AddItem( int iInvenIdx, tagITEM& Item );
	CItem*	AddItem( tagITEM& Item );
	bool	DelItem( int iInvenIdx );
	int		SetItem( int iInvenIdx, tagITEM& Item );
	void	SubItem( int iInvenIdx, tagITEM& Item );
	void	UpdateItem( int iInvenIdx, tagITEM& Item );
	void	ChangeItemStatus( int iInvenIdx );
	CItem*	GetItem( int iInvenIdx );
	int		GetEmptySlot( int iStartIdx, int iEndIdx );
	void	Clear();
private:
	CItem*	CreateItem( int iInvenIdx, tagITEM& Item );

private:
	
	std::vector< CItem*	>	m_listItems;

	CTCommand*	m_pCmdItemEquipInInventory;
	CTCommand*  m_pCmdItemUseInInventory;
	CTCommand*  m_pCmdItemEtcInInventory;
	CTCommand*	m_pCmdItemPatInInventory;
	CTCommand*	m_pCmdItemEquiped;
	CTCommand*  m_pCmdItemBullet;
	CTCommand*  m_pCmdItemPatEquiped;

	CTEventItem*	m_pEvent;
};
#endif