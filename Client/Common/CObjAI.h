/*
	$Header: /Client/Common/CObjAI.h 132   05-10-27 12:27p Gioend $
*/
#ifndef	__CCOMMAND_H
#define	__CCOMMAND_H
#include "CGameOBJ.h"
#include "IO_Motion.h"
#include "IO_Skill.h"
#include "..\AI_LIB\CAI_LIB.h"


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

/// 공통액션들
enum
{
	COMMON_COMMAND_SIT			= 1,
	COMMON_COMMAND_PICK_ITEM	= 2,
	COMMON_COMMAND_JUMP			= 3,
	COMMON_COMMAND_AIR_JUMP		= 4,
	COMMON_COMMAND_AUTO_TARGET	= 5,
	COMMON_COMMAND_ATTACK		= 6,
	COMMON_COMMAND_DRIVE_CART	= 7,
	COMMON_COMMAND_ADD_FRIEND	= 8,
	COMMON_COMMAND_PARTY		= 9,	///파티결성, 파티조인신청
	COMMON_COMMAND_EXCHANGE		= 10,	///거래신청
	COMMON_COMMAND_PRIVATESTORE = 11,
	COMMON_COMMAND_SELFTARGET   = 12,
	COMMON_COMMAND_BOARD_CART	= 13,	// 카트 탑승 스킬.
};


#define	WALK_CmPerSec	200

//-------------------------------------------------------------------------------------------------

class CObjCHAR;
class CObjAI;


///
///	class for object that has target.
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
	CAI_OBJ *				Get_TargetOBJ ();

public  :
			int				Get_TargetIDX ()						{	return m_iServerTarget;			}
	virtual void			Set_TargetIDX (int iTargetIndex)		{	m_iServerTarget=iTargetIndex;	}

	CObjTARGET ()	{		m_iServerTarget = 0;	}
} ;



#define	RECOVER_JUST_CHECK_TIME			0		/// 암것도 안하고 시간만 간다.
#define	RECOVER_STATE_CHECK_TIME_OLD	4000	/// 4초당 회복..
#define RECOVER_STATE_CHECK_TIME_NEW	8000	/// 8초당 회복

#define	RECOVER_STATE_SIT_ON_CHAIR		30		/// 의자 앉기		:: nRecoverMODE = 30 : 현재 없는 상태( 2005/5/20 )

#define	RECOVER_STATE_SIT_ON_GROUND_OLD	20		/// 바닥 앉기       :: nRecoverMODE = 20
#define	RECOVER_STATE_SIT_ON_GROUND_NEW	25		/// 바닥 앉기       :: nRecoverMODE = 25//대만, 필리핀 적용( 2005/5/19 )

#define RECOVER_STATE_STOP_OR_WALK_OLD	8		/// 정지,걷기 상태	:: nRecoverMODE = 8
#define RECOVER_STATE_STOP_OR_WALK_NEW	5		/// 정지,걷기 상태	:: nRecoverMODE = 5//대만, 필리핀 적용( 2005/5/19 )
///
///	CObjAI class 
///
class CObjAI : public CGameOBJ, public CAI_OBJ, public CObjTARGET 
{
private	  :
	WORD				m_wState;
	WORD				m_wCommand;
	WORD				m_wBeforeCMD;			/// self skill 사용시 현재 명령 저장..			



protected :

	//////////////////////////////////////////////////////////////////////////////////////////
	///>
	///> CObjAI virtual functions
	///>

	virtual bool		IsInRANGE (CObjCHAR *pT, int iRange)=0{ *(int*)0 = 10; };
	virtual void		ActionEVENT (int iActionIDX)=0{ *(int*)0 = 10; };
	virtual void		Adjust_HEIGHT () = 0{ *(int*)0 = 10; };
	virtual int			GetCurrentFrame () = 0{ *(int*)0 = 10; };
	virtual void		Set_MOTION (short nActionIdx, float fMoveSpeed=0, float fAniSpeed=1.0f, bool bAttackMotion=false, int iRepeatCnt = 0 )=0{ *(int*)0 = 10; };
	virtual void		Move_COMPLETED ()=0{ *(int*)0 = 10; };

	virtual int			GetANI_Stop()=0{ *(int*)0 = 10; };
	virtual int			GetANI_Die()=0{ *(int*)0 = 10; };
	virtual int			GetANI_Hit()=0{ *(int*)0 = 10; };
	virtual int			GetANI_Move()=0{ *(int*)0 = 10; };
	virtual int			GetANI_Attack()=0{ *(int*)0 = 10; };
	virtual int			GetANI_Casting()=0{ *(int*)0 = 10; };
	virtual int			GetANI_CastingRepeat()=0{ *(int*)0 = 10; };
	virtual int			GetANI_Skill()=0{ *(int*)0 = 10; };
	virtual int			GetANI_Sitting()	{	return GetANI_Stop();	}
	virtual int			GetANI_Standing()	{	return GetANI_Stop();	}
	virtual int			GetANI_Sit()		{	return GetANI_Stop();	}
	virtual int			GetANI_Ride()		{   return 0; }
	virtual int			GetANI_PickITEM()	{   return GetANI_Stop();	}	

	virtual int			Def_AttackRange()=0{ *(int*)0 = 10; };
	virtual int			Get_R_WEAPON ()=0{ *(int*)0 = 10; };
	virtual bool		Attack_START (CObjCHAR *pTarget)=0{ *(int*)0 = 10; };
	virtual bool		Skill_START (CObjCHAR *pTarget)=0{ *(int*)0 = 10; };
	virtual bool		Casting_START( CObjCHAR* pTarget )=0{ *(int*)0 = 10; };
	virtual void		Casting_END ()=0{ *(int*)0 = 10; };
	virtual void		MoveStart (){ *(int*)0 = 10; };

	virtual bool		ToggleRunMODE ( float fAdjRate )				{	return true;	}
	virtual bool		ToggleSitMODE ()				{	return true;	}

#ifndef __SERVER
	virtual void		RideCartToggle( bool bRide ){};
#endif


#ifndef	__SERVER
	virtual void		Attack_END (bool bStopTrail=true) = 0{ *(int*)0 = 10; };
	virtual int			Get_L_WEAPON ()=0{ *(int*)0 = 10; };	
	virtual void		SetMotionRepeatCount( int iRepeatCount ) = 0{ *(int*)0 = 10; };

	/// 실제 스킬동작을 할수 있는가?( 서버로 부터 실제 결과를 받았는가? )
	virtual bool		bCanActionActiveSkill() = 0{ *(int*)0 = 10; };
	virtual void		SetEffectedSkillFlag( bool bResult )= 0{ *(int*)0 = 10; };
	virtual bool		bCanStartSkill() = 0{ *(int*)0 = 10; };							/// 서버로 부터 캐스팅 스타트 패킷을 받았는가?
	virtual void		SetStartSkill( bool bResult ) = 0{ *(int*)0 = 10; };

	virtual void		Set_ModelDIR (t_POSITION &PosToView, bool bImmediate = false )=0{ *(int*)0 = 10; };
	virtual void		Set_ModelDIR( float fAngleDegree, bool bImmediate = false ) = 0{ *(int*)0 = 10; };
	virtual void		Set_ModelSPEED (float fSpeed)=0{ *(int*)0 = 10; };
	virtual float		Get_ModelSPEED () = 0{ *(int*)0 = 10; };
#endif


	/// 아래 3개의 함수는 자신일경우 자체 계산된 값 / 파티원일경우 서버에서 받은 값 리턴
	virtual int			Get_CON()					{	return 0;	}	// 유저일 경우 return pAVATAR->GetCur_CON();
	virtual int			GetAdd_RecoverHP()			{	return 0;	}	// 유저일 경우 return pAVATAR->m_btRecoverHP;
	virtual int			GetAdd_RecoverMP()			{	return 0;	}	// 유저일 경우 return pAVATAR->m_btRecoverHP;
	
	
	/// RECOVER_STATE_CHECK_TIME 간격으로 Get_RecoverHP에서 구한 HP, Get_ReocverMP에서 구한 MP를 더해 준다.	
	virtual void		RecoverHP( short nRecoverMODE )		{	/* nop */	}	
	virtual void		RecoverMP( short nRecoverMODE )		{	/* nop */	}

	

#ifndef	__SERVER
public:
			float		m_fAdjustSPEED;
	virtual void		Adj_MoveSPEED ( WORD wSrvDIST, const D3DVECTOR& PosGOTO)=0{ *(int*)0 = 10; };
	virtual void		Adj_AniSPEED ( float fAniSpeed )=0{ *(int*)0 = 10; };;
	virtual float		Get_DefaultSPEED ()=0{ *(int*)0 = 10; };
	
	virtual void		Reset_Position () = 0{ *(int*)0 = 10; }; /// 이동 시작시, 현재 위치를 재조정
#endif


protected:
	virtual short		GetPsv_ATKSPEED (float fCurSpeed, short nRightWeaponItemNo)				{	return 0;		}
	virtual short		GetPsv_ATKSPEED()	{ return 0; }
	virtual float		Get_fAttackSPEED()=0{ *(int*)0 = 10; };			/// 기본속도 1.0을 기준 :: 기본 공격 속도( m_fAtkAniSPEED ) + 스킬에 의해 보정된 값 ...
	virtual float		Get_MoveSPEED ()=0{ *(int*)0 = 10; };
	virtual int			Get_AttackRange () = 0{ *(int*)0 = 10; };
	virtual float		Get_SCALE() = 0{ *(int*)0 = 10; };

public:
	virtual int			Get_HP ()	= 0{ *(int*)0 = 10; };
	virtual int			Get_MP ()	= 0{ *(int*)0 = 10; };
	virtual int			Get_MaxHP() = 0{ *(int*)0 = 10; };
	virtual int			Get_MaxMP() = 0{ *(int*)0 = 10; };



protected:
	virtual DWORD		GetIngDurationStateFLAG() = 0{ *(int*)0 = 10; };						/// 현재 지속 상태 플레그값을 얻는다.
	virtual short		GetIngDurationStateSKILL( eING_TYPE eTYPE ) = 0{ *(int*)0 = 10; };	/// 현재 지속 상태에 적용된 스킬번호

public:
	virtual void		Set_HP( int iHP ){};
	virtual void		Set_MP( int iMP ){};

protected :
	virtual bool		SetCMD_ATTACK (int iServerTarget);

public    :
	virtual void		SetCMD_SIT (void);
	virtual void		SetCMD_STAND (void);
	virtual void		SetCMD_MOVE ( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget);
	virtual void		SetCMD_DIE ();
	virtual bool		SetCMD_TOGGLE (BYTE btTYPE);
	
	virtual bool		SetCMD_STOP ();
	virtual bool		SetCMD_MOVE2D ( float fPosX, float fPosY, BYTE btRunMODE );
	virtual bool		SetCMD_MOVE ( const D3DVECTOR& vPosTo, BYTE btRunMODE );
	
	virtual void		SetCMD_Skill2SELF (short nSkillIDX);
	virtual bool		SetCMD_Skill2OBJ ( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget, short nSkillIDX);
	virtual void		SetCMD_Skill2POS ( const D3DVECTOR& PosGOTO, short nSkillIDX);

	virtual void		SetCMD_RUNnATTACK(int iTargetObjTAG)
	{	
		if ( this->SetCMD_ATTACK (iTargetObjTAG) )		// MOB AI SetCMD_RUNnATTACK()... 
			this->m_bRunMODE = true;
	}

	virtual void		Do_CreatedAI	(void)								{	/* nop */	}
	virtual void		Do_StopAI		(void)								{	/* nop */	}
	virtual void		Do_AttackMoveAI (CObjCHAR *pTarget)					{	/* nop */	}
	virtual void		Do_DamagedAI	(CObjCHAR *pTarget, int iDamage)	{	/* nop */	}
	virtual void		Do_KillAI		(CObjCHAR *pTarget, int iDamage)	{	/* nop */	}
	virtual void		Do_DeadAI		(CObjCHAR *pTarget, int iDamage)	{	/* nop */	}	
	virtual void		Do_DeadEvent	(CObjCHAR *pTarget)	{	/* nop */	}	


	virtual bool		Is_SameTYPE(int iType)			{	return (iType==this->Get_TYPE());	}

	



#ifndef	__SERVER
	///
	/// Managing avt's action mode
	///
	virtual void		ChangeActionMode( int iActionMode ){};	

	///
	/// Check PET MODE
	/// Just avt have to override this virtual methodes.
	/// if mode < 0, this char is normal mode
	virtual int			GetPetMode(){ return -1; }
	virtual bool		CanAttackPetMode(){ return false; }
	
	virtual void		SetCMD_PET_MOVE( const D3DVECTOR& PosTO, BYTE btRunMODE ){};
	virtual void		SetCMD_PET_MOVE( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget){};
	virtual void		SetCMD_PET_STOP(){};
	virtual void		SetCMD_PET_ATTACK ( int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosTO ){ };
	virtual int		Get_WorldTIME(void);

//-----------------------------------------------------------------------------------------------
	//박지호:: 가상함수 재선언 
	//카트 타켓 스킬을 사용하기 위해서 선언함 
	virtual int		ProcCMD_Skill2OBJECT_PET() { return 0; };
	//카트 스킬 사용후 다음 행동을 정의 하기 위해 선언함 .
	virtual void		SetNewCommandAfterSkill_PET( int iSkillNO ) {  }	
//-----------------------------------------------------------------------------------------------


#endif


	/// > End
	//////////////////////////////////////////////////////////////////////////////////////////








	tagMOTION*			m_pCurMOTION;
	int					m_iCurMotionFRAME;

	bool				Set_CurMOTION( tagMOTION* pMotion );
	bool				Chg_CurMOTION( tagMOTION* pMotion );

	#ifndef	__SERVER
		HNODE			Get_ZMOTION()			{	return m_pCurMOTION->m_hMotion;		}
		short			Get_ActionIDX()			{	return m_pCurMOTION->m_nActionIdx;	}
	#endif

	void				Start_MOVE (float fSpeed);
	void				Restart_MOVE (t_POSITION &PosGOTO);
    void                Restart_MOVE_AL (t_POSITION &PosGOTO);

	void				Start_ATTACK (CObjCHAR *pTarget);
	char				Do_SKILL (int iServerTarget, CObjCHAR *pTarget=NULL);
	void				SetSkillDelay( int iSkillIdx );

	void				SetNewCommandAfterSkill( int iSkillNO );
	int					ProcSkillCastingAction( int iServerTarget, CObjCHAR* pTarget );
	int					ProcSkillCastingLoop( CObjCHAR *pTarget );
	int					ProcSkillAction( CObjCHAR *pTarget );					///< 실제 스킬 액션동작을 한다.


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

	
	short				Get_RecoverHP( short nRecoverMODE );
	short				Get_RecoverMP( short nRecoverMODE );

	virtual int			GetOri_MaxHP();
	virtual int			GetOri_MaxMP();



public    :
	bool				m_bAttackSTART;
	bool				m_bCastingSTART;
	int					m_iCastingStartTime;	/// 디버깅을 위한 변수...
		

	int					m_iWaitLoopCnt;			/// 서버로부터의 결과를 기다리며..

	int					m_SkillActionState;		///< 현재 스킬의 동작상태( 캐스팅중인가? 대기 루프 중인가? 실제 액션 중인가? )
	short				m_nToDoSkillIDX;		/// 사용할(설정된) 스킬

	short				m_nActiveSkillIDX;		/// 현재 동작에 적용된 (캐스팅중) 스킬
	int					m_iActiveObject;		/// 현재 동작에 적용된 타켓 :: 클라이언트 객체 번호 
	short				m_nDoingSkillIDX;		/// 현재 액션 진행중인 스킬번호. 액션동작 시작시 세팅하고 각 모션 종료시 리셋..
	

	float				m_fRunAniSPEED;

	float				m_fCurMoveSpeed;

	int					m_iMoveDistance;
	t_POSITION			m_PosBORN;
	t_POSITION			m_PosMoveSTART;
	t_POSITION			m_PosGOTO;
	tPOINTF				m_MoveVEC;		// 1/1000 당 이동량
    BYTE				m_bRunMODE;
	BYTE				m_btMoveMODE;	// 0:걷기, 1:뛰기, 2:타기



public:
	CObjAI ();
	virtual ~CObjAI ();

	void				SetCastingState( bool bStart );
	bool				ProcOneActionFrame( int iIndex );
	bool				ProcMotionFrame (void);

	void				Set_STATE( WORD wState )				{	m_wState = wState;								}	/// 상태 설정
	WORD				Get_STATE ()							{	return m_wState;								}
	WORD				Get_COMMAND ()							{	return m_wCommand;								}
	void				Set_COMMAND (WORD wCommand)				{	m_wCommand = wCommand;							}
	
//--------------------------------------------------------------------------------------------------------------------
	//박지호:: 이전 명령을 컨트롤 하기위해 정의함.
	WORD				Get_BECOMMAND()							{	return m_wBeforeCMD;							}
	void				Set_BECOMMAND(WORD wCommand)			{	m_wBeforeCMD = wCommand;						}
//--------------------------------------------------------------------------------------------------------------------

	float				Get_MoveAniSPEED()						{	return ( m_bRunMODE ) ? m_fRunAniSPEED : 1.0f;	}

	void				Reset_MoveVEC ();
	int					Cal_AtkAniSPEED( short nRightWeaponItemNO );
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
		_ASSERT( this->Get_MaxHP() );
		return ( this->Get_HP()*100 / this->Get_MaxHP() );
	}

	CAI_OBJ *			Get_TARGET ()						{	return this->Get_TargetOBJ();		}	
	void				Set_TargetObjIDX (int iTargetOBJ)	{	this->Set_TargetIDX(iTargetOBJ);	}	





#ifndef	__SERVER
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/// queuing the command 
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	/// 현재 명령을 적용할수 있는가?
	virtual bool		CanApplyCommand() = 0{ *(int*)0 = 10; };

	virtual void		PushCommandSit() = 0{ *(int*)0 = 10; };
	virtual void		PushCommandStand() = 0{ *(int*)0 = 10; };
	virtual void		PushCommandStop() = 0{ *(int*)0 = 10; };
	virtual void		PushCommandMove( const D3DVECTOR& PosTo, BYTE btRunMODE ) = 0{ *(int*)0 = 10; };
	virtual void		PushCommandMove( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget ) = 0{ *(int*)0 = 10; };
	virtual void		PushCommandAttack( int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosTO ) = 0{ *(int*)0 = 10; };
	virtual void		PushCommandDie() = 0{ *(int*)0 = 10; };
	virtual void		PushCommandToggle( BYTE btTYPE ) = 0{ *(int*)0 = 10; };
	virtual void		PushCommandSkill2Self( short nSkillIDX ) = 0{ *(int*)0 = 10; };
	virtual void		PushCommandSkill2Obj( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget, short nSkillIDX ) = 0{ *(int*)0 = 10; };
	virtual void		PushCommandSkill2Pos( const D3DVECTOR& PosGOTO, short nSkillIDX ) = 0{ *(int*)0 = 10; };


#endif




} ;

//-------------------------------------------------------------------------------------------------
#endif
