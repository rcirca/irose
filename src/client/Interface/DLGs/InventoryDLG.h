
#ifndef _INVENTORY_DLG_H
#define _INVENTORY_DLG_H

#include "tgamectrl/tdialog.h"
#include "../../gamecommon/IObserver.h"
#include "../Icon/CIconItem.h"
#include "subclass/CSlot.h"

// *--------------------------------------------------------------------* //
// 인벤토리 윈도우 
// *--------------------------------------------------------------------* //
enum invDLG {
	INV_IMG,							//0,판낼
	INV_BTN_CLOSE = 3,					//1,닫기 
	INV_BTN_TAB1  = 5,					//2,탭1
	INV_BTN_TAB2  = 6,					//3,탭2
	INV_BTN_TAB3  = 7,					//4,탭3
	INV_BTN_MONEY = 10,					//돈의 이동시 사용되는 버튼
	INV_MAX_BTN_CNT
};
class CDragItem;
class CTCmdNumberInput;

class CInventoryDLG : public CTDialog
{
private:
	CSlot				m_ItemSlots[ MAX_INV_TYPE ][ INVENTORY_PAGE_SIZE ];
	short				m_nInvType;	///< Item type[ wasting item, Equipment item, .. ]	
	
public:
	CInventoryDLG( int iType );
	virtual ~CInventoryDLG();

	virtual void Draw();
	virtual void MoveWindow( POINT ptMouse );
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void Hide();
	virtual void Show();
	virtual void Update( POINT pt );

	enum{
		INVEN_TYPE_EQUIP,
		INVEN_TYPE_USE,
		INVEN_TYPE_ETC
	};

	void	MoveIcon( int iInvenIdx );
	void	MoveIcon( int iVirtualInvenIdx1 , int iVirtualInvenIdx2 ,int iInvenType );
/*
	void	UpdateInventory();	
	/// 아이템 사용..
	bool	UseItem( short nRealIndex );
*/
	///인자를 이용해서 해당하는 첫번째 아이템의 실제 인벤토리 인덱스를 구한다.
	///@return  
	///	0 > :Real Inventory Index, 
	///	-1	:NotFound
	int		FindItem( int iItemType , int iItemNo );///대분류와 아이템 No( STB Line No )로 찾기
	int		FindItem( int iClass );///3자리의 중분류로 찾기


private:
	bool	On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam );
	void	UpdateSlotPosition( int iType );

/*	POINT	GetVirtualSlotIdxByRealInvenIdx(int iInvenIdx );
	short   GetVirtualSlotIdxByRealInvenIdxNType( int iType, int iInvenIdx );
	int		GetVirtualSlotIdxByPoint( POINT pt );
	int		GetVirtualEmptySlot( int iInvenType );

	short	GetMouse_PointITEM(tagITEM& sITEM,POINT ptPoint);

	void	CreateVirtualInventory();
	void    SaveVirtualInventoryTable();
*/
	void	DrawWeightANDGold();
/*
	/// 아이템 사용요구를 만족시키는가?
	bool	CheckBUseItem( short nSelectNo );
	*/
	CDragItem*	m_pDragItem;

	CTCmdNumberInput*	m_pCmdDropMoney;
	CTCmdNumberInput*	m_pCmdAddMyMoney2Exchange;

};




#endif