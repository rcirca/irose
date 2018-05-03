/*
	** 주의 사항 !!!
	common/*.cpp, *.h 파일들은 서버와 소스를 공유함으로
	클라이언트만 생각해서 고치면 절대로 안됨 !!!! ***

	$Header: /7HeartsOnline/Server/SHO_GS/Sho_gs_lib/Common/CObjAI.cpp 44    05-10-06 10:21p Maior $
*/
#include "stdAFX.h"
#include "CObjCHAR.h"
#include "OBJECT.h"
#ifndef	__SERVER
#include "..\Network\CNetwork.h"
#include "Skill.h"
#include "it_mgr.h"
#endif

//-------------------------------------------------------------------------------------------------
/// Target Object 를 구한다.( HP 체크 )
CAI_OBJ *CObjTARGET::Get_TargetOBJ(bool bCheckHP)
{	
	if ( m_iServerTarget ) {
		CObjCHAR *pTarget = g_pObjMGR->Get_ClientCharOBJ( m_iServerTarget, bCheckHP /*true*/ );
		if ( pTarget ) {
			if ( pTarget->GetZONE() == this->GetCur_ZONE() )
				return pTarget;
		}
		m_iServerTarget = 0;
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------
CObjAI::CObjAI ()
{
	m_pCurMOTION = NULL;
	m_iCurMotionFRAME = 0, m_fAccMotionFRAME = 0.f;

    m_wState        = 0;
    m_wCommand      = 0;

	m_bAttackSTART  = false;
	m_iActiveObject = 0;

	m_bCastingSTART	= false;

	m_bRunMODE		= false;	// 기본은 걷기 모드...

	m_fRunAniSPEED  = 1.0f;
	m_fCurMoveSpeed = 0;

	m_nToDoSkillIDX = 0;
	m_nActiveSkillIDX = 0;
	m_nDoingSkillIDX = 0;

	m_iWaitLoopCnt = 0;
}
CObjAI::~CObjAI ()
{
}

//-------------------------------------------------------------------------------------------------
/// 공식이 바뀔경우 서버랑 맞추기 위해 이곳으로 옮김... 2004. 2. 6
/// 무기 번호로 공격속도를 계산 : 몹/NPC제외 : 아바타/유저만 해당.
float CObjAI::Cal_AtkAniSPEED( short nRightWeaponItemNO )
{
	/// 100을 기준으로 계산됨.
	short nWeaponSpeed = WEAPON_ATTACK_SPEED( nRightWeaponItemNO ) + 5;
	float fSpeed = this->GetPsv_ATKSPEED() + ( 1500.f / nWeaponSpeed );
	return fSpeed;
}

//-------------------------------------------------------------------------------------------------
/// 새로운 모션을 현재 모션으로 세팅, 프레임 리셋.
bool CObjAI::Set_CurMOTION( tagMOTION* pMotion )
{
	m_pCurMOTION = pMotion;
	m_iCurMotionFRAME = 0, m_fAccMotionFRAME = 0.f;

	return ( NULL != m_pCurMOTION );
}

//-------------------------------------------------------------------------------------------------
/// 현재 모션을 바꾼다.( 이전 모션이랑 같으면 그냥 패스 )
bool CObjAI::Chg_CurMOTION( tagMOTION* pMotion )
{
	if ( pMotion && m_pCurMOTION != pMotion ) {
		m_pCurMOTION = pMotion;
		m_iCurMotionFRAME = 0, m_fAccMotionFRAME = 0.f;
		return true;
	}

	return false;
}


//-------------------------------------------------------------------------------------------------
/// 공격시작.
void CObjAI::Start_ATTACK (CObjCHAR *pTarget)
{
	/// 일반 공격시작 상태 설정.
	m_wState = CS_ATTACK;
	m_iActiveObject = g_pObjMGR->Get_ClientObjectIndex( Get_TargetIDX() );

	// if ( Attack_START( pTarget ) )
	{
		// 공격 애니메이션 & 속도 설정. :: 크리티컬 공격일 경우 모션을 별도로 존재하지 않고 효과로 출력한다.
		if ( this->Set_MOTION( this->GetANI_Attack(), 0, ( this->Get_nAttackSPEED() / 100.f ), true ) ) {
			this->Attack_START( pTarget );

			#if defined(_DEBUG) && !defined(__SERVER)
				if ( m_pCurMOTION->m_nActionPointCNT <= 0 ) {
					char *szMsg = CStr::Printf("%s need attack point frame!!!", Get_NAME());
					g_pCApp->ErrorBOX( szMsg, "ERROR" );
					LogString (LOG_NORMAL, szMsg);
				}
			#endif
		}
	}
}


//-------------------------------------------------------------------------------------------------
/// casting
char CObjAI::Do_SKILL (int iServerTarget, CObjCHAR *pTarget)
{
#ifdef	__INC_WORLD
	if ( this->IsUSER() ) {
		LogString( 0xffff, "%s( %.0f/%.0f ) DO_Skill: bCast:%d / ToDo:%d / Active%d, CMD:%x, STATE:%x \n", 
				this->Get_NAME(),
				this->m_PosCUR.x, this->m_PosCUR.y,
				m_bCastingSTART, m_nToDoSkillIDX, m_nActiveSkillIDX,
				m_wCommand, m_wState);
	}
#endif

	if ( !m_bCastingSTART && m_nToDoSkillIDX ) 
	{
		if ( !this->SkillChk_ACTIVE( m_nToDoSkillIDX ) ) 
		{
			// 필요 능력치 부족...
			return 2;	// 스킬 완료된걸로 리턴 다음 동작 진행하도록...
		}

		// 실제 필요 수치 소모 적용...
		this->SkillUse_ABILITY( m_nToDoSkillIDX );

		Casting_START( NULL );

		m_iActiveObject   = g_pObjMGR->Get_ClientObjectIndex( iServerTarget );
		m_nActiveSkillIDX = m_nToDoSkillIDX;
		m_nToDoSkillIDX   = 0;

		if ( this->GetANI_Casting() ) {
			/// 스킬 주문 동작 !!!
			/// Casting 시작 상태 설정.
			/// 스킬 동작중에는 맞는 동작도 적용 안된다.

			this->m_wState = CS_CASTING;
			this->Set_MOTION( this->GetANI_Casting(), 0, g_SkillList.Get_CastingAniSPEED( m_nActiveSkillIDX ) );
			return 1;
		}
	}

	if ( m_nActiveSkillIDX ) 
	{
		/// 캐스팅 완료된 스킬의 실제 동작 모션...
		Casting_END ();

		if ( 1 == Get_PatHP_MODE() ) {
			Set_PatHP_MODE( 2 );
		}

		/// 스킬 동작중에는 맞는 동작도 적용 안된다.
		this->Set_MOTION( this->GetANI_Skill(), 0, g_SkillList.Get_ActionAniSPEED( m_nActiveSkillIDX ), true );
		if ( this->Skill_START( pTarget ) ) {
			switch( SKILL_ACTION_MODE( m_nActiveSkillIDX ) ) {
				case SA_ATTACK  :	// 무조건 공격으로( 단 타겟이 존제해야...)
					m_wCommand = CMD_ATTACK;
					m_wState   = CS_ATTACK;
					break;

				case SA_RESTORE :	// 이전 명령이 스킬이면 정지로
					m_wState = CS_NEXT_STOP2;

					switch( m_wBeforeCMD ) {
						case CMD_SKILL2SELF	:
						case CMD_SKILL2OBJ	:
						case CMD_SKILL2POS	:
							m_wCommand = CMD_STOP;
							break;
						default :
							m_wCommand = m_wBeforeCMD;
					}
					break;

				default :			// 무조건 stop으로...
					m_wCommand = CMD_STOP;
					m_wState   = CS_NEXT_STOP2;				
			}
			this->Del_ActiveSKILL ();
			return 1;
/*
			if ( SA_ATTACK == SKILL_ACTION_MODE( m_nActiveSkillIDX ) ) {
				// 공격명령을 이어서할 스킬... 공격중/ 공격명령으로 설정...
				m_wCommand = CMD_ATTACK;
				m_wState   = CS_ATTACK;

				Del_ActiveSKILL ();
				return 1;
			} else {
				m_wState = CS_NEXT_STOP2;
				Del_ActiveSKILL ();

				return 2;
			}
*/
		}
		
		Del_ActiveSKILL ();
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
/// Move vector reset
/// 1. Normalize DirVEC, 2. DirVEC * Move Speed
/// 몬스터 중에는 이동속도 0인게 있음..
void CObjAI::Reset_MoveVEC ()
{
	tPOINTF DirVEC;

#ifndef	__SERVER
	this->Set_ModelDIR ( m_PosGOTO );
	this->Set_ModelSPEED( m_fCurMoveSpeed );
	DirVEC.x = m_PosGOTO.x - m_PosCUR.x;
	DirVEC.y = m_PosGOTO.y - m_PosCUR.y;
#else
	DirVEC  = m_PosGOTO - m_PosCUR;
#endif
	float fLength = DirVEC.Length () * 1000.f;
	if ( m_fCurMoveSpeed > 0 && fLength != 0.0f ) 
	{
		m_MoveVEC = ( DirVEC * m_fCurMoveSpeed ) / fLength; // Normalized dirvec * move speed
		assert( m_MoveVEC.x != 0 || m_MoveVEC.y != 0 );
	} else {
		// 이동할 필요가 없는데...
		m_MoveVEC.x = 0;
		m_MoveVEC.y = 0;
	}
}

///
/// Start move
/// 현재 이동중이면 거리 계산하고, MoveVEC 만 리셋.
/// 이동중이 아니라면 모션 세팅
void CObjAI::Start_MOVE( float fSpeed )
{
	m_fCurMoveSpeed = fSpeed;
	m_PosMoveSTART  = m_PosCUR;

	//LogString( LOG_NORMAL, "Start_MOVE( %s:%.0f ) , (%.0f,%.0f) => (%.0f,%.0f) \n", this->Get_NAME(), m_fCurMoveSpeed, m_PosCUR.x, m_PosCUR.y, m_PosGOTO.x, m_PosGOTO.y);

	m_iMoveDistance = distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)m_PosGOTO.x, (int)m_PosGOTO.y);
	this->Reset_MoveVEC ();
	if ( Get_STATE() != CS_MOVE ) {
		// 이동중이 아니다.
		m_wState  = CS_MOVE;
		this->Set_MOTION( this->GetANI_Move(), m_fCurMoveSpeed, this->Get_MoveAniSPEED() );
	}
}

/// Restart move
/// 최종좌표가 바뀌지 않았다면 그대로 고.
/// 최종좌표가 수정되었다면, 다시 스타트.
void CObjAI::Restart_MOVE (t_POSITION &PosGOTO)
{
	if ( Get_STATE() == CS_MOVE && PosGOTO.x == m_PosGOTO.x && PosGOTO.y == m_PosGOTO.y ) {
		// 최종 좌표가 바뀌지 않았다.
#ifdef	__INC_WORLD
		if ( this->IsUSER() ) {
			LogString( 0xffff, ">>SKIP Restart_MOVE ( %s ): Dist:%.0f   Pos( %.0f,%.0f )\n",
				this->Get_NAME(),
				::distance_square( m_PosCUR, m_PosGOTO ),
				this->m_PosGOTO.x, this->m_PosGOTO.y );
		}
#endif
		return;
	}

	m_PosGOTO = PosGOTO;

	this->Start_MOVE( this->Get_MoveSPEED() );
}


//-------------------------------------------------------------------------------------------------
bool CObjAI::SetCMD_SIT (void)
{
#ifdef	__BLOCK_WHEN_SKILL
	if ( m_nActiveSkillIDX ) {
		return false;
	}
#endif

	m_wState		= CS_SITTING;
	m_wCommand		= CMD_SIT;
	m_fCurMoveSpeed = 0;

	this->Set_MOTION( this->GetANI_Sitting() );
	return true;
}
bool CObjAI::SetCMD_STAND (void)
{
#ifdef	__BLOCK_WHEN_SKILL
	if ( m_nActiveSkillIDX ) {
		return false;
	}
#endif

	m_wState		= CS_STANDING;
	m_wCommand		= CMD_STOP;
	m_fCurMoveSpeed = 0;

	this->Set_MOTION( this->GetANI_Standing() );
	return true;
}

//-------------------------------------------------------------------------------------------------
/// Stop command 세팅.
/// I must set "m_iTargetObject" to 0??? if My state is STOP
bool CObjAI::SetCMD_STOP (void)
{
#ifdef	__BLOCK_WHEN_SKILL
	if ( m_nActiveSkillIDX ) {
		return false;
	}
#endif

	m_wState		= CS_STOP;
	m_wCommand		= CMD_STOP;
	m_fCurMoveSpeed = 0;

#ifdef	__SERVER
	this->Set_TargetIDX( 0 );
#endif

	this->Set_MOTION( this->GetANI_Stop() );
	return true;
}

//-------------------------------------------------------------------------------------------------
/// Move command 세팅.
/// SetCMD_MOVE (tPOINTF &PosCUR, tPOINTF &PosTO, int iTargetObjIDX) 호출
bool CObjAI::SetCMD_MOVE2D (float fPosX, float fPosY, BYTE btRunMODE)
{
#ifdef	__BLOCK_WHEN_SKILL
	if ( m_nActiveSkillIDX ) {
		return false;
	}
#endif
	tPOINTF PosGOTO;

	PosGOTO.x = fPosX;
	PosGOTO.y = fPosY;

	this->m_bRunMODE = ( btRunMODE != 0 );
	CObjAI::SetCMD_MOVE ((tPOINTF&)this->m_PosCUR, PosGOTO, 0);
	return true;
}

/// Move command 세팅.
bool CObjAI::SetCMD_MOVE (tPOINTF &PosFROM, tPOINTF &PosTO, int iServerTarget)
{
#ifdef	__BLOCK_WHEN_SKILL
	if ( m_nActiveSkillIDX ) {
		return false;
	}
#endif
	//if ( this->Get_WeightRATE() >= WEIGHT_RATE_STOP ) {
	//	// 무겁다.. 명령 불가...
	//	return false;
	//}

	m_wCommand = CMD_MOVE;

	CGameOBJ *pDestOBJ = g_pObjMGR->Get_ClientOBJECT( iServerTarget );
	/// 타겟 오브젝트가 있다면. 타겟 오브젝트로 이동.
	if ( pDestOBJ ) 
	{
		/// 대상이 아이템이라면 CMD_PICK_ITEM 명령.
		if ( pDestOBJ->Get_TYPE() == OBJ_ITEM )
			m_wCommand = CMD_PICK_ITEM;

		m_PosGOTO = pDestOBJ->m_PosCUR;
		this->Set_TargetIDX( iServerTarget );
	} else {
		// m_pRecvPacket->m_gsv_MOUSECMD.m_PosCUR;
		m_PosGOTO.x = PosTO.x;
		m_PosGOTO.y = PosTO.y;
		this->Set_TargetIDX( 0 );
	}

	if ( CS_BIT_INT & this->Get_STATE() ) {
		if ( ( CS_BIT_MOV | CS_BIT_MOV2 ) & this->Get_STATE() )
			m_wState = CS_NEXT_STOP | CS_BIT_MOV2;
		else
			m_wState = CS_NEXT_STOP;
	} else {
		this->Start_MOVE( this->Get_MoveSPEED() );
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
/// Attack command 세팅.
/// 타겟이 없는 경우는?( 죽었나? )
bool CObjAI::SetCMD_ATTACK (int iServerTarget)
{
#ifdef	__BLOCK_WHEN_SKILL
	if ( m_nActiveSkillIDX ) {
		return false;
	}
#endif
	//if ( this->Get_WeightRATE() >= WEIGHT_RATE_STOP ) {
	//	// 무겁다.. 명령 불가...
	//	return false;
	//}

	CObjCHAR *pDestCHAR = g_pObjMGR->Get_ClientCharOBJ( iServerTarget, true );
	if ( pDestCHAR ) {
		// 투명 상태면...
		if ( pDestCHAR->m_IngSTATUS.IsSET( FLAG_ING_DISGUISE | FLAG_ING_TRANSPARENT ) )
			return false;

		if ( CMD_ATTACK == m_wCommand && this->Get_TargetIDX() == iServerTarget ) {
			// 변화 없다..
			return false;
		}

		m_wCommand = CMD_ATTACK;
		m_PosGOTO  = pDestCHAR->m_PosCUR;

		this->Set_TargetIDX( iServerTarget );

		if ( ( CS_BIT_MOV | CS_BIT_MOV2 ) & this->Get_STATE() ) {	// 이동중...
			m_wState = CS_BIT_MOV2 | ( ( CS_BIT_INT & this->Get_STATE() ) ? CS_NEXT_STOP : CS_STOP );
		} else {
			m_wState = ( CS_BIT_INT & this->Get_STATE() ) ? CS_NEXT_STOP : CS_STOP;
		}
	} else {
		this->Set_TargetIDX( 0 );

		if ( CMD_STOP == m_wCommand && 0 == this->Get_TargetIDX() ) {
			// 변화 없다..
			return false;
		}
		m_wCommand = CMD_STOP;
		m_wState =  ( CS_BIT_INT & this->Get_STATE() ) ? CS_NEXT_STOP : CS_STOP;
	}

	/// 상태에 변화가 생겼다.
	return true;
}

//-------------------------------------------------------------------------------------------------
/// Die command 세팅.
void CObjAI::SetCMD_DIE ()
{
	this->Set_TargetIDX( 0 );

	m_wState   = CS_DIE;
	m_wCommand = CMD_DIE;

	this->Del_ActiveSKILL ();
	this->Set_MOTION( this->GetANI_Die() );
}


//-------------------------------------------------------------------------------------------------
bool CObjAI::SetCMD_Skill2SELF (short nSkillIDX)
{
#ifdef	__BLOCK_WHEN_SKILL
	if ( m_nActiveSkillIDX ) {
		return false;
	}
#endif

	if ( CS_BIT_INT & this->m_wState )
		m_wState = CS_NEXT_STOP;
	else
		m_wState = CS_STOP;

	m_nToDoSkillIDX = nSkillIDX;
/*
	switch( m_wCommand ) {
		case CMD_SKILL2SELF	:
		case CMD_SKILL2OBJ	:
		case CMD_SKILL2POS	:
			m_wBeforeCMD = CMD_STOP;	// 스킬일 경우 짱박지 않는다.
			break;
		default :
			m_wBeforeCMD = m_wCommand;
	} 
*/
	m_wBeforeCMD = m_wCommand;
	m_wCommand = CMD_SKILL2SELF;

	return true;
}

bool CObjAI::SetCMD_Skill2OBJ (int iServerTarget, short nSkillIDX)
{
#ifdef	__BLOCK_WHEN_SKILL
	if ( m_nActiveSkillIDX ) {
		return false;
	}
#endif
	if ( nSkillIDX == m_nToDoSkillIDX && CMD_SKILL2OBJ == m_wCommand && iServerTarget == this->Get_TargetIDX() )
		return false;
	
	//if ( this->Get_WeightRATE() >= WEIGHT_RATE_STOP ) {
	//	// 무겁다.. 명령 불가...
	//	return false;
	//}

	CObjCHAR *pDestCHAR = g_pObjMGR->Get_ClientCharOBJ( iServerTarget, false /*true*/ );
	if ( pDestCHAR ) {
		bool bCheckHP;
		// 리져렉션일 경우
		if ( SKILL_TARGET_FILTER_DEAD_USER == SKILL_CLASS_FILTER( nSkillIDX ) ) {
			// 사용자이고 죽은넘.
			if ( !pDestCHAR->IsUSER() || pDestCHAR->Get_HP() > 0 )
				return false;
			bCheckHP = false;
		} else {
			if ( pDestCHAR->Get_HP() <= 0 )
				return false;

			// 투명 상태면...
			if ( pDestCHAR->m_IngSTATUS.IsSET( FLAG_ING_DISGUISE | FLAG_ING_TRANSPARENT ) && !this->Is_ALLIED(pDestCHAR) )
				return false;

			bCheckHP = true;
		}

#ifdef	__INC_WORLD
		if ( this->IsUSER() ) {
			LogString( 0xffff, ">>>> SetCMD_Skill2OBJ %d :: %.0f, %.0f \n", nSkillIDX, m_PosCUR.x, m_PosCUR.y );
		}
#endif
		
		if ( ( CS_BIT_MOV | CS_BIT_MOV2 ) & this->Get_STATE() ) {
			m_wState = CS_BIT_MOV2 | ( ( CS_BIT_INT & this->Get_STATE() ) ? CS_NEXT_STOP: CS_STOP );
		} else {
			m_wState = ( CS_BIT_INT & this->Get_STATE() ) ? CS_NEXT_STOP : CS_STOP;
		}

		m_wBeforeCMD	= m_wCommand;
		m_wCommand		= CMD_SKILL2OBJ;
		m_nToDoSkillIDX = nSkillIDX;
		m_PosGOTO		= pDestCHAR->m_PosCUR;	// ?????

		this->Set_TargetIDX( iServerTarget, bCheckHP );

		return true;
	} 

	return false;
}

bool CObjAI::SetCMD_Skill2POS (tPOINTF &PosGOTO, short nSkillIDX)
{
#ifdef	__BLOCK_WHEN_SKILL
	if ( m_nActiveSkillIDX ) {
		return false;
	}
#endif

	//if ( this->Get_WeightRATE() >= WEIGHT_RATE_STOP ) {
	//	// 무겁다.. 명령 불가... 스킬 가능 거리가 ???
	//	float fDist = ::distance_square( this->m_PosCUR, PosGOTO );
	//	if ( fDist > SKILL_DISTANCE( nSkillIDX ) ) 
	//		return false;
	//}

	if ( ( CS_BIT_MOV | CS_BIT_MOV2 ) & this->Get_STATE() ) {
		m_wState = CS_BIT_MOV2 | ( ( CS_BIT_INT & this->Get_STATE() ) ? CS_NEXT_STOP : CS_STOP );
	} else {
		m_wState = ( CS_BIT_INT & this->Get_STATE() ) ? CS_NEXT_STOP : CS_STOP;
	}

	m_nToDoSkillIDX = nSkillIDX;

	m_wBeforeCMD	= m_wCommand;
	m_wCommand		= CMD_SKILL2POS;
	m_PosGOTO.x		= PosGOTO.x;
	m_PosGOTO.y		= PosGOTO.y;
	this->Set_TargetIDX( 0 );
	return true;
}

//-------------------------------------------------------------------------------------------------
/// 애니매이션 진행..
/// @bug -->안에서 AI Action에 의해 m_pCurMOTION이 바뀌어 오는 경우가 생겨 뻑~~~~~~~
// 이전 호출되었던 부분에서 경과된 시간만큼 모션 프레임을 진행.
bool CObjAI::ProcMotionFrame (void)
{
	int iFrame = this->GetCurrentFrame();
	if ( iFrame < 0 ) 
	{
		m_iCurMotionFRAME = 0, m_fAccMotionFRAME = 0.f;

		m_wState &= ~CS_BIT_INT;
		return false;
	}

#ifdef	__INC_WORLD
	assert( iFrame <= m_pCurMOTION->m_wTotalFrame );
	assert( m_iCurMotionFRAME >= 0 );
#endif

	/// 동작프레임을 체크 해야된다면..
	if ( m_wState & CS_BIT_CHK ) 
	{
		/// 한번만 체크
		if ( m_wState & CS_BIT_ONE ) 
		{
			/// iFrame < m_iCurMotionFRAME 애니매이션이 한바귀 돌아 iFrame이 리셋됬을경우지
			if ( iFrame < m_iCurMotionFRAME || m_iCurMotionFRAME >= m_pCurMOTION->m_wTotalFrame ) 
			{
				// 모션이 완료 됐다.
				m_iCurMotionFRAME = 0, m_fAccMotionFRAME = 0.f;
				m_wState &= ~CS_BIT_INT;
				return false;
			}
		}
	}

	/// @todo m_iCurMotionFRAME = iFrame;	  이걸 넣었는데 어떨지..
	/// 모션이 끝났다.
	/// iFrame < m_iCurMotionFRAME 애니매이션이 한바귀 돌아 iFrame이 리셋됬을경우지
	if ( iFrame < m_iCurMotionFRAME || m_iCurMotionFRAME >= m_pCurMOTION->m_wTotalFrame ) 
	{
		m_iCurMotionFRAME = iFrame;	
		return false;
	}

	m_iCurMotionFRAME = iFrame;	

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjAI::Goto_TARGET( CObjCHAR *pTarget, int iRange )
{
/*
#ifdef	__INC_WORLD
	if ( this->IsUSER() ) {
		LogString( 0xffff, ">>Goto_TARGET( %s -> %s ): Dist:%.0f   Pos( %.0f,%.0f )=> ( %.0f,%.0f )\n",
			this->Get_NAME(), pTarget->Get_NAME(),
			::distance_square( m_PosCUR, pTarget->m_PosCUR ),
			this->m_PosCUR.x, this->m_PosCUR.y,
			pTarget->m_PosCUR.x, pTarget->m_PosCUR.y );
	}
#endif
*/

	if ( IsInRANGE( pTarget, iRange ) ) {
		/// 거리안에 들어 왔다...
		this->m_PosGOTO = this->m_PosCUR;
#ifndef	__SERVER
		this->Move_COMPLETED ();
#endif
		return true;
	}

	// 최종 위치가 바뀌었으면 이동 벡터 다시 계산...
	// 이동중이 아닌 상태면...이동하지 않는다.
	this->Restart_MOVE( pTarget->m_PosCUR );

	return false;
}

bool CObjAI::Goto_POSITION (int iRange)
{
	if ( !(Get_STATE() & CS_BIT_MOV) )
	{
		// 이동중이 아니면 이동 시작...
		this->Start_MOVE( this->Get_MoveSPEED() );
	}

	int iDistance = distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y);

/*
#ifdef	__INC_WORLD
	if ( this->IsUSER() ) {
		int iDist2 = distance ((int)m_PosMoveSTART.x, (int)m_PosMoveSTART.y, (int)m_PosGOTO.x, (int)m_PosGOTO.y);

		LogString( 0xffff, ">>Goto_POSITION( %s ): Dist:(%d+%d)%d / %d, Start( %.0f,%.0f ), Cur( %.0f,%.0f ), GoTo( %.0f,%.0f ),,, Start->Goto:%d\n",
			this->Get_NAME(),
			iDistance, iRange, iDistance + iRange, m_iMoveDistance,
			this->m_PosMoveSTART.x, this->m_PosMoveSTART.y,
			this->m_PosCUR.x, this->m_PosCUR.y,
			this->m_PosGOTO.x, this->m_PosGOTO.y,
			iDist2 );
	}
#endif
*/

	if ( iDistance+iRange >= m_iMoveDistance ) {
#ifdef	__INC_WORLD
		if ( this->IsUSER() ) {
			int iTemp=0;
		}
#endif
		/// 도착 
		/// this->Move_COMPLETED ();  iRange값에 의해 정지 될경우 호출되어 좌표 오류가 발생했었다...
		return true;
	} 

	return false;
}

//-------------------------------------------------------------------------------------------------
int CObjAI::ProcCMD_SIT ()
{
	if ( this->Get_STATE() != CS_SIT ) {
		this->Set_STATE( CS_SIT );
		this->Set_MOTION( this->GetANI_Sit() );
	} 

	return 1;
}

//-------------------------------------------------------------------------------------------------
/// Stop command
/// 서버와 연결상태에서는 Do_StopAI 호출은 무의미 하다..
int CObjAI::ProcCMD_STOP ()
{
	if ( Get_STATE() != CS_STOP ) 
	{
		CObjAI::SetCMD_STOP ();
	} 

	this->Do_StopAI ();
	return 1;
}

//-------------------------------------------------------------------------------------------------
/// 이동 프로세스..
/// 이동중 대상이 NPC이거나, ITEM 일 경우 처리..
int CObjAI::ProcCMD_MOVE ()
{
	CObjCHAR *pTarget = (CObjCHAR*)this->Get_TARGET();

#ifdef	__INC_WORLD
	if ( this->IsUSER() ) {
		int i=0;
	}
#endif

	if ( pTarget )
	{
		if( pTarget->Is_AVATAR() ) 
		{
			/// 대상이 사용자라면
			if ( Goto_TARGET( pTarget, AVT_CLICK_EVENT_RANGE ) ) 
			{
				CObjAI::SetCMD_STOP ();
	 		}

			return 1;
		} else 
		if ( pTarget->IsA( OBJ_NPC ) ) 
		{
			/// 대상이 NPC라면
			if ( Goto_TARGET( pTarget, NPC_CLICK_EVENT_RANGE ) ) 
			{
				// 도착했다... 대화 !
				pTarget->Check_EVENT( this );
				CObjAI::SetCMD_STOP ();
			}

			return 1;
		}
	}

	/// m_PosGOTO로 이동..
	if ( this->Goto_POSITION () ) {
		CObjAI::SetCMD_STOP ();
	} 

	return 1;
}


//-------------------------------------------------------------------------------------------------
/// 이동중 아이템이 대상일경우의 처리..
/// 사실 아이템이랑 이름 붙었지만.. Command 가 CMD_PICK_ITEM 이 아니면 일반 동작.
int CObjAI::ProcCMD_PICK_ITEM ()
{
	/// m_PosGOTO로 이동..
	if ( this->Goto_POSITION( ITEM_CLICK_EVENT_RANGE ) ) 
	{
		CObjAI::SetCMD_STOP ();
	} 

	return 1;
}

//-------------------------------------------------------------------------------------------------
///
int CObjAI::ProcCMD_ATTACK ()
{	
	CObjCHAR *pTarget = (CObjCHAR*)this->Get_TARGET();
	if ( pTarget ) {
		if ( Get_STATE() & CS_BIT_ATTACK ) {
			/// 공격중이다..
			if ( !(Get_STATE() & CS_BIT_INT) ) {			
				if ( !IsInRANGE( pTarget, this->Get_AttackRange( 0 ) ) ) {
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
		if ( Goto_TARGET( pTarget, this->Get_AttackRange( 0 ) ) ) {
			// 도착 했으면 공격 시작 !!!
			this->Start_ATTACK (pTarget);
		} else {
			if ( !(Get_STATE() & CS_BIT_MOV) ) {
				this->Start_MOVE( this->Get_MoveSPEED() );
			} else
				this->Do_AttackMoveAI( pTarget );		/// MOB 공격 이동중 인공지능 처리..
		}
	} else {
		/// 타겟이 없다 :: 공격 변경형 스킬였다면 ???
		if ( Get_ActiveSKILL() )
			Del_ActiveSKILL ();

		this->Set_TargetIDX( 0 );
		m_wCommand = CMD_STOP;
	}

	return 1;
}


//-------------------------------------------------------------------------------------------------
/// 현재 동작후 바로 처리될 스킬 --> 스톱 상태로 된다.
int CObjAI::ProcCMD_Skill2SELF ()
{
	if ( 1 != this->Do_SKILL( 0 ) ) 
	{
		// casting: 1, cancel: 0, active: 2
		// m_wCommand = CMD_STOP;
		m_wCommand = m_wBeforeCMD;
	}
	return 1;
}

//-------------------------------------------------------------------------------------------------
/// 지역으로 이동후 스킬 처리 --> 스톱 상태로 된다.
int CObjAI::ProcCMD_Skill2POSITION ()
{
	// 장소로 이동... & 스킬 적용
	if ( !m_bCastingSTART ) {
		if ( !this->Goto_POSITION ( this->Get_AttackRange( m_nToDoSkillIDX ) ) ) 
			return 1;
	}

	/// 캐스팅 또는 실제동작 적용...
	if ( 1 != this->Do_SKILL( 0 ) ) 
	{
		// casting: 1, cancel: 0, active: 2
		m_wCommand = CMD_STOP;
	}

	return 1;
}

//-------------------------------------------------------------------------------------------------
/// 타겟으로 이동후 스킬 처리 --> 스톱 상태로 된다.
int CObjAI::ProcCMD_Skill2OBJECT ()
{
	// 타켓으로 이동... & 스킬 적용
	CObjCHAR *pTarget = (CObjCHAR*)this->Get_TARGET( false /* true */ );
	if ( pTarget ) 
	{
		if ( !m_bCastingSTART )
		{
			/// 캐스팅중이 아니면 타겟으로 이동...
			if( !Goto_TARGET( pTarget, this->Get_AttackRange( m_nToDoSkillIDX ) ) ) {
				if ( !(Get_STATE() & CS_BIT_MOV) ) {
					// 이동중이 아니면..
					this->Start_MOVE( this->Get_MoveSPEED() );
				}
				return 1;
			}
			/// 이동이 완료 됐으면....
			/// Cool Time 적용 ///권형근

		}

		/// 캐스팅 또는 실제동작 적용...
		switch ( this->Do_SKILL( this->Get_TargetIDX(), pTarget ) ) {
			case 2 :	// 능력치 부족..
				this->Send_gsv_SKILL_CANCEL( SKILL_CANCEL_NEED_ABILITY );
			case 0 :
				// casting: 1, cancel: 0, active: 2
				m_wCommand = CMD_STOP;
				break;
		}
	} else {
		// 타겟 삭제되어 스킬 취소...
		if ( this->m_nActiveSkillIDX ) {
			// 케스팅 시작해서 MP가 소모됐다...
			this->Send_gsv_SKILL_CANCEL( SKILL_CANCEL_TARGET_NOT_FOUND );
		} else {
			this->Send_gsv_SKILL_CANCEL( SKILL_CANCEL_NEED_TARGET );
		}

		this->Set_TargetIDX( 0 );
		this->Del_ActiveSKILL ();
		m_wCommand = CMD_STOP;
	}

	return 1;
}

//-------------------------------------------------------------------------------------------------
