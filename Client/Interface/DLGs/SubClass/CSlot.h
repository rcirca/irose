#ifndef _CSLOT_
#define _CSLOT_


#include "WinCtrl.h"

class CIcon;
class CTCommand;
class CDragItem;

/**
*  CIcon을 Attach하여 가지고 있는 Class
*	- 한번에 한개씩만 가질수 있다.
*	- Detach시에 Icon을 delete한다.
*	- CTCommand를 가질수 있다.
*	- CDragItem을 가지면 Icon의 Drag&Drop을 시작시킨다
*
* @Author	최종진
* @Date		2005/9/15
*/
//-------------------------------------------------------------------------------------------
class CSlot : public CWinCtrl
{
public:
	CSlot(void);
	virtual ~CSlot(void);
	virtual bool	AttachIcon( CIcon* pIcon );
	virtual void	DetachIcon();
	virtual CIcon *	MoveIcon();

	virtual void	Draw();
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual void	MoveWindow( POINT pt );///Parent Dialog의 Left-Top Point
	virtual void	Update( POINT ptMouse );




	CIcon*	GetIcon(){ return m_pIcon; }

	void		SetParent( int iID );
	int			GetParent();
	void		SetDragAvailable();
	void		SetDragDisable();
		

	void		SetDragItem( CDragItem* pDragItem );
	CDragItem*	GetDragItem();
	void		SetCommand( CTCommand* pCmd );
	void		ResetClicked();

	void		SetTooltipType( DWORD dwType );
protected:
	void    SetIconPosition( POINT pt );
	void	SetPosition( POINT pt );


protected:
	CIcon*		m_pIcon;
	int			m_iParentID;		/// Slot의 Parent ID( 현재는 TDialog만 가능 ..)
	bool		m_bDragAvailable;	/// Slot에 Item이 있을때 Drag가능한가?
	CDragItem*	m_pDragItem;
	CTCommand*	m_pCommand;			/// Slot이 비어있을경우에 실행되는 Command
	bool		m_bClicked;
	POINT		m_ptClicked;
	DWORD       m_dwTooltipType;
};

/*
* 아이템 창의 Pat 탭에서 장착된 파츠를 보여주는 곳의 슬롯( 사용하지 않고 있다 )
*/
//class CSlotPatEquip : public CSlot
//{
//public:
//	CSlotPatEquip(void){}
//	virtual ~CSlotPatEquip(void){}
//	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
//	virtual void Draw();
//};

/**
* CPrivateStoreDlg에서 매입탭에 사용되는 Slot
*/
class CSlotBuyPrivateStore : public CSlot
{
public:
	CSlotBuyPrivateStore(void);
	virtual ~CSlotBuyPrivateStore(void){}
	virtual void DetachIcon();
	virtual void Draw();
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	void	SetExhibition( bool b );
	bool	IsExhibition();
	bool	IsSelected();
	void	SetSelected();
protected:
	bool		m_bExhibition;///찜목록에서 선택해서 매입 리스트에 올려진 아이템을 가지고 있는가?
	static		CSlotBuyPrivateStore* s_pSelectedSlot;

};
#endif