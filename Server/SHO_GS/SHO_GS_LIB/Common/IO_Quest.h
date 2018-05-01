#ifndef	__IO_QUEST_H
#define	__IO_QUEST_H
#include "../util/classSTR.h"
#include "../util/classHASH.h"
#include "IO_STB.h"
#include "CUserDATA.h"///추가 2004 / 2 /19 : 추가 nAvy
#pragma warning( disable : 4200 )
//-------------------------------------------------------------------------------------------------

#define	QST_VARTYPE_VAR			0x0000
#define	QST_VARTYPE_SWITCH		0x0100
#define	QST_VARTYPE_TIMER		0x0200
#define	QST_VARTYPE_EPISODE		0x0300
#define	QST_VARTYPE_JOB			0x0400
#define	QST_VARTYPE_PLANET		0x0500
#define	QST_VARTYPE_UNION		0x0600

/// \struct STR_QUEST_DATA
/// \brief  퀘스트 데이타의 헤더
///		    STR_COND_001, STR_REWD_002에 필요한 Structure. Quest변수에 대한 체크에 사용됨
struct STR_QUEST_DATA
{
	union {
		int		iType;	///< 0x00000000~0x00000010 ==> QuestVar0 ~ QuestVar9, 
						///< 0x01000000~0x0100001F ==> switch0 ~ switch32
						///< 0x02000000 ==> 퀘스트 타이머를 의미함
						///< 0x03000000 ~ 0x03000004 ==> 에피소드 진행변수 5개
						///< 0x04000000 ~ 0x04000002 ==> 직업진행변수 3개
						///< 0x05000000 ~ 0x05000006 ==> 행성별 진행변수 7개
						///< 0x06000000 ~ 0x06000009 ==> 조합별 진행변수 10개
		struct {
			WORD	m_wVarNO;
			WORD	m_wVarTYPE;
		} ;
	} ;

	// 비교는 :: 얻은값 op nValue
	short	nValue;	///< 비교할 데이터값. 0~255, 스위치일 경우 0, 1 (iType에 해당하는 값 Op btValue)
	BYTE	btOp;	///< iData값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다, 
					///< (이후 액션쪽) 5 = 값바꿈, 6 = 증가(주어진 만큼), 7 = 감소, 8 = 스위치 Off, 9 = 스위치 On
};

/// \struct STR_ABIL_DATA
/// \brief  STR_COND_003, STR_REWD_004 에 필요한 Structure. 능력치 체크에 사용되는 데이터
struct STR_ABIL_DATA
{
	int		iType;	///< enum t_AbilityINDEX와 동일

	int		iValue;	///< 비교할 데이터값 (iType의 능력치 Op iValue)
	BYTE	btOp;	///< iData값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
					///< (액션쪽) 5 = 값바꿈, 6 = 증가, 7 = 감소
};

/// \struct STR_ITEM_DATA
/// \brief STR_COND_004에 필요한 Structure. 한개 Item체크 데이터
struct STR_ITEM_DATA
{
	unsigned int uiItemSN;	///< 아이템 번호 (총5자리 = 앞2자리(아이템분류) + 뒷3자리(아이템번호)
							///< 퀘스트 아이템은 아이템분류가 13
	int		iWhere;			///< 아이템 장착위치 (enum t_EquipINDEX와 동일)
							///< EQUIP_IDX_NULL 이면 인벤토리에 있는지를 체크
	int		iRequestCnt;	///< 체크할 갯수. (장착아이템일 경우는 1) (iTemSN의갯수 Op iRequestCnt)
	BYTE	btOp;			///< iRequestCnt에 대한 OP.  0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
};


//-------------------------------------------------------------------------------------------------

struct StrHeader 
{
	unsigned int	uiSize;
	int				iType;
} ;

/****************************************************************************
 * 현재 퀘스트를 수행중인가
 */
struct STR_COND_000
{
	unsigned int uiSize;	/// 이 스트럭쳐의 크기
	int			 iType;		/// 컨디션 타입

	int			 iQuestSN;	/// 검사할 퀘스트 번호
} ;


/****************************************************************************
 * 퀘스트 데이터 체크
 */
struct STR_COND_001
{
	unsigned int	uiSize;			/// 이 스트럭쳐의 크기
	int				iType;			/// 컨디션 타입

	int				iDataCnt;		/// 뒤에 몇개의 데이터가 있는지
	STR_QUEST_DATA	CheckData[ 1 ]; /// 퀘스터 변수값이나, 스위치에 대해 검사하는 데이터
} ;


/******************************************************************************
 * 퀘스트 진행 변수 체크
 */ 
struct STR_COND_002
{
	unsigned int		uiSize;
	int					iType;

	int					iDataCnt;
	STR_QUEST_DATA		CheckData[ 1 ]; /// 체크할 퀘스트진행 데이터가 여기에 배열로
};


/****************************************************************************
 * 능력치 체크
 */
struct STR_COND_003
{
	unsigned int	uiSize;			/// 이 스트럭쳐의 크기
	int				iType;			/// 컨디션 타입

	int				iDataCnt;
	STR_ABIL_DATA	CheckData[ 1 ];
} ;


/****************************************************************************
 * 소지/장착 아이템 체크
 */
struct STR_COND_004
{
	unsigned int	uiSize;
	int				iType;

	int				iDataCnt;
	STR_ITEM_DATA	CheckData[ 1 ];
} ;


/****************************************************************************
 * 파티 체크
 */
struct STR_COND_005
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btIsLeader;		/// 파티리더인가 ? 0 = 따질필요 없음. 1 = 파티리더인지 체크
	int				iLevel;			/// 파티레벨 얼마이상 (이상만 생각함)
	BYTE			btReversed;		/// 이후를 위해서 그냥 추가. 2004.02.09현재는 무조건 0
};


/****************************************************************************
 * 위치 체크
 */
struct STR_COND_006
{
	unsigned int	uiSize;
	int				iType;

	int				iZoneSN;		/// 존번호
	int				iX;				/// X좌표
	int				iY;				/// Y좌표
	int				iZ;				/// 나중을 위해서. 2004.02.09현재는 무조건 0
	int				iRadius;		/// 반지름. (X,Y)를 기준으로 하여 반지름 얼마이내에..
};


/****************************************************************************
 * 월드 시각 체크
 */
struct STR_COND_007
{
	unsigned int	uiSize;
	int				iType;

	unsigned long	ulTime;			/// 
	unsigned long	ulEndTime;		/// 매일 0시를 기준으로 월드시각이 ulTime이상 ulEndTime이하인가 ?
};


/****************************************************************************
 * 등록퀘스트 수행 잔여시간 체크
 */
struct STR_COND_008
{
	unsigned int	uiSize;
	int				iType;

	unsigned long	ulTime;			/// 체크할 값 (ulTime Op 퀘스트등록시간)
	BYTE			btOp;			/// ulTime에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
};


/****************************************************************************
 * 스킬을 보유하고 있는지 체크
 */
struct STR_COND_009
{
	unsigned int	uiSize;
	int				iType;

	int				iSkillSN1;
	int				iSkillSN2;	/// iSkillSN1 ~ iSkillSN2 번 스킬을 가지고 있는가/없는가 ?
	BYTE			btOp;		/// 0 = 없는지 체크, 1 = 있는지 체크
} ;


/****************************************************************************
 * 확률
 */
struct STR_COND_010
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btLowPcnt;	///
	BYTE			btHighPcnt; /// btLowPcnt <= 확률 AND  확률 <= btHighPcnt
};

/****************************************************************************
 * NPC 변수 값 체크
 */
typedef struct	tagValueQST
{
	unsigned int	uiSize;
	int				Type;

	BYTE			btWho;	// 0:NPC, 1:EventOBJ

	short			nVarNo;	// 변수 번호 : 0 ~ , 존의 경제데이터 변수
	int				iValue;	// 비교할 데이터값 (iType의 능력치 Op iValue)
	BYTE			btOp;	// iValue값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
							// (액션쪽) 5 = 값바꿈, 6 = 증가, 7 = 감소
} STR_COND_011, STR_REWD_011;

/****************************************************************************
 * 이벤트 선택
 */
struct STR_COND_012
{
	unsigned int	uiSize;
	int				iType;

	short			iZone;	/// 04.03.40 추가...

	int				iX;
	int				iY;
	union {
		int			iEventID;
		t_HASHKEY	m_HashEventOBJ;
	} ;
};


/****************************************************************************
 * NPC 선택
 */
struct STR_COND_013
{
	unsigned int	uiSize;
	int				iType;

//	short			nZoneNo;	/// 04.03.30 추가...
	int				iNpcNo; /// 선택할 NPC 번호
};


/****************************************************************************
 * Switch 체크
 */
struct STR_COND_014
{
	unsigned int	uiSize;
	int				iType;

	short			nSN; // 0 ~ 255
	BYTE			btOp; // 0 = Off인지 체크, 1 = On인지 체크
};


/****************************************************************************
 * 파티인원 체크. 
 */
struct STR_COND_015
{
	unsigned int	uiSize;
	int				iType;

	short			nNumber1;
	short			nNumber2; // nNumber1 <= 파티원수 <= nNumber2 이면 true
							  // 비파티체크는 0 <= 파티월수 <= 0 이 true 인것으로 판단
};

/****************************************************************************
 * 존시각 체크
 */
struct STR_COND_016
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btWho;		/// 0=현재 선택된 NPC, 1=조건에서 지정된 Event, 2=사용자

	unsigned long	ulTime;		//
	unsigned long	ulEndTime;	// ulTime <= 현재 존시각 <= ulEndTime 이면 true
};


struct STR_NPCVAR
{
	int				iNpcNo;
	short			nVarNo;
};

/****************************************************************************
 * NPC 변수 비교. (NPC1의 A변수 OP NPC2의 C변수)
 */
struct STR_COND_017
{
	unsigned int	uiSize;
	int				iType;

	STR_NPCVAR		NpcVar1;			
	STR_NPCVAR		NpcVar2;			
	BYTE			btOp; // 동일 .  <NpcVar1.nVarNo의 값> btOp <NpcVar2.nVarNo의 값>
};


/****************************************************************************
 * 날짜 + 시각 체크
 */
struct STR_COND_018
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btDate;		// 날짜. (1 ~ 31)

	BYTE			btHour1;	// 시 (1 ~ 24)
	BYTE			btMin1;		// 분 (1 ~ 60)
	BYTE			btHour2;	// 시
	BYTE			btMin2;		// 분
	//  현재의 날짜가 btDate 이고, btHour1시 btMin1분  <= 현재 시각 <= btHour1시 btMin1분 이면 true
};

/****************************************************************************
 * 요일 + 시각 체크
 */
struct STR_COND_019
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btWeekDay;	// 요일 (0 ~ 6)

	BYTE			btHour1;	// 시
	BYTE			btMin1;		// 분
	BYTE			btHour2;	
	BYTE			btMin2;		
	//  현재의 요일이 btWeekDay 이고, btHour1시 btMin1분  <= 현재 시각 <= btHour1시 btMin1분 이면 true
};

/****************************************************************************
 * (PVP 존)팀번호 체크
 */
struct STR_COND_020
{
	unsigned int	uiSize;
	int				iType;

	int				iNo1;
	int				iNo2; // nNo1 <= 팀번호 <= nNo2 이면 true
};


/****************************************************************************
 * NPC와 거래 체크...
 */
struct STR_COND_021
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btSelObjType;	/// 거리 비교한 선택한 객체...
	int				iRadius;		/// 반지름. (X,Y)를 기준으로 하여 반지름 x 미터내에..
};

/****************************************************************************
 * 채널 서버 체크
 */
struct STR_COND_022
{
	unsigned int	uiSize;
	int				iType;

	unsigned short	nX;
	unsigned short	nY;
};


//
// 클랜 관련
//

// 클랜에 가입 체크 - 추가 데이터 없음
struct STR_COND_023
{
	unsigned int	uiSize;
	int				iType;
	BYTE			btReg;	// 0 = 비가입자냐? 1= 가입자냐?
};

// 클랜 직위 체크
struct STR_COND_024
{
	unsigned int	uiSize;
	int				iType;

	short			nPOS;	// 직위 번호
	BYTE			btOP;	// 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
};

// 클랜 개인 기여도 체크
struct STR_COND_025
{
	unsigned int	uiSize;
	int				iType;

	short			nCONT;	// 기여도
	BYTE			btOP;	// 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
};

// 등급 체크
struct STR_COND_026
{
	unsigned int	uiSize;
	int				iType;

	short			nGRD;	// 클랜 등급
	BYTE			btOP;	// 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
};

// 점수 체크
struct STR_COND_027
{
	unsigned int	uiSize;
	int				iType;

	short			nPOINT;	// 클랜 점수
	BYTE			btOP;	// 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
};

// 머니 체크
struct STR_COND_028
{
	unsigned int	uiSize;
	int				iType;

	int				iMONEY;	// 클랜 머니
	BYTE			btOP;	// 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
};

// 인원 체크
struct STR_COND_029
{
	unsigned int	uiSize;
	int				iType;

	short			nMemberCNT;	// 클랜 인원
	BYTE			btOP;	// 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
};

// 스킬 체크
struct STR_COND_030
{
	unsigned int	uiSize;
	int				iType;

	short			nSkill1;
	short			nSkill2; // nSkill1 ~ nSkill2 의 스킬이 있는지/없는지 체크
	BYTE			btOP;	// 0: 없는가 ?  1: 있는가 ?
};




//-------------------------------------------------------------------------------------------------

// 퀘스트 등록/삭제 요청
struct STR_REWD_000
{
	unsigned int	uiSize;
	int				iType;

	int				iQuestSN;	/// Quest 번호. 삭제시는 현재 퀘스트를 삭제하는 거니까, 필요 없음.
	BYTE			btOp;		/// 0 = 삭제, 1 = 등록, 2 = 변경(데이터 유지), 3 = 변경/최기화 
} ;


// 퀘스트전용 아이템 주기/뺏기 (일반 아이템도 이거 사용해도 됨)
// 뺏기는 Quest Inventory에서. 주는거는 일반, 퀘스트 아이템 모두 가능
struct STR_REWD_001
{
	unsigned int	uiSize;
	int				iType;

	unsigned int	uiItemSN;
	BYTE			btOp;			/// 0 = 빼앗기, 1 = 주기
	short			nDupCNT;		/// 중복 개수
	BYTE			btPartyOpt;		/// 0 = 파티원 적용 안함, 1 = 파티원 적용
} ;


// 퀘스트 변수값/스위치값 변경
struct STR_REWD_002
{
	unsigned int	uiSize;
	int				iType;

	int				iDataCnt;		/// 뒤에 몇개의 데이터가 있는지
	STR_QUEST_DATA	CheckData[ 1 ]; /// 퀘스터 변수값이나, 스위치에 대해 검사하는 데이터
} ;


// 캐릭터 능력치 변경 - (변경)
struct STR_REWD_003
{
	unsigned int	uiSize;
	int				iType;

	int				iDataCnt;		/// 뒤에 몇개의 데이터가 있는지
	STR_ABIL_DATA	CheckData[ 1 ]; /// 변경시킬 능력치
	BYTE			btPartyOpt;		/// 0 = 파티원 적용 안함, 1 = 파티원 적용
} ;


// 캐릭터 진행변수값 변경
struct STR_REWD_004
{
	unsigned int	uiSize;
	int				iType;

	int				iDataCnt;		/// 뒤에 몇개의 데이터가 있는지
	STR_QUEST_DATA	CheckData[ 1 ];
} ;


// 보상
struct STR_REWD_005
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btTarget;	/// 0 = 경험치, 1 = 돈 , 2 = 아이템
	BYTE			btEquation;	/// 사전에 약속된 계산식
	int				iValue;		/// 기준값
	int				iItemSN;	/// btTarget = 2 일 경우 아이템번호는 이거로 참조
	BYTE			btPartyOpt;	/// 0 = 파티원 적용 안함, 1 = 파티원 적용
	// 2004-06-03 이후
	short			nItemOpt;	// 아이템 보상. list_jemitem.stb의 라인번호. (btTarget=2일 경우만 유효)
} ;


// 캐릭터 Hp, Mp 회복
struct STR_REWD_006
{
	unsigned int	uiSize;
	int				iType;

	int				iPercentOfHP;	/// 모두 회복이면 100% (현재는 무조건 100)
	int				iPercentOfMP;	/// 모두 회복이면 100% (현재는 무조건 100)
	BYTE			btPartyOpt;	/// 0 = 파티원 적용 안함, 1 = 파티원 적용
} ;


// 워프
struct STR_REWD_007
{
	unsigned int	uiSize;
	int				iType;

	int				iZoneSN;		/// 존번호
	int				iX;				/// 해당존의 옮길 X좌표
	int				iY;				/// 해당존의 옮길 Y좌표
	BYTE			btPartyOpt;		/// 0 = 파티원 적용 안함, 1 = 파티원 적용
} ;


// 몹소환
struct STR_REWD_008
{
	unsigned int	uiSize;
	int				iType;

	int				iMonsterSN;		/// 소환할 몬스터 번호
	int				iHowMany;		/// 소환할 마리수

	/// 0 = 유저(아바타)주변, 1 = NPC주변, 2 = 이벤트주변, 3 = 해당사항 없음(좌표적용)
	BYTE			btWho;		

	/// btWhere == 2 일 경우 아래 적용
	int				iZoneSN;		/// 존번호
	int				iX;				/// X
	int				iY;				/// Y

	/// 공통적으로 적용
	int				iRange;			/// 미터 이내에
	int				iTeamNo;
} ;


// *** 몇번 체크로 이동 ==> 변수명 변경, shNameLen 추가
struct STR_REWD_009
{
	unsigned int	uiSize;
	int				iType;

	short			shNameLen;
	char			szNextTriggerSN[ 1 ];	/// 현재 패턴에서 iNextTriggerSN(O베이스)번째 이동해서 트리거체크
};

// 스텟 초기화 !!!
struct STR_REWD_010
{
	unsigned int	uiSize;
	int				iType;
};


struct STR_REWD_012
{
	unsigned int	uiSize;
	int				iType;

	BYTE		btMsgType;		/// 0 = 현재필드, 1 = 현재존, 2=전체월드
//	short		nMsgLength;		/// 대사의 길이 NULL포함
//	char		szMsg[ 1 ];		/// 대사
	int			iStrID;
};
struct STR_REWD_012_STR
{
	unsigned int	uiSize;
	int				iType;

	BYTE		btMsgType;		/// 0 = 현재필드, 1 = 현재존, 2=전체월드
	char		szMsg[ 1 ];		/// 대사
};


// 몆초 후에 어떤 트리거 수행
struct STR_REWD_013
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btWho;	// 0:NPC, 1:EventOBJ

	int				iSec;	/// 몇초후에.
	short			nNameLength;
	char			szTriggerName[ 1 ]; /// 트리거명 NULL포함

	t_HASHKEY		m_HashNextTrigger;	/// 로딩시 트리거명을 헤쉬값으로 변경 시킴...
};


// 스킬를 주거나 삭제한다
struct STR_REWD_014
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btOp;		// 0 - 뺏는다 , 1 - 준다
	int				iSkillNo;	// 스킬번호
};


// Switch Off On
//#define		TYPE_REWD_015
struct STR_REWD_015
{
	unsigned int	uiSize;
	int				iType;

	short			nSN;	// 스위치 번호 0 ~ 512
	BYTE			btOp;	// 0 = Off , 1 = On
};

// 그룹별 Switch Clear
//#define		TYPE_REWD_016
struct STR_REWD_016
{
	unsigned int	uiSize;
	int				iType;

	short			nGroupSN;	// 스위치 번호 0 ~ 512 / 32
};

// 전체 Switch Clear
//#define		TYPE_REWD_017
struct STR_REWD_017
{
	unsigned int	uiSize;
	int				iType;

	// 데이터 필요 없음.
};



// NPC 변수 출력
struct STR_REWD_018
{
	unsigned int	uiSize;		// 문자열 길이로 대체...
	int				iType;

	int				iStrID;		// 문자열 시작위치로 대체..
	short			nCnt;
	BYTE			Data[ 1 ];
	/* "xxxx %d xxxx %d xxx "에서 %d가 nCnt만큼 있음. */
};

// 특정 팀번호를 가진 아바타에게 트리거 실행
struct STR_REWD_019
{
	unsigned int	uiSize;
	int				iType;

	short			nZoneNo;

	short			nTeamNo;
	short			nTriggerLength;
	char			TriggerName[ 1 ];	// char Trigger[ nTriggerLength ], NULL 포함

	t_HASHKEY		m_HashTrigger;		/// 로딩시 트리거명을 헤쉬값으로 변경 시킴...
};

// 팀번호 부여 (PVP 존) - 트리거를 발동시킨 분께 팀번호 부여
struct STR_REWD_020
{
	unsigned int	uiSize;
	int				iType;

	unsigned char	btNoType;	// 0:유니크 팀번호 부여, 1:클랜번호로 부여, 2:파티번호로 부여
};

// 현재존의 부활 위치 지정 (PVP 존) 
struct STR_REWD_021
{
	unsigned int	uiSize;
	int				iType;

	int				iX;			// 클라이언트의 작은맵에 보이는 좌표 실제좌표 * 1000된값
	int				iY;
};

// 현재존의 리젠 시스템 토글..
struct STR_REWD_022
{
	unsigned int	uiSize;
	int				iType;

	short			nZonNo;	// 0 : 현재존, 이외: 존번호
	BYTE			btOp;	// 0: Off, 1: On, 2: Toggle
};



//
// 클랜
//
// 클랜 등급 한단계 증가 - 추가 데이터 없음
struct STR_REWD_023
{
	unsigned int	uiSize;
	int				iType;
} ;

// 머니 감소 / 증가 
struct STR_REWD_024
{
	unsigned int	uiSize;
	int				iType;

	int				iMONEY; // 
	BYTE			btOP;	// 5 = 값바꿈, 6 = 증가(주어진 만큼), 7 = 감소(주어진 만큼)
} ;
// 점수 감소/ 증가
struct STR_REWD_025
{
	unsigned int	uiSize;
	int				iType;

	short			nPOINT;
	BYTE			btOP;	// 5 = 값바꿈, 6 = 증가(주어진 만큼), 7 = 감소(주어진 만큼)
} ;

// 스킬 습득 / 삭제
struct STR_REWD_026
{
	unsigned int	uiSize;
	int				iType;

	short			nSkillNo; // 스킬번호
	BYTE			btOP;	// 0: 삭제, 1: 습득
} ;

// 기여도 감소 / 증가
struct STR_REWD_027
{
	unsigned int	uiSize;
	int				iType;

	short			nCONT; // 기여도
	BYTE			btOP;	// 5 = 값바꿈, 6 = 증가(주어진 만큼), 7 = 감소(주어진 만큼)
} ;


// 클랜원 워프
struct STR_REWD_028
{
	unsigned int	uiSize;
	int				iType;

	int				iRange; // 주변 몇 미터
	short			nZoneNo; // 타겟 존번호
	int				iX;		// 타겟 x좌표
	int				iY;		// 타겟 y좌표
} ;


// 스크립트 실행. ( 클라이언트에만 적용 )
struct STR_REWD_029
{
	unsigned int	uiSize;
	int				iType;

	short			nLength; // 스크립트명의 길이 ( NULL 포함 )
	char			szSrciptName[ 1 ]; // 스크립트명
} ;

// 스킬 초기화
struct STR_REWD_030
{
	unsigned int	uiSize;
	int				iType;
};

// 몬스터 사냥 변수 증가
struct STR_REWD_031
{
	unsigned int	uiSize;
	int				iType;

	int				iMonsterSN;		/// 소환할 몬스터 번호
	int				iCompareValue;	// 비교 수치
	STR_QUEST_DATA	Var;			// 변수 정보
};



// 퀘스트 아이템 획득 정보
struct STR_REWD_032
{
	unsigned int	uiSize;
	int				iType;

	unsigned int	uiItemSN;
	int				iCompareValue;
	BYTE			btPartyOpt;		// 0 = 파티 적용 안 됨, 파티 적용됨
};


// 선택보상 트리거 정보
struct STR_REWD_033
{
	unsigned int	uiSize;
	int				iType;

	short			nNextRewardSplitter;	// -1 이면 보상트리거의 끝임
};

// NPC 숨기기/보이기/토글하기
struct STR_REWD_034
{
	unsigned int	uiSize;
	int				iType;

	BYTE			btHIDE; // 0 = 숨기기 , 1 = 보이기, 2 = 토글하기
};

//-------------------------------------------------------------------------------------------------
union uniQstENTITY {
	struct {
		unsigned int	uiSize;
		int				iType;
	} ;
	BYTE				m_pDATA[0];

	STR_COND_000		m_Cond000;
	STR_COND_001		m_Cond001;
	STR_COND_002		m_Cond002; 
	STR_COND_003		m_Cond003; 
	STR_COND_004		m_Cond004; 
	STR_COND_005		m_Cond005; 
	STR_COND_006		m_Cond006; 
	STR_COND_007		m_Cond007; 
	STR_COND_008		m_Cond008; 
	STR_COND_009		m_Cond009;
	STR_COND_010		m_Cond010;
	STR_COND_011		m_Cond011;
	STR_COND_012		m_Cond012;
	STR_COND_013		m_Cond013;
	STR_COND_014		m_Cond014;
	STR_COND_015		m_Cond015;
	STR_COND_016		m_Cond016;
	STR_COND_017		m_Cond017;
	STR_COND_018		m_Cond018;
	STR_COND_019		m_Cond019;
	STR_COND_020		m_Cond020;
	STR_COND_021		m_Cond021;
	STR_COND_022		m_Cond022;
	STR_COND_023		m_Cond023;
	STR_COND_024		m_Cond024;
	STR_COND_025		m_Cond025;
	STR_COND_026		m_Cond026;
	STR_COND_027		m_Cond027;
	STR_COND_028		m_Cond028;
	STR_COND_029		m_Cond029;
	STR_COND_030		m_Cond030;

	STR_REWD_000		m_Rewd000;
	STR_REWD_001		m_Rewd001;
	STR_REWD_002		m_Rewd002; 
	STR_REWD_003		m_Rewd003; 
	STR_REWD_004		m_Rewd004; 
	STR_REWD_005		m_Rewd005; 
	STR_REWD_006		m_Rewd006; 
	STR_REWD_007		m_Rewd007; 
	STR_REWD_008		m_Rewd008; 
	STR_REWD_009		m_Rewd009;
	STR_REWD_010		m_Rewd010;
	STR_REWD_011		m_Rewd011;
	STR_REWD_012		m_Rewd012;
	STR_REWD_012_STR	m_Rewd012_STR;
	STR_REWD_013		m_Rewd013;
	STR_REWD_014		m_Rewd014;

	STR_REWD_015		m_Rewd015;
	STR_REWD_016		m_Rewd016;
	STR_REWD_017		m_Rewd017;

	STR_REWD_018		m_Rewd018;
	STR_REWD_019		m_Rewd019;

	STR_REWD_020		m_Rewd020;
	STR_REWD_021		m_Rewd021;
	STR_REWD_022		m_Rewd022;

	STR_REWD_023		m_Rewd023;
	STR_REWD_024		m_Rewd024;
	STR_REWD_025		m_Rewd025;
	STR_REWD_026		m_Rewd026;
	STR_REWD_027		m_Rewd027;
	STR_REWD_028		m_Rewd028;

	STR_REWD_031		m_Rewd031;
	STR_REWD_032		m_Rewd032;
	STR_REWD_033		m_Rewd033;

	STR_REWD_034		m_Rewd034;
} ;



//-------------------------------------------------------------------------------------------------
/*
enum eQE_CODE {		// Quest error code
	QSTERR_QUEST_OK = 0x0000,
	QSTERR_QUEST_NOT_REGISTERED,	// 현재 수행중인 퀘스트가 아니다
} ;
*/
class CUserDATA;
class CQUEST;
class CQuestTRIGGER;
class CObjNPC;
class CObjEVENT;

#ifdef	__SERVER
class classUSER;
#endif

struct tQST_PARAM {
#ifndef	__SERVER
	CQuestTRIGGER*m_pCurrentTRIGGER;
	CUserDATA	*m_pOWNER;				///< 퀘스트 사용자
#else
	CObjVAR		*m_pEventVAR;			///< 현재 선택된 Event Object
	CObjVAR		*m_pNpcVAR;				///< 현재 선택된 NPC
	classUSER	*m_pOWNER;				///< 퀘스트 사용자
	CGameOBJ	*m_pCallOBJ;
#endif

	short		 m_nCurZoneNO;			///< 퀘스트 발생자가 있는 존 번호

	BYTE		 m_btQuestSLOT;
	CQUEST		*m_pQUEST;				///< 현재 선택된 퀘스트 
	t_HASHKEY	 m_HashNextTRIGGER;		///< 조건이 만족할 경우 추가 검색할 퀘스트 트리거

	bool		 m_bServerFUNC;	
	short		 m_nErrSTEP;

	void Init (CUserDATA *pOWNER, short nZoneNO=0)
	{
		m_btQuestSLOT = QUEST_PER_PLAYER;
		m_pQUEST = NULL;
		m_HashNextTRIGGER = 0;
		m_nErrSTEP	= -1;
		m_bServerFUNC = false;
		m_nCurZoneNO = nZoneNO;
#ifdef	__SERVER
		m_pNpcVAR	= NULL;
		m_pEventVAR = NULL;
		m_pOWNER = (classUSER*)pOWNER;
#else
		m_pOWNER = pOWNER;
#endif
	}
} ;

//-------------------------------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class CQuestTRIGGER
 * \brief 개별 퀘스트 트리거 데이타및 처리
 */
class CQuestTRIGGER {
private:
	BYTE			m_btCheckNEXT;
	unsigned int	m_uiCondCNT;
	unsigned int	m_uiRewdCNT;

	uniQstENTITY **m_ppCondition;
	uniQstENTITY **m_ppReward;

	void Init_COND( uniQstENTITY *pCOND );
	void Init_REWD( uniQstENTITY *pREWD );

public :
	CStrVAR			m_Name;
	CQuestTRIGGER  *m_pNextTrigger;

	CQuestTRIGGER ()	{	m_pNextTrigger = NULL;	m_iOwerNpcIDX = 0;		}
	~CQuestTRIGGER ()	{	this->Free();									}

	BYTE GetCheckNext()	{	return m_btCheckNEXT;							}

#ifdef	__SERVER
	int	 m_iOwerNpcIDX;
	bool Load (FILE *fpIN, STBDATA *pSTB, int iLangCol);
#else
	bool Client_Load( CFileSystem* pFileSystem );
#endif
	void Free ();

	bool Proc (tQST_PARAM *pPARAM, bool bDoReward, short nSelectReward);
} ;


//-------------------------------------------------------------------------------------------------
// QUEST 
#define QUEST_NAME(I)				g_QuestList.m_STB.m_ppNAME[ I ]
#define QUEST_DESC(I)				g_QuestList.m_STB.m_ppDESC[ I ]

#define	QUEST_TIME_LIMIT(I)			g_QuestList.m_STB.m_ppDATA[ I ][ 1 ]
#define	QUEST_OWNER_TYPE(I)			g_QuestList.m_STB.m_ppDATA[ I ][ 2 ]

enum eQST_RESULT {
	QST_RESULT_INVALID = 0,	// 서버에서 짤린다.
	QST_RESULT_SUCCESS,
	QST_RESULT_FAILED,		// 서버에서 실패 통보 된다.
	QST_RESULT_STOPPED,		// 서버 점검중이다.
} ;

/**
 * \ingroup SHO_GS_LIB
 * \class	CQuestDATA
 * \author	wookSang.Jo
 * \brief	전체 퀘스트 트리거 데이타 보관및 처리하는 클래스
 */
class CQuestDATA {
private:
	classHASH< CQuestTRIGGER* >	m_HashQUEST;
	CStrVAR		m_QuestListSTB;
	CStrVAR		m_QuestFILE;
	CStrVAR		m_BaseDIR;
	
	CStrVAR		m_QuestLangSTB;
	int			m_iLangCol;
	STBDATA	   *m_pSTB;

	bool		m_bEnable;
	long		m_lRefCnt;

	#ifdef	__SERVER
		void	LoadQuestTrigger (FILE *fpIN, unsigned int uiTriggerCNT, char *szFileName);
		bool	LoadDATA (char *szFileName);
	#else
		void	LoadQuestTrigger (CFileSystem* pFileSystem, unsigned int uiTriggerCNT, char *szFileName);
		bool	Client_LoadDATA (char *szFileName);
	#endif

public :
	STBDATA		m_STB;

	bool LoadQuestTable ();
	bool LoadQuestTable (char *szQuestFile, char *szQuestListFile, char *szBaseDIR=NULL, char *szQustLangFile=NULL, int iLangCol=0);
	void Free ();
	CQuestDATA ()		{	m_bEnable=false, m_lRefCnt=0;	}
	~CQuestDATA ()		{	this->Free();		}

	eQST_RESULT CheckQUEST( CUserDATA *pUSER, t_HASHKEY HashQuest, bool bDoReward=false, int iEventNpcIDX=0, CGameOBJ *pCallOBJ=NULL, short nSelectReward=-1 );
	CQuestTRIGGER *GetQuest( t_HASHKEY HashQuest ) 
	{
		tagHASH< CQuestTRIGGER* > *pHashNode;

		pHashNode = m_HashQUEST.Search( HashQuest );
		return ( pHashNode ) ? pHashNode->m_DATA : NULL;
	}

	void CheckAllQuest( CUserDATA *pUSER );

	bool IsEnable ()	
	{
		return m_bEnable;
	}
	#ifdef	__SERVER
	void ToggleEnable ()
	{
		m_bEnable = !m_bEnable;
	}
	long GetRefCNT ()	{	return m_lRefCnt;	}
	#endif
} ;
extern CQuestDATA g_QuestList;


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#pragma warning( default : 4200 )
#endif