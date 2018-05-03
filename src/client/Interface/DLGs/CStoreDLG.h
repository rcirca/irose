#ifndef _STOREDLG_
#define _STOREDLG_

#include "tgamectrl/tdialog.h"
#include "../../Common/CItem.h"
#include "../../gamecommon/IObserver.h"
#include "subclass/cslot.h"
#include "../../GameData/CStore.h"
//enum StoreDLG 
//{
//	STORE_IMG,								//0,판낼
//	STORE_BTN_CLOSE = 3,					//1,닫기 
//	STORE_RADIOBOX  = 4,
//	STORE_BTN_TAB1  = 5,					//2,탭1
//	STORE_BTN_TAB2  = 6,					//3,탭2
//	STORE_BTN_TAB3  = 7,					//4,탭3
//	STORE_BTN_TAB4  = 8,
//	SROTE_MAX_BTN_CNT
//};


class CTCmdNumberInput;
class CTCmdOpenNumberInputDlg;

/**
* NPC상점용 다이얼로그
*	- Observable : CStore
*
* @Author		최종진
* @Date			2005/9/14
**/
class CStoreDLG : public CTDialog, public IObserver
{
private:
	CSlot						m_Slots[MAX_INV_TYPE][c_iSlotCountPerTab];	/// 판매 아이템이 Attach될 Slot들
	short						m_nInvType;									///< Item type[ wasting item, Equipment item, .. ]	

	CTCmdNumberInput*			m_pCmdNumberInput;
	CTCmdOpenNumberInputDlg*	m_pCmdOpenNumberInput;

	CDragItem*					m_pDragItem;

public:
	CStoreDLG( int iType );
	virtual ~CStoreDLG();

	virtual void Draw();
	virtual void Update( POINT ptMouse );
	virtual void Show();
	virtual void MoveWindow( POINT pt );
	virtual int	 IsInValidShow();
	
	virtual unsigned int Process( UINT uiMsg, WPARAM wParam, LPARAM lParam );

	virtual void Update( CObservable* pObservable, CTObject* pObj );

	void SetTab(short nInvType)			{ m_nInvType = nInvType; }

protected:
	enum{
		IID_BTN_CLOSE	= 20,
		IID_RADIOBOX	= 30,
		IID_BTN_TAB1	= 31,
		IID_BTN_TAB2	= 32,
		IID_BTN_TAB3	= 33,
		IID_BTN_TAB4	= 34
	};

	bool On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam );

	void SetEnableTabButton( int iInvType, bool bEnable );					
	void SetTabButtonText( short iIndex, const char* szText );
	void UpdateSlotPosition( int iType );

};


#endif //_STOREDLG_