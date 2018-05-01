#ifndef	__CAI_OBJ_H
#define	__CAI_OBJ_H

///</
///</ 인공지능 객체의 가상 클레스...
///</
class STBDATA;
class CAI_OBJ 
{
private :
	ULONG			 m_ulAICheckTIME[ 2 ];

public  :
	CAI_OBJ ()		{	::ZeroMemory(m_ulAICheckTIME,sizeof(ULONG)*2);		}

	unsigned long	 Get_AICheckTIME(int iIDX)								{	return m_ulAICheckTIME[iIDX];		}
	void			 Set_AICheckTIME(int iIDX, unsigned long lCheckTIME)	{	m_ulAICheckTIME[iIDX] = lCheckTIME;	}

	virtual float	 Get_CurXPOS() = 0{ *(int*)0 = 10; }					///< 현재 X축 위치
	virtual float	 Get_CurYPOS() = 0{ *(int*)0 = 10; }					///< 현재 Y축 위치
#ifndef	__SERVER
	virtual float	 Get_CurZPOS() = 0{ *(int*)0 = 10; }					///< 현재 Z축 위치
#endif
	virtual float	 Get_BornXPOS() = 0{ *(int*)0 = 10; }					///< 처음 생성 X축 위치
	virtual float	 Get_BornYPOS() = 0{ *(int*)0 = 10; }					///< 처음 생성 Y축 위치

	virtual int		 Get_TAG() = 0{ *(int*)0 = 10; }						///< 다른 객체와 중복되지 않는 유일한 값을 구한다.( 일반적으로 슬롯 인덱스 )
//	virtual int		 Get_ObjTYPE() = 0{ *(int*)0 = 10; }					///< 객체의 타입을 얻는다.
//	virtual bool	 Is_SameTYPE(int iType) = 0{ *(int*)0 = 10; }			///< 객체의 타입이 iType가 같은가?
//	virtual bool	 Is_AVATAR() = 0{ *(int*)0 = 10; }						///< 객체의 타입이 아바타 인가?

	virtual int		 Get_PercentHP() = 0{ *(int*)0 = 10; }
	virtual int		 Get_HP() = 0{ *(int*)0 = 10; }						///< 객체의 생명력을 구한다.
	virtual int		 Get_CharNO() = 0{ *(int*)0 = 10; }					///< 객체의 케릭터 번호를 얻는다.

	virtual int		 Get_LEVEL ()	=0;
	virtual int		 Get_ATK ()		=0;
	virtual int		 Get_DEF ()		=0;
	virtual int		 Get_RES ()		=0;
	virtual int		 Get_HIT ()		=0;	
	virtual int		 Get_CHARM ()	=0;
	virtual int		 Get_AVOID ()	=0;
	virtual int		 Get_SENSE ()	=0;
	virtual int		 Get_GiveEXP ()	=0;
	virtual int		 Get_CRITICAL()	=0;	

	virtual CAI_OBJ *Get_TARGET() = 0{ *(int*)0 = 10; }					///< 공격 대상을 구한다.
	virtual CAI_OBJ *Get_OWNER()						{	return NULL;	}

//	virtual void	 Set_TARGET (CAI_OBJ *pAIObj) = 0{ *(int*)0 = 10; }	///< 공격 대상을 설정한다.
	virtual float	 Get_DISTANCE(CAI_OBJ *pAIObj) = 0{ *(int*)0 = 10; }	///< 대상과의 거리를 구한다.
	virtual float	 Get_MoveDISTANCE() = 0{ *(int*)0 = 10; }				///< 처음 시작 위치와 현재위치의 거리를 구한다.

	virtual int		 Get_RANDOM(int iMod) = 0{ *(int*)0 = 10; }			///< 객체에 의존한 렌덤 수치를 구한다.

	virtual bool	 Change_CHAR(int /*iCharIDX*/)			{	return false;	}	///< iCharIDX의 객체로 변환 시킨다.
	virtual bool	 Create_PET (int /*iCharIDX*/, float /*fPosX*/, float /*fPosY*/, int /*iRange*/)	{	return false;	}	///< iCharIDX의 객체를 소환 시킨다.

	virtual void	 Say_MESSAGE (char* /*szMessage*/)		{	;	}		///< 대화를 출력한다.
	virtual void	 Run_AWAY(int /*iDistance*/)			{	;	}
	virtual void	 Drop_ITEM(short /*nDropITEM*/)			{	;	}

	// command interface function
	virtual void Set_EMOTION(short /*nEmotionIDX*/)			{	;	}		///< 감정 표현 동작을 설정한다.

	virtual bool SetCMD_STOP() = 0{ *(int*)0 = 10; }										///< 객체에 정지 명령을 설정한다.
	virtual bool SetCMD_MOVE2D (float fPosToX, float fPosToY, BYTE btRunMODE) = 0{ *(int*)0 = 10; }///< 객체에 이동 명령을 설정한다.
#ifndef	__SERVER
	virtual bool SetCMD_MOVE (const D3DVECTOR& PosTo, BYTE btRunMODE) = 0{ *(int*)0 = 10; }///< 객체에 이동 명령을 설정한다.
#endif
	virtual void SetCMD_RUNnATTACK(int iTargetObjTAG) = 0{ *(int*)0 = 10; }				///< 객체에 공격 명령을 설정한다.
	virtual void Special_ATTACK() = 0{ *(int*)0 = 10; }									///< 객체에 특수 공격 명령을 설정한다.

	virtual CAI_OBJ* AI_FindFirstOBJ( int iDistance ) = 0{ *(int*)0 = 10; }
	virtual CAI_OBJ* AI_FindNextOBJ () = 0{ *(int*)0 = 10; }

	virtual int		Get_EconomyVAR(short /*nVarIDX*/)		{	return 0;	}
	virtual int		Get_WorldVAR  (short /*nVarIDX*/)		{	return 0;	}

	virtual int		Get_TeamNO () = 0{ *(int*)0 = 10; }

			///PVP 존에서는 이걸로만 판단해서는 안된다. 2005/07/14 - nAvy
			bool	Is_ALLIED( CAI_OBJ *pDestOBJ )
			{
			#ifdef	__SERVER
				if ( this->Get_OWNER() ) {
					if ( this->Get_OWNER() == pDestOBJ || this->Get_OWNER() == pDestOBJ->Get_OWNER() )
						return true;
				}
			#endif
				if ( -1 == pDestOBJ->Get_TeamNO() )		// 팀번호 -1 이면 무조건 적
					return false;

				if ( this->Get_TeamNO() == pDestOBJ->Get_TeamNO() ||
					 (__int64)(this->Get_TeamNO()) * (__int64)(pDestOBJ->Get_TeamNO()) <= 100 )
					return true;

				return false;
			}
			bool	Is_SameTEAM( CAI_OBJ *pDestOBJ )
			{
			#ifdef	__SERVER
				if ( this->Get_OWNER() ) {
					if ( this->Get_OWNER() == pDestOBJ || this->Get_OWNER() == pDestOBJ->Get_OWNER() )
						return true;
				}
			#endif
				if ( -1 == pDestOBJ->Get_TeamNO() )		// 팀번호 -1 이면 무조건 적
					return false;

				return ( this->Get_TeamNO() == pDestOBJ->Get_TeamNO() );
			}

	virtual BYTE	Is_DAY (void) = 0{ *(int*)0 = 10; }						// 밤낮인지 리턴... 낮:1, 밤0
	virtual int		Get_ZoneTIME (void) = 0{ *(int*)0 = 10; }				// 현재 존 시간을 리턴한다...
	virtual int		Get_WorldTIME(void) = 0{ *(int*)0 = 10; }				// 현재 월드 시간을 리턴한다...
	virtual void	Set_TRIGGER (t_HASHKEY /*HashKEY*/)		{	;	}
#ifdef	__SERVER
	virtual DWORD	Get_MagicSTATUS() = 0{ *(int*)0 = 10; }
	virtual CAI_OBJ*Find_LocalNPC(int iNpcNO) = 0{ *(int*)0 = 10; }

	virtual void	Set_EconomyVAR(short nVarIDX, int iValue) = 0{ *(int*)0 = 10; }
	virtual void	Set_WorldVAR  (short nVarIDX, int iValue) = 0{ *(int*)0 = 10; }

	virtual void	Add_DAMAGE ( WORD wDamage ) = 0{ *(int*)0 = 10; }

	virtual bool	Send_gsv_CHAT ( char *szMsg ) = 0{ *(int*)0 = 10; }
	virtual bool	Send_gsv_SHOUT( char *szMsg ) = 0{ *(int*)0 = 10; }
	virtual void	Send_gsv_ANNOUNCE_CHAT( char *szMsg ) = 0{ *(int*)0 = 10; }

	virtual bool	SetCMD_Skill2SELF (short /*nSkillIDX*/, short /*nMotion*/)							{	return true;	}
	virtual bool	SetCMD_Skill2OBJ (int /*iTargetObjIDX*/, short /*nSkillIDX*/, short /*nMotion*/)	{	return true;	}
#endif
} ;


///<-------------------------------------------------------------------------------------------------
///
///	인공지능 파일 클레스..
///

class CAI_PATTERN;
class CAI_FILE
{
private:
	CAI_PATTERN	 *m_pPattern;
	int			  m_iPatternCNT;
	unsigned long m_ulCheckStopAI;
	int			  m_iRateDamagedAI;

	void AI_Check (WORD wPattern, CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR=NULL, int iDamage=0);
public :
	CAI_FILE ();
	~CAI_FILE ();

	bool Load (char *szFileName, STBDATA *pSTB, int iLangCol);

	/// 0		처음 생성시
	void AI_WhenCREATED (CAI_OBJ *pSourCHAR);

	/// 1		정지 상태일때
	void AI_WhenSTOP (CAI_OBJ *pSourCHAR);

	/// 2		공격 이동시
	void AI_WhenAttackMOVE (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR);

	/// 3		타격 당했을때
	void AI_WhenDAMAGED (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage);

	/// 4		상대방을 죽였을때
	void AI_WhenKILL (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage);

	/// 5		자신이 죽을때
	void AI_WhenDEAD (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage);
} ;


///<-------------------------------------------------------------------------------------------------
///
/// 인공지능 객체에서 참조하는 환경 함수...
///

extern int		AI_SysRANDOM( int iMod );

///<-------------------------------------------------------------------------------------------------
#endif