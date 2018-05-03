#ifndef _CAVATARINFODLG_
#define _CAVATARINFODLG_

#include "tgamectrl/tdialog.h"
#include "../gamecommon/IObserver.h"
#include "subclass/CSlot.h"

class CItem;

/**
* 화면 왼쪽위 내 아타바에 대한 정보를 표시하는 인터페이스
*	- Observable : CItemSlot
*
* @Author		최종진
* @Date			2005/9/6
*/
class CAvatarInfoDlg : public CTDialog, public IObserver
{
public:
	CAvatarInfoDlg( int iType );
	virtual ~CAvatarInfoDlg(void);
	virtual bool Create ( const char* IDD );
	virtual void Draw();
	virtual void Update( POINT ptMouse );
	virtual void MoveWindow( POINT pt );
	virtual unsigned Process(UINT uiMsg,WPARAM wParam,LPARAM lParam);
	virtual void Update( CObservable* pObservable, CTObject* pObj );
	
	virtual void Show();
	virtual void Hide();
	virtual void RefreshDlg();
	virtual void SetInterfacePos_After();

	void		SetMaxView(bool bIsMax = true);
	void		SetMinView();
	bool		GetIsMaxView();

	void		SetMiniMapPos();

	enum
	{
		IID_GUAGE_HP = 6,					/// 체력게이지
		IID_GUAGE_MP,						/// 마나게이지
		IID_GUAGE_EXP,						/// 경험치게이지
		IID_BTN_SELFTARGET = 10,			/// 클릭시 자신선택 - NOIMAGE 
		IID_BTN_MENU,						/// 메뉴창을 여는 버튼
		IID_BTN_DIALOG2ICON,				/// 모든 Dialog Icon화 버튼, Icon화 불가능한 Dialog는 당근 동작 안한다.(TGameCtrl에서 처리할것, 멤버로 icon화 가능한지 Setting할것 )
		IID_BTN_SCREENSHOT					/// 스크린샷 찍는 버튼
	};
	

private:
	CSlot	m_Slot;							/// 장착된 무기에 대한 아이콘이 들어갈 슬롯
	CItem*	m_pRepresentBullet;				/// 장착된 무기에 대한 소모탄
	bool	m_bIsMaxView;					/// 확대 or 축소

};
#endif