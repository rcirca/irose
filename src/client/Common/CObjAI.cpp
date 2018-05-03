/*
	** 주의 사항 !!!
	common/*.cpp, *.h 파일들은 서버와 소스를 공유함으로
	클라이언트만 생각해서 고치면 절대로 안됨 !!!! ***

	$Header: /Client/Common/CObjAI.cpp 234   05-09-30 10:57a Gioend $

	** 2004/4/28 **
	서버와의 코드 분리로.. CObjAI 는 이제 클라이언트에서만 사용함..
*/
#include "stdAFX.h"
#include "CObjCHAR.h"
#include "OBJECT.h"
#include "..\Network\CNetwork.h"
#include "../GameCommon/Skill.h"
#include "it_mgr.h"
#include "../CommandFilter.h"

#include "../IO_Terrain.h"
#include "../Country.h"



enum SKILL_STATE
{
	SKILL_NONE_STATE = 0,
	SKILL_CASTING_STATE = 1,
	SKILL_CASTING_LOOP_STATE = 2,
	SKILL_ACTION_STATE = 3,
};



int Global_GetWorldTIME()
{
	return ::Get_WorldTIME();
}



//--------------------------------------------------------------------------------
/// class : CObjTARGET
/// @param
/// @brief  : Target Object 를 구한다.( HP 체크 )
//--------------------------------------------------------------------------------

CAI_OBJ *CObjTARGET::Get_TargetOBJ()
{	
	return ( m_iServerTarget ) ? g_pObjMGR->Get_ClientCharOBJ( m_iServerTarget, true ) : NULL;
}

//--------------------------------------------------------------------------------
/// class : CObAI
/// @param
/// @brief  : Constructor
//--------------------------------------------------------------------------------

CObjAI::CObjAI ()
{
	m_pCurMOTION = NULL;
	m_iCurMotionFRAME = 0;

    m_wState        = 0;
    m_wCommand      = 0;

	m_bAttackSTART  = false;
	m_iActiveObject = 0;

	m_bCastingSTART	= false;
	SetCastingState( false );

	m_bRunMODE		= false;	// 기본은 걷기 모드...
	m_btMoveMODE	= 0;

	m_fRunAniSPEED  = 1.0f;
//	m_fAtkAniSPEED  = 1.0f;
	m_fCurMoveSpeed = 0;


	m_SkillActionState = SKILL_NONE_STATE;

	m_nToDoSkillIDX = 0;
	m_nActiveSkillIDX = 0;
	m_nDoingSkillIDX = 0;
	
	m_iWaitLoopCnt = 0;
}

//--------------------------------------------------------------------------------
/// class : CObAI
/// @param
/// @brief  : Destructor
//--------------------------------------------------------------------------------

CObjAI::~CObjAI ()
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 	현재 월드 시간을 리턴한다...
//----------------------------------------------------------------------------------------------------
int	CObjAI::Get_WorldTIME(void) 
{ 
	return ::Global_GetWorldTIME(); 
}



//--------------------------------------------------------------------------------
/// class : CObAI
/// @param short nRightWeaponItemNO 현재 오른손 무기번호
/// @brief  : 공식이 바뀔경우 서버랑 맞추기 위해 이곳으로 옮김... 2004. 2. 6
///				무기 번호로 공격속도를 계산 : 몹/NPC제외 : 아바타/유저만 해당.
//--------------------------------------------------------------------------------

int	CObjAI::Cal_AtkAniSPEED( short nRightWeaponItemNO )
{
	if( CCountry::GetSingleton().IsApplyNewVersion() )
	{
		/// 100을 기준으로 계산됨.
		float fWeaponSpeed = (int) ( 1500.f / ( WEAPON_ATTACK_SPEED( nRightWeaponItemNO ) + 5 ) );
		return (int)( fWeaponSpeed + this->GetPsv_ATKSPEED(fWeaponSpeed, nRightWeaponItemNO ) );
	}
	else
		return (int) ( this->GetPsv_ATKSPEED() + 1500.f / ( WEAPON_ATTACK_SPEED( nRightWeaponItemNO ) + 5 ) );
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param tagMOTION* pMotion
/// @brief  : 새로운 모션을 현재 모션으로 세팅, 프레임 리셋.
/// 
//--------------------------------------------------------------------------------

bool CObjAI::Set_CurMOTION( tagMOTION* pMotion )
{
	m_pCurMOTION = pMotion;
	m_iCurMotionFRAME = 0;

	return ( NULL != m_pCurMOTION );
}

//--------------------------------------------------------------------------------
/// class : CObAI
/// @param tagMOTION* pMotion
/// @brief  : 현재 모션을 바꾼다.( 이전 모션이랑 같으면 그냥 패스 )
//--------------------------------------------------------------------------------

bool CObjAI::Chg_CurMOTION( tagMOTION* pMotion )
{
	if ( pMotion && m_pCurMOTION != pMotion ) {
		m_pCurMOTION = pMotion;
		m_iCurMotionFRAME = 0;
		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param CObjCHAR *pTarget 타겟 오브젝트
/// @brief  : 공격시작.
//--------------------------------------------------------------------------------

void CObjAI::Start_ATTACK (CObjCHAR *pTarget)
{
	_ASSERT( pTarget );

//---------------------------------------------------------------------------------
#if defined(_GBC)
   //박지호::펫모드일때는 회전을 시키지 않는다.
   if((GetPetMode() < 0) && pTarget )
		Set_ModelDIR( pTarget->m_PosCUR );
#else
	Set_ModelDIR( pTarget->m_PosCUR );
#endif
//---------------------------------------------------------------------------------

	/// 공격 명령중에 실행될 스킬이 있는가 ???
	/*if ( this->Do_SKILL( Get_TargetIDX(), pTarget ) )
		return;*/

	/// 일반 공격시작 상태 설정.
	m_wState = CS_ATTACK;
	m_iActiveObject = g_pObjMGR->Get_ClientObjectIndex( Get_TargetIDX() );


	_ASSERT( m_iServerTarget == g_pObjMGR->Get_ServerObjectIndex( m_iActiveObject ) );

	if ( Attack_START( pTarget ) )
	{
		// 공격 애니메이션 & 속도 설정. :: 크리티컬 공격일 경우 모션을 별도로 존재하지 않고 효과로 출력한다.
		this->Set_MOTION( this->GetANI_Attack(), 0, this->Get_fAttackSPEED(), true );
	
		#if defined(_DEBUG) && !defined(__SERVER)
			if ( m_pCurMOTION->m_nActionPointCNT <= 0 ) {
				char *szMsg = CStr::Printf("%s 공격 타점 프레임 설정 필요!!!", Get_NAME());
				g_pCApp->ErrorBOX( szMsg, "ERROR" );
				LogString (LOG_DEBUG, szMsg);
			}
		#endif
	}
}

//--------------------------------------------------------------------------------
/// class : CObAI
/// @param int iServerTarget 타겟의 서버 인덱스
/// @param CObjCHAR *pTarget 타겟 오브젝트
/// @brief  : casting
//--------------------------------------------------------------------------------

char CObjAI::Do_SKILL (int iServerTarget, CObjCHAR *pTarget)
{
	/// 캐스팅 중에는 계속 타겟을 바라본다...
	/// 타겟 방향으로 몸을 돌린다.
//---------------------------------------------------------------------------------
#if defined(_GBC)
   //박지호::펫모드일때는 회전을 시키지 않는다.
   if((GetPetMode() < 0) && pTarget )
		Set_ModelDIR( pTarget->m_PosCUR );
#else 
	if(pTarget)
		Set_ModelDIR( pTarget->m_PosCUR );
#endif
//---------------------------------------------------------------------------------

	switch( m_SkillActionState )
	{
		case SKILL_CASTING_STATE:
			/// 정상적으로 캐스팅 동작을 수행했다면 캐스팅 루프 동작으로 전이
			/// 5는 나만의 상징적 정상 진행 수.. ^^;
			if( ProcSkillCastingAction( iServerTarget, pTarget ) == 5 )
			{
				/// 명령 필터링을 위해 저장된 명령 클리어
				g_CommandFilter.SetPrevCommand( NULL );
				/// 캐스팅 동작이 있는데 별문제 없을때..
			}
			/// 캐스팅 동작이 아예 없거나.. 혹은 데이터가 없을때.
			m_SkillActionState = SKILL_CASTING_LOOP_STATE;							
			return 1;

		case SKILL_CASTING_LOOP_STATE:
			if( ProcSkillCastingLoop( pTarget ) == 5 )
			{	
				m_SkillActionState = SKILL_ACTION_STATE;
			}
			return 1;

		case SKILL_ACTION_STATE:
			ProcSkillAction( pTarget );
			/// 한번 액션 수행후에는 모든 스킬정보 해지..
			m_SkillActionState = SKILL_NONE_STATE;

			Casting_END ();	
			return 1;
	}

	
	Casting_END ();	

	return 0;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 스킬의 캐스팅 동작을 처리..
//----------------------------------------------------------------------------------------------------

int CObjAI::ProcSkillCastingAction( int iServerTarget, CObjCHAR* pTarget )
{
	if ( m_nToDoSkillIDX ) 
	{
		/// 설정된 스킬이 있다..
		if ( !Casting_START( NULL ) )
			return 0;
		//----------------------------------------------------------------------------------------------------
		/// 스킬 주문 동작 !!!
		/// Casting 시작 상태 설정.
		/// 스킬 동작중에는 맞는 동작도 적용 안된다.
		//----------------------------------------------------------------------------------------------------
		this->m_wState = CS_CASTING;


		m_iActiveObject   = g_pObjMGR->Get_ClientObjectIndex( iServerTarget );
		_ASSERT( iServerTarget == g_pObjMGR->Get_ServerObjectIndex( m_iActiveObject ) );

		m_nActiveSkillIDX = m_nToDoSkillIDX;
		m_nToDoSkillIDX   = 0;
		
		
		if ( this->GetANI_Casting() || this->IsA( OBJ_MOB ) ) 
		{			
			this->Set_MOTION( this->GetANI_Casting(), 0, g_SkillList.Get_CastingAniSPEED( m_nActiveSkillIDX ), false, 1 );			
			Log_String( LOG_NORMAL, "스킬 캐스팅동작 세팅[%d]\n", this->GetANI_Casting() );
			return 5;
		}else
			assert( 0 && "Invalid Casting animation" );
	}

	return 0;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 캐스팅 동작과 실제 동작사이의 루프를 채우는 동작을 처리한다.
//----------------------------------------------------------------------------------------------------

int CObjAI::ProcSkillCastingLoop( CObjCHAR *pTarget )
{
	/// 적용할 스킬에 대한 결과가 서버로 부터 날라왔다면 액션진입 아니라면 루프
	if( SKILL_TYPE( m_nActiveSkillIDX ) >= SKILL_ACTION_FIRE_BULLET ||
		SKILL_TYPE( m_nActiveSkillIDX ) == SKILL_ACTION_IMMEDIATE )
		//&& SKILL_TYPE( m_nActiveSkillIDX ) <= SKILL_ACTION_TARGET_STATE_DURATION )
	{

		/*
		*  타겟이 없으면 바로 캐스팅 동작으로 들어간다..
		*	스킬시전중 캐릭터 모션이 정지하는 버그가 이거랑 연관이 있는가?
		*   어쨌던 내가 죽던지 적이 죽던지에 버그가 있다 - 04/5/25
		*/

		/// 타겟이 설정되어야만 하는 스킬인데 타겟이 없다면..
		if( ( CSkillManager::GetSkillTargetType( m_nActiveSkillIDX ) != SKILL_TARGET_NONE ) &&
			( pTarget == NULL ) )
		{
			SetEffectedSkillFlag( true );
		}else
		{
			if( ( !bCanActionActiveSkill() )				||	/// 서버로부터 결과를 못받았거나
				( SKILL_ANI_CASTING_REPEAT_CNT( m_nActiveSkillIDX ) != 0 ) )	/// 캐스팅 동작 루프가 설정됬을경우..
			{
				if( ( m_iWaitLoopCnt < 10 ) )
				{
					if( !bCanActionActiveSkill() ) //|| ( m_iWaitLoopCnt < SKILL_ANI_CASTING_REPEAT_CNT( m_nActiveSkillIDX ) ) )
					{						
						m_iWaitLoopCnt++;
						if ( this->GetANI_CastingRepeat() ) 
						{				
							/// 스킬 주문 동작 !!!
							/// Casting 시작 상태 설정.
							/// 스킬 동작중에는 맞는 동작도 적용 안된다.
							this->m_wState = CS_CASTING;
							this->Set_MOTION( this->GetANI_CastingRepeat(), 0, g_SkillList.Get_CastingAniSPEED( m_nActiveSkillIDX ), false, 1 );
							Log_String( LOG_NORMAL, "스킬 반복동작 세팅[%d][Count:%d]\n", this->GetANI_CastingRepeat(), m_iWaitLoopCnt );
							return 1;
						}			
					}
				}else
				{
					assert( 0 && "Not received result of skill" );
				}
			}
		}
	}
	
	m_iWaitLoopCnt = 0;

	return 5;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  캐스팅 완료된 스킬의 실제 동작 모션...
//----------------------------------------------------------------------------------------------------

int CObjAI::ProcSkillAction( CObjCHAR *pTarget )
{
	if ( m_nActiveSkillIDX ) 
	{			
		if ( this->Skill_START( pTarget ) ) 
		{
			/// 스킬 동작중에는 맞는 동작도 적용 안된다.
			m_wState = CS_NEXT_STOP2;
			this->Set_MOTION( this->GetANI_Skill(), 0, g_SkillList.Get_ActionAniSPEED( m_nActiveSkillIDX ), true, 1 );
			Log_String( LOG_NORMAL, "스킬 액션동작 세팅[%d]\n", this->GetANI_Skill() );
		
			SetNewCommandAfterSkill( m_nActiveSkillIDX );
		

			/// Casting_END 로 이동.. 모든 스킬이 끝나면 리셋된다. 
			m_nDoingSkillIDX = m_nActiveSkillIDX;			
			

			SetEffectedSkillFlag( false );			
			SetStartSkill( false );

			return 2;
		}
		
		/*m_nActiveSkillIDX = 0;
		SetEffectedSkillFlag( false );*/
	}

	return 0;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param 
/// @brief  : 스킬시전후 새로운 명령 세팅
/// @todo 임의로 17번타입은 사용후 서게 만들었다.
//--------------------------------------------------------------------------------

void CObjAI::SetNewCommandAfterSkill( int iSkillNO )
{
	
//--------------------------------------------------------------------------
#if defined(_GBC)
	//박지호::펫모드일때 스킬 마직막 카트의 마지막 동작으로 처리한다. 
	if(GetPetMode() >= 0)
	{
		SetNewCommandAfterSkill_PET(iSkillNO);
		return;
	}
	
#endif 
//--------------------------------------------------------------------------

	switch( SKILL_ACTION_MODE( iSkillNO ) )
	{
		case SA_STOP:
			{
				m_wCommand = CMD_STOP;
			}
			break;
		case SA_ATTACK:
			{
				/// 공격설정이 되었더라도, 타겟이 없다면 STOP~
				if( this->Get_TargetOBJ() != NULL )
				{
					m_wCommand = CMD_ATTACK;


					CObjCHAR* pDestCHAR = (CObjCHAR*)(this->Get_TargetOBJ());
					/// 나일경우 PVP존이 아닌존에서 유져공격명령은 취소한다.
					if( this->IsA( OBJ_USER ) && pDestCHAR->IsUSER() )
					{
						if( !g_pTerrain->IsPVPZone() || g_GameDATA.m_iPvPState == PVP_CANT )
						{
							g_pNet->Send_cli_STOP( g_pAVATAR->Get_CurPOS() );
							m_wCommand      = CMD_STOP;
							return;
						}
					}

				}
				else
					m_wCommand = CMD_STOP;
			}
			break;
		case SA_RESTORE:
			{
				m_wCommand = m_wBeforeCMD;
				m_wBeforeCMD = CMD_STOP;

				/// 공격명령 복구시킬때는 타겟 조사..
				if( m_wCommand == CMD_ATTACK )
				{
					CObjCHAR* pDestCHAR = (CObjCHAR*)(this->Get_TargetOBJ());

					if( pDestCHAR )
					{
						/// 나일경우 PVP존이 아닌존에서 유져공격명령은 취소한다.
						if( this->IsA( OBJ_USER ) && pDestCHAR->IsUSER() )
						{
							if( !g_pTerrain->IsPVPZone() || g_GameDATA.m_iPvPState == PVP_CANT )
							{
								g_pNet->Send_cli_STOP( g_pAVATAR->Get_CurPOS() );
								m_wCommand      = CMD_STOP;
								return;
							}
						}
					}
				}
			}
			break;
	}	
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param 
/// @brief  : Move vector reset
///				1. Normalize DirVEC, 2. DirVEC * Move Speed
///				몬스터 중에는 이동속도 0인게 있음..
//--------------------------------------------------------------------------------

void CObjAI::Reset_MoveVEC ()
{
	tPOINTF DirVEC;


	this->Set_ModelDIR ( m_PosGOTO );
	this->Set_ModelSPEED( m_fCurMoveSpeed );
	DirVEC.x = m_PosGOTO.x - m_PosCUR.x;
	DirVEC.y = m_PosGOTO.y - m_PosCUR.y;


	float fLength = DirVEC.Length () * 1000.f;
	if ( m_fCurMoveSpeed > 0 && fLength != 0.0f ) 
	{
		m_MoveVEC = ( DirVEC * m_fCurMoveSpeed ) / fLength; // Normalized dirvec * move speed
		_ASSERT( m_MoveVEC.x != 0 || m_MoveVEC.y != 0 );
	} else 
	{
		// 이동할 필요가 없는데...
		m_MoveVEC.x = 0;
		m_MoveVEC.y = 0;
	}
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param float fSpeed 이동속도
/// @brief  :  Start move
///				현재 이동중이면 거리 계산하고, MoveVEC 만 리셋.
///				이동중이 아니라면 모션 세팅 및 이동 시작 높이 설정.
//--------------------------------------------------------------------------------

void CObjAI::Start_MOVE( float fSpeed )
{

	m_fCurMoveSpeed = fSpeed;
	m_PosMoveSTART  = m_PosCUR;


	m_iMoveDistance = CD3DUtil::distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)m_PosGOTO.x, (int)m_PosGOTO.y);
	if ( m_iMoveDistance <= 0 ) {
		m_wState  = CS_STOP;
		this->Set_MOTION( this->GetANI_Stop() );
		return;
	}

	this->Reset_MoveVEC ();
	if ( Get_STATE() != CS_MOVE ) {
		// 이동중이 아니다.
		m_wState  = CS_MOVE;
		this->Set_MOTION( this->GetANI_Move(), m_fCurMoveSpeed, this->Get_MoveAniSPEED() );

		Reset_Position(); // 이동 시작 높이 설정. 서버와의 높이 좌표를 여기에서 맞춘다.
	}

	this->MoveStart();
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param t_POSITION &PosGOTO 이동 목적지
/// @brief  : Restart move
///			최종좌표가 바뀌지 않았다면 그대로 고.
///			최종좌표가 수정되었다면, 다시 스타트.
//--------------------------------------------------------------------------------

void CObjAI::Restart_MOVE (t_POSITION &PosGOTO)
{
	if ( Get_STATE() == CS_MOVE && ( abs( PosGOTO.x - m_PosGOTO.x ) < 0.0001 ) && ( abs( PosGOTO.x - m_PosGOTO.x ) < 0.0001 ) ) 
	{
		// 뉐최종 좌표가 바뀌지 않았다.
		return;
	}

	m_PosGOTO = PosGOTO;

	this->Start_MOVE( this->Get_MoveSPEED() );	
}

void CObjAI::Restart_MOVE_AL (t_POSITION &PosGOTO)
{

	m_PosGOTO = PosGOTO;

	this->Start_MOVE( this->Get_MoveSPEED() );	
}



//--------------------------------------------------------------------------------
/// class : CObAI
/// @param 
/// @brief  : 앉기 명령 세팅
//--------------------------------------------------------------------------------

void CObjAI::SetCMD_SIT (void)
{
	if( this->IsA( OBJ_USER ) )
	{
		CObjSitCommand* pObjCommand = (CObjSitCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_SIT );
		pObjCommand->SetCMD_SIT();

		g_CommandFilter.SetPrevCommand( pObjCommand );
	}

/// 현재 명령이 들어갈수 있나?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandSit();
		return;
	}

	m_wState		= CS_SITTING;
	m_wCommand		= CMD_SIT;
	m_fCurMoveSpeed = 0;

	this->Set_MOTION( this->GetANI_Sitting() );
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param 
/// @brief  : 서기 명령 세팅
//--------------------------------------------------------------------------------

void CObjAI::SetCMD_STAND (void)
{
	if( this->IsA( OBJ_USER ) )
	{
		CObjStandCommand* pObjCommand = (CObjStandCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_STAND );
		pObjCommand->SetCMD_STAND();

		g_CommandFilter.SetPrevCommand( pObjCommand );
	}

	/// 현재 명령이 들어갈수 있나?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandStand();
		return;
	}

	m_wState		= CS_STANDING;
	m_wCommand		= CMD_STOP;
	m_fCurMoveSpeed = 0;

	this->Set_MOTION( this->GetANI_Standing() );
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param 
/// @brief  : Stop command 세팅.
///				I must set "m_iTargetObject" to 0??? if My state is STOP
//--------------------------------------------------------------------------------

bool CObjAI::SetCMD_STOP (void)
{
	if( this->IsA( OBJ_USER ) )
	{
		CObjStopCommand* pObjCommand = (CObjStopCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_STOP );
		pObjCommand->SetCMD_STOP( );

		g_CommandFilter.SetPrevCommand( pObjCommand );
	}
	
	/// 현재 명령이 들어갈수 있나?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandStop();
		return false;
	}

	/// 캐스팅중.. 스킬취소나 기타 이유에 의해서 STOP명령이 설정될경우에는 캐스팅을 종료한다.
	this->Casting_END();

//------------------------------------------------------------------------------------
#if defined(_GBC)
	//박지호::카트 스킬 사용 후 아바타 앉히기 위해서...
 	/// If this char is under pet mode, pass the command to pet.
	if( this->GetPetMode() >= 0 )
	{
		SetCMD_PET_STOP( );
		//return;
	}
#endif 
//------------------------------------------------------------------------------------
	m_wState		= CS_STOP;
	m_wCommand		= CMD_STOP;
	m_fCurMoveSpeed = 0;

	this->Set_MOTION( this->GetANI_Stop() );

	ChangeActionMode( AVATAR_NORMAL_MODE );

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param float fPosX 이동목적지 X 좌표
/// @param float fPosY 이동목적지 Y 좌표
/// @param BYTE btRunMODE
/// @brief  : Move command 세팅.
//--------------------------------------------------------------------------------

bool CObjAI::SetCMD_MOVE2D (float fPosX, float fPosY, BYTE btRunMODE)
{	
	D3DVECTOR PosTO;
	PosTO.x = fPosX;
	PosTO.y = fPosY;
	PosTO.z = this->m_PosCUR.z;

	CObjAI::SetCMD_MOVE( PosTO, btRunMODE );

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param const D3DVECTOR& PosTO 이동목적지 좌표
/// @param BYTE btRunMODE
/// @brief  : Move command 세팅.
///				SetCMD_MOVE (tPOINTF &PosCUR, tPOINTF &PosTO, int iTargetObjIDX) 호출
//--------------------------------------------------------------------------------

bool CObjAI::SetCMD_MOVE (const D3DVECTOR& PosTO, BYTE btRunMODE)
{	
	if( this->IsA( OBJ_USER ) )
	{
		CObjMoveCommand* pObjCommand = (CObjMoveCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_MOVE );
		pObjCommand->SetCMD_MOVE( PosTO, btRunMODE );

		g_CommandFilter.SetPrevCommand( pObjCommand );
	}

	/// 현재 명령이 들어갈수 있나?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandMove( PosTO, btRunMODE );
		return false;
	}

	/// If this char is under pet mode, pass the command to pet.
	if( GetPetMode() >= 0 )
	{
		SetCMD_PET_MOVE( PosTO, btRunMODE );
		return false;
	}


	this->m_bRunMODE = ( btRunMODE != 0 );

	WORD wServeDist = CD3DUtil::distance ((int)this->m_PosCUR.x, (int)this->m_PosCUR.y, 
										(int)PosTO.x, (int)PosTO.y);

	if( this->IsPET() )
	{
		LogString( LOG_NORMAL, "CMD_MOVE:[ %f, %f ]\n", PosTO.x, PosTO.y );
	}

	CObjAI::SetCMD_MOVE ( wServeDist, PosTO, 0);

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param tPOINTF &PosFROM 이동 출발지
/// @param tPOINTF &PosTO   이동 목적지
/// @param int iServerTarget 타겟의 서버 인덱스
/// @brief  : Move command 세팅.
//--------------------------------------------------------------------------------

void CObjAI::SetCMD_MOVE ( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget )
{
	if( this->IsA( OBJ_USER ) )
	{
		CObjMoveCommand* pObjCommand = (CObjMoveCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_MOVE );
		pObjCommand->SetCMD_MOVE( wSrvDIST, PosTO, iServerTarget );

		g_CommandFilter.SetPrevCommand( pObjCommand );
	}

	/// 현재 명령이 들어갈수 있나?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandMove( wSrvDIST, PosTO, iServerTarget );
		return;
	}

	/// If this char is under pet mode, pass the command to pet.
	if( GetPetMode() >= 0 )
	{
		SetCMD_PET_MOVE( wSrvDIST, PosTO, iServerTarget );
		return;
	}

	// TODO:: adjust current position ..
	// m_PosCUR = PosCUR;
	m_wCommand = CMD_MOVE;

	CGameOBJ *pDestOBJ = g_pObjMGR->Get_ClientOBJECT( iServerTarget );
	/// 타겟 오브젝트가 있다면. 타겟 오브젝트로 이동.
	if ( pDestOBJ ) 
	{
		/// 대상이 아이템이라면 CMD_PICK_ITEM 명령.
		if ( pDestOBJ->Get_TYPE() == OBJ_ITEM )
			m_wCommand = CMD_PICK_ITEM;

		m_PosGOTO	    = pDestOBJ->Get_CurPOS();
		this->Set_TargetIDX( iServerTarget );
	} else {
		// m_pRecvPacket->m_gsv_MOUSECMD.m_PosCUR;
		m_PosGOTO = PosTO;
		this->Set_TargetIDX( 0 );
	}

	// 클라이언트에서는 타겟 유지..
	m_iServerTarget = iServerTarget;
	this->Adj_MoveSPEED( wSrvDIST, PosTO );

	if ( CS_BIT_INT & m_wState )
	{
		m_wState = CS_NEXT_STOP;		
	}
	else {
		this->Start_MOVE( this->Get_MoveSPEED() );
	}
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param int iServerTarget 타겟의 서버 인덱스
/// @brief  : Attack command 세팅.
///				타겟이 없는 경우는?( 죽었나? )
//--------------------------------------------------------------------------------

bool CObjAI::SetCMD_ATTACK (int iServerTarget)
{	

	/// 현재 명령이 들어갈수 있나?
	/// 아래 부분도 CObjCHAR::SetCMD_Attack( .., .., .. ) 로 간다.
	/*if( this->CanApplyCommand() == false )
	{
		this->PushCommandAttack( iServerTarget );
		return false;
	}*/


	/// SetCMD_ATTACK 는 CObjAI 가 아니라 최초 명령은
	/// CObjCHAR 의 SetCMD_ATTACK( .., .., .. );  이 받는다.

	this->Casting_END();

	if ( CS_BIT_INT & this->m_wState )
		m_wState = CS_NEXT_STOP;
	else
		m_wState = CS_STOP;

	CObjCHAR *pDestCHAR = g_pObjMGR->Get_ClientCharOBJ( iServerTarget, true );
	if ( pDestCHAR ) 
	{
		m_wCommand      = CMD_ATTACK;
		m_PosGOTO       = pDestCHAR->m_PosCUR;

		this->Set_TargetIDX( iServerTarget );


		/// 나일경우 PVP존이 아닌존에서 유져공격명령은 취소한다.
		if( this->IsA( OBJ_USER ) && pDestCHAR->IsUSER() )
		{
			if( !g_pTerrain->IsPVPZone() || g_GameDATA.m_iPvPState == PVP_CANT )
			{
				g_pNet->Send_cli_STOP( g_pAVATAR->Get_CurPOS() );
				m_wCommand      = CMD_STOP;
				return false;
			}
		}


	} else 
	{
		/// 클라이언트에서는 타겟이 없어도 공격 명령과 서버 타겟을 유지한다.
		m_wCommand		= CMD_ATTACK;
		m_iServerTarget = iServerTarget;
	}

	ChangeActionMode( AVATAR_ATTACK_MODE );

	/// 상태에 변화가 생겼다.
	return true;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param 
/// @brief  : Die command 세팅.
///			현재 명령이 들어갈수 있나?
///			죽는건 바로 적용해보자.( Command 큐에 넣지 않음 ) ( 04/4/28 )
//--------------------------------------------------------------------------------

void CObjAI::SetCMD_DIE ()
{
	if( this->IsA( OBJ_USER ) )
	{
		g_CommandFilter.SetPrevCommand( NULL );
	}

	/// 현재 명령이 들어갈수 있나?
	/// 죽는건 바로 적용해보자
	/*if( this->CanApplyCommand() == false )
	{
		this->PushCommandDie( );
		return;
	}*/

	this->Set_TargetIDX( 0 );

	m_wState   = CS_DIE;
	m_wCommand = CMD_DIE;
	this->Set_MOTION( this->GetANI_Die() );

	this->Casting_END();
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param BYTE btTYPE 현재 토글 타입
/// @brief  : 상태 토글( 뛰기, 걷기 토글.. 앉기 서기 토글 )
//--------------------------------------------------------------------------------

bool CObjAI::SetCMD_TOGGLE (BYTE btTYPE)
{
	if( this->IsA( OBJ_USER ) )
	{
		g_CommandFilter.SetPrevCommand( NULL );
	}

	/// 현재 명령이 들어갈수 있나?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandToggle( btTYPE );
		return false;
	}

	/// Move Mode 에 변화가 있다..
	if( btTYPE >= TOGGLE_TYPE_DRIVE )
	{
		btTYPE -= TOGGLE_TYPE_DRIVE;
		this->m_btMoveMODE = btTYPE;

		switch( btTYPE )
		{
			case MOVE_MODE_WALK:
				{
					if( this->GetPetMode() >= 0 )
						this->RideCartToggle( false );
										
					float fAdjRate = this->Get_MoveSPEED() / this->Get_DefaultSPEED ();
					float fMove = this->Get_MoveSPEED();
					float fDefault =  this->Get_DefaultSPEED ();

					LogString( LOG_NORMAL, "WALK : %.3f, %.3f \n", fMove, fDefault );

					if( !(Get_STATE() == CS_SIT || Get_STATE() == CS_SITTING) )
					{
						this->m_bRunMODE = false;
						this->ToggleRunMODE ( fAdjRate );			/// m_bRunMode 세팅후에 호출해야된다.
					}
				}
				break;
			case MOVE_MODE_RUN:
				{
					if( this->GetPetMode() >= 0 )
						this->RideCartToggle( false );
					
					float fAdjRate = this->Get_MoveSPEED() / this->Get_DefaultSPEED ();
					float fMove = this->Get_MoveSPEED();
					float fDefault =  this->Get_DefaultSPEED ();

					LogString( LOG_NORMAL, "RUN : %.3f, %.3f \n", fMove, fDefault );

					if( !(Get_STATE() == CS_SIT || Get_STATE() == CS_SITTING) )
					{
						this->m_bRunMODE = true;
						this->ToggleRunMODE ( fAdjRate );
					}
				}
				break;
			case MOVE_MODE_DRIVE:
				{
					this->RideCartToggle( true );
				}
				break;
		}

	}else
	{
		switch( btTYPE ) 
		{
			/*case TOGGLE_TYPE_RUN :
				this->ToggleRunMODE ();
				break;*/

			case TOGGLE_TYPE_SIT :
				if ( !this->ToggleSitMODE () ) {
					return false;
				}
				break;
			/*case TOGGLE_TYPE_DRIVE:
				{
	#ifndef __SERVER
					this->RideCartToggle();
	#endif
				}
				break;*/
		}
	}

	return true;
}

//--------------------------------------------------------------------------------
/// class : CObAI
/// @brief  : 스킬 딜레이를 세팅한다.
///			
//--------------------------------------------------------------------------------
void CObjAI::SetSkillDelay( int iSkillIdx )
{
	//--------------------------------------------------------------------------------
	/// 유져일경우는 타이머를 세팅한다.
	//--------------------------------------------------------------------------------
	if( this->IsA( OBJ_USER ) )
	{
		/// 타이머 설정
		CSkillSlot* pSkillSlot = g_pAVATAR->GetSkillSlot();
		CSkill* pSkill = pSkillSlot->GetSkillBySkillIDX( iSkillIdx );
		if( pSkill )
		{
			pSkill->SetSkillDelayTime( SKILL_RELOAD_TIME( iSkillIdx ) * 200 );
		}
	}
}

//--------------------------------------------------------------------------------
/// class : CObAI
/// @param short nSkillIDX 스킬인덱스
/// @brief  : SetCMD_Skill2SELF
///				4/28 능력치 소모를 Result_Of_skill 에서 처리하게 옮김
//--------------------------------------------------------------------------------

void CObjAI::SetCMD_Skill2SELF (short nSkillIDX)
{
	/// 2005/7/25 CObjUSER로 이동 : nAvy
	//if( this->IsA( OBJ_USER ) )
	//{
	//	CObjSkill2SelfCommand* pObjCommand = (CObjSkill2SelfCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_Skill2SELF );
	//	pObjCommand->SetCMD_Skill2SELF( nSkillIDX );

	//	g_CommandFilter.SetPrevCommand( pObjCommand );
	//}


	/// 현재 명령이 들어갈수 있나?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandSkill2Self( nSkillIDX );
		return;
	}

	SetEffectedSkillFlag( false );
	Log_String( LOG_NORMAL, "SetCMD_Skill2SELF\n" );

	if ( CS_BIT_INT & this->m_wState )
		m_wState = CS_NEXT_STOP;
	else
		m_wState = CS_STOP;

	m_nToDoSkillIDX = nSkillIDX;

	SetSkillDelay( m_nToDoSkillIDX );

	//----------------------------------------------------------------------------------------------------	
	/// @brief 복구할 명령 설정
	//----------------------------------------------------------------------------------------------------
	switch( m_wCommand )
	{
		case CMD_SKILL2SELF:
		case CMD_SKILL2OBJ:
		case CMD_SKILL2POS:
			m_wBeforeCMD = CMD_STOP;
			break;
		default:
			m_wBeforeCMD = m_wCommand;
			break;
	}
    
	m_wCommand = CMD_SKILL2SELF;

	/// 일단 여기서 깍자..소모치
	/// 명령을 세팅할때 깍을까나?
	///CSkillManager::UpdateUseProperty( this, nSkillIDX );

	//-----------------------------------------------------------------------------------------
	/// 처음 시작상태는 캐스팅 상태부터 시작..
	//-----------------------------------------------------------------------------------------
	m_SkillActionState = SKILL_CASTING_STATE;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param int iServerTarget 타겟의 서버 인덱스
/// @param short nSkillIDX	 스킬 인덱스
/// @brief  : 오브젝트타겟 스킬
//--------------------------------------------------------------------------------
bool CObjAI::SetCMD_Skill2OBJ ( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget, short nSkillIDX)
{
	/// 2005/7/25 CObjUSER로 이동 : nAvy
	//if( this->IsA( OBJ_USER ) )
	//{
	//	CObjSkill2ObjCommand* pObjCommand = (CObjSkill2ObjCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_Skill2OBJ );
	//	pObjCommand->SetCMD_Skill2OBJ( wSrvDIST, PosTO, iServerTarget, nSkillIDX );

	//	g_CommandFilter.SetPrevCommand( pObjCommand );
	//}


	//-----------------------------------------------------------------------------------------
	/// 현재 명령이 들어갈수 있나?
	//-----------------------------------------------------------------------------------------
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandSkill2Obj( wSrvDIST, PosTO, iServerTarget, nSkillIDX );
		return false;
	}

	 SetEffectedSkillFlag( false );
	 Log_String( LOG_NORMAL, "SetCMD_Skill2OBJ\n" );

	 //-----------------------------------------------------------------------------------------
	/// 일단 여기서 깍자..소모치
	/// 명령을 세팅할때 깍을까나?

	/// 04/4/27 스킬 처리 결과를 받았을때 처리...
	//CSkillManager::UpdateUseProperty( this, nSkillIDX );
	 //-----------------------------------------------------------------------------------------

	if ( CS_BIT_INT & this->m_wState )
		m_wState = CS_NEXT_STOP;
	//else
	//	m_wState = CS_STOP;

	//----------------------------------------------------------------------------------------------------	
	/// @brief 복구할 명령 설정
	//----------------------------------------------------------------------------------------------------
	switch( m_wCommand )
	{
		case CMD_SKILL2SELF:
		case CMD_SKILL2OBJ:
		case CMD_SKILL2POS:
			m_wBeforeCMD = CMD_STOP;
			break;
		default:
			m_wBeforeCMD = m_wCommand;
			break;
	}

	///CObjCHAR *pDestCHAR = g_pObjMGR->Get_ClientCharOBJ( iServerTarget, true );
	CObjCHAR *pDestCHAR = CSkillManager::GetSkillTarget( iServerTarget, nSkillIDX );

	if ( pDestCHAR )	
	{
		m_nToDoSkillIDX = nSkillIDX;

		SetSkillDelay( m_nToDoSkillIDX );

		/*if ( SA_TARGET_ATTACK == SKILL_ACTION_MODE( nSkillIDX ) )
			m_wCommand = CMD_ATTACK;
		else*/
			m_wCommand = CMD_SKILL2OBJ;


		m_PosGOTO       = pDestCHAR->m_PosCUR;

		this->Set_TargetIDX( iServerTarget );

		LogString (LOG_DEBUG,"SetCMD_Skill2OBJ[ Skill:%d ] %s==>%s \n", nSkillIDX, this->Get_NAME(), pDestCHAR->Get_NAME() );		

	}
	else 
	{
		//-----------------------------------------------------------------------------------------
		// TODO:: 타겟을 못찾았을때...
		/// 클라이언트에서는 타겟이 없어도 공격 명령과 서버 타겟을 유지한다.
		//-----------------------------------------------------------------------------------------
		m_wCommand		= CMD_SKILL2OBJ;
		m_iServerTarget = iServerTarget;

		m_PosGOTO       = PosTO;
	}

	this->Adj_MoveSPEED( wSrvDIST, m_PosGOTO );	


	//-----------------------------------------------------------------------------------------
	/// 처음 시작상태는 캐스팅 상태부터 시작..
	//-----------------------------------------------------------------------------------------
	m_SkillActionState = SKILL_CASTING_STATE;


	return false;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param tPOINTF &PosGOTO	 타겟의 위치
/// @param short nSkillIDX	 스킬 인덱스
/// @brief  : 지역 타겟 스킬
//--------------------------------------------------------------------------------

void CObjAI::SetCMD_Skill2POS (const D3DVECTOR& PosGOTO, short nSkillIDX)
{
	/// 2005/7/25 CObjUSER로 이동 : nAvy
	//if( this->IsA( OBJ_USER ) )
	//{
	//	CObjSkill2PosCommand* pObjCommand = (CObjSkill2PosCommand*)g_CommandFilter.GetCommandObject( OBJECT_COMMAND_Skill2POS );
	//	pObjCommand->SetCMD_Skill2POS( PosGOTO, nSkillIDX );

	//	g_CommandFilter.SetPrevCommand( pObjCommand );
	//}

	/// 현재 명령이 들어갈수 있나?
	if( this->CanApplyCommand() == false )
	{
		this->PushCommandSkill2Pos( PosGOTO, nSkillIDX );
		return;
	}


	SetEffectedSkillFlag( false );
	Log_String( LOG_NORMAL, "SetCMD_Skill2POS\n" );

	/// 일단 여기서 깍자..소모치
	/// 명령을 세팅할때 깍을까나?
	///CSkillManager::UpdateUseProperty( this, nSkillIDX );


	if ( CS_BIT_INT & this->m_wState )
		m_wState = CS_NEXT_STOP;
	else
		m_wState = CS_STOP;

	//----------------------------------------------------------------------------------------------------	
	/// @brief 복구할 명령 설정
	//----------------------------------------------------------------------------------------------------
	switch( m_wCommand )
	{
		case CMD_SKILL2SELF:
		case CMD_SKILL2OBJ:
		case CMD_SKILL2POS:
			m_wBeforeCMD = CMD_STOP;
			break;
		default:
			m_wBeforeCMD = m_wCommand;
			break;
	}

	m_nToDoSkillIDX = nSkillIDX;

	SetSkillDelay( m_nToDoSkillIDX );


	m_wCommand		= CMD_SKILL2POS;
	m_PosGOTO		= PosGOTO;

	this->Set_TargetIDX( 0 );

	//-----------------------------------------------------------------------------------------
	/// 처음 시작상태는 캐스팅 상태부터 시작..
	//-----------------------------------------------------------------------------------------
	m_SkillActionState = SKILL_CASTING_STATE;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 디버깅용 스킬 스타트 타임 기록..
//----------------------------------------------------------------------------------------------------

void CObjAI::SetCastingState( bool bStart )
{
	if( bStart )
	{
		m_bCastingSTART		= true;
		m_iCastingStartTime = g_GameDATA.GetGameTime();
	}else
	{
		m_bCastingSTART		= false;
		m_iCastingStartTime = 0;

		m_SkillActionState = SKILL_CASTING_STATE;
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjAI::ProcOneActionFrame( int iIndex )
{
	/// 이건 필요없는것 같은데..
	if ( m_pCurMOTION && m_pCurMOTION->m_nActionPointCNT ) 
	{
		if ( m_pCurMOTION->m_pFrameEvent[ iIndex ] ) 
		{
			/// @bug :: vd_Client일경우 안에서 AI Action에 의해 m_pCurMOTION이 바뀌어 오는 경우가 생겨 뻑~~~~~~~ 
			//    구조를 바꿔야...
			ActionEVENT( m_pCurMOTION->m_pFrameEvent[ iIndex ] );

			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------
/// class : CObAI
/// @param tPOINTF &PosGOTO	 타겟의 위치
/// @param short nSkillIDX	 스킬 인덱스
/// @brief  : 애니매이션 진행..
/// @bug -->안에서 AI Action에 의해 m_pCurMOTION이 바뀌어 오는 경우가 생겨 뻑~~~~~~~
//--------------------------------------------------------------------------------

bool CObjAI::ProcMotionFrame (void)
{
	int iFrame = this->GetCurrentFrame();
	if ( iFrame < 0 ) 
	{
		_ASSERT( 0 );
		m_iCurMotionFRAME = 0;
		m_wState &= ~CS_BIT_INT;
		return false;
	}	

	_ASSERT( iFrame <= m_pCurMOTION->m_wTotalFrame );
	_ASSERT( m_iCurMotionFRAME >= 0 );

	/// CS_BIT_CHK 에 의해 체크된것은 이후에 MOV 때 이벤트 체크를 하면 안된다.
	bool bCheckedActionFrame = false;

	/// 동작프레임을 체크 해야된다면..
	if ( m_wState & CS_BIT_CHK ) 
	{
		if ( m_pCurMOTION && m_pCurMOTION->m_nActionPointCNT ) 
		{

			//----------------------------------------------------------------------------------------------------			
			/// @brief iFrame < m_iCurMotionFRAME 일경우 내재된 버그가 있다.
			///        그래서 두 경우를 분리해서 처리
			//----------------------------------------------------------------------------------------------------
			if( iFrame >= m_iCurMotionFRAME )
			{
				/// check frame action index.
				for( int iL = m_iCurMotionFRAME ; iL < iFrame && iL < m_pCurMOTION->Get_TotalFRAME() ; iL++ ) 
				{
					bCheckedActionFrame = ProcOneActionFrame( iL );
				}
			}else
			{				
				/// 먼저 끝프레임까지 진행한 후에.
				for( int iL = m_iCurMotionFRAME ; iL < m_pCurMOTION->Get_TotalFRAME() ; iL++ ) 
				{
					bCheckedActionFrame = ProcOneActionFrame( iL );
				}
			
				/// 첫프레임부터 iFrame 까지 처리
				for( int iL = 0 ; iL < iFrame ; iL++ ) 
				{
					bCheckedActionFrame = ProcOneActionFrame( iL );
				}
			}
		}

		/// 한번만 체크
		if ( m_wState & CS_BIT_ONE ) 
		{
			/// iFrame < m_iCurMotionFRAME 애니매이션이 한바귀 돌아 iFrame이 리셋됬을경우지
			if ( iFrame < m_iCurMotionFRAME || m_iCurMotionFRAME >= ( m_pCurMOTION->m_wTotalFrame - 1 ) ) 
			{
				// 모션이 완료 됐다.
				m_iCurMotionFRAME = 0;
				m_wState &= ~CS_BIT_INT;
				return false;
			}
		}
	}


	//--------------------------------------------------------------------------------
	///// 발자국 소리때문에 이동시에도 프레임 체크
	//--------------------------------------------------------------------------------
	if( !bCheckedActionFrame && m_wState & CS_MOVE )
	{
		//----------------------------------------------------------------------------------------------------			
		/// @brief iFrame < m_iCurMotionFRAME 일경우 내재된 버그가 있다.
		///        그래서 두 경우를 분리해서 처리
		//----------------------------------------------------------------------------------------------------
		if( iFrame >= m_iCurMotionFRAME )
		{
			/// check frame action index.
			for( int iL = m_iCurMotionFRAME ; iL < iFrame && iL < m_pCurMOTION->Get_TotalFRAME() ; iL++ ) 
			{
				bCheckedActionFrame = ProcOneActionFrame( iL );
			}
		}else
		{				
			/// 먼저 끝프레임까지 진행한 후에.
			for( int iL = m_iCurMotionFRAME ; iL < m_pCurMOTION->Get_TotalFRAME() ; iL++ ) 
			{
				bCheckedActionFrame = ProcOneActionFrame( iL );
			}
		
			/// 첫프레임부터 iFrame 까지 처리
			for( int iL = 0 ; iL < iFrame ; iL++ ) 
			{
				bCheckedActionFrame = ProcOneActionFrame( iL );
			}
		}
	}	

	
	/// @todo m_iCurMotionFRAME = iFrame;	  이걸 넣었는데 어떨지..
	/// 모션이 끝났다.
	/// iFrame < m_iCurMotionFRAME 애니매이션이 한바귀 돌아 iFrame이 리셋됬을경우지
	if ( iFrame < m_iCurMotionFRAME || m_iCurMotionFRAME >= ( m_pCurMOTION->m_wTotalFrame - 1 ) ) 
	{
		m_iCurMotionFRAME = iFrame;	
		return false;
	}

	m_iCurMotionFRAME = iFrame;	

	return true;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param CObjCHAR *pTarget	타겟 오브젝트
/// @param int iRange			범위( 거리 )
/// @brief  : 타겟으로 이동
//--------------------------------------------------------------------------------

bool CObjAI::Goto_TARGET( CObjCHAR *pTarget, int iRange )
{
	if ( IsInRANGE( pTarget, iRange ) ) 
	{
		/// 거리안에 들어 왔다...
		this->m_PosGOTO = this->m_PosCUR;
		this->Move_COMPLETED ();
		return true;
	}
	
	// 최종 위치가 바뀌었으면 이동 벡터 다시 계산...
	// 이동중이 아닌 상태면...이동하지 않는다.
	this->Restart_MOVE( pTarget->m_PosCUR );

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @param int iRange			범위( 거리 )
/// @brief  :  이거 뭐하는데 사용하는거지?
//--------------------------------------------------------------------------------

bool CObjAI::Goto_POSITION (int iRange)
{
	if ( !(Get_STATE() & CS_BIT_MOV) )
	{
		// 이동중이 아니면 이동 시작...
		this->Start_MOVE( this->Get_MoveSPEED() );
	}

	int iDistance = CD3DUtil::distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y);
	if ( iDistance+iRange >= m_iMoveDistance ) 
	{
		/// 거리안에 들어 왔다...
		this->m_PosGOTO = this->m_PosCUR;
		this->Move_COMPLETED ();		
		return true;
	} 

	return false;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @brief  : sit 컴맨드 처리 함수
//--------------------------------------------------------------------------------

int CObjAI::ProcCMD_SIT ()
{
	if ( this->Get_STATE() != CS_SIT ) {
		this->Set_STATE( CS_SIT );
		this->Set_MOTION( this->GetANI_Sit() );
	} 

	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @brief  : Stop command 처리 함수
///			서버와 연결상태에서는 Do_StopAI 호출은 무의미 하다..
//--------------------------------------------------------------------------------

int CObjAI::ProcCMD_STOP ()
{
	if ( Get_STATE() != CS_STOP ) 
	{
		CObjAI::SetCMD_STOP ();
	} 

	this->Do_StopAI ();
	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @brief  : 이동 프로세스..
///				이동중 대상이 NPC이거나, ITEM 일 경우 처리..
//--------------------------------------------------------------------------------
#include "../Interface/Dlgs/CAvatarStoreDlg.h"
int CObjAI::ProcCMD_MOVE ()
{
	
	CObjCHAR *pTarget = (CObjCHAR*)this->Get_TARGET();

	/*
	if ( this->IsA( OBJ_AVATAR ) ) {
		LogString (LOG_DEBUG, "%s Move (%.0f,%.0f) ==> (%.0f,%.0f) \n", this->Get_NAME(), this->m_PosCUR.x, this->m_PosCUR.y, this->m_PosGOTO.x, this->m_PosGOTO.y );
	}
	*/

	if ( pTarget )
	{
		if( pTarget->Is_AVATAR() ) 
		{
			/// 대상이 사용자라면
			if ( Goto_TARGET( pTarget, AVT_CLICK_EVENT_RANGE ) ) 
			{
				
				if( this->IsA(OBJ_USER) )
				{
					CObjAVT* pAvt = (CObjAVT*)pTarget;
					if( pAvt->IsPersonalStoreMode() )
					{
						g_pNet->Send_cli_P_STORE_LIST_REQ( g_pObjMGR->Get_ServerObjectIndex(pAvt->Get_INDEX()) );			
						CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
						if( pDlg )
						{
							CAvatarStoreDlg* pStoreDlg = (CAvatarStoreDlg*)pDlg;
							pStoreDlg->SetMasterSvrObjIdx( g_pObjMGR->Get_ServerObjectIndex(pAvt->Get_INDEX()) );
							pStoreDlg->SetTitle( pAvt->GetPersonalStoreTitle() );
						}
					}
				}

				SetCMD_STOP ();
	 		}

			return 1;

		} else 	if ( pTarget->IsA( OBJ_NPC ) ) 
		{
			/// 대상이 NPC라면
			if ( Goto_TARGET( pTarget, NPC_CLICK_EVENT_RANGE ) ) 
			{
				// 도착했다... 대화 !
				pTarget->Check_EVENT( this );
				SetCMD_STOP ();
			}

			return 1;
		}
	}

	/// m_PosGOTO로 이동..
	if ( this->Goto_POSITION () ) 
	{
		SetCMD_STOP ();
	} 

	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @brief  : 이동중 아이템이 대상일경우의 처리..
///			사실 아이템이랑 이름 붙었지만.. Command 가 CMD_PICK_ITEM 이 아니면 일반 동작.
//--------------------------------------------------------------------------------

int CObjAI::ProcCMD_PICK_ITEM ()
{
	/// m_PosGOTO로 이동..
	if ( this->Goto_POSITION() ) 
	{		
		// 도착했다.	
		if ( Get_COMMAND() == CMD_PICK_ITEM ) 
		{
			SetCMD_STOP ();

			// 아이템 갖기를 서버에 요청.
			if( g_pNet->Send_cli_GET_FIELDITEM_REQ( this, m_iServerTarget ) )
			{					
				this->Set_MOTION( this->GetANI_PickITEM() );
				this->SetMotionRepeatCount( 1 );		
			}				
		}			
	}else
	{		
		//----------------------------------------------------------------------------------------------------
		/// 아이템 줍기의경우에는 적정 거리안에 들어오면 도착하지 않아도 줍기 요청..
		//----------------------------------------------------------------------------------------------------
		const int iPickItemRange = 150;
		int iDistance = CD3DUtil::distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y);
		if ( iDistance + iPickItemRange >= m_iMoveDistance ) 
		{
			// 도착했다.	
			if ( Get_COMMAND() == CMD_PICK_ITEM ) 
			{
				SetCMD_STOP ();

				// 아이템 갖기를 서버에 요청.
				if( g_pNet->Send_cli_GET_FIELDITEM_REQ( this, m_iServerTarget ) )
				{					
					this->Set_MOTION( this->GetANI_PickITEM() );
					this->SetMotionRepeatCount( 1 );		
				}				
			}
		}
	}

	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @brief  : Attack command 처리
///
//--------------------------------------------------------------------------------

int CObjAI::ProcCMD_ATTACK ()
{	
//-------------------------------------------------------------------------------
#if defined(_GBC)
	//박지호::......
	if(GetPetMode() >= 0)
		 return 1;
#endif 
//-------------------------------------------------------------------------------

	CObjCHAR *pTarget = (CObjCHAR*)this->Get_TARGET();
	if ( pTarget ) 
	{
		if ( Get_STATE() & CS_BIT_ATTACK ) 
		{
			/// 공격중이다..
			if ( !(Get_STATE() & CS_BIT_INT) ) 
			{			
				// TODO :: 같은 존의 다른 위치로 워프 했다면...
				if ( !IsInRANGE( pTarget, this->Get_AttackRange() ) ) {
					// 범위를 벗어 났으면 이동...
					m_wState  = CS_STOP;
					m_PosGOTO = pTarget->m_PosCUR;
					this->Start_MOVE( this->Get_MoveSPEED() );
					return 1;
				}

				// 죽을때 까지 공격 !!!
				this->Start_ATTACK (pTarget);
			}

			return 1;
		} 

		/// 타겟으로 이동...
		if ( this->Goto_TARGET( pTarget, this->Get_AttackRange() ) ) 
		{
			// 도착 했으면 공격 시작 !!!
			this->Start_ATTACK (pTarget);
		} else 
		{
			if ( !(Get_STATE() & CS_BIT_MOV) ) 
			{
				this->Start_MOVE( this->Get_MoveSPEED() );
			} else
				this->Do_AttackMoveAI( pTarget );		/// MOB 공격 이동중 인공지능 처리..
		}

	} else 
	{
		/// 타겟이 없다..
		ChangeActionMode( AVATAR_NORMAL_MODE );

		/// 내 아바타일경우는 타겟이 없을경우 죽은것으로 간주.. STOP 명령을 내림
		this->Set_TargetIDX( 0 );
		m_wCommand      = CMD_STOP;

		pTarget = g_pObjMGR->Get_ClientCharOBJ( m_iServerTarget, false);
		if ( pTarget ) {
			this->m_iServerTarget = 0;
			this->SetCMD_STOP ();
		} else 
		{
			int iDistance = CD3DUtil::distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y);
			if ( iDistance < m_iMoveDistance )
			{
				// 클라이언트에서는 서버에서 받은 최종 좌표까지 이동후 명령 유지...
				if ( this->Goto_POSITION() ) 
				{					
					m_wState = CS_STOP;

					this->Set_MOTION( this->GetANI_Stop() );
					this->Set_ModelSPEED( 0.0f );
				}
			}else
			{
				m_wState = CS_STOP;

				this->Set_MOTION( this->GetANI_Stop() );
				this->Set_ModelSPEED( 0.0f );
			}
		}	
	}

	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @brief  : 셀프스킬 command 처리
///				현재 동작후 바로 처리될 스킬 --> 스톱 상태로 된다.
//--------------------------------------------------------------------------------

int CObjAI::ProcCMD_Skill2SELF ()
{
	if ( 1 != this->Do_SKILL( 0 ) ) 
	{
		;
	}
	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @brief  : 지역으로 이동후 스킬 처리 --> 스톱 상태로 된다.
///
//--------------------------------------------------------------------------------

int CObjAI::ProcCMD_Skill2POSITION ()
{
	//-----------------------------------------------------------------------------------------
	/// 아직 스킬 캐스팅을 시작하지 않았다면..
	//-----------------------------------------------------------------------------------------	
	if ( !m_bCastingSTART )
	{
		t_POSITION posTemp = this->m_PosGOTO;
		// 장소로 이동... & 스킬 적용
		if ( this->Goto_POSITION ( this->Get_AttackRange() ) == false ) 
		{
			return 1;		
		}		
		///
		/// Goto_POSITION 내부에서 레인지안으로 도착할경우 PosGOTO를 현재위치로세팅한다.
		/// 그러나 포지션 스킬의 경우 실제 이펙트가 PosGOTO에 출력되므로 리셋되면 안된다.
		///
		this->m_PosGOTO = posTemp;
	}

	/// 캐스팅 또는 실제동작 적용...
	if ( 1 != this->Do_SKILL( 0 ) ) 
	{
		// casting: 1, cancel: 0, active: 2
		///m_wCommand = CMD_STOP;
	}

	return 1;
}


//--------------------------------------------------------------------------------
/// class : CObAI
/// @brief  : 타겟으로 이동후 스킬 처리 --> 스톱 상태로 된다.
///           04/4/28 타겟에 도착했다고 캐스팅을 시작하지말고 기다리다가 서버로 부터
///				start 패킷을 받으면 캐스팅을 시작하라.
///				타겟이 없을경우.. Cast_end()만 호출한게 아니라 모든 스킬 인덱스로 리셋..
//--------------------------------------------------------------------------------

int CObjAI::ProcCMD_Skill2OBJECT ()
{
	
//---------------------------------------------------------------------------------	
#if defined(_GBC)
	//박지호::펫모드일때 펫을 이동시키고,아바타 스킬을 캐스팅한다.
	if(GetPetMode() >= 0)
		return ProcCMD_Skill2OBJECT_PET();
#endif 
//---------------------------------------------------------------------------------

	// 타켓으로 이동... & 스킬 적용
	///CObjCHAR *pTarget = (CObjCHAR*)this->Get_TARGET();
	CObjCHAR *pTarget = CSkillManager::GetSkillTarget( m_iServerTarget, (this->m_nToDoSkillIDX)? this->m_nToDoSkillIDX : this->m_nActiveSkillIDX  );
	if ( pTarget ) 
	{ 
		//-----------------------------------------------------------------------------------------
		/// 아직 스킬 캐스팅을 시작하지 않았다면..
		//-----------------------------------------------------------------------------------------	
		if ( !m_bCastingSTART )
		{
			int iAttackRange = this->Get_AttackRange();
			
			//-----------------------------------------------------------------------------------------	
			/// 캐스팅중이 아니면 타겟으로 이동...			
			//-----------------------------------------------------------------------------------------				
			if( this->Goto_TARGET( pTarget, iAttackRange ) == false )
			{
				if ( !(Get_STATE() & CS_BIT_MOV) ) 
				{
					LogString( LOG_NORMAL, "ProcCMD_Skill2Object::Start_Move[ %x ]\n", Get_STATE() );
					this->Start_MOVE( this->Get_MoveSPEED() );										
				}
				
				return 1;
			}
		
			//-----------------------------------------------------------------------------------------
			/// 서버로부터 스킬시작패킷을 못받아도 타겟으로 이동..
			//-----------------------------------------------------------------------------------------				
			if( !bCanStartSkill() ) 
			{
				m_wState  = CS_STOP;
				m_fCurMoveSpeed = 0;
				///this->Set_MOTION( this->GetANI_Casting() );
				return 1;
			}			
		}
		

		//-----------------------------------------------------------------------------------------
		/// 타겟 방향으로 몸을 돌린다.
		//-----------------------------------------------------------------------------------------			
		Set_ModelDIR( pTarget->m_PosCUR );


		//-----------------------------------------------------------------------------------------
		/// 타겟이 죽어버렸다...
		//-----------------------------------------------------------------------------------------			
		if( pTarget->m_bDead )
		{
			SetEffectedSkillFlag( true );
		}

		/// 캐스팅 또는 실제동작 적용...
		if ( 1 != this->Do_SKILL( this->Get_TargetIDX(), pTarget ) ) 
		{
			// casting: 1, cancel: 0, active: 2
			/// Do_Skill 내부에서 결정
			//m_wCommand = CMD_STOP;
			;
		} 

	} 
	else
	//-----------------------------------------------------------------------------------------
	/// 타겟이 없다. 근데.. 이미 캐스팅이 시작되었는데 중간에 죽어버리거나 한다면?
	//-----------------------------------------------------------------------------------------			
	{
		/// 내 아바타일경우는 타겟이 없을경우 죽은것으로 간주.. STOP 명령을 내림
		this->Set_TargetIDX( 0 );
		m_wCommand      = CMD_STOP;

		pTarget = g_pObjMGR->Get_ClientCharOBJ( m_iServerTarget, false);


		//-----------------------------------------------------------------------------------------
		/// 타겟이 없다면 스킬에 대한 설정을 무조건 리셋한다.
		//-----------------------------------------------------------------------------------------
		SetEffectedSkillFlag( true );
		m_nActiveSkillIDX = 0;
		Casting_END();		

		//-----------------------------------------------------------------------------------------
		/// 죽어있는 시체도 없다.
		//-----------------------------------------------------------------------------------------
		if ( !pTarget ) 
		{
			int iDistance = CD3DUtil::distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y);
			if ( iDistance < m_iMoveDistance )
			{
				//-----------------------------------------------------------------------------------------
				/// 클라이언트에서는 서버에서 받은 최종 좌표까지 이동후 명령 유지...
				/// 이거 이상하다.. 
				//-----------------------------------------------------------------------------------------
				if ( this->Goto_POSITION() ) 
				{					
					m_wState = CS_STOP;

					this->Set_MOTION( this->GetANI_Stop() );
					this->Set_ModelSPEED( 0.0f );
				}else
					return 1;
			}
		}	
		
		SetCMD_STOP();		
	}

	return 1;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 명령상태에 따른 회복 수치를 구한다.
///			- 수정 : 계산식에서 최대HP의 값이 모든 최대 MP -> 순수 최대 HP( 버프, 패시브등을 뺀)로 수정		2005/7/13 - nAvy
//----------------------------------------------------------------------------------------------------
short CObjAI::Get_RecoverHP( short nRecoverMODE )	
{
	short nRecoverHP = 0;

	switch ( Get_COMMAND() ) 
	{
		case CMD_SIT :
			{
				if( CCountry::GetSingleton().IsApplyNewVersion() )
					//nRecoverHP = this->GetAdd_RecoverHP() + this->Get_MaxHP() / 12.f + 1;
					nRecoverHP = this->GetAdd_RecoverHP() + this->GetOri_MaxHP() / 12.f + 1;
					
				else
					nRecoverHP = this->GetAdd_RecoverHP() + ( this->Get_CON()+30 ) / 8 * (nRecoverMODE+3) / 10;	
			}
			break;
		default:
			{
				if( CCountry::GetSingleton().IsApplyNewVersion() )
					//nRecoverHP = this->GetAdd_RecoverHP() + this->Get_MaxHP() / 50 + 1;
					nRecoverHP = this->GetAdd_RecoverHP() + this->GetOri_MaxHP() / 50 + 1;
				else
					nRecoverHP = ( this->GetAdd_RecoverHP() + ( this->Get_CON()+40 ) / 6 ) / 6;	
			}
			break;	
	}
	return nRecoverHP;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 명령상태에 따른 마나 회복 수치를 구한다.
///			- 수정 : 계산식에서 최대MP의 값이 모든 최대 MP -> 순수 최대 MP( 버프, 패시브등을 뺀)로 수정		2005/7/13 - nAvy
//----------------------------------------------------------------------------------------------------
short CObjAI::Get_RecoverMP( short nRecoverMODE )	
{	
	short nRecoverMP = 0;
	switch( Get_COMMAND() )
	{
	case CMD_SIT:
		if( CCountry::GetSingleton().IsApplyNewVersion() )
			//nRecoverMP = this->GetAdd_RecoverMP() + this->Get_MaxMP() / 12.f + 1;
			nRecoverMP = this->GetAdd_RecoverMP() + this->GetOri_MaxMP() / 12.f + 1;
		else
			nRecoverMP = ( this->GetAdd_RecoverMP() + ( this->Get_CON()+20 ) / 10 * nRecoverMODE / 7 );
		break;
	default:
		if( CCountry::GetSingleton().IsApplyNewVersion() )
		{
			//nRecoverMP = this->GetAdd_RecoverMP() + this->Get_MaxMP() / 35.f + 1;
			nRecoverMP = this->GetAdd_RecoverMP() + this->GetOri_MaxMP() / 35.f + 1;
		}
		break;
	}

	return nRecoverMP;
}
//----------------------------------------------------------------------------------------------------
/// @brief 버프나 패시브 스킬등으로 추가되기전의 순수 MaxHP구하는 메쏘드 : 2005/7/13 - nAvy
//----------------------------------------------------------------------------------------------------
int CObjAI::GetOri_MaxHP()
{
	_RPT0 ( _CRT_WARN, "GetOri_MaxHP() 호출시에 문제 생길수 있다" );
	return 0;
}
//----------------------------------------------------------------------------------------------------
/// @brief 버프나 패시브 스킬등으로 추가되기전의 순수 MaxMP구하는 메쏘드 : 2005/7/13 - nAvy
//----------------------------------------------------------------------------------------------------
int CObjAI::GetOri_MaxMP()
{
	_RPT0 ( _CRT_WARN, "GetOri_MaxMP() 호출시에 문제 생길수 있다" );
	return 0;
}