#ifndef _DragCommand_
#define _DragCommand_
#include "tcommand.h"

#include "../InterfaceType.h"

/// 인벤토리에서 분리/분해 창으로 아이템 이동
class CTCmdDragItem2SeparateDlg : public CTCommand
{
public:
	CTCmdDragItem2SeparateDlg(void){}
	virtual ~CTCmdDragItem2SeparateDlg(void){}
	virtual bool Exec( CTObject* pObj );
};

/// 아이템을 퀵바에 등록
class CTCmdDragInven2QuickBar : public CTCommand
{
public:
#ifdef _NEW_UI
		CTCmdDragInven2QuickBar(void){	m_nType = DLG_TYPE_QUICKBAR;	}
	CTCmdDragInven2QuickBar( short nType )	{	m_nType = nType;	}
#else
	CTCmdDragInven2QuickBar(void){};
	virtual ~CTCmdDragInven2QuickBar(void){};
#endif
	virtual bool	Exec( CTObject* pObj );
	short			m_nType;
};

/// 스킬을 퀵바에 등록
class CTCmdDragSkill2QuickBar : public CTCommand
{
public:

#ifdef _NEW_UI
	CTCmdDragSkill2QuickBar(void){	m_nType = DLG_TYPE_QUICKBAR;	};
	CTCmdDragSkill2QuickBar( short nType )	{	m_nType = nType;	}
#else
		CTCmdDragSkill2QuickBar(void){	m_nType = DLG_TYPE_QUICKBAR;	}
	CTCmdDragSkill2QuickBar( short nType )	{	m_nType = nType;	}
#endif

	virtual ~CTCmdDragSkill2QuickBar(void){}
	virtual bool	Exec( CTObject* pObj );
	short			m_nType; 
};


/// 클랜 스킬을  퀵바에 스킬 등록
class CTCmdDragClanSkill2QuickBar : public CTCommand
{
public:
	CTCmdDragClanSkill2QuickBar(void){};
	virtual ~CTCmdDragClanSkill2QuickBar(void){}
	virtual bool	Exec( CTObject* pObj );
};

/// 퀵바에서 아이템(스킬등 포함)을 뺀다.
class CTCmdDragItemFromQuickBar : public CTCommand
{
public:
#ifdef _NEW_UI
		CTCmdDragItemFromQuickBar(void){	m_nType = DLG_TYPE_QUICKBAR;	}
	CTCmdDragItemFromQuickBar( short nType )	{	m_nType = nType;	}
#else
	CTCmdDragItemFromQuickBar(void){};
	virtual ~CTCmdDragItemFromQuickBar(void){};
#endif
	virtual bool	Exec( CTObject* pObj );
	short			m_nType;
};

/// 퀵바안에서 등록된 아이템(스킬등 포함)을 위치 이동한다.
class CTCmdMoveIconInQuickBar : public CTCommand
{
#ifdef _NEW_UI
	public:
	CTCmdMoveIconInQuickBar(void){	m_nType = DLG_TYPE_QUICKBAR;	}
	CTCmdMoveIconInQuickBar( short nType )	{	m_nType = nType;	}
	
	virtual ~CTCmdMoveIconInQuickBar(void){};
	virtual bool	Exec( CTObject* pObj );
	short			m_nType;
#else
public:
	CTCmdMoveIconInQuickBar(void){};
	virtual ~CTCmdMoveIconInQuickBar(void){};
	virtual bool	Exec( CTObject* pObj );

#endif
};


/// 인벤토리에서 캐릭터 창으로의 장착
class CTCmdDragItemEquipFromInven : public CTCommand
{
public:
	CTCmdDragItemEquipFromInven(void){}
	virtual ~CTCmdDragItemEquipFromInven(void){}
	virtual bool Exec( CTObject* pObj );
};


/// 아이템창 안에서 장착된 아이템을 Drag해서 탈착
class CTCmdDragItemFromEquipInItemDlg : public CTCommand
{
public:
	CTCmdDragItemFromEquipInItemDlg(void){}
	virtual ~CTCmdDragItemFromEquipInItemDlg(void){}
	virtual bool Exec( CTObject* pObj );
};

/// 아이템창 안에서 인벤토리에 있는 아이템을 Drag하는 Command
class CTCmdDragItemFromInvenInItemDlg : public CTCommand
{
public:
	CTCmdDragItemFromInvenInItemDlg(void){}
	virtual ~CTCmdDragItemFromInvenInItemDlg(void){}
	virtual bool Exec( CTObject* pObj );

};

/// 아이템창의 인벤토리에서 내가 개설한 개인상점으로 아이템을 이동하는 Command
class CTCmdDragItem2PrivateStoreDlg : public CTCommand
{
public:
	CTCmdDragItem2PrivateStoreDlg(void){}
	virtual ~CTCmdDragItem2PrivateStoreDlg(void){}
	virtual bool Exec( CTObject* pObj );
};

/// WishList에 아이템 추가
class CTCmdDragItem2WishList : public CTCommand
{
public:
	CTCmdDragItem2WishList(void){}
	virtual ~CTCmdDragItem2WishList(void){}
	virtual bool Exec( CTObject* pObj );
};

/// 내가 개설한 개인상점에서 판매 물품을 빼는 Command
class CTCmdDragSellItemFromPrivateStoreDlg : public CTCommand
{
public:
	CTCmdDragSellItemFromPrivateStoreDlg(void){}
	virtual ~CTCmdDragSellItemFromPrivateStoreDlg(void){}
	virtual bool Exec( CTObject* pObj );
};

/// 내가 개설한 개인상점에서 매입 물품을 빼는 Command
class CTCmdDragBuyItemFromPrivateStoreDlg : public CTCommand
{
public:
	CTCmdDragBuyItemFromPrivateStoreDlg(void){}
	virtual ~CTCmdDragBuyItemFromPrivateStoreDlg(void){}
	virtual bool Exec( CTObject* pObj );
};

/// 다른 아바타의 개인상점에서 물품을 살때 드래그하는   Command
class CTCmdDragSellItemFromAvatarStoreDlg : public CTCommand
{
public:
	CTCmdDragSellItemFromAvatarStoreDlg(void){}
	virtual ~CTCmdDragSellItemFromAvatarStoreDlg(void){}
	virtual bool Exec( CTObject* pObj );
};


/// 제련에서 드래그앤드랍으로 타겟 아이템을 빼는 Command
class CTCmdDragTakeoutTargetItemFromUpgradeDlg : public CTCommand
{
public:
	CTCmdDragTakeoutTargetItemFromUpgradeDlg(){}
	virtual ~CTCmdDragTakeoutTargetItemFromUpgradeDlg(){}
	virtual bool Exec( CTObject* pObj );
};


/// 제련에서 드래그앤드랍으로 재료 아이템을 빼는 Command
class CTCmdDragTakeoutMaterialItemFromUpgradeDlg : public CTCommand
{
public:
	CTCmdDragTakeoutMaterialItemFromUpgradeDlg(){}
	virtual ~CTCmdDragTakeoutMaterialItemFromUpgradeDlg(){}
	virtual bool Exec( CTObject* pObj );
};

/// 아이템 창에서 제련창으로 아이템을 드래그앤드랍하는 Command
class CTCmdDragTakeinItem2UpgradeDlg : public CTCommand
{
public:
	CTCmdDragTakeinItem2UpgradeDlg(){}
	virtual ~CTCmdDragTakeinItem2UpgradeDlg(){}
	virtual bool Exec( CTObject* pObj );
};
#endif