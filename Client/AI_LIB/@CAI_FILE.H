/*
	$Header: /Client/AI_LIB/CAI_FILE.H 2     03-09-26 1:30p Icarus $
*/
#ifndef	__CAI_FILE_H
#define	__CAI_FILE_H
#include "CAI_LIB.h"
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
struct		AICOND02 
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 체크거리
	BYTE			cChrType ;					// 캐릭터 타입 (아바타,몬스터)
	char			cLevelDiff ;				// 레벨차 (-128~127 ) 
	WORD			wChrNum ;					// 캐릭터 숫자
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
	char			cLevelDiff ;				// 레벨차 (-128~127 ) 자신의 레벨보다 cLevelDiff이상인
	BYTE			cChrType ;					// 캐릭터 타입 (아바타=0,몬스터=1)
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
	BYTE			cSpeed ;					// 천천히 , 보통 , 빠르게
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
	BYTE			cSpeed ;					// 천천히 , 보통 , 빠르게
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
	BYTE			cSpeed ;					// 천천히 , 보통 , 빠르게
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
	BYTE			cSpeed ;					// 천천히 , 보통 , 빠르게
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
	} ;
} ;

//-------------------------------------------------------------------------------------------------
class CAI_EVENT
{
private:
	AI_CONDITION  **m_ppConditionLIST;
	int				m_iConditionCNT;

	AI_ACTION	  **m_ppActionLIST;
	int				m_iActionCNT;

public :
	CAI_OBJ	*m_pSourCHAR;
	CAI_OBJ	*m_pDestCHAR;
	CAI_OBJ	*m_pFindCHAR;
	CAI_OBJ	*m_pNearCHAR;

	int			 m_iDamage;

	CAI_EVENT ();
	~CAI_EVENT ();

	bool Load (FILE *fp);
	bool Check (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage);
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

	bool Load (FILE *fp);
	void Check (CAI_OBJ *pSourCHAR, CAI_OBJ *pDestCHAR, int iDamage);
} ;


//-------------------------------------------------------------------------------------------------
struct tagConditionFUNC {
	bool (*m_fpCondition) (stCondHead *pConDATA, CAI_EVENT *pEVENT);
} ;
extern tagConditionFUNC g_FuncCOND[];

struct tagActionFUNC {
	void (*m_fpAction) (stActHead *pActDATA, CAI_EVENT *pEVENT);
} ;
extern tagActionFUNC	g_FuncACTION[];


//-------------------------------------------------------------------------------------------------

int   AI_Get_Ability (CAI_OBJ *pCHAR, int iAbTYPE);
float AI_Get_SPEED   (CAI_OBJ *pCHAR, BYTE btSpeed);

//-------------------------------------------------------------------------------------------------
#endif
