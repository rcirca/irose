#include "stdafx.h"
#include ".\ciconskillclan.h"
#include "../SlotContainer/ClanSkillSlot.h"
#include "../../Object.h"
#include "../../GameCommon/Skill.h"
#include "../Dlgs/SubClass/CSlot.h"
#include "../interfacetype.h"
#include "../../util/Localizing.h"
#include "../../GameData/CClan.h"
#include "../../util/classTime.h"

CIconSkillClan::CIconSkillClan(void)
{
}

CIconSkillClan::CIconSkillClan( int iSkillSlot )
{
	SetSkillSlotToIcon( iSkillSlot );
}

CIconSkillClan::~CIconSkillClan(void)
{

}

int CIconSkillClan::GetIndex()
{
	return GetSkillSlotFromIcon();
}

void CIconSkillClan::SetSkillSlotToIcon( int iSkillSlot )
{
	m_iSkillSlot = iSkillSlot;
}

int	CIconSkillClan::GetSkillSlotFromIcon()
{
	return m_iSkillSlot;
}

int	CIconSkillClan::GetSkillLevel()
{
	CSkill* pSkill = GetSkill();
	if( pSkill )
		return pSkill->GetSkillLevel();

	return 0;
}

int	CIconSkillClan::GetSkillIndex()
{
	CSkill* pSkill = GetSkill();
	if( pSkill )
		return pSkill->GetSkillIndex();

	return 0;
}

CSkill*	CIconSkillClan::GetSkill()
{
	CClanSkillSlot* pSkillSlot = CClan::GetInstance().GetClanSkillSlot();
	return  pSkillSlot->GetSkill( m_iSkillSlot );
}

void	CIconSkillClan::Draw()
{
	/// get skill.
	CClanSkillSlot* pSkillSlot = CClan::GetInstance().GetClanSkillSlot();
	CSkill* pSkill = pSkillSlot->GetSkill( m_iSkillSlot );

	if( pSkill )
	{
		pSkill->DrawIcon( m_ptPosition.x, m_ptPosition.y );
	}
}
void	CIconSkillClan::ExecuteCommand()
{
	CClanSkillSlot* pSkillSlot = CClan::GetInstance().GetClanSkillSlot();
	CSkill* pSkill = pSkillSlot->GetSkill( m_iSkillSlot );

	if( pSkill )
	{
		pSkill->Execute();
	}
}
CIcon*	CIconSkillClan::Clone()
{
	return new CIconSkillClan( m_iSkillSlot );
}
void	CIconSkillClan::GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType )
{
	CIconSkill::GetToolTip( ToolTip, dwDialogType, dwType );
	CClanSkillSlot* pSkillSlot = CClan::GetInstance().GetClanSkillSlot();
	CSkill* pSkill = pSkillSlot->GetSkill( m_iSkillSlot );

	if( pSkill && pSkill->HasExpiredTime() )
	{
		SYSTEMTIME st;
		DWORD dwExpiredTime = pSkill->GetExpiredTime();
		classTIME::AbsSecondToSystem ( dwExpiredTime, st);
		ToolTip.AddString( 
			CStr::Printf("%s : %d/%d/%d %d:%02d", 
			STR_EXPIRED_TIME, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute ) );
	}
}

const char*	CIconSkillClan::GetName()
{
	CClanSkillSlot* pSkillSlot = CClan::GetInstance().GetClanSkillSlot();
	int iSkillIndex = pSkillSlot->GetSkillIndex( m_iSkillSlot );

	if( iSkillIndex >= 1 && iSkillIndex <= g_SkillList.Get_SkillCNT() )
		return SKILL_NAME( iSkillIndex );

	return NULL;
}
