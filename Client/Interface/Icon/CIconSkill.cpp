#include "stdafx.h"
#include ".\ciconskill.h"
#include "../SlotContainer/CSkillSlot.h"
#include "../../Object.h"
#include "../../GameCommon/Skill.h"
#include "../Dlgs/SubClass/CSlot.h"
#include "../interfacetype.h"
#include "../it_mgr.h"
#include "../../util/Localizing.h"

#include "SplitHangul.h"
//----------------------------------------------------------------------------------------------------
/// Class CIconSkill
/// @brief	Skill 객체에 대한 View Class
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Constructor
//----------------------------------------------------------------------------------------------------
CIconSkill::CIconSkill(void)
{
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Constructor
//----------------------------------------------------------------------------------------------------
CIconSkill::CIconSkill( int iSkillSlot )
{
	SetSkillSlotToIcon( iSkillSlot );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Destructor
//----------------------------------------------------------------------------------------------------

CIconSkill::~CIconSkill(void)
{
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Overrided from CIcon
/// @modify CSlot에서 위치를 가져오지 않고 CSlot::MoveWindow에서 바꿔주는 Position을 사용한다. nAvy 2004/5/20
//----------------------------------------------------------------------------------------------------

void CIconSkill::Draw()
{

	/// get skill.
	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
	CSkill* pSkill = pSkillSlot->GetSkill( m_iSkillSlot );

	if( pSkill )
	{
		pSkill->DrawIcon( m_ptPosition.x, m_ptPosition.y );
	}

}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Overrided from CIcon
//----------------------------------------------------------------------------------------------------

void CIconSkill::ExecuteCommand()
{

	/// get skill.
	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
	CSkill* pSkill = pSkillSlot->GetSkill( m_iSkillSlot );

	if( pSkill  )
	{
		if( pSkill->IsEnable() )
			pSkill->Execute();
		else
			g_itMGR.AppendChatMsg( STR_CANT_CASTING_STATE, IT_MGR::CHAT_TYPE_SYSTEM );
	}
}
//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Overrided from CIcon
//----------------------------------------------------------------------------------------------------
CIcon*	CIconSkill::Clone()
{
	return new CIconSkill( m_iSkillSlot );
}
const char*	CIconSkill::GetName()
{
	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
	int iSkillIndex = pSkillSlot->GetSkillIndex( m_iSkillSlot );

	if( iSkillIndex >= 1 && iSkillIndex <= g_SkillList.Get_SkillCNT() )
		return SKILL_NAME( iSkillIndex );

	return NULL;
}

void	CIconSkill::GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType )
{

	switch( dwDialogType )
	{
	case DLG_TYPE_QUICKBAR:
#ifdef _NEW_UI // 2nd SkillBar
	case DLG_TYPE_QUICKBAR_EXT:
#endif
		dwType = INFO_STATUS_FEW;
		break;
	default:
		if( GetAsyncKeyState( VK_RBUTTON ) < 0 )
			dwType = INFO_STATUS_DETAIL;
		break;
	}



	//SIZE sizeString = {0,0};

	int		iWidth  = 0;
	int		iHeight = 0;

	int		iSkillNo = GetSkillIndex();
	int		iSkillLv = GetSkillLevel();

	if( dwType & INFO_STATUS_FEW )
	{
		AddSkillName( iSkillNo, ToolTip );
		AddSkillUseProperty( iSkillNo, ToolTip );
	}
	else
	{
		switch( SKILL_TYPE( iSkillNo) )
		{
		case SKILL_BASE_ACTION:///1
			{
				AddSkillName( iSkillNo, ToolTip, false );
				
				if( (dwType & INFO_STATUS_SIMPLE) || (dwType & INFO_STATUS_DETAIL) )
					AddSkillTypeTarget( iSkillNo, ToolTip );
				
				if( dwType & INFO_STATUS_DETAIL )
					AddSkillDesc( iSkillNo, ToolTip );

				break;
			}
		case SKILL_CREATE_WINDOW:///2
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip, false );
				AddSkillUseProperty( iSkillNo, ToolTip );

				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );

				if( dwType & INFO_STATUS_DETAIL )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );
						AddSkillTypeTarget( iSkillNo, ToolTip, false );
						AddSkillUseProperty( iSkillNo, ToolTip );


						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
				break;
			}
		case SKILL_ACTION_IMMEDIATE:///3,				///< 근접 즉시 발동.( 즉시 모션 교체 )
		case SKILL_ACTION_ENFORCE_WEAPON:///4,		///< 무기상태 변경( 강화, 효과 연출( 정령탄? ) )
		case SKILL_ACTION_ENFORCE_BULLET:///5,		///< 강화총알 변경 발사. ( 아이스 애로우.. 실제 화살이 변하는.. )
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillPower( iSkillNo, ToolTip );
				AddSkillStatus( iSkillNo, ToolTip );

				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );			
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );	

				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillPower( iNextLevelNo, ToolTip );
						AddSkillStatus( iNextLevelNo, ToolTip );


						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
		
				break;
			}
		case SKILL_ACTION_FIRE_BULLET:///6,			///< 발사.( 파이어볼 )
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillPower( iSkillNo, ToolTip );
				AddSkillDistanceScope( iSkillNo, ToolTip, false );
				AddSkillStatus( iSkillNo, ToolTip );
				
				
				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );

				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillPower( iNextLevelNo, ToolTip );
						AddSkillDistanceScope( iNextLevelNo, ToolTip, false );
						AddSkillStatus( iNextLevelNo, ToolTip );

						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
				break;
			}
		case SKILL_ACTION_AREA_TARGET :///= 7,			///< 지역 공격마법( 당근 범위.. )
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillPower( iSkillNo, ToolTip );
				AddSkillDistanceScope( iSkillNo, ToolTip, true );
				AddSkillStatus( iSkillNo, ToolTip );


				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );
				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillPower( iNextLevelNo, ToolTip );
						AddSkillDistanceScope( iNextLevelNo, ToolTip, true );
						AddSkillStatus( iNextLevelNo, ToolTip );

						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
				break;
			}
		case SKILL_ACTION_SELF_BOUND_DURATION:///=8
		case SKILL_ACTION_SELF_STATE_DURATION:/// = 12,		///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
			{
				AddSkillName( iSkillNo, ToolTip );

				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillScope( iSkillNo, ToolTip );
				AddSkillStatus( iSkillNo, ToolTip );


				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );

				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillScope( iNextLevelNo, ToolTip );
						AddSkillStatus( iNextLevelNo, ToolTip );


						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
				break;
			}
		case SKILL_ACTION_TARGET_BOUND_DURATION:/// = 9,	///< 타겟에게 발동 지속 마법.( 캐스팅 유 ) 능력치
		case SKILL_ACTION_TARGET_STATE_DURATION:/// = 13,	///< 상대에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillDistanceScope( iSkillNo, ToolTip, true );
				AddSkillStatus( iSkillNo, ToolTip );
				

				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );				
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );
				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillDistanceScope( iNextLevelNo, ToolTip, true );
						AddSkillStatus( iNextLevelNo, ToolTip );

						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
		
				break;
			}
		case SKILL_ACTION_SELF_BOUND:/// = 10,			///< 자신에게 발동 바로 업 마법.( 캐스팅 유 ) 능력치
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillScope( iSkillNo, ToolTip );
				AddSkillStatus( iSkillNo, ToolTip );
				
				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );			
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );
				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillScope( iNextLevelNo, ToolTip );
						AddSkillStatus( iNextLevelNo, ToolTip );

						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
				break;
			}
		case SKILL_ACTION_TARGET_BOUND:/// = 11,			///< 타겟에게 발동 바로 업 마법.( 캐스팅 유 ) 능력치
			{

				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillDistanceScope( iSkillNo, ToolTip , true);
				AddSkillStatus( iSkillNo, ToolTip );

				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );			
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );
				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillDistanceScope( iNextLevelNo , ToolTip, true);
						AddSkillStatus( iNextLevelNo, ToolTip );
						
						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
				break;
			}
		case SKILL_ACTION_SUMMON_PET:/// = 14,				///< 팻 소환 스킬
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip, false );
				AddSkillUseProperty( iSkillNo, ToolTip );
				///필요 소환량
				if( int iNpcNo = SKILL_SUMMON_PET( iSkillNo ) )
					ToolTip.AddString( CStr::Printf("%s : %d", STR_REQUIRE_SUMMONQUANTITY ,NPC_NEED_SUMMON_CNT( iNpcNo ) ) );


				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );
				if( dwType & INFO_STATUS_DETAIL )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );
						AddSkillTypeTarget( iNextLevelNo, ToolTip, false );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						
						if( int iNpcNo = SKILL_SUMMON_PET( iNextLevelNo ) )
							ToolTip.AddString( CStr::Printf("%s : %d", STR_REQUIRE_SUMMONQUANTITY ,NPC_NEED_SUMMON_CNT( iNpcNo ) ) );

						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
				break;
			}
		case SKILL_ACTION_PASSIVE:/// = 15,	
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip, false );
				AddSkillStatus( iSkillNo, ToolTip );

				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );			
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );
				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip , false);
						AddSkillStatus( iNextLevelNo, ToolTip );

						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
				break;
			}

		case SKILL_EMOTION_ACTION:/// = 16
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip, false );
				AddSkillUseProperty( iSkillNo, ToolTip );
				break;
			}
		case SKILL_ACTION_SELF_DAMAGE:/// = 17
			{

				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillPower( iSkillNo, ToolTip );
				AddSkillScope( iSkillNo, ToolTip );
				AddSkillStatus( iSkillNo, ToolTip );

				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo , ToolTip);
				AddSkillRequireSkill( iSkillNo, ToolTip);				
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );				
				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo , ToolTip);
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillPower( iNextLevelNo , ToolTip);
						AddSkillScope( iNextLevelNo , ToolTip);
						AddSkillStatus( iNextLevelNo , ToolTip);

						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo , ToolTip);
						AddSkillRequireSkill( iNextLevelNo , ToolTip);
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}

				break;
			}
		case SKILL_ACTION_SELF_AND_TARGET:/// 19번
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillPower( iSkillNo, ToolTip );
				AddSkillSuction( iSkillNo, ToolTip );
				AddSkillStatus( iSkillNo, ToolTip );

				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );			
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );	
				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillPower( iNextLevelNo, ToolTip );
						AddSkillSuction( iNextLevelNo, ToolTip );
						AddSkillStatus( iNextLevelNo, ToolTip );


						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
				break;
			}
		case SKILL_ACTION_RESURRECTION:
			{
				AddSkillName( iSkillNo, ToolTip );
				AddSkillTypeTarget( iSkillNo, ToolTip );
				AddSkillUseProperty( iSkillNo, ToolTip );
				AddSkillDistanceScope( iSkillNo, ToolTip, true );
				ToolTip.AddString( CStr::Printf("%s : %d%%",STR_RECOVERY_EXP,SKILL_POWER( iSkillNo )) );
				//AddSkillStatus( iSkillNo, ToolTip );

				AddSkillRequireJob( iSkillNo, ToolTip );
				AddSkillRequireAbility( iSkillNo, ToolTip );
				AddSkillRequireSkill( iSkillNo, ToolTip );				
				AddSkillRequireSkillPoint2Learn( iSkillNo ,ToolTip );
				AddSkillRequireEquip( iSkillNo, ToolTip );
				if( (dwType & INFO_STATUS_DETAIL)  )
				{
					AddSkillDesc( iSkillNo, ToolTip );
					int iNextLevelNo = 0;
					if( iNextLevelNo = GetSkillNextLevelNo( iSkillNo ) )
					{
						AddSkillNextLevelTitle( iNextLevelNo, ToolTip );

						AddSkillTypeTarget( iNextLevelNo, ToolTip );
						AddSkillUseProperty( iNextLevelNo, ToolTip );
						AddSkillDistanceScope( iNextLevelNo, ToolTip, true );
						ToolTip.AddString( CStr::Printf("%s : %d%%",STR_RECOVERY_EXP,SKILL_POWER( iNextLevelNo )) );
						//AddSkillStatus( iNextLevelNo, ToolTip );

						AddSkillRequireJob( iNextLevelNo, ToolTip );
						AddSkillRequireAbility( iNextLevelNo, ToolTip );
						AddSkillRequireSkill( iNextLevelNo, ToolTip );
						AddSkillRequireSkillPoint( iSkillNo, ToolTip );
					}
				}
			}
			break;
		default:
			{
				AddSkillName( iSkillNo , ToolTip);
				ToolTip.AddString("New Type Skill");
				AddSkillDesc( iSkillNo , ToolTip);
			}
			break;
		}
	}	

}



///*---------------------------------------------------------------------------------------------/
/// 스킬 툴팁 관련
void CIconSkill::AddSkillTypeTarget( int iSkillNo, CInfo& ToolTip , bool bAddTarget )
{
	char* pszBuf = NULL;
	if( bAddTarget )
		pszBuf = CStr::Printf( "%s:%s    %s:%s",
					STR_ITEM_TYPE, 
					CStringManager::GetSingleton().GetSkillType( SKILL_TYPE( iSkillNo )), 
					STR_TARGET, 
					CStringManager::GetSingleton().GetSkillTarget( SKILL_CLASS_FILTER(iSkillNo) ) );
	else
		pszBuf = CStr::Printf( "%s:%s",STR_ITEM_TYPE, CStringManager::GetSingleton().GetSkillType(SKILL_TYPE( iSkillNo )) );
		
	ToolTip.AddString( pszBuf );
}
void CIconSkill::AddSkillSummon( int iSkillNo, CInfo& ToolTip )
{
	char* pszBuf = CStr::Printf( "[%s:%s(%d)]", STR_SUMMON_MOB,NPC_NAME(SKILL_SUMMON_PET(iSkillNo)), SKILL_LEVEL( iSkillNo ) );
	ToolTip.AddString( pszBuf,g_dwBlueToolTip );
}

void CIconSkill::AddSkillDistanceScope( int iSkillNo, CInfo& ToolTip ,bool bAddScope )
{
	///거리가 0일경우는 표시하지 않는다.
	char* pszBuf;
	int iDistance = SKILL_DISTANCE( iSkillNo ) / 100;
	int iScope    = SKILL_SCOPE( iSkillNo ) / 100;

	if( bAddScope )
	{
		if( iDistance && iScope )
		{
			pszBuf = CStr::Printf( "%s : %dm %s : %dm",
				STR_SHOOT_RANGE, iDistance,
				STR_APPLY_RANGE, iScope );
			ToolTip.AddString( pszBuf );
		}
		else if( iDistance )
		{
			pszBuf = CStr::Printf( "%s : %dm", STR_SHOOT_RANGE, iDistance );
			ToolTip.AddString( pszBuf );
		}
		else if( iScope )
		{
			pszBuf = CStr::Printf( "%s : %dm", STR_APPLY_RANGE, iScope );
			ToolTip.AddString( pszBuf );
		}
	}
	else
	{
		if( iDistance )
		{
			pszBuf = CStr::Printf( "%s : %dm",STR_SHOOT_RANGE, iDistance );
			ToolTip.AddString( pszBuf );
		}
	}
	
}
void CIconSkill::AddSkillScope( int iSkillNo, CInfo& ToolTip )
{
	if( SKILL_SCOPE( iSkillNo ) )
	{
		char* pszBuf = CStr::Printf( "%s : %dm",STR_APPLY_RANGE,SKILL_SCOPE( iSkillNo ) / 100);
		ToolTip.AddString( pszBuf );
	}
}
//void CIconSkill::AddSkillIncreaseAbility( int iSkillNo, CInfo& ToolTip )
//{
//	char* pszBuf;
//	for( int i = 0; i < SKILL_INCREASE_ABILITY_CNT; ++i )
//	{
//		if( SKILL_INCREASE_ABILITY( iSkillNo, i ) )
//		{
//			int iAbility = SKILL_INCREASE_ABILITY( iSkillNo, i );
//			int iValue   = SKILL_INCREASE_ABILITY_VALUE( iSkillNo, i );
//			pszBuf = CStr::Printf("[%s:%s %d] ",STR_EFFECT,CStringManager::GetSingleton().GetAbility( iAbility ), iValue );
//
//			ToolTip.AddString( pszBuf, g_dwBlueToolTip );
//		}
//	}
//}

void CIconSkill::AddSkillName( int iSkillNo ,CInfo& ToolTip, bool bAddLevel )
{
	if( bAddLevel )
	{
		char* pszBuf = CStr::Printf( "%s[%s:%d]", SKILL_NAME(iSkillNo ),STR_LEVEL, SKILL_LEVEL(iSkillNo ));
		ToolTip.AddString( pszBuf ,g_dwYELLOW, g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD] );
	}
	else
	{
		ToolTip.AddString( SKILL_NAME( iSkillNo ) ,g_dwYELLOW, g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD]);
	}
}

void CIconSkill::AddSkillDuration( int iSkillNo, CInfo& ToolTip, DWORD color )
{
	char* pszBuf = CStr::Printf( "%s:%d%s", STR_CONTINUE_TIME,SKILL_DURATION( iSkillNo) ,STR_SECOND);
	ToolTip.AddString( pszBuf , color );
}

int  CIconSkill::GetSkillNextLevelNo( int iSkillNo )
{

	if( iSkillNo >= g_SkillList.Get_SkillCNT() )///최대값
		return 0;

	if( SKILL_1LEV_INDEX( iSkillNo ) == SKILL_1LEV_INDEX( iSkillNo + 1 ) )
	{
		if( SKILL_LEVEL( iSkillNo + 1 ) == SKILL_LEVEL( iSkillNo ) + 1 )
			return iSkillNo + 1;
	}
	return 0;
}

void CIconSkill::AddSkillNextLevelTitle( int iSkillNo, CInfo& ToolTip )
{
	ToolTip.AddString(" ");
	char* pszBuf = CStr::Printf( "[%s:%d]" ,STR_NEXT_LEVEL, SKILL_LEVEL( iSkillNo ));
	ToolTip.AddString( pszBuf );
}

void CIconSkill::AddSkillPower( int iSkillNo, CInfo& ToolTip  )
{
	std::string		strMsg = STR_SKILL_POWER;
	strMsg.append( " : " );

	switch( SKILL_DAMAGE_TYPE( iSkillNo ) )
	{
	case 0:
		strMsg.append( STR_SKILLPOWER_EFFECT_0 );
		break;
	case 1:
		strMsg.append( STR_SKILLPOWER_EFFECT_1 );
		break;
	case 2:
		strMsg.append( STR_SKILLPOWER_EFFECT_2 );
		break;
	case 3:
		strMsg.append( STR_SKILLPOWER_EFFECT_3 );
		break;
	default:
		break;
	}

	strMsg.append( CStr::Printf( "(%d)",  SKILL_POWER( iSkillNo ) ) );
	ToolTip.AddString( strMsg.c_str() );
}


void  CIconSkill::AddSkillDesc( int iSkillNo, CInfo& ToolTip )
{
	int iMaxSizeString = ToolTip.GetMaxSizeString();

	CSplitHangul	splitHAN( (char*)SKILL_DESC( iSkillNo ), iMaxSizeString ,CLocalizing::GetSingleton().GetCurrentCodePageNO());
	short	nCNT   = splitHAN.GetLineCount();
	if( nCNT > 0 )
		ToolTip.AddString(" ");

	for(short nI=0;nI<nCNT;nI++) 
		ToolTip.AddString( splitHAN.GetString(nI) );

}
void CIconSkill::AddSkillUseProperty( int iSkillNo, CInfo& ToolTip )
{
	char* pszBuf;
	DWORD dwColor = 0;
	int		iUseValue = 0; 
	for( int i = 0; i < SKILL_USE_PROPERTY_CNT; ++i )
	{
		if( SKILL_USE_PROPERTY(iSkillNo, i) )
		{
			iUseValue = g_pAVATAR->Skill_ToUseAbilityVALUE( iSkillNo, i );

			pszBuf = CStr::Printf( "[%s:%s %d]",
				STR_CONSUME_ABILITY, 
				CStringManager::GetSingleton().GetAbility( SKILL_USE_PROPERTY(iSkillNo,i) ), 
				iUseValue 
			);

			if( g_pAVATAR->GetCur_AbilityValue(SKILL_USE_PROPERTY( iSkillNo, i)) >= iUseValue )
				dwColor = g_dwGREEN;
			else
				dwColor = g_dwRED;
				ToolTip.AddString( pszBuf, dwColor );
		}
	}
}

void CIconSkill::AddSkillRequireEquip( int iSkillNo, CInfo& ToolTip )
{
	std::string strTemp("[");
	strTemp.append(  STR_REQUIRE_EQUIP  );
	strTemp.append(":");

	int iCount = 0;
	DWORD dwColor = g_dwRED;
	const char* pszChar;
	tagITEM	EquipItem;

	for( int i = 0; i < SKILL_NEED_WEAPON_CNT ;++i )
	{
		if( SKILL_NEED_WEAPON(iSkillNo,i) )///아이템 타입이다
		{
			for( int iEquipIdx = 0; iEquipIdx < MAX_EQUIP_IDX; ++iEquipIdx )
			{
				EquipItem = g_pAVATAR->m_Inventory.m_ItemEQUIP[ iEquipIdx ];
				if( !EquipItem.IsEmpty() )
				{
					if( ITEM_TYPE( EquipItem.GetTYPE(), EquipItem.GetItemNO() ) == SKILL_NEED_WEAPON(iSkillNo,i) )
					{
						dwColor = g_dwGREEN;
						break;
					}
				}
			}

			pszChar = CStringManager::GetSingleton().GetItemType( SKILL_NEED_WEAPON(iSkillNo,i) );
	
			if( pszChar )
			{
				strTemp.append( pszChar );
				strTemp.append(" ");
				++iCount;
			}
		}
	}
	if( iCount )
	{
		strTemp.append("]");	
		ToolTip.AddString( strTemp.c_str(), dwColor );
	}
}

void CIconSkill::AddSkillRequireUnion( int iSkillNo,CInfo& ToolTip  )
{
	std::string		strTemp("[조합:");

	DWORD	dwColor		= g_dwRED;
	int		iCheckCount = 0;
	bool	bCorrect	= false;


	for( int i = 0; i < SKILL_AVAILBLE_UNION_CNT; i++ )
	{
		if( 0 != SKILL_AVAILBLE_UNION( iSkillNo, i ))
		{
			++iCheckCount;
			strTemp.append(" ");
			strTemp.append( UNION_NAME( SKILL_AVAILBLE_UNION( iSkillNo, i )));

			if( SKILL_AVAILBLE_UNION( iSkillNo, i ) == g_pAVATAR->GetCur_UNION() )
			{
				bCorrect = true;
				break;
			}
		}
	}

	strTemp.append("]");

	if( iCheckCount )
	{
		if( bCorrect )
			dwColor = g_dwGREEN;
		
		ToolTip.AddString( strTemp.c_str(), dwColor );
	}
}

void CIconSkill::AddSkillRequireJob( int iSkillNo, CInfo& ToolTip  )
{
	int iClass =  SKILL_AVAILBLE_CLASS_SET( iSkillNo);
	///REMARK FOR TEST<- 2004.3.18.nAvy LIST_SKILL.STB가 아직 수정안되있으므로 해서 임시적으로 막는 코드
	if( iClass >= g_TblClass.m_nDataCnt )
		return;
	///->
	if( iClass )
	{
		const char* pszTemp = CStr::Printf("[%s %s]",STR_REQUIRE_JOB, CLASS_NAME(iClass));
		DWORD  color = g_dwRED;

		int iJob = g_pAVATAR->Get_JOB();

		if( iJob )
		{
			for( int i = 0; i < CLASS_INCLUDE_JOB_CNT; ++i )
			{
				if( iJob == CLASS_INCLUDE_JOB(iClass,i) )
				{
					color = g_dwGREEN;
					break;
				}
			}
		}
		ToolTip.AddString( pszTemp, color );
	}
}


void CIconSkill::AddSkillRequireSkill( int iSkillNo, CInfo& ToolTip  )
{
	std::string strTemp("[");

	strTemp.append( STR_REQUIRE_SKILL );
	strTemp.append(":");
	DWORD dwColor = g_dwGREEN;

	int iCount = 0;
	int iLearnSkillLv = 0;
	for( int i = 0; i < SKILL_NEED_SKILL_CNT; ++i )
	{
		if( SKILL_NEED_SKILL_INDEX( iSkillNo, i ) )
		{
			strTemp.append( SKILL_NAME( SKILL_NEED_SKILL_INDEX( iSkillNo, i ) + SKILL_NEDD_SKILL_LEVEL( iSkillNo, i ) - 1) );
			strTemp.append( 
				CStr::Printf("(%s:%d)", 
						CStringManager::GetSingleton().GetAbility( AT_LEVEL ), 
						SKILL_NEDD_SKILL_LEVEL( iSkillNo, i )) 
			);
			
			strTemp.append(" ");
			++iCount;

			iLearnSkillLv = g_pAVATAR->Skill_FindLearnedLevel( SKILL_NEED_SKILL_INDEX(iSkillNo,i) );
			if( iLearnSkillLv < SKILL_NEDD_SKILL_LEVEL( iSkillNo, i ) )
				dwColor = g_dwRED;
		}
	}
	if( iCount )
	{
		strTemp.append("]");	
		ToolTip.AddString( strTemp.c_str(), dwColor );
	}
}

void CIconSkill::AddSkillSuction( int iSkillNo, CInfo& ToolTip )
{
	if( SKILL_INCREASE_ABILITY( iSkillNo, 0 ) )
	{
		int iAbility = SKILL_INCREASE_ABILITY( iSkillNo, 0 );
		int iValue   = SKILL_INCREASE_ABILITY_VALUE( iSkillNo, 0 );

		ToolTip.AddString( CStr::Printf("%s:%s %d ",
							STR_ABSORPTION,
							CStringManager::GetSingleton().GetAbility( iAbility ), iValue ), 
			g_dwBlueToolTip );
	}
}

void CIconSkill::AddSkillRequireAbility( int iSkillNo, CInfo& ToolTip )
{
	std::string strTemp("[");
	strTemp.append( STR_REQUIRE_ABILITY );
	strTemp.append( ":");

	char* pszBuf;
	
	int iCount = 0;
	DWORD dwColor = g_dwRED;
	for( int i = 0; i < SKILL_NEED_ABILITY_TYPE_CNT; ++i )
	{
		if( SKILL_NEED_ABILITY_TYPE( iSkillNo, i) )
		{
			pszBuf = CStr::Printf( "%s%d ",
									CStringManager::GetSingleton().GetAbility( SKILL_NEED_ABILITY_TYPE( iSkillNo, i) ),
									SKILL_NEED_ABILITY_VALUE( iSkillNo, i) );
			strTemp.append( pszBuf );
			++iCount;
			if( g_pAVATAR->GetCur_AbilityValue(SKILL_NEED_ABILITY_TYPE( iSkillNo, i)) >= SKILL_NEED_ABILITY_VALUE( iSkillNo, i) )
				dwColor = g_dwGREEN;
		}
	}
	if( iCount )
	{
		strTemp.append("]");
		ToolTip.AddString( strTemp.c_str(), dwColor );
	}
}
bool CIconSkill::GetSkillIncreaseAbility( int iSkillNo, int iColumn, std::string& strOut ,bool bAddTypeName )
{
	if( iColumn >= SKILL_INCREASE_ABILITY_CNT )
		return false;

	if( SKILL_INCREASE_ABILITY( iSkillNo, iColumn ) == 0 )
		return false;

	strOut.erase( strOut.begin(), strOut.end());

	strOut.append("[");

	if( bAddTypeName )
	{
		strOut.append( CStringManager::GetSingleton().GetAbility( SKILL_INCREASE_ABILITY( iSkillNo, iColumn ) ) );
		strOut.append( " " );
	}


	if( SKILL_INCREASE_ABILITY_VALUE( iSkillNo, iColumn ) )
	{
		int iIncreaseAbilityValue = 0;
		switch( SKILL_TYPE( iSkillNo) )
		{
		case 8:
		case 9:
		case 10:
		case 11:
			iIncreaseAbilityValue = SKILL_INCREASE_ABILITY_VALUE( iSkillNo, iColumn ) * ( g_pAVATAR->Get_INT() + 300 ) / 315;
			break;
		default:
			iIncreaseAbilityValue = SKILL_INCREASE_ABILITY_VALUE( iSkillNo, iColumn );
			break;
		}
		

		if( SKILL_INCREASE_ABILITY( iSkillNo, iColumn ) == AT_PSV_SAVE_MP )
			strOut.append( CStr::Printf("%d%%", iIncreaseAbilityValue ) );
		else
			strOut.append( CStr::Printf("%d", iIncreaseAbilityValue ) );

		if( SKILL_CHANGE_ABILITY_RATE( iSkillNo, iColumn ) )
			strOut.append(" ");
	}

	
	if( SKILL_CHANGE_ABILITY_RATE( iSkillNo, iColumn ) )
		strOut.append( CStr::Printf("%d%%", SKILL_CHANGE_ABILITY_RATE( iSkillNo, iColumn )));

	strOut.append("]");
	return true;
}
void CIconSkill::AddSkillStatus( int iSkillNo, CInfo& ToolTip  )
{
	std::string strPassiveHeader( STR_CHANGE_ABILITY );
	strPassiveHeader.append(" : ");

	std::string strOtherHeader( STR_STATE );
	strOtherHeader.append(" : ");

	const char*	pszHeader = strOtherHeader.c_str();
	if( SKILL_TYPE(iSkillNo ) == SKILL_ACTION_PASSIVE )
		pszHeader = strPassiveHeader.c_str();


	char* pszBuf = NULL;
	bool	bAddString = false;
	std::string strTemp;
	std::string strOut;

	int iStateNo0 = SKILL_STATE_STB(iSkillNo ,0);
	int iStateNo1 = SKILL_STATE_STB(iSkillNo ,1);


	if( iStateNo0 )
	{

		if( STATE_TYPE( iStateNo0 )  == ING_DUMMY_DAMAGE )
		{
			//strTemp.append( STATE_NAME( iStateNo0 ) );
			//strTemp.append( CStr::Printf(" : %d%%",SKILL_POWER( iSkillNo ) ) );
			//ToolTip.AddString( strTemp.c_str() ,g_dwBlueToolTip );
		}
		else
		{
			strTemp = pszHeader;
			strTemp.append( STATE_NAME( iStateNo0 ) );
			if( GetSkillIncreaseAbility( iSkillNo, 0, strOut , false ) )
				strTemp.append( strOut.c_str() );
			ToolTip.AddString( strTemp.c_str() ,g_dwBlueToolTip );
		}		
	}
	else
	{
		strTemp = pszHeader;
		if( GetSkillIncreaseAbility( iSkillNo, 0, strOut , true ) )
		{
			strTemp.append( strOut.c_str() );
			ToolTip.AddString( strTemp.c_str() ,g_dwBlueToolTip );
		}
	}

	if( iStateNo1 )
	{
		if( STATE_TYPE( iStateNo1 ) == ING_DUMMY_DAMAGE )
		{
			strTemp.append( STATE_NAME( iStateNo1 ) );
			strTemp.append( CStr::Printf(" : %d%%",SKILL_POWER( iSkillNo ) ) );
		}
		else
		{
			strTemp = pszHeader;
			strTemp.append( STATE_NAME( iStateNo1 ) );
			if( GetSkillIncreaseAbility( iSkillNo, 1, strOut , false ) )
				strTemp.append( strOut.c_str() );
		}
		ToolTip.AddString( strTemp.c_str() ,g_dwBlueToolTip );
	}
	else
	{
		strTemp = pszHeader;
		if( GetSkillIncreaseAbility( iSkillNo, 1, strOut , true ) )
		{
			strTemp.append( strOut.c_str() );
			ToolTip.AddString( strTemp.c_str() ,g_dwBlueToolTip );
		}
	}



	//if( iStateNo0 && iStateNo1 )
	//{
	//	strTemp = pszHeader;
	//	strTemp.append( STATE_NAME( iStateNo0 ) );
	//	if( GetSkillIncreaseAbility( iSkillNo, 0, strOut , false ) )
	//		strTemp.append( strOut.c_str() );
	//	ToolTip.AddString( strTemp.c_str() ,g_dwBlueToolTip );

	//	strTemp = pszHeader;
	//	strTemp.append( STATE_NAME( iStateNo1 ) );
	//	if( GetSkillIncreaseAbility( iSkillNo, 1, strOut , false ) )
	//		strTemp.append( strOut.c_str() );

	//	ToolTip.AddString( strTemp.c_str() ,g_dwBlueToolTip );

	//}
	//else if( iStateNo0 )
	//{
	//	strTemp = pszHeader;
	//	strTemp.append( STATE_NAME( iStateNo0 ) );
	//	for( int i = 0; i < SKILL_INCREASE_ABILITY_CNT; ++i )
	//	{
	//		if( GetSkillIncreaseAbility( iSkillNo, i, strOut , false ) )
	//			strTemp.append( strOut.c_str() );
	//	}

	//	ToolTip.AddString( strTemp.c_str() , g_dwBlueToolTip );

	//}
	//else if( iStateNo1 )
	//{
	//	strTemp = pszHeader;
	//	strTemp.append( STATE_NAME( iStateNo1 ) );
	//	for( int i = 0; i < SKILL_INCREASE_ABILITY_CNT; ++i )
	//	{
	//		if( GetSkillIncreaseAbility( iSkillNo, i, strOut , false) )
	//			strTemp.append( strOut.c_str() );
	//	}
	//	ToolTip.AddString( strTemp.c_str() ,g_dwBlueToolTip );

	//}
	//else
	//{
	//	strTemp = pszHeader;
	//	bAddString = false;
	//	for( int i = 0; i < SKILL_INCREASE_ABILITY_CNT; ++i )
	//	{
	//		if( GetSkillIncreaseAbility( iSkillNo, i, strOut , true) )
	//		{
	//			bAddString = true;			
	//			strTemp.append( strOut.c_str() );
	//		}
	//	}

	//	if( bAddString )
	//		ToolTip.AddString( strTemp.c_str() , g_dwBlueToolTip );
	//}

	if( iStateNo0 || iStateNo1 )
		AddSkillSuccessRateDuration( iSkillNo ,ToolTip );

}

void CIconSkill::AddSkillSuccessRateDuration( int iSkillNo, CInfo& ToolTip )
{
	char* pszBuf;

	///80% ~ 100%
	if( SKILL_SUCCESS_RATIO( iSkillNo ) )
	{
		pszBuf = CStr::Printf( "%s:%d-%d%%  %s:%d%s",
			STR_SUCCESS_RATE, (int)(SKILL_SUCCESS_RATIO( iSkillNo ) * 0.8),(int)(SKILL_SUCCESS_RATIO( iSkillNo )),
			STR_CONTINUE_TIME,SKILL_DURATION( iSkillNo) ,STR_SECOND );
	}
	else///성공률이 0일경우 100%로 처리
	{
		pszBuf = CStr::Printf( "%s:%d%%  %s:%d%s",
			STR_SUCCESS_RATE, 100,
			STR_CONTINUE_TIME,SKILL_DURATION( iSkillNo) ,STR_SECOND );
	}


	ToolTip.AddString( pszBuf );
}

void CIconSkill::AddSkillRequireSkillPoint( int iSkillNo, CInfo& ToolTip )
{
	int iNeedPoint = GetNeedPoint4LevelUp( iSkillNo );

	char* pszBuf = CStr::Printf("[%s : %d]",STR_REQUIRE_SKILLPOINT, iNeedPoint);

	if( g_pAVATAR->GetCur_SkillPOINT() >= iNeedPoint )
		ToolTip.AddString( pszBuf, g_dwGREEN );
	else
		ToolTip.AddString( pszBuf, g_dwRED );
}

int CIconSkill::GetNeedPoint4LevelUp( int iSkillNo )
{
	
	int iNextLevelSkillIDX = iSkillNo + 1;

	if ( iNextLevelSkillIDX >= g_SkillList.Get_SkillCNT() ) {
		// 더이상 레벨업 할수 없다.
		return 0;
	}

	// 같은 종류의 스킬이고 배우려는 레벨이 현재 레벨의 다음 레벨인가 ??
	if ( SKILL_1LEV_INDEX( iSkillNo) != SKILL_1LEV_INDEX( iNextLevelSkillIDX ) ||
		 SKILL_LEVEL( iSkillNo )+1   != SKILL_LEVEL( iNextLevelSkillIDX ) ) {
		return 0;
	}


	//TODO:: 여기서 skill stb의 컬럼에 있는 값을 전송...
	return SKILL_NEED_LEVELUPPOINT( iNextLevelSkillIDX );
}

int CIconSkill::GetSkillLevel()
{
	CSkill* pSkill = GetSkill();
	if( pSkill )
		return pSkill->GetSkillLevel();

	return 0;
}

int CIconSkill::GetSkillIndex()
{
	CSkill* pSkill = GetSkill();
	if( pSkill )
		return pSkill->GetSkillIndex();

	return 0;
}

CSkill* CIconSkill::GetSkill()
{
	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
	return  pSkillSlot->GetSkill( m_iSkillSlot );
}

void CIconSkill::AddSkillRequireSkillPoint2Learn( int iSkillNo, CInfo& ToolTip )
{
	
	int iNeedPoint = SKILL_NEED_LEVELUPPOINT( iSkillNo );
	if( iNeedPoint && SKILL_TAB_TYPE(iSkillNo) != 3 )///필요포인트가 있고, 클랜스킬이 아닐경우에만( 클랜스킬일경우 다른 데이타가 들어간다 )
	{
		char* pszBuf = CStr::Printf("[%s : %d]",STR_REQUIRE_SKILLPOINT, iNeedPoint);

		if( g_pAVATAR->GetCur_SkillPOINT() >= iNeedPoint )
			ToolTip.AddString( pszBuf, g_dwGREEN );
		else
			ToolTip.AddString( pszBuf, g_dwRED );
	}
}

int CIconSkill::GetIndex()
{
	return GetSkillSlotFromIcon();
}