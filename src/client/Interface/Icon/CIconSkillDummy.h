#ifndef _CIconSkillDummy_
#define _CIconSkillDummy_
#include "ciconskill.h"

/**
* 스킬 트리창에서 사용되는 Skill Icon 
*	- 실제 배운 스킬이 아닌 전체 스킬을 보여주기위하여 더미 데이타를 가진 스킬 아이콘
*
* @Author	최종진
* @Date		2005/9/12
*/
class CIconSkillDummy :	public CIconSkill
{
public:
	CIconSkillDummy( void );
	CIconSkillDummy( int skillindex );
	virtual ~CIconSkillDummy(void);
	virtual void Draw();
	virtual int GetSkillIndex();
	virtual int	GetSkillLevel();
private:
	int		m_skillindex;
};
#endif