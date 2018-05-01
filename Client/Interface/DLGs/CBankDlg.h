#ifndef _CBANKDLG_
#define _CBANKDLG_

#include "../../../TGameCtrl/TDialog.h"
#include "../../Common/CItem.h"
#include "../../gamedata/cbank.h"
#include "../../gamecommon/iobserver.h"
#include "subclass/CSlot.h"

class CObservable;
class CDragItem;
/**
* 은행에 사용되는 다이얼로그( 인터페이스 )
*	- Observable : CBank( 은행 Data Class )
*
*
* @Author		최종진
* @Date			2005/9/12
*/
class CBankDlg : public CTDialog, public IObserver
{
public:
	CBankDlg( int iType );
	virtual ~CBankDlg(void);

	virtual void			Show();
	virtual void			Draw();
	virtual void			Update( POINT ptMouse );
	virtual unsigned int	Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual bool			Create( const char* szIDD );
	virtual void			MoveWindow( POINT pt );


	virtual void			Update( CObservable* pObservable, CTObject* pObj );

	int						GetCurrentTab();										
	bool					IsFullFirstTab();									/// 현재 첫번째 탭이 꽉 차 있는가?

	enum{
		IID_BTN_CLOSE		= 20,
		IID_BTN_SAVE		= 21, ///줄리 보관 버튼
		IID_BTN_WITHDRAW	= 22, ///줄리 찾기 버튼
		IID_RADIOBOX		= 30,
		IID_BTN_TAB1		= 31,
		IID_BTN_TAB2		= 32,
		IID_BTN_TAB3		= 33,
		IID_BTN_TAB4		= 34,
	};


private:
	void OnLButtonUp( unsigned iProcID, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown( unsigned iProcID, WPARAM wParam, LPARAM lParam );
	void SetTabButtonText( int iID, char* szText );

private:
	int			m_iCurrBankPage;												/// 현재 탭

	CSlot		m_Slots[g_iPageCount][g_iSlotCountPerPage];
	CDragItem*	m_pDragItem;
};
#endif