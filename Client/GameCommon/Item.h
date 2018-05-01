#ifndef _CItem_
#define _CItem_

#include <list>
#include "../Common/CItem.h"
#include "../../TGameCtrl/TObject.h"
#include "../interface/CInfo.h"

class CIcon;
class CTCommand;
class CItemFragment;
class CItem : public CTObject
{
	friend	CItemFragment;
public:
	CItem(void);
	CItem(tagITEM* pItemData );
	virtual ~CItem(void);

	virtual void		GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType );
	void				GetToolTipRidePart_BattleCart( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType );
	virtual int			GetQuantity();
	virtual tagITEM&	GetItem();
	virtual int			GetIndex();
	virtual bool		IsEnable();


	int				GetType();
	void			Excute();
	void			Clear();
	
	void			SetIndex( int iIndex );


	void			SetIndexType( int iType );
	int				GetIndexType();
	void			SubQuantity( int iQuantity );
	void			SetLife( short nLife );
	bool			IsEmpty();
	
	const char*		GetName();
	int				GetItemNo();
	bool			IsEnableDupCNT();
	CIcon*			CreateItemIcon();

	//int			SubItemQuantity( int iQuantity ); 

	void		SetDefaultIcon( CIcon* pIcon );


	void		SetItem( tagITEM& Item );
	void		SetCommand( CTCommand* pCmd );
	CTCommand*	GetCommand();

	void		SetLastExecTime( DWORD dwTime );
	DWORD		GetLastExecTime();

	virtual void		SetUnitPrice( int iPrice );
	virtual int			GetUnitPrice();

	static	int			GetItemRareType( int type, int no );
	static  DWORD		GetItemNameColor( int type, int no );
	///INDEX TYPE
	enum{
		IT_NONE,
		IT_INVENTORY,
		IT_STORE,
	};
protected:

	///<-ToolTip생성용 Method
	void AddItemName( tagITEM& sItem, CInfo& Info );
	void AddItemWeight( tagITEM& sItem, CInfo& Info );
	void AddItemPatUseFuelRate( tagITEM& sItem, CInfo& Info );
	void AddItemPatAttackRange( tagITEM& sItem, CInfo& Info );
	void AddItemPatAttackPowSpeed( tagITEM& sItem, CInfo& Info );
	void AddItemPatCartGauage( tagITEM& sItem, CInfo& Info );
	void AddItemPatDesc( tagITEM& sItem, CInfo& Info );
	void AddItemPatVer( tagITEM& sItem, CInfo& Info );
	void AddItemPatClass( tagITEM& sItem, CInfo& Info );
	void AddItemPatEquipRequireJob( tagITEM& sItem, CInfo& Info );

	void AddItemUseNeedData( tagITEM& sItem, CInfo& Info );
	void AddItemMoveSpeed( tagITEM& sItem, CInfo& Info );
	void AddItemSpecialJob( tagITEM& sItem, CInfo& Info );
	void AddItemSpecialUnion( tagITEM& sItem, CInfo& Info );
	void AddItemEquipRequireJob( tagITEM& sItem, CInfo& Info );
	void AddItemEquipRequireUnion( tagITEM& sItem, CInfo& Info );
	void AddItemEquipRequireCondition( tagITEM& sItem, CInfo& Info );
	void AddItemDefaultBottomText( tagITEM& sItem, CInfo& Info );
	void AddItemDefaultTopTextDefenceItem( tagITEM& sITEM ,CInfo& Info );
	void AddItemDefence( tagITEM& sItem , CInfo& Info );
	void AddItemDefaultTopText( tagITEM& sItem, CInfo& Info );
	void AddItemOption( tagITEM& sItem, CInfo& Info );
	void AddItemPrice( tagITEM& sItem, DWORD dwDlgType, DWORD dwType, CInfo& Info );
	void AddItemLifeDuration( tagITEM& sItem, CInfo& Info );
	void AddItemPatAddAbility( tagITEM& sItem, CInfo& Info );




	short GetAttackSpeedType( short nSpeed );	// 어텍 스피드 타입( 매우빠름, 빠름... )

	void GetAttackSpeedString( short nSpeed, std::string& strMsg );
	int	 GetAttackPower( tagITEM& sItem );
	int	 GetDefence( tagITEM& sItem );
	int	 GetOption( tagITEM& sItem, short nOption, short nCol );
	const char* GetAddDataValueString( tagITEM& sItem, int iColumn );

	void	SetEnable();
	void	SetDisable();

protected:
	DWORD					m_dwLastExecTime;
	CTCommand*				m_pCommand;
	
	int						m_iIndex;
	int						m_iIndexType;

	CIcon*					m_pDefaultIcon;
	int						m_iStateLocation;
	int						m_iStateAction;
	tagITEM					m_Item;				///Location State가 LS_NONE일경우 사용된다.

	///인벤토리에 있는 아이템중 다른곳에서 부분적으로 사용중이라면 사용불가로 한다.
	/// ItemCommand, IconCommand, SlotCommand를 모두 막는다.
	bool					m_bEnable;
	int						m_iUnitPrice;
};

///*----------------------------------------------------------------------------------/
/// 인벤토리에서 개수만을 별도로 가지는 아이템( 예: 매매, 교환, 제조창에 올려진 아이템 )
/// 대부분의 기능은 내부적으로 가진 멤버 변수로 처리하고 일단 개수만 
/// 현재 매우 불안한 클래스 이다. 상황에 따라 CItem의 다른 Method들을 재정의해주어야 할것이다.
/// . Constructor에서 param으로 들어온 CItem을 SetDisable하고 Destuctor에서 SetEnable시킨다.
class CItemFragment : public CItem
{
public:
	CItemFragment( CItem* pItem );
	virtual ~CItemFragment();

	virtual void		GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType );
	virtual int			GetQuantity();
	virtual tagITEM&	GetItem();
	virtual int			GetIndex();
	virtual bool		IsEnable();
	virtual void		SetUnitPrice( int iPrice );
	virtual int			GetUnitPrice();

	void			SetQuantity( int iQuantity );
	//void			SetUnitPrice( int iUnitPrice );
	//int				GetUnitPrice();

	CItem*			GetInnerCItem();
	CIcon*			CreateItemIcon();


protected:
	//int			m_iUnitPrice;
	int			m_iQuantity;
	CItem*		m_pItem;
};
#endif  