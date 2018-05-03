
#include "stdAFX.h"
#include "CObjNPC.h"
#include "GS_ThreadZONE.h"
#include "IO_Quest.h"

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CObjMOB::Init (CZoneTHREAD *pZONE, short nCharIdx, float fXPos, float fYPos, int iTeamNO, CRegenPOINT *pRegenPOINT, short nQuestIDX)
{
	m_pCharMODEL  = g_pCharDATA->GetMODEL( nCharIdx );
	if ( NULL == m_pCharMODEL )
		return false;
	if ( !m_pCharMODEL->IsValid() )
		return false;

	CObjTARGET::Set_TargetIDX( 0 );
	m_IngSTATUS.Reset( true );

	m_iCallerMobObjIDX = 0;
	m_iTeamNO     = iTeamNO;

    m_nCharIdx    = nCharIdx;
	m_nQuestIDX	  = nQuestIDX;
	m_pRegenPOINT = pRegenPOINT;

    m_PosBORN.x = fXPos;
    m_PosBORN.y = fYPos;
    m_PosCUR    = m_PosBORN;
	m_nPosZ		= 0;

	m_fRunAniSPEED = 1.0f;

	if ( IsTAIWAN() ) {
		// 몬스터의 CRITICAL 수치는 몬스터의 (레벨 수치* 2.5) 을 사용한다. (대만 2005.06.01)
		m_nCritical = (short) ( NPC_LEVEL( m_nCharIdx ) * 2.5f );
	} else {
		m_nCritical = (short)( NPC_LEVEL( m_nCharIdx ) * 0.6f );
	}

	m_fScale	= NPC_SCALE( m_nCharIdx ) / 100.f;
	m_iOriMaxHP = NPC_LEVEL( m_nCharIdx ) * NPC_HP( m_nCharIdx );
    m_iHP		= m_iOriMaxHP;

	m_nSavedDamageCNT = NPC_HP( m_nCharIdx ) / 8 + 4;
	m_SavedDAMAGED = new tagSavedDAMAGE[ m_nSavedDamageCNT ];
	::ZeroMemory( m_SavedDAMAGED, sizeof(tagSavedDAMAGE) * m_nSavedDamageCNT );

	::ZeroMemory( m_iAiVAR, sizeof(int)*MAX_MOB_VAR_CNT );

	if ( NULL == this->GetZONE() ) {
		CObjAI::SetCMD_STOP ();
		return pZONE->Add_OBJECT( this );		// 몹 생성
	}

	// 정지 명령후 전송...
	SetCMD_STOP ();

    return true;
}

//-------------------------------------------------------------------------------------------------
int CObjMOB::Get_AttackRange( short nSkillIDX )
{
	/// 스킬에 공격 거리가 입력되어 있다면 스킬거리 아니면 무기 거리..
	if ( nSkillIDX && SKILL_DISTANCE( nSkillIDX ) ) 
	{
		return SKILL_DISTANCE( nSkillIDX );
	}

	// 몹은 입력된 데이타로...
	return (int) ( Def_AttackRange () + ( this->Get_SCALE() * 120 ) );
}

//-------------------------------------------------------------------------------------------------
bool CObjMOB::Make_gsv_ADD_OBJECT( classPACKET *pCPacket )
{
    if ( this->Get_HP() <= 0 )  return false;

	this->Init_ADD_CHAR( pCPacket );

    pCPacket->m_HEADER.m_wType = GSV_MOB_CHAR;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_MOB_CHAR );

    pCPacket->m_gsv_MOB_CHAR.m_nCharIdx   = this->m_nCharIdx;
	pCPacket->m_gsv_MOB_CHAR.m_nQuestIDX  = this->m_nQuestIDX;

	this->Add_ADJ_STATUS( pCPacket );

    return true;
}

//-------------------------------------------------------------------------------------------------
CAI_OBJ*CObjMOB::Get_CALLER()
{
	if ( this->GetCallerObjIDX() ) {
		CObjCHAR *pOwner = g_pObjMGR->Get_CharOBJ( this->GetCallerObjIDX(), true );
		if ( pOwner && pOwner->Get_CharHASH() == this->GetCallerHASH() )
			return pOwner;
	}
	return NULL;
}

bool CObjMOB::SetCMD_Skill2SELF (short nSkillIDX, short nMotion)
{
	// called from CAI_OBJ..
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() || this->m_IngSTATUS.IsSET( FLAG_ING_DUMB ) ) return true;

	if ( CObjAI::SetCMD_Skill2SELF( nSkillIDX ) ) {
		m_cSkillMotionIDX = (char)nMotion;
		return this->Send_gsv_SELF_SKILL( nSkillIDX, nMotion );
	}
	return false;
}
bool CObjMOB::SetCMD_Skill2OBJ (int iTargetObjIDX, short nSkillIDX, short nMotion)
{
	// called from CAI_OBJ..
	if ( this->m_IngSTATUS.IsIgnoreSTATUS() || this->m_IngSTATUS.IsSET( FLAG_ING_DUMB ) ) return true;

	if ( CObjAI::SetCMD_Skill2OBJ( iTargetObjIDX, nSkillIDX ) ) {
		m_cSkillMotionIDX = (char)nMotion;
		return this->Send_gsv_TARGET_SKILL( nSkillIDX, nMotion );
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CObjMOB::Make_gsv_SUB_OBJECT( classPACKET *pCPacket )
{
	if ( this->Get_HP() <= 0 ) {
		// 맞아 죽을경우는 클라이언트에서 알아서 뺄수 있으니까...
		return false;
	}

	return CGameOBJ::Make_gsv_SUB_OBJECT( pCPacket );
}

//-------------------------------------------------------------------------------------------------
bool CObjMOB::SendPacketToTARGET( CObjCHAR *pAtkCHAR, classPACKET *pCPacket )
{
	classDLLNODE< CObjAVT* > *pTargetNODE;

	this->LockTargetLIST ();
	{
		pTargetNODE = this->m_TargetLIST.GetHeadNode ();
		while( pTargetNODE ) {
			pTargetNODE->DATA->SendPacket( pCPacket );
			pTargetNODE = this->m_TargetLIST.GetNextNode( pTargetNODE );
		}
	}
	this->UnlockTargetLIST ();

	return true;
}


//-------------------------------------------------------------------------------------------------
#ifdef	__CS_TARGET_LIST
void CObjMOB::LockTargetLIST ()
{
	if ( !m_csTargetLIST.TryLock () ) {
		HANDLE hOwningThread = m_csTargetLIST.GetOwningThread ();
		// 현재 쓰레드를 소유한 쓰레드의 콜스택을 보자...
	#ifdef	__USE_TRACE
		TRACE_THREADSTACKMSG( hOwningThread, "**OwningTHREAD**" );
		// 현재 호출된 콜스택을 보자...
		TRACE_STACKMSG ("**TryTHREAD**");
	#endif
		::sndPlaySound( "Connect.WAV", SND_ASYNC);
		g_LOG.CS_ODS( 0xfff, "LockTargetLIST:: Critical !!!! in %s :: %s \n", __FILE__, m_StrLOC.Get() );

		m_csTargetLIST.Lock ();
	}
	m_StrLOC.Set( __FILE__ );
}
void CObjMOB::UnlockTargetLIST ()
{
	m_StrLOC.Del ();
	m_csTargetLIST.Unlock ();
}
#endif

//-------------------------------------------------------------------------------------------------
void CObjMOB::Add_ToTargetLIST( CObjAVT *pAVTChar )
{	
	this->LockTargetLIST ();
		this->m_TargetLIST.AppendNode( pAVTChar->m_pTargetNODE );
	this->UnlockTargetLIST ();
}
void CObjMOB::Sub_FromTargetLIST( CObjAVT *pAVTChar )
{	
	this->LockTargetLIST ();
		this->m_TargetLIST.DeleteNode( pAVTChar->m_pTargetNODE );
	this->UnlockTargetLIST ();
}

//-------------------------------------------------------------------------------------------------
bool CObjMOB::Dead( CObjCHAR *pKiller )
{
	if ( CObjCHAR::Dead(NULL) ) {
		classDLLNODE< CObjAVT* > *pTargetNODE;

		if ( this->GetCallerUsrIDX() ) {
			// 소환수가 있으면 갯수 감소
			CObjCHAR *pOwner = g_pObjMGR->Get_CharOBJ( this->GetCallerUsrIDX(), true );
			if ( pOwner && pOwner->IsUSER() ) {
				if ( this->GetCallerHASH() == pOwner->Get_CharHASH() ) {
					short nNeedValue = NPC_NEED_SUMMON_CNT( this->Get_CharNO() ); 
					pOwner->Sub_SummonCNT( nNeedValue );
					// pOwner->Send_gsv_SET_SUMMON_COUNT (); damage로 날라감...
				}
			}
		}

		this->LockTargetLIST ();
		{
			pTargetNODE = this->m_TargetLIST.GetHeadNode ();
			while( pTargetNODE ) {
				assert( this->Get_INDEX() == pTargetNODE->DATA->Get_TargetIDX() );

				this->m_TargetLIST.DeleteNode( pTargetNODE );
				pTargetNODE->DATA->CObjTARGET::Set_TargetIDX( 0 );
				pTargetNODE = this->m_TargetLIST.GetHeadNode ();
			}
		}
		this->UnlockTargetLIST ();

		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
#define	EXPIRED_DAMAGED_TIME	( 10 * 1000 )		// 10 sec
#define	IGNORE_DAMAGED_TIME		( 5 * 60 * 1000 )	// 5 min
int CObjMOB::Save_Damage( int iAttackerIDX, int iDamage)
{
	short nElderSlot=-1;
	DWORD dwElderTime = 0x0ffffffff;
	DWORD dwCurTIME = (this->GetZONE())->GetCurrentTIME ();

	for (short nI=0; nI<this->m_nSavedDamageCNT; nI++) {
		if ( this->m_SavedDAMAGED[ nI ].m_iObjectIDX == iAttackerIDX ) {
			this->m_SavedDAMAGED[ nI ].m_dwUpdateTIME = dwCurTIME;
			this->m_SavedDAMAGED[ nI ].m_iDamage += iDamage;
			return this->m_SavedDAMAGED[ nI ].m_iDamage;
		}

		if ( dwCurTIME - this->m_SavedDAMAGED[ nI ].m_dwUpdateTIME >= EXPIRED_DAMAGED_TIME ) {
			if ( this->m_SavedDAMAGED[ nI ].m_dwUpdateTIME < dwElderTime ) {
				// 가장 오래된 공격자를 삭제..
				dwElderTime = this->m_SavedDAMAGED[ nI ].m_dwUpdateTIME;
				nElderSlot = nI;
			}
		}
	}

	if ( nElderSlot >= 0 ) {
		this->m_SavedDAMAGED[ nElderSlot ].m_dwInsertTIME = dwCurTIME;
		this->m_SavedDAMAGED[ nElderSlot ].m_dwUpdateTIME = dwCurTIME;
		this->m_SavedDAMAGED[ nElderSlot ].m_iObjectIDX = iAttackerIDX;
		this->m_SavedDAMAGED[ nElderSlot ].m_iDamage = iDamage;
		return iDamage;
	}

	return 0;
}

int CObjMOB::Get_AbilityValue (WORD wType)
{
	switch( wType ) {
//		case AT_STR		:		return Get_STR ();
//		case AT_DEX		:		return Get_DEX ();
		case AT_INT		:		return Get_INT ();
//		case AT_CON		:		return Get_CON ();
		case AT_CHARM	:		return Get_CHARM ();
		case AT_SENSE	:		return Get_SENSE ();
		case AT_HP		:		return Get_HP (); 
		case AT_MP		:		return Get_MP ();
		case AT_ATK		:		return Get_ATK ();
		case AT_DEF		:		return Get_DEF ();
		case AT_HIT		:		return Get_HIT ();
		case AT_RES		:		return Get_RES ();
		case AT_AVOID	:		return Get_AVOID ();
		case AT_SPEED	:		return (int)Get_MoveSPEED ();
		case AT_ATK_SPD :		return Get_nAttackSPEED();
		case AT_WEIGHT	:		return Get_WEIGHT ();
		case AT_CRITICAL   :	return Get_CRITICAL ();

		case AT_LEVEL	:		return Get_LEVEL ();
//		case AT_BONUSPOINT :	return Get_BonusPOINT ();
//		case AT_SKILLPOINT :	return Get_SkillPOINT ();

		case AT_MAX_HP	:		return Get_MaxHP ();
		case AT_MAX_MP	:		return Get_MaxMP ();
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
classUSER *CObjMOB::Give_EXP ()
{
	DWORD dwElderTIME = 0x0ffffffff;
	CObjCHAR  *pHitterCHAR;
	classUSER *pHitterUSER, *pOwner=NULL;
	// int iDistance;
	bool bPartyUser=false;

	DWORD dwCurTIME = (this->GetZONE())->GetCurrentTIME ();
	short nI;
	int   iEXP;
	for (nI=0; nI<this->m_nSavedDamageCNT; nI++) {
		if ( this->m_SavedDAMAGED[ nI ].m_dwInsertTIME ) {
			if ( dwCurTIME - this->m_SavedDAMAGED[ nI ].m_dwUpdateTIME > IGNORE_DAMAGED_TIME ) {
				// 오래된 누적 데이미지는 경험치 주지 않는다.
				this->m_SavedDAMAGED[ nI ].m_iObjectIDX = 0;
				continue;
			}

			pHitterCHAR = g_pObjMGR->Get_CharOBJ( this->m_SavedDAMAGED[ nI ].m_iObjectIDX, true );
			if ( NULL == pHitterCHAR /* || !pHitterCHAR->IsUSER() */ ) {	// 05.02.17 부분 주석처리..
				this->m_SavedDAMAGED[ nI ].m_iObjectIDX = 0;
				continue;
			}
			pHitterUSER = (classUSER*)pHitterCHAR->Get_CALLER ();

			//if ( !pHitterCHAR->IsUSER() ) {
			//	// 사용자에 의해 소환된 NPC냐 ????
			//	pHitterUSER = g_pObjMGR->Get_UserOBJ( pHitterCHAR->GetOwnerOBJ() );
			//	this->m_SavedDAMAGED[ nI ].m_iObjectIDX = pHitterCHAR->GetOwnerOBJ();
			//} else
			//	pHitterUSER = (classUSER*)pHitterCHAR;

			if ( pHitterUSER && pHitterUSER->GetZONE() == this->GetZONE() ) {
				this->m_SavedDAMAGED[ nI ].m_iObjectIDX = pHitterUSER->Get_INDEX();		// 2004. 9. 30 추가...
				if ( dwCurTIME - this->m_SavedDAMAGED[ nI ].m_dwUpdateTIME < EXPIRED_DAMAGED_TIME &&
					this->m_SavedDAMAGED[ nI ].m_dwInsertTIME < dwElderTIME ) {
					// EXPIRED_DAMAGED_TIME이 지나지 않은 유저중 먼저 때린넘으로..
					dwElderTIME = this->m_SavedDAMAGED[ nI ].m_dwInsertTIME;
					pOwner = pHitterUSER;
				}

				iEXP = CCal::Get_EXP( pHitterUSER, this, this->m_SavedDAMAGED[ nI ].m_iDamage);
				//if ( !pHitterCHAR->IsUSER() ) {
				//	// 소환 NPC로 부터 얻은 경치는 50% ...
				//	nEXP /= 2;
				//}

				if ( pHitterUSER->GetPARTY() ) {
					this->m_SavedDAMAGED[ nI ].m_iDamage = iEXP;
					bPartyUser = true;
					continue;
				}
				if ( iEXP > 0 ) {
					// 파티 없으면 바로 경험치 올림.
					pHitterUSER->Add_EXP( iEXP, true, this->Get_INDEX() );
				}
			}

			this->m_SavedDAMAGED[ nI ].m_iObjectIDX = 0;
		}
	}

	// 파티 경험치 분배...
	if ( bPartyUser ) {
		classUSER *pPartyUSER;
		short nJ;

		for (nI=0; nI<this->m_nSavedDamageCNT; nI++) {
			if ( this->m_SavedDAMAGED[ nI ].m_iObjectIDX && this->m_SavedDAMAGED[ nI ].m_dwInsertTIME ) {
				pHitterUSER = g_pObjMGR->Get_UserOBJ( this->m_SavedDAMAGED[ nI ].m_iObjectIDX );
				if ( !pHitterUSER || !pHitterUSER->GetPARTY() )		// 0x00061abb 오류... 소환몹일경우 뻑나나???
					continue;

				for (nJ=nI+1; nJ<this->m_nSavedDamageCNT; nJ++) {
					if ( this->m_SavedDAMAGED[ nJ ].m_iObjectIDX && this->m_SavedDAMAGED[ nJ ].m_dwInsertTIME ) {
						pPartyUSER = g_pObjMGR->Get_UserOBJ( this->m_SavedDAMAGED[ nJ ].m_iObjectIDX );
						if ( pPartyUSER && pHitterUSER->GetPARTY() == pPartyUSER->GetPARTY() ) {
							// 같은 파티원의 경험치를 몰아서 얻는다.
							this->m_SavedDAMAGED[ nI ].m_iDamage += this->m_SavedDAMAGED[ nJ ].m_iDamage;
							this->m_SavedDAMAGED[ nJ ].m_iObjectIDX = 0;
						}
					}
				}

				// 누적된 경험치를 파티를 통해 전송.
				if ( !pHitterUSER->m_pPartyBUFF->AddEXP( this, this->m_SavedDAMAGED[ nI ].m_iDamage,
					ZONE_PARTY_EXP_A( this->GetZONE()->Get_ZoneNO() ), ZONE_PARTY_EXP_B( this->GetZONE()->Get_ZoneNO() ) ) ) {
					// 파티 경험치 적용안됐다.
					pHitterUSER->Add_EXP( this->m_SavedDAMAGED[ nI ].m_iDamage, true, this->Get_INDEX() );
				}
			}
		}
	}

	return pOwner;
}


//-------------------------------------------------------------------------------------------------
void CObjMOB::Run_AWAY (int iDistance)
{
	float fX, fY;

	fX = (float)( RANDOM( iDistance*2 ) - iDistance );
	fY = (float)( RANDOM( iDistance*2 ) - iDistance );
	if ( m_pRegenPOINT ) {
		// 리젠 포인트를 중심으로 iDistance만큼 이동...
		fX += m_pRegenPOINT->m_fXPos;
		fY += m_pRegenPOINT->m_fYPos;
	} else {
		// 소환된 몹이다..
		fX += m_PosBORN.x;
		fY += m_PosBORN.y;
	}

	if ( this->SetCMD_MOVE2D (fX, fY, true) )
		this->Set_COMMAND( CMD_RUNAWAY );
}

//-------------------------------------------------------------------------------------------------
void CObjMOB::Drop_ITEM (short nDropITEM, BYTE btToOwner)
{
	tagITEM sITEM;
	sITEM.Init( nDropITEM );

	// 아이템 수치가 맞게 입력되어 있는가 ??
	if ( sITEM.GetTYPE() && sITEM.GetItemNO() ) {
		CObjITEM *pObjITEM = new CObjITEM;
		if ( pObjITEM ) {
			tPOINTF DropPOS = this->m_PosCUR;
			DropPOS.x += ( RANDOM(1001) - 500 );	// 랜덤 5미터..
			DropPOS.y += ( RANDOM(1001) - 500 );

			if ( btToOwner ) {
				DWORD dwElderTIME = 0x0ffffffff;
				CObjCHAR  *pHitterCHAR;
				classUSER *pHitterUSER, *pMobOWNER=NULL;
				DWORD dwCurTIME = (this->GetZONE())->GetCurrentTIME ();
				for (short nI=0; nI<this->m_nSavedDamageCNT; nI++) {
					if ( this->m_SavedDAMAGED[ nI ].m_dwInsertTIME ) {
						if ( dwCurTIME - this->m_SavedDAMAGED[ nI ].m_dwUpdateTIME > IGNORE_DAMAGED_TIME ) {
							// 오래된 누적 데이미지는 경험치 주지 않는다.
							// this->m_SavedDAMAGED[ nI ].m_iObjectIDX = 0;
							continue;
						}
						pHitterCHAR = g_pObjMGR->Get_CharOBJ( this->m_SavedDAMAGED[ nI ].m_iObjectIDX, true );
						if ( NULL == pHitterCHAR /* || !pHitterCHAR->IsUSER() */ ) {
							// this->m_SavedDAMAGED[ nI ].m_iObjectIDX = 0;
							continue;
						}
						pHitterUSER = (classUSER*)pHitterCHAR->Get_CALLER ();
						if ( pHitterUSER && pHitterUSER->GetZONE() == this->GetZONE() ) {
							//this->m_SavedDAMAGED[ nI ].m_iObjectIDX = pHitterUSER->Get_INDEX();		// 2004. 9. 30 추가...
							if ( dwCurTIME - this->m_SavedDAMAGED[ nI ].m_dwUpdateTIME < EXPIRED_DAMAGED_TIME &&
								this->m_SavedDAMAGED[ nI ].m_dwInsertTIME < dwElderTIME ) {
								// EXPIRED_DAMAGED_TIME이 지나지 않은 유저중 먼저 때린넘으로..
								dwElderTIME = this->m_SavedDAMAGED[ nI ].m_dwInsertTIME;
								pMobOWNER = pHitterUSER;
							}
						}
					}
				}
				if ( pMobOWNER )
					pObjITEM->InitItemOBJ( NULL, DropPOS, this->m_PosSECTOR, sITEM, pMobOWNER, true, pMobOWNER->GetPARTY() );	// 죽을때 떨굼
				else
					pObjITEM->InitItemOBJ( NULL, DropPOS, this->m_PosSECTOR, sITEM );
			} else
				pObjITEM->InitItemOBJ( NULL, DropPOS, this->m_PosSECTOR, sITEM );

			this->GetZONE()->Add_DIRECT( pObjITEM );		// AI에서 떨굼 아이템
		/*
			if ( sITEM.m_cType != ITEM_TYPE_MONEY )
				LogString( LOG_NORMAL, "Zone: %d :: 아이템 %s 드롭 Type: %d, NO: %d \n", pObjITEM->GetZONE()->Get_ZoneNO(), ITEM_NAME( sITEM.m_cType, sITEM.m_nItemNo ), sITEM.m_cType, sITEM.m_nItemNo );
			else 
				LogString( LOG_NORMAL, "Zone: %d :: 돈 드롭 %d \n", pObjITEM->GetZONE()->Get_ZoneNO(), sITEM.m_nItemNo);
		*/
		}
	}
}

bool CObjMOB::Change_CHAR (int iCharIDX)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType = GSV_CHANGE_NPC;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHANGE_NPC );
	pCPacket->m_gsv_CHANGE_NPC.m_wObjectIDX = this->Get_INDEX ();
	pCPacket->m_gsv_CHANGE_NPC.m_nNPCNo = iCharIDX;

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );
	
	// 이거만 해도 될려나 ???
	// 모델 데이타 !!!! 안해서 뻑~~~
	this->m_nCharIdx	= iCharIDX;
	this->m_pCharMODEL  = g_pCharDATA->GetMODEL( iCharIDX );
	this->m_fScale		= NPC_SCALE( iCharIDX ) / 100.f;
	this->m_iOriMaxHP   = NPC_LEVEL( m_nCharIdx ) * NPC_HP( iCharIDX );
    this->m_iHP			= m_iOriMaxHP;

	return true;
}
void CObjMOB::Set_EMOTION (short nEmotionIDX)
{
	if ( !this->Send_gsv_SET_MOTION( (WORD)BIT_MOTION_STOP_CMD, nEmotionIDX ) )
		return;
	
	this->Set_MOTION( nEmotionIDX, m_fCurMoveSpeed );
}

/*
bool CObjMOB::Check_EVENT (CGameOBJ *pSourOBJ, short nEventIDX)
{
	if ( pSourOBJ->IsA( OBJ_USER ) && m_nQuestIDX ) {
		return g_pEventLIST->Run_EVENT( this->Get_INDEX(), m_nQuestIDX, nEventIDX );
	}

	return false;
}
*/

//-------------------------------------------------------------------------------------------------
int CObjMOB::Proc ()
{
	if ( this->Get_HP() <= 0 && CMD_DIE != this->Get_COMMAND() ) {
		// HP가 DEAD_HP인넘이 CMD가 STOP이되어 리제포인트에서 빠지지 않아
		// 리젠이 되지 않는경우가 있다... 어떤 경우냐 ???
		LogString( 0xffff, ">>Warnning:: DeadMOB( %s ), Zone%d:%s, HP: %d, CMD: %x\n",
			this->Get_NAME(),
			this->GetZONE()->Get_ZoneNO(),
			this->GetZONE()->Get_NAME(),
			this->Get_HP(),
			this->Get_COMMAND() );

		this->Set_COMMAND( CMD_DIE );
	}

	return CObjCHAR::Proc ();
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void CObjSUMMON::SetCallerOBJ( int iOwnerOBJ, t_HASHKEY HashOwnerOBJ, int iOwnerLEVEL, short nSkillIDX, CObjCHAR * pOwnerAVT)
{
	m_iCallerUserObjIDX = iOwnerOBJ;
	m_HashCALLER = HashOwnerOBJ;

	m_iLevel = iOwnerLEVEL;
	m_nSummonedSkillIDX = nSkillIDX;

	short nSkillLEV = SKILL_LEVEL( nSkillIDX );

	if( IsTAIWAN() )
	{
		// 2005-6-30(kchs) m_iOriMaxHP, m_iOriATK, m_iOriHIT, m_iOriDEF, m_iOriRES, m_iOriAVOID 모두 수정
		if ( NPC_FACE_ICON( m_nCharIdx ) ) {
			m_iOriMaxHP = NPC_HP( m_nCharIdx );
		} else {
			m_iOriMaxHP = (int)( NPC_HP( m_nCharIdx ) + pOwnerAVT->Get_MaxHP() * 0.8f );
		}

		m_iOriATK	= (int)( CObjMOB::GetOri_ATK()	+ pOwnerAVT->Get_ATK() * (nSkillLEV * 5 + 75 ) * 0.01f );
		m_iOriHIT	= (int)( CObjMOB::GetOri_HIT()	+ pOwnerAVT->Get_HIT() );
		m_iOriDEF	= (int)( CObjMOB::GetOri_DEF()	+ pOwnerAVT->Get_DEF() );
		m_iOriRES	= (int)( CObjMOB::GetOri_RES()	+ pOwnerAVT->Get_RES() );
		m_iOriAVOID = (int)( CObjMOB::GetOri_AVOID() + pOwnerAVT->Get_AVOID() );
	}
	else
	{
		m_iOriMaxHP = (int)( NPC_HP( m_nCharIdx )   * ( nSkillLEV + 16 ) * ( iOwnerLEVEL + 85  ) / 2600.f );
		m_iOriATK	= (int)( CObjMOB::GetOri_ATK()	* ( nSkillLEV + 22 ) * ( iOwnerLEVEL + 100 ) / 4000.f );
		m_iOriHIT	= (int)( CObjMOB::GetOri_HIT()	* ( nSkillLEV + 30 ) * ( iOwnerLEVEL + 50  ) / 3200.f );
		m_iOriDEF	= (int)( CObjMOB::GetOri_DEF()	* ( nSkillLEV + 30 ) * ( iOwnerLEVEL + 80  ) / 4400.f );
		m_iOriRES	= (int)( CObjMOB::GetOri_RES()	* ( nSkillLEV + 24 ) * ( iOwnerLEVEL + 90  ) / 3600.f );
		m_iOriAVOID = (int)( CObjMOB::GetOri_AVOID()* ( nSkillLEV + 22 ) * ( iOwnerLEVEL + 90  ) / 3400.f );
	}
	this->Set_HP( m_iOriMaxHP );
}

int CObjSUMMON::Proc ()
{
	if ( IsTAIWAN() ) {
		//CObjCHAR *pOwner = g_pObjMGR->Get_CharOBJ( this->GetCallerUsrIDX(), true );
		//if ( pOwner && this->m_btSummonCMD != pOwner->Get_SummonCMD() ) {
		//	if ( pOwner->Get_CharHASH() == this->GetCallerHASH() ) {
		//		this->m_btSummonCMD = pOwner->Get_SummonCMD ();
		//		;
		//		;
		//		;
		//	}
		//}
	}

	return CObjCHAR::Proc ();
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CObjNPC::CObjNPC ()
{
	//TODO:: Get npc data from db ...
	m_bShow = true;
}
CObjNPC::~CObjNPC ()
{
	//TODO:: Set npc data to db
}

//-------------------------------------------------------------------------------------------------
short CObjNPC::VGetCur_ZoneNO()
{
	if ( !this->GetZONE() )
		return 0;
	return this->GetZONE()->Get_ZoneNO ();
}

//-------------------------------------------------------------------------------------------------
bool CObjNPC::Send_gsv_SET_EVENT_STATUS ()
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType	= GSV_SET_EVENT_STATUS;
	pCPacket->m_HEADER.m_nSize	= sizeof( gsv_SET_EVENT_STATUS );
	pCPacket->m_gsv_SET_EVENT_STATUS.m_wObjectIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_SET_EVENT_STATUS.m_nEventSTATUS	= this->m_nEventSTATUS;

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}
//-------------------------------------------------------------------------------------------------
bool CObjNPC::Make_gsv_ADD_OBJECT( classPACKET *pCPacket )
{
    if ( this->Get_HP() <= 0 )  return false;

	this->Init_ADD_CHAR( pCPacket );

    pCPacket->m_HEADER.m_wType = GSV_NPC_CHAR;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_NPC_CHAR );

	if ( this->m_bShow )
		pCPacket->m_gsv_NPC_CHAR.m_nCharIdx = this->m_nCharIdx;
	else
		pCPacket->m_gsv_NPC_CHAR.m_nCharIdx = -this->m_nCharIdx;

	pCPacket->m_gsv_NPC_CHAR.m_nQuestIDX  = this->m_nQuestIDX;
	pCPacket->m_gsv_NPC_CHAR.m_fModelDIR  = this->m_fModelDIR;

	pCPacket->m_gsv_NPC_CHAR.m_nEventSTATUS  = this->m_nEventSTATUS;

	this->Add_ADJ_STATUS( pCPacket );

    return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjNPC::Get_SellITEM( short nSellTAB, short nSellCOL, tagITEM &OutITEM )
{
	short nListSellROW, nSellITEM;

	nListSellROW = NPC_SELL_TAB( this->Get_CharNO(), nSellTAB );
	if ( 0 == nListSellROW || nListSellROW >= g_TblStore.m_nDataCnt ) 
		return false;

	if ( nSellCOL < 0 || nSellCOL+2 >= g_TblStore.m_nColCnt )
		return false;

	nSellITEM    = STORE_ITEM( nListSellROW, nSellCOL );
	if ( 0 == nSellITEM )
		return false;

	OutITEM.Init( nSellITEM );

	return true;
}

//-------------------------------------------------------------------------------------------------
void CObjNPC::VSet_SHOW	( BYTE btShowMode )
{
	switch( btShowMode ) {
		case 0 :	// 숨기기
			if ( !this->m_bShow )	return;
			this->m_bShow = false;
			break;
		case 1 :	// 보이기
			if ( this->m_bShow )	return;
			this->m_bShow = true;
			break;
		case 2 :	// 토글
			if ( this->m_bShow ) {
				this->m_bShow = false;
			} else {
				this->m_bShow = true;
			}
			break;
	}

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( pCPacket ) {
		pCPacket->m_HEADER.m_wType	= GSV_SET_NPC_SHOW;
		pCPacket->m_HEADER.m_nSize	= sizeof( gsv_SET_NPC_SHOW );

		pCPacket->m_gsv_SET_NPC_SHOW.m_wObjectIDX = this->Get_INDEX();
		pCPacket->m_gsv_SET_NPC_SHOW.m_bShow = this->m_bShow;

		this->GetZONE()->SendPacketToSectors( this, pCPacket );

		Packet_ReleaseNUnlock( pCPacket );
	}
}


//-------------------------------------------------------------------------------------------------
int CObjNPC::Proc ()
{
	if ( CObjVAR::ProcVAR( (this->GetZONE())->GetPassTIME() )  ) {
		// 다음 이벤트 처리...
		// m_HashCurrentTrigger = m_HashNextTrigger;
		g_QuestList.CheckQUEST( NULL, m_HashNextTrigger, true, 0, this );
		m_HashNextTrigger = 0;
	}

	return CObjCHAR::Proc ();
}

//-------------------------------------------------------------------------------------------------
