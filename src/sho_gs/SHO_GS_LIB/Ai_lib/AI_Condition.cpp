#include "stdAFX.h"
#include "CAI_FILE.h"
#include "CAI_LIB.h"

#ifdef	__SERVER
	extern short Get_ServerChannelNO ();
#endif

//-------------------------------------------------------------------------------------------------
bool F_AICOND_00 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
	// 사용안함.
	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_01 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND01
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDamage;					// 데미지량
	BYTE			cRecvOrGive ;				// 0=받다 , 1=주다
} ;
*/
	AICOND01 *pCond = (AICOND01*)pConDATA;

	if ( pCond->cRecvOrGive )
		return false;

	if ( pAIParam->m_iDamage >= pCond->iDamage )
		return true;

	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_02 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return false;
#endif

	AICOND02 *pCond = (AICOND02*)pConDATA;

	bool bAllied = ( 0 != pCond->btIsAllied );
	// int CharTYPE = ( pCond->cChrType ) ? OBJ_MOB : OBJ_AVATAR;

//	short nMyTeamNO = pAIParam->m_pSourCHAR->Get_TeamNO();
	CAI_OBJ *pFindCHAR = pAIParam->m_pSourCHAR->AI_FindFirstOBJ( pCond->iDistance );
	int iFindCNT = 0, iNearDist=999999999, iCurDist;

	while( pFindCHAR ) {
		if ( ( pAIParam->m_pSourCHAR!=pFindCHAR ) && (bAllied==pAIParam->m_pSourCHAR->Is_ALLIED( pFindCHAR ) ) ) {
			if ( pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() >= pCond->nLevelDiff &&
				 pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() <= pCond->nLevelDiff2 ) {
				iFindCNT ++;

				iCurDist = (int)( pAIParam->m_pSourCHAR->Get_DISTANCE( pFindCHAR ) );
				if ( iCurDist < iNearDist ) {
					iNearDist = iCurDist;
					// 가장 가까운 넘...
					pAIParam->m_pNearCHAR = pFindCHAR;
				}

				if ( iFindCNT >= pCond->wChrNum ) {
					// 가장 마지막에 찾은넘
					pAIParam->m_pFindCHAR = pFindCHAR;
					return true;
				}
			}
		}
		pFindCHAR = pAIParam->m_pSourCHAR->AI_FindNextOBJ ();
	} 
	
	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_03 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return false;
#endif
/*
struct		AICOND03
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
} ;
*/
	AICOND03 *pCond = (AICOND03*)pConDATA;
	int iDistance = (int)( pAIParam->m_pSourCHAR->Get_MoveDISTANCE () );
	if ( iDistance >= pCond->iDistance )
		return true;

	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_04 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND04
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cMoreLess ;					// 이상 이하
} ;
*/
	AICOND04 *pCond = (AICOND04*)pConDATA;
	CAI_OBJ *pTarget = pAIParam->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return false;

	int iDistance = (int)( pAIParam->m_pSourCHAR->Get_DISTANCE( pTarget ) );
	if ( pCond->cMoreLess ) {
		// 이하
		if ( iDistance <= pCond->iDistance )
			return true;
	} else {
		// 이상
		if ( iDistance >= pCond->iDistance )
			return true;
	}

	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_05 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND05
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				cAbType ;					// 능력치 타입
	int				iDiff ;
	BYTE			cMoreLess ;					// 큰가 , 작은가
} ;
	cAbType 
		0 레벨
		1 공격력
		2 방어력
		3 항마력
		4 HP
		5 매력
*/
	AICOND05 *pCond = (AICOND05*)pConDATA;

	// 타겟의...
	CAI_OBJ *pTarget = pAIParam->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return false;

	int iValue = AI_Get_Ability ( pTarget, pCond->cAbType );
	if ( pCond->cMoreLess ) {
		// 이하
		if ( iValue <= pCond->iDiff )
			return true;
	} else {
		// 이상
		if ( iValue >= pCond->iDiff )
			return true;
	}

	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_06 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND06
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	DWORD			wHP ;						// HP 수치
	BYTE			cMoreLess ;					// 큰가 , 작은가
} ;
*/
	AICOND06 *pCond = (AICOND06*)pConDATA;

	if ( pCond->cMoreLess ) {
		// 이하
		if ( pAIParam->m_pSourCHAR->Get_PercentHP() <= (int)pCond->wHP )
			return true;
	} else {
		// 이상
		if ( pAIParam->m_pSourCHAR->Get_PercentHP() >= (int)pCond->wHP )
			return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_07 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
struct		AICOND07
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	BYTE			cPercent ;					// 확률
} ;
*/
	AICOND07 *pCond = (AICOND07*)pConDATA;

	if ( pAIParam->m_pSourCHAR->Get_RANDOM( 100 ) < pCond->cPercent )
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------
bool F_AICOND_08 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return false;
#endif

/*
// 주변 distance내에 자신과 레벨차가 있는 아바타/몬스터가 있으면
// 타겟으로 지정.
struct		AICOND08
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	char			cLevelDiff ;				// 레벨차 (-128~127 ) 자신의 레벨보다 cLevelDiff이상인
	BYTE			cChrType ;					// 캐릭터 타입 (아바타=0,몬스터=1)
} ;
*/
	AICOND08 *pCond = (AICOND08*)pConDATA;

	bool bAllied = ( 0 != pCond->btIsAllied );

	CAI_OBJ *pFindCHAR = pAIParam->m_pSourCHAR->AI_FindFirstOBJ( pCond->iDistance );
//	short nMyTeamNO = pAIParam->m_pSourCHAR->Get_TeamNO();
	while ( pFindCHAR ) {
		if ( ( pAIParam->m_pSourCHAR!=pFindCHAR ) && (bAllied==pAIParam->m_pSourCHAR->Is_ALLIED( pFindCHAR ) ) ) {
			if ( pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() >= pCond->nLevelDiff &&
				 pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() <= pCond->nLevelDiff2 ) {
				pAIParam->m_pFindCHAR = pFindCHAR;
				pAIParam->m_pNearCHAR = pFindCHAR;
				return true;
			}
		}
		pFindCHAR = pAIParam->m_pSourCHAR->AI_FindNextOBJ ();
	} 

	return false;
}

//-------------------------------------------------------------------------------------------------
bool F_AICOND_09 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
// 자신을 공격한 케릭터와 공격 타겟이 다른 케릭터 인가?
struct		AICOND09
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
} ;
*/
	AICOND09 *pCond = (AICOND09*)pConDATA;
	if ( pAIParam->m_pSourCHAR->Get_TARGET() != pAIParam->m_pDestCHAR )
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------
bool F_AICOND_10 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
// 자신을 공격한 케릭터의 능력치가 공격 타겟 케릭터보다 큰가/작은가?
struct		AICOND10
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	BYTE			cAbType ;					// 능력치 타입
	BYTE			cMoreLess ;					// 큰=0 , 작은=1
} ;
*/
	AICOND10 *pCond = (AICOND10*)pConDATA;

	// 반드시 타겟이 존재해야 한다.
	if ( NULL == pAIParam->m_pSourCHAR->Get_TARGET()  || 
		 NULL == pAIParam->m_pDestCHAR )
		return false;

	int iV1, iV2;
	iV1 = AI_Get_Ability (pAIParam->m_pDestCHAR, pCond->cAbType );
	iV2 = AI_Get_Ability (pAIParam->m_pSourCHAR->Get_TARGET(), pCond->cAbType );
	if ( pCond->cMoreLess ) {
		// 작은
		if ( iV1 < iV2 )
			return true;
	} else {
		// 큰
		if ( iV1 > iV2 )
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool F_AICOND_11 (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
/*
 * Explain : 자신을 공격한 캐릭터의 A능력치가 B값보다 크거나/작은 경우
struct		AICOND11
{
	DWORD			dwSize ;		// 이 스트럭쳐의 크기
	AITYPE			Type ;			// 조건 타입 = AICOND_11
	BYTE			cAbType ;		// 능력치 (레벨=0, 공격력=1, 방어력=2, 항마력=3, HP=4, 매력=5)
	int				iValue;			// 비교값. 이값보다 큰가/작은가. 이런식으로
	BYTE			cMoreLess ;		// 큰=0 , 작은=1
} ;
*/
	AICOND11 *pCond = (AICOND11*)pConDATA;

	// 반드시 타겟이 없어야 한다.
	if ( pAIParam->m_pSourCHAR->Get_TARGET() )
		return false;

	if ( NULL == pAIParam->m_pDestCHAR )
		return false;

	int iValue = AI_Get_Ability (pAIParam->m_pDestCHAR, pCond->cAbType );

	if ( pCond->cMoreLess ) {
		// 이하
		if ( iValue <= pCond->iValue )
			return true;
	} else {
		// 이상
		if ( iValue >= pCond->iValue )
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool F_AICOND_12 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
/// 현재 존이 지금 낮인가 / 밤인가 ?
struct		AICOND12
{
	DWORD			dwSize ;		// 이 스트럭쳐의 크기
	AITYPE			Type ;			// 조건 타입 = AICOND_12
	BYTE			cWhen;			// 0 = 낮인지 체크, 1 = 밤인지 체크
};
*/
	int iZoneTIME = pAIParam->m_pSourCHAR->Get_ZoneTIME();

	return ( ((AICOND12*)pConDATA)->cWhen != pAIParam->m_pSourCHAR->Is_DAY() );
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_13 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
/// 자신/타겟에게 걸려있는 상태 체크. 
struct		AICOND13
{
	DWORD			dwSize;
	AITYPE			Type;

	BYTE			btCheckTarget;	// 0 = 자기자신, 1 = 공격타겟
	BYTE			btStatusType;	// 0 = 해로운, 1 = 유리한, 2 = 해로운/유리한 아무거나
	BYTE			btHave;			// 0 = 없는가 ? 1 = 있는가 ?
};
*/
#ifndef	__SERVER
	return false;
#else
	AICOND13 *pCond = (AICOND13*)pConDATA;

	CAI_OBJ *pTarget;
	if ( pCond->btCheckTarget ) {
		// 공격 타켓 ...
		pTarget = pAIParam->m_pSourCHAR->Get_TARGET ();
		if ( NULL == pTarget )
			return false;
	} else {
		// 자신..
		pTarget = pAIParam->m_pSourCHAR;
	}

	DWORD dwStatus;
	switch( pCond->btStatusType ) {
		case 0 :	// 해로운
			dwStatus = FLAG_ING_BAD  & pTarget->Get_MagicSTATUS();
			break;
		case 1 :	// 유리한
			dwStatus = FLAG_ING_GOOD & pTarget->Get_MagicSTATUS();
			break;
		case 2 :	// 암거나..
			dwStatus = pTarget->Get_MagicSTATUS();
			break;
	}

	if ( dwStatus ) {
		// 상태있다..
		return ( pCond->btHave ) ? true : false;
	}

	// 상태없다.
	return ( 0 == pCond->btHave ) ? true : false;
#endif
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_14 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
/*
// NPC 변수 체크	- AICOND14
typedef struct	tagValue
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// 변수 번호 : 0 ~ , 존의 경제데이터 변수
	int				iValue;	// 비교할 데이터값 (iType의 능력치 Op iValue)
	BYTE			btOp;	// iValue값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
							// (액션쪽) 5 = 값바꿈, 6 = 증가, 7 = 감소
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27;
*/
#ifndef	__SERVER
	return true;
#else
	if ( NULL == pAIParam->m_pLocalNPC )
		return false;

	tagValueAI *pVALUE = (tagValueAI*)pConDATA;

	int iValue = pAIParam->m_pLocalNPC->Get_ObjVAR( (BYTE)pVALUE->nVarNo );
	return ::Check_AiOP( pVALUE->btOp, iValue, pVALUE->iValue );
#endif
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_15 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
// 월드변수 체크	- AICOND15
typedef struct	tagValue
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// 변수 번호 : 0 ~ , 존의 경제데이터 변수
	int				iValue;	// 비교할 데이터값 (iType의 능력치 Op iValue)
	BYTE			btOp;	// iValue값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
							// (액션쪽) 5 = 값바꿈, 6 = 증가, 7 = 감소
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27;
*/
#ifndef	__SERVER
	return false;
#else
	tagValueAI *pVALUE = (tagValueAI*)pConDATA;

	int iValue = pAIParam->m_pSourCHAR->Get_WorldVAR( pVALUE->nVarNo );
	return ::Check_AiOP( pVALUE->btOp, iValue, pVALUE->iValue );
#endif
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_16 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
// 경제 데이터 체크 - AICOND16
typedef struct	tagValue
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// 변수 번호 : 0 ~ , 존의 경제데이터 변수
	int				iValue;	// 비교할 데이터값 (iType의 능력치 Op iValue)
	BYTE			btOp;	// iValue값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.
							// (액션쪽) 5 = 값바꿈, 6 = 증가, 7 = 감소
} AICOND14, AICOND15, AICOND16, AIACT25, AIACT26, AIACT27;
*/
#ifndef	__SERVER
	return false;
#else
	tagValueAI *pVALUE = (tagValueAI*)pConDATA;

	int iValue = pAIParam->m_pSourCHAR->Get_EconomyVAR( pVALUE->nVarNo );
	return ::Check_AiOP( pVALUE->btOp, iValue, pVALUE->iValue );
#endif
}
//-------------------------------------------------------------------------------------------------
bool F_AICOND_17 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{	
/*
/// NPC 선택
struct AICOND17
{
	DWORD			dwSize;
	AITYPE			Type;

	int				iNpcNo; /// 선택할 NPC 번호
};
*/
#ifndef	__SERVER
	return false;
#else
	AICOND17 *pCond = (AICOND17*)pConDATA;

	pAIParam->m_pLocalNPC = (CObjNPC*)pAIParam->m_pSourCHAR->Find_LocalNPC( pCond->iNpcNo );

	return ( NULL != pAIParam->m_pLocalNPC );
#endif
}

bool F_AICOND_18 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
#ifndef	__SERVER
	return false;
#else
	CAI_OBJ *pOwner = pAIParam->m_pSourCHAR->Get_CALLER ();
	if ( NULL == pOwner ) 
		return false;

	AICOND18 *pCond = (AICOND18*)pConDATA;
	int iDistance = static_cast<int>( pAIParam->m_pSourCHAR->Get_DISTANCE( pOwner ) );

	return ::Check_AiOP( pCond->btOp, iDistance, pCond->iDistance );
#endif
}

bool F_AICOND_19 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
#ifndef	__SERVER
	return false;
#else
	AICOND19 *pCond = (AICOND19*)pConDATA;

	if ( (UINT)pAIParam->m_pSourCHAR->Get_ZoneTIME() >= pCond->ulTime &&
		 (UINT)pAIParam->m_pSourCHAR->Get_ZoneTIME() <= pCond->ulEndTime )
		return true;

	return false;
#endif
}


bool F_AICOND_20 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
	AICOND20 *pCond = (AICOND20*)pConDATA;
/*
// 자신(몬스터)의 능력치/기타/주인캐릭터 관련 데이터 체크
struct AICOND20
{
	DWORD	dwSize;
	AITYPE	Type;

	BYTE	btAbType ;		// 능력치 (레벨=0, 공격력=1, 방어력=2, 항마력=3, HP=4, 매력=5)

	int		iValue;	// 비교할 데이터값 (iType의 능력치 Op iValue)
	BYTE	btOp;	// iData값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.					
};
*/
#ifndef	__SERVER
	return false;
#else
	int iValue;

	iValue = AI_Get_Ability (pAIParam->m_pSourCHAR, pCond->btAbType );
	return ::Check_AiOP( pCond->btOp, iValue, pCond->iValue );
#endif
}

bool F_AICOND_21 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
#ifndef	__SERVER
	return false;
#else
	AICOND21 *pCond = (AICOND21*)pConDATA;
	/// 주인캐릭터가 존재 유무 확인

	return ( NULL == pAIParam->m_pSourCHAR->Get_CALLER() );
#endif
}

bool F_AICOND_22 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
#ifndef	__SERVER
	return false;
#else
	AICOND22 *pCond = (AICOND22*)pConDATA;
	/// 주인캐릭터가 현재 공격타겟을 가지고 있는가 ?

	CAI_OBJ *pOwner = pAIParam->m_pSourCHAR->Get_CALLER ();
	if ( !pOwner )
		return false;
	
	return ( NULL != pOwner->Get_TARGET() );
#endif
}

bool F_AICOND_23 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
	/// 월드시각 체크
	AICOND23 *pCond = (AICOND23*)pConDATA;

	if ( (UINT)pAIParam->m_pSourCHAR->Get_WorldTIME() >= pCond->ulTime &&
		 (UINT)pAIParam->m_pSourCHAR->Get_WorldTIME() <= pCond->ulEndTime )
		return true;

	return false;
}

bool F_AICOND_24 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
/*
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
*/
#ifdef	__SERVER
	AICOND24 *pCond = (AICOND24*)pConDATA;
	SYSTEMTIME sTIME;

	::GetLocalTime( &sTIME );
	if ( pCond->btDate && sTIME.wDay != pCond->btDate )
		return false;
	
	WORD wMin = sTIME.wHour * 60 + sTIME.wMinute;
	WORD wFrom, wTo;

	wFrom = pCond->btHour1 * 60 + pCond->btMin1;
	wTo   = pCond->btHour2 * 60 + pCond->btMin2;
	if ( wMin >= wFrom && wMin <= wTo )
		return true;
#endif

	return false;
}

bool F_AICOND_25 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
/*
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
*/
#ifdef	__SERVER
	AICOND25 *pCond = (AICOND25*)pConDATA;
	SYSTEMTIME sTIME;

	::GetLocalTime( &sTIME );
	if ( sTIME.wDayOfWeek != pCond->btWeekDay )
		return false;

	WORD wMin = sTIME.wHour * 60 + sTIME.wMinute;
	WORD wFrom, wTo;

	wFrom = pCond->btHour1 * 60 + pCond->btMin1;
	wTo   = pCond->btHour2 * 60 + pCond->btMin2;
	if ( wMin >= wFrom && wMin <= wTo )
		return true;
#endif
	return false;
}

bool F_AICOND_26 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
	AICOND26 *pCond = (AICOND26*)pConDATA;
#ifdef	__SERVER
	short nChannelNo = ::Get_ServerChannelNO ();
	return ( pCond->nX >= nChannelNo && pCond->nY <= nChannelNo );
#else
	return false;
#endif
}

bool F_AICOND_27 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
/*
 * Struct Name : AICOND_27
 * 주변캐릭터 상황 체크 
 * Explain : 체크범위 iDistance이내에 캐릭터타입 cChrType이고 레벨차가 cLevelDiff이상이고 캐릭터수가 wChrNum이상/이하인 경우
 truct		AICOND27			// 2번 조건하고 비슷...
{
	DWORD		dwSize ;		
	AITYPE		Type ;			
	int			iDistance ;		// 체크거리
	BYTE		cChrType ;		// 캐릭터 타입 (아바타=0,몬스터=1)
	short		nLevelDiff ;	// 
	short		nLevelDiff2 ;	// nLevelDiff <= XXXX <= nLevelDiff2

	WORD		wChrNum ;		// 캐릭터 숫자
	BYTE		btOp;			// 캐릭터 숫자에 대한 OP (0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.)
} ;
*/
#ifdef	__SERVER
	AICOND27 *pCond = (AICOND27*)pConDATA;

	bool bAllied = ( 0 != pCond->btIsAllied );

	CAI_OBJ *pFindCHAR = pAIParam->m_pSourCHAR->AI_FindFirstOBJ( pCond->iDistance );
	int iFindCNT = 0, iNearDist=999999999, iCurDist;

	while( pFindCHAR ) {
		if ( ( pAIParam->m_pSourCHAR!=pFindCHAR ) && (bAllied==pAIParam->m_pSourCHAR->Is_ALLIED( pFindCHAR ) ) ) {
			if ( pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() >= pCond->nLevelDiff &&
				 pAIParam->m_pSourCHAR->Get_LEVEL() - pFindCHAR->Get_LEVEL() <= pCond->nLevelDiff2 ) {
				iFindCNT ++;

				iCurDist = (int)( pAIParam->m_pSourCHAR->Get_DISTANCE( pFindCHAR ) );
				if ( iCurDist < iNearDist ) {
					iNearDist = iCurDist;
					// 가장 가까운 넘...
					pAIParam->m_pNearCHAR = pFindCHAR;
				}

				//if ( iFindCNT >= pCond->wChrNum ) {
					// 가장 마지막에 찾은넘
				//	pAIParam->m_pFindCHAR = pFindCHAR;
				//	return true;
				//}
			}
		}
		pFindCHAR = pAIParam->m_pSourCHAR->AI_FindNextOBJ ();
	} 
	pAIParam->m_pFindCHAR = pAIParam->m_pNearCHAR;

	return ::Check_AiOP( pCond->btOp, iFindCNT, pCond->wChrNum );
#endif
	return false;
}

bool F_AICOND_28 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
/*
// 몬스터 변수 체크
struct	AICOND28
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// 변수 번호 : 0 ~ 4
	int				iValue;	// 비교할 데이터값
	BYTE			btOp;	// iValue값에 대한 비교방법. 0 = 같다, 1 = 크다, 2 = 크거나 같다. 3=작다, 4=작거나 같다.							
} ;
*/
#ifdef	__SERVER
	tagValueAI *pVALUE = (tagValueAI*)pConDATA;

	int iValue = pAIParam->m_pSourCHAR->Get_AiVAR( (BYTE)pVALUE->nVarNo );
	return ::Check_AiOP( pVALUE->btOp, iValue, pVALUE->iValue );
#endif
}


// 공격자 / 공격타겟이 클랜마스터인가 ?
bool F_AICOND_29 (stCondHead *pConDATA, t_AIPARAM *pAIParam)	
{
	AICOND29 *pCond = (AICOND29*)pConDATA;

	switch( pCond->btTargetType ) {
		case 0 :	// 0 : 공격자
			if ( NULL == pAIParam->m_pDestCHAR )
				return false;
			return pAIParam->m_pDestCHAR->Is_ClanMASTER ();

		case 1 :	// 1 : 공격타겟
		{
			CAI_OBJ *pTarget = pAIParam->m_pSourCHAR->Get_TARGET ();
			if ( NULL == pTarget )
				return false;

			return pTarget->Is_ClanMASTER ();
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool F_AICOND_NULL (stCondHead *pConDATA, t_AIPARAM *pAIParam)
{
	return false;
}

tagConditionFUNC g_FuncCOND[] = {
	{	NULL			},
	{	F_AICOND_00		},	// 0x000000001
	{	F_AICOND_01		},	// 0x000000002
	{	F_AICOND_02		},	// 0x000000003
	{	F_AICOND_03		},	// 0x000000004
	{	F_AICOND_04		},	// 0x000000005
	{	F_AICOND_05		},	// 0x000000006
	{	F_AICOND_06		},	// 0x000000007
	{	F_AICOND_07		},  // 0x000000008
	{	F_AICOND_08		},	// 0x000000009
	{	F_AICOND_09		},	// 0x00000000A
	{	F_AICOND_10		},	// 0x00000000B

	{	F_AICOND_11		},	// 0x00000000c

	{	F_AICOND_12		},	
	{	F_AICOND_13		},	
	{	F_AICOND_14		},	
	{	F_AICOND_15		},	
	{	F_AICOND_16		},	
	{	F_AICOND_17		},

	{	F_AICOND_18		},
	{	F_AICOND_19		},

	{	F_AICOND_20		},
	{	F_AICOND_21		},
	{	F_AICOND_22		},
	{	F_AICOND_23		},

	{	F_AICOND_24		},
	{	F_AICOND_25		},

	{	F_AICOND_26		},

	{	F_AICOND_27		},
	{	F_AICOND_28		},

	{	F_AICOND_29		},

	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
	{	F_AICOND_NULL	},
} ;

//-------------------------------------------------------------------------------------------------
