//---------------------------------------------------------------------------

#include "LIB_gsMAIN.h"
#include "CObjCHAR.h"
#include "GS_ThreadZONE.h"
#include "ZoneLIST.h"
//#include "CVector.h"
//#include "CObjAVT.h"
//#include "Calculation.h"
//#include "GS_PARTY.h"
//#include "GS_ThreadLOG.h"
//#include "IO_Quest.h"

//-------------------------------------------------------------------------------------------------
CAI_OBJ* CObjCHAR::AI_FindFirstOBJ( tPOINTF &PosCENTER, int iDistance )
{
	if ( this->GetZONE() )
		return this->GetZONE()->FindFirstCHAR( PosCENTER, iDistance, this );

	return NULL;
}
CAI_OBJ* CObjCHAR::AI_FindFirstOBJ( int iDistance )
{
	if ( this->GetZONE() )
		return this->GetZONE()->FindFirstCHAR( this, iDistance, this );

	return NULL;
}
CAI_OBJ* CObjCHAR::AI_FindNextOBJ ()
{
	return this->GetZONE()->FindNextCHAR ();
}

CAI_OBJ* CObjCHAR::Find_LocalNPC( int iNpcNO )
{
	return g_pZoneLIST->Get_LocalNPC( iNpcNO );
}

//-------------------------------------------------------------------------------------------------
int	CObjCHAR::Get_EconomyVAR (short nVarIDX)
{
	return this->GetZONE()->Get_EconomyVAR( nVarIDX );
}
int	CObjCHAR::Get_WorldVAR  (short nVarIDX)
{
	return ::Get_WorldVAR( nVarIDX );
}

void CObjCHAR::Set_EconomyVAR(short nVarIDX, int iValue)
{
#pragma	COMPILE_TIME_MSG ( "TODO:: CObjCHAR::Set_EconomyVAR(short nVarIDX, int iValue) " )
}
void CObjCHAR::Set_WorldVAR  (short nVarIDX, int iValue)
{
	g_pZoneLIST->Set_WorldVAR( nVarIDX, iValue );
}

//-------------------------------------------------------------------------------------------------
DWORD CObjCHAR::GetCurAbsSEC ()
{
	if ( this->GetZONE() )
		this->GetZONE()->GetCurAbsSEC();

	return classTIME::GetCurrentAbsSecond();
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Create_PET (int iCharIDX, float fPosX, float fPosY, int iRange, BYTE btSetOwner, bool bImmediate)
{
	CObjMOB *pNewPET;
	pNewPET = this->GetZONE()->RegenCharacter( fPosX, fPosY, iRange, iCharIDX, 1, this->Get_TeamNO()/* TEAMNO_MOB */, bImmediate );
	if ( pNewPET ) {
		if ( btSetOwner ) {
			// 소환 주체를 오너로 설정...
			pNewPET->m_iCallerMobObjIDX = this->Get_INDEX();
		}
		if ( this->m_IngSTATUS.IsSET( FLAG_ING_DEC_LIFE_TIME ) ) {
			// 소환된 케릭이 소환할 경우...
			pNewPET->m_IngSTATUS.UpdateIngSTATUS( pNewPET, 
						IDX_ING_STB_DEC_LIFE_TIME, 
						0 /* SKILL_DURATION(nSkillIDX) */, 
						0 /* nAdjValue */, 
						0 /* nSkillIDX */ );
		}
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
BYTE CObjCHAR::Is_DAY (void)
{
	return this->GetZONE()->Is_DAY ();
}

//-------------------------------------------------------------------------------------------------
CObjCHAR::CObjCHAR ()
{
	m_IngSTATUS.Reset( true );

	m_iHP = 0;
	m_iMP = 0;
	m_fCurAniSPEED = 1.0f;

	m_fScale = 1.0f;
}
CObjCHAR::~CObjCHAR ()
{
//	LogString (LOG_NORMAL, "~CObjCHAR()  ObjTYPE: %d, ObjIDX: %d \n", Get_TYPE(), Get_INDEX() );
}


//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_SKILL_CANCEL( BYTE btWhy )
{	
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType	= GSV_SKILL_CANCEL;
	pCPacket->m_HEADER.m_nSize	= sizeof( gsv_SKILL_CANCEL );
	pCPacket->m_gsv_SKILL_CANCEL.m_wObjectIDX = this->Get_INDEX();
	pCPacket->m_gsv_SKILL_CANCEL.m_btWHY = btWhy;

	this->GetZONE()->SendPacketToSectors( this, pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_CHAT( char *szMsg )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_CHAT;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHAT );
    pCPacket->m_gsv_CHAT.m_wObjectIDX = this->Get_INDEX();
    pCPacket->AppendString( szMsg );

    this->GetZONE()->SendPacketToSectors( this, pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}
bool CObjCHAR::Send_gsv_SHOUT( char *szMsg )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_SHOUT;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_SHOUT );
    pCPacket->AppendString( this->Get_NAME() );
    pCPacket->AppendString( szMsg );

	this->GetZONE()->SendShout( this, pCPacket );
    Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
void CObjCHAR::Send_gsv_ANNOUNCE_CHAT( char *szMsg )
{
	// 현재 맵 공지
	g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT( this->GetZONE()->Get_ZoneNO(), szMsg, this->Get_NAME() );
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_STOP ()
{
    assert( this->GetZONE() );
    
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType			= GSV_STOP;
	pCPacket->m_HEADER.m_nSize			= sizeof( gsv_STOP );
	pCPacket->m_gsv_STOP.m_wObjectIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_STOP.m_PosCUR		= this->m_PosCUR;

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_MOUSECMD (int iTargetObject)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType				= GSV_MOUSECMD;
	pCPacket->m_HEADER.m_nSize				= sizeof( gsv_MOUSECMD);
	pCPacket->m_gsv_MOUSECMD.m_wSourObjIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_MOUSECMD.m_wDestObjIDX	= iTargetObject;
	pCPacket->m_gsv_MOUSECMD.m_PosTO		= this->m_PosGOTO;
	pCPacket->m_gsv_MOUSECMD.m_wSrvDIST		= (WORD)::distance( m_PosCUR, m_PosGOTO );
	pCPacket->m_gsv_MOUSECMD.m_nPosZ		= this->m_nPosZ;

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_MOVE ()
{
//	LogString( 0xffff, "Send_gsv_MOVE( %s:%.0f ) , (%.0f,%.0f) => (%.0f,%.0f) \n", this->Get_NAME(), m_fCurMoveSpeed, m_PosCUR.x, m_PosCUR.y, m_PosGOTO.x, m_PosGOTO.y);

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType			= GSV_MOVE;
	pCPacket->m_HEADER.m_nSize			= sizeof( gsv_MOVE );
	pCPacket->m_gsv_MOVE.m_wSourObjIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_MOVE.m_wDestObjIDX	= this->Get_TargetIDX();
	pCPacket->m_gsv_MOVE.m_PosTO		= this->m_PosGOTO;
	pCPacket->m_gsv_MOVE.m_wSrvDIST		= (WORD)::distance( m_PosCUR, m_PosGOTO );
	pCPacket->m_gsv_MOVE.m_btMoveMODE	= this->Get_MoveMODE();

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_ATTACK (int iTargetObject)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType			= GSV_ATTACK;
	pCPacket->m_HEADER.m_nSize			= sizeof( gsv_ATTACK );
	pCPacket->m_gsv_ATTACK.m_wAtkObjIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_ATTACK.m_wDefObjIDX	= iTargetObject;
	pCPacket->m_gsv_ATTACK.m_wSrvDIST	= (WORD)::distance( m_PosCUR, m_PosGOTO );
	pCPacket->m_gsv_ATTACK.m_PosTO		= this->m_PosGOTO;		// 2003. 12. 26 추가...

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_TOGGLE( BYTE btTYPE, bool bUpdateSpeed )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType			= GSV_TOGGLE;
	pCPacket->m_HEADER.m_nSize			= sizeof( gsv_TOGGLE );

	pCPacket->m_gsv_TOGGLE.m_wObjectIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_TOGGLE.m_btTYPE		= btTYPE;

	if ( bUpdateSpeed ) {
		pCPacket->m_HEADER.m_nSize += sizeof(short);
		pCPacket->m_gsv_TOGGLE.m_nRunSPEED[ 0 ] = this->GetOri_RunSPEED();
	}

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_SELF_SKILL( short nSkillIDX, short nNpcSkillMotion )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType				 = GSV_SELF_SKILL;
	pCPacket->m_gsv_SELF_SKILL.m_wSourObjIDX = this->Get_INDEX ();
	pCPacket->m_gsv_SELF_SKILL.m_nSkillIDX	 = nSkillIDX;

	if ( nNpcSkillMotion >= 0 ) {
		pCPacket->m_HEADER.m_nSize	= sizeof(char) + sizeof( gsv_SELF_SKILL );
		pCPacket->m_gsv_SELF_SKILL.cNpcSkillMOTION[ 0 ] = (char)nNpcSkillMotion;
	} else {
		pCPacket->m_HEADER.m_nSize	= sizeof( gsv_SELF_SKILL );
	}

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_TARGET_SKILL (short nSkillIDX, short nNpcSkillMotion )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType				   = GSV_TARGET_SKILL;
	pCPacket->m_gsv_TARGET_SKILL.m_wSourObjIDX = this->Get_INDEX ();
	pCPacket->m_gsv_TARGET_SKILL.m_wDestObjIDX = this->Get_TargetIDX();
	pCPacket->m_gsv_TARGET_SKILL.m_nSkillIDX   = nSkillIDX;
	pCPacket->m_gsv_TARGET_SKILL.m_wSrvDIST	   = (WORD)::distance( m_PosCUR, m_PosGOTO );
	pCPacket->m_gsv_TARGET_SKILL.m_PosTO	   = this->m_PosGOTO;

	if ( nNpcSkillMotion >= 0 ) {
		pCPacket->m_HEADER.m_nSize	= sizeof(char) + sizeof( gsv_TARGET_SKILL );
		pCPacket->m_gsv_TARGET_SKILL.cNpcSkillMOTION[ 0 ] = (char)nNpcSkillMotion;
	} else {
		pCPacket->m_HEADER.m_nSize	= sizeof( gsv_TARGET_SKILL );
	}

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_POSITION_SKILL (short nSkillIDX)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType				    = GSV_POSITION_SKILL;
	pCPacket->m_HEADER.m_nSize					= sizeof( gsv_POSITION_SKILL );
	pCPacket->m_gsv_POSITION_SKILL.m_wObjectIDX = this->Get_INDEX ();
	pCPacket->m_gsv_POSITION_SKILL.m_nSkillIDX  = nSkillIDX;
	pCPacket->m_gsv_POSITION_SKILL.m_PosTARGET	= this->m_PosGOTO;;

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_CLEAR_STATUS (DWORD dwClearedFLAG)
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	BYTE btIDX=0;	
	if ( dwClearedFLAG & FLAG_ING_HP )
		pCPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ] = this->Get_HP();

	if ( dwClearedFLAG & FLAG_ING_MP )
		pCPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ] = this->Get_MP();

/*
	if ( dwClearedFLAG & FLAG_ING_INC_MOV_SPEED )
		pCPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ] = this->m_IngSTATUS.Inc_RUN_SPEED ();

	if ( dwClearedFLAG & FLAG_ING_DEC_MOV_SPEED ) 
		pCPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ] = this->m_IngSTATUS.Dec_RUN_SPEED ();

	if ( dwClearedFLAG & FLAG_ING_INC_ATK_SPEED )
		pCPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ] = this->m_IngSTATUS.Inc_ATK_SPEED ();
	
	if ( dwClearedFLAG & FLAG_ING_DEC_ATK_SPEED )
		pCPacket->m_gsv_CLEAR_STATUS.m_nAdjVALUE[ btIDX++ ] = this->m_IngSTATUS.Dec_ATK_SPEED ();
*/

	pCPacket->m_HEADER.m_wType				    = GSV_CLEAR_STATUS;
	pCPacket->m_HEADER.m_nSize					= sizeof( gsv_CLEAR_STATUS ) + btIDX * sizeof(short);
	pCPacket->m_gsv_CLEAR_STATUS.m_wObjectIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_CLEAR_STATUS.m_dwStatusFLAG = this->m_IngSTATUS.GetFLAGs();

	if ( dwClearedFLAG & ~(FLAG_ING_HP|FLAG_ING_MP) ) {
		// 기타 적용이면 주변에 전송, 파티제외... 주변 파티원을 알아서, 멀리있는 파티원은 몰라..
		this->GetZONE()->SendPacketToSectors( this, pCPacket );
		// this->SendPacketToPartyExceptME( pCPacket );
	} else {
		// 물약 적용이면 파티원 한테만...
		this->SendPacketToPARTY( pCPacket );
	}

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}


//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_SET_MOTION( WORD wValue, short nMotionNO )
{
	if ( wValue & BIT_MOTION_STOP_CMD ) {
		if ( !CObjAI::SetCMD_STOP () )
			return true;
	}

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType				= GSV_SET_MOTION;
	pCPacket->m_HEADER.m_nSize				= sizeof( gsv_SET_MOTION );
	pCPacket->m_gsv_SET_MOTION.m_wObjectIDX = this->Get_INDEX ();
	pCPacket->m_gsv_SET_MOTION.m_wValue		= wValue;
	pCPacket->m_gsv_SET_MOTION.m_nMotionNO  = nMotionNO;

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_DAMAGE2Sector( int iAttackObject, WORD wDamage, CObjITEM *pDropITEM)
{
	// iAttackObject가 자신을 죽일때...
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType			= GSV_DAMAGE;

	pCPacket->m_gsv_DAMAGE.m_wAtkObjIDX	= iAttackObject;
	pCPacket->m_gsv_DAMAGE.m_wDefObjIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_DAMAGE.m_Damage.m_wDamage = wDamage;

	if ( NULL == pDropITEM ) {
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_DAMAGE );
	} else {
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_DAMAGE ) + sizeof( tag_DROPITEM );

		pDropITEM->SetACTIVE ();

		pCPacket->m_gsv_DAMAGE.m_DropITEM[0].m_PosCUR			= pDropITEM->m_PosCUR;
		pCPacket->m_gsv_DAMAGE.m_DropITEM[0].m_ITEM				= pDropITEM->m_ITEM;
		pCPacket->m_gsv_DAMAGE.m_DropITEM[0].m_wServerItemIDX	= pDropITEM->Get_INDEX();
		pCPacket->m_gsv_DAMAGE.m_DropITEM[0].m_wOwnerObjIDX		= pDropITEM->m_iOwnerObjIDX;
		//pCPacket->m_gsv_DAMAGE.m_FieldITEM[0].m_wRemainTIME = pDropITEM->m_iRemainTIME;
	}

    this->GetZONE()->SendPacketToSectors( this, pCPacket );

	// 자신을 소환한 케릭이 있다면...
	if ( this->GetCallerUsrIDX() ) {
		CObjCHAR *pOwner = g_pObjMGR->Get_CharOBJ( this->GetCallerUsrIDX(), true );
		if ( pOwner && pOwner->Get_CharHASH() == this->GetCallerHASH() ) {
			if ( !this->IsNEIGHBOR( pOwner ) )		// 패킷이 전송되는 영역 밖에 있나?
				pOwner->SendPacket( pCPacket );
		}
	}

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_DAMAGE2Target( CObjCHAR *pAtkOBJ, WORD wDamage )
{
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	pCPacket->m_HEADER.m_wType			= GSV_DAMAGE;
	pCPacket->m_HEADER.m_nSize			= sizeof( gsv_DAMAGE );
	pCPacket->m_gsv_DAMAGE.m_wAtkObjIDX	= pAtkOBJ->Get_INDEX();
	pCPacket->m_gsv_DAMAGE.m_wDefObjIDX	= this->Get_INDEX ();
	pCPacket->m_gsv_DAMAGE.m_Damage.m_wDamage = wDamage;

	// 아바타 자신이 맞은 경우에 소속된 파티에 전송, 때리넘이 아바타이면( PvsP ) 때리넘 한테도 전송.
	// 몹이 맞을 경우 자신을 타겟팅하고 있는 유저들에게 전송...
	this->SendPacketToTARGET( pAtkOBJ, pCPacket );

    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
void CObjCHAR::Init_ADD_CHAR( classPACKET *pCPacket )
{
    pCPacket->m_tag_ADD_CHAR.m_wObjectIDX	= this->Get_INDEX ();
    pCPacket->m_tag_ADD_CHAR.m_PosCUR		= this->m_PosCUR;
    pCPacket->m_tag_ADD_CHAR.m_PosTO		= this->m_PosGOTO;
    pCPacket->m_tag_ADD_CHAR.m_wCommand		= this->Get_COMMAND ();
    pCPacket->m_tag_ADD_CHAR.m_wTargetOBJ	= this->Get_TargetIDX();
    pCPacket->m_tag_ADD_CHAR.m_iHP			= this->Get_HP ();

	pCPacket->m_tag_ADD_CHAR.m_btMoveMODE	= this->Get_MoveMODE();
	pCPacket->m_tag_ADD_CHAR.m_dwStatusFALG = this->m_IngSTATUS.GetFLAGs();

#ifdef	__APPLY_04_10_15_TEAMNO
	pCPacket->m_tag_ADD_CHAR.m_iTeamNO		= this->Get_TeamNO();
#else
	pCPacket->m_tag_ADD_CHAR.m_cTeamNO		= this->Get_TeamNO();
#endif
}
void CObjCHAR::Add_ADJ_STATUS( classPACKET *pCPacket )
{
	if ( this->m_IngSTATUS.GetFLAGs() & 
		  ( FLAG_ING_MAX_HP |
			FLAG_ING_INC_MOV_SPEED | FLAG_ING_DEC_MOV_SPEED |
			FLAG_ING_INC_ATK_SPEED | FLAG_ING_DEC_ATK_SPEED |
			FLAG_ING_DEC_LIFE_TIME ) ) {
		BYTE btIDX=0;
		short *pAdjSTATUS = (short*)&pCPacket->m_pDATA[ pCPacket->m_HEADER.m_nSize ];

		if ( this->m_IngSTATUS.IsSET( FLAG_ING_MAX_HP ) )
			pAdjSTATUS[ btIDX++ ] = this->m_IngSTATUS.Inc_MAX_HP();

		if ( this->m_IngSTATUS.IsSET( FLAG_ING_INC_MOV_SPEED ) )
			pAdjSTATUS[ btIDX++ ] = this->m_IngSTATUS.Inc_RUN_SPEED();

		if ( this->m_IngSTATUS.IsSET( FLAG_ING_DEC_MOV_SPEED ) )
			pAdjSTATUS[ btIDX++ ] = this->m_IngSTATUS.Dec_RUN_SPEED();

		if ( this->m_IngSTATUS.IsSET( FLAG_ING_INC_ATK_SPEED ) )
			pAdjSTATUS[ btIDX++ ] = this->m_IngSTATUS.Inc_ATK_SPEED();

		if ( this->m_IngSTATUS.IsSET( FLAG_ING_DEC_ATK_SPEED ) )
			pAdjSTATUS[ btIDX++ ] = this->m_IngSTATUS.Dec_ATK_SPEED();

		if ( this->m_IngSTATUS.IsSET( FLAG_ING_DEC_LIFE_TIME ) ) {
			( (WORD*)pAdjSTATUS )[ btIDX++ ] = this->GetCallerUsrIDX ();
			if ( this->GetCallerUsrIDX () ) {
				pAdjSTATUS[ btIDX++ ] = this->GetSummonedSkillIDX ();
			}
		}

		pCPacket->m_HEADER.m_nSize += sizeof(short) * btIDX;
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Send_gsv_EFFECT_OF_SKILL (int iSpellOBJ, short nSkillIDX, BYTE btResult, short nSpellerINT )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_EFFECT_OF_SKILL;
    pCPacket->m_HEADER.m_nSize = sizeof( gsv_EFFECT_OF_SKILL );

	pCPacket->m_gsv_EFFECT_OF_SKILL.m_wObjectIDX	= this->Get_INDEX();
	pCPacket->m_gsv_EFFECT_OF_SKILL.m_wSpellObjIDX	= iSpellOBJ;
	pCPacket->m_gsv_EFFECT_OF_SKILL.m_nSkillIDX		= nSkillIDX;
	pCPacket->m_gsv_EFFECT_OF_SKILL.m_btSuccessBITS	= btResult;
	pCPacket->m_gsv_EFFECT_OF_SKILL.m_nINT			= nSpellerINT;

	this->GetZONE()->SendPacketToSectors( this, pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}
bool CObjCHAR::Send_gsv_DAMAGE_OF_SKILL (int iSpellOBJ, short nSkillIDX, WORD wDamage, BYTE btResult, short nSpellerINT, CObjITEM *pDropITEM)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

    pCPacket->m_HEADER.m_wType = GSV_DAMAGE_OF_SKILL;

	pCPacket->m_gsv_DAMAGE_OF_SKILL.m_wObjectIDX	= this->Get_INDEX();
	pCPacket->m_gsv_DAMAGE_OF_SKILL.m_wSpellObjIDX	= iSpellOBJ;
	pCPacket->m_gsv_DAMAGE_OF_SKILL.m_nSkillIDX		= nSkillIDX;
	pCPacket->m_gsv_DAMAGE_OF_SKILL.m_btSuccessBITS = btResult;
	pCPacket->m_gsv_DAMAGE_OF_SKILL.m_nINT			= nSpellerINT;
	pCPacket->m_gsv_DAMAGE_OF_SKILL.m_wDamage		= wDamage;

	if ( NULL == pDropITEM ) {
	    pCPacket->m_HEADER.m_nSize = sizeof( gsv_DAMAGE_OF_SKILL );
	} else {
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_DAMAGE_OF_SKILL ) + sizeof( tag_DROPITEM );

		pDropITEM->SetACTIVE ();

		pCPacket->m_gsv_DAMAGE_OF_SKILL.m_DropITEM[0].m_PosCUR			= pDropITEM->m_PosCUR;
		pCPacket->m_gsv_DAMAGE_OF_SKILL.m_DropITEM[0].m_ITEM			= pDropITEM->m_ITEM;
		pCPacket->m_gsv_DAMAGE_OF_SKILL.m_DropITEM[0].m_wServerItemIDX	= pDropITEM->Get_INDEX();
		pCPacket->m_gsv_DAMAGE_OF_SKILL.m_DropITEM[0].m_wOwnerObjIDX	= pDropITEM->m_iOwnerObjIDX;
	}

	this->GetZONE()->SendPacketToSectors( this, pCPacket );

	// 자신을 소환한 케릭이 있다면...
	if ( this->GetCallerUsrIDX() ) {
		CObjCHAR *pOwner = g_pObjMGR->Get_CharOBJ( this->GetCallerUsrIDX(), true );
		if ( pOwner && pOwner->IsUSER() && pOwner->Get_CharHASH() == this->GetCallerHASH() ) {
			if ( !this->IsNEIGHBOR( pOwner ) )		// 패킷이 전송되는 영역 밖에 있나?
				pOwner->SendPacket( pCPacket );
		}
	}

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}
//-------------------------------------------------------------------------------------------------
void CObjCHAR::Send_gsv_RESULT_OF_SKILL (short nSkillIDX)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;
	
	pCPacket->m_HEADER.m_wType = GSV_RESULT_OF_SKILL;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_RESULT_OF_SKILL );

	pCPacket->m_gsv_RESULT_OF_SKILL.m_wObjectIDX = this->Get_INDEX();
	pCPacket->m_gsv_RESULT_OF_SKILL.m_nSkillIDX  = nSkillIDX;

	this->GetZONE()->SendPacketToSectors( this, pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::SetCMD_STOP ()
{
	if ( CObjAI::SetCMD_STOP () )
		return this->Send_gsv_STOP ();

	return false;
}
bool CObjCHAR::SetCMD_MOVE (tPOINTF &PosCUR, tPOINTF &PosTO, int iTargetObjIDX)
{
	if ( CObjAI::SetCMD_MOVE (PosCUR, PosTO, iTargetObjIDX) ) {
		// 위장술도 안풀려~
		return this->Send_gsv_MOUSECMD( iTargetObjIDX );
	}
	return false;
}
bool CObjCHAR::SetCMD_MOVE2D (float fPosX, float fPosY, BYTE btRunMODE)
{
	// 인공지능에서 호출되는 이동함수 !!!
	if ( !this->GetZONE()->IsMovablePOS( (int)fPosX, (int)fPosY ) )
		return false;

	// 타운트 상태면 풀어라...
	if ( this->m_IngSTATUS.IsSET( FLAG_ING_TAUNT ) ) {
		this->m_IngSTATUS.ExpireSTATUS( ING_TAUNT );
		//this->m_IngSTATUS.ClearSTATUS( ING_TAUNT );
	}

	if ( CObjAI::SetCMD_MOVE2D (fPosX, fPosY, btRunMODE) ) {
		return this->Send_gsv_MOVE ();
	}
	return false;
}
bool CObjCHAR::SetCMD_ATTACK (int iTargetObject)
{
/*
	if ( this->m_iTargetObject != iTargetObject ) {
		if ( this->m_iTargetObject ) {
			pEnemy->Del_ENEMY( this );
		}
	}
*/
	if ( CObjAI::SetCMD_ATTACK( iTargetObject ) ) {
		if ( CMD_ATTACK == this->Get_COMMAND() ) {
			// 위장술 해제...
			if ( this->m_IngSTATUS.IsSET( FLAG_ING_DISGUISE ) ) {	// 위장중이면 해제...
				this->m_IngSTATUS.ClearStatusFLAG( FLAG_ING_DISGUISE );
			}
			this->Send_gsv_ATTACK( iTargetObject );
		} else
			this->Send_gsv_STOP ();

		return true;
	}
	return false;
}


//-------------------------------------------------------------------------------------------------
bool CObjCHAR::SetCMD_Skill2SELF (short nSkillIDX)
{
	if ( SKILL_TYPE_01 != SKILL_TYPE( nSkillIDX ) ) {
		if ( CObjAI::SetCMD_Skill2SELF( nSkillIDX ) ) {
			// 위장술/투명 해제...
			if ( this->m_IngSTATUS.IsSET( FLAG_ING_DISGUISE ) ) {	// 위장중이면 해제...
				this->m_IngSTATUS.ClearStatusFLAG( FLAG_ING_DISGUISE );
			}
			return this->Send_gsv_SELF_SKILL( nSkillIDX );
		}
	}
	return false;
}
bool CObjCHAR::SetCMD_Skill2OBJ (int iTargetObjIDX, short nSkillIDX)
{
	if ( CObjAI::SetCMD_Skill2OBJ( iTargetObjIDX, nSkillIDX ) ) {
		// 위장술 해제...
		if ( this->m_IngSTATUS.IsSET( FLAG_ING_DISGUISE ) ) {	// 위장중이면 해제...
			this->m_IngSTATUS.ClearStatusFLAG( FLAG_ING_DISGUISE );
		}
		return this->Send_gsv_TARGET_SKILL( nSkillIDX );
	}
	return false;
}
bool CObjCHAR::SetCMD_Skill2POS (tPOINTF &PosGOTO, short nSkillIDX)
{
	if ( CObjAI::SetCMD_Skill2POS( PosGOTO, nSkillIDX ) ) {
		// 위장술 해제...
		if ( this->m_IngSTATUS.IsSET( FLAG_ING_DISGUISE ) ) {	// 위장중이면 해제...
			this->m_IngSTATUS.ClearStatusFLAG( FLAG_ING_DISGUISE );
		}
		return this->Send_gsv_POSITION_SKILL( nSkillIDX );
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Set_MOTION (short nActionIdx, float fMoveSpeed, float fAniSpeed, bool bAttackMotion)
{
/*
	if ( !bAttackMotion && m_bAttackSTART )
		Attack_END ();
*/
	if ( fAniSpeed <= 0.f || fAniSpeed > 6.f ) {
		g_LOG.CS_ODS( 0xffff, ">>> ERROR:: Invalid motion speed: ActionIdx:%d, MoveSpeed:%f, AniApeed:%f, bAtk:%d \n", nActionIdx, fMoveSpeed, fAniSpeed, bAttackMotion );
		// assert( fAniSpeed > 0.f && fAniSpeed <= 5.f );
	}

	tagMOTION *pMotion = this->Get_MOTION (nActionIdx);
	if ( !pMotion )
		return false;

	if ( this->Chg_CurMOTION( pMotion ) ) {
		m_fCurAniSPEED = fAniSpeed;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Dead (CObjCHAR *pKiller)
{
    if ( Get_HP() <= DEAD_HP )
        return false;

	// 기절 상태로 만들어 명령 불복 시킨다.
	this->m_IngSTATUS.SetFLAG( FLAG_ING_FAINTING );

	this->Set_HP( DEAD_HP );
	this->SetCMD_DIE ();

	return true;
}

//-------------------------------------------------------------------------------------------------
#define SEND_DAMAGE_TO_NULL		0x00	// Miss
#define	SEND_DAMAGE_TO_TARGET	0x01
#define	SEND_DAMAGE_TO_SECTOR	0x02
BYTE CObjCHAR::Apply_DAMAGE (CObjCHAR *pTarget, uniDAMAGE &sDamage, CObjITEM **ppOutITEM, bool bDropItem)
{
	if ( pTarget->m_IngSTATUS.IsSET( FLAG_ING_REVIVE ) ) {
		// 타겟이 죽었다가 금방 부활한 상태면... 뎀지 안들어 오게...
		return SEND_DAMAGE_TO_NULL;
	}

	int  iDamage = sDamage.m_wVALUE;

	//// 위장술/투명 해제...
	//if ( this->m_IngSTATUS.IsSET( FLAG_ING_DISGUISE | FLAG_ING_TRANSPARENT) ) {	// 위장중이면 해제...
	//	this->m_IngSTATUS.ClearStatusFLAG( FLAG_ING_DISGUISE | FLAG_ING_TRANSPARENT );
	//}

	// Normal Attack !
	if ( iDamage ) {
		if ( this->GetCallerUsrIDX() ) {
			//if ( NULL == this->Get_OWNER() )	주인이 죽었을때 소환몹이 자살시 스킵되어 생략..
			//	return SEND_DAMAGE_TO_NULL;
			if ( DIV02( iDamage ) > 0 )	// iDamge > 2 
				pTarget->Save_Damage( this->GetCallerUsrIDX(), DIV02( iDamage ) );		// 소환된 넘은 경치를 1/2로...
		} else
			pTarget->Save_Damage( this->Get_INDEX(), iDamage );

		// 무적 상태인가 ???
		if ( pTarget->m_IngSTATUS.IsSubSET( FLAG_CHEAT_INVINCIBLE ) ) {
			if ( pTarget->Get_HP() <= iDamage ) {
				pTarget->Set_HP( iDamage+100 );
			}
		}

		this->Stamp_AttackTIME ();

		if ( pTarget->Sub_HP( iDamage ) <= 0 ) {
			// 공격자가 사람또는 사람이 소환한 몹 !!!  경험치 & 레벨업
			if ( this->IsUSER() || this->GetCallerUsrIDX() ) {
				if ( pTarget->IsUSER() || pTarget->GetCallerUsrIDX() ) {		// USER 또는 소환된몹
					// TODO:: PK !!!!
				} else
				if ( pTarget->IsNPC () ) {		// MOB or NPC
					classUSER *pMobOWNER = pTarget->Give_EXP ();

					pTarget->Do_DeadAI( pMobOWNER, iDamage );	// pMobOWNER == NULL일때도 수행되도록...

					if ( pMobOWNER ) {
						if ( NPC_DEAD_EVENT( pTarget->Get_CharNO() ) )  {
							// Target죽을때 이벤트 있으면 클라이언트에 체크해 봐라 전송...
							pMobOWNER->Send_gsv_CHECK_NPC_EVENT( pTarget->Get_CharNO() );
						}

						if ( bDropItem ) {
							// 아이템 드롭, 지역 마법에 의에서는 아이템생성 안함..왜 ? 패킷 관리가 귀찮아서...
							tagITEM sITEM;
#ifdef FRAROSE
							if ( CCal::Get_DropITEM ( pMobOWNER->Get_LEVEL()-pTarget->Get_LEVEL(), (CObjMOB*)pTarget, sITEM, this->GetZONE()->Get_ZoneNO(), pMobOWNER->GetCur_DropRATE() * pMobOWNER->m_GrowAbility.GetDropBoost( pMobOWNER->GetCurAbsSEC() ), pMobOWNER->Get_CHARM() ) )
#else
							if ( CCal::Get_DropITEM ( pMobOWNER->Get_LEVEL()-pTarget->Get_LEVEL(), (CObjMOB*)pTarget, sITEM, this->GetZONE()->Get_ZoneNO(), pMobOWNER->GetCur_DropRATE(), pMobOWNER->Get_CHARM() ) ) 
#endif
							{
								// Create field item ...
								*ppOutITEM = new CObjITEM;
								if ( *ppOutITEM ) {
									tPOINTF PosSET;
									PosSET.x = pTarget->m_PosCUR.x + RANDOM( 201 ) - 100;
									PosSET.y = pTarget->m_PosCUR.y + RANDOM( 201 ) - 100;

									// this는 사용자나 사용자가 소환한 소환수다.
									CObjCHAR *pSummonOWNER = (CObjCHAR*)this->Get_CALLER();
									CParty *pCParty = ( pSummonOWNER ) ? pSummonOWNER->GetPARTY() : NULL;

									// pMobOWNER->Set_ItemSN( (*ppOutITEM)->m_ITEM );	// 몹 죽을때 => 습득할때로 변경...

									(*ppOutITEM)->InitItemOBJ( NULL, PosSET, pTarget->m_PosSECTOR, sITEM, pMobOWNER, false, pCParty );	// 죽을때 떨굼
									this->GetZONE()->Add_DIRECT( *ppOutITEM );		// 드롭 아이템
								}
							}
						}
					}
				}
			} else {
				this->Do_KillAI( pTarget, iDamage );
			}

			pTarget->Dead( this );

			sDamage.m_wACTION &= ~DMG_ACT_HITTED;
			sDamage.m_wACTION |= DMG_ACT_DEAD;

			return SEND_DAMAGE_TO_SECTOR;
		} else {
			// 수면 상태에서 상대방의 공격을 받으면 풀리게된다.
			if ( pTarget->m_IngSTATUS.IsSET( FLAG_ING_SLEEP ) ) {
				pTarget->m_IngSTATUS.ClearStatusFLAG( FLAG_ING_SLEEP );
				pTarget->Send_gsv_CLEAR_STATUS( FLAG_ING_SLEEP );
			}

			pTarget->Do_DamagedAI ( this, iDamage );

			if ( sDamage.m_wACTION & DMG_ACT_HITTED ) {
				// 맞는 동작해야...
				if ( !(pTarget->Get_STATE() & CS_BIT_INT2) ) {
					pTarget->Set_MOTION( pTarget->GetANI_Hit() );
					pTarget->Set_STATE( CS_HIT );
				}
			}

			return SEND_DAMAGE_TO_TARGET;
		}
	} else {
		// 대상이 사용자 일경우 Miss시에도 방어구 감소 처리..
		if ( pTarget->IsUSER() ) {
			pTarget->Save_Damage( this->Get_INDEX(), iDamage );
		}

		// 수면 상태에서 상대방의 공격을 받으면 풀리게된다.
		if ( pTarget->m_IngSTATUS.IsSET( FLAG_ING_SLEEP ) ) {
			pTarget->m_IngSTATUS.ClearStatusFLAG( FLAG_ING_SLEEP );
			pTarget->Send_gsv_CLEAR_STATUS( FLAG_ING_SLEEP );
		}

		pTarget->Do_DamagedAI ( this, 0 );
	}

	return SEND_DAMAGE_TO_NULL;
}

//-------------------------------------------------------------------------------------------------
void CObjCHAR::Give_DAMAGE (CObjCHAR *pTarget, uniDAMAGE sDamage, bool bDropItem)
{
	if ( this->m_IngSTATUS.IsSET( FLAG_ING_REVIVE ) ) {
		this->m_IngSTATUS.ExpireSTATUS( ING_REVIVE );
	}

	CObjITEM *pOutITEM=NULL;

	switch( Apply_DAMAGE (pTarget, sDamage, &pOutITEM, bDropItem) ) {
		case SEND_DAMAGE_TO_SECTOR :
			pTarget->Send_gsv_DAMAGE2Sector( this->Get_INDEX(), sDamage.m_wDamage, pOutITEM );
			return;

		case SEND_DAMAGE_TO_TARGET :
			// 데미지 전송 :: 아바타만 해당 ...
			pTarget->Send_gsv_DAMAGE2Target( this, sDamage.m_wDamage ); 

			if ( pTarget->m_IngSTATUS.IsSET( FLAG_ING_SHIELD_DAMAGE ) ) {
				// 방패 데미지 설정됐다 !!!
				if ( !this->m_IngSTATUS.IsSET( FLAG_ING_SHIELD_DAMAGE ) ) {
					// 왼손에 방패 아이템 종류 261번 장착하고 있어야 :: 스킬 시작 조건에 들어 있겠지...
					if ( pTarget->IsInRANGE (this, 4000) ) {	// 거리가 4미터 이내 이면...
						short nShieldSKILL = pTarget->m_IngSTATUS.GetSkillIDX( ING_SHIELD_DAMAGE );
						sDamage.m_wDamage = (WORD) ( sDamage.m_wVALUE * SKILL_POWER( nShieldSKILL ) / 100.f );

						if ( sDamage.m_wDamage > 0 ) {
							sDamage.m_wDamage |= DMG_BIT_IMMEDIATE;
							pTarget->Give_DAMAGE( this, sDamage );
						}
					}
				}
			}
			return;
	}
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Skill_IsPassFilter( CObjCHAR *pTarget, short nSkillIDX )
{
	if ( NULL == pTarget )
		return false;

	bool bResult=false;
	switch( SKILL_CLASS_FILTER( nSkillIDX ) ) {
		case SKILL_TARGET_FILTER_GROUP		:///< 그룹(파티)원
			if ( this->GetPARTY() ) {
				bResult = ( pTarget->GetPARTY() == this->GetPARTY() );
				break;
			}
			// else 아래에서 파티가 없으면 자신인지 판다...

		case SKILL_TARGET_FILTER_SELF		:///< 자기 자신
			bResult = ( pTarget == this );
			break;

		case SKILL_TARGET_FILTER_GUILD		:///< 길드원
			bResult = ( this->GetGUILD() && pTarget->GetGUILD() == this->GetGUILD() );
			break;

		case SKILL_TARGET_FILTER_FRIEND_ALL :///< 아군 (아바타, 소환몹)
			bResult = ( this->Is_ALLIED( pTarget ) );
			break;

		case SKILL_TARGET_FILTER_MOB		:///< 모든 몬스터 (소환몹,적군몹)
			bResult = pTarget->IsA( OBJ_MOB );
			break;

		case SKILL_TARGET_FILTER_ENEMY_ALL	:///< 적 PC + 몬스터 
			bResult = ( !this->Is_ALLIED( pTarget ) );
			break;

		case SKILL_TARGET_FILTER_ENEMY_PC	:///< 적 PC( 적 상태 포함 )
			bResult = ( pTarget->IsUSER() && !this->Is_ALLIED( pTarget ) );
			break;

		case SKILL_TARGET_FILTER_ALL_PC		:///< 모든 PC
			bResult = ( pTarget->IsUSER() );
			break;

		case SKILL_TARGET_FILTER_DEAD_USER	:// 죽은 플레이어
			return ( pTarget->Get_HP() == DEAD_HP && pTarget->IsUSER() && this->Is_ALLIED( pTarget ) );

		case SKILL_TARGET_FILTER_ENEMY_MOB	:// 적 몬스터.
			bResult = ( !this->Is_ALLIED( pTarget ) && pTarget->IsA( OBJ_MOB ) );
			break;

		case SKILL_TARGET_FILTER_ALL_CHAR :
			bResult = true;
			break;

		default :
			return false;
	}

	if ( pTarget->Get_HP() > 0 )
		return bResult;
	return false;
}

//-------------------------------------------------------------------------------------------------
/*
BYTE CObjCHAR::Skill_IncAbilityValue( short nSkillIDX )
{
	BYTE btResult=0;
	for (short nI=0; nI<SKILL_INCREASE_ABILITY_CNT; nI++) {
		switch( SKILL_INCREASE_ABILITY( nSkillIDX, nI ) ) {
			case AT_HP	:
				this->Add_HP( SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI ) );
				btResult |= ( 1<<nI );
				continue;
			case AT_MP	:
				this->Add_MP( SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI ) );
				btResult |= ( 1<<nI );
				continue;
		}
	}

	return btResult;
}
*/

//-------------------------------------------------------------------------------------------------
// 능력치의 증/감을 일정시간 지속(셀프)
// 능력치의 증/감을 일정시간 지속(타겟)
// 상태 지속형(셀프)	중독, 벙어리, 기절, 투명, 방패데미지, 추가 데미지
// 상태 지속형(타겟)	중독, 벙어리, 기절, 투명, 방패데미지, 추가 데미지
BYTE CObjCHAR::Skill_ApplyIngSTATUS(short nSkillIDX, CObjCHAR *pSpeller)
{
	if ( this->Get_HP() <= 0 )
		return 0;

	if ( SKILL_HARM( nSkillIDX ) ) {
		// 적대유발...
		if ( this->m_IngSTATUS.IsSET( FLAG_ING_REVIVE ) ) {
			// 타겟이 죽었다가 금방 부활한 상태면... 뎀지 안들어 오게...
			return 0;
		}

		this->Do_DamagedAI ( pSpeller, 0 );

		this->Stamp_AttackTIME ();
		pSpeller->Stamp_AttackTIME ();
	}

	short nI, nIngSTB, nAdjValue;

	BYTE btSuccessBITS=0;	// 성공여부
	for (nI=0; nI<2; nI++) {
		nIngSTB = SKILL_STATE_STB( nSkillIDX, nI );
		if ( 0 == nIngSTB )	{
			switch( SKILL_INCREASE_ABILITY( nSkillIDX, nI ) ) {
				case AT_STAMINA	:
					this->Add_STAMINA( SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI ) );
					break;
				case AT_MONEY	:
					this->Add_MONEY( SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI ) );
					break;
				case AT_HP		:
				{
					nAdjValue = CCal::Get_SkillAdjustVALUE( this, nSkillIDX, nI, pSpeller->Get_INT() );
					this->Add_HP( nAdjValue );
					break;
				}
				case AT_MP		:
					this->Add_MP( SKILL_INCREASE_ABILITY_VALUE( nSkillIDX, nI ) );
					break;
				default			:
					continue;
			}
			btSuccessBITS |= ( 1<<nI );
			continue;
		}

		if ( SKILL_SUCCESS_RATIO( nSkillIDX ) ) {
			// 성공도 0 일경우는 계산없이 무조건 성공...
			if ( STATE_PRIFITS_LOSSES( nIngSTB ) ) {
				// 불리
				// 상대 레벨 * 0.4 + (상대 항마력-10) * 0.5 - 자신레벨 * 0.4 - (자신 항마력-10) * 0.5
//#ifndef	__INC_WORLD
//				if ( SKILL_SUCCESS_RATIO( nSkillIDX ) < ( this->Get_LEVEL() * 0.4f + ( this->Get_RES()-10 ) * 0.5f - pSpeller->Get_LEVEL() * 0.4f - ( pSpeller->Get_RES()-10 ) *0.5f + (1+RANDOM(100)) ) )
//					continue;
//#else
#ifdef	__APPLY_2ND_JOB
				// 후에 면역력 수치 넣어서 계산식 수정할곳...
				if ( SKILL_SUCCESS_RATIO( nSkillIDX ) * ( pSpeller->Get_LEVEL() * 2 + pSpeller->Get_INT()+20 ) / ( this->Get_RES()*0.6f + 5 + this->Get_AVOID() ) <= (1+RANDOM(100)) ) 
					continue;
#else
				if ( SKILL_SUCCESS_RATIO( nSkillIDX ) * ( pSpeller->Get_LEVEL() * 2 + pSpeller->Get_INT()+20 ) / ( this->Get_RES()*0.6f + 5 + this->Get_AVOID() ) <= (1+RANDOM(100)) ) 
					continue;
#endif
//#endif
			} else {
				// 유리
				// 성공룔(12번칼럼) > [상대 레벨 - 자신레벨 + 랜덤수치(1~100) ]  일경우 상태적용 성공
				if ( SKILL_SUCCESS_RATIO( nSkillIDX ) < this->Get_LEVEL() - pSpeller->Get_LEVEL() + (1+RANDOM(100)) )
					continue;
			}
		}

		nAdjValue = CCal::Get_SkillAdjustVALUE( this, nSkillIDX, nI, pSpeller->Get_INT() );
		// 적용 가능 한가 ???
		if ( !this->m_IngSTATUS.IsEnableApplay( nIngSTB, nAdjValue ) )	continue;

		btSuccessBITS |= ( 1<<nI );

		if ( this->m_IngSTATUS.UpdateIngSTATUS( this, nIngSTB, SKILL_DURATION(nSkillIDX), nAdjValue, nSkillIDX, pSpeller->Get_INDEX() ) ) {
			// 명령을 STOP으로 바꿀꺼... 기절, 수면 걸리면 정지 명령 설정...
			this->Del_ActiveSKILL();
			CObjAI::SetCMD_STOP ();
		}

		// 타운트 스킬이면...공격 대상 바꿈...
		if ( ING_TAUNT == STATE_TYPE(nIngSTB) ) {
			this->SetCMD_RUNnATTACK( pSpeller->Get_INDEX() );
		}
	}

	return btSuccessBITS;
}

//-------------------------------------------------------------------------------------------------
void CObjCHAR::Skill_ChangeIngSTATUS (CObjCHAR *pTarget)
{
	short nResultCNT=0;
	if ( pTarget->Get_RideMODE() < MOVE_MODE_DRIVE 
#ifdef __KCHS_BATTLECART__
		|| IsTAIWAN() // 타이완이면.. 팻모드에서 쓸수 있다.
#endif
		) {	
		// 데미지 적용된후 상태 변경이와서 죽은후 속성적용 안되도록~~~
		BYTE btResult;
		if ( 0 == SKILL_SCOPE( this->Get_ActiveSKILL() ) ) {
			if ( this->Skill_IsPassFilter( pTarget, this->Get_ActiveSKILL() ) ) {
				btResult = pTarget->Skill_ApplyIngSTATUS( this->Get_ActiveSKILL(), this );
				if ( btResult )  {
					pTarget->Send_gsv_EFFECT_OF_SKILL( this->Get_INDEX(), this->Get_ActiveSKILL(), btResult, this->Get_INT() );
					nResultCNT ++;
				}
			}
		} else {
			CObjCHAR *pFindCHAR;
			pFindCHAR = (CObjCHAR*)pTarget->AI_FindFirstOBJ( SKILL_SCOPE( Get_ActiveSKILL() ) );
			while( pFindCHAR ) {
				if ( this->Skill_IsPassFilter( pFindCHAR, Get_ActiveSKILL() ) ) {
					btResult = pFindCHAR->Skill_ApplyIngSTATUS( Get_ActiveSKILL(), this );
					if ( btResult ) {
						pFindCHAR->Send_gsv_EFFECT_OF_SKILL( this->Get_INDEX(), Get_ActiveSKILL(), btResult, this->Get_INT() );
						nResultCNT ++;
					}
				}

				pFindCHAR = (CObjCHAR*)pTarget->AI_FindNextOBJ ();
			}
		}
	}

	this->Send_gsv_RESULT_OF_SKILL( Get_ActiveSKILL() /*, nResultCNT */ );
}

//-------------------------------------------------------------------------------------------------
// 7, 17번 유형 스킬...
bool CObjCHAR::Skill_DamageToAROUND ( tPOINTF &PosCENTER )
{
	// 범위안의 적용할 타켓을 모두 구한다...
	// 각각 타겟에 데미지 적용...
	BYTE btResult;
	uniDAMAGE sDamage;
	CObjCHAR *pFindCHAR;
	short nResultCNT=0;

	CObjITEM *pOutITEM;
	// PosGOTO좌표를 중심으로 탐색...
	pFindCHAR = (CObjCHAR*)this->AI_FindFirstOBJ( PosCENTER, SKILL_SCOPE( Get_ActiveSKILL() ) );
	while( pFindCHAR ) {
		if ( this->Skill_IsPassFilter( pFindCHAR, Get_ActiveSKILL() ) ) {
			sDamage.m_wDamage  = CCal::Get_SkillDAMAGE( this, pFindCHAR, Get_ActiveSKILL(), 1 );

			pOutITEM = NULL;
			this->Apply_DAMAGE (pFindCHAR, sDamage, &pOutITEM, true);

			if ( this->Skill_IsPassFilter( pFindCHAR, Get_ActiveSKILL() ) ) {
				btResult = pFindCHAR->Skill_ApplyIngSTATUS( Get_ActiveSKILL(), this );
			} else btResult = 0;

			if ( sDamage.m_wDamage || btResult ) {
				sDamage.m_wDamage |= DMG_BIT_IMMEDIATE;		// 스킬 뎀쥐는 즉각 반응...
				pFindCHAR->Send_gsv_DAMAGE_OF_SKILL( this->Get_INDEX(), Get_ActiveSKILL(), sDamage.m_wDamage, btResult, this->Get_INT(), pOutITEM );
				nResultCNT ++;
			}
		}

		pFindCHAR = (CObjCHAR*)this->AI_FindNextOBJ ();
	}

	this->Send_gsv_RESULT_OF_SKILL( Get_ActiveSKILL() /*, nResultCNT */ );

	// 공격자 무기 수명 감소...
	if ( 3 != SKILL_DAMAGE_TYPE( Get_ActiveSKILL() ) ) {
		this->Dec_WeaponLife ();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
void CObjCHAR::Skill_START_03_04_05 (CObjCHAR *pTarget)
{
	assert( m_pCurMOTION->m_wTatalAttackFrame );

	// 03~06 :: 적 케릭터 공격 스킬...
	// @bug:: 스킬을 사용하기 전에 필터링을 하지만... 
	// 스킬 사용중 다른 스킬 명령이 올경우 중간에 스킬이 바뀌어
	// 필터링을 적용하지 않으면 뻑난다.
	if ( this->Skill_IsPassFilter( pTarget, Get_ActiveSKILL() ) ) 
	{
		uniDAMAGE sDamage;

		sDamage.m_wDamage = CCal::Get_SkillDAMAGE( this, pTarget, Get_ActiveSKILL(), m_pCurMOTION->m_wTatalAttackFrame);

		CObjITEM *pOutITEM=NULL;
		this->Apply_DAMAGE (pTarget, sDamage, &pOutITEM, true);
		pTarget->Send_gsv_DAMAGE_OF_SKILL( this->Get_INDEX(), Get_ActiveSKILL(), sDamage.m_wDamage, 0, 0, pOutITEM );
		this->Skill_ChangeIngSTATUS( pTarget );
	}

	// 공격자 무기 수명 감소...
	if ( 3 != SKILL_DAMAGE_TYPE( Get_ActiveSKILL() ) ) {
		this->Dec_WeaponLife ();
	}
}
//-------------------------------------------------------------------------------------------------
void CObjCHAR::Skill_START_10_11 (CObjCHAR *pTarget)
{
	short nResultCNT=0;
	BYTE btResult;
	if ( 0 == SKILL_SCOPE( Get_ActiveSKILL() ) ) {
		if ( this->Skill_IsPassFilter( pTarget, Get_ActiveSKILL() ) ) {
			//btResult = pTarget->Skill_IncAbilityValue( Get_ActiveSKILL() );
			btResult = pTarget->Skill_ApplyIngSTATUS( Get_ActiveSKILL(), this );
			if ( btResult ) {
				pTarget->Send_gsv_EFFECT_OF_SKILL( this->Get_INDEX(), Get_ActiveSKILL(), btResult, this->Get_INT() );
				nResultCNT ++;
			}
		}
	} else {
		CObjCHAR *pFindCHAR;
		pFindCHAR = (CObjCHAR*)pTarget->AI_FindFirstOBJ( SKILL_SCOPE( Get_ActiveSKILL() ) );
		while( pFindCHAR ) {
			if ( this->Skill_IsPassFilter( pFindCHAR, Get_ActiveSKILL() ) ) {
				// btResult = pFindCHAR->Skill_IncAbilityValue( Get_ActiveSKILL() );
				btResult = pFindCHAR->Skill_ApplyIngSTATUS( Get_ActiveSKILL(), this );
				if ( btResult ) {
					pFindCHAR->Send_gsv_EFFECT_OF_SKILL( this->Get_INDEX(), Get_ActiveSKILL(), btResult, this->Get_INT() );
					nResultCNT ++;
				}
			}

			pFindCHAR = (CObjCHAR*)pTarget->AI_FindNextOBJ ();
		}
	}

	this->Send_gsv_RESULT_OF_SKILL( Get_ActiveSKILL() /*, nResultCNT */ );
}
//-------------------------------------------------------------------------------------------------
void CObjCHAR::Skill_START_19 (CObjCHAR *pTarget)
{
	// 03~06 :: 적 케릭터 공격 스킬...
	// @bug:: 스킬을 사용하기 전에 필터링을 하지만... 
	// 스킬 사용중 다른 스킬 명령이 올경우 중간에 스킬이 바뀌어
	// 필터링을 적용하지 않으면 뻑난다.
	if ( this->Skill_IsPassFilter( pTarget, Get_ActiveSKILL() ) ) 
	{
		uniDAMAGE sDamage;

		sDamage.m_wDamage = CCal::Get_SkillDAMAGE( this, pTarget, Get_ActiveSKILL(), m_pCurMOTION->m_wTatalAttackFrame);

		CObjITEM *pOutITEM=NULL;
		this->Apply_DAMAGE (pTarget, sDamage, &pOutITEM, true);
		if ( sDamage.m_wVALUE > 0 ) {
			BYTE btResult = this->Skill_ApplyIngSTATUS( Get_ActiveSKILL(), pTarget );
			pTarget->Send_gsv_DAMAGE_OF_SKILL( this->Get_INDEX(), Get_ActiveSKILL(), sDamage.m_wDamage, btResult, this->Get_INT(), pOutITEM );
		} else {
			pTarget->Send_gsv_DAMAGE_OF_SKILL( this->Get_INDEX(), Get_ActiveSKILL(), sDamage.m_wDamage, 0, 0, pOutITEM );
		}
		this->Send_gsv_RESULT_OF_SKILL( Get_ActiveSKILL() /*, nResultCNT */ );
//		this->Skill_ChangeIngSTATUS( pTarget );
	}

	// 공격자 무기 수명 감소...
	if ( 3 != SKILL_DAMAGE_TYPE( Get_ActiveSKILL() ) ) {
		this->Dec_WeaponLife ();
	}
}
bool CObjCHAR::Skill_START (CObjCHAR *pTarget)
{
	// 서버에서는 스킬이 완료된후 결과만 처리 하면 된다....
	switch( SKILL_TYPE( Get_ActiveSKILL() ) ) {
		/*
		case SKILL_TYPE_01 :	// 기본 동작 스킬
		case SKILL_TYPE_02 :	// 창 생성 스킬
		case SKILL_TYPE_15 :	// 패시브
		case SKILL_TYPE_16 :	// emotion
		case SKILL_TYPE_18 :	// warp
			break;
		*/

		case SKILL_TYPE_03 :	// 공격 동작 변경형
		case SKILL_TYPE_04 :	// 무기 상태 변경형
		case SKILL_TYPE_05 :	// 총알 변경헝 공격형
			this->Skill_START_03_04_05( pTarget );
			return true;

		case SKILL_TYPE_06 :	// 발사 마법형
		{
			if ( this->Skill_IsPassFilter( pTarget, Get_ActiveSKILL() ) ) 
			{
				uniDAMAGE sDamage;

				sDamage.m_wDamage = CCal::Get_SkillDAMAGE( this, pTarget, Get_ActiveSKILL(), m_pCurMOTION->m_wTatalAttackFrame);
				this->Give_DAMAGE( pTarget, sDamage );
				this->Skill_ChangeIngSTATUS( pTarget );
			}

			// 공격자 무기 수명 감소...
			if ( 3 != SKILL_DAMAGE_TYPE( Get_ActiveSKILL() ) ) {
				this->Dec_WeaponLife ();
			}
			return true;
		}

		case SKILL_TYPE_07 :	// 지역 마법 공격형
			return this->Skill_DamageToAROUND( this->m_PosGOTO );
		case SKILL_TYPE_17 :	// self & damage ...
			return this->Skill_DamageToAROUND( this->m_PosCUR );

		case SKILL_TYPE_10 :	// 능력치 변경형(셀프)	:	한방에 올림
			if ( pTarget ) {
				this->CloseNETWORK ();
				return false;
			}
			this->Skill_START_10_11( this );
			return true;
		case SKILL_TYPE_11 :	// 능력치 변경형(타겟)	:	한방에 올림
			this->Skill_START_10_11( pTarget );
			return true;

		case SKILL_TYPE_08 :	// 능력치의 증/감을 일정시간 지속(셀프)
		case SKILL_TYPE_12 :	// 상태 지속형(셀프)	중독, 벙어리, 기절, 투명, 방패데미지, 추가 데미지
			/*
			if ( pTarget ) {
				this->CloseNETWORK ();
				return false;
			}
			*/
			this->Skill_ChangeIngSTATUS( this );
			return true;
			
		case SKILL_TYPE_09 :	// 능력치의 증/감을 일정시간 지속(타겟)
		case SKILL_TYPE_13 :	// 상태 지속형(타겟)	중독, 벙어리, 기절, 투명, 방패데미지, 추가 데미지
			this->Skill_ChangeIngSTATUS( pTarget );
			return true;

		case SKILL_TYPE_14 :	// 자신의 팀 번호로 NPC를 소환...
		{
			CObjSUMMON *pNewPET;
			pNewPET = new CObjSUMMON;
			
			if ( pNewPET && this->GetZONE()->RegenCharacter( pNewPET, this->m_PosCUR.x, this->m_PosCUR.y, 150, SKILL_SUMMON_PET( Get_ActiveSKILL() ), this->Get_TeamNO() ) ) {
				// pNewPET->m_btSummonCMD = this->Get_SummonCMD();
				pNewPET->SetCallerOBJ( this->Get_INDEX(), this->Get_CharHASH(), this->Get_LEVEL(), this->Get_ActiveSKILL() , this); // 2005-06-30(kchs) 대만 소환수 능력치 수정 관련...

				pNewPET->m_IngSTATUS.UpdateIngSTATUS( pNewPET, 
							IDX_ING_STB_DEC_LIFE_TIME, 
							0 /* SKILL_DURATION(nSkillIDX) */, 
							0 /* nAdjValue */, 
							0 /* nSkillIDX */ );
				short nNeedValue = NPC_NEED_SUMMON_CNT( pNewPET->Get_CharNO() ); 
				this->Add_SummonCNT( nNeedValue );
				this->Send_gsv_RESULT_OF_SKILL( Get_ActiveSKILL() /*, 1*/ );

				// 소환된 케릭 AI
				pNewPET->Do_CreatedAI ();
			} else
				SAFE_DELETE( pNewPET );
			return true;
		}

		case SKILL_TYPE_19 :	// (타겟에 데미지공격을 하면서 HP,MP 뺏어오기 개념)
			this->Skill_START_19( pTarget );
			return true;

		case SKILL_TYPE_20 :	// 부활
		{
			if ( this->Skill_IsPassFilter( pTarget, Get_ActiveSKILL() ) ) {
				pTarget->Send_gsv_EFFECT_OF_SKILL( this->Get_INDEX(), Get_ActiveSKILL(), 0x01, this->Get_INT() );
				this->Send_gsv_RESULT_OF_SKILL( Get_ActiveSKILL() /*, 1*/ );
				( (CObjAVT*)pTarget )->Resurrection( Get_ActiveSKILL() );
			}
			return true;
		}
		default :
			this->CloseNETWORK ();
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CObjCHAR::Attack_START (CObjCHAR *pTarget)
{
	if ( !m_pCurMOTION->m_wTatalAttackFrame ) {
#ifdef	__INC_WORLD
		assert( 0 != m_pCurMOTION->m_wTatalAttackFrame );
#endif
		return false;
	}

	// 소모탄 체크...
	if ( !this->Dec_ShotITEM( (BYTE)( m_pCurMOTION->m_wTatalAttackFrame ) ) )
		return true;

	uniDAMAGE sDamage;
	sDamage.m_wDamage = CCal::Get_DAMAGE( this, pTarget, m_pCurMOTION->m_wTatalAttackFrame );
	this->Give_DAMAGE (pTarget, sDamage);

	// 공격자 무기 수명 감소...
	this->Dec_WeaponLife ();

	return true;
}

void CObjCHAR::ActionEVENT (int iActionIDX)
{
	// 서버는 일루 못와 !!!
	assert( 0 && "Invalid function called" );
}

bool CObjCHAR::Casting_START (CObjCHAR *pTarget)
{
	m_bCastingSTART = true;

	if ( CMD_SKILL2OBJ == this->Get_COMMAND() ) {
		// 스킬 타겟이 이동중일경우 클라이언트가 케스팅 시작 위치를 알지 못하므로...
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( !pCPacket ) 
			return false;

		pCPacket->m_HEADER.m_wType = GSV_SKILL_START;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_SKILL_START );
		pCPacket->m_gsv_SKILL_START.m_wObjectIDX = this->Get_INDEX ();

		this->GetZONE()->SendPacketToSectors( this, pCPacket );
		Packet_ReleaseNUnlock( pCPacket );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
/*
void CObjCHAR::Move_COMPLETED ()
{
	m_PosCUR = m_PosGOTO;
}
*/


int CObjCHAR::RealProc (DWORD dwPassTIME)
{
	if ( ( CS_BIT_MOV | CS_BIT_MOV2 ) & this->Get_STATE() ) {
		m_PosCUR += ( m_MoveVEC * dwPassTIME );
		if ( !this->GetZONE()->UpdateSECTOR( this ) ) {	// 섹터 변환이 있을경우 호출하는 함수.
			// 좌표가 이상한넘이다 짤러 !!!
			return 0;
		}

		this->UpdateCartGuest();
	}

	//if ( this->Get_MoveMODE() < MOVE_MODE_DRIVE && this->Get_HP() > 0 ) {
	//	// 탑승 모드가 아니고 앉기 모드면...
	//	this->RecoverHPnMP (dwPassTIME);
	//}

	bool bFrameING;
	bFrameING = this->ProcMotionFrame ();
	if ( bFrameING && ( Get_STATE()&CS_BIT_INT ) )	{	// 이동작중에는 암거두 할수 없다.
		return 1;
	}

	if ( 2 == Get_PatHP_MODE() ) 
		Set_PatHP_MODE( 3 );

	switch( Get_COMMAND() ) {
		case CMD_DIE		:
			return this->IsUSER ();

		case CMD_SIT		:
			return ProcCMD_SIT ();

		case CMD_STOP		:
			return ProcCMD_STOP ();

		case CMD_MOVE		:
			return ProcCMD_MOVE ();

		case CMD_RUNAWAY	:
		case CMD_PICK_ITEM	:
			return ProcCMD_PICK_ITEM ();

		case CMD_ATTACK		:
			return ProcCMD_ATTACK ();

		case CMD_SKILL2SELF :
			return ProcCMD_Skill2SELF ();

		case CMD_SKILL2POS	:
			return ProcCMD_Skill2POSITION ();

		case CMD_SKILL2OBJ	:
			return ProcCMD_Skill2OBJECT ();
	}

	return 1;
}

void CObjCHAR::Proc_IngSTATUS (DWORD dwPassTime)
{
	// 무적 빼고 지속상태가 설정됐다.
	DWORD dwClearedFLAGS = this->m_IngSTATUS.Proc( this, dwPassTime );
	if ( dwClearedFLAGS ) {
		if ( dwClearedFLAGS & ( ING_INC_MOV_SPD | ING_DEC_MOV_SPD ) ) {
			// 이속에 변화가 생겼고 현재 모션이 이동 모션이면...이속 수정
			if ( m_pCurMOTION == this->Get_MOTION( this->GetANI_Move() ) ) {
				m_fCurMoveSpeed = this->Get_MoveSPEED();
			}
		}
		if ( dwClearedFLAGS & ( ING_INC_ATK_SPD | ING_DEC_ATK_SPD ) ) {
			// 공속에 변화가 생겼고 현재 모션이 공격 모션이면...공속 수정
			if ( m_pCurMOTION == this->Get_MOTION(this->GetANI_Attack() ) ) {
				m_fCurAniSPEED = ( this->Get_nAttackSPEED() / 100.f );	
			}
		}

		// 상태가 바뀌었다....
		this->Send_gsv_CLEAR_STATUS( dwClearedFLAGS );
	}
}

int CObjCHAR::Proc (void)
{
	if ( NULL == m_pCurMOTION ) {
		g_LOG.CS_ODS( 0xffff, "		**** ERROR:: m_pCurMOTION == NULL CObjCHAR::Proc[ %s ] ..\n", this->Get_NAME() );
		return 0;
	}

	DWORD dwNeedTIME, dwPassTIME = (this->GetZONE())->GetPassTIME ();
	WORD  wRamainFRAME;

	// 지속상태 처리...
	if ( this->Get_HP() > 0 && this->m_IngSTATUS.GetFLAGs() ) {
		this->Proc_IngSTATUS( dwPassTIME );
		//// 무적 빼고 지속상태가 설정됐다.
		//dwNeedTIME = this->m_IngSTATUS.Proc( this, dwPassTIME );
		//if ( dwNeedTIME ) {
		//	if ( dwNeedTIME & ( ING_INC_MOV_SPD | ING_DEC_MOV_SPD ) ) {
		//		// 이속에 변화가 생겼고 현재 모션이 이동 모션이면...이속 수정
		//		if ( m_pCurMOTION == this->Get_MOTION( this->GetANI_Move() ) ) {
		//			m_fCurMoveSpeed = this->Get_MoveSPEED();
		//		}
		//	}
		//	if ( dwNeedTIME & ( ING_INC_ATK_SPD | ING_DEC_ATK_SPD ) ) {
		//		// 공속에 변화가 생겼고 현재 모션이 공격 모션이면...공속 수정
		//		if ( m_pCurMOTION == this->Get_MOTION(this->GetANI_Attack() ) ) {
		//			m_fCurAniSPEED = ( this->Get_nAttackSPEED() / 100.f );	
		//		}
		//	}

		//	// 상태가 바뀌었다....
		//	this->Send_gsv_CLEAR_STATUS( dwNeedTIME );
		//}
	}

	// 현재 동작의 남은 프레임을 계산...
	wRamainFRAME = m_pCurMOTION->Get_ReaminFRAME( m_iCurMotionFRAME );

	//assert( m_pCurMOTION->Get_TotalFRAME() == ( m_iCurMotionFRAME + wRamainFRAME ) );
	//assert( m_fCurAniSPEED != 0.f );

	// 남은 프레임동안 소요될 시간계산...
	dwNeedTIME = m_pCurMOTION->Get_NeedTIME ( wRamainFRAME, m_fCurAniSPEED );

	// 2003.09.19 >= 를 >로 해서 프레임이 넘어가는 오류가 있었음...
	while( dwPassTIME >= dwNeedTIME ) {
		// 모션이 완료된다..
		m_iCurMotionFRAME += wRamainFRAME;
		if ( !this->RealProc( dwNeedTIME ) )
			return 0;

		dwPassTIME  -= dwNeedTIME;

		m_iCurMotionFRAME = 0, m_fAccMotionFRAME = 0.f;

		wRamainFRAME = m_pCurMOTION->Get_TotalFRAME ();
		dwNeedTIME   = m_pCurMOTION->Get_NeedTIME( wRamainFRAME, m_fCurAniSPEED );
	}

	// 현재동작을 dwPassTIME만큼 진행...
	// 초당 30 frame 일경우 전체 프레임이 30보다 작으면???
	float fPassFrame = m_pCurMOTION->Get_PassFRAME( dwPassTIME, m_fCurAniSPEED );
	m_fAccMotionFRAME += fPassFrame;
	wRamainFRAME = (WORD)m_fAccMotionFRAME;
	if ( wRamainFRAME ) {
		m_fAccMotionFRAME -= wRamainFRAME;
		m_iCurMotionFRAME += wRamainFRAME;
		return this->RealProc( dwPassTIME );
	}

	return 1;
}

//-------------------------------------------------------------------------------------------------
