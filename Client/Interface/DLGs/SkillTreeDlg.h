#ifndef _CSkillTreeDlg_
#define _CSkillTreeDlg_
#include "TDialog.h"
#include "../../gamecommon/IObserver.h"
class CIconSkill;
struct node_skill
{
	int		m_level;
	int		m_index;
	int     m_level_limit;
	int		m_offset_x;
	int		m_offset_y;
	CIconSkill* m_icon;
};

struct node_baseskill : public node_skill
{
	HNODE	m_texture;
	std::list<node_skill> m_children;
	void    clear();
};

struct s_userdata{
	int depth;
	std::list< node_baseskill > base_skills;
	void clear()
	{
		depth = 0;
		std::list< node_baseskill >::iterator iter;
		for( iter = base_skills.begin(); iter != base_skills.end();)
		{
			iter->clear();
			iter = base_skills.erase( iter );
		}
	}
};
/**
* 전체 스킬에 대한 정보를 트리형식으로 보여주는 다이얼로그
*	- 데이타는 XML형식으로 되어 있다.
*	- PARSER는 MSXML이 아닌 EXPAT을 사용한다.
*	- Observable : CSkillSlot
*
*
* @Warning	대만버젼의 경우 스킬트리창을 보여주지 않으며 데이타도 만들어져 있지 않다.
* @TODO		MSXML의 경우 호환성 문제로 낮은 버젼을 사용중인데 사용중인 버젼에서는 메모리에서 화일을 로드 할수 없다..
*			그래서 XML의 경우 가상화일시스템을 사용못하고 있다..그래서 테스트겸해서 EXPAT으로 구현하였다. 나중에 TGameCtrl에서의 Parser도 교체하는게 좋을듯하다.
* @Author	최종진
* @Date		2005/9/15
*/
class CSkillTreeDlg : public CTDialog, public IObserver
{
public:
	CSkillTreeDlg(void);
	virtual ~CSkillTreeDlg(void);
	virtual void Show();
	virtual void Hide();
	virtual void Draw();
	virtual void MoveWindow( POINT pt );
	virtual void Update( POINT ptMouse );
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	
	
	virtual void Update( CObservable* pObservable, CTObject* pObj );


	static CIconSkill* MakeIcon( int skillindex , int level, int level_limit );

private:

	void	LevelupSkill( int skillslotindex );
	void	AddSkill( int skillslotindex );
	void	RemoveSkill( int skillslotindex );
	enum{
		IID_BTN_ICONIZE		= 10,
		IID_BTN_CLOSE,
		IID_TEXT_SOLDIER	= 21,
		IID_TEXT_MUSE,
		IID_TEXT_HOWKER,
		IID_TEXT_DEALER
	};

	s_userdata	m_userdata;
};



#endif