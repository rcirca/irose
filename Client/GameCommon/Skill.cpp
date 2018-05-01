/*
	$Header: /Client/GameCommon/Skill.cpp 71    05-10-26 2:56p Gioend $
*/
#include "stdAFX.h"
#include "Skill.h"
#include "JCommandState.h"	
#include "Network\CNetWork.h"	
#include "../Interface/SlotContainer/CSkillSlot.h"	
#include "../Interface/it_Mgr.h"	
#include "../Interface/IO_ImageRes.h"
#include "../Interface/Dlgs/ChattingDlg.h"
#include "../Interface/Dlgs/CPartyDlg.h"
#include "../Interface/CTDrawImpl.h"
#include "CSkillCommand.h"
#include "ReloadProcess.h"
#include "GameData/CParty.h"
#include "IO_Terrain.h"
#include "GameProc/SkillCommandDelay.h"

#include "../Country.h"
#include "../GameProc/UseItemDelay.h"

CSkillManager	g_SkillManager;




CSkill::CSkill()
{
	m_iSkillLevel		= 0;
	m_iSkillDelayTimer	= 0;

	m_iSkillSlot		= 0;
	m_pCommand			= NULL;

	m_iElapsedTime		= 0;

	m_dwExpiredTime		= 0;

	m_bEnable			= true;
}

CSkill::~CSkill()
{
	// Destruct
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  스킬 레벨업시에는 UserData 의 Skill 정보만 업데이트 하기때문에
///			SkillSlot 의 스킬 정보들도 내부에 보관하지 않고 직접 UserData 의 Skill 정보를 참조한다.
//----------------------------------------------------------------------------------------------------

short CSkill::GetSkillIndex()
{
	if( m_iSkillSlot >= MAX_LEARNED_SKILL_CNT 
		|| m_iSkillSlot < 0  )
		return 0;

	short nSkillIDX = 0;
	
	switch( m_iSkillSlotType )
	{
	case SKILL_SLOT_MAIN:
		nSkillIDX = g_pAVATAR->m_Skills.m_nSkillINDEX[ m_iSkillSlot ];
		break;
	case SKILL_SLOT_CLAN:
		nSkillIDX = m_nClanSkillIndex;
		break;
	default:
		break;
	}	

	return nSkillIDX;
}
//----------------------------------------------------------------------------------------------------
/// @modify 직접 STB에서 가져오는것으로 수정 nAvy 2004/6/30;
//----------------------------------------------------------------------------------------------------
short CSkill::GetSkillLevel()
{ 
	short nSkillIDX = GetSkillIndex();
	assert( nSkillIDX );
	return SKILL_LEVEL( nSkillIDX );
};

bool CSkill::Create( int iSkillType, short nSkillIdx, short nSkillType, short nSkillLevel )
{	
	m_iSkillSlotType	= iSkillType;

	m_nClanSkillIndex		= nSkillIdx;
	m_iSkillLevel		= nSkillLevel;
	m_iSkillDelayTimer	= 0;
	
	
	m_btSkillActionType = SKILL_TYPE( nSkillIdx );
	m_btSkillFilterType = SKILL_CLASS_FILTER( nSkillIdx );

	
	switch( m_btSkillActionType )
	{
		/// 기본명령( 앉기, 서기, 감정표현등 )
		case SKILL_BASE_ACTION:
			{
				m_pCommand = (CCommand*) g_QuickCommandPool[ COMMAND_COMMAND ];
			}
			break;
		case SKILL_EMOTION_ACTION:
			{
				m_pCommand = (CCommand*) g_QuickCommandPool[ EMOTION_COMMAND ];
			}
			break;		
		default:
			{				
				m_pCommand = (CCommand*) g_QuickCommandPool[ SKILL_COMMAND ];
			}
			break;
	}

	return true;
}

bool CSkill::Execute()
{
	if( m_pCommand == NULL )
		return false;

	m_pCommand->SetMember( m_iSkillSlot );
	return m_pCommand->Execute();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 주 용도는 스킬 딜레이 타임의 진행이다.
//----------------------------------------------------------------------------------------------------

int CSkill::Process()
{	
	if( this->m_iSkillDelayTimer > 0 )
	{
		this->m_iSkillDelayTimer -= g_GameDATA.GetElapsedFrameTime();

		if( this->m_iSkillDelayTimer < 0 )
			this->m_iSkillDelayTimer = 0;
	}

	return 1;
}


void CSkill::SetSkillDelayTime( int iTime )
{ 
	m_iSkillDelayTimer = iTime; 
	m_iElapsedTime = 0;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief draw icon..
//----------------------------------------------------------------------------------------------------
void CSkill::DrawIcon( int iX, int iY )
{
	
 	CTDrawImpl* pDrawObj = g_itMGR.GetDrawImplPtr();
	short nIconNo = SKILL_ICON_NO( GetSkillIndex() );

//-----------------------------------------------------------------------------------------------------
/////////2005. 5. 30.  박 지호 
	if(CCountry::GetSingleton().IsUseItemDelayNewVersion())
	{
		//스킬 딜레이 타입을 가져온다. 
		int iDelayType = SKILL_RELOAD_TYPE( GetSkillIndex() );
			
		float srcTick = 0.0f;
		float desTick = 0.0f;

		//그룹타입이라면 처음 선택 되어진 타입의 딜레이 틱을 가져온다. 
		if(iDelayType)
		{
		   srcTick = g_CurSkillDelayTick.GetUseItemDelay(iDelayType);
		   desTick = g_UseSkillDelay.GetUseItemDelay( iDelayType );
		}
		//단독으로 구동되는 스킬이라면 자신의 딜레이 틱을 설정한다. 
		else
		{
			srcTick = g_SkillList.GetDelayTickCount( GetSkillIndex() );
		   desTick = g_SoloSkillDelayTick.GetUseItemDelay( GetSkillIndex() );
		}

		//만약 딜레이 랜더링 중이라면...
		if(desTick > 0.0f)	
		{
			float fProcessRate =  (desTick / srcTick);
				
			CReloadProcess* pReloadProcess = g_itMGR.GetReloadProcess();
			pReloadProcess->Draw( iX , iY, IMAGE_RES_SKILL_ICON, nIconNo, fProcessRate );	
		}
		else
		{
			if( IsEnable() )		
				pDrawObj->Draw( iX,	iY,	IMAGE_RES_SKILL_ICON, nIconNo ); 
			else
				pDrawObj->Draw( iX,	iY,	IMAGE_RES_SKILL_ICON, nIconNo ,D3DCOLOR_RGBA( 128, 128, 128, 255 )); 
		}
	}
//-------------------------------------------------------------------------------------------------------
	else
	{
		if( this->m_iSkillDelayTimer > 0 )
		{
			float fProcessRate = (float)m_iSkillDelayTimer / (float)( SKILL_RELOAD_TIME( this->GetSkillIndex() ) * 200 );

			CReloadProcess* pReloadProcess = g_itMGR.GetReloadProcess();
			pReloadProcess->Draw( iX , iY, IMAGE_RES_SKILL_ICON, nIconNo, fProcessRate );		
		}else
		{	pDrawObj->Draw( iX,	iY,	IMAGE_RES_SKILL_ICON, nIconNo ); 	}

		CSkillCommandDelay::GetSingleton().Draw( iX, iY );
	}
	
	
}	


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief get/set Skill slot
//----------------------------------------------------------------------------------------------------
void CSkill::SetSkillSlot( int iSlot )
{ 
	m_iSkillSlot = iSlot; 

	/// 모든 Skill 객체가 컴맨드 객체를 가지는것이 아니라 공유한다.. 매 호출시마다 맴버 세팅해라..
	////if( m_pCommand )
	////	m_pCommand->SetMember( iSlot );
	////else
	////	assert( 0 && "CSkill has null pointer to command" );
}

bool CSkill::HasExpiredTime()
{
	short nSkillIndex = GetSkillIndex();
	if( SKILL_TAB_TYPE( nSkillIndex ) == 3 && SKILL_NEED_LEVELUPPOINT( nSkillIndex ) >= 1 )
		return true;

	return false;
}

//bool CSkill::HasLimitUseCount()
//{
//}
//
//bool CSkill::HasLimitClass()
//{
//}
DWORD CSkill::GetExpiredTime()
{
	return m_dwExpiredTime;
}

void CSkill::SetExpiredTime( DWORD dwAbsSec )
{
	m_dwExpiredTime   = dwAbsSec;
}
//int CSkill::GetLimitUseCount()
//{
//}
//
//int CSkill::GetLimitClass()
//{
//}
void CSkill::SetEnable()
{
	m_bEnable = true;
}
void CSkill::SetDisable()
{
	m_bEnable = false;
}
bool CSkill::IsEnable()
{
	return m_bEnable;
}


//////////////////////////////////////////////////////////////////////////////////////
/// Base action Skill
//////////////////////////////////////////////////////////////////////////////////////

CBaseActionSkill::CBaseActionSkill()
{
	// Construct
}

CBaseActionSkill::~CBaseActionSkill()
{
	// Destruct
}

/*
*	Skill Action interface
*/
void CBaseActionSkill::Action( CObjCHAR* pAttacker, CObjCHAR* pTarget )
{
	
}

void CBaseActionSkill::DrawIcon( int iX, int iY )
{
	CSkill::DrawIcon( iX, iY );
}

int CBaseActionSkill::Process()
{
	return CSkill::Process();
}

int CBaseActionSkill::SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO )
{
	return 0;
}











//////////////////////////////////////////////////////////////////////////////////////
/// Action create window Skill
//////////////////////////////////////////////////////////////////////////////////////

CCreateWindowActionSkill::CCreateWindowActionSkill()
{
	// Construct
}

CCreateWindowActionSkill::~CCreateWindowActionSkill()
{
	// Destruct
}

/*
*	Skill Action interface
*/
void CCreateWindowActionSkill::Action( CObjCHAR* pAttacker, CObjCHAR* pTarget )
{

}

void CCreateWindowActionSkill::DrawIcon( int iX, int iY )
{
	CSkill::DrawIcon( iX, iY );
}

int CCreateWindowActionSkill::Process()
{
	return CSkill::Process();
}

int CCreateWindowActionSkill::SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO )
{
	//g_itMGR.OpenMakeDlg( iSkillSlot );


	int iWindowType = SKILL_POWER(GetSkillIndex() );
	if( iWindowType >= 11 && iWindowType <= 39 )
		g_itMGR.OpenDialogBySkill( iSkillSlot, DLG_TYPE_MAKE );
	else if( iWindowType == 41 )
		g_itMGR.OpenDialogBySkill( iSkillSlot, DLG_TYPE_SEPARATE );
	else if( iWindowType == 42 )
		g_itMGR.OpenDialogBySkill( iSkillSlot, DLG_TYPE_UPGRADE );

	return 0;
}











//////////////////////////////////////////////////////////////////////////////////////
/// CImmediateActionSkill Skill
//////////////////////////////////////////////////////////////////////////////////////

CImmediateActionSkill::CImmediateActionSkill()
{
	// Construct
}

CImmediateActionSkill::~CImmediateActionSkill()
{
	// Destruct
}

/*
*	Skill Action interface
*/
void CImmediateActionSkill::Action( CObjCHAR* pAttacker, CObjCHAR* pTarget )
{

}

void CImmediateActionSkill::DrawIcon( int iX, int iY )
{
	CSkill::DrawIcon( iX, iY );
}

int CImmediateActionSkill::Process()
{

	return CSkill::Process();
}

int CImmediateActionSkill::SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO )
{
	g_pNet->Send_cli_TARGET_SKILL( iTarget, iSkillSlot );
	return 0;
}









//////////////////////////////////////////////////////////////////////////////////////
/// Fire Skill
//////////////////////////////////////////////////////////////////////////////////////

CFireSkill::CFireSkill()
{
	// Construct
}

CFireSkill::~CFireSkill()
{
	// Destruct
}

/*
*	Skill Action interface
*/
void CFireSkill::Action( CObjCHAR* pAttacker, CObjCHAR* pTarget )
{
	int iBulletIDX = SKILL_BULLET_NO( GetSkillIndex() );
	if ( pTarget && iBulletIDX ) 
	{
		g_pBltMGR->Add_BULLET( pAttacker, pTarget, iBulletIDX );

		/// 타격사운드 출력

	}
}

void CFireSkill::DrawIcon( int iX, int iY )
{
	CSkill::DrawIcon( iX, iY );
}

int CFireSkill::Process()
{
	return CSkill::Process();
}

int CFireSkill::SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO )
{
	g_pNet->Send_cli_TARGET_SKILL( iTarget, iSkillSlot );

	return 0;
}



//////////////////////////////////////////////////////////////////////////////////////
/// Fire to position Skill
//////////////////////////////////////////////////////////////////////////////////////

CFireToPositionSkill::CFireToPositionSkill()
{
	// Construct
}

CFireToPositionSkill::~CFireToPositionSkill()
{
	// Destruct
}

/*
*	Skill Action interface
*/
void CFireToPositionSkill::Action( CObjCHAR* pAttacker, CObjCHAR* pTarget )
{
	int iBulletIDX = SKILL_BULLET_NO( GetSkillIndex() );
	if ( pTarget && iBulletIDX ) 
	{
		g_pBltMGR->Add_BULLET( pAttacker, pTarget, iBulletIDX );

		/// 타격사운드 출력

	}
}

void CFireToPositionSkill::DrawIcon( int iX, int iY )
{
	CSkill::DrawIcon( iX, iY );
}

int CFireToPositionSkill::Process()
{
	return CSkill::Process();
}

int CFireToPositionSkill::SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO )
{
	g_pNet->Send_cli_POSITION_SKILL( PosTO, iSkillSlot );

	return 0;
}









//////////////////////////////////////////////////////////////////////////////////////
/// Self Bound Skill
//////////////////////////////////////////////////////////////////////////////////////

CSelfBoundSkill::CSelfBoundSkill()
{
	// Construct
}

CSelfBoundSkill::~CSelfBoundSkill()
{
	// Destruct
}

/*
*	Skill Action interface
*/
void CSelfBoundSkill::Action( CObjCHAR* pAttacker, CObjCHAR* pTarget )
{
}

void CSelfBoundSkill::DrawIcon( int iX, int iY )
{
	CSkill::DrawIcon( iX, iY );
}

int CSelfBoundSkill::Process()
{
	return CSkill::Process();
}

int CSelfBoundSkill::SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO )
{
	g_pNet->Send_cli_SELF_SKILL ( iSkillSlot );
	return 0;
}




//////////////////////////////////////////////////////////////////////////////////////
/// Target Bound Skill
//////////////////////////////////////////////////////////////////////////////////////

CTargetBoundSkill::CTargetBoundSkill()
{
	// Construct
}

CTargetBoundSkill::~CTargetBoundSkill()
{
	// Destruct
}

/*
*	Skill Action interface
*/
void CTargetBoundSkill::Action( CObjCHAR* pAttacker, CObjCHAR* pTarget )
{
}

void CTargetBoundSkill::DrawIcon( int iX, int iY )
{
	CSkill::DrawIcon( iX, iY );
}

int CTargetBoundSkill::Process()
{
	return CSkill::Process();
}

int CTargetBoundSkill::SendSkillActionReq( int iSkillSlot, int iTarget, D3DXVECTOR3 &PosTO )
{
	g_pNet->Send_cli_TARGET_SKILL ( iTarget, iSkillSlot );
	return 0;
}












//////////////////////////////////////////////////////////////////////////////////////
/// Skill Manager
//////////////////////////////////////////////////////////////////////////////////////

CSkillManager::CSkillManager()
{
	// Construct


	g_QuickCommandPool[ COMMAND_COMMAND ] = (CCommand*)new CBasicCommand();
	g_QuickCommandPool[ EMOTION_COMMAND ] = (CCommand*)new CEmotionCommand();
	g_QuickCommandPool[ SKILL_COMMAND ] = (CCommand*)new CSkillCommand();	

}

CSkillManager::~CSkillManager()
{
	// Destruct

	for( int i = 0; i < MAX_QUICK_COMMAND; i++ )
	{
		if( g_QuickCommandPool[ i ] != NULL )
		{
			delete g_QuickCommandPool[ i ];
			g_QuickCommandPool[ i ] = NULL;
		}
	}
}



CSkill* CSkillManager::CreateNewSkill( int iSkillSlotType, short nSkillIdx, char cSkillLevel )
{	
	CSkill* pSkill = NULL;
	int iSkillType = SKILL_TYPE( nSkillIdx );

	/// 후에 적당한 팩토리로 교체하자..
	switch( iSkillType )
	{
		case SKILL_EMOTION_ACTION:
		case SKILL_BASE_ACTION:					///< 기본 명령( 앉기, 서기, 줍기... )
			{
				pSkill = new CBaseActionSkill();
				pSkill->Create( iSkillSlotType, nSkillIdx, iSkillType, cSkillLevel );
			}
			break;		
		case SKILL_CREATE_WINDOW:
			{
				pSkill = new CCreateWindowActionSkill();
				pSkill->Create( iSkillSlotType, nSkillIdx, iSkillType, cSkillLevel );
			}
			break;


		case SKILL_ACTION_IMMEDIATE:			///< 근접 즉시 발동.( 즉시 모션 교체 )
			{
				pSkill = new CImmediateActionSkill();
				pSkill->Create( iSkillSlotType, nSkillIdx, iSkillType, cSkillLevel );
			}
			break;

		case SKILL_ACTION_ENFORCE_BULLET:
		case SKILL_ACTION_FIRE_BULLET:			///< 발사.
			{
				pSkill = new CFireSkill();
				pSkill->Create( iSkillSlotType, nSkillIdx, iSkillType, cSkillLevel );
			}
			break;

		case SKILL_ACTION_AREA_TARGET:
			{
				pSkill = new CFireToPositionSkill();
				pSkill->Create( iSkillSlotType, nSkillIdx, iSkillType, cSkillLevel );
			}
			break;

		/// Self bound
		case SKILL_ACTION_SELF_BOUND_DURATION:	///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 능력치 
		case SKILL_ACTION_SELF_BOUND:			///< 자신에게 발동.( 캐스팅 유 )
		case SKILL_ACTION_SELF_STATE_DURATION:	///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
		case SKILL_ACTION_SELF_DAMAGE:
		case SKILL_ACTION_SUMMON_PET:
			{
				pSkill = new CSelfBoundSkill();
				pSkill->Create( iSkillSlotType, nSkillIdx, iSkillType, cSkillLevel );
			}
			break;

		/// Target bound
		case SKILL_ACTION_TARGET_BOUND_DURATION:	///< 상대에게 발동 지속 마법.( 캐스팅 유 ) 능력치 
		case SKILL_ACTION_TARGET_BOUND:				///< 상대에게 발동.( 캐스팅 유 )
		case SKILL_ACTION_TARGET_STATE_DURATION:	///< 상대에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
		case SKILL_ACTION_SELF_AND_TARGET:			///< 타겟에게 영향을 주는동시에 나에게 영향을 줌
		case SKILL_ACTION_RESURRECTION:
			{
				pSkill = new CTargetBoundSkill();
				pSkill->Create( iSkillSlotType, nSkillIdx, iSkillType, cSkillLevel );
			}
			break;
		
		case SKILL_ACTION_PASSIVE:	
			{
				pSkill = new CBaseActionSkill();
				pSkill->Create( iSkillSlotType, nSkillIdx, iSkillType, cSkillLevel );
			}			
			break;
	}

	return pSkill;
}


/// 스킬의 액션 타입에 따른 타겟 타입을 구한다.
/*static*/ int	CSkillManager::GetSkillTargetType( int iSkillIndex )
{
	switch( SKILL_TYPE( iSkillIndex ) )
	{
		case SKILL_BASE_ACTION:						///< 기본 명령( 앉기, 서기, 줍기... ), 감정표현		
		case SKILL_CREATE_WINDOW:					///< 창생성( 제조, 파티, 개인상점... )	
		case SKILL_ACTION_SELF_BOUND_DURATION:		///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 능력치 
		case SKILL_ACTION_SELF_BOUND:				///< 자신에게 발동 바로 업 마법.( 캐스팅 유 ) 능력치
		case SKILL_ACTION_SELF_STATE_DURATION:		///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
		case SKILL_ACTION_SUMMON_PET:				///< 팻 소환 스킬
		case SKILL_ACTION_PASSIVE:					///< 패시브..
		case SKILL_ACTION_SELF_DAMAGE:				///< 자신주위로 데미지를 주는 스킬..( 이건 셀프로 안되나? ) 물론 범위..
		
			/// 타겟이 필요없는 것들이다.
			return SKILL_TARGET_NONE;

		case SKILL_ACTION_ENFORCE_WEAPON:			///< 무기상태 변경( 강화, 효과 연출( 정령탄? ) )
			/// 이건좀 모호하군..
			return SKILL_TARGET_OBJECT;

		case SKILL_ACTION_IMMEDIATE:				///< 근접 즉시 발동.( 즉시 모션 교체 )				
			return SKILL_TARGET_OBJECT;

		case SKILL_ACTION_ENFORCE_BULLET:			///< 강화총알 변경 발사. ( 아이스 애로우.. 실제 화살이 변하는.. )		
		case SKILL_ACTION_FIRE_BULLET:				///< 발사.( 파이어볼 )		
		case SKILL_ACTION_TARGET_BOUND_DURATION:	///< 타겟에게 발동 지속 마법.( 캐스팅 유 ) 능력치
		case SKILL_ACTION_TARGET_BOUND:				///< 타겟에게 발동 바로 업 마법.( 캐스팅 유 ) 능력치
		case SKILL_ACTION_TARGET_STATE_DURATION:	///< 자신에게 발동 지속 마법.( 캐스팅 유 ) 상태관련
		case SKILL_ACTION_RESURRECTION:
			return SKILL_TARGET_OBJECT;
		
		case SKILL_ACTION_AREA_TARGET:				///< 지역 공격마법( 당근 범위.. )!!! 허나 지금은 UI문제로 타겟 오브젝트의 위치로 시전..
			return SKILL_TARGET_OBJECT;
	}

	return SKILL_TARGET_NONE;
}

///
/// @todo 적절한 메세지를 서버에 날려라.. <= 현재는 서버에서 명령을 하달받음.. 그러니 날릴 필요없지..
/// 현재 실제 액션은 각 스킬 클래스 자체에서..
bool CSkillManager::ActionSkill( short nSkillIdx, CObjCHAR* pSrc, CObjCHAR* pTarget, D3DXVECTOR3 PosTo, bool bTargetPos )
{
	/// 행위 방식에 따라서...
	int iSkillActionType = GetSkillActionType( nSkillIdx );

	switch( iSkillActionType )
	{
		case SKILL_BASE_ACTION:					///< 기본 명령( 앉기, 서기, 줍기... )
			break;

		case SKILL_CREATE_WINDOW:			
			break;

		case SKILL_ACTION_IMMEDIATE:
			break;

		case SKILL_ACTION_FIRE_BULLET:
			{
				int iBulletIDX = SKILL_BULLET_NO( nSkillIdx );
				if ( pTarget && iBulletIDX ) 
				{
					CBullet* pBullet = g_pBltMGR->Add_BULLET( pSrc, pTarget, iBulletIDX, true, SKILL_BULLET_LINKED_POINT( nSkillIdx ) );
					if( pBullet )
						pBullet->SetSkillIDX( nSkillIdx );
					
					/// 타격사운드 출력

				}
			}
			break;

		/// 지역공격 마법
		case SKILL_ACTION_AREA_TARGET:
			{
				if( bTargetPos )
				{
					int iBulletIDX = SKILL_BULLET_NO( nSkillIdx );
					if ( iBulletIDX ) 
					{
						short nEffectFileIDX = EFFECT_BULLET_NORMAL( iBulletIDX );
						/// 지역공격마법은 총알을 쏘는게 아니라 타겟에 이펙트를 붙인다.
						CEffect *pHitEFT = g_pEffectLIST->Add_EffectWithIDX( nEffectFileIDX, true );
						if ( pHitEFT ) 
						{	
							/*if( pTarget )
							{
								pHitEFT->LinkNODE ( pTarget->GetZMODEL() );
								pHitEFT->UnlinkVisibleWorld ();
							}else*/
							{
								pHitEFT->SetPOSITION( PosTo );
							}
							
							pHitEFT->InsertToScene ();
						}
					}
				}
			}
			break;

		///< 자신에게 발동.( 캐스팅 유 )
		case SKILL_ACTION_SELF_BOUND_DURATION:
		case SKILL_ACTION_SELF_BOUND:
		case SKILL_ACTION_SELF_STATE_DURATION:
		case SKILL_ACTION_SELF_DAMAGE:
			{
				/// Self 스킬의 경우에는 총알효과, 타격효과를 모두 나한테 붙여준다.

				/// 총알 효과..
				int iShotEffectIDX = SKILL_BULLET_NO( nSkillIdx );
				int iEffectIDX = EFFECT_BULLET_NORMAL( iShotEffectIDX );

				CEffect *pEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );				
				if( pEffect )
				{
					pSrc->LinkDummy( pEffect->GetZNODE(), SKILL_BULLET_LINKED_POINT( nSkillIdx) );

					pEffect->SetParentCHAR( pSrc );
					pSrc->AddExternalEffect( pEffect );

					pEffect->InsertToScene ();
				}

				/// 타격 효과..				
				int iHitEffectIDX = SKILL_HIT_EFFECT( nSkillIdx );

				pEffect = g_pEffectLIST->Add_EffectWithIDX( iHitEffectIDX, true );				
				if( pEffect )
				{
					pSrc->LinkDummy( pEffect->GetZNODE(), SKILL_HIT_EFFECT_LINKED_POINT( nSkillIdx) );

					pEffect->SetParentCHAR( pSrc );
					pSrc->AddExternalEffect( pEffect );

					pEffect->InsertToScene ();
				}
			}
			break;	

			///< 자신에게 발동.( 캐스팅 유 )
		case SKILL_ACTION_TARGET_BOUND_DURATION:
		case SKILL_ACTION_TARGET_BOUND:
		case SKILL_ACTION_TARGET_STATE_DURATION:
		case SKILL_ACTION_RESURRECTION:
			{
				int iBulletIDX = SKILL_BULLET_NO( nSkillIdx );
				if ( pTarget && iBulletIDX ) 
				{					
					CBullet* pBullet = g_pBltMGR->Add_BULLET( pSrc, pTarget, iBulletIDX, true, SKILL_BULLET_LINKED_POINT( nSkillIdx ), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), false, true, nSkillIdx );
					if( pBullet )
						pBullet->SetSkillIDX( nSkillIdx );
					
					/// 타격사운드 출력

				}

				/*int iShotEffectIDX = SKILL_BULLET_NO( nSkillIdx );
				int iEffectIDX = EFFECT_HITTED_NORMAL( iShotEffectIDX );

				CEffect *pEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectIDX, true );
				if( pEffect )
				{
					pSrc->LinkDummy( pEffect->GetZNODE(), SKILL_BULLET_LINKED_POINT( nSkillIdx) );

					pEffect->SetParentCHAR( pSrc );
					pSrc->AddExternalEffect( pEffect );

					pEffect->InsertToScene ();
				}*/
			}
			break;
	}

	return true;
}

/// 스킬 발동 조건 체크..
bool CSkillManager::CheckConditionForFireSkill( int iSkillIDX, int iCurrentTarget )
{
	/// 스킬 사용 조건( 타겟팅 ) 체크..
	if( CSkillManager::CheckCastingTarget( iSkillIDX, g_pAVATAR, iCurrentTarget ) == false )
	{
		g_itMGR.AppendChatMsg( CStr::Printf("%s[ %d ]", STR_NOTIFY_05, CSkillManager::GetSkillTargetType( iSkillIDX ) ), IT_MGR::CHAT_TYPE_SYSTEM );
		return false;
	}

	/// 타겟 필터링 검사	
	if( CSkillManager::CheckCastingTargetFilter( iSkillIDX, g_pAVATAR, iCurrentTarget ) == false )
	{
		g_itMGR.AppendChatMsg( CStr::Printf("%s[ %d ]", STR_NOTIFY_05, CSkillManager::GetSkillTargetType( iSkillIDX ) ), IT_MGR::CHAT_TYPE_SYSTEM );
		return false;
	}

	/// 소환몹 조건 체크
	if( CSkillManager::CheckSummonCapacity( iSkillIDX, g_pAVATAR, iCurrentTarget ) == false )
	{	
		g_itMGR.AppendChatMsg( STR_CANT_SUMMON_NPC, IT_MGR::CHAT_TYPE_SYSTEM);
		return false;	
	}


	/// 스킬 사용 조건 체크..
	if( CSkillManager::CheckCastingCondition( iSkillIDX, g_pAVATAR, iCurrentTarget ) == false )
	{
		g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_NOTIFY_06 ),IT_MGR::CHAT_TYPE_SYSTEM);
		return false;
	}

	
	/// 중복 스킬 적용 여부 체크..
	if( CSkillManager::CheckDuplicatable( iSkillIDX, g_pAVATAR, iCurrentTarget ) == false )
	{
		g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_NOTIFY_CANT_DUPLICATE ), IT_MGR::CHAT_TYPE_SYSTEM);
		return false;
	}
	
	return true;
}



///----------------------------------------------------------------------------------------------------
/// @param
/// @brief 스킬 발동.. CSkill::Exec에서 호출한다. 실제 스킬 발동을 서버에 요청하고 발동전 조건 체크등을 한다.
///----------------------------------------------------------------------------------------------------
bool CSkillManager::FireSkill( int iSkillSlotIDX, int iTargetObj, D3DXVECTOR3 &PosTO )
{

	/// 현재 벙어리 상태이면 사용불가
	if( g_pAVATAR->m_EndurancePack.GetStateValue( ING_DUMB ) )
	{
		///AddMsgToChatWND( STR_DOING_SKILL_ACTION,  g_dwRED ,CChatDLG::CHAT_TYPE_SYSTEM);
		return false;
	}

	/// 현재 스킬 시도 중일때는 스킬 사용불가..	
	if( g_pAVATAR->CanApplyCommand() == false )
	{
		g_itMGR.AppendChatMsg( STR_DOING_SKILL_ACTION,  IT_MGR::CHAT_TYPE_SYSTEM);
		return false;
	}

	if( iSkillSlotIDX < 0 )
		return false;

	/// get skill.
	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
	CSkill* pSkill = pSkillSlot->GetSkill( iSkillSlotIDX );

	if( pSkill == NULL )
		return false;
	
	//----------------------------------------------------------------------------------------------------
	/// 현재 사용하려는 스킬이 요청된 스킬과 같은것이라면 무시..( 연타.. 방지 )
	//----------------------------------------------------------------------------------------------------	
	if( g_pAVATAR->m_nToDoSkillIDX == pSkill->GetSkillIndex() )
	{
		g_itMGR.AppendChatMsg( STR_DOING_SKILL_ACTION,  IT_MGR::CHAT_TYPE_SYSTEM);
	}

	//----------------------------------------------------------------------------------------------------
	/// PVP 가능존이라면 소환몹 소환금지
	/// 204/12/6   
	if( ZONE_PVP_STATE( g_pTerrain->GetZoneNO() ) == 2 )
	{
		/// 소환몹 번호가 세팅된 스킬은 모두 금지
		if( SKILL_SUMMON_PET( pSkill->GetSkillIndex() ) )
		{
			g_itMGR.AppendChatMsg( STR_CANT_SUMMON_IN_PVPZONE,  IT_MGR::CHAT_TYPE_SYSTEM);
			return false;
		}
	}
	//----------------------------------------------------------------------------------------------------

	int iSkillType = CSkillManager::GetSkillActionType( pSkill->GetSkillIndex() );
	//----------------------------------------------------------------------------------------------------
	/// 아지트라면 제한된 사용..
	/// 204/12/6   
	if( ZONE_PVP_STATE( g_pTerrain->GetZoneNO() ) == 11 )
	{
		if( iSkillType != 1 && iSkillType != 2 && iSkillType != 16 )
		{
			g_itMGR.AppendChatMsg( STR_CANT_SKILL_IN_AGIT,  IT_MGR::CHAT_TYPE_SYSTEM);
			return false;
		}
	}
	//----------------------------------------------------------------------------------------------------



	//----------------------------------------------------------------------------------------------------
	/// pet 탑승시에는..
	/// 드라이브 스킬(17번스킬, 팻 타기/내리기), 태우기(25번 스킬) 등의 팻관련 스킬 가능함
	//----------------------------------------------------------------------------------------------------		
	
	//-----------------------------------------------------------------------------------------------------
	///박지호::대만 버전이 아니라면...
#ifndef _GBC
		if( g_pAVATAR->GetPetMode() >= 0 )
		{		
			switch( iSkillType )
			{
				case SKILL_BASE_ACTION:
					{
						int iSkillIdx = pSkill->GetSkillIndex();
						if( iSkillIdx == 17 ||
							iSkillIdx == 25 )
							break;
					}
					return false;

				default:
					g_itMGR.AppendChatMsg( STR_CANT_USESKILL_USING_DRIVESKILL,  IT_MGR::CHAT_TYPE_SYSTEM);
					return false;				
			}
		}
#endif
	//-----------------------------------------------------------------------------------------------------


	if( CSkillManager::GetSkillTargetType( iSkillType ) != SKILL_TARGET_NONE )
	{
		if ( iTargetObj ) 
		{			
			if( g_pObjMGR->m_pOBJECTS[ iTargetObj ] == NULL )
			{
				assert( 0 && "Fire skill[ target is invalid ]" );
				return false;
			}

			switch ( g_pObjMGR->m_pOBJECTS[ iTargetObj ]->Get_TYPE() ) 
			{
			case OBJ_MOB :
				g_UserInputSystem.SetCurrentTarget( iTargetObj );			
				g_pAVATAR->m_iServerTarget = g_pObjMGR->Get_ServerObjectIndex( iTargetObj );
				break;
			case OBJ_AVATAR:
				{
					//if( g_GameDATA.m_iPvPState == PVP_CANT )
					//{
					//	/// 적대관계를 유발하는 스킬은 아바타에게 걸수 없다.
					//	if( SKILL_HARM( pSkill->GetSkillIndex() ) != 0 )
					//		return false;
					//}				
				}			
				break;
			}
		}
	}
	
	int iCurrentTarget = g_UserInputSystem.GetCurrentTarget();		
	
	//타켓에 대한 판별
	if( CSkillManager::CheckConditionForFireSkill( pSkill->GetSkillIndex(), iCurrentTarget ) == false )
			return false;


	//------------------------------------------------------------------------------------------------
//2005 5. 30 박 지호
	//스킬 딜레이 타입 체크 후 재설정 만약 딜레이 중이라면 리턴 
 	if(CCountry::GetSingleton().IsUseItemDelayNewVersion())
	{
		int iDelayType = SKILL_RELOAD_TYPE( pSkill->GetSkillIndex() );
		int iSkill	   = pSkill->GetSkillIndex();
		float iDelayTick = g_SkillList.GetDelayTickCount( iSkill );

		////0 이 아니면 딜레이를 설정한다. 
		if( iDelayType )
		{
			//딜레이 타입을 넣어서 같은 타입이 딜레이 중이라면 사용할수 없다는 
			//메세지를 채팅창에 출력한다. 
			if( g_UseSkillDelay.GetUseItemDelay( iDelayType ) > 0.0f)
			{
				///Use item delay 가 설정되어 있다면 패스..
				g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_NOTIFY_04 ), IT_MGR::CHAT_TYPE_SYSTEM );
				return false;
			}
		}
		//공백일때 단독으로 설정한다. 
		else
		{	
			//자기 자신의 딜렉 중이라면....
			if( g_SoloSkillDelayTick.GetUseItemDelay( iSkill ) > 0.0f)
			{	
				g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_NOTIFY_04 ), IT_MGR::CHAT_TYPE_SYSTEM );
				return false;
			}
		} 
		
	} 
//------------------------------------------------------------------------------------------------
	else
	{
		/// 스킬 타이머가 정지 했는가? (정지하지 않았다면 메세지 창에 출력)
		if( CSkillManager::CheckSkillTimer( iSkillSlotIDX ) == false )
		{
			g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_NOTIFY_04 ), IT_MGR::CHAT_TYPE_SYSTEM );
			return false;
		}
		//if( CSkillManager::CheckConditionForFireSkill( pSkill->GetSkillIndex(), iCurrentTarget ) == false )
		//	return false;

		// 일단 여기서 깍자..소모치
		// 여기서 깍으면 안된다.. 서버에서 거부할경우는 문제가 있다..
		//CSkillManager::UpdateUseProperty( pSkill->GetSkillIndex() );

		//-----------------------------------------------------------------------------------------------
		/// 각 스킬별 딜레이가 아니라 모든 스킬 명령들에 대한 딜레이가 적용된다.
		/// 여기서 현재 이전의 스킬 명령이후 정해진 딜레이가 지났는지 체크
		//-----------------------------------------------------------------------------------------------	
		if( CSkillCommandDelay::GetSingleton().CanCastSkill() == false )
		{
			g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_NOTIFY_04 ), IT_MGR::CHAT_TYPE_SYSTEM );
			return false;
		}else
		{
			/// 스킬 입력 딜레이가 없을때는 새로 세팅
			CSkillCommandDelay::GetSingleton().StartSkill();
		}
	}
 
	/// request skill action	
	pSkill->SendSkillActionReq( iSkillSlotIDX, iCurrentTarget, PosTO );
	

	return true;
}

/// 현재 활성화된 스킬 발동..
bool CSkillManager::ActionActiveSkill( int iTargetObj, D3DXVECTOR3 &PosTO )
{
	int iCurrentActiveSkillSlot = g_UserInputSystem.GetCurrentActiveSkillSlot();

	if( iCurrentActiveSkillSlot < 0 )
		return false;

	return FireSkill( iCurrentActiveSkillSlot, iTargetObj, PosTO );
}


/*static*/ void CSkillManager::UpdateUseProperty( CObjAI* pObjAI, int iSkillIdx )
{	
	/// 행위 방식에 따라서...
	int iSkillActionType = GetSkillActionType( iSkillIdx );

	/// 창생성 스킬의 경우는 마나를 안깍는다..
	if( iSkillActionType == SKILL_CREATE_WINDOW )
		return;


	/// 유져일경우는..
	if( pObjAI->IsA( OBJ_USER ) )
	{
		g_pAVATAR->Skill_UseAbilityValue( iSkillIdx );
		return;
	}


	short nCurValue = 0;
	for ( short nI=0; nI<SKILL_USE_PROPERTY_CNT; nI++) 
	{
		if ( 0 == SKILL_USE_PROPERTY( iSkillIdx, nI ) )
			break;

		//nCurValue = g_pAVATAR->Skill_GetAbilityValue( SKILL_USE_PROPERTY( iSkillIdx, nI ) );
		switch( SKILL_USE_PROPERTY( iSkillIdx, nI ) )
		{
			case AT_HP :
				pObjAI->Set_HP( pObjAI->Get_HP() - SKILL_USE_VALUE( iSkillIdx, nI ) );
				break;
			case AT_MP :
				pObjAI->Set_MP( pObjAI->Get_MP() - SKILL_USE_VALUE( iSkillIdx, nI ) );
				break;
		}
	}	
}


///
///	Skill condition check function ( static function )
///
/* static */ bool CSkillManager::CheckCastingCondition(int iSkillIDX,CObjCHAR* pCaster,int iTargetObjIDX)
{

	//박지호::스킬에 맞는 무기 체크 적용
	if(!g_pAVATAR->Skill_ActionCondition( iSkillIDX ))
		return FALSE;

	/// 착용 장비에 대한 체크( 스킬 사용가능한 장비를 착용하고 있는가?
	if(CSkillManager::CheckNeedWeapon( iSkillIDX, pCaster ) == false )
	{
		g_itMGR.AppendChatMsg( STR_WEAPON_MISMATCH, g_dwRED );
		return false;
	}

	/// 서버쪽에서 정의한 함수를 사용한다.
	//return g_pAVATAR->Skill_ActionCondition( iSkillIDX );

	/*
	/// 소모 마나, HP 체크..
	if( CSkillManager::CheckNeedProperty( iSkillIDX, pCaster ) == false )
	{
		g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_MANA, g_dwRED );
		return false;
	}

	/// 해당 스킬에 대한 타겟 유효성을 검정한다.
	if( CSkillManager::CheckCastingTarget( iSkillIDX, pCaster, iTargetObjIDX ) == false )
	{
		g_itMGR.AppendChatMsg( STR_INVALID_TARGET, g_dwRED );
		return false;
	}

	/// 착용 장비에 대한 체크( 스킬 사용가능한 장비를 착용하고 있는가?
	if( CSkillManager::CheckNeedWeapon( iSkillIDX, pCaster ) == false )
	{
		g_itMGR.AppendChatMsg( STR_WEAPON_MISMATCH, g_dwRED );
		return false;
	}
	*/

	return true;
}

/// 타이머 체크( 연속 사용여부관련 )..
/// 내껏만 체크하지 남의것 해줄일없지???
/*static*/ bool CSkillManager::CheckSkillTimer( int iActiveSkillSlot )
{
	CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();

	if( pSkillSlot->GetSkillDelayTime( iActiveSkillSlot ) != 0 )
		return false;

	return true;
}

/// @todo 타겟의 HP를 체크해야하는가?
/// 당연히 죽은놈한테 쏠수는 없으니 해야될듯
/*static*/ bool CSkillManager::CheckCastingTarget( int iSkillIDX, CObjCHAR* pCaster, int iTargetObjIDX )
{
	if( pCaster == NULL )
		return false;

	CObjCHAR *pTargetCHAR = g_pObjMGR->Get_CharOBJ( iTargetObjIDX, true );

	int iSkillTargetType = CSkillManager::GetSkillTargetType( iSkillIDX );

	switch( iSkillTargetType )
	{
	case SKILL_TARGET_NONE:			
		return true;

	case SKILL_TARGET_OBJECT:	
		{
			/// 스킬이 리절렉션이라면 HP체크없이 타겟을 얻어돈다.
			if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_RESURRECTION )
			{
				pTargetCHAR = g_pObjMGR->Get_CharOBJ( iTargetObjIDX, false );
			}

			if( pTargetCHAR == NULL )
				return false;
		}		
		return true;

	case SKILL_TARGET_GROUND:			
		return true;

	case SKILL_TARGET_PB:			
		return true;

	case SKILL_TARGET_SCREEN:			
		return true;

	case SKILL_TARGET_ZONE:			
		return true;

	case SKILL_TARGET_SERVER:						
		return true;

	default:
		break;
	}

	return false;
}

/// 타겟 필터링 검사
bool CSkillManager::CheckCastingTargetFilter( int iSkillIDX, CObjCHAR* pCaster, int iTargetObjIDX )
{
	if( pCaster == NULL )
		return false;

	int iSkillTargetType = CSkillManager::GetSkillTargetType( iSkillIDX );

	switch( iSkillTargetType )
	{
		case SKILL_TARGET_NONE:			
			return true;
	}


	CObjCHAR *pTargetCHAR = g_pObjMGR->Get_CharOBJ( iTargetObjIDX, false );

	int iTargetFilterType = SKILL_CLASS_FILTER( iSkillIDX );

	switch( iTargetFilterType )
	{
		case SKILL_TARGET_FILTER_SELF:
			return true;

		case SKILL_TARGET_FILTER_GROUP:
			{	
				/// 타겟이 있을경우.. 타겟이 우리편이 아니면.. false
				if( pTargetCHAR != NULL )
				{
					/// 파티원이 아니다.
					if( !CParty::GetInstance().IsPartyMember( g_pObjMGR->Get_ServerObjectIndex( iTargetObjIDX ) ))
					{				
						g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_SKILL_TARGET_FILTER_NOTIFY_01 ), IT_MGR::CHAT_TYPE_SYSTEM );
						return false;
					}
				}
			}
			break;
		case SKILL_TARGET_FILTER_GUILD:
			{
				//AddMsgToChatWND ( CStr::Printf("%s", STR_SKILL_TARGET_FILTER_NOTIFY_02, g_dwRED,CChatDLG::CHAT_TYPE_SYSTEM );
			}
			break;
		case SKILL_TARGET_FILTER_FRIEND_ALL:
			{
				/// 타겟이 있을경우 나랑 동맹이 아닐경우 false
				if( pTargetCHAR != NULL )
				{
					if( CUserInputState::IsEnemy( pTargetCHAR ) )
					{
						g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_SKILL_TARGET_FILTER_NOTIFY_03 ), IT_MGR::CHAT_TYPE_SYSTEM );
						return false;
					}
				}
			}
			break;
		case SKILL_TARGET_FILTER_MOB:
			{
				if( pTargetCHAR == NULL )
					return false;

				if( !pTargetCHAR->IsA( OBJ_MOB ) )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_SKILL_TARGET_FILTER_NOTIFY_04 ), IT_MGR::CHAT_TYPE_SYSTEM );
					return false;
				}
			}
			break;
		case SKILL_TARGET_FILTER_ENEMY_ALL:
			{
				if( pTargetCHAR == NULL )
					return false;

				if( CUserInputState::IsEnemy( pTargetCHAR ) == false )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_SKILL_TARGET_FILTER_NOTIFY_05 ), IT_MGR::CHAT_TYPE_SYSTEM );
					return false;
				}
			}
			break;
		case SKILL_TARGET_FILTER_ENEMY_PC:
			{
				if( pTargetCHAR == NULL )
					return false;

				/// 동맹이거나.. 아바타가 아닐경우는..
				if( CUserInputState::IsEnemy( pTargetCHAR ) == false || !pTargetCHAR->IsA( OBJ_AVATAR ) )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_SKILL_TARGET_FILTER_NOTIFY_05 ), IT_MGR::CHAT_TYPE_SYSTEM );
					return false;
				}
			}
			break;

		case SKILL_TARGET_FILTER_ALL_PC:
			{
				if( pTargetCHAR == NULL )
					return false;

				if( !pTargetCHAR->IsUSER( ) )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_SKILL_TARGET_FILTER_NOTIFY_06 ), IT_MGR::CHAT_TYPE_SYSTEM );
					return false;
				}
			}			
			break;
		case SKILL_TARGET_FILTER_ALL_CHAR:
			{
				if( pTargetCHAR == NULL )
					return false;

				if( !( pTargetCHAR->IsUSER() || pTargetCHAR->IsA( OBJ_MOB ) ) )
				{
					g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_SKILL_TARGET_FILTER_NOTIFY_07 ), IT_MGR::CHAT_TYPE_SYSTEM );
					return false;
				}
			}
			break;

		/// 죽은 유져일 경우는 아군만
		case SKILL_TARGET_FILTER_DEAD_USER:
			{
				/// 타겟이 있을경우 나랑 동맹이 아닐경우 false
				if( pTargetCHAR != NULL )
				{
					if( CUserInputState::IsEnemy( pTargetCHAR ) )
					{
						g_itMGR.AppendChatMsg( CStr::Printf("%s", STR_SKILL_TARGET_FILTER_NOTIFY_03 ), IT_MGR::CHAT_TYPE_SYSTEM );
						return false;
					}
				}
			}
			break;

		default:
			break;
	}

	return true;
}

/// 소환몹 capacity 검사
bool CSkillManager::CheckSummonCapacity( int iSkillIDX, CObjCHAR* pCaster, int iTargetObjIDX )
{
	if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SUMMON_PET )
	{
		int iMobNO = SKILL_SUMMON_PET( iSkillIDX );
		if( iMobNO )
		{
			/// 소환량 초과
			if( ( NPC_NEED_SUMMON_CNT( iMobNO ) + g_pAVATAR->GetCur_SummonUsedCapacity() ) > g_pAVATAR->GetCur_SummonMaxCapacity() )
				return false;
		}
	}

	return true;
}

/// 필요 능력치 체크..
bool CSkillManager::CheckNeedProperty( int iSkillIDX, CObjCHAR* pCaster )
{
	int iCurrentHP = pCaster->Get_HP();	
	int iCurrentMP = pCaster->Get_MP();

	for( int i = 0; i < SKILL_USE_PROPERTY_CNT; i++ )
	{
		switch( SKILL_USE_PROPERTY( iSkillIDX, i ) )
		{
			case AT_HP:
				{
					if( iCurrentHP < SKILL_USE_VALUE( iSkillIDX, i ) )
						return false;
				}
				break;

			case AT_MP:
				{
					if( iCurrentMP < SKILL_USE_VALUE( iSkillIDX, i ) )
						return false;
				}
				break;
		}		
	}

	return true;
}

/// 필요장비 체크..
/*static*/ bool CSkillManager::CheckNeedWeapon( int iSkillIDX, CObjCHAR* pCaster )
{
	bool bResult = true;

	int iCurrentRWeaponNO = pCaster->Get_RWeapon();	

	for( int i = 0; i < SKILL_NEED_WEAPON_CNT; i++ )
	{
		/// 조건 무기가 있다면 일단,false 로 만들고 만족하면 바로 리턴..
		if( SKILL_NEED_WEAPON( iSkillIDX, i ) )
		{
			bResult = false;
			if( SKILL_NEED_WEAPON( iSkillIDX, i ) == WEAPON_TYPE( iCurrentRWeaponNO ) )
				return true;
		}
	}

	return bResult;
}

/// 중복적용 가능 체크...
/*static*/ bool CSkillManager::CheckDuplicatable( int iSkillIDX, CObjCHAR* pCaster, int iTargetObjIDX )
{
	if( pCaster == NULL )
		return false;	

	/// 지속형 스킬의 경우.. 중복여부를 체크한다.
	if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND_DURATION ||
		SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_BOUND_DURATION ||
		SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_STATE_DURATION ||
		SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_TARGET_STATE_DURATION )
	{
		int		iStateSTB		= 0;
		int		iSkillState		= 0;
		DWORD	dwSkillStateFlag = 0;
		DWORD	dwState 		 = 0;

		int		iBoundStateSTB = 0;

		CObjCHAR *pTargetCHAR = NULL;
		
		/// 나한테 거는거라면..
		if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_BOUND_DURATION ||
			SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_SELF_STATE_DURATION )
		{
			pTargetCHAR = pCaster;	
		}else
		{
			pTargetCHAR = g_pObjMGR->Get_CharOBJ( iTargetObjIDX, false );	
		}

		if( pTargetCHAR == NULL )
			return false;

		for( int i = 0; i < 2; i++ )
		{			
			iStateSTB = SKILL_STATE_STB( iSkillIDX, i );
			if( iStateSTB == 0 )
				continue;

			iSkillState = STATE_TYPE( iStateSTB );
			dwSkillStateFlag = c_dwIngFLAG[ iSkillState ];

			/// 상태 해지 스킬은 중복상태를 체크하지 않음	
			if( ( iSkillState == ING_CLEAR_GOOD ) ||
				( iSkillState == ING_CLEAR_BAD ) ||
				( iSkillState == ING_CLEAR_ALL ) )
			{
				continue;
			}

			dwState = pTargetCHAR->m_EndurancePack.GetStateFlag();			

			/// 현재 걸려는 스킬타입이 이미 걸려있다.
			if( dwState & dwSkillStateFlag )
			{
				/// 중복불가..
				if( STATE_CAN_DUPLICATED( iStateSTB ) == 0 )
				{
					return false;
				}

				CEnduranceProperty* pEntity = pTargetCHAR->m_EndurancePack.GetEntityByStateType( iSkillState );

				/// 새로 발동한 스킬이 현재 걸린 스킬의 LIST_STATUS.STB 상의 번호가 작을때는 패스..
				if( pEntity && iStateSTB < pEntity->GetStateSTBNO() )
				{
					return false;
				}
			}
		}		
	}

	return true;
}


///
///	스킬 습득가능 여부 체크..
///

/// 유효한 직업인가?
/// 배울때 필요하다..
/*static*/ bool CSkillManager::CheckJobForStudy( int iSkillIDX )
{
	
	return false;
}

/// 스킬을 배우기 위한 선행 스킬이 만족하는가?
/*static*/ bool CSkillManager::CheckProSkillForStudy( int iSkillIDX )
{
	bool bResult = true;	

	/*
	for( int i = 0; i < SKILL_NEED_SKILL_CNT; i++ )
	{
		if( SKILL_NEED_SKILL_INDEX( iSkillIDX, i ) )
		{
			if( SKILL_NEED_SKILL_INDEX( iSkillIDX, i ) );
				
		}
	}
	*/

	return bResult;	
}

/// 스킬을 배우기 위한 기본 능력치를 만족하는가?
/*static*/ bool CSkillManager::CheckPropertyForStudy( int iSkillIDX )
{
	bool bResult = true;

	for( int i = 0; i < SKILL_NEED_ABILITY_TYPE_CNT; i++ )
	{
		if( SKILL_NEED_ABILITY_TYPE( iSkillIDX, i ) )
		{
			bResult = false;

			switch( SKILL_NEED_ABILITY_TYPE( iSkillIDX, i ) )
			{
				case AT_LEVEL:
					{
						if( SKILL_NEED_ABILITY_VALUE( iSkillIDX, i ) <= g_pAVATAR->Get_LEVEL() )
							return true;
					}
					break;
			}
		}
	}

	return bResult;
}


///
///	스킬 사용을 위한 타겟 구하기
/// 일반 스킬일경우 hp체크를 하지만 리졀렉션같은 스킬의 경우 hp 체크를 하지 않는다.
///
/*static*/ CObjCHAR* CSkillManager::GetSkillTarget( int iServerObjIDX, int iSkillIDX )
{
	if( iSkillIDX == 0 )
		return g_pObjMGR->Get_ClientCharOBJ( iServerObjIDX, true );

	if( iSkillIDX >= g_SkillList.Get_SkillCNT() )
	{
		assert( 0 && "Skill index is greater than max skill count" );
		return g_pObjMGR->Get_ClientCharOBJ( iServerObjIDX, true );
	}

	if( SKILL_TYPE( iSkillIDX ) == SKILL_ACTION_RESURRECTION )
		return g_pObjMGR->Get_ClientCharOBJ( iServerObjIDX, false );

	return g_pObjMGR->Get_ClientCharOBJ( iServerObjIDX, true );
}