#ifndef	__CAI_FILE_H
#define	__CAI_FILE_H
#include "CAI_LIB.h"

#pragma pack()		// default

//-------------------------------------------------------------------------------------------------

#define	AIG_NAME_MAX	32
typedef	DWORD			AITYPE;


#define		AICOND_GENERAL				0x04000000						// 일반적인 조건들

#define		AICOND_00					(0x000000001 | AICOND_GENERAL)	// 조건-00 , 자신이 비전투중/대기중 일경우
#define		AICOND_01					(0x000000002 | AICOND_GENERAL)
#define		AICOND_02					(0x000000003 | AICOND_GENERAL)
#define		AICOND_03					(0x000000004 | AICOND_GENERAL)
#define		AICOND_04					(0x000000005 | AICOND_GENERAL)
#define		AICOND_05					(0x000000006 | AICOND_GENERAL)
#define		AICOND_06					(0x000000007 | AICOND_GENERAL)
#define		AICOND_07					(0x000000008 | AICOND_GENERAL)
#define		AICOND_08					(0x000000009 | AICOND_GENERAL)
#define		AICOND_09					(0x00000000A | AICOND_GENERAL)
#define		AICOND_10					(0x00000000B | AICOND_GENERAL)
#define		AICOND_11					(0x00000000C | AICOND_GENERAL)

/// 지금이 낮인가 / 밤인가 ?
#define		AICOND_12					(0x00000000D | AICOND_GENERAL)
/// 자신/타겟에게 걸려있는 상태 체크.
#define		AICOND_13					(0x00000000E | AICOND_GENERAL)
// NPC 변수 체크
#define		AICOND_14					(0x00000000F | AICOND_GENERAL)
#define		AICOND_15					(0x000000010 | AICOND_GENERAL)
#define		AICOND_16					(0x000000011 | AICOND_GENERAL)

/// NPC 선택
#define		AICOND_17					(0x000000012 | AICOND_GENERAL)
/// 자신과 주인캐릭터(자신을 소환한 아바타)와의 거리 체크
#define		AICOND_18					(0x000000013 | AICOND_GENERAL)

/// 현재 존시각 체크
#define		AICOND_19					(0x000000014 | AICOND_GENERAL)


///////////////////////////////////////////////////////////////////////////////////////////////////
// 파일 헤더
// 파일 헤더 바로 다음에 파일 제목이 옴
struct	AI_FILE_HEADER 
{
	int		iNumOfPattern ;
	int		iSecond ;				// 정지상태일때 체크 초단위
	int		iSecondOfAttackMove;	// 타격시 체크확률
	int		iNumOfTitle ;
} ;

//======================================================================================
//Structure Name : stEvent
//======================================================================================
struct stEvent {
	char szEventName[AIG_NAME_MAX] ;
	int		iNumOfCond ;
//	std::vector<stCond *> vecpCond ;
	int		iNumOfAct ;
//	std::vector<stAct *> vecpAct ;
} ;

//============================================================================
//struct stPattern
//============================================================================
struct stPattern 
{ 
	char						szName[AIG_NAME_MAX] ;
	int							iNumOfEvent ;
//	std::vector<stEvent *>		vecEvent ;
} ;





////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_00
// Explain : 
//
struct		AICOND00
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	BYTE			cNotfightOrDelay ;			// 0= 비전투 , 1=대기
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_01
// Explain : 
//
struct		AICOND01
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDamage;					// 데미지량
	BYTE			cRecvOrGive ;				// 0=받다 , 1=주다
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_02
// Explain : 
//
/*
struct		AICOND02 
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 체크거리
	BYTE			cChrType ;					// 캐릭터 타입 (아바타,몬스터)
	char			cLevelDiff ;				// 레벨차 (-128~127 ) 
	WORD			wChrNum ;					// 캐릭터 숫자
} ;
*/
/****************************************************************************************************
 * Struct Name : AICOND_02 - <<2004년 1월 2일 수정>>
 * 주변캐릭터 상황 체크 (관련 클래스 CCondDlg_02)
 * Explain : 체크범위 iDistance이내에 캐릭터타입 cChrType이고 레벨차가 cLevelDiff이상이고 캐릭터수가 wChrNum이상인 경우
 */
struct		AICOND02
{
	DWORD			dwSize ;				// 이 스트럭쳐의 크기
	AITYPE			Type ;					// 조건 타입
	int				iDistance;				// 체크거리
	BYTE			btIsAllied;				// 캐릭터 타입 (적군=0,아군=1)
	short			nLevelDiff ;			// 레벨차 (-128~127 )  이거 이상
	short			nLevelDiff2;			// 레벨차 (-128~127 )  이거 이상
	WORD			wChrNum;
	/*
	union {
		struct {
			char	cLevelDiff2 ;			// 레벨차 (-128~127 )  이거 이하. 예: cLevelDiff이상이고 cLevelDiff2이하인 .. <<2004년 1월 2일 수정>>
			WORD	wChrNum ;				// 캐릭터 숫자
		} ;
		struct {
			WORD	wChrNum_Before;
		} ;
	} ;
	*/
} ;



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_03
// Explain : 
//
struct		AICOND03
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_04
// Explain : 
//
struct		AICOND04
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cMoreLess ;					// 이상 이하
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_05
// Explain : 
//
struct		AICOND05
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
//	int				cAbType ;					// 능력치 타입
	BYTE			cAbType ;					// 능력치 타입
	int				iDiff ;
	BYTE			cMoreLess ;					// 큰가 , 작은가
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_06
// Explain : 
//
struct		AICOND06
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	DWORD			wHP ;						// HP 수치
	BYTE			cMoreLess ;					// 큰가 , 작은가
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AICOND_07
// Explain : 
//
struct		AICOND07
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	BYTE			cPercent ;					// 확률
} ;

struct		AICOND08
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	short			nLevelDiff ;				// 레벨차 (-128~127 ) 자신의 레벨보다 cLevelDiff이상인
	short			nLevelDiff2;				// 레벨차 (-128~127 ) 자신의 레벨보다 cLevelDiff이상인
	//BYTE			cChrType ;					// 캐릭터 타입 (아바타=0,몬스터=1)
	BYTE			btIsAllied;					// 캐릭터 타입 (적군=0,아군=1)
} ;

struct		AICOND09
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
} ;

struct		AICOND10
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	BYTE			cAbType ;					// 능력치 타입
	BYTE			cMoreLess ;					// 큰=0 , 작은=1
} ;

/*******************************************************************************************
 * Struct Name : AICOND_11
 * Explain : 자신을 공격한 캐릭터의 A능력치가 B값보다 크거나/작은 경우
 */
struct		AICOND11
{
	DWORD			dwSize ;		// 이 스트럭쳐의 크기
	AITYPE			Type ;			// 조건 타입 = AICOND_11
	BYTE			cAbType ;		// 능력치 (레벨=0, 공격력=1, 방어력=2, 항마력=3, HP=4, 매력=5)
	int				iValue;			// 비교값. 이값보다 큰가/작은가. 이런식으로
	BYTE			cMoreLess ;		// 큰=0 , 작은=1
} ;


// 여기서부터 서버소스와 맞는지 비교
/// 지금이 낮인가 / 밤인가 ?
struct		AICOND12
{
	DWORD			dwSize ;		// 이 스트럭쳐의 크기
	AITYPE			Type ;			// 조건 타입 = AICOND_12
	BYTE			cWhen;			// 0 = 낮인지 체크, 1 = 밤인지 체크
};

/// 자신/타겟에게 걸려있는 상태 체크. 
struct		AICOND13
{
	DWORD			dwSize;
	AITYPE			Type;

	BYTE			btCheckTarget;	// 0 = 자기자신, 1 = 공격타겟
	BYTE			btStatusType;	// 0 = 해로운, 1 = 유리한, 2 = 해로운/유리한 아무거나
	BYTE			btHave;			// 0 = 없는가 ? 1 = 있는가 ?
};

// NPC 변수 체크	- AICOND14 AIACT25
// 월드변수 체크	- AICOND15 AIACT26
// 경제 데이터 체크 - AICOND16 AIACT27
typedef struct	tagValueAI
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// 변수 번호 : 0 ~ , 존의 경제데이터 변수
	int				iValue;	// 비교할 데이터값 (iType의 능력치 Op iValue)
	BYTE			btOp;	// iValue값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
							// (액션쪽) 5 = 값바꿈, 6 = 증가, 7 = 감소
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27;


/// NPC 선택
#define		AICOND_17					(0x000000012 | AICOND_GENERAL)

/// NPC 선택
struct AICOND17
{
	DWORD			dwSize;
	AITYPE			Type;

//	short			nZoneNo;
	int				iNpcNo;		/// 선택할 NPC 번호
};


/// 자신과 주인캐릭터(자신을 소환한 아바타)와의 거리 체크 - 소환된 몹만 사용
struct AICOND18
{
	DWORD			dwSize;
	AITYPE			Type;


	int				iDistance;	// 선택할 NPC 번호. (주인과의 거리 Op iDistance)
	BYTE			btOp;		// 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.

	// 소환된 몹 거리 관련 있으면 아래 활용. 2004-4-9 현재는 위만 사용
	// BYTE			btOrg;		// 0 = 소환한 캐릭터, ... 
};


/// 현재 존시각 체크
struct AICOND19
{
	DWORD			dwSize;
	AITYPE			Type;

	unsigned long	ulTime;		//
	unsigned long	ulEndTime;	// ulTime <= 현재 존시각 <= ulEndTime 인가 ?
};


// 자신(몬스터)의 능력치/기타/주인캐릭터 관련 데이터 체크
struct AICOND20
{
	DWORD	dwSize;
	AITYPE	Type;

	BYTE	btAbType ;		// 능력치 (레벨=0, 공격력=1, 방어력=2, 항마력=3, HP=4, 매력=5)

	int		iValue;	// 비교할 데이터값 (iType의 능력치 Op iValue)
	BYTE	btOp;	// iData값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.					
};

// 주인캐릭터가 존재 유무 확인
struct AICOND21
{
	DWORD	dwSize;
	AITYPE	Type;
};

// 주인캐릭터가 현재 공격타겟을 가지고 있는가 ?
struct AICOND22
{
	DWORD	dwSize;
	AITYPE	Type;
};

/// 월드시각 체크
struct AICOND23
{
	DWORD			dwSize;
	AITYPE			Type;

	unsigned long	ulTime;		//
	unsigned long	ulEndTime;	// ulTime <= 현재 존시각 <= ulEndTime 인가 ?
};


/// 날짜 + 시각 체크
struct AICOND24
{
	DWORD			dwSize;
	AITYPE			Type;

	BYTE			btDate;		// 날짜. (1 ~ 31)

	BYTE			btHour1;	// 시 (1 ~ 24)
	BYTE			btMin1;		// 분 (1 ~ 60)
	BYTE			btHour2;	// 시
	BYTE			btMin2;		// 분
	//  현재의 날짜가 btDate 이고, btHour1시 btMin1분  <= 현재 시각 <= btHour1시 btMin1분 이면 true
};

/// 요일 + 시각 체크
struct AICOND25
{
	DWORD			dwSize;
	AITYPE			Type;

	BYTE			btWeekDay;	// 요일 (0 ~ 6)

	BYTE			btHour1;	// 시
	BYTE			btMin1;		// 분
	BYTE			btHour2;	
	BYTE			btMin2;		
	//  현재의 요일이 btWeekDay 이고, btHour1시 btMin1분  <= 현재 시각 <= btHour1시 btMin1분 이면 true
};

/// 채널 서버 체크
struct AICOND26
{
 	DWORD			dwSize;
 	AITYPE			Type;
 	unsigned short	nX; 
 	unsigned short	nY; // (nX <= 채널서버 <= nY) 이면 true
};


struct stCondHead 
{
	DWORD	dwSize ;
	AITYPE	Type ;
} ;

struct AI_CONDITION		// stCond
{
	union 
	{
		BYTE			m_pDATA[ 1 ];
		stCondHead		m_Header;
		/*
		struct 
		{
			DWORD	dwSize ;
			AITYPE	Type ;
		} ;
		*/
		//------------------------------------------------------------------------------
		// 이하 존건 스트럭쳐 모두 리스트 할껏
		AICOND00 st00;
		AICOND01 st01;
		AICOND02 st02;
		AICOND03 st03;
		AICOND04 st04;
		AICOND05 st05;
		AICOND06 st06;
		AICOND07 st07;
		AICOND08 st08;
		AICOND09 st09;
		AICOND10 st10;

		AICOND11 st11;
		AICOND12 st12;
		AICOND13 st13;
		AICOND14 st14;
		AICOND15 st15;
		AICOND16 st16;
		AICOND17 st17;

		AICOND18 st18;
	} ;
} ;


#define		AIACT_GENERAL				0x0B000000

#define		AIACT_00					(0x000000001 | AIACT_GENERAL) 
#define		AIACT_01					(0x000000002 | AIACT_GENERAL) 
#define		AIACT_02					(0x000000003 | AIACT_GENERAL) 
#define		AIACT_03					(0x000000004 | AIACT_GENERAL) 
#define		AIACT_04					(0x000000005 | AIACT_GENERAL) 
#define		AIACT_05					(0x000000006 | AIACT_GENERAL) 
#define		AIACT_06					(0x000000007 | AIACT_GENERAL) 
#define		AIACT_07					(0x000000008 | AIACT_GENERAL) 
#define		AIACT_08					(0x000000009 | AIACT_GENERAL) 
#define		AIACT_09					(0x00000000A | AIACT_GENERAL) 
#define		AIACT_10					(0x00000000B | AIACT_GENERAL) 
#define		AIACT_11					(0x00000000C | AIACT_GENERAL) 
#define		AIACT_12					(0x00000000D | AIACT_GENERAL)
#define		AIACT_13					(0x00000000E | AIACT_GENERAL)
#define		AIACT_14					(0x00000000F | AIACT_GENERAL)
#define		AIACT_15					(0x000000010 | AIACT_GENERAL)
#define		AIACT_16					(0x000000011 | AIACT_GENERAL)
#define		AIACT_17					(0x000000012 | AIACT_GENERAL)

#define		AIACT_18					(0x000000013 | AIACT_GENERAL)
#define		AIACT_19					(0x000000014 | AIACT_GENERAL)
#define		AIACT_20					(0x000000015 | AIACT_GENERAL)

// #define		AIACT_21					(0x000000016 | AIACT_GENERAL)	// 기술 구현. ** 사용 안함
#define		AIACT_22					(0x000000017 | AIACT_GENERAL)	// 지정된 몬스터를 지정된 위치에 소환시킨다
#define		AIACT_23					(0x000000018 | AIACT_GENERAL)	// 자살 시켜 버린다
#define		AIACT_24					(0x000000019 | AIACT_GENERAL)	// 대상을 향해 스킬/동작 구현


#define		AIACT_25					(0x00000001A | AIACT_GENERAL)	
#define		AIACT_26					(0x00000001B | AIACT_GENERAL)	
#define		AIACT_27					(0x00000001C | AIACT_GENERAL)	
#define		AIACT_28					(0x00000001D | AIACT_GENERAL)	

// 주인캐릭터(자기를 소환한 캐릭터)가 있는 곳으로 이동
#define		AIACT_29					(0x00000001E | AIACT_GENERAL)	
// 트리거실행
#define		AIACT_30					(0x00000001F | AIACT_GENERAL)	

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT00
// Explain : 
//

struct		AIACT00
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT01
// Explain : 
//

struct		AIACT01
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	BYTE			cAction ;					// 동작번호
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT02
// Explain : 
//

struct		AIACT02
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	// char			szCon[1] ;					// 대사 데이터
	int				iStrID;
} ;
struct		AIACT02_STR
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	char			szCon[1] ;					// 대사 데이터
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT03
// Explain : 
//

struct		AIACT03
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cSpeed ;					// 0: 걷기, 1: 뛰기
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT04
// Explain : 
//

struct		AIACT04
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cSpeed ;					// 0: 걷기, 1: 뛰기
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT05
// Explain : 
//
struct		AIACT05
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	BYTE			cSpeed ;					// 0: 걷기, 1: 뛰기
	//BYTE			cAttackOrMove ;				// 공격 , 이동
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT06
// Explain : 
//

struct		AIACT06
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cAbType ;					// 능력치 타입
	BYTE			cMoreLess ;					// 큰가 , 작은가
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT07
// Explain : 
//

struct		AIACT07
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
												// 데이터 필요 없음
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT08
// Explain : 
//

struct		AIACT08
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cSpeed ;					// 0: 걷기, 1: 뛰기
} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT09
// Explain : 
//

typedef struct		tagAIACT09
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
//	BYTE			cMonster ;					// 몬스터번호
	WORD			wMonster ;					// 몬스터번호
} AIACT09 , AIACT10 ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT10
// Explain : 
//

//struct		AIACT10
//{
//	DWORD			dwSize ;					// 이 스트럭쳐의 크기
//	AITYPE			Type ;						// 조건 타입
//} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Struct Name : AIACT11
// Explain : 
//

struct		AIACT11
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
//	BYTE			cCallType ;					// 공격호출 형태 , (모든 , 비전투중인)
	int				iNumOfMonster;
} ;


struct		AIACT12
{
		DWORD			dwSize ;		// 이 스트럭쳐의 크기
		AITYPE			Type ;			// 조건 타입
} ;


struct		AIACT13
{
		DWORD			dwSize ;		// 이 스트럭쳐의 크기
		AITYPE			Type ;			// 조건 타입
} ;

struct		AIACT14
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
} ;

struct		AIACT15
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
} ;


/**********************************************************************************
 * Struct Name : AIACT16
 * 도망 : 이동과 구분됨. 도망도중에는 맞아도 계속 도망감
 */
struct AIACT16
{
	DWORD	dwSize;		/// 이 스트럭쳐의 크기
	AITYPE	Type;		/// Type = AIACT_16 . 도망
	int		iDistance ;	/// 리젠을 중심으로 몇 미터 이내로 도망
};

/**********************************************************************************
 * Struct Name : AIACT17
 * 도망 : 아이템번호를 5개 가지고 있다가 랜덤하게 떨어뜨림. 아이템 번호는 5자리
 */
struct AIACT17
{
	DWORD	dwSize;		/// 이 스트럭쳐의 크기
	AITYPE	Type;		/// Type = AIACT_16 . 도망
	union {
		struct {
			short	item0;		/// 아이템 번호 0  . 만약 아이템번호가 없으면 -1을 집어 넣는다
			short	item1;		/// 아이템 번호 1
			short	item2;		/// 아이템 번호 2
			short	item3;		/// 아이템 번호 3
			short	item4;		/// 아이템 번호 4 
		} ;
		short	m_ITEMS[ 5 ];
	} ;
};


/**********************************************************************************
 * Struct Name : AIACT18 - <<2004.1.29 추가>>
 * 지정된 번호의 비전투중인 몹에게 공격호출 명령. (몬스터번호, 거리, 마리수) 
 * 추가 설명 : 지정된 거리안에 지정된 마리수만큼의 지정된 번호의 몬스터에게 공격호출 명령
 */
struct AIACT18
{
	DWORD		dwSize;			/// 이 스트럭쳐의 크기
	AITYPE		Type;			/// Type값은 AIACT_18

	WORD		cMonster ;		// 몬스터번호
	WORD		wHowMany ;		// 몇마리
	int			iDistance ;		// 거리
};


/**********************************************************************************
 * Struct Name : AIACT19 - <<2004.1.29 추가>>
 * 가장 가까이 있는 아바타를 공격타겟으로 지정 
 */
struct AIACT19
{
	DWORD		dwSize;				/// 이 스트럭쳐의 크기
	AITYPE		Type;				/// Type값은 AIACT_19

	/// 추가 조건 필요
};


/**********************************************************************************
 * Struct Name : AIACT20 - <<2004.1.29 추가>>
 * 지정된 몬스터를 소환시킨다. 자신/공격자(자신을 공격한 캐릭터)/공격타겟(자신이 공격하는 캐릭터).
 * 근처 ?미터 이내에 소환 
 * 추가설명 : 마리수는 1마리이다.
 */
struct AIACT20
{
	DWORD		dwSize;				/// 이 스트럭쳐의 크기
	AITYPE		Type;				/// Type값은 AIACT_20

	WORD		cMonster;			/// 몬스터번호
	BYTE		btPos;				/// 소환위치 : 0 = 자신 주위,  1 = 공격자 주위(자신을 공격한), 2 = 공격타겟 주위(자신이 공격하고 있는)
	int			iDistance ;			// 거리 . 이 거리 이내에 소환
};



/**********************************************************************************
 * Struct Name : AIACT22 - <<2004.2.21 추가>>
 * 지정된 몬스터를 지정된 위치에 소환
 */
/*
struct AIACT22
{
	DWORD		dwSize;			/// 이 스트럭쳐의 크기
	AITYPE		Type;			/// Type값은 AIACT_22

};
*/

// 자살시킨다
struct AIACT23
{
	DWORD	dwSize;
	AITYPE	Type;
};

/// 스킬과 동작을 대상을 향해 사용
struct AIACT24
{
	DWORD		dwSize;			/// 이 스트럭쳐의 크기
	AITYPE		Type;			/// Type

	BYTE		btTarget;		// 0=조건에 체크된 캐릭터, 1=현재 공격타겟, 2=자기자신(셀프스킬)
	short		nSkill;			// 스킬번호
	short		nMotion;		// 동작번호 -1 = 동작없음
};

struct AIACT28
{
	DWORD		dwSize;			/// 이 스트럭쳐의 크기
	AITYPE		Type;			/// Type

	BYTE		btMsgType;		/// 0 = 현재필드, 1 = 현재존, 2=전체월드
	// short	nMsgLength;		/// 대사의 길이 NULL포함
	// char		szMsg[ 1 ];		/// 대사
	int			iStrID;
};
struct AIACT28_STR
{
	DWORD		dwSize;			/// 이 스트럭쳐의 크기
	AITYPE		Type;			/// Type

	BYTE		btMsgType;		/// 0 = 현재필드, 1 = 현재존, 2=전체월드
	char		szMsg[ 1 ];		/// 대사
};


// 주인캐릭터(자신을 소환한 캐릭터)가 있는 곳으로 이동
struct AIACT29
{
	DWORD		dwSize;
	AITYPE		Type;

	// 2004-4-9 현재까지는 데이터 없음
	// short	nCheckUnit; // 얼마 만큼 단위로 체크 . 이것도 들어갈 수 있음. 단위=시간, 프레임
	// BYTE		btSpeed; // 0 = 천천히 , 1 = 빨리 (소환자에게 가는데 이동속도 있을수 있음)
	// BYTE		btAddOp; // 소환자가 죽었을 경우 (또는 없을 경우) 돌아갈 위치 없음
};


// 트리거 실행
struct AIACT30
{
	DWORD		dwSize;
	AITYPE		Type;

	short		shNameLen;

//  union 설정하면 struct 구조정렬이 에디터와 틀려져 삑사리남
//	union {
//		t_HASHKEY	m_HashNextTrigger[0];	/// 로딩시 트리거명을 헤쉬값으로 변경 시킴...
		char		szTrigger[ 1 ];	// 트리거명. NULL포함
//	} ;
};


// 주인이 가지고 있는 공격타겟 공유
struct AIACT31
{
	DWORD		dwSize;
	AITYPE		Type;
};

// PK 모드 On/Off. NPC가 존재하는 존에.
struct AIACT32
{
	DWORD		dwSize;
	AITYPE		Type;

	short		nZoneNo;	// 존번호, 현재는 무조건 0
	BYTE		btOnOff;	// 0 = Off , 1 = On
};


struct stActHead 
{
	DWORD	dwSize ;
	AITYPE	Type ;
} ;
struct AI_ACTION	// stAct
{
	union 
	{
		BYTE			m_pDATA[ 1 ];
		stActHead		m_Header;
		/*
		struct
		{
			DWORD	dwSize ;
			AITYPE	Type ;
		} ;
		*/
		AIACT00			st00 ;
		AIACT01			st01 ;
		AIACT02			st02 ;
		AIACT03			st03 ;
		AIACT04			st04 ;
		AIACT05			st05 ;
		AIACT06			st06 ;
		AIACT07			st07 ;
		AIACT08			st08 ;
		AIACT09			st09 ;
		AIACT10			st10 ;
		AIACT11			st11 ;
		AIACT12			st12 ;
		AIACT13			st13 ;
		AIACT14			st14 ;
		AIACT15			st15 ;
		AIACT16			st16 ;
		AIACT17			st17 ;
		AIACT18			st18 ;
		AIACT19			st19 ;
		AIACT20			st20 ;

		// AIACT22		st22 ;
		AIACT23			st23 ;
		AIACT24			st24 ;
		AIACT25			st25 ;
		AIACT26			st26 ;
		AIACT27			st27 ;
		AIACT28			st28 ;
		AIACT29			st29 ;
		AIACT30			st30 ;
		AIACT31			st31 ;

		AIACT32			st32 ;
	} ;
} ;


//-------------------------------------------------------------------------------------------------
struct t_AIPARAM {
	CAI_OBJ	*m_pSourCHAR;	
	CAI_OBJ	*m_pDestCHAR;
	CAI_OBJ	*m_pFindCHAR;
	CAI_OBJ	*m_pNearCHAR;
#ifdef	__SERVER
	CObjNPC *m_pLocalNPC;
#endif

	int	     m_iDamage;
} ;

class CFileSystem;
class CAI_EVENT
{
private:
	AI_CONDITION  **m_ppConditionLIST;
	int				m_iConditionCNT;

	AI_ACTION	  **m_ppActionLIST;
	int				m_iActionCNT;

public :
	CAI_EVENT ();
	~CAI_EVENT ();

#ifdef	__SERVER
	bool Load (FILE *fp, STBDATA *pSTB, int iLangCol);
#else
	bool Load (CFileSystem* pFileSystem, STBDATA *pSTB, int iLangCol );
#endif
	bool Check (t_AIPARAM *pAIParam);
} ;

//-------------------------------------------------------------------------------------------------


class CAI_PATTERN
{
private:
	CAI_EVENT	*m_pEvent;
	int			 m_iEventCNT;

public :
	CAI_PATTERN ();
	~CAI_PATTERN ();

#ifdef	__SERVER
	bool Load ( FILE *fp, STBDATA *pSTB, int iLangCol );
#else
	bool Load ( CFileSystem* pFileSystem, STBDATA *pSTB, int iLangCol );
#endif
	void Check( t_AIPARAM *pAIParam );
} ;


//-------------------------------------------------------------------------------------------------
struct tagConditionFUNC {
	bool (*m_fpCondition) (stCondHead *pConDATA, t_AIPARAM *pParam);
} ;
extern tagConditionFUNC g_FuncCOND[];

struct tagActionFUNC {
	void (*m_fpAction) (stActHead *pActDATA, t_AIPARAM *pParam);
} ;
extern tagActionFUNC	g_FuncACTION[];


//-------------------------------------------------------------------------------------------------

int  AI_Get_Ability (CAI_OBJ *pCHAR, int iAbTYPE);

//-------------------------------------------------------------------------------------------------

template <class dType1, class dType2>
inline bool Check_AiOP( BYTE btOP, dType1 iLeft, dType2 iRight)
{
	bool bResult;
	switch( btOP ) {
		case 0 :	// 같다
			bResult = ( iLeft == iRight );	break;
		case 1 :	// 크다
			bResult = ( iLeft >  iRight );	break;
		case 2 :	// 크거나 같다
			bResult = ( iLeft >= iRight );	break;
		case 3 :	// 작다
			bResult = ( iLeft <  iRight );	break;
		case 4 :	// 작거나 같다.
			bResult = ( iLeft <= iRight );	break;
		case 10 :	// 같지 않다.
			bResult = ( iLeft != iRight );	break;
		default :
			return false;
	}

	return bResult;
}

template <class dType>
inline dType Result_AiOP( BYTE btOP, dType iLeft, dType iRight)
{
	switch( btOP ) {
		case 5 :	// 값바꿈
			return iRight;
		case 6 :	// 증가(주어진 만큼)
			return (iLeft+iRight);
		case 7 :	// 감소
			return (iLeft-iRight);
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
#endif
