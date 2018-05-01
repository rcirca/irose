#ifndef	__CTHREADLOG_H
#define	__CTHREADLOG_H
#include "classSYNCOBJ.h"
#include "classTIME.h"
#include "CSqlTHREAD.h"
//-------------------------------------------------------------------------------------------------

#define	__NEW_LOG
// #define	__NEW_LOG_2005_05

#if !defined( __SHO_WS ) && !defined( __SHO_GS )
	#error >>> ERROR :: must defined __SHO_WS or __SHO_GS
#endif

#ifdef	__SHO_WS
	#include "CWS_Client.h"
	typedef	CWS_Client	classUSER;
	#include "IO_STB.h"
#endif

#define	SP_M_DEFLOG		"{call gs_M_DefLOG(%I64d, \'%s\',\'%s\',\'%s\',\'%s\',\'%s\')}"

// INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP ) VALUES( default, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM,  @szOBJ, @szOBJIP )
#define	SP_M_OBJLOG		"{call gs_M_ObjLOG(%I64d, \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\')}"

// INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM,txtDESC ) VALUES( default, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM, @szDESC )
#define	SP_M_DESCLOG	"{call gs_M_DescLOG(%I64d, \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\')}"

// INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP, txtDESC ) VALUES( default, @szSUB, @szSBJIP, @szACT, @szLOC, @szITEM,  @szOBJ, @szOBJIP, @szDESC )
#define SP_M_OBJDESCLOG	"{call gs_M_ObjDescLOG(%I64d, \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\')}"

// INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtOBJECT, txtSBJIP, txtACTION, txtLOC ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
#define	SP_M_LOGInOut	"{call gs_M_LogInOut(%I64d, \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\')}"

#ifdef	__NEW_LOG_2005_05
	// gs_M_ChangeABILITY( 계정, 캐릭터, 스탯타입, 소모포인트, IP, 위치, X좌표, Y좌표 ) 
	#define SP_M_CHAGEABILITY "{call AddChangeAbility(%d, \'%s\', \'%s\', %d, %d, \'%s\', \'%s\', %d, %d)}"
#endif


//-------------------------------------------------------------------------------------------------
#define	NEWLOG_NEW_CHAR			0
#define	NEWLOG_DEL_START_CHAR	1
#define	NEWLOG_DEL_END_CHAR		2

#define	NEWLOG_LOGIN			0
#define	NEWLOG_LOGOUT			1
#define	NEWLOG_CHARBACK			2

#define	NEWLOG_SUCCESS			0
#define	NEWLOG_FAILED			1

#define	NEWLOG_GEMMING			0
#define	NEWLOG_UNGEMMING		1

#define	NEWLOG_CREATE			0
#define	NEWLOG_BREAKUP			1


#define	NEWLOG_QUEST_RECV		0
#define	NEWLOG_QUEST_DONE		1
#define	NEWLOG_QUEST_GIVEUP		2
#define	NEWLOG_QUEST_HASH_DONE	3			// 단순 클라이언트 요청에 의해 수행되는 트리거...


#define	NEWLOG_UPGRADE_FAILED			1
#define	NEWLOG_UPGRADE_SUC_WITH_SKILL	2
#define	NEWLOG_UPGRADE_SUC_FROM_NPC		3

#define	NEWLOG_CLAN_CREATE_START		0	// : 클랜 생성 시작
#define	NEWLOG_CLAN_CREATE_SUCCESS		1	// : 클랜 생성 성공
#define	NEWLOG_CLAN_CREATE_FAILED		2	// : 클랜 생성 실패
#define	NEWLOG_CLAN_DESTROYED			3	// : 클랜 해체
#define	NEWLOG_CLAN_ADD_SKILL_DONE		4	// 클랜 스킬 추가 완료
#define	NEWLOG_CLAN_DEL_SKILL			5	// 클랜 스킬 삭제

#define	NEWLOG_CLAN_JOIN_MEMBER			6	// 클랜 가입
#define	NEWLOG_CLAN_QUIT_MEMBER			7	// 클랜 탈퇴
#define	NEWLOG_CLAN_KICK_MEMBER			8	// 클랜 강퇴
#define	NEWLOG_CLAN_CHANGE_MASTER		9	// 클랜짱 위임
#define	NEWLOG_CLAN_CHANGE_POSITION		10	// 클랜직위 변경

#define	NEWLOG_UNION_JOIN				0
#define	NEWLOG_UNION_CHANGE				1
#define	NEWLOG_UNION_QUIT				2
#define NEWLOG_UNION_ADD_POINT			3
#define	NEWLOG_UNION_USE_POINT			4


#ifdef	__NEW_LOG
	/*
	[AddLevelUpLog]
		@CharID int,
		@CharName nvarchar(30),
		@toLevel smallint,
		@BPoint smallint,
		@SPoint smallint,
		@Location varchar(24),
		@LocX int,
		@LocY int
	*/
	#define	SP_LevelUpLog	"{call AddLevelUpLog(%d,\'%s\',%d,%d,%d,\'%s\',%d,%d)}"


	/*
	[AddSkillLog]
		@CharID int,
		@CharName nvarchar(30),
		@SkillID int,
		@SkillName varchar(24),
		@SkillLevel smallint,
		@SPoint smallint,
		@Location varchar(24),
		@LocX int,
		@LocY int
	*/
	#define	SP_SkillLog		"{call AddSkillLog(%d,\'%s\',%d,\'%s\',%d,%d,\'%s\',%d,%d)}"


	/*
	[AddUpgradeLog]
		@CharID int,
		@CharName nvarchar(30),
		@ItemID varchar(10),
		@ItemName varchar(50),
		@UpLevel smallint,
		@Success tinyint,
		@Location varchar(24),
		@LocX int,
		@LocY int

	*/
	#define	SP_UpgradeLog	"{call AddUpgradeLog(%d,\'%s\',\'%s\',\'%s\',%d,%d,\'%s\',%d,%d)}"

	/*
	[AddGemmingLog]
		@CharID int,
		@CharName nvarchar(30),
		@ItemID varchar(10),
		@ItemName varchar(50),
		@JewelID varchar(10),
		@JewelName varchar(24),
		@Gemming tinyint,
		@Success tinyint,
		@Location varchar(24),
		@LocX int,
		@LocY int
	*/
	#define	SP_GemmingLog	"{call AddGemmingLog(%d,\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%d,%d,\'%s\',%d,%d)}"


	/*
	[AddCreateLog]
		@CharID int,
		@CharName nvarchar(30),
		@ItemID varchar(10),
		@ItemName varchar(50),
		@Stuff1 varchar(24),
		@Stuff2 varchar(24),
		@Stuff3 varchar(24),
		@Stuff4 varchar(24),
		@Making tinyint,
		@Success tinyint,
		@Location varchar(24),
		@LocX int,
		@LocY int
	*/
	#define	SP_CreateLog	"{call AddCreateLog(%d,\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%d,%d,\'%s\',%d,%d)}"


	/*
	[AddLoginLog]
		@Login tinyint,
		@CharName nvarchar(30),
		@Channel tinyint,
		@CharLevel smallint,
		@Money bigint,
		@Location varchar(24),
		@LocX int,
		@LocY int,
		@LoginIP varchar(15)
	*/
	#define	SP_AddLoginLog	"{call AddLoginLog(%d,\'%s\',%d,%d,%I64d,\'%s\',%d,%d,\'%s\')}"


	/*
	[AddQuestLog]
		@CharID int,
		@CharName nvarchar(30),
		@QuestID int,
		@QuestDo tinyint

	*/
	#define	SP_AddQuestLog	"{call AddQuestLog(%d,\'%s\',%d,%d)}"


	/*
	[AddCheatLog] 
		@Account nvarchar(20),
		@CharName nvarchar(30),
		@ChannelNo tinyint,
		@CheatCode varchar(25),
	*/
	#define	SP_AddCheatLog	"{call AddCheatLog(\'%s\',\'%s\',%d,\'%s\')}"


	/*
	[AddItemLog]
		@Action int,
		@SbjAccount nvarchar(20),
		@SbjCharID int,
		@SbjCharName nvarchar(30),
		@ItemID varchar(10),
		@ItemName varchar(50),
		@ItemCount smallint,
		@ItemSN bigint,
		@Money bigint,
		@Location varchar(24),
		@LocX int,
		@LocY int,
		@ObjAccount nvarchar(20),
		@ObjCharID int,
		@ObjCharName nvarchar(30),
		@SbjIP varchar(15),
		@ObjIP varchar(15)
	*/
#ifdef	__NEW_LOG_2005_05
	#define	SP_AddItemLog	"{call AddItemLog(%d,\'%s\',%d,\'%s\',\'%s\',\'%s\',%d,%I64d,%d,%I64d,\'%s\',%d,%d,\'%s\',%d,\'%s\',\'%s\',\'%s\')}"
#else
	#define	SP_AddItemLog	"{call AddItemLog(%d,\'%s\',%d,\'%s\',\'%s\',\'%s\',%d,%I64d,%I64d,\'%s\',%d,%d,\'%s\',%d,\'%s\',\'%s\',\'%s\')}"
#endif

	/*
	[AddCharacterLog]
		@AccountName nvarchar(20),
		@CharName nvarchar(30),
		@DelAdd tinyint
	*/
	#define	SP_AddCharacterLog	"{call AddCharacterLog(\'%s\',\'%s\',%d)}"


	/*
	[AddDieLog]
		@CharName varchar(32),
		@Money bigint,
		@KillPos varchar(24),
		@CharLevel smallint,
		@Exp int,
		@PosX int,
		@PosY int,
		@ObjectName varchar(32)
	*/
	#define	SP_AddDieLog	"{call AddDieLog(\'%s\',%I64d,\'%s\',%d,%d,%d,%d,\'%s\')}"


	/*
	[AddPeriodicCHARLog]
		@CharName varchar(32),
		@Channel tinyint,
		@CharLevel smallint,
		@Money bigint,
		@Exp int,
		@BPoint smallint,
		@SPoint smallint,
		@Location varchar(24),
		@LocX int,
		@LocY int
	*/
	#define	SP_AddPeriodicCHARLog	"{call AddPeriodicCHARLog(\'%s\',%d,%d,%I64d,%d,%d,%d,\'%s\',%d,%d)}"


	/*
	[Success] Column
		0 : 클랜 생성 시작
		1 : 클랜 생성 성공
		2 : 클랜 생성 실패
		3 : 클랜 해체
	[AddClanLog]
		@CharName nvarchar(32),
		@ClanName nvarchar(20),
		@ClanLevel smallint,
		@Point int,
		@Success tinyint,
		@Location varchar(24),
		@LocX int,
		@LocY int
	*/
	#define	SP_AddClanLog			"{call AddClanLog(\'%s\',\'%s\',%d,%d,%d,\'%s\',%d,%d)}"


#ifdef	__NEW_LOG_2005_05
	/*
	[AddUnionLog]
		@iCharID int,
		@szCharName nvarchar(30),
		@nAction smallint,
		@nCurUnion smallint,
		@iCurPoint int,
		@nAfterUnion smallint,
		@iAfterPoint int,
		@szLocation varchar(50),
		@iLocX int,
		@iLocY int
	*/
	#define	SP_AddUnionLog			"{call AddUnionLog(%d,\'%s\',%d,%d,%d,%d,%d,\'%s\',%d,%d)}"
#endif

#endif
//-------------------------------------------------------------------------------------------------




class CClan;

/**
 * \ingroup SHO_GS_LIB
 * \class	CThreadLOG
 * \author	wookSang.Jo
 * \brief	게임 로그를 디비에 기록하는 쓰래드 클래스
 */
class CThreadLOG : public CSqlTHREAD
{
public :
	CStrVAR		m_PosSTR;
	char		*GetLocationSTR(short nZoneNO, tPOINTF &PosCUR )
	{
		char *szZoneName = ZONE_NAME( nZoneNO );
		m_PosSTR.Printf( "%d:%s(%6.0f,%6.0f)", nZoneNO, szZoneName?szZoneName:"null", PosCUR.x, PosCUR.y );
		return m_PosSTR.Get();
	}

	CStrVAR		m_QuerySTR;
	CStrVAR		m_ItemSTR;
	CStrVAR		m_DescSTR;
	//char		*MakeItemDESC( CStrVAR *pStrVAR, tagITEM *pITEM, bool bCreated );
	//char		*GetItemINFO( tagITEM *pITEM, bool bCreated=false );

	CThreadLOG ();
	virtual ~CThreadLOG ();
	void Set_EVENT ()									{	m_pEVENT->SetEvent ();	}
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET )		{	return true;			}
	void Execute ();

public :
	bool				m_bWaiting;
	CCriticalSection	m_csSQL;
	SYSTEMTIME			m_locTime;
	char				m_szDateTime[ 30 ];

	char	   *GetCurDateTimeSTR()
	{
		::GetLocalTime( &m_locTime );

		// date & time format = "0000-00-00 00:00:00"
		sprintf( m_szDateTime, "%04d-%02d-%02d %02d:%02d:%02d", m_locTime.wYear, m_locTime.wMonth, m_locTime.wDay, m_locTime.wHour, m_locTime.wMinute, m_locTime.wSecond);
		return m_szDateTime;
	}
	void Send_gsv_LOG_SQL();


	// 시스템 오류...
	bool When_ERROR ( classUSER *pUSER, char *szFile, int iLine, char *szDesc );
	bool When_SysERR ( char *szFile, int iLine, char *szDesc );

	// 로그인/아웃
	//bool When_LogIN  ( classUSER *pUSER );
	//bool When_LogOUT ( classUSER *pUSER );

	// 케릭 생성/삭제
#ifdef	__NEW_LOG
	// @param:: btDelOrNew: 0:생성, 1:삭제
	bool When_CharacterLOG ( classUSER *pSourAVT, char *szCharNAME, BYTE btDelOrNew );
#else
	bool When_CreateCHAR ( classUSER *pSourAVT, char *szCharNAME );
	bool When_DeleteCHAR ( classUSER *pSourAVT, char *szCharNAME );
#endif

	//bool When_DieBY( CObjCHAR *pKillOBJ, classUSER *pDeadAVT );

	// 레벨업
	//bool When_LevelUP ( classUSER *pSourAVT, int iGetEXP );

	// 치트 관련
	//bool When_CheatCODE		( classUSER *pSourAVT,	char *szCode );

	bool When_ws_CLAN ( char *szName, char *szIP, char *szLogType, CClan *pClan, BYTE btSucType, int iSkillNo=0, int iNewPos=0 );
} ;

#ifdef	__SHO_WS
extern CThreadLOG *g_pThreadLOG;
#endif

//-------------------------------------------------------------------------------------------------
#endif
