#ifndef _CSkillListItem_
#define _CSkillListItem_
#include "WinCtrl.h"
#include "CSlot.h"
#include "singlelinestring.h"

class CTButton;
class CIconSkill;
class CDragItem;

/**
* 스킬창에서 ZListBox의 item Class
*
* @Author		최종진
* @Date			2005/9/15
*/

class CSkillListItem : public CWinCtrl
{
public:
	CSkillListItem(void);
	virtual ~CSkillListItem(void);
	virtual void Draw();
	virtual void MoveWindow( POINT pt );
	virtual void Update( POINT ptMouse );
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void Show();
	virtual void Hide();
	
	void		SetIcon( CIconSkill* pIcon );
	CIconSkill*	GetIcon();

	void		Update();

protected:
	bool	IsValidLevelUp( int iSkillSlotIdx );
	bool	IsHasNextLevel( int iSkillSlotIdx );
protected:
	CSlot		m_Slot;
	CTButton*	m_pButton;
	CDragItem*	m_pDragItem;
	std::string m_error_msg;


	std::vector<CSinglelineString>	m_vecSingleString;
	CSinglelineString				m_strUseAbility;

};
#endif