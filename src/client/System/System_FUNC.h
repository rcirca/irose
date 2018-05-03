/*
	$Header: /Client/System/System_FUNC.h 36    05-06-17 9:13p Navy $
*/
#ifndef	__SYSTEM_FUNC_H
#define	__SYSTEM_FUNC_H
#include "LUA_Func.h"
//-------------------------------------------------------------------------------------------------

#define	SYSTEM_SCRIPT
typedef const char*	ZSTRING;



SYSTEM_SCRIPT
void SC_InitGame ( void );



///
/// 새로운스크립트 실행
///
SYSTEM_SCRIPT
int SC_DoScript ( ZSTRING strName );



///
/// 캐릭터 생성관련
///

SYSTEM_SCRIPT
HNODE SC_CreateAVT ( ZSTRING strName, int iRace, int iFACE, int iHAIR, int iHELMET,
				   int iARMOR, int iGAUNTLET, int iBOOTS,
				   int iFACE_ITEM, int iKNAPSACK, int iWEAPON_R, int iWEAPON_L );

SYSTEM_SCRIPT
void SC_DestroyAVT ( HNODE hAVT );



/// 모션 세팅
SYSTEM_SCRIPT
void SC_SetAvatarMotionByName ( ZSTRING strName, ZSTRING strMotion );

SYSTEM_SCRIPT
void SC_SetAvatarMotionByIndex ( ZSTRING strName, int iMotionIDX );

SYSTEM_SCRIPT
void SC_SetVisible ( HNODE hAVT, int iIsVisible );


///
/// adjust transform
///

SYSTEM_SCRIPT
void SC_SetPosition ( HNODE hNODE, float x, float y, float z );







SYSTEM_SCRIPT
HNODE SC_LoadModel ( ZSTRING pModelName, HNODE hSkeleton, HNODE hMotion, float ScaleInLoad );

SYSTEM_SCRIPT
HNODE SC_LoadSkeleton ( ZSTRING pSkeletonName, ZSTRING pSkeletonPath );

SYSTEM_SCRIPT
HNODE SC_GetSkeleton ( int iIsfeMale );

SYSTEM_SCRIPT
HNODE SC_FindNode ( ZSTRING pNodeName );

SYSTEM_SCRIPT
HNODE SC_LoadMotion ( ZSTRING pMotionName, ZSTRING pMotionFileName, int bUseLoop, int InterpTypePosition, int InterpTypeRotation, float Scale, int bForModel );

SYSTEM_SCRIPT
void SC_AttachMotion ( HNODE hNode, HNODE hMotion );

SYSTEM_SCRIPT
void SC_ControlAnimation ( HNODE hNode, int iControl );

/// 모션 조작 관련
SYSTEM_SCRIPT
void SC_SetMotionFrame ( HNODE hNode, int iFrame );

SYSTEM_SCRIPT
int SC_GetTotalFrame ( HNODE hNode );


SYSTEM_SCRIPT
HNODE SC_LoadCamera ( ZSTRING pCameraName, ZSTRING pCameraPath, HNODE hCameraMotion );

SYSTEM_SCRIPT
HNODE SC_SetCameraDefault ( HNODE hCamera );

SYSTEM_SCRIPT
void SC_UnloadNode ( HNODE hNode );

SYSTEM_SCRIPT
int SC_SetRepeatCount ( HNODE hAnimatable, int iRepeatCount );

SYSTEM_SCRIPT
HNODE SC_GetMotion ( HNODE hAnimatable );

SYSTEM_SCRIPT
void SC_SetScale ( HNODE hVisible, float x, float y, float z );



SYSTEM_SCRIPT
void SC_SetModelDirection ( HNODE hModel, float fAngleDegree );

SYSTEM_SCRIPT
void SC_SetModelPosition ( HNODE hModel, float x, float y, float z );


SYSTEM_SCRIPT
HNODE SC_AddCreateAVT ( ZSTRING pszName, int iRace, int iFace, int iHair, int iHelmet,
					 int iArmor, int iGauntlet, int iBoots,
					 int iFaceItem, int iKnapsack, int iRWeapon, int iLWeapon );

SYSTEM_SCRIPT
void SC_RemoveCreateAVT ( ZSTRING pszName );




SYSTEM_SCRIPT
void SC_SetAvatarFace ( ZSTRING pszName, int iFace );

SYSTEM_SCRIPT
void SC_SetAvatarHair ( ZSTRING pszName, int iHair );
////인터페이스용
SYSTEM_SCRIPT
void SC_AddSelectItem4CreateAvatar ( int iType, ZSTRING pszItem, ZSTRING pszValue );

SYSTEM_SCRIPT
void SC_ClearSelectItem4CreateAvatar ( int iType );

SYSTEM_SCRIPT
void SC_SelectItem4CreateAvatar ( int iType, int iSubscript );

///
/// 존 관리
///
SYSTEM_SCRIPT
int SC_GetBGZoneNO ( void );



//------------------------------------------------------------------------------------------------
/// 이벤트 관련
//------------------------------------------------------------------------------------------------


///
/// 이벤트 오브젝트 관련..
/// type 0 : 포지션 노드
/// type 1 : 애니매이션 노드
///
SYSTEM_SCRIPT
HNODE SC_GetEventObject ( int iEventID, int iType );

SYSTEM_SCRIPT
int SC_GetEventObjectIndex ( int iEventID );

///
/// Con 파일 실행
///
SYSTEM_SCRIPT
void SC_RunEvent ( int iOwnerIndex, ZSTRING szQuestFILE, int iEventIDX );

SYSTEM_SCRIPT
void SC_RunEventFieldWarp ( int iOwnerIndex, ZSTRING szQuestFILE, int iEventIDX );

SYSTEM_SCRIPT
void SC_RunEventObjectEvent ( int iOwnerIndex, ZSTRING szQuestFILE, int iEventIDX );

///
/// 알림 메세지박스
///
SYSTEM_SCRIPT
void SC_ShowNotifyMessage ( int iStringIDX );


///
/// Log/OutputDebugString
///
SYSTEM_SCRIPT
void SC_LogString ( ZSTRING szMessage );


//------------------------------------------------------------------------------------------------
/// 컷씬 관련
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
int SC_ChangeState ( int iNewState );

SYSTEM_SCRIPT
int SC_ScreenFadeInStart ( float fade_in_t, float fade_m_t, float fade_out_t, int color_r, int color_g, int color_b );


//------------------------------------------------------------------------------------------------
/// 이펙트 관련
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
int		SC_GetEffectUseFile ( ZSTRING szEffectFile );

SYSTEM_SCRIPT
int		SC_GetEffectUseIndex ( int iEffectIndex );

SYSTEM_SCRIPT
void	SC_EffectOnChar ( int iObject, int iEffectHash );

SYSTEM_SCRIPT
void	SC_EffectOnObject ( HNODE hNode, int iEffectHash );

//------------------------------------------------------------------------------------------------
/// 인터페이스 관련
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
void	SC_OpenDialog ( int iDialog );

SYSTEM_SCRIPT
void	SC_OpenMsgbox ( ZSTRING pszMsg );

SYSTEM_SCRIPT
void	SC_SetButtonBlink ( int iParentDlg, int iButtonID );

SYSTEM_SCRIPT
void	SC_CreateEventButton ( int iButtonIndex );


SYSTEM_SCRIPT
void	SC_AddNpcIndicator ( int npcno, int auto_remove );

SYSTEM_SCRIPT
void	SC_AddCoordinatesIndicator ( int index, int zoneno, float x, float y );

SYSTEM_SCRIPT
void	SC_RemoveNpcIndicator ( int npcno );

SYSTEM_SCRIPT
void	SC_RemoveCoordinatesIndicator ( int index );
//------------------------------------------------------------------------------------------------
/// 아바타 정보
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
int	SC_GetAvatarEXP ( );

SYSTEM_SCRIPT
int	SC_GetAvatarLEVEL ( );




//------------------------------------------------------------------------------------------------
/// Send msg to web
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
void SC_SendMsgToWeb ( ZSTRING pstrIP, ZSTRING pstrPage, ZSTRING pstrMsg );

//------------------------------------------------------------------------------------------------
/// 스크립트를 실행한다. 2005-05-26 김창수
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
void SC_QF_doQuestTrigger ( ZSTRING TriggerName );

//------------------------------------------------------------------------------------------------
/// 퀘스트 트리거 컨디션을 체크한다. 2005-05-26 김창수
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
int SC_QF_checkQuestCondition ( ZSTRING szQuestTriggerName );


//------------------------------------------------------------------------------------------------
/// SC => GF/QF 연동 스크립드
/// 자세한 설명은 SC_를 뺀 스크립트 참조
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
int SC_GF_getVariable ( int iVarTYPE );	

SYSTEM_SCRIPT
int SC_QF_getNpcQuestZeroVal ( int iNpcNO );

SYSTEM_SCRIPT
int SC_QF_findQuest ( int iQuestID );

SYSTEM_SCRIPT
int SC_QF_getEpisodeVAR ( int iVarNO );

SYSTEM_SCRIPT
int SC_QF_getJobVAR ( int iVarNO );

SYSTEM_SCRIPT
int SC_QF_getPlanetVAR ( int iVarNO );

SYSTEM_SCRIPT
int SC_QF_getUnionVAR ( int iVarNO );

SYSTEM_SCRIPT
int SC_QF_getQuestVar ( int hQUEST, int iVarNO );

SYSTEM_SCRIPT
int SC_QF_getQuestSwitch ( int hQUEST, int iSwitchNO );

SYSTEM_SCRIPT
int SC_QF_getQuestItemQuantity ( int iQuestID, int iItemNo/*5자리:Type+No*/ );

SYSTEM_SCRIPT
int SC_QF_getUserSwitch ( int iSwitchNO );
//------------------------------------------------------------------------------------------------
/// 아바타의 부활존을 현재 존으로 변경한다.
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
void SC_GF_setRevivePosition ( );
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
/// Tutorial Image Show 관련 
//------------------------------------------------------------------------------------------------
SYSTEM_SCRIPT
void SC_ShowTutorialImage ( ZSTRING filename, int x, int y, float fadein_endtime, float fadeout_starttime, float max_time, int append_or_renewal );




//------------------------------------------------------------------------------------------------
// 함수 이름뒤에 한칸 띄운다. ( 다음에 한칸 띄운다, 파라미터 타입 뒤에 한칸 띄운다. 
// 파라미터 이름 뒤에 한칸 띄운다
//------------------------------------------------------------------------------------------------

int SC_QF_GetNpcQuestStatus ( int npcindex );
#endif //__SYSTEM_FUNC_H