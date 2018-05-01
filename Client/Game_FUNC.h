/*
	$Header: /Client/Game_FUNC.h 34    05-06-19 5:33p Navy $
*/
#ifndef	__GAME_FUNC_H
#define	__GAME_FUNC_H
#include "LUA_Func.h"
//-------------------------------------------------------------------------------------------------

#define	GF_SCRIPT
typedef const char*	ZSTRING;

//-------------------------------------------------------------------------------------------------
GF_SCRIPT
void	GF_log ( ZSTRING szStr );

GF_SCRIPT
int		GF_GetEffectUseFile ( ZSTRING szEffectFile );

GF_SCRIPT
int		GF_GetEffectUseIndex ( int iEffectIndex );

GF_SCRIPT
void	GF_EffectOnObject ( int iObject, int iEffectHash );

GF_SCRIPT
void	GF_WeatherEffectOnObject ( int iObject, int iEffectHash );

GF_SCRIPT
void	GF_DeleteEffectFromObject ( int iObject );

GF_SCRIPT
int		GF_GetTarget ( int iObject );

GF_SCRIPT
int		GF_GetMotionUseFile ( ZSTRING szMotionFile );

GF_SCRIPT
void	GF_SetMotion ( int iObject, int iMotionHashKEY, int iRepeatCnt );

GF_SCRIPT
void	GF_error ( void );


/*
=================================================================================
	게임 환경 데이터
=================================================================================
*/
GF_SCRIPT
int		GF_getVariable ( int iVarTYPE );	// iVarTYPE = GF_VAR

GF_SCRIPT
ZSTRING	GF_getName ( void );

GF_SCRIPT
void	GF_setVariable ( int iVarTYPE, int iValue );

GF_SCRIPT
int		GF_getZone ( void );

GF_SCRIPT
ZSTRING	GF_getDate ( void );

GF_SCRIPT
ZSTRING	GF_getGameVersion ( void );

enum {
	GTV_POPULATION=0,
	GTV_PRICE,
	GTV_LIFEQUALITY,
} ;

GF_SCRIPT
int		GF_getTownVar ( int iVarTYPE );


/*
=================================================================================
	경제 관련 : GF_System.cpp
=================================================================================
*/

/* TOWN_RATE 설정하기 */
GF_SCRIPT
void	GF_setTownRate ( int iTownRate );

/* TOWN_RATE 가져오기 */
GF_SCRIPT
int		GF_getTownRate ( void );

/* WORLD_RATE 가져오기 */
GF_SCRIPT
int		GF_getWorldRate ( void );

/* WORLD_RATE 설정하기 */
GF_SCRIPT
void	GF_setWorldRate ( int iWorldRate );

/* 아이템 물가 알아내기 */
GF_SCRIPT
int		GF_getItemRate ( int iItemTYPE );

/* 아이템 보유량 가져오기 */
GF_SCRIPT
int		GF_checkTownItem ( int iItemTYPE );



/*
=================================================================================
아이템 인벤토리에 넣기 / 제거하기 / 갯수 알아내기
=================================================================================
*/

/* 아이템 인덱스 알아내기 */
GF_SCRIPT
int		GF_getIDXOfInvItem ( int nItemNo, int nItemType );

/* 아이템 갯수 알아내기 */
GF_SCRIPT
int		GF_checkNumOfInvItem ( int nItemNo, int nItemType );

/* 장비 아이템 인벤에 넣기 */
GF_SCRIPT
void	GF_giveEquipItemIntoInv ( int cQuality, int cResmelt, int cJAMMING3, int cJAMMING2, int cJAMMING1, int nItemNo, int cType );

/* 아이템 인벤에서 꺼내기 */
GF_SCRIPT
void	GF_takeItemFromInv ( int nNum, int nItemNo, int cType );

/* 소모성/기타 아이템 주기 */
GF_SCRIPT
void	GF_giveUsableItemIntoInv ( int iQuantity, int iItemNo, int iItemType );

/*
=================================================================================
	아바타 아이템 장착 / 해제하기
=================================================================================
*/

/* 아바타 아이템 장착/해제하기 bWear : 1(장착), 0(해제) , 인벤토리와 상관 없음 */
GF_SCRIPT
void	GF_setEquipedItem ( int nPartIDX, int iItemIDX, int bWear );

/* 아이템 벗기 (장착 ==> 인벤) */
GF_SCRIPT
void	GF_putoffItem ( int iPartIDX, int iType );

/* 아이템 착용 (인벤 ==> 장착) */
GF_SCRIPT
void	GF_putonItem ( int iPartIDX, int iItemNo, int iItemType );


/*
=================================================================================
	아바타 현금 관련 : GF_ITEM.CPP
=================================================================================
*/

/* 아바타 현금소지량 보기 */
GF_SCRIPT
int		GF_checkUserMoney ( void );


/* 아바타 현금 주기 */
GF_SCRIPT
void	GF_addUserMoney ( int iAmount );

/* 아바타 소지현금 가져오기 */
GF_SCRIPT
void	GF_takeUserMoney ( int iAmount );

/*
=================================================================================
	사운드, 이펙트 관련...	GF_EFFECT.cpp
=================================================================================
*/
GF_SCRIPT
void	GF_playSound ( ZSTRING szSoundFile,	int iRepeat, int iOuterOpt );

// GF_SCRIPT
// void	GF_playEventSound ( int iEventNo, ZSTRING szSoundFile, int iRepeat, int iOuterOpt );


/*
=================================================================================
	카메라 관련...
=================================================================================
*/
GF_SCRIPT
void	GF_rotateCamera ( int iPole, float iDegree );

GF_SCRIPT
void	GF_zoomCamera ( int iPercent );

GF_SCRIPT
void	GF_playEffect ( int iEffect, float x, float y );

// GF_SCRIPT
// void	GF_playEffectAtEvent ( int iEventNo, ZSTRING szEffectName );

/*
=================================================================================
이동 / 워프
=================================================================================
*/
GF_SCRIPT
void	GF_moveXY ( int iMapNo, int iX, int iY );

GF_SCRIPT
int		GF_movableXY ( int iMapNo, int iX, int iY );

GF_SCRIPT
int		GF_moveEvent ( int iEffectNo );

GF_SCRIPT
void	GF_warp ( int iWarpIndex );

GF_SCRIPT
ZSTRING	GF_getReviveZoneName ( );

GF_SCRIPT
void	GF_setRevivePosition ( );


/*
=================================================================================
	몬스터...
=================================================================================
*/
GF_SCRIPT
void	GF_spawnMonXY ( int iMonNo, int iMonLevel, int iX, int iY );

GF_SCRIPT
void	GF_spawnMonAtEvent ( int iMonNo, int iMonLevel, int iEvnetNo );


/*
=================================================================================
상점 관련
=================================================================================
*/
GF_SCRIPT
void	GF_openStore ( int iNPCNO, int bSpecialTab );

/*
=================================================================================
계정창고 관련
=================================================================================
*/
GF_SCRIPT
void	GF_openBank ( int iNPCNO );

/*
=================================================================================
마일리지아이템창고 관련
=================================================================================
*/

GF_SCRIPT
void	GF_openDeliveryStore ( );


/*
=================================================================================
수리 관련
=================================================================================
*/
GF_SCRIPT
void	GF_repair ( int iNPCNO );


/*
=================================================================================
분리 / 분해
=================================================================================
*/
GF_SCRIPT
void	GF_openSeparate ( int iNPCNO );


/*
=================================================================================
제련
=================================================================================
*/
GF_SCRIPT
void	GF_openUpgrade ( int iNPCNO );


/*
=================================================================================
아이템 감정
=================================================================================
*/
GF_SCRIPT
void	GF_appraisal ( );

/*
=================================================================================
클랜 창설 / 해체
=================================================================================
*/
GF_SCRIPT
void	GF_organizeClan ( int iNPCNO );

GF_SCRIPT
void	GF_disorganizeClan ( int iNPCNO );

//----------------------------------------------------------------------------------------------------
///
/// 게임내 이벤트에 따른 효과 출력
///
//----------------------------------------------------------------------------------------------------

/*
아바타 관련
*/

/// 아바타 처음 생성할 때 생기는 효과
void SE_CharCreate  ( int iObject );
/// 아바타가 처음 월드에 조인할때
void SE_CharJoinZone( int iObject ); 
/// 아바타 사망시
void SE_CharDie		( int iObject );
/// 아바타 워프
void SE_CharWarp	( int iObject );
/// 아바타 레벨업
void SE_CharLevelUp ( int iObject );
/// 파티 레벨업
void SE_PartyLevelUp ( int iObject );

///제조 시작
void SE_StartMake ( int iObject );
///제련 시작
void SE_StartUpgrade ( int iObject );
/// 제조 성공
void SE_SuccessMake ( int iObject );
/// 제련 성공
void SE_SuccessUpgrade ( int iObject );
/// 분리/분해 성공
void SE_SuccessSeparate ( int iObject );
/// 제조 실패
void SE_FailMake ( int iObject );
///제련 실패
void SE_FailUpgrade ( int iObject );

///스킬레벨업
void SE_SkillLevelUp ( int iObject );




/*
몬스터 관련
*/

/// 몬스터 죽을때
void SE_MobDie ( int iObject );



///
/// Log/OutputDebugString
///
GF_SCRIPT
void GF_LogString ( ZSTRING szMessage );

/*
랜덤넘버 제네레이터
*/
GF_SCRIPT
int	GF_Random ( int iMax );


bool InitScript ();
void FreeScript ();




//------------------------------------------------------------------------------------------------
/// 날씨 이펙트
//------------------------------------------------------------------------------------------------
void SE_WeatherEffect ( int iObject, int iWeatherType );




//------------------------------------------------------------------------------------------------
/// 컷씬 존관련
//------------------------------------------------------------------------------------------------
GF_SCRIPT
int GF_ChangeState ( int iNewState );



//-------------------------------------------------------------------------------------------------
#endif

