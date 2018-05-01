#ifndef _SKILLDLG_
#define _SKILLDLG_


#include "../SlotContainer/CSkillSlot.h"
#include "../../gamecommon/IObserver.h"
#include "subclass/cslot.h"

#include "TDialog.h"

class CZListBox;



/**
* 현재 배운 스킬에 대한 정보를 보여주는 다이얼로그
*
* @Warning		스킬트리창을 여는 버튼은 대만판에서는 제거되어 있다( XML Data )
* @Author		최종진
* @Date			2005/9/14
*/
class CSkillDLG  : public CTDialog, public IObserver
{
private:

	int				m_iCurrentSkillTab;
public:
	CSkillDLG( int iType );
	virtual ~CSkillDLG();

	virtual void Draw( );
	virtual void Show( );
	virtual bool Create( const char* IDD );
	virtual bool IsVision();
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );

	virtual void Update( CObservable* pObservable, CTObject* pObj );


	enum SKILL_TAB_TYPE
	{
		TAB_BASIC	= 0,
		TAB_ACTIVE	= 1,
		TAB_PASSIVE	= 2
	};

	enum{
		IID_BTN_CLOSE	= 10,
		IID_BTN_ICONIZE,
		IID_BTN_OPEN_SKILLTREE,
		IID_TABBEDPANE	= 20,
		IID_TAB_BASIC	= 2,
		IID_BTN_BASIC	= 25,
		IID_ZLISTBOX_BASIC	= 26,
		IID_TAB_ACTIVE	= 0,
		IID_BTN_ACTIVE	= 35,
		IID_ZLISTBOX_ACTIVE = 36,
		IID_TAB_PASSIVE	= 1,
		IID_BTN_PASSIVE = 45,
		IID_ZLISTBOX_PASSIVE = 46
	};

protected:

	bool	On_LButtonUP( unsigned iProcID, WPARAM wParam, LPARAM lParam );
	void	DrawTexts();														
	bool	IsValidLevelUp( int iSkillSlotIdx );								/// 레벨업이 가능한 상태인가?
	void	RemoveSkillListItem( CZListBox* pListBox, int iSkillSlotIndex );
	void	UpdateSkillListBySkillLevelUp( CZListBox* pListBox );			/// 스킬의 레벨업시 스킬의 정보표시 갱신
	void	UpdateSkillLists();												/// 스킬에 대한 소모 능력치및 기타 변경사항 갱신
};


#endif //_SKILLDLG_