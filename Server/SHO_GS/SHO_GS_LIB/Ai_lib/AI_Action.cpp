/*
pEVENT->m_pDestCHAR;
pEVENT->m_pFindCHAR;
pEVENT->m_pLocalNPC;
pEVENT->m_pNearCHAR;
pEVENT->m_pSourCHAR;
*/

#include "stdAFX.h"
#include "CAI_FILE.h"
#include "CAI_LIB.h"

#ifdef	__SERVER
#include "ZoneLIST.h"
#endif

//-------------------------------------------------------------------------------------------------
void F_AIACT00 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif
/*
	정지 동작으로..
*/

	AIACT00 *pAct = (AIACT00*)pActDATA;
	pEVENT->m_pSourCHAR->SetCMD_STOP ();
}

//-------------------------------------------------------------------------------------------------
void F_AIACT01 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
/*
struct		AIACT01
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	BYTE			cAction ;					// 동작번호
} ;

	cAction : 감정 표현동작.
*/
	AIACT01 *pAct = (AIACT01*)pActDATA;
	pEVENT->m_pSourCHAR->Set_EMOTION( pAct->cAction );
}

//-------------------------------------------------------------------------------------------------
void F_AIACT02 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
/*
struct		AIACT02
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	char			szCon[1] ;					// 대사 데이터
} ;
*/
//	AIACT02 *pAct = (AIACT02*)pActDATA;
	AIACT02_STR *pAct = (AIACT02_STR*)pActDATA;
	pEVENT->m_pSourCHAR->Say_MESSAGE( pAct->szCon );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT03 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// 자신의 현재위치에서 임의 이동...
struct		AIACT03
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cSpeed ;					// 천천히 , 보통 , 빠르게
} ;
*/
	AIACT03 *pAct = (AIACT03*)pActDATA;

	float fPosX, fPosY;
	fPosX = pEVENT->m_pSourCHAR->Get_CurXPOS() + AI_SysRANDOM( pAct->iDistance*2 ) - pAct->iDistance;
	fPosY = pEVENT->m_pSourCHAR->Get_CurYPOS() + AI_SysRANDOM( pAct->iDistance*2 ) - pAct->iDistance;

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, pAct->cSpeed );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT04 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// 자신의 출현위치에서 임의 이동...
struct		AIACT04
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cSpeed ;					// 천천히 , 보통 , 빠르게
} ;
*/
	AIACT04 *pAct = (AIACT04*)pActDATA;

	float fPosX, fPosY;
	fPosX = pEVENT->m_pSourCHAR->Get_BornXPOS() + AI_SysRANDOM( pAct->iDistance*2 ) - pAct->iDistance;
	fPosY = pEVENT->m_pSourCHAR->Get_BornYPOS() + AI_SysRANDOM( pAct->iDistance*2 ) - pAct->iDistance;

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, pAct->cSpeed );
}

//-------------------------------------------------------------------------------------------------
void F_AIACT05 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct		AIACT05
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	BYTE			cSpeed ;					// 천천히 , 보통 , 빠르게
	//BYTE			cAttackOrMove ;				// 공격 , 이동
} ;
*/
	if ( NULL == pEVENT->m_pFindCHAR )
		return;

	AIACT05 *pAct = (AIACT05*)pActDATA;

	float fPosX, fPosY;

	// 2003. 1. 2 주변 2m 랜덤 위치로..
	fPosX = pEVENT->m_pFindCHAR->Get_CurXPOS() + AI_SysRANDOM( 400 ) - 200;
	fPosY = pEVENT->m_pFindCHAR->Get_CurYPOS() + AI_SysRANDOM( 400 ) - 200;

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, pAct->cSpeed );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT06 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// 거리내의 아바타중 능력치가 가장장 x한 것을 공격.
struct		AIACT06
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cAbType ;					// 능력치 타입
	BYTE			cMoreLess ;					// 큰가 , 작은가
} ;
*/
	AIACT06 *pAct = (AIACT06*)pActDATA;

	int iMinValue=9999999, iMaxValue=-9999999, iValue;

	CAI_OBJ *pMinCHAR=NULL, *pMaxCHAR=NULL;
	CAI_OBJ *pFindCHAR = pEVENT->m_pSourCHAR->AI_FindFirstOBJ( pAct->iDistance );
	int iFindCNT = 0;
//	short nMyTeamNO = pEVENT->m_pSourCHAR->Get_TeamNO();

	while( pFindCHAR ) {
		if ( ( pEVENT->m_pSourCHAR!=pFindCHAR ) && !pEVENT->m_pSourCHAR->Is_ALLIED( pFindCHAR ) ) {
			iValue = AI_Get_Ability( pFindCHAR, pAct->cAbType );
			if ( iValue < iMinValue ) {
				pMinCHAR = pFindCHAR;
				iMinValue = iValue;
			}
			if ( iValue > iMaxValue ) {
				pMaxCHAR = pFindCHAR;
				iMaxValue = iValue;
			}
		}
		pFindCHAR = pEVENT->m_pSourCHAR->AI_FindNextOBJ ();
	} 

	if ( pAct->cMoreLess ) {
		// 작은
		if ( pMinCHAR )
			pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pMinCHAR->Get_TAG() );
	} else {
		// 큰
		if ( pMaxCHAR )
			pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pMaxCHAR->Get_TAG() );
	}
}
//-------------------------------------------------------------------------------------------------
void F_AIACT07 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct		AIACT07
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
												// 데이터 필요 없음
} ;
*/
	pEVENT->m_pSourCHAR->Special_ATTACK();
}
//-------------------------------------------------------------------------------------------------
void F_AIACT08 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct		AIACT08
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	BYTE			cSpeed ;					// 천천히 , 보통 , 빠르게
} ;
*/
	AIACT08 *pAct = (AIACT08*)pActDATA;

	CAI_OBJ *pTarget = pEVENT->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return;

	float fPosX, fPosY;
	float fDistance, fX, fY;

	fX = pTarget->Get_CurXPOS() - pEVENT->m_pSourCHAR->Get_CurXPOS();
	fY = pTarget->Get_CurYPOS() - pEVENT->m_pSourCHAR->Get_CurYPOS();
	fDistance = pEVENT->m_pSourCHAR->Get_DISTANCE( pTarget );

	fPosX = pEVENT->m_pSourCHAR->Get_CurXPOS() - ( pAct->iDistance * fX / fDistance );
	fPosY = pEVENT->m_pSourCHAR->Get_CurYPOS() - ( pAct->iDistance * fY / fDistance );

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, pAct->cSpeed );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT09 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif
/*
typedef struct		tagAIACT09
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	WORD			wMonster ;					// 몬스터번호
} AIACT09 , AIACT10 ;
*/
	AIACT09 *pAct = (AIACT09*)pActDATA;
	pEVENT->m_pSourCHAR->Change_CHAR( pAct->wMonster );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT10 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif
/*
typedef struct		tagAIACT09
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	WORD			wMonster ;					// 몬스터번호
} AIACT09 , AIACT10 ;
*/
	AIACT10 *pAct = (AIACT10*)pActDATA;
	pEVENT->m_pSourCHAR->Create_PET ( 
			pAct->wMonster, 
			pEVENT->m_pSourCHAR->Get_CurXPOS(),
			pEVENT->m_pSourCHAR->Get_CurYPOS(), 150, 0, pEVENT->m_wPattern != AI_PATTERN_DEAD );
}
//-------------------------------------------------------------------------------------------------
void F_AIACT11 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// idistance안의 비전투중인 동료 numofmons 수만큼 자신의
// 공격 타겟을 협공하라고 명령.
struct		AIACT11
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
	int				iNumOfMonster;
} ;
*/
	AIACT11 *pAct = (AIACT11*)pActDATA;
	CAI_OBJ *pTarget = pEVENT->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return;

//	short nMyTeamNO = pEVENT->m_pSourCHAR->Get_TeamNO();

	CAI_OBJ *pFindCHAR = pEVENT->m_pSourCHAR->AI_FindFirstOBJ( pAct->iDistance );
	int iFindCNT = 0;
	while( pFindCHAR ) {
		if ( ( pEVENT->m_pSourCHAR!=pFindCHAR )		&&
			 pEVENT->m_pSourCHAR->Is_SameTEAM( pFindCHAR )	&&
			 pFindCHAR->Get_TARGET() == NULL ) {

			pFindCHAR->SetCMD_RUNnATTACK( pTarget->Get_TAG() );

			if ( ++iFindCNT >= pAct->iNumOfMonster )
				return;
		}
		pFindCHAR = pEVENT->m_pSourCHAR->AI_FindNextOBJ ();
	} 
}
//-------------------------------------------------------------------------------------------------
void F_AIACT12 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// 가작 가까이 있는 캐릭터를 공격 타겟으로...
struct		AIACT12
{
		DWORD			dwSize ;		// 이 스트럭쳐의 크기
		AITYPE			Type ;			// 조건 타입
} ;
*/
	if ( pEVENT->m_pNearCHAR ) {
		pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pEVENT->m_pNearCHAR->Get_TAG() );
	}
}
//-------------------------------------------------------------------------------------------------
void F_AIACT13 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// 조건에서 체크된 케릭터를 공격 타겟으로...
struct		AIACT13
{
		DWORD			dwSize ;		// 이 스트럭쳐의 크기
		AITYPE			Type ;			// 조건 타입
} ;
*/
	if ( pEVENT->m_pFindCHAR ) {
		pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pEVENT->m_pFindCHAR->Get_TAG() );
	}
}
//-------------------------------------------------------------------------------------------------
void F_AIACT14 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// 근처 distance내의 같은 종류의 동료에게 자신의 공격 타겟을 
// 공격하라고 명령한다.
struct		AIACT14
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
	int				iDistance ;					// 거리
} ;
*/
	CAI_OBJ *pTarget   = pEVENT->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return;

	AIACT14 *pAct = (AIACT14*)pActDATA;

//	short nMyTeamNO = pEVENT->m_pSourCHAR->Get_TeamNO ();
	short CharNO    = pEVENT->m_pSourCHAR->Get_CharNO ();

	CAI_OBJ *pFindCHAR = pEVENT->m_pSourCHAR->AI_FindFirstOBJ( pAct->iDistance );
	while( pFindCHAR ) {
		if ( ( pEVENT->m_pSourCHAR!=pFindCHAR )		&&
			 pFindCHAR->Get_CharNO() == CharNO		&&
			 pEVENT->m_pSourCHAR->Is_SameTEAM( pFindCHAR )	&&
			 pFindCHAR->Get_TARGET() == NULL ) {
			pFindCHAR->SetCMD_RUNnATTACK( pTarget->Get_TAG() );
		}
		pFindCHAR = pEVENT->m_pSourCHAR->AI_FindNextOBJ ();
	} 
}
//-------------------------------------------------------------------------------------------------
void F_AIACT15 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
// 공격 타겟을 현재 자신을 공격한 케릭터로 바꾼다.
struct		AIACT15
{
	DWORD			dwSize ;					// 이 스트럭쳐의 크기
	AITYPE			Type ;						// 조건 타입
} ;
*/
	if ( pEVENT->m_pDestCHAR ) {
		pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pEVENT->m_pDestCHAR->Get_TAG() );
	}
}

//-------------------------------------------------------------------------------------------------
void F_AIACT16 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct AIACT16
{
	DWORD	dwSize;		/// 이 스트럭쳐의 크기
	AITYPE	Type;		/// Type = AIACT_16 . 도망
	int		iDistance ;	/// 리젠을 중심으로 몇 미터 이내로 도망
};
*/
	AIACT16 *pAct = (AIACT16*)pActDATA;

	pEVENT->m_pSourCHAR->Run_AWAY ( pAct->iDistance );
}

//-------------------------------------------------------------------------------------------------
void F_AIACT17 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
struct AIACT17
{
	DWORD	dwSize;		/// 이 스트럭쳐의 크기
	AITYPE	Type;		/// Type = AIACT_16 . 도망
	short	item0;		/// 아이템 번호 0  . 만약 아이템번호가 없으면 -1을 집어 넣는다
	short	item1;		/// 아이템 번호 1
	short	item2;		/// 아이템 번호 2
	short	item3;		/// 아이템 번호 3
	short	item4;		/// 아이템 번호 4 
};
*/
	AIACT17 *pAct = (AIACT17*)pActDATA;

	short nDropITEM = pAct->m_ITEMS[ pEVENT->m_pSourCHAR->Get_RANDOM(5) ];
	if ( nDropITEM > 0 )
		pEVENT->m_pSourCHAR->Drop_ITEM( nDropITEM, (BYTE)pAct->iToOwner );
}

//-------------------------------------------------------------------------------------------------
void F_AIACT18 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif

/*
 * Struct Name : AIACT18 - <<2004.1.29 추가>>
 * 지정된 번호의 비전투중인 몹에게 공격호출 명령. (몬스터번호, 거리, 마리수) 
 * 추가 설명 : 지정된 거리안에 지정된 마리수만큼의 지정된 번호의 몬스터에게 공격호출 명령
struct AIACT18
{
	DWORD		dwSize;			/// 이 스트럭쳐의 크기
	AITYPE		Type;			/// Type값은 AIACT_18

	WORD		cMonster ;		// 몬스터번호
	WORD		wHowMany ;		// 몇마리
	int			iDistance ;		// 거리
};
*/
	CAI_OBJ *pTarget   = pEVENT->m_pSourCHAR->Get_TARGET ();
	if ( NULL == pTarget )
		return;

	AIACT18 *pAct = (AIACT18*)pActDATA;

	CAI_OBJ *pFindCHAR = pEVENT->m_pSourCHAR->AI_FindFirstOBJ( pAct->iDistance );

//	short nMyTeamNO = pEVENT->m_pSourCHAR->Get_TeamNO();

	WORD wHowMany = 0;
	while( pFindCHAR ) {
		if ( ( pEVENT->m_pSourCHAR!=pFindCHAR )				&&
			 pFindCHAR->Get_CharNO() == pAct->cMonster		&&
			 pEVENT->m_pSourCHAR->Is_SameTEAM( pFindCHAR )	&&
			 pFindCHAR->Get_TARGET() == NULL ) {

			pFindCHAR->SetCMD_RUNnATTACK( pTarget->Get_TAG() );

			if ( ++wHowMany >= pAct->wHowMany )
				break;
		}
		pFindCHAR = pEVENT->m_pSourCHAR->AI_FindNextOBJ ();
	} 
}

//-------------------------------------------------------------------------------------------------
void F_AIACT19 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif
/*
 * Struct Name : AIACT19 - <<2004.1.29 추가>>
 * 가장 가까이 있는 아바타를 공격타겟으로 지정 
struct AIACT19
{
	DWORD		dwSize;				/// 이 스트럭쳐의 크기
	AITYPE		Type;				/// Type값은 AIACT_19

	/// 추가 조건 필요
};
*/
	if ( pEVENT->m_pNearCHAR ) {
		pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pEVENT->m_pNearCHAR->Get_TAG() );
	}
}

//-------------------------------------------------------------------------------------------------
void F_AIACT20 (stActHead *pActDATA, t_AIPARAM *pEVENT)
{
#if !defined( __SERVER ) && !defined( __VIRTUAL_SERVER )
	return;
#endif
/*
 * Struct Name : AIACT20 - <<2004.1.29 추가>>
 * 지정된 몬스터를 소환시킨다. 자신/공격자(자신을 공격한 캐릭터)/공격타겟(자신이 공격하는 캐릭터).
 * 근처 ?미터 이내에 소환 
 * 추가설명 : 마리수는 1마리이다.
struct AIACT20
{
	DWORD		dwSize;				/// 이 스트럭쳐의 크기
	AITYPE		Type;				/// Type값은 AIACT_20

	WORD		cMonster;			/// 몬스터번호
	BYTE		btPos;				/// 소환위치 : 0 = 자신 주위,  1 = 공격자 주위(자신을 공격한), 2 = 공격타겟 주위(자신이 공격하고 있는)
	int			iDistance ;			// 거리 . 이 거리 이내에 소환
};
*/

	AIACT20 *pAct = (AIACT20*)pActDATA;
	switch( pAct->btPos ) {
		case 0 :	// 자신 주위...
		{
			pEVENT->m_pSourCHAR->Create_PET( pAct->cMonster, 
							pEVENT->m_pSourCHAR->Get_CurXPOS(),
							pEVENT->m_pSourCHAR->Get_CurYPOS(),
							pAct->iDistance, 0, pEVENT->m_wPattern != AI_PATTERN_DEAD );
			break;
		}

		case 1 :	// 자신을 공격한...
		{
			if ( pEVENT->m_pDestCHAR ) {
				pEVENT->m_pSourCHAR->Create_PET( pAct->cMonster, 
								pEVENT->m_pDestCHAR->Get_CurXPOS(),
								pEVENT->m_pDestCHAR->Get_CurYPOS(),
								pAct->iDistance, 0, pEVENT->m_wPattern != AI_PATTERN_DEAD );
			}
			break;
		}
			
		case 2 :	// 자신이 공격중인...
		{
			CAI_OBJ *pTarget = pEVENT->m_pSourCHAR->Get_TARGET ();
			if ( pTarget ) {
				pEVENT->m_pSourCHAR->Create_PET( pAct->cMonster, 
								pTarget->Get_CurXPOS(),
								pTarget->Get_CurYPOS(),
								pAct->iDistance, 0, pEVENT->m_wPattern != AI_PATTERN_DEAD );
			}
			break;
		}
	} ;
}

void F_AIACT21 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
	// 사용 안함.
}
void F_AIACT22 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
	// 사용 안함.
}
void F_AIACT23 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{
/*
// 자살시킨다
struct AIACT23
{
	DWORD	dwSize;
	AITYPE	Type;
};
*/
#ifdef	__SERVER
	pEVENT->m_pSourCHAR->Add_DAMAGE( pEVENT->m_pSourCHAR->Get_HP()+1 );
#endif
}
void F_AIACT24 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{
/*
/// 스킬과 동작을 대상을 향해 사용
struct AIACT24
{
	DWORD		dwSize;			/// 이 스트럭쳐의 크기
	AITYPE		Type;			/// Type

	BYTE		btTarget;		// 0=조건에 체크된 캐릭터, 1=현재 공격타겟, 2=자기자신(셀프스킬)
	short		nSkill;			// 스킬번호
	short		nMotion;		// 동작번호 -1 = 동작없음
};
*/
#ifdef	__SERVER
	AIACT24 *pAct = (AIACT24*)pActDATA;
	switch( pAct->btTarget )
	{
		case 0 :
			if ( pEVENT->m_pFindCHAR ) {
				pEVENT->m_pSourCHAR->SetCMD_Skill2OBJ( pEVENT->m_pFindCHAR->Get_TAG(), pAct->nSkill, pAct->nMotion );
			}
			break;
		case 1 :
		{
			CAI_OBJ *pTarget   = pEVENT->m_pSourCHAR->Get_TARGET ();
			if ( pTarget ) {
				pEVENT->m_pSourCHAR->SetCMD_Skill2OBJ( pTarget->Get_TAG(), pAct->nSkill, pAct->nMotion );
			}
			break;
		}
		case 2 :
			pEVENT->m_pSourCHAR->SetCMD_Skill2SELF( pAct->nSkill, pAct->nMotion );
			break;
	}
#endif
}
void F_AIACT25 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
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
#ifdef	__SERVER
	if ( NULL == pEVENT->m_pLocalNPC )
		return;

	AIACT25 *pAct = (AIACT25*)pActDATA;

	int iValue  = pEVENT->m_pLocalNPC->Get_ObjVAR( (BYTE)pAct->nVarNo );
	int iResult = ::Result_AiOP( pAct->btOp, iValue, pAct->iValue );

	pEVENT->m_pLocalNPC->Set_ObjVAR( (BYTE)pAct->nVarNo, iResult );
#endif
}
void F_AIACT26 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
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
#ifdef	__SERVER
	AIACT26 *pAct = (AIACT26*)pActDATA;

	int iValue  = pEVENT->m_pSourCHAR->Get_WorldVAR( pAct->nVarNo );
	int iResult = ::Result_AiOP( pAct->btOp, iValue, pAct->iValue );

	pEVENT->m_pSourCHAR->Set_WorldVAR( pAct->nVarNo, iResult );
#endif
}
void F_AIACT27 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
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
#ifdef	__SERVER
	AIACT27 *pAct = (AIACT27*)pActDATA;

	int iValue  = pEVENT->m_pSourCHAR->Get_EconomyVAR( pAct->nVarNo );
	int iResult = ::Result_AiOP( pAct->btOp, iValue, pAct->iValue );
	
	pEVENT->m_pSourCHAR->Set_EconomyVAR( pAct->nVarNo, iResult );
#endif
}
void F_AIACT28 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
/*
struct AIACT28
{
	DWORD		dwSize;			/// 이 스트럭쳐의 크기
	AITYPE		Type;			/// Type

	BYTE		btMsgType;		/// 0 = 현재필드, 1 = 현재존, 2=전체월드
	short		nMsgLength;		/// 대사의 길이 NULL포함
	char		szMsg[ 1 ];		/// 대사
};
*/
//	AIACT28 *pAct = (AIACT28*)pActDATA;
	AIACT28_STR *pAct = (AIACT28_STR*)pActDATA;
	
#ifndef	__SERVER
	pEVENT->m_pSourCHAR->Say_MESSAGE( pAct->szMsg );
#else
	char *szMSG = pAct->szMsg;
	switch( pAct->btMsgType )
	{
		case 0 :
			pEVENT->m_pSourCHAR->Send_gsv_CHAT( szMSG  );
			break;
		case 1 :
			pEVENT->m_pSourCHAR->Send_gsv_SHOUT( szMSG );
			break;
		case 2 :
			pEVENT->m_pSourCHAR->Send_gsv_ANNOUNCE_CHAT( szMSG );
			break;
	}
#endif
}

void F_AIACT29 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
#ifdef	__SERVER
	CAI_OBJ *pOwner = pEVENT->m_pSourCHAR->Get_CALLER ();
	if ( !pOwner )
		return;

	AIACT29 *pAct = (AIACT29*)pActDATA;

	float fPosX, fPosY;
	float fDistance;

	fDistance = pEVENT->m_pSourCHAR->Get_DISTANCE( pOwner );

	fPosX = pOwner->Get_CurXPOS() - ( 0.2f * fDistance );
	fPosY = pOwner->Get_CurYPOS() - ( 0.2f * fDistance );

	pEVENT->m_pSourCHAR->SetCMD_MOVE2D( fPosX, fPosY, 1 /* run mode */ );
#endif
}

void F_AIACT30 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
#ifdef	__SERVER
	AIACT30 *pAct = (AIACT30*)pActDATA;

	t_HASHKEY*pHash = (t_HASHKEY*)( pAct->szTrigger );

	if ( OBJ_MOB == pEVENT->m_pSourCHAR->Get_CharObjTAG() ) {
		// 몹은 스스로 트리거를 발동할수 없다 !!! 왜??? 변수가 존제하지 않는다.
		if ( pEVENT->m_pLocalNPC  )
			pEVENT->m_pLocalNPC->Set_TRIGGER( *pHash );
		return;
	}

	pEVENT->m_pSourCHAR->Set_TRIGGER( *pHash );
#endif
}

void F_AIACT31 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
/*
// 주인이 가지고 있는 공격타겟 공유
struct AIACT31
{
	DWORD		dwSize;
	AITYPE		Type;
};
*/
#ifdef	__SERVER
	AIACT31 *pAct = (AIACT31*)pActDATA;

	CAI_OBJ *pOwner = pEVENT->m_pSourCHAR->Get_CALLER ();
	if ( pOwner ) {
		CAI_OBJ *pTarget = pOwner->Get_TARGET();
		if ( pTarget ) {
			// 공격
			if ( !pEVENT->m_pSourCHAR->Is_ALLIED( pTarget ) )
				pEVENT->m_pSourCHAR->SetCMD_RUNnATTACK( pTarget->Get_TAG() );
		}
	}
#endif
}

void F_AIACT32 (stActHead *pActDATA, t_AIPARAM *pEVENT)		
{	
/*
// PK 모드 On/Off. NPC가 존재하는 존에.
struct AIACT32
{
	DWORD		dwSize;
	AITYPE		Type;

	short		nZoneNo;	// 존번호, 현재는 무조건 0
	BYTE		btOnOff;	// 0 = Off , 1 = On
};
*/	
#ifdef	__SERVER
	AIACT32 *pAct = (AIACT32*)pActDATA;

	if ( 0 == pAct->nZoneNo ) {
		// 현재 존...
		g_pZoneLIST->Set_PK_FLAG( pEVENT->m_pSourCHAR, pAct->btOnOff );
	} else {
		g_pZoneLIST->Set_PK_FLAG( pAct->nZoneNo, pAct->btOnOff );
	}
#endif
}

void F_AIACT33 (stActHead *pActDATA, t_AIPARAM *pEVENT )
{
/*
// 현재/몇번 존의 리젠을 On/Off 함
struct AIACT33
{
	DWORD		dwSize;
	AITYPE		Type;

	short		nZonNo;	// 0 : 현재존, 이외: 존번호
	BYTE		btOp;	// 0: Off, 1: On, 2: Toggle
};
*/
#ifdef	__SERVER
	AIACT33 *pAct = (AIACT33*)pActDATA;

	if ( 0 == pAct->nZonNo ) {
		// 현재존..
		if ( 2 == pAct->btOp ) {
			g_pZoneLIST->Toggle_RegenSYSTEM( pEVENT->m_pSourCHAR );
		} else {
			g_pZoneLIST->Set_RegenSYSTEM( pEVENT->m_pSourCHAR, pAct->btOp );
		}
	} else {
		if ( 2 == pAct->btOp ) {
			g_pZoneLIST->Toggle_RegenSYSTEM( pAct->nZonNo );
		} else {
			g_pZoneLIST->Set_RegenSYSTEM( pAct->nZonNo, pAct->btOp );
		}
	}
#endif
}

void F_AIACT34 (stActHead *pActDATA, t_AIPARAM *pEVENT )
{
/*
// 주인캐릭터에게 아이템 지급
struct AIACT34
{
	DWORD		dwSize;
	AITYPE		Type;

	short		nItemNo;
	short		nCnt;		// nItemNo 를 nCnt 갯수 만큼 지급
};
*/
#ifdef	__SERVER
	AIACT34 *pAct = (AIACT34*)pActDATA;

	CAI_OBJ *pOwner = pEVENT->m_pSourCHAR->Get_CALLER ();
	if ( !pOwner )
		return;

	pOwner->Recv_ITEM( pAct->nItemNo, pAct->nCnt );
#endif
}

void F_AIACT35 (stActHead *pActDATA, t_AIPARAM *pEVENT )
{
/*
// 몬스터 변수 값 변경
struct	AIACT35
{
	DWORD			dwSize;
	AITYPE			Type;

	short			nVarNo;	// 변수 번호 : 0 ~ 4
	int				iValue;	// 비교할 데이터값
	BYTE			btOp;	// (액션쪽) 5 = 값바꿈, 6 = 증가, 7 = 감소
};
*/
#ifdef	__SERVER
	AIACT35 *pAct = (AIACT35*)pActDATA;

	int iValue  = pEVENT->m_pSourCHAR->Get_AiVAR( pAct->nVarNo );
	int iResult = ::Result_AiOP( pAct->btOp, iValue, pAct->iValue );

	pEVENT->m_pSourCHAR->Set_AiVAR( pAct->nVarNo, iResult );
#endif
}


void F_AIACT36 (stActHead *pActDATA, t_AIPARAM *pEVENT )
{
/*
// 자신의 근처에 ?번 몬스터 소환 ( 이전 10번꺼에 주인 지정 추가 ) 
struct		AIACT36 
{
	DWORD		dwSize ;		// 이 스트럭쳐의 크기
	AITYPE		Type ;			// 조건 타입
	WORD		cMonster ;		// 몬스터번호

	BYTE		btMaster;	// 0: 비지정, 1: 자신(호출한)을 주인으로 지정
} ;
*/
	AIACT36 *pAct = (AIACT36*)pActDATA;
	pEVENT->m_pSourCHAR->Create_PET ( 
			pAct->cMonster, 
			pEVENT->m_pSourCHAR->Get_CurXPOS(),
			pEVENT->m_pSourCHAR->Get_CurYPOS(), 150, pAct->btMaster, pEVENT->m_wPattern != AI_PATTERN_DEAD );
}

void F_AIACT37 (stActHead *pActDATA, t_AIPARAM *pEVENT )
{
/*
// ?주변에 ?미터 이내에 ?번 몬스터 소환 ( 이전 20번꺼에 주인 지정 추가 )
struct AIACT37
{
	DWORD		dwSize;				/// 이 스트럭쳐의 크기
	AITYPE		Type;				/// Type값은 AIACT_20

	WORD		cMonster;			/// 몬스터번호
	BYTE		btPos;				/// 소환위치 : 0 = 자신 주위,  1 = 공격자 주위(자신을 공격한), 2 = 공격타겟 주위(자신이 공격하고 있는)
	int			iDistance;			// 거리 . 이 거리 이내에 소환

	BYTE		btMaster;	// 0: 비지정, 1: 자신(호출한)을 주인으로 지정
};
*/
	AIACT37 *pAct = (AIACT37*)pActDATA;

	switch( pAct->btPos ) {
		case 0 :	// 자신 주위...
		{
			pEVENT->m_pSourCHAR->Create_PET( pAct->cMonster, 
							pEVENT->m_pSourCHAR->Get_CurXPOS(),
							pEVENT->m_pSourCHAR->Get_CurYPOS(),
							pAct->iDistance, pAct->btMaster, pEVENT->m_wPattern != AI_PATTERN_DEAD );
			break;
		}

		case 1 :	// 자신을 공격한...
		{
			if ( pEVENT->m_pDestCHAR ) {
				pEVENT->m_pSourCHAR->Create_PET( pAct->cMonster, 
								pEVENT->m_pDestCHAR->Get_CurXPOS(),
								pEVENT->m_pDestCHAR->Get_CurYPOS(),
								pAct->iDistance, pAct->btMaster, pEVENT->m_wPattern != AI_PATTERN_DEAD );
			}
			break;
		}
			
		case 2 :	// 자신이 공격중인...
		{
			CAI_OBJ *pTarget = pEVENT->m_pSourCHAR->Get_TARGET ();
			if ( pTarget ) {
				pEVENT->m_pSourCHAR->Create_PET( pAct->cMonster, 
								pTarget->Get_CurXPOS(),
								pTarget->Get_CurYPOS(),
								pAct->iDistance, pAct->btMaster, pEVENT->m_wPattern != AI_PATTERN_DEAD );
			}
			break;
		}
	} ;
}

//-------------------------------------------------------------------------------------------------
void F_AIACT_NULL(stActHead *pActDATA, t_AIPARAM *pEVENT )
{
	;
}

tagActionFUNC g_FuncACTION[] = {
	{	NULL		},
	{	F_AIACT00	},
	{	F_AIACT01	},
	{	F_AIACT02	},
	{	F_AIACT03	},
	{	F_AIACT04	},
	{	F_AIACT05	},
	{	F_AIACT06	},
	{	F_AIACT07	},
	{	F_AIACT08	},
	{	F_AIACT09	},
	{	F_AIACT10	},
	{	F_AIACT11	},
	{	F_AIACT12	},
	{	F_AIACT13	},
	{	F_AIACT14	},
	{	F_AIACT15	},
	{	F_AIACT16	},
	{	F_AIACT17	},
	{	F_AIACT18	},
	{	F_AIACT19	},
	{	F_AIACT20	},

	{	F_AIACT21	},

	{	F_AIACT22	},
	{	F_AIACT23	},
	{	F_AIACT24	},
	{	F_AIACT25	},
	{	F_AIACT26	},
	{	F_AIACT27	},
	{	F_AIACT28	},

	{	F_AIACT29	},
	{	F_AIACT30	},

	{	F_AIACT31	},
	{	F_AIACT32	},

	{	F_AIACT33	},
	{	F_AIACT34	},
	{	F_AIACT35	},

	{	F_AIACT36	},
	{	F_AIACT37	},

	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
	{	F_AIACT_NULL	},
} ;

//-------------------------------------------------------------------------------------------------
