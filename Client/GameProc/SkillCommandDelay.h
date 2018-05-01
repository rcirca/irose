#ifndef _SKILL_COMMAND_DELAY_
#define _SKILL_COMMAND_DELAY_

#include "../Util/JSingleton.h"


///
/// class CSkillCommandDelay 
/// 
/// Managing delay of casting time
///

class CSkillCommandDelay : public CJSingleton< CSkillCommandDelay >
{
private:
	const DWORD		m_dwCastingTimeDelay;


	DWORD			m_dwCastingStartTime;
	bool			m_bCanCastSkill;
	int				m_iProgressRatio;
		


	HNODE			m_hCoverTexture;
	int				m_iAnimationFrame;


public:
	CSkillCommandDelay(void);
	~CSkillCommandDelay(void);

	
	bool			Init();
	void			Release();


	void			StartSkill();
	bool			CanCastSkill();

	void			Proc();
	void			Draw( int x, int y );


	int				GetSkillCommandDelayProgressRatio(){ return m_iProgressRatio; }
};

#endif //_SKILL_COMMAND_DELAY_
