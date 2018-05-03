#ifndef _CDeliveryStoreDlg_
#define _CDeliveryStoreDlg_

#include "tgamectrl/tdialog.h"
#include "subclass/cslot.h"

const int c_deliverystore_slot_count = 48;

class CDragItem;
struct tagBaseITEM;



/**
* 한국에서의 마일리지 아이템 창고( 몰아이템 창고) 용 다이얼로그
*
* @Warning		선물할 대상의 이름을 서버로 보내 있는지 체크해서 있다는 패킷을 받고 보낼것인가를 유저에게 다시 물어보는 구조이다.
* @Author		최종진
* @Date			2005/9/14
*/
class CDeliveryStoreDlg : public CTDialog
{
public:
	CDeliveryStoreDlg(void);
	virtual ~CDeliveryStoreDlg(void);
	virtual bool Create( const char* IDD );
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void Update( POINT ptMouse );
	virtual void MoveWindow( POINT pt );
	virtual void Draw();
	virtual void Show();
	virtual void Hide();

	void	AddItem( tagBaseITEM* pItem, const char* pszFrom, const char* pszDesc, const char* pszTo );
	void	SetItem( int slotindex, tagBaseITEM& Item );
	void	ClearItem(  );

	void	save_receiver_name( const char* pszName );
	int		get_selecteditem_slotindex();
	const char* GetSelectedItemName();
	const char* get_receiver_name();

private:
	enum{
		IID_BTN_CLOSE   = 20,
		IID_BTN_GIFT	= 21
	};

	CDragItem*					m_pDragItem;
	CSlot						m_Slots[c_deliverystore_slot_count];
	int							m_emptyslot;

	std::string					m_receiver_name;						/// 다른 유저에게 
	short						m_npc_client_object_index;
};

#endif