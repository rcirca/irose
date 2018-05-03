/*
	$Header: /7HeartsOnline/Server/SHO_GS/Sho_gs_lib/Common/CObjAI.h 30    05-09-21 4:29p Icarus $
*/
#ifndef	__CCOMMAND_H
#define	__CCOMMAND_H
#include "CGameOBJ.h"
#include "IO_Motion.h"
#include "IO_Skill.h"
#include "CAI_LIB.h"
//-------------------------------------------------------------------------------------------------
//
// Character state
//
#define	CS_BIT_INT		0x1000					// 동작중 인터럽트 걸리면 안되는것..
#define CS_BIT_ADJ		0x2000					// 동작중 좌표 보정할것.
#define	CS_BIT_CHK		0x4000					// 동작 프레임 체크..
#define CS_BIT_ONE	  (	0x8000 | CS_BIT_CHK )	// 동작이 1번 루프 돌고 끝날것.
#define CS_BIT_MOV		0x0100					// 이동중이다.
#define	CS_BIT_INT2	  (	0x0200 | CS_BIT_INT )	// 히트 동작조차도 적용안된다..
#define	CS_BIT_MOV2		0x0400					// 상태가 바뀌었지만 이전 이동한 수치만큼은 적용..

#define	CS_BIT_ATTACK	0x0002

#define	CS_STOP		  (	0x0000									  )
#define	CS_MOVE		  (	0x0001			| CS_BIT_ADJ | CS_BIT_MOV )
#define	CS_ATTACK	  (	CS_BIT_ATTACK	| CS_BIT_INT | CS_BIT_ONE )
#define CS_HIT		  ( 0x0003			| CS_BIT_INT | CS_BIT_ONE )
#define CS_FALL		  ( 0x0004			| CS_BIT_INT | CS_BIT_ONE )
#define CS_RAISE	  ( 0x0005			| CS_BIT_INT | CS_BIT_ONE )
#define CS_SITTING	  ( 0x0005			| CS_BIT_INT | CS_BIT_ONE )
#define CS_SIT		  ( 0x0006			| CS_BIT_INT			  )		// 지속 되는 동작
#define CS_STANDING	  ( 0x0007			| CS_BIT_INT | CS_BIT_ONE )
#define CS_NEXT_STOP  ( 0x0008			| CS_BIT_INT | CS_BIT_ONE )
#define CS_NEXT_STOP2 ( 0x0009			| CS_BIT_INT2| CS_BIT_ONE )
#define	CS_DIE		  (	0x0010			| CS_BIT_INT | CS_BIT_ONE )
#define CS_CASTING	  ( 0x0011			| CS_BIT_INT2| CS_BIT_ONE )

#define	CMD_BIT_INT		0x8000					// 명령중 인터럽트 걸리면 안되는것..

#define	CMD_STOP		0x0000
#define	CMD_MOVE		0x0001
#define	CMD_ATTACK		0x0002
#define	CMD_DIE			0x0003
#define	CMD_PICK_ITEM	0x0004
#define CMD_SKILL2SELF	0x0006
#define	CMD_SKILL2OBJ	0x0007
#define	CMD_SKILL2POS	0x0008
#define	CMD_RUNAWAY	  ( 0x0009			| CMD_BIT_INT )
#define	CMD_SIT			0x000a

#define	CMD_STORE		0x000b

/// 공통액션들
enum
{
	COMMON_COMMAND_SIT			= 1,
	COMMON_COMMAND_PICK_ITEM	= 2,
	COMMON_COMMAND_JUMP			= 3,
	COMMON_COMMAND_AIR_JUMP		= 4,
	COMMON_COMMAND_AUTO_TARGET	= 5,
	COMMON_COMMAND_ATTACK		= 6,
};


#define	WALK_CmPerSec			200

#define	WEIGHT_RATE_WALK		100
#define WEIGHT_RATE_CANT_ATK	110


//-------------------------------------------------------------------------------------------------

class CObjCHAR;
class CObjAI;
class CZoneTHREAD;


///
///	\class CObjTARGET
/// \brief class for object that has target.
///
class CObjTARGET 
{
#ifdef	__SERVER
private :
#else
public  :
#endif
	int						m_iServerTarget;		/// 공격할 서버 객제 번호

protected :
	CAI_OBJ *				Get_TargetOBJ (bool bCheckHP);

public  :
			int				Get_TargetIDX ()								{	return m_iServerTarget;			}
	virtual void			Set_TargetIDX (int iTargetIndex, bool bHP=true)	{	m_iServerTarget=iTargetIndex;	}
	virtual CZoneTHREAD*	GetCur_ZONE () = 0;
	CObjTARGET ()	{		m_iServerTarget = 0;	}
} ;


#define	RECOVER_JUST_CHECK_TIME			0		/// 암것도 안하고 시간만 간다.
#define	RECOVER_STATE_CHECK_TIME		8000	/// 4초당 회복..
#define	USE_FUEL_CHEC_TIME				10000	/// 10초당 소모..
#define	RECOVER_STATE_SIT_ON_CHAIR		30		/// 의자 앉기		:: nRecoverMODE = 30
#define	RECOVER_STATE_SIT_ON_GROUND		20		/// 바닥 앉기       :: nRecoverMODE = 20
#define RECOVER_STATE_STOP_OR_WALK		8		/// 정지,걷기 상태	:: nRecoverMODE = 8

/**
 * \ingroup SHO_GS_LIB
 * \class	CObjAI
 * \author	wookSang.Jo
 * \brief	케릭터 인공지능 처리 클래스
 *			인공지능 데이타클래스(CAI_OBJ)를 상속 받음
 */
class CObjAI : public CGameOBJ, public CAI_OBJ, public CObjTARGET 
{
private	  :
	WORD				m_wState;
	WORD				m_wCommand;
	WORD				m_wBeforeCMD;			/// self skill 사용시 현재 명령 저장..			

	CZoneTHREAD*		GetCur_ZONE ()	{	return this->GetZONE();		}

protected :

	//////////////////////////////////////////////////////////////////////////////////////////
	///>
	///> CObjAI virtual functions
	///>

	virtual bool		IsInRANGE (CObjCHAR *pT, int iRange)=0;
	virtual void		ActionEVENT (int iActionIDX)=0;
	virtual void		Adjust_HEIGHT () = 0;
	virtual int			GetCurrentFrame () = 0;
	virtual bool		Set_MOTION (short nActionIdx, float fMoveSpeed=0, float fAniSpeed=1.0f, bool bAttackMotion=false)=0;
//	virtual void		Move_COMPLETED ()=0;

	virtual int			GetANI_Stop()=0;
	virtual int			GetANI_Die()=0;
	virtual int			GetANI_Hit()=0;
	virtual int			GetANI_Move()=0;
	virtual int			GetANI_Attack()=0;
	virtual int			GetANI_Casting()=0;
	virtual int			GetANI_Skill()=0;
	virtual int			GetANI_Sitting()	{	return GetANI_Stop();	}
	virtual int			GetANI_Standing()	{	return GetANI_Stop();	}
	virtual int			GetANI_Sit()		{	return GetANI_Stop();	}

	virtual int			Def_AttackRange()=0;
	virtual int			Get_R_WEAPON ()=0;
	virtual bool		Attack_START (CObjCHAR *pTarget)=0;
	virtual bool		Skill_START (CObjCHAR *pTarget)=0;
	virtual bool		Casting_START( CObjCHAR* pTarget )=0;
	virtual void		Casting_END ()=0;

	virtual bool		ToggleRunMODE ()				{	return true;	}
	virtual bool		ToggleSitMODE ()				{	return true;	}

#ifndef	__SERVER
	virtual void		Attack_END (bool bStopTrail=true) = 0;
	virtual int			Get_L_WEAPON ()=0;

	/// 실제 스킬동작을 할수 있는가?( 서버로 부터 실제 결과를 받았는가? )
	virtual bool		bCanActionActiveSkill() = 0;
	virtual void		SetEffectedSkillFlag( bool bResult )= 0;

	virtual void		Set_ModelDIR (t_POSITION &PosToView)=0;
	virtual void		Set_ModelDIR( float fAngleDegree ) = 0;
	virtual void		Set_ModelSPEED (float fSpeed)=0;
#endif


	/// 아래 3개의 함수는 자신일경우 자체 계산된 값 / 파티원일경우 서버에서 받은 값 리턴
	virtual int			Get_CON()						{	return 0;	}	// 유저일 경우 return pAVATAR->GetCur_CON();
	virtual int			GetAdd_RecoverHP()				{	return 0;	}	// 유저일 경우 return pAVATAR->m_btRecoverHP;
	virtual int			GetAdd_RecoverMP()				{	return 0;	}	// 유저일 경우 return pAVATAR->m_btRecoverHP;
	
	
#ifndef	__SERVER
public:
			float		m_fAdjustSPEED;
	virtual void		Adj_MoveSPEED (tPOINTF PosCUR, tPOINTF PosGOTO)=0;
	virtual float		Get_DefaultSPEED ()=0;
#endif


protected:
	virtual short		GetPsv_ATKSPEED ()				{	return 0;		}
	virtual short		Get_nAttackSPEED()=0;			/// 기본속도 100을 기준 :: 기본 공격 속도( m_fAtkAniSPEED ) + 스킬에 의해 보정된 값 ...
	virtual float		Get_MoveSPEED ()=0;
	virtual int			Get_AttackRange(short /*nSKillIDX*/)=0;
	virtual float		Get_SCALE() = 0;

public:
	virtual int			Get_HP ()	= 0;
	virtual int			Get_MP ()	= 0;
	virtual int			Get_MaxHP() = 0;
	virtual int			Get_MaxMP() = 0;
	virtual short		Get_WeightRATE()				{	return 0;		}

protected:
	virtual DWORD		GetIngDurationStateFLAG() = 0;						/// 현재 지속 상태 플레그값을 얻는다.
	virtual short		GetIngDurationStateSKILL( eING_TYPE eTYPE ) = 0;	/// 현재 지속 상태에 적용된 스킬번호

public:
	virtual void		Set_HP( int iHP )									{	;	};
	virtual void		Set_MP( int iMP )									{	;	};

protected :
	virtual bool		SetCMD_ATTACK (int iServerTarget);

public    :
	virtual bool		SkillChk_ACTIVE ( short /*nSkillIDX*/ )				{	return true;	}
	virtual void		SkillUse_ABILITY( short /*nSkillIDX*/ )				{	;				}

	virtual bool		SetCMD_SIT (void);
	virtual bool		SetCMD_STAND (void);
	virtual bool		SetCMD_MOVE (tPOINTF &PosFROM, tPOINTF &PosTO, int iServerTarget);
	virtual void		SetCMD_DIE ();
//	virtual bool		SetCMD_TOGGLE (BYTE btTYPE);

	virtual bool		SetCMD_Skill2SELF (short nSkillIDX);
	virtual bool		SetCMD_Skill2OBJ (int iServerTarget, short nSkillIDX);
	virtual bool		SetCMD_Skill2POS (tPOINTF &PosGOTO, short nSkillIDX);

	virtual void		Do_CreatedAI	(void)								{	/* nop */	}
	virtual void		Do_StopAI		(void)								{	/* nop */	}
	virtual void		Do_AttackMoveAI (CObjCHAR *pTarget)					{	/* nop */	}
	virtual void		Do_DamagedAI	(CObjCHAR *pTarget, int iDamage)	{	/* nop */	}
	virtual void		Do_KillAI		(CObjCHAR *pTarget, int iDamage)	{	/* nop */	}
	virtual void		Do_DeadAI		(CObjCHAR *pTarget, int iDamage)	{	/* nop */	}	
	virtual void		Do_DeadEvent	(CObjCHAR *pTarget)	{	/* nop */	}	

	virtual bool		Is_SameTYPE(int iType)			{	return (iType==this->Get_TYPE());	}
	virtual bool		Is_TauntSTATUS(int iTargetObject)=0;

	virtual bool		SetCMD_STOP ();
	virtual bool		SetCMD_MOVE2D (float fPosX, float fPosY, BYTE btRunMODE);
	virtual void		SetCMD_RUNnATTACK(int iTargetObjTAG)
	{	
		// 인공지능에서 호출되는 공격 명령 !!!
		if ( !Is_TauntSTATUS( iTargetObjTAG ) ) {
			if ( this->SetCMD_ATTACK (iTargetObjTAG) )		// MOB AI SetCMD_RUNnATTACK()... 
				this->m_bRunMODE = true;
		}
	}
	virtual bool		Send_gsv_SKILL_CANCEL( BYTE btWhy )=0;

	virtual short		Get_PatHP_MODE()			{	return 0;	}
	virtual void		Set_PatHP_MODE(short nV)	{	/* nop */	}
	/// > End
	//////////////////////////////////////////////////////////////////////////////////////////



	tagMOTION*			m_pCurMOTION;
	int					m_iCurMotionFRAME;
	float				m_fAccMotionFRAME;

	bool				Set_CurMOTION( tagMOTION* pMotion );
	bool				Chg_CurMOTION( tagMOTION* pMotion );

	#ifndef	__SERVER
		HNODE			Get_ZMOTION()			{	return m_pCurMOTION->m_hMotion;		}
		short			Get_ActionIDX()			{	return m_pCurMOTION->m_nActionIdx;	}
	#endif

	void				Start_MOVE (float fSpeed);
	void				Restart_MOVE (t_POSITION &PosGOTO);

	void				Start_ATTACK (CObjCHAR *pTarget);

private:
	char				Do_SKILL (int iServerTarget, CObjCHAR *pTarget=NULL);

public :
	bool				Goto_TARGET( CObjCHAR *pTarget, int iRange );
	bool				Goto_POSITION (int iRange=0);


	int					ProcCMD_SIT ();
	int					ProcCMD_STOP ();
	int					ProcCMD_MOVE ();
	int					ProcCMD_PICK_ITEM ();
	int					ProcCMD_ATTACK ();
	int					ProcCMD_Skill2SELF ();
	int					ProcCMD_Skill2POSITION ();
	int					ProcCMD_Skill2OBJECT ();

	
	short				Get_RecoverHP( short nRecoverMODE )	{	return ( this->GetAdd_RecoverHP() + ( this->Get_CON()+30 ) / 8 * (nRecoverMODE+3) / 10 );	}
	short				Get_RecoverMP( short nRecoverMODE )	{	return ( this->GetAdd_RecoverMP() + ( this->Get_CON()+20 ) / 10 * nRecoverMODE / 7 );		}

	
	

public    :
	bool				m_bAttackSTART;
	bool				m_bCastingSTART;

	int					m_iWaitLoopCnt;			/// 서버로부터의 결과를 기다리며..

	short				m_nToDoSkillIDX;		/// 사용할(설정된) 스킬

private :
	short				m_nActiveSkillIDX;		/// 현재 동작에 적용된 (캐스팅중) 스킬

public :
	short				Get_ActiveSKILL()		{	return m_nActiveSkillIDX;					}
	void				Del_ActiveSKILL()		{	m_bCastingSTART=false, m_nActiveSkillIDX=0;	}

	int					m_iActiveObject;		/// 현재 동작에 적용된 타켓 :: 클라이언트 객체 번호 
	short				m_nDoingSkillIDX;		/// 현재 진행중인 스킬( 헤비어택같은 근거리 타격스킬을 위해서 )

	float				m_fRunAniSPEED;

	float				m_fCurMoveSpeed;

	int					m_iMoveDistance;
	t_POSITION			m_PosBORN;
	t_POSITION			m_PosMoveSTART;
	t_POSITION			m_PosGOTO;
	tPOINTF				m_MoveVEC;		// 1/1000 당 이동량
    BYTE				m_bRunMODE;



public:
	CObjAI ();
	virtual ~CObjAI ();

	bool				ProcMotionFrame (void);

	void				Set_STATE( WORD wState )				{	m_wState = wState;								}	/// 상태 설정
	WORD				Get_STATE ()							{	return m_wState;								}
	WORD				Get_COMMAND ()							{	return m_wCommand;								}
	void				Set_COMMAND (WORD wCommand)			{	m_wCommand = wCommand;							}
	float				Get_MoveAniSPEED()						{	return ( m_bRunMODE ) ? m_fRunAniSPEED : 1.0f;	}

	void				Reset_MoveVEC ();
	float				Cal_AtkAniSPEED( short nRightWeaponItemNO );
	float				Cal_RunAniSPEED( short nCmPerSec )
	{
		// 이동 동작 딜레이 = ( 이동속도+180) /600
		return ( (nCmPerSec+180.f) / 600.f );
	}

public :	
	int					Get_TAG ()							{	return this->Get_INDEX();			}
	int					Get_ObjTYPE()						{	return this->Get_TYPE();			}
	bool				Is_AVATAR()							{	return this->IsUSER();				}
	int					Get_PercentHP()
	{
		assert( this->Get_MaxHP() );
		return ( this->Get_HP()*100 / this->Get_MaxHP() );
	}

	CAI_OBJ *			Get_TARGET (bool bCheckHP=true)		{	return this->Get_TargetOBJ(bCheckHP);	}	
	void				Set_TargetObjIDX (int iTargetOBJ)	{	this->Set_TargetIDX(iTargetOBJ);		}	
} ;


#define	__BLOCK_WHEN_SKILL


//-------------------------------------------------------------------------------------------------
#endif
