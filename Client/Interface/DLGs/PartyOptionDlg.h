#ifndef _CPartyOptionDlg_
#define _CPartyOptionDlg_
#include "tgamectrl/tdialog.h"
#include "../../GameCommon/IObserver.h"



/**
* 파티시 파티옵션을 보거나 바꿀수(파티장만) 있는 다이얼로그
*	- 파티창에서 버튼을 눌러 오픈한다.
*	- Observable : CParty
*
* @Author		최종진
* @Date			2005/9/15
*/
class CPartyOptionDlg :	public CTDialog, public IObserver
{
public:
	CPartyOptionDlg(void);
	virtual ~CPartyOptionDlg(void);
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual bool Create( const char* IDD );
	virtual void Draw();
	virtual void Show();
	virtual void Update( CObservable* pObservable, CTObject* pObj );
	enum{
		IID_BTN_CLOSE   = 10,
		IID_BTN_CONFIRM = 11,

		IID_RADIOBOX_ITEM						= 34,
		IID_RADIOBUTTON_ITEM_PICK				= 35,	///줍는사람이 임자 버튼
		IID_RADIOBUTTON_ITEM_SEQUENCE			= 36,	///일정순서대로 돌아간다( 누가 줍던지 간에 )
		IID_RADIOBOX_EXP						= 37,
		IID_RADIOBOX_EXP_EQUALITY				= 38,	///균등하게 분배
		IID_RADIOBOX_EXP_RATIO_LEVEL			= 39,	///레벨별로 경험치 분배
		IID_CHECKBOX_SHOW_PARTYMEMBER_HPGUAGE	= 40,
	};
private:
	void  OnLButtonUp( unsigned uiProcID );


	BYTE	m_party_rule;
};
#endif
