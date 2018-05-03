#ifndef _CPrivateStore_
#define _CPrivateStore_
#include "../GameCommon/CObservable.h"
#include "../GameCommon/IObserver.h"
#include "../Common/CItem.h"
#include "../Common/CUserData.h"
#include "Net_Prototype.h"
class CItem;
class CItemFragment;
struct t_PACKET;
/**
* 개인상점용 Data Class
* - 개인상점용 UI는 Show시에 Observer로 등록하고 Hide시에 해제한다.
* - Observer로 등록전에 WishList를 가져가서 매입리스트에 추가하고, BuyList, SellList( in UI )는 Clear한다.
* - CPrivateStore의 Buy, Sell List는 UI가 Show or Hide시에 별도로 Clear해준다.
* - 판매물품이 팔렸거나, 매입물품이 구입이 완료되었을 경우를 고려하자
* - 판매물품이 팔렸다면 판매리스트에서 빼고 매입물품이 구입된다면 개수에 따라서 매입리스트에서 뺀다.
*		-- CTEventItem : EID_DEL_ITEM,EID_CHANGE_ITEM를 처리하고 CItemSlot에 Observer로 Open시에 등록, Close시에 해제한다.
* - 리스트에 있는 아이템의 개수가 주는 경우와 없어지는 경우에 대해서도 생각해보자
*
* - 개인상점을 자신이 열때와 이용할때를 구분해야 한다.( Data or UI ?)
*		-- 일단 Dialog는 2개를 별도로 작성한다.( 개설과 이용 2가지 )
*		-- Data는 CPrivateStore를 쓰는것으로 하고 이용에 주의하자.( WishList는 항상 유지하고, BuyList와 SellList를 Clear해준다.)
*
* - 네트워크( 2004 / 6 / 9 )
*		-- 개인상점개설시 판매물품 
*			--- 구매자에게는 gsv_SET_MONEYnINV 와 gsv_P_STORE_RESULT가 온다.
*			--- 개인상점개설자에게는 gsv_P_STORE_RESULT만 온다. ->여기서 인벤토리에서 빼자, 물론 판매리스트에서도
*		-- 개인상점개설시 매입물품
*			--- 이용자와 개설자에게 gsv_SET_MOENYnINV과 gsv_P_STORE_RESULT가 모두 온다.
*
*
* @Author		최종진
* @Date			2005/9/15
*/
class CPrivateStore : public CObservable, public IObserver
{
	CPrivateStore(void);
	~CPrivateStore(void);
public:
	static CPrivateStore& GetInstance();

	virtual void Update( CObservable* pObservable, CTObject* pObj );

	void	AddItemSellList( CItem* pItem, int iQuantity, int iUnitPrice );
	void	SortItemSellList();
	void	RemoveItemSellList( int iIndex );
	CItem*	GetItemSellList( int iIndex );
	void	UpdateSellList( tagPS_SLOT_ITEM& SlotITEM );
	void	UpdateBuyList( tagPS_SLOT_ITEM& SlotITEM );


	void	AddItemBuyList( int iIndex, int iUnitCost, int iQuantity );
	void	RemoveItemBuyList( int iIndex );
	//CItem*	GetItemBuyList( int iIndex );


	void	AddItemWishList( tagITEM& Item , bool bSendPacket, int iSlotIndex = -1 );
	void	RemoveItemWishList( int iIndex );

	void	ClearSellList();
	void	ClearBuyList();


	bool	Open();
	void	Close();
	void	Clear();

	const	char* GetTitle();
	void	SetTitle( const char* pszTitle );

	CItem*	GetWishItem( int iIndex );
	bool	IsOpened();
	bool	IsValidOpen();
private:


private:
	std::string			m_strTitle;
	
	std::vector< CItemFragment* >		m_SellItems;///판매 아이템 리스트
	/// 구입 아이템 리스트
	std::list< CItemFragment* >			m_BuyItems; 
	CItem* 								m_WishItems[MAX_WISH_ITEMS];///찜리스트 - 항상 유지
	bool				m_bOpened;	///현재 상점이 열려있는가?
	
};
#endif