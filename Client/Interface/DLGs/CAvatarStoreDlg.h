#ifndef _CAvatarStoreDlg_
#define _CAvatarStoreDlg_
#include "TDialog.h"
#include "subclass/CSlot.h"
#include "Net_Prototype.h"
#include "../../GameCommon/CObservable.h"
#include "../../GameData/Event/CTEventAvatarStore.h"


#include "SinglelineString.h"


const int c_iAvatarStoreMaxSlotCount = 30;
class CDragItem;
class CItem;
class CTCmdNumberInput;

/**
* 다른 유저의 상점을 이용할때 사용되는 Dialog( UI + Data )
*	- Observer : CDragNDropMgr
*
* @Author		최종진
* @Date			2005/9/12
*/
class CAvatarStoreDlg :	public CTDialog, public CObservable
{
public:
	CAvatarStoreDlg( int iDlgType );
	virtual ~CAvatarStoreDlg(void);
	virtual void Show();
	virtual void Hide();
	virtual void Draw();
	virtual void MoveWindow( POINT pt );
	virtual void Update( POINT ptMouse );
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	void	SetMasterSvrObjIdx( WORD wSvrObjIdx );
	WORD    GetMasterSvrObjIdx();
	void	SetTitle( const char* pszTitle );
	
	void    AddItem2SellList( tagPS_SLOT_PRICE& SlotITEM );						/// 서버에서 받은 판매 희망 아이템 추가
	void	AddItem2BuyList( tagPS_SLOT_PRICE& SlotITEM );						/// 서버에서 받은 구입 희망 아이템 추가

	void    UpdateSellList( tagPS_SLOT_ITEM& SlotITEM );						/// 판매 희망아이템 리스트에 변동사항이 있을경우 업데이트
	void	UpdateBuyList( tagPS_SLOT_ITEM& SlotITEM );							/// 구입 희망아이템 리스트에 변동사항이 있을경우 업데이트

	int		IsBuyItem( tagITEM& Item );											/// 개인상점에 내가 팔려는 아이템이 구입희망리스트에 있는 아이템인가?	

	int		GetWishIndex( tagITEM& Item ,int& iUnitPrice );

private:
	void	OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam );
	void	OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam );

private:
	enum{
		IID_BTN_CLOSE	= 20,
		IID_RADIOBOX	= 30,
		IID_BTN_SELL	= 31,
		IID_BTN_BUY		= 32,
	};
	int					m_iTab;										/// 현재 구입/판매탭 구분 저장

	CSinglelineString	m_title;									/// 상점 간판
	WORD				m_wMasterSvrObjIdx;							/// 상점주인의 서버인덱스

	CSlot				m_SellSlots[c_iAvatarStoreMaxSlotCount];	/// 판매희망아이템의 아이콘이 Attach될 슬롯들 
	CSlot				m_BuySlots[c_iAvatarStoreMaxSlotCount];		/// 구입희망아이템의 아이콘이 Attach될 슬롯들

	CItem*				m_SellItems[c_iAvatarStoreMaxSlotCount];	/// 판매희망 아이템들
	CItem*				m_BuyItems[c_iAvatarStoreMaxSlotCount];		/// 구입희망 아이템들

	CDragItem*			m_pSellDragItem;							/// 드래그&드랍에 사용될 CDragItem


	CTCmdNumberInput*	m_pBuyItemAtAvatarStore;					/// 판매 희망 아이템을 구입할때 사용될 TCommand( 숫자 입력기를 띄운다 )

	CTEventAvatarStore  m_Event;									/// Observer에게 보낼 이벤트
};
#endif