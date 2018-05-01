
#ifndef	__CUSERDATA_H
#define	__CUSERDATA_H
#include "CQuest.h"
#include "CInventory.h"
#include "CHotICON.h"
#include "Calculation.h"
#include "IO_STB.H"

#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------

#ifdef	__BORLANDC__
#include <crtdbg.h>
#endif

#pragma pack (push, 1)
//-------------------------------------------------------------------------------------------------
/// 기본 정보 데이터...
struct tagBasicINFO {
	// NAME
	// SEX
	char	m_cBirthStone;		// 탄생석
	char	m_cFaceIDX;			// 얼굴 모양
	char	m_cHairIDX;			// 머리 모양
	short	m_nClass;			// 직업 ( 1~30 )
	char	m_cUnion;			// 소속 조합 ( 1~20 )
	char	m_cRank;			// 계급 ( 1~15 )
	char	m_cFame;			// 명성 ( 0 ~ 99 )
	void Init (char cBirthStone, char cFaceIDX, char cHairIDX)
	{
		m_cBirthStone = cBirthStone;		
		m_cFaceIDX    = cFaceIDX;			
		m_cHairIDX    = cHairIDX;			

		m_nClass = 0;
		m_cUnion = 0;
		m_cRank  = 0;
		m_cFame  = 0;
	}
} ;

/// 기본 능력치 데이터...
struct tagBasicAbility {
	union {
		struct {
			short	m_nSTR;			// 근력 1~200
			short	m_nDEX;			// 민첩 1~200
			short	m_nINT;			// 지력 1~200
			short	m_nCON;			// concentration 집중력 1~200
			short	m_nCHARM;		// 매력 1~200
			short	m_nSENSE;		// 감각 1~200
		} ;
		short		m_nBasicA[ BA_MAX ];
	} ;
	void Init () 
	{
		m_nSTR = 10;
		m_nDEX = 10;
		m_nINT = 10;
		m_nCON = 10;
		m_nCHARM = 10;
		m_nSENSE = 10;
	}
} ;

#ifdef FRAROSE
#define	MAX_BASIC_ABLITY		410		// 250 기본 능력치 최대치를 300으로 수정
#define	MAX_LEVEL				255		// 200 최대 레벨을 250으로 수정
#else
#define	MAX_BASIC_ABLITY		300		// 250 기본 능력치 최대치를 300으로 수정
#define	MAX_LEVEL				210		// 200 최대 레벨을 250으로 수정
#endif
#define	MAX_EXPERIENCE			0x7FFFFFFFFFFFFFFF




 #define	MAX_MAINTAIN_STATUS		40		// 8 * 50 = 400
 struct tagMaintainSTATUS {
 	DWORD	m_dwExpiredSEC;
 	short	m_nValue;
 	short	m_nDummy;
 } ;

/// 성장 능력치 데이터 ...
struct tagGrowAbility {
	short	m_nHP;				// 0~2000
	short	m_nMP;				// 0~1000

	__int64	m_lEXP;				// 경험치 1~100000
	short	m_nLevel;			// 0~250
	short	m_nBonusPoint;		// 1~999
	short	m_nSkillPoint;		// 1~
	BYTE	m_btBodySIZE;		// 몸통크기
	BYTE	m_btHeadSIZE;		// 머리크기
	__int64	m_lPenalEXP;		// 추가 경험치...

	short	m_nFameG;			// 선행지수	: 퀘스트에 의해 증감됨	: 2004.5.27 추가
	short	m_nFameB;			// 선행지수 : 퀘스트에 의해 증감됨	: 2004.5.27 추가
	short	m_nUnionPOINT[ MAX_UNION_COUNT ];	// 조합 포인트		: 2004.5.27 추가

	int		m_iGuildNO;			// 길드 번호	: 2004.5.27 추가
	short	m_nGuildCNTRB;		// 길드 기여도	: 2004.5.27 추가
	BYTE	m_btGuildPOS;		// 길드 직위	: 2004.5.27 추가

	short	m_nPKFlag;			// 2004. 6. 17 추가..
	short	m_nSTAMINA;			// 2004. 8. 23 추가..

	tagMaintainSTATUS	m_STATUS[ MAX_MAINTAIN_STATUS ];

#ifdef _GBC
	short	m_nPatHP;
	DWORD	m_dwPatCoolTIME;
#endif
/*
	char	m_cChaos;			// 성향
	short	m_nBattle_LV;		// 베틀레벨
	short	m_nPK_LV;			//
*/
	void Init () 
	{
		m_nHP = 50;
		m_nMP = 40;

		m_lEXP = 0;
		m_nLevel = 1;
		m_nBonusPoint = 0;
		m_nSkillPoint = 0;
		m_btBodySIZE = 100;		// 몸통크기
		m_btHeadSIZE = 100;		// 머리크기
		m_lPenalEXP = 0;

		m_nFameG = m_nFameB = 0;
		::ZeroMemory( m_nUnionPOINT, sizeof(m_nUnionPOINT) );
		m_iGuildNO = m_nGuildCNTRB = m_btGuildPOS = 0;

		m_nPKFlag = 0;
	}
} ;


/// 아바타 보유스킬 - 여분으로 120으로 설정 기획서상 사용은 90개
#define	MAX_LEARNED_SKILL_CNT			120
#define	MAX_LEARNED_SKILL_PAGE			4
#define	MAX_LEARNED_SKILL_PER_PAGE		( MAX_LEARNED_SKILL_CNT/MAX_LEARNED_SKILL_PAGE )

struct tagSkillAbility {
	union {
		short	m_nSkillINDEX[ MAX_LEARNED_SKILL_CNT ];
		short	m_nIndex[ MAX_LEARNED_SKILL_PAGE ][ MAX_LEARNED_SKILL_PER_PAGE ];	// 전투, 상업/제조, 패시브 스킬 각각 10개까지 보유.
	} ;
	void Init ()
	{
		::ZeroMemory( m_nSkillINDEX,	sizeof(short) * MAX_LEARNED_SKILL_CNT );
		m_nSkillINDEX[ 0 ] = 11;	// 앉기
		m_nSkillINDEX[ 1 ] = 12;	// 줍기
		m_nSkillINDEX[ 2 ] = 16;	// 일반공격
		m_nSkillINDEX[ 3 ] = 19;	// 파티신청
		m_nSkillINDEX[ 4 ] = 20;	// 거래신청
		m_nSkillINDEX[ 5 ] = 21;	// 상점개설
	}
} ;

/// 퀘스트 데이터 ...

// #define	__APPLY_EXTAND_QUEST_VAR		// 05.05.21 추가 확장 스위치및 변수 적용여부..

#define	QUEST_PER_PLAYER			10		// 개인당 최대 등록 가능 퀘스트수...

#define	QUEST_EPISODE_VAR_CNT		5		// 에피소드 진행 변수 갯수
#define	QUEST_JOB_VAR_CNT			3		// 직업별 진행 변수 갯수
#define	QUEST_PLANET_VAR_CNT		7		// 행성별...
#define	QUEST_UNION_VAR_CNT			10		// 조합별...

#ifdef	__APPLY_EXTAND_QUEST_VAR
	#define	QUEST_ETC_VAR_CNT			53
	#define	QUEST_SWITCH_CNT			(512+512)	// 전체 퀘스트 진행 스위치수..
#else
	#define	QUEST_SWITCH_CNT			(512    )	// 전체 퀘스트 진행 스위치수..
#endif
#define	QUEST_SWITCH_GROUP_CNT	  ( QUEST_SWITCH_CNT/sizeof(DWORD) )

struct tagQuestData {
private:
	enum {	BIT_SHIFT=3, WHICH_BIT=0x07 };
	BYTE GetBit (int iIndex)
	{
		return ( m_btSWITCHES[ iIndex >> BIT_SHIFT ] & g_btSwitchBitMask[ iIndex & WHICH_BIT ] ) ? 1 : 0;
	}
	void SetBit (int iIndex)
	{
		m_btSWITCHES[ iIndex >> BIT_SHIFT ] |= g_btSwitchBitMask[ iIndex & WHICH_BIT ];
	}
	void ClearBit (int iIndex)
	{
		m_btSWITCHES[ iIndex >> BIT_SHIFT ] &= ~(g_btSwitchBitMask[ iIndex & WHICH_BIT ]);
	}
	BYTE FlipBit (int iIndex)
	{
		if ( this->GetBit( iIndex ) )
			this->ClearBit( iIndex );
		else
			this->SetBit ( iIndex );

		return this->GetBit( iIndex );
	}

public :
	short		m_nEpisodeVAR[ QUEST_EPISODE_VAR_CNT	];
	short		m_nJobVAR	 [ QUEST_JOB_VAR_CNT		];
	short		m_nPlanetVAR [ QUEST_PLANET_VAR_CNT		];
	short		m_nUnionVAR  [ QUEST_UNION_VAR_CNT		];
	CQUEST		m_QUEST		 [ QUEST_PER_PLAYER			];
	union {
		DWORD	m_dwSWITCHES [ QUEST_SWITCH_CNT/32		];
		BYTE	m_btSWITCHES [ QUEST_SWITCH_CNT/8		];
	} ;

#ifdef	__APPLY_EXTAND_QUEST_VAR
	short		m_nEtcVAR	 [ QUEST_ETC_VAR_CNT		];		// 2 * 50
#endif

	void Init ()
	{
		::ZeroMemory( m_nEpisodeVAR,	sizeof(short) * QUEST_EPISODE_VAR_CNT	);
		::ZeroMemory( m_nJobVAR,		sizeof(short) * QUEST_JOB_VAR_CNT		);
		::ZeroMemory( m_nPlanetVAR,		sizeof(short) * QUEST_PLANET_VAR_CNT	);
		::ZeroMemory( m_nUnionVAR,		sizeof(short) * QUEST_UNION_VAR_CNT		);
#ifdef	__APPLY_EXTAND_QUEST_VAR
		::ZeroMemory( m_nEtcVAR,		sizeof(short) * QUEST_ETC_VAR_CNT		);
#endif

		this->ClearAllSwitchs ();

		for (short nI=0; nI<QUEST_PER_PLAYER; nI++) {
			m_QUEST[ nI ].Init ();
		}
	}

	//-------------------------------------------------------------------------------------------------
	void ClearAllSwitchs ()
	{
		::ZeroMemory (m_dwSWITCHES, sizeof(m_dwSWITCHES));
	}
	void Set_SWITCH (int iSwitchNO, int iValue)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return;

		if ( iValue )
			this->SetBit( iSwitchNO );
		else
			this->ClearBit( iSwitchNO );
	}
	int  Get_SWITCH (int iSwitchNO)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return -1;

		return this->GetBit( iSwitchNO );
	}
	int  Flip_SWITCH(int iSwitchNO)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return -1;

		return this->FlipBit( iSwitchNO );
	}
} ;


#define	BANKSLOT_TOTAL_SIZE		160
struct tagBankData {
	tagITEM		m_ItemLIST[	BANKSLOT_TOTAL_SIZE ];
	__int64		m_i64ZULY;
	void Init ()
	{
		::ZeroMemory( m_ItemLIST,	sizeof(tagITEM)*BANKSLOT_TOTAL_SIZE );
	}

	short Get_EmptySlot (short nStartSlotNO);
	short Add_ITEM(tagITEM &sITEM);
	short Add_ITEM(short nSlotNO, tagITEM &sITEM);
	short Sub_ITEM(short nSlotNO, tagITEM &sITEM);
	void  Set_ITEM(short nSlotNO, tagITEM &sITEM)
	{
		m_ItemLIST[ nSlotNO ] = sITEM;
	}
} ;


#define	MAX_WISH_ITEMS		30				// 구입 희망하는 찜 아이템 최대 갯수
struct tagWishLIST {
	tagITEM	m_WishITEM[ MAX_WISH_ITEMS ];

	void Init ()
	{
		::ZeroMemory( m_WishITEM,	sizeof(tagITEM)*MAX_WISH_ITEMS );
	}
	bool Set(BYTE btSlotNO, tagITEM &sITEM)
	{
		if ( btSlotNO >= MAX_WISH_ITEMS )
			return false;

		this->m_WishITEM[ btSlotNO ] = sITEM;
		return true;
	}
} ;

//-------------------------------------------------------------------------------------------------
#pragma pack (pop)


/// 전투 능력치 데이터 - 계산되어져 얻음
struct tagBattleAbility {
	short	m_nMaxHP;
	short	m_nMaxMP;
	short	m_nATT;			// 공격력 1~200
	short	m_nDEF;			// 방어력 1~200
	short	m_nMAG;			// 마법력 1~200
	short	m_nHIT;			// 명중력 1~500
	short	m_nRES;			// 항마력 1~200
	short	m_nAVOID;		// 회피력 1~999
	short	m_nSPD;			// 이동력 1~50
	short	m_nMaxWEIGHT;	// weight 아이템소지량.
	short	m_nWEIGHT;		// 현재 소지한량

//	int		m_iDefQuality;	// 전체 방어구 품질
	int		m_iDefDurabity;	// 전체 방어구 내구도합
	int		m_iDefGrade;	// 전체 방어구 등급합

	int		m_iCritical;	// 크리티컬 수치
	float	m_fRateUseMP;	// MP 절감 비율로 계산되어진 MP 소모될 비율.


#ifdef _GBC
	short	m_nImmunity;	// 면역력
#endif
	

} ;


//-------------------------------------------------------------------------------------------------

/// Base user data
class CUserDATA {
protected:
	inline int	GetPassiveSkillValue(short nAbilityTYPE)
	{
		return this->m_iAddValue[ nAbilityTYPE ];
	}
	inline void AddPassiveSkillValue(short nAbilityTYPE, short nValue)
	{
		this->m_iAddValue[ nAbilityTYPE ] += nValue;
	}
	inline short GetPassiveSkillRate(short nAbilityTYPE)
	{
		return this->m_nPassiveRate[ nAbilityTYPE ];
	}
	inline void AddPassiveSkillRate(short nAbilityTYPE, short nRate)
	{
		this->m_nPassiveRate[ nAbilityTYPE ] += nRate;
	}

	void	Cal_AddAbility (tagITEM &sITEM, short nItemTYPE);
	void	Cal_AddAbility ();
	
	void	Cal_RecoverHP ();
	void	Cal_RecoverMP ();

	int		Cal_MaxHP ();
	int		Cal_MaxMP ();

	int		Cal_HIT ();
	int		Cal_DEFENCE();

	int		Cal_RESIST ();
	int		Cal_MaxWEIGHT ();
	int		Cal_AvoidRATE ();
	int		Cal_CRITICAL ();

	short	GetPassiveSkillAttackPower( int iCurAP, short nRightWeaponItemNo );
protected :
	short	GetPassiveSkillAttackSpeed( float fCurSpeed, short nRightWeaponItemNo );
	short	GetPassiveSkillAttackSpeed( short nRightWeaponItemNo );
protected :
	float	Cal_RunSPEED ();
	void	Cal_BattleAbility ();
	int		Cal_ATTACK ();

public :
	short GetCur_HP ()				{	return	this->m_GrowAbility.m_nHP;			}	// 생명력
	short GetCur_MP ()				{	return	this->m_GrowAbility.m_nMP;			}
	virtual short GetCur_WEIGHT ()			{	return	this->m_Battle.m_nWEIGHT;			}

	int	  GetCur_LEVEL ()			{	return	this->m_GrowAbility.m_nLevel;		}

	int	  GetDef_ATK ()				{	return	this->m_Battle.m_nATT;				}	// 공격력
	int	  GetDef_DEF ()				{	return	this->m_Battle.m_nDEF;				}	// 방어력
	int	  GetDef_RES ()				{	return	this->m_Battle.m_nRES;				}	// 항마력
	int	  GetDef_HIT ()				{	return	this->m_Battle.m_nHIT;				}
	int	  GetDef_AVOID ()			{	return	this->m_Battle.m_nAVOID;			}	// 회피력
	int	  GetDef_CRITICAL()			{	return	this->m_Battle.m_iCritical;			}


	int	  GetDef_IMMUNITY();
	void  SetDef_IMMUNITY(int iImmunity);


	int	  GetCur_BIRTH()			{	return  this->m_BasicINFO.m_cBirthStone;	}	// 탄생석
	int	  GetCur_RANK()				{	return	this->m_BasicINFO.m_cRank;			}	// 계급
	int	  GetCur_UNION ()			{	return	this->m_BasicINFO.m_cUnion;			}	// 소속
	int	  GetCur_FAME ()			{	return	this->m_BasicINFO.m_cFame;			}	// 명성
	int	  GetCur_JOB ()				{	return	this->m_BasicINFO.m_nClass;			}	// 직업

	int	  GetCur_EXP ()				{	return	this->m_GrowAbility.m_lEXP;			}
	int	  GetCur_BonusPOINT ()		{	return	this->m_GrowAbility.m_nBonusPoint;	}
	int	  GetCur_SkillPOINT ()		{	return	this->m_GrowAbility.m_nSkillPoint;	}
	BYTE  GetCur_HeadSIZE()			{	return	this->m_GrowAbility.m_btHeadSIZE;	}
	BYTE  GetCur_BodySIZE()			{	return	this->m_GrowAbility.m_btBodySIZE;	}
	
	short	GetCur_PatHP();
	void	SetCur_PatHP( short nPatHP );
	DWORD	GetCur_PatCoolTIME();
	void	SetCur_PatCoolTIME( DWORD dwCoolTIME );


	/// 기본 능력치...
	int	  GetDef_STR ()				{	return	this->m_BasicAbility.m_nSTR;		}	// 근력
	int	  GetDef_DEX ()				{	return	this->m_BasicAbility.m_nDEX;		}	// 민첩
	int	  GetDef_INT ()				{	return	this->m_BasicAbility.m_nINT;		}	// 지력
	int	  GetDef_CON ()				{	return	this->m_BasicAbility.m_nCON;		}	// 집중
	int	  GetDef_CHARM ()			{	return	this->m_BasicAbility.m_nCHARM;		}	// 매력
	int	  GetDef_SENSE ()			{	return	this->m_BasicAbility.m_nSENSE;		}	// 감각

	void  SetDef_STR(short nValue)	{	this->m_BasicAbility.m_nSTR=nValue;			}
	void  SetDef_DEX(short nValue)	{	this->m_BasicAbility.m_nDEX=nValue;			}
	void  SetDef_INT(short nValue)	{	this->m_BasicAbility.m_nINT=nValue;			}
	void  SetDef_CON(short nValue)	{	this->m_BasicAbility.m_nCON=nValue;			}
	void  SetDef_CHARM(short nValue){	this->m_BasicAbility.m_nCHARM=nValue;		}
	void  SetDef_SENSE(short nValue){	this->m_BasicAbility.m_nSENSE=nValue;		}

	int	  GetCur_STR ()				{	return	( GetDef_STR()   + m_iAddValue[ AT_STR		] + m_PassiveAbilityFromRate[ AT_STR  -AT_STR ] );	}	// 근력
	int	  GetCur_DEX ()				{	return	( GetDef_DEX()   + m_iAddValue[ AT_DEX		] + m_PassiveAbilityFromRate[ AT_DEX  -AT_STR ] );	}	// 민첩
	int	  GetCur_INT ()				{	return	( GetDef_INT()   + m_iAddValue[ AT_INT		] + m_PassiveAbilityFromRate[ AT_INT  -AT_STR ] );	}	// 지력
	int	  GetCur_CON ()				{	return	( GetDef_CON()   + m_iAddValue[ AT_CON		] + m_PassiveAbilityFromRate[ AT_CON  -AT_STR ] );	}	// 집중
	int	  GetCur_CHARM ()			{	return	( GetDef_CHARM() + m_iAddValue[ AT_CHARM	] + m_PassiveAbilityFromRate[ AT_CHARM-AT_STR ] );	}	// 매력
	int	  GetCur_SENSE ()			{	return	( GetDef_SENSE() + m_iAddValue[ AT_SENSE	] + m_PassiveAbilityFromRate[ AT_SENSE-AT_STR ] );	}	// 감각

	int	  GetCur_SaveMP ()	// MP 절감 비율
	{
		return  ( GetPassiveSkillValue( AT_PSV_SAVE_MP ) + m_iAddValue[ AT_SAVE_MP ] + (short)(m_iAddValue[ AT_SAVE_MP ]*GetPassiveSkillRate( AT_PSV_SAVE_MP )/100.f) );	
	}
	//int	  GetCur_DropRATE()			{	return  ( GetPassiveSkillValue( AT_PSV_DROP_RATE ) + m_iAddValue[ AT_DROP_RATE ] );	}	// 드롭 확률
	int	  GetCur_DropRATE()			{	return  m_iDropRATE;	}	// 드롭 확률
	void  Cal_DropRATE ()	// 드롭 확률		
	{
		m_iDropRATE = GetPassiveSkillValue( AT_PSV_DROP_RATE ) + m_iAddValue[ AT_DROP_RATE ] + (short)(m_iAddValue[ AT_DROP_RATE ]*GetPassiveSkillRate( AT_PSV_DROP_RATE )/100.f);	
	}
	

	int	  GetCur_UnionPOINT( WORD wAbilityTYPE )				{	return this->m_GrowAbility.m_nUnionPOINT[ wAbilityTYPE-AT_UNION_POINT1 ];	}
	void  SetCur_UnionPOINT( WORD wAbilityTYPE, short nValue )	{	this->m_GrowAbility.m_nUnionPOINT[ wAbilityTYPE-AT_UNION_POINT1 ] = nValue;	}
	void  AddCur_UnionPOINT( WORD wAbilityTYPE, short nValue )	{	this->m_GrowAbility.m_nUnionPOINT[ wAbilityTYPE-AT_UNION_POINT1 ]+= nValue;	}

	int	  GetCur_FameB ()			{	return	this->m_GrowAbility.m_nFameB;		}
	int	  GetCur_FameG ()			{	return	this->m_GrowAbility.m_nFameG;		}

	int	  GetCur_GuildNO()			{	return	this->m_GrowAbility.m_iGuildNO;		}
	int	  GetCur_GuildPOS()			{	return	this->m_GrowAbility.m_nGuildCNTRB;	}
	int	  GetCur_GuildCONTRIBUTION(){	return	this->m_GrowAbility.m_btGuildPOS;	}


	short GetDef_MaxHP ()			{	return	this->m_Battle.m_nMaxHP;			}
	short GetDef_MaxMP ()			{	return	this->m_Battle.m_nMaxMP;			}

	short GetCur_MaxWEIGHT ()		{	return	this->m_Battle.m_nMaxWEIGHT;		}
	int	  GetCur_MAG ()				{	return	this->m_Battle.m_nMAG;				}	// 마법력

//	int	  GetTot_DEF_QUALITY()		{	return	this->m_Battle.m_iDefQuality;		}
	int	  GetTot_DEF_DURABITY()		{	return	this->m_Battle.m_iDefDurabity;		}
	int	  GetTot_DEF_GRADE()		{	return	this->m_Battle.m_iDefGrade;			}

	float GetCur_RateUseMP()		{	return	this->m_Battle.m_fRateUseMP;		}

	void	SetCur_MONEY (__int64 iMoney)	{	this->m_Inventory.m_i64Money=iMoney;	}
	__int64 GetCur_MONEY ()					{	return	this->m_Inventory.m_i64Money;	}	// 돈
	void	Add_CurMONEY (int iMoney)		{	this->m_Inventory.m_i64Money+=iMoney;	}
	void	Sub_CurMONEY (int iMoney)		{	this->m_Inventory.m_i64Money-=iMoney;	}
	void  SetCur_HP (short nValue);// 생명력
	void  SetCur_MP (short nValue)	{	this->m_GrowAbility.m_nMP=nValue;				}

	int   AddCur_HP (short nValue)	
	{	
		m_GrowAbility.m_nHP+=nValue; 
		if ( m_GrowAbility.m_nHP > GetCur_MaxHP() )
			m_GrowAbility.m_nHP = GetCur_MaxHP();
		return m_GrowAbility.m_nHP;
	}
	int	  AddCur_MP (short nValue)	
	{	
		m_GrowAbility.m_nMP+=nValue;
		if ( m_GrowAbility.m_nMP > GetCur_MaxMP() )
			m_GrowAbility.m_nMP = GetCur_MaxMP();
		return m_GrowAbility.m_nMP;
	}
	int   SubCur_HP (short nValue)
	{	
		m_GrowAbility.m_nHP-=nValue;
		if ( m_GrowAbility.m_nHP < 0 )
			m_GrowAbility.m_nHP = 0;
		return m_GrowAbility.m_nHP;
	}
	int	  SubCur_MP (short nValue)
	{	
		m_GrowAbility.m_nMP-=nValue;
		if ( m_GrowAbility.m_nMP < 0 )
			m_GrowAbility.m_nMP = 0;
		return m_GrowAbility.m_nMP;
	}

#ifndef _SERVER
	int   AddCur_STAMINA (short nValue)	
	{	
		m_GrowAbility.m_nSTAMINA += nValue;		
		if ( m_GrowAbility.m_nSTAMINA > GetCur_MaxSTAMINA() )
			m_GrowAbility.m_nSTAMINA = GetCur_MaxSTAMINA();
		return m_GrowAbility.m_nSTAMINA;
	}
	int   SubCur_STAMINA (short nValue)
	{	
		m_GrowAbility.m_nSTAMINA-=nValue;
		if ( m_GrowAbility.m_nSTAMINA < 0 )
			m_GrowAbility.m_nSTAMINA = 0;
		return m_GrowAbility.m_nSTAMINA;
	}
#endif


	void  SetCur_FAME (char cValue)			{	this->m_BasicINFO.m_cFame=cValue;			}
	void  SetCur_UNION (char cValue)		{	this->m_BasicINFO.m_cUnion=cValue;			}
	void  SetCur_RANK (char cValue)			{	this->m_BasicINFO.m_cRank=cValue;			}

	virtual void  SetCur_JOB (short nValue)	{	this->m_BasicINFO.m_nClass=nValue;			}

	void  SetCur_LEVEL(WORD wValue)			{	this->m_GrowAbility.m_nLevel = ( wValue>MAX_LEVEL) ? MAX_LEVEL : wValue;	}
	void  SetCur_EXP (__int64 lValue)			{	this->m_GrowAbility.m_lEXP=lValue;			}
	void  SetCur_BonusPOINT(short nValue)	{	this->m_GrowAbility.m_nBonusPoint=nValue;	}
	void  SetCur_SkillPOINT(short nValue)	{	this->m_GrowAbility.m_nSkillPoint=nValue;	}

	tagBasicINFO		m_BasicINFO;					// 기본 정보 데이터...	
	tagBasicAbility		m_BasicAbility;					// 기본 능력치 데이터...
	tagGrowAbility		m_GrowAbility;					// 성장 능력치 데이터 ...
	tagSkillAbility		m_Skills;						// 아바타 보유스킬

	tagQuestData		m_Quests;						// quest
	CInventory			m_Inventory;					// inventory
	CHotICONS			m_HotICONS;						// 단축아이콘 등록 정보...
	tagBankData			m_Bank;							// 은행 데이타.
	tagWishLIST			m_WishLIST;						// 개인 상점에서 물건 구입시 사용할 구입희망 아이템목록.

	tagBattleAbility	m_Battle;						// 전투 능력치 데이터 - 계산되어져 얻음

	int					m_iAddValue   [ AT_MAX ];		// 장착 아이템에 의해 증가된 수치 : 계산되어짐.
	short				m_nPassiveRate[ AT_MAX ];		// 패시브 스킬에 의해 보정된 비율

	BYTE				m_btRecoverHP;					// == m_PassiveSkill[ PST_RECOVER_HP ] + this->m_iAddValue[ AT_RECOVER_HP ]
	BYTE				m_btRecoverMP;					// == m_PassiveSkill[ PST_RECOVER_MP ] + this->m_iAddValue[ AT_RECOVER_MP ]

	int					m_iDropRATE;					// 드롭 확률

	short				m_PassiveAbilityFromValue[ BA_MAX ];	// 기본 능력치 :: 수치 패시브 스킬에 의해 보정된 값
	short				m_PassiveAbilityFromRate [ BA_MAX ];	// 기본 능력치 :: 비율 패시브 스킬에 의해 보정된 값
	short				m_nPassiveAttackSpeed;					// 한국/일본/IRose를 위한 패시브 스킬에 의한 공속 추가


	int					m_iAppliedPenaltyEXP;			/// 부활시 경험치 복구를 위한 변수..


	tagITEM	Get_EquipITEM (WORD wEquipIDX)							{	return m_Inventory.m_ItemEQUIP[ wEquipIDX ];			}
	tagITEM*Get_EquipItemPTR(WORD wEquipIDX)						{	return &m_Inventory.m_ItemEQUIP[ wEquipIDX ];			}

	tagITEM Get_InventoryITEM (WORD wTotalListIndex)				{	return m_Inventory.m_ItemLIST[ wTotalListIndex ];		}
	tagITEM Get_InventoryITEM (t_InvTYPE InvTYPE, WORD wPageIndex)	{	return m_Inventory.m_ItemPAGE[ InvTYPE ][ wPageIndex ];	}

#ifdef __SERVER
	short	Add_CatchITEM (short nListRealNO, tagITEM &sITEM);//		{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT );	}
	short	Add_ITEM (tagITEM &sITEM)								{	return m_Inventory.AppendITEM( sITEM, m_Battle.m_nWEIGHT );					}
	short	Add_ITEM(short nListRealNO, tagITEM &sITEM);//				{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);		}
	void	Sub_ITEM(short nListRealNO, tagITEM &sITEM);//				{	m_Inventory.SubtractITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);			}
	void    Set_ITEM(short nListRealNO, tagITEM& sITEM);
	void	ClearITEM(WORD wListRealNO)									
	{	
///		m_Battle.m_nWEIGHT -= m_Inventory.GetWEIGHT (wListRealNO);
		m_Inventory.DeleteITEM(wListRealNO); 
	}
#else
	virtual short	Add_CatchITEM (short nListRealNO, tagITEM &sITEM);//		{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT );	}
	virtual short	Add_ITEM (tagITEM &sITEM)								{	return m_Inventory.AppendITEM( sITEM, m_Battle.m_nWEIGHT );					}
	virtual short	Add_ITEM(short nListRealNO, tagITEM &sITEM);//				{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);		}
	virtual void	Sub_ITEM(short nListRealNO, tagITEM &sITEM);//				{	m_Inventory.SubtractITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);			}
	virtual void    Set_ITEM(short nListRealNO, tagITEM& sITEM);
	virtual void	ClearITEM(WORD wListRealNO)									
	{	
///		m_Battle.m_nWEIGHT -= m_Inventory.GetWEIGHT (wListRealNO);
		m_Inventory.DeleteITEM(wListRealNO); 
	}

#endif
	/// 지속 스킬에 의해 보정된 값을 리턴할 함수...
	virtual short GetCur_RACE() = 0;            /// 종족,,,, gsv_SELECT_CHAR.m_btCharRACE 값 리턴하면됨
	virtual short GetCur_ATK_SPD () = 0;
	virtual float GetCur_MOVE_SPEED () = 0;
	virtual int	  GetCur_ATK() = 0;
	virtual int	  GetCur_DEF() = 0;
	virtual int	  GetCur_RES() = 0;
	virtual int	  GetCur_HIT() = 0;
	virtual int	  GetCur_AVOID() = 0;
	virtual int	  GetCur_CRITICAL() = 0;

	virtual int	  GetCur_MaxHP () = 0;			/// 스킬에 의해 보정된 값이 있을경우 더해져서 리턴할 함수
	virtual int	  GetCur_MaxMP () = 0;	        /// 스킬에 의해 보정된 값이 있을경우 더해져서 리턴할 함수
#ifndef _SERVER
	virtual int	  GetCur_MaxSTAMINA () = 0;
#endif
	virtual void  UpdateCur_Ability() = 0;
	virtual BYTE  GetCur_MOVE_MODE ()			{	return 0;	}

	virtual void  SetCur_TeamNO(short /*nValue*/) = 0;
	void	SetCur_PK_FLAG (short nValue )		{	this->m_GrowAbility.m_nPKFlag;				}
	short	GetCur_PK_FLAG ()					{	return this->m_GrowAbility.m_nPKFlag;		}

	virtual void  AddCur_EXP ( int /*iValue*/ )	{	/* nop */	}	// 호출될경우 서버에서 전송됨.
	void	AddCur_BonusPOINT (short nValue)	{	this->m_GrowAbility.m_nBonusPoint+=nValue;	}
	void	AddCur_SkillPOINT (short nValue)	{	this->m_GrowAbility.m_nSkillPoint+=nValue;	}

	void	SubCur_BounsPOINT (short nValue)	{	this->m_GrowAbility.m_nBonusPoint-=nValue;	}
	void	SubCur_SkillPOINT (short nValue)	{	this->m_GrowAbility.m_nSkillPoint-=nValue;	}
		
	bool	Set_AbilityValue (WORD nType, int iValue);
	void	Add_AbilityValue (WORD nType, int iValue);

	virtual void	ChangeModelInfo( short nType, int iValue ) = 0;


	
#ifdef	__SERVER
	virtual int GetCur_AbilityValue (WORD nType) = 0;
#else
	int		GetCur_AbilityValue (WORD nType)						{	return Get_AbilityValue(nType);		}
	int		Get_AbilityValue (WORD nType);
	void	RefreshLookUpTable()									{   m_Inventory.MakeItemIndexList();	}
	virtual void  UpdateModelData() = 0{ *(int*)0 = 10; }		/// 모델 데이터 갱신..
#endif

	bool	Use_ITEM (WORD wUseItemNO);
	bool	Set_EquipITEM (short nEquipIDX, tagITEM &sITEM);
	bool	Check_PatEquipCondition(tagITEM &sITEM );
	bool	Check_EquipCondition (tagITEM &sITEM);
	bool	Check_JobCollection( short nClassStbIDX );
	
	virtual void  SetCur_PartITEM (short nPartIdx, tagITEM &sItem) = 0;
	virtual short GetCur_PartITEM(short nPartIdx) = 0;

public :
	__int64	Get_NeedEXP (int iLevel)
	{
		return ( m_GrowAbility.m_lPenalEXP + CCal::Get_NeedRawEXP(iLevel) );
	}

	/// 사망시 패널티 경험치
	void Set_PenalEXP (BYTE btAddPercent)
	{
		if ( this->GetCur_LEVEL() >= 10 ) 
		{
			// 10렙 이상이면 페널티 적용..
			int iNeedEXP  = CCal::Get_NeedRawEXP( m_GrowAbility.m_nLevel );
			int iPenalEXP = (int) ( iNeedEXP * btAddPercent / 100.f );
			m_iAppliedPenaltyEXP = iPenalEXP;

			if ( m_GrowAbility.m_lEXP >= iPenalEXP ) 
			{
				m_GrowAbility.m_lEXP -= iPenalEXP;
			} else 
			{
				iPenalEXP -= m_GrowAbility.m_lEXP;
				m_GrowAbility.m_lPenalEXP += iPenalEXP;
				m_GrowAbility.m_lEXP = 0;

				if ( m_GrowAbility.m_lPenalEXP > DIV02(iNeedEXP) ) 
				{
					m_iAppliedPenaltyEXP -= ( DIV02(iNeedEXP) - m_GrowAbility.m_lPenalEXP );
					m_GrowAbility.m_lPenalEXP = DIV02(iNeedEXP);
				}
			}
		}

	}

	/// 부활시 복구시킬 경험치
	void Cancel_PenalEXP (BYTE btPercent)
 	{
 		if ( this->GetCur_LEVEL() >= 10 && m_iAppliedPenaltyEXP > 0 ) 
		{
 			// 10렙 이상이면 적용했던 페널티 해제...
 			m_iAppliedPenaltyEXP = (int)( m_iAppliedPenaltyEXP * btPercent / 100.f );
 			if ( m_iAppliedPenaltyEXP ) {
 				m_GrowAbility.m_lEXP += m_iAppliedPenaltyEXP;
 			}
 		}
 		m_iAppliedPenaltyEXP = 0;
 	}


	short Get_NeedPoint2AbilityUP (short BasicAbilityType)
	{
		_ASSERT( BasicAbilityType >= 0 && BasicAbilityType < 6 );

		return (short)( m_BasicAbility.m_nBasicA[ BasicAbilityType ] * 0.2 );
	}

	// skill stb의 컬럼값을 사용하는것으로...
	short Get_NeedPoint2SkillUP (short nSkillSLOT);

	//  -------------------------------------------------------------------------------------------
	/// 스킬 조건 관련 함수...
	virtual int		GetCur_R_WEAPON () = 0;
	virtual int		GetCur_L_WEAPON () = 0;
	virtual DWORD	GetCur_IngStatusFLAG() = 0;

	/// 스킬의 빈 스롯 찾기
	short Skill_FindEmptySlot( short nSkillIDX );

	/// 습득한 스킬의 레벨을 얻는다.
	short Skill_FindLearnedLevel( short nSkillIDX );

	/// 습득한 스킬의 슬롯을 찾는다.
	short Skill_FindLearnedSlot ( short nSkillIDX );

	/// 능력치 얻는 함수가 있지만 스킬에서 사용하는 능력치는 소수에 불과 함으로
	/// 속도를 높이기 위해서 스킬에 사용하는 능력치만 적용한 함수...
	int   Skill_GetAbilityValue( short nAbilityType );

	/// 스킬에서 소모되는 수치 적용...
	int   Skill_ToUseAbilityVALUE(short nSKillIDX, short nPropertyIDX );
	bool  Skill_UseAbilityValue( short nSkillIDX );


	bool Skill_CheckJOB( short nSkillIDX );				/// 습득, 렙업등에서 직업 체크 
	bool Skill_CheckLearnedSKILL( short nSkillIDX );	/// 습득, 렙업등에서 이미 습득한 스킬 체크 
	bool Skill_CheckNeedABILITY( short nSkillIDX );		/// 습득, 렙업등에서 사용자 능력치 체크..

	/// 스킬 처음 습득 조건 체크 함수
	BYTE  Skill_LearnCondition( short nSkillIDX );		

	/// 스킬 레벨업 조건 체크 함수
	BYTE  Skill_LevelUpCondition( short nCurLevelSkillIDX, short nNextLevelSkillIDX );

	/// 스킬 사용 조건 체크 함수
	bool  Skill_ActionCondition( short nSkillIDX );

	/// 스킬 습득 :: 습득이나 레벨업시 호출해야 패시브 스킬 적용됨..
	BYTE  Skill_LEARN( short nSkillSLOT, short nSkillIDX, bool bSubPOINT=true );

	/// 습득한 스킬을 삭제한다.
	bool  Skill_DELETE( short nSkillSLOT, short nSkillIDX );

	/// 패시브 스킬 관련 함수.
	void  InitPassiveSkill ();				/// 서버로 부터 스킬 데이타 받으면 한번 호출해야함...
	short GetPassiveSkillAttackSpeed()		{	return m_nPassiveAttackSpeed + m_iAddValue[ AT_ATK_SPD ];	}
	//short GetPassiveSkillAttackSpeed();

	short GetBuySkillVALUE()				{	return GetPassiveSkillValue( AT_PSV_BUYING  );	}
	short GetSellSkillVALUE()				{	return GetPassiveSkillValue( AT_PSV_SELLING );	}

	//  -------------------------------------------------------------------------------------------
	/// 퀘스트 조건 관련 함수...
#ifdef	__SERVER
	virtual bool Quest_IsPartyLEADER() = 0;						    /// 파티 짱이냐 ?
	virtual int	 Quest_GetPartyLEVEL() = 0;						    /// 현재 파티 레벨
	virtual int	 Quest_GetZoneNO() = 0;							    /// 현재 존 번호
	virtual int	 Quest_DistanceFrom(int iX, int iY, int iZ) = 0;	/// 현재위치까지 거리
	virtual int	 Quest_PartyMemberCNT() = 0;                        /// 파티 인원수
#else
	virtual bool Quest_IsPartyLEADER() = 0							{	*(int*)0 = 10; return false;		}	/// 파티 짱이냐 ?
	virtual int	 Quest_GetPartyLEVEL() = 0							{	*(int*)0 = 10; return -1;			}	/// 현재 파티 레벨
	virtual int	 Quest_GetZoneNO() = 0								{	*(int*)0 = 10; return 0;			}	/// 현재 존 번호
	virtual int	 Quest_DistanceFrom(int iX, int iY, int iZ) = 0		{	*(int*)0 = 10; return 999999999;	}	/// 현재위치까지 거리
	virtual int	 Quest_PartyMemberCNT() = 0							{	*(int*)0 = 10; return 0;			}	/// 파티 인원수
#endif

	BYTE    Quest_GetRegistered( int iQuestIDX );
	short	Quest_Append( int iQuestIDX );
	bool	Quest_Append( BYTE btSlot, int iQuestIDX );
	bool	Quest_Delete( int iQuestIDX );
	bool	Quest_Delete( BYTE btSLOT, int iQuestID );
	tagITEM*Quest_FindEquipITEM(WORD wEquipIDX)		{	return ( wEquipIDX < MAX_EQUIP_IDX ) ? &m_Inventory.m_ItemEQUIP[ wEquipIDX ] : NULL;		}
	tagITEM*Quest_FindITEM( tagITEM &sFindITEM )	
	{
		short nFindSLOT = m_Inventory.FindITEM( sFindITEM );
		return ( nFindSLOT >= 0 ) ? &m_Inventory.m_ItemLIST[ nFindSLOT ] : NULL;
	}
	bool	Quest_SubITEM( tagITEM &sSubITEM );

	short Quest_GetEpisodeVAR( WORD wVarIDX )		{	return ( wVarIDX < QUEST_EPISODE_VAR_CNT	) ? m_Quests.m_nEpisodeVAR	[ wVarIDX ] : 0;	}
	short Quest_GetJobVAR( WORD wVarIDX )			{	return ( wVarIDX < QUEST_JOB_VAR_CNT		) ? m_Quests.m_nJobVAR		[ wVarIDX ] : 0;	}
	short Quest_GetUnionVAR( WORD wVarIDX )			{	return ( wVarIDX < QUEST_UNION_VAR_CNT		) ? m_Quests.m_nUnionVAR	[ wVarIDX ] : 0;	}
	short Quest_GetPlanetVAR( WORD wVarIDX )		
	{	
		if ( wVarIDX < QUEST_PLANET_VAR_CNT	) 
			return m_Quests.m_nPlanetVAR[ wVarIDX ];
#ifdef	__APPLY_EXTAND_QUEST_VAR
		else
		if ( wVarIDX < QUEST_PLANET_VAR_CNT+QUEST_ETC_VAR_CNT ) {
			return m_Quests.m_nEtcVAR[ wVarIDX-QUEST_PLANET_VAR_CNT ];
		}
#endif
		return 0;
	}

	void Quest_SetEpisodeVAR( WORD wI, short nV )	{	if ( wI < QUEST_EPISODE_VAR_CNT )	m_Quests.m_nEpisodeVAR[ wI ] = nV;	}
	void Quest_SetJobVAR	( WORD wI, short nV )	{	if ( wI < QUEST_JOB_VAR_CNT )		m_Quests.m_nJobVAR[ wI ] = nV;		}
	void Quest_SetUnionVAR	( WORD wI, short nV )	{	if ( wI < QUEST_UNION_VAR_CNT )	m_Quests.m_nUnionVAR[ wI ] = nV;	}
	void Quest_SetPlanetVAR	( WORD wI, short nV )	
	{	
		if ( wI < QUEST_PLANET_VAR_CNT ) 
			m_Quests.m_nPlanetVAR[ wI ] = nV;
#ifdef	__APPLY_EXTAND_QUEST_VAR
		else
		if ( wI < QUEST_PLANET_VAR_CNT+QUEST_ETC_VAR_CNT ) {
			m_Quests.m_nEtcVAR[ wI-QUEST_PLANET_VAR_CNT ] = nV;
		}
#endif
	}

	// 퀘스트 보상 관련 함수
#ifdef	__SERVER
	virtual bool Reward_WARP( int iZoneNO, tPOINTF &PosGOTO) = 0;
#endif

	virtual bool Quest_CHANGE_SPEED()							{	return true;	}

	virtual bool Add_MoneyNSend( int iAmount, WORD wType )		{	return true;	}
	virtual bool Add_ExpNSend( int iExp )						{	return true;	}
	virtual bool Add_ItemNSend( tagITEM &sITEM )				{	return true;	}
	virtual bool Add_SkillNSend( short nSkillIDX )				{	return true;	}
	virtual bool Sub_SkillNSend( short nSkillIDX )				{	return true;	}
	virtual bool Set_AbilityValueNSend( WORD nType, int iValue ){	return true;	}
	virtual void Add_AbilityValueNSend( WORD wType, int iValue ){	/* nop */		}

	bool Reward_InitSTATUS (void);
	bool Reward_InitSKILL (void);
	bool Reward_ITEM	( tagITEM &sITEM, BYTE btRewardToParty, BYTE btQuestSLOT );
	bool Reward_ABILITY	( bool bSetOrAdj, int iType, int iValue, BYTE btRewardToParty );
	bool Reward_CalEXP	( BYTE btEquation, int iBaseValue, BYTE btRewardToParty );
	bool Reward_CalMONEY( BYTE btEquation, int iBaseValue, BYTE btRewardToParty, short nDupCNT );
	bool Reward_CalITEM	( BYTE btEquation, int iBaseValue, BYTE btRewardToParty, int iItemSN, short nItemOP, BYTE btQuestSLOT );

public :
	DWORD	m_dwCoolItemStartTime[ MAX_USEITEM_COOLTIME_TYPE ];
	DWORD	m_dwCoolItemEndTime	 [ MAX_USEITEM_COOLTIME_TYPE ];
} ;


//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif
