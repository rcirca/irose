#pragma once
#include "../../gamecommon/cobservable.h"
#include "NET_Prototype.h"
#include "../../Common/CUserDATA.h"	
#include "../../gamedata/event/CTEventClanSkill.h"


class CSkill;

/**
* 클랜스킬전용 컨테이너 클래스
*
* @Author		최종진
* @Date			2005/9/15
*/
class CClanSkillSlot :	public CObservable
{
private:
	CSkill*							m_SkillSlot[ MAX_CLAN_SKILL_SLOT ];	
	CTEventClanSkill				m_Event;

public:
	CClanSkillSlot(void);
	~CClanSkillSlot(void);

	void	ClearSlot();
	void	InitSlot();

	//----------------------------------------------------------------------------------------------------
	/// 주목적은 타이머의 진행을 위해서임
	//----------------------------------------------------------------------------------------------------
	void	UpdateSkillSlot();


	void	SetSkillSlot( short nSlotIndex, short nSkillIndex, short nSkillLevel ,DWORD expire_time_abssec);
	void	RemoveBySlotIndex( short nSlotIndex );
	void	SkillLevelUp( int iSkillSlotNo, int iSkillIndex );

	CSkill* GetSkill( short nSlotIndex ){ return m_SkillSlot[ nSlotIndex ]; }
	CSkill* GetSkillBySkillIDX( int iSkillIDX );
	CSkill* GetSkillByBaseSkillIDX( int iBaseSkillIDX );

	short	GetSkillIndex( short nSlotIndex );	
	short	GetSkillLevel( short nSlotIndex );
	
	short	GetSkillDelayTime( short nSlotIndex );
	short	GetSkillDelayTime( short nIndex, short nTabIndex );

	//----------------------------------------------------------------------------------------------------
	/// method for UI interface
	//----------------------------------------------------------------------------------------------------
	short	GetSkillIconIndex( short nSlotIndex );
	short	GetSkillIconIndex( short nIndex, short nTabIndex );	

};
