#ifndef _CSKILLSLOT_
#define _CSKILLSLOT_


#include "../../Common/CUserDATA.h"	
#include "../../gamecommon/cobservable.h"
#include "../../gamedata/event/CTEventSkill.h"
class CSkill;

/**
* CItemSlot과 같은 이유로 만들어진 스킬 컨테이너 클래스
*
* @Author		이동현, 최종진
* @Date			2005/9/15
*/
class CSkillSlot : public CObservable
{	
private:
	CSkill*						m_SkillSlot[ MAX_LEARNED_SKILL_CNT ];	
	CTEventSkill				m_Event;
	

public:
	CSkillSlot();
	~CSkillSlot();


	void	ClearSlot();
	void	InitSlot();

	//----------------------------------------------------------------------------------------------------
	/// 주목적은 타이머의 진행을 위해서임
	//----------------------------------------------------------------------------------------------------
	void	UpdateSkillSlot();


	void	SetSkillSlot( short nSlotIndex, short nSkillIndex, short nSkillLevel );
	void	RemoveBySlotIndex( short nSlotIndex );
	void	SkillLevelUp( int iSkillSlotNo, int iSkillIndex );

	CSkill* GetSkill( short nSlotIndex ){ return m_SkillSlot[ nSlotIndex ]; }
	CSkill* GetSkillBySkillIDX( int iSkillIDX );
	CSkill* GetSkillByBaseSkillIDX( int iBaseSkillIDX );

	short	GetSkillIndex( short nSlotIndex );
	short	GetSkillIndex( short nIndex, short nTabIndex );
	short	GetSkillLevel( short nSlotIndex );
	short	GetSkillLevel( short nIndex, short nTabIndex );
	short	GetSkillDelayTime( short nSlotIndex );
	short	GetSkillDelayTime( short nIndex, short nTabIndex );

	//----------------------------------------------------------------------------------------------------
	/// method for UI interface
	//----------------------------------------------------------------------------------------------------
	short	GetSkillIconIndex( short nSlotIndex );
	short	GetSkillIconIndex( short nIndex, short nTabIndex );	


	//----------------------------------------------------------------------------------------------------
	/// Pat 탑승여부에 따른 액티브스킬의 활성화 / 비활성화
	//----------------------------------------------------------------------------------------------------
	void	SetActiveSkillEnableByRideState( bool ride );

	//----------------------------------------------------------------------------------------------------
	/// 2인승 카트 Pat 보조석 탑승여부에 따른 액티브스킬의 활성화 / 비활성화
	//----------------------------------------------------------------------------------------------------
	void	SetActiveSkillEnableByRideState_Aid( bool ride );
};


#endif //_CSKILLSLOT_