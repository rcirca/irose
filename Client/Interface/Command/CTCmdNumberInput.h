#ifndef _CTCMDNUMBERINPUT_
#define _CTCMDNUMBERINPUT_
#include "../../../tgamectrl/tcommand.h"
#include "../../Common/CItem.h"


/// Item이동시 숫자입력이 요구될수 있는 처리에 대한 Command
class CTCmdNumberInput : public CTCommand
{
public:
	CTCmdNumberInput(void){};
	~CTCmdNumberInput(void){};
	virtual bool Exec( CTObject* pObj) { return true; }
	void SetNumber( __int64 iNumber ){ m_iNumber = iNumber; }

protected:
	__int64	m_iNumber;
};


/// 인벤토리에서 바닥으로 아이템 버린다
class CTCmdDropItem : public CTCmdNumberInput
{
public:
	CTCmdDropItem(void){}
	virtual ~CTCmdDropItem(void){}
	virtual bool	Exec( CTObject* pObj);
};

/// @breief 인벤토리에서 바닥으로 아이템 돈을 버린다.
class CTCmdDropMoney : public CTCmdNumberInput
{
public:
	CTCmdDropMoney(void){}
	virtual ~CTCmdDropMoney(void){}
	virtual bool Exec( CTObject* pObj );
};

///1:1 거래시 인벤토리에서 거래창으로의 아이템 이동
class CTCmdAddMyItem2Exchange : public CTCmdNumberInput
{
public:
	CTCmdAddMyItem2Exchange(void){};
	virtual ~CTCmdAddMyItem2Exchange(void){};
	virtual bool	Exec( CTObject* pObj );
};

/// 1:1거래시 거래창에서 인벤토리로 아이템 이동
class CTCmdRemoveMyItemFromExchange : public CTCmdNumberInput
{
public:
	CTCmdRemoveMyItemFromExchange(void){};
	virtual ~CTCmdRemoveMyItemFromExchange(void){};
	virtual bool	Exec( CTObject* pObj );
};

/// 1:1 거래시 인벤토리에서 거래창으로 돈 이동
class CTCmdAddMyMoney2Exchange : public CTCmdNumberInput
{
public:
	CTCmdAddMyMoney2Exchange(void){};
	~CTCmdAddMyMoney2Exchange(void){};
	virtual bool Exec( CTObject* pObj );
};

/// 1:1 거래시 거래창에서 인벤토리로돈을 뺀다.
class CTCmdRemoveMyMoneyFromExchange : public CTCmdNumberInput
{
public:
	CTCmdRemoveMyMoneyFromExchange(void){};
	~CTCmdRemoveMyMoneyFromExchange(void){};

	virtual bool Exec( CTObject* pObj );
};


/// 인벤토리에서 창고로 아이템 이동
class CTCmdMoveItemInv2Bank : public CTCmdNumberInput
{
public:
	CTCmdMoveItemInv2Bank(void){}
	virtual ~CTCmdMoveItemInv2Bank(void){}
	virtual bool Exec( CTObject* pObj );
};

/// 창고에서 인벤토리로 아이템 이동
class CTCmdMoveItemBank2Inv : public CTCmdNumberInput
{
public:
	CTCmdMoveItemBank2Inv(){}
	virtual ~CTCmdMoveItemBank2Inv(void){}
	virtual bool Exec( CTObject* pObj );
};

/// NPC상점 거래시 인벤토리에서 거래창으로 아이템 이동
class CTCmdAddItem2DealFromInventory : public CTCmdNumberInput
{
public:
	CTCmdAddItem2DealFromInventory(void){}
	virtual ~CTCmdAddItem2DealFromInventory(void){}
	virtual bool Exec( CTObject* pObj );
};

/// NPC 상점 거래시 상점에서 거래창으로 아이템 이동
class CTCmdAddItem2DealFromStore : public CTCmdNumberInput
{
public:
	CTCmdAddItem2DealFromStore(void){}
	virtual ~CTCmdAddItem2DealFromStore(void){}
	virtual bool Exec(CTObject* pObj );
};

/// NPC상점거래시 상점에서 아이템을 직접 드래그&드랍하여 아이템 구입시 사용되는 Command
class CTCmdBuyItem : public CTCmdNumberInput
{
public:
	CTCmdBuyItem(void){}
	virtual ~CTCmdBuyItem(void){}
	virtual bool Exec(CTObject* pObj );
};

/// NPC상점거래시 인벤토리에서 아이템을 직접 드래그&드랍하여 아이템 판매시 사용되는 Command
class CTCmdSellItem : public CTCmdNumberInput
{
public:
	CTCmdSellItem(void){}
	virtual ~CTCmdSellItem(void){}
	virtual bool Exec(CTObject* pObj );
};

/// 개인상점에서 아이템을 산다.
class CTCmdBuyItemAtAvatarStore : public CTCmdNumberInput
{
public:
	CTCmdBuyItemAtAvatarStore(void){}
	virtual ~CTCmdBuyItemAtAvatarStore(void){}
	virtual bool Exec( CTObject* pObj );
};

/// 개인상점에 아이템을 판다.
class CTCmdSellItem2AvatarStore : public CTCmdNumberInput
{
public:
	CTCmdSellItem2AvatarStore(void){}
	virtual ~CTCmdSellItem2AvatarStore(void){}
	virtual bool Exec( CTObject* pObj );
};

/// 마일리지 창고에서 아이템을 가져온다.
class CTCmdBringItemFromMall : public CTCmdNumberInput
{
public:
	CTCmdBringItemFromMall(){}
	virtual ~CTCmdBringItemFromMall(){}
	virtual bool Exec( CTObject* pObj );
};

#endif