#ifndef	__IO_SKILL_H
#define	__IO_SKILL_H

#include "IO_STB.h"
#ifndef __SERVER
	#include "../GameCommon/StringManager.h"
#endif
//-------------------------------------------------------------------------------------------------

enum {
	SKILL_TYPE_01 = 1,
	SKILL_TYPE_02 ,
	SKILL_TYPE_03 ,
	SKILL_TYPE_04 ,
	SKILL_TYPE_05 ,
	SKILL_TYPE_06 ,
	SKILL_TYPE_07 ,
	SKILL_TYPE_08 ,
	SKILL_TYPE_09 ,
	SKILL_TYPE_10 ,
	SKILL_TYPE_11 ,
	SKILL_TYPE_12 ,
	SKILL_TYPE_13 ,
	SKILL_TYPE_14 ,
	SKILL_TYPE_15 ,							// 패시브 스킬
	SKILL_TYPE_PASSIVE = SKILL_TYPE_15,
	SKILL_TYPE_16 ,							// Emotion
	SKILL_TYPE_17 ,							// Self & damage
	SKILL_TYPE_18 ,							/// 추가 2004.4.8 : warp skill
	SKILL_TYPE_19,							/* 추가 2004.6.16 :
											능력치 변경(10번유형) & 데미지공격(3번유형)
													기존의 3번스킬 유형으로 타겟에게 데미지를 주고, 자신에게는
													지정된 능력치(21번,24번 칼럼)의 수치(22,25번칼럼)가 증가된다.
													스킬성공계산식을 적용하여 성공시에만 능력치 변경을 적용시킨다.
													(타겟에 데미지공격을 하면서 HP,MP 뺏어오기 개념)
											*/
	SKILL_TYPE_20,
} ;


enum enumSKILL_TAGER_FILTER
{
	SKILL_TARGET_FILTER_SELF = 0,			///< 자기 자신
	SKILL_TARGET_FILTER_GROUP = 1,			///< 그룹(파티)원
	SKILL_TARGET_FILTER_GUILD = 2,			///< 길드원
	SKILL_TARGET_FILTER_FRIEND_ALL = 3,		///< 아군 (아바타, 소환몹)
	SKILL_TARGET_FILTER_MOB = 4,			///< 모든 몬스터 (소환몹,적군몹)
	SKILL_TARGET_FILTER_ENEMY_ALL = 5,		///< 적 PC + 몬스터 	
	SKILL_TARGET_FILTER_ENEMY_PC = 6,		///< 적 PC( 적 상태 포함 )
	SKILL_TARGET_FILTER_ALL_PC = 7,			///< 모든 PC
	SKILL_TARGET_FILTER_ALL_CHAR = 8,		///< 모든 캐릭터( 몬스터, 플레이어 )	
	SKILL_TARGET_FILTER_DEAD_USER = 9,		/// 죽은 사용자
	SKILL_TARGET_FILTER_ENEMY_MOB = 10,		/// 적 몬스터
	SKILL_TARGET_FILTER_MAX,
};

//-------------------------------------------------------------------------------------------------

#ifdef __SERVER
	#define	SKILL_NAME(I)						g_SkillList.m_SkillDATA.m_ppNAME[ I ]			// 스킬 이름
	#define SKILL_DESC(I)						g_SkillList.m_SkillDATA.m_ppDESC[ I ]			// 스킬 설명
#else
	#define	SKILL_NAME(I)						CStringManager::GetSingleton().GetSkillName( I )
	#define SKILL_DESC(I)						CStringManager::GetSingleton().GetSkillDesc( I )
#endif

#define	SKILL_1LEV_INDEX(S)					g_SkillList.m_SkillDATA.m_ppDATA[ S ][  1 ]
#define	SKILL_LEVEL(S)						g_SkillList.m_SkillDATA.m_ppDATA[ S ][  2 ]
#define	SKILL_NEED_LEVELUPPOINT(S)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][  3 ]
#define	SKILL_TAB_TYPE(S)					g_SkillList.m_SkillDATA.m_ppDATA[ S ][  4 ]
#define SKILL_TYPE( I )						g_SkillList.m_SkillDATA.m_ppDATA[ I ][  5 ]

#define SKILL_DISTANCE( I )					g_SkillList.m_SkillDATA.m_ppDATA[ I ][  6 ]		//	타겟과의 거리
#define SKILL_WARP_PLANET_NO( I )			g_SkillList.m_SkillDATA.m_ppDATA[ I ][  6 ]		//	워프 스킬일 경우 존이 위치한 행성 번호

#define SKILL_CLASS_FILTER( I )				g_SkillList.m_SkillDATA.m_ppDATA[ I ][  7 ]	
#define	SKILL_SCOPE(S)						g_SkillList.m_SkillDATA.m_ppDATA[ S ][  8 ]		// 적용범위
#define	SKILL_POWER(S)						g_SkillList.m_SkillDATA.m_ppDATA[ S ][  9 ]
#define	SKILL_ITEM_MAKE_NUM		SKILL_POWER	// 제조번호
#define SKILL_HARM( I )						g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 10 ]	

#define SKILL_STATE_STB( I,C )				g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 11+C ]	//	상태 번호
#define SKILL_STATE_STB1( I )				g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 11 ]		//	상태 번호
#define SKILL_STATE_STB2( I )				g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 12 ]		//	상태 번호
#define SKILL_SUCCESS_RATIO( I )			g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 13 ]		//	성공률
#define SKILL_DURATION( I )					g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 14 ]		//	지속 시간
#define	SKILL_DAMAGE_TYPE(S)				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 15 ]		// 적용될 계산식 유형


#define	SKILL_USE_PROPERTY_CNT				2
#define	SKILL_USE_PROPERTY(S,T)				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 16+(T)*2 ]
#define	SKILL_USE_VALUE(S,T)				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 17+(T)*2 ]


#define	SKILL_RELOAD_TIME(S)				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 20 ]

// 패시브(지속성) 스킬일 경우 일정시간 상승시킬 능력치...
#define	SKILL_INCREASE_ABILITY_CNT			2
#define	SKILL_INCREASE_ABILITY(S,T)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 21+(T)*3 ]
#define	SKILL_INCREASE_ABILITY_VALUE(S,T)	g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 22+(T)*3 ]
#define	SKILL_CHANGE_ABILITY_RATE(S,T)		g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 23+(T)*3 ]

#define	SKILL_WARP_ZONE_NO( S )				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 21 ]		// 워프 스킬일 경우 워프할 존 번호
#define	SKILL_WARP_ZONE_XPOS( S )			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 22 ]		// 워프 스킬일 경우 워프할 존 위치
#define	SKILL_WARP_ZONE_YPOS( S )			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 23 ]

#define	SKILL_RELOAD_TYPE(S)				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 27 ]		// 스킬여백

#define	SKILL_SUMMON_PET(S)					g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 28 ]		//	소환몹 번호

#define	SKILL_ACTION_MODE(S)				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 29 ]		// 스킬공격 유형
// 사용시 장착하고 있어야하는 장비.  T = 0 ~ 3
#define	SKILL_NEED_WEAPON_CNT				5		// <--- 2004. 2. 26 4->5로 늘림고 이후 컬럼 번호+1 by icarus
#define	SKILL_NEED_WEAPON(S,T)				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 30+(T) ]

/*
// 최초 배울때 필요한 직업.	T = 0 ~ 3
#define	SKILL_AVAILBLE_CLASS_CNT			4
#define	SKILL_AVAILBLE_CLASS(S,T)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 1+34+(T) ]
*/
// LIST_CLASS.stb 참조 번호...
#define	SKILL_AVAILBLE_CLASS_SET(S)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 35 ]

// 최초 배울때 소속 조합 체크...
#define	SKILL_AVAILBLE_UNION_CNT			3
#define	SKILL_AVAILBLE_UNION(S,T)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 36+(T) ]


// 최초로 배울때 미리 소유하고 있어야하는 스킬. T = 0 ~ 2
#define	SKILL_NEED_SKILL_CNT				3
#define	SKILL_NEED_SKILL_INDEX(S,T)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 39+(T)*2 ]
#define	SKILL_NEDD_SKILL_LEVEL(S,T)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 40+(T)*2 ]

// 최초로 배우거나 레벨업시 필요한 조건 능력치. T = 0 ~ 2
#define	SKILL_NEED_ABILITY_TYPE_CNT			2
#define	SKILL_NEED_ABILITY_TYPE(S,T)		g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 45+(T)*2 ]	// 값은 AT_DEX~AT_SENSE 만 사용됨
#define	SKILL_NEED_ABILITY_VALUE(S,T)		g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 46+(T)*2 ]

#define SKILL_SCRIPT1( I )					g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 49 ]		

#define SKILL_RESERVE_02( I )				g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 50 ]		
#define SKILL_ICON_NO( I )					g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 51 ]		

// 캐스팅 동작
#define	SKILL_ANI_CASTING(S)				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 52 ]
#define	SKILL_ANI_CASTING_SPEED(S)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 53 ]

/// 캐스팅 루프 동작
#define	SKILL_ANI_CASTING_REPEAT(S)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 54 ]
#define	SKILL_ANI_CASTING_REPEAT_CNT(S)		g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 55 ]

/// 캐스팅 이펙트
#define SKILL_CASTING_EFFECT_CNT			4
#define SKILL_CASTING_EFFECT( I,T )			g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 56 + (T * 3) ]
#define SKILL_CASTING_EFFECT_POINT( I,T )	g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 57 + (T * 3) ]
#define SKILL_CASTING_SOUND( I,T )			g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 58 + (T * 3) ]

// 실제 동작
#define	SKILL_ANI_ACTION_TYPE(S)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 68 ]
#define	SKILL_ANI_ACTION_SPEED(S)			g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 69 ]
#define	SKILL_ANI_HIT_COUNT(S)				g_SkillList.m_SkillDATA.m_ppDATA[ S ][ 70 ]

/// 발사 총알 이펙트
#define SKILL_BULLET_NO( I )				g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 71 ]
#define SKILL_BULLET_LINKED_POINT( I )		g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 72 ]
#define SKILL_BULLET_FIRE_SOUND( I )		g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 73 ]

/// 타격 이펙트
#define SKILL_HIT_EFFECT( I )				g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 74 ]			//	타격 효과
#define SKILL_HIT_EFFECT_LINKED_POINT( I )	g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 75 ]			//	타격 효과
#define SKILL_HIT_SOUND( I )				g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 76 ]			//	타격 효과음

/// 연타 더미 이펙트
#define SKILL_HIT_DUMMY_EFFECT_CNT			2
#define SKILL_HIT_DUMMY_EFFECT( I, T )					g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 77 + 3*T ]			//	더미 타격 효과
#define SKILL_HIT_DUMMY_EFFECT_LINKED_POINT( I, T )		g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 78 + 3*T ]			//	더미 타격 효과
#define SKILL_HIT_DUMMY_SOUND( I, T )					g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 79 + 3*T ]			//	더미 타격 효과음

#define SKILL_AREA_HIT_EFFECT( I )			g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 83 ]								//	지역 타격 효과
#define SKILL_AREA_HIT_SOUND( I )			g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 84 ]								//	지역 타격 효과음

#define	SKILL_LEVELUP_NEED_ZULY( I )		g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 85 ]				


#define	SKILL_ATTRIBUTE( I )				g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 86 ]///1 : 아바타 스킬 , 2: PAT스킬 , 3: 공통 스킬

#define SKILL_ATTRIBUTE_AVATAR				1
#define SKILL_ATTRIBUTE_CART				2
#define SKILL_ATTRIBUTE_CASTLEGEAR			4

//	스킬 레렙업시 소모 비용 / 100
//#define SKILL_DURATION_EFFECT( I )			g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 66 ]		//	지속마법 효과
//#define SKILL_DURATION_SOUND( I )			g_SkillList.m_SkillDATA.m_ppDATA[ I ][ 67 ]		//	지속마법 효과음


/*
#define	SA_SELF_STOP							0		// 스킬 적용후 정지
#define	SA_TARGET_STOP							1		// 타겟 설정, 스킬 적용후 정지 
#define	SA_TARGET_ATTACK						2		// 타겟 설정, 스킬 적용후 타겟을 공격
#define	SA_POSITION_STOP						3		// 지역에 스킬 적용후 정지
*/
#define	SA_STOP						0	// 스킬 적용후 정지
#define	SA_ATTACK					1	// 스킬 적용후 타겟을 공격
#define	SA_RESTORE					2	// 스킬 사용후 이전 명령 복귀

#ifdef	__SERVER
	#define	SKILL_RELOAD_SECOND(S)			g_SkillList.m_pReloadTIME[ S ]
#endif

class CSkillLIST 
{
private:
	int			m_iSkillCount;	

	float	   *m_pCastingAniSPEED;		/// 미리 계산되어진 스킬 케스팅 동작 속도
	float	   *m_pActionAniSPEED;		/// 미리 계산되어지 스킬 실제 동작 속도.

public :
	STBDATA		m_SkillDATA;
#ifdef	__SERVER
	DWORD	  *m_pReloadTIME;
#else
	float	  *m_pReloadTIME;
#endif

	CSkillLIST ();
	~CSkillLIST ();
	void	Free ();

	int		Get_SkillCNT()							{	return m_iSkillCount;					}
	float	Get_CastingAniSPEED(short nSkillIDX)	{	return m_pCastingAniSPEED[ nSkillIDX ];	}
	float	Get_ActionAniSPEED(short nSkillIDX)		{	return m_pActionAniSPEED[ nSkillIDX ];	}

	bool	LoadSkillTable( const char* pFileName );
	float  GetDelayTickCount(DWORD i);
} ;

extern CSkillLIST g_SkillList;

//-------------------------------------------------------------------------------------------------
#endif
