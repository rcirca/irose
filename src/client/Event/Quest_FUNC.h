/*
	$Header: /Client/Event/Quest_FUNC.h 41    05-10-17 8:50p Raning $
*/
#ifndef	__QUEST_FUNC_H
#define	__QUEST_FUNC_H
#include "..\Common\CQuest.h"
#include "..\LUA_Func.h"
//-------------------------------------------------------------------------------------------------

#define	QF_SCRIPT
typedef const char*	ZSTRING;

/*
=================================================================================
	퀘스트 관련...
=================================================================================
*/

QF_SCRIPT
int		QF_getQuestCount ( void );

QF_SCRIPT
int		QF_findQuest ( int iQuestID );

QF_SCRIPT
int		QF_getQuestID ( int hQUEST );

QF_SCRIPT
int		QF_appendQuest ( int iQuestID );

QF_SCRIPT
void	QF_deleteQuest ( int iQuestID );


QF_SCRIPT
int		QF_checkQuestCondition ( ZSTRING szQuestTriggerName );

QF_SCRIPT
int		QF_doQuestTrigger ( ZSTRING szQuestTriggerName );


QF_SCRIPT
int		QF_getQuestVar ( int hQUEST, int iVarNO );

QF_SCRIPT
int		QF_getQuestSwitch ( int hQUEST, int iSwitchNO );

QF_SCRIPT
int		QF_getEpisodeVAR ( int iVarNO );

QF_SCRIPT
int		QF_getJobVAR ( int iVarNO );

QF_SCRIPT
int		QF_getPlanetVAR ( int iVarNO );

QF_SCRIPT
int		QF_getUnionVAR ( int iVarNO );

QF_SCRIPT
int		QF_getQuestItemQuantity ( int iQuestID, int iItemNo/*5자리:Type+No*/ );

QF_SCRIPT
int		QF_getNpcQuestZeroVal ( int iNpcNO );

QF_SCRIPT
int		QF_getUserSwitch ( int iSwitchNO );


/*
=================================================================================
	대화 관련...	Qf_CONVERSATION.cpp
=================================================================================
*/

QF_SCRIPT
int		QF_getEventOwner ( int hEvent );

QF_SCRIPT
void	QF_gotoCon ( int hEvent, int iDataIDX, int iItemIDX );

QF_SCRIPT
void	QF_beginCon ( void );

QF_SCRIPT
void	QF_closeCon ( void );

QF_SCRIPT
void	QF_ChangetalkImage ( ZSTRING szTalkImage );

QF_SCRIPT
void	QF_ChangetalkName ( ZSTRING szTalkName );


QF_SCRIPT
void	QF_NpcTalkinterfaceHide ( int iTime );

QF_SCRIPT
void	QF_NpcTalkinterfaceView ( void );

QF_SCRIPT
void	QF_NpcHide ( int iNpcIndex  );

QF_SCRIPT
void	QF_NpcView ( int iNpcIndex  );

QF_SCRIPT
void	QF_EffectCallSelf ( ZSTRING szEffectName );

QF_SCRIPT
void	QF_EffectCallNpc ( ZSTRING szEffectName, int iNpcIndex );

QF_SCRIPT
void	QF_MotionCallSelf ( ZSTRING szEffectName );

QF_SCRIPT
void	QF_MotionCallNpc ( ZSTRING szEffectName, int iNpcIndex );

QF_SCRIPT
void	QF_CameraworkingSelf ( ZSTRING szCameraName );

QF_SCRIPT
void	QF_CameraworkingNpc ( ZSTRING szCameraName, int iNpcIndex );

QF_SCRIPT
void	QF_CameraworkingPoint ( ZSTRING szCameraName, int PosX, int PosY );


/*
=================================================================================
포인트, 스킬 관련...
=================================================================================
*/

QF_SCRIPT
int		QF_getSkillLevel ( int iSkillNo );

QF_SCRIPT
int		QF_givePoint ( int iPoint );




/*
=================================================================================
Quest Function 관련 상수
=================================================================================
*/

#define		RCPOLE_AVATA		0
#define		RCPOLE_CAMERA		1


/*
	전역 참조 객체
*/
void QF_Init (lua_State *L);

#ifndef		g_pSoundLIST
extern CSoundLIST	   *g_pSoundLIST;
#endif

//-------------------------------------------------------------------------------------------------
#endif
