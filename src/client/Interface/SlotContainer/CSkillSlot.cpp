#include "stdafx.h"
#include "CSkillSlot.h"
#include "../../Common/IO_Skill.h"
#include "../../GameCommon/Skill.h"
#include "../../Country.h"



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief constructor
//----------------------------------------------------------------------------------------------------

CSkillSlot::CSkillSlot()
{
	memset( m_SkillSlot, 0, sizeof( m_SkillSlot ) );	
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief destructor
//----------------------------------------------------------------------------------------------------

CSkillSlot::~CSkillSlot()
{
	// Destruct
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief delete all skill slot
//----------------------------------------------------------------------------------------------------
void CSkillSlot::ClearSlot()
{
	for( int i = 0; i < MAX_LEARNED_SKILL_CNT ; i++ )
	{
		if( m_SkillSlot[ i ] != NULL )
		{
			m_Event.SetID( CTEventSkill::EID_DEL_SKILL );
			m_Event.SetIndex( i );
			SetChanged();
			NotifyObservers( &m_Event );
			
			delete m_SkillSlot[ i ];
			m_SkillSlot[ i ] = NULL;
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 스킬 슬롯 초기화
//----------------------------------------------------------------------------------------------------

void CSkillSlot::InitSlot()
{
	ClearSlot();
}


//----------------------------------------------------------------------------------------------------
/// 주목적은 타이머의 진행을 위해서임
//----------------------------------------------------------------------------------------------------
void CSkillSlot::UpdateSkillSlot()
{
	CSkill* pSkill = NULL;

	for( int i = 0; i < MAX_LEARNED_SKILL_CNT ; i++ )
	{
		pSkill = GetSkill( i );
		if( pSkill != NULL )
		{
			pSkill->Process();
		}
	}
}



//----------------------------------------------------------------------------------------------------
/// @param nSlotIndex 스킬 슬롯 인덱스
/// @param nSlotIndex 스킬 인덱스
/// @param nSlotIndex 스킬 레벨
/// @param nSlotIndex 스킬 슬롯 인덱스
/// @brief 새로운 스킬을 스킬 슬롯에 등록
//----------------------------------------------------------------------------------------------------

void CSkillSlot::SetSkillSlot(short nSlotIndex, short nSkillIndex, short nSkillLevel )
{
	if( m_SkillSlot[ nSlotIndex ] != NULL )
	{
		return;
	}

	CSkill* pSkill = g_SkillManager.CreateNewSkill( SKILL_SLOT_MAIN, nSkillIndex );
	if( pSkill == NULL )
	{
		//assert( 0 && "SetSkillSlot[ pSkill == NULL ]" );
		return;
	}

	pSkill->SetSkillSlot( nSlotIndex );
	m_SkillSlot[ nSlotIndex ] = pSkill;	

	m_Event.SetID( CTEventSkill::EID_ADD_SKILL );
	m_Event.SetIndex( nSlotIndex );
	SetChanged();
	NotifyObservers( &m_Event );
}
//----------------------------------------------------------------------------------------------------
/// @param nSlotIndex 스킬 슬롯 인덱스
/// @param nSlotIndex 스킬 인덱스
/// @brief 스킬레벨업을 하면서 타입이 변경되는 스킬일경우 기존것을 지우고 새로 만들어주어야 한다.
//----------------------------------------------------------------------------------------------------
void CSkillSlot::SkillLevelUp( int iSkillSlotNo, int iSkillIndex )
{
	assert( m_SkillSlot[ iSkillSlotNo ] ); 
	if( m_SkillSlot[ iSkillSlotNo ] == NULL )
		return;


	int iOldSkillType = SKILL_TYPE( iSkillIndex - 1 );
	int iNewSkillType = SKILL_TYPE( iSkillIndex );

	if( iOldSkillType != iNewSkillType )
	{
		CSkill* pSkill = g_SkillManager.CreateNewSkill( SKILL_SLOT_MAIN, iSkillIndex );
		if( pSkill == NULL )
			return;

		pSkill->SetSkillSlot( iSkillSlotNo );
		delete m_SkillSlot[ iSkillSlotNo ];
		m_SkillSlot[ iSkillSlotNo ] = pSkill;	

	}

	m_Event.SetID( CTEventSkill::EID_LEVELUP );
	m_Event.SetIndex( iSkillSlotNo );
	SetChanged();
	NotifyObservers( &m_Event );

}
//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 스킬번호로 스킬슬롯을 얻고 실제 스킬객체를 얻어온다.
///			서버로 부터는 스킬번호만 받기 때문에 역으로 번호로 슬롯을 얻어올 필요가 있다.( 타이머 세팅 )
//----------------------------------------------------------------------------------------------------

CSkill* CSkillSlot::GetSkillBySkillIDX( int iSkillIDX )
{
	CSkill* pSkill = NULL;

	for( int i = 0; i < MAX_LEARNED_SKILL_CNT ; i++ )
	{
		pSkill = GetSkill( i );
		if( pSkill != NULL )
		{
			if( pSkill->GetSkillIndex() == iSkillIDX )
				return pSkill;
		}
	}

	return pSkill;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 스킬번호로 스킬슬롯을 얻고 실제 스킬객체를 얻어온다.
///			서버로 부터는 스킬번호만 받기 때문에 역으로 번호로 슬롯을 얻어올 필요가 있다.( 타이머 세팅 )
//----------------------------------------------------------------------------------------------------
CSkill* CSkillSlot::GetSkillByBaseSkillIDX( int iBaseSkillIDX )
{
	CSkill* pSkill = NULL;

	for( int i = 0; i < MAX_LEARNED_SKILL_CNT ; i++ )
	{
		if( pSkill = GetSkill( i ) )
		{
			if( SKILL_1LEV_INDEX( pSkill->GetSkillIndex() ) == iBaseSkillIDX )
				return pSkill;
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 해당 슬롯의 스킬 번호를 얻어온다.
//----------------------------------------------------------------------------------------------------

short CSkillSlot::GetSkillIndex(short nSlotIndex)
{
	if( nSlotIndex >= MAX_LEARNED_SKILL_CNT 
		|| nSlotIndex < 0 
		|| m_SkillSlot[ nSlotIndex ] == NULL )
		return 0;
	
	return m_SkillSlot[ nSlotIndex ]->GetSkillIndex();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

short CSkillSlot::GetSkillIndex( short nIndex, short nTabIndex )
{
	if( nIndex < 0 )
		return 0;

	if( nTabIndex*( MAX_LEARNED_SKILL_CNT/4 ) + nIndex >= MAX_LEARNED_SKILL_CNT 
		|| m_SkillSlot[ nTabIndex*( MAX_LEARNED_SKILL_CNT/4 ) + nIndex ] == NULL )
		return 0;

	return m_SkillSlot[ nTabIndex*( MAX_LEARNED_SKILL_CNT/4 ) + nIndex ]->GetSkillIndex();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 해당 슬롯의 레벨을 얻어온다.
//----------------------------------------------------------------------------------------------------

short CSkillSlot::GetSkillLevel(short nSlotIndex)
{
	if( nSlotIndex >= MAX_LEARNED_SKILL_CNT 
		|| nSlotIndex < 0
		|| m_SkillSlot[ nSlotIndex ] == NULL )
		return 0;

	return m_SkillSlot[ nSlotIndex ]->GetSkillLevel();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

short CSkillSlot::GetSkillLevel( short nIndex, short nTabIndex )
{
	if( nIndex < 0 )
		return 0;

	if( nTabIndex*( MAX_LEARNED_SKILL_CNT/4 ) + nIndex >= MAX_LEARNED_SKILL_CNT 
		|| m_SkillSlot[ nTabIndex*( MAX_LEARNED_SKILL_CNT/4 ) + nIndex ] == NULL )
		return 0;

	return m_SkillSlot[ nTabIndex*( MAX_LEARNED_SKILL_CNT/4 ) + nIndex ]->GetSkillLevel();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 해당 슬롯의 Delay time를 얻어온다.
//----------------------------------------------------------------------------------------------------

short CSkillSlot::GetSkillDelayTime(short nSlotIndex)
{
	if( nSlotIndex >= MAX_LEARNED_SKILL_CNT 
		|| nSlotIndex < 0
		|| m_SkillSlot[ nSlotIndex ] == NULL )
		return 0;

	return m_SkillSlot[ nSlotIndex ]->GetSkillDelayTime();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

short CSkillSlot::GetSkillDelayTime( short nIndex, short nTabIndex )
{
	if( nIndex < 0 )
		return 0;

	if( nTabIndex*( MAX_LEARNED_SKILL_CNT/4 ) + nIndex >= MAX_LEARNED_SKILL_CNT 
		|| m_SkillSlot[ nTabIndex*( MAX_LEARNED_SKILL_CNT/4 ) + nIndex ] == NULL )
		return 0;

	return m_SkillSlot[ nTabIndex*( MAX_LEARNED_SKILL_CNT/4 ) + nIndex ]->GetSkillDelayTime();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 아이콘 번호를 얻어온다.
//----------------------------------------------------------------------------------------------------

short CSkillSlot::GetSkillIconIndex(short nSlotIndex)
{
	short nSkillIndex = GetSkillIndex( nSlotIndex );

	if( nSkillIndex == 0 )
		return 0;

	short nIconIndex = SKILL_ICON_NO( nSkillIndex );

	return nIconIndex;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

short CSkillSlot::GetSkillIconIndex( short nIndex, short nTabIndex )
{
	short nSkillIndex = GetSkillIndex( nIndex, nTabIndex );

	if( nSkillIndex == 0 )
		return 0;

	short nIconIndex = SKILL_ICON_NO( nSkillIndex );

	return nIconIndex;
}

void CSkillSlot::RemoveBySlotIndex( short nSlotIndex )
{
	assert( nSlotIndex >= 0 && nSlotIndex < MAX_LEARNED_SKILL_CNT );
	if( nSlotIndex >= 0 && nSlotIndex < MAX_LEARNED_SKILL_CNT )
	{
		if( m_SkillSlot[ nSlotIndex ] != NULL )
		{
			m_Event.SetID( CTEventSkill::EID_DEL_SKILL );
			m_Event.SetIndex( nSlotIndex );
			SetChanged();
			NotifyObservers( &m_Event );
			
			delete m_SkillSlot[ nSlotIndex ];
			m_SkillSlot[ nSlotIndex ] = NULL;
		}
	}
}

//--------------------------------------------------------------------------------------------------------/
/// Pat탑승 여부에 따른 스킬 활성화 / 비활성화 셋팅 : 2005/7/29 - nAvy
///
//--------------------------------------------------------------------------------------------------------/
void CSkillSlot::SetActiveSkillEnableByRideState( bool ride )
{
	_RPT0( _CRT_WARN," CSkillSlot::SetActiveSkillEnableByRideState()\n ");
	///배틀 카트가 적용되지 않는 버젼에서는 컬럼수가 틀리다.

#if defined(_GBC)
	if( ride )///pat 스킬만 Enable 나머지는 Disable
	{
		if( false == g_pAVATAR->m_Inventory.m_ItemRIDE[0].IsEmpty() )
		{
			int item_type = ITEM_TYPE( g_pAVATAR->m_Inventory.m_ItemRIDE[0].GetTYPE(), g_pAVATAR->m_Inventory.m_ItemRIDE[0].GetItemNO() );
			switch( item_type )
			{
			case TUNING_PART_BODY_CART:///카트 본체
				for( int index = MAX_LEARNED_SKILL_PER_PAGE; index < MAX_LEARNED_SKILL_PER_PAGE * 2; ++index )
				{
					if( m_SkillSlot[ index ] != NULL )
					{
						if( SKILL_ATTRIBUTE_CART == SKILL_ATTRIBUTE( m_SkillSlot[ index ]->GetSkillIndex() ) )
							m_SkillSlot[ index ]->SetEnable();
						else
							m_SkillSlot[ index ]->SetDisable();
					}
				}
				break;
			case TUNING_PART_BODY_CASTLEGEAR:///캐슬기어 본체
				for( int index = MAX_LEARNED_SKILL_PER_PAGE; index < MAX_LEARNED_SKILL_PER_PAGE * 2; ++index )
				{
					if( m_SkillSlot[ index ] != NULL )
					{
						if( SKILL_ATTRIBUTE_CASTLEGEAR == SKILL_ATTRIBUTE( m_SkillSlot[ index ]->GetSkillIndex() ) )
							m_SkillSlot[ index ]->SetEnable();
						else
							m_SkillSlot[ index ]->SetDisable();
					}
				}
				break;
			default:
				_RPTF0( _CRT_ASSERT,"Invalid Type");
				break;
			}
		}
	}
	else///아바타 상태 스킬만 Enable 나머지는 Disable
	{
		for( int index = MAX_LEARNED_SKILL_PER_PAGE; index < MAX_LEARNED_SKILL_PER_PAGE * 2; ++index )
		{
			if( m_SkillSlot[ index ] != NULL )
			{
				if( SKILL_ATTRIBUTE_AVATAR == SKILL_ATTRIBUTE( m_SkillSlot[ index ]->GetSkillIndex() ) )
					m_SkillSlot[ index ]->SetEnable();
				else
					m_SkillSlot[ index ]->SetDisable();
			}
		}
	}
#endif

}

//2인승 카트
void CSkillSlot::SetActiveSkillEnableByRideState_Aid( bool ride )
{
// 보조석의 유저는 공통 스킬 이외에는 사용하지 못한다.
// Stb 5번(스킬 유형 패턴) 컬럼 값이 1 : 공통 스킬.
#if defined(_GBC)

	if( ride )
	{
		for( int index = 0; index < MAX_LEARNED_SKILL_CNT; ++index )
		{
			if( m_SkillSlot[ index ] != NULL )
			{			
				// 이모티콘 사용 가능.
				if( SKILL_TYPE(m_SkillSlot[ index ]->GetSkillIndex())==SKILL_EMOTION_ACTION )
				{
					m_SkillSlot[ index ]->SetEnable();
				}
				else if( SKILL_TYPE(m_SkillSlot[ index ]->GetSkillIndex())==SKILL_BASE_ACTION )
				{
					// 기본스킬중에 13:카트 태우기 사용가능.
					if( SKILL_DISTANCE(m_SkillSlot[ index ]->GetSkillIndex())==13)
					{
						m_SkillSlot[ index ]->SetEnable();
					}
					else
					{
						m_SkillSlot[ index ]->SetDisable();
					}				
				}
				else
				{	
					m_SkillSlot[ index ]->SetDisable();
				}			
			}
		}
	}
	else
	{
		for( int index = 0; index < MAX_LEARNED_SKILL_CNT; ++index )
		{
			if( m_SkillSlot[ index ] != NULL )
			{			
				m_SkillSlot[ index ]->SetEnable();				
			}				
		}
	}	

#endif
}


