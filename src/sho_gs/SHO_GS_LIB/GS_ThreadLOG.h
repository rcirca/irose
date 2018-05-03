/**
 * \ingroup SHO_GS
 * \file	GS_ThreadLOG.h
 * \brief	게임 로그를 로그디비에 기록하는 쓰레드 클래스
 */
#ifndef	__GS_THREADLOG_H
#define	__GS_THREADLOG_H
//#include "classSYNCOBJ.h"
//#include "classTIME.h"
//#include "CSqlTHREAD.h"
#include "CThreadLOG.h"
//-------------------------------------------------------------------------------------------------

enum {
	LIA_BANK		= 0,
	LIA_DEPOSIT,
	LIA_WITHDRAW,
	LIA_SELL2NPC,
	LIA_BUYFROMNPC,
	LIA_P2PTRADE,
	LIA_GIVE		= 6,
	LIA_RECV		= 7,
	LIA_DROP,
	LIA_PICK,
	LIA_TIMEOUT,
	LIA_CHEATECODE,
	LIA_BUYFROMUNION,
} ;

/**
 * \ingroup SHO_GS_LIB
 * \class	GS_CThreadLOG
 * \author	wookSang.Jo
 * \brief	로그 디비와 연결되는 쓰래드 클래스
 *			기본 로그를 처리하는 CThreadLOG를 상속 받음
 */
class GS_CThreadLOG : public CThreadLOG	// CSqlTHREAD
{
public :
/*
	CStrVAR		m_PosSTR;
	char		*GetLocationSTR(short nZoneNO, tPOINTF &PosCUR )
	{
		m_PosSTR.Printf( "%d:%s(%6.0f,%6.0f)", nZoneNO, ZONE_NAME( nZoneNO ), PosCUR.x, PosCUR.y );
		return m_PosSTR.Get();
	}

	CStrVAR		m_QuerySTR;
	CStrVAR		m_ItemSTR;
	CStrVAR		m_DescSTR;
*/
	char		*MakeItemDESC( CStrVAR *pStrVAR, tagITEM *pITEM, int iItmePrice, int iDupCnt, int iMoneyAmout, DWORD dwTradeMoney, int iTradeCnt );
	char		*GetItemINFO( tagITEM *pITEM, int iItmePrice=CHECK_ITEM_LOG_PRICE, int iDupCnt=CHECK_ITEM_LOG_CNT, int iMoneyAmout=CHECK_ITEM_MONEY, DWORD dwTradeMoney=0, int iTradeCnt=0 );
/*
	GS_CThreadLOG ();
	virtual ~GS_CThreadLOG ();
	void Set_EVENT ()									{	m_pEVENT->SetEvent ();	}
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET )		{	return true;			}
	void Execute ();
*/
public :
/*
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
*/
	// 로그인/아웃
#ifdef	__NEW_LOG
	// @param btTYPE : 0:login, 1:logout, 2:char backup
	bool When_LogInOrOut ( classUSER *pUSER, BYTE btType );
#else
	bool When_LogIN  ( classUSER *pUSER );
	bool When_LogOUT ( classUSER *pUSER );
#endif

	bool When_DeletedITEM	( CObjITEM *pItemOBJ );

#ifdef	__NEW_LOG
	bool Check_ItemLogFilter( tagITEM *pItem, short nQuantity, __int64 biZuly );
	bool When_TagItemLOG( int iAction, classUSER *pSourAVT, tagITEM *pItem, short nQuantity=1, __int64 biTradeZuly=0, classUSER *pDestAVT=NULL, bool bForce=false);
	bool When_ObjItemLOG( int iAction, classUSER *pSourAVT, CObjITEM *pItemOBJ);
#else
	// 줍거나 버리기
	bool When_PickITEM	( classUSER *pSourAVT, CObjITEM *pItemOBJ );
	bool When_DropITEM	( classUSER *pSourAVT, CObjITEM *pItemOBJ );

	// 팔거나 주기
	bool When_NpcTRADE  ( classUSER *pSourAVT, tagITEM *pITEM, CObjCHAR  *pDestCHR, char *szLogType, int iPriceTOT, int iDupCnt );
	bool When_P2PTRADE	( classUSER *pSourAVT, tagITEM *pITEM, classUSER *pDestAVT, DWORD dwTradeMoney );

	bool When_GiveITEM ( classUSER *pSourAVT, tagITEM  *pITEM, classUSER *pDestAVT, short nTradeSlotNO );
	bool When_RecvITEM ( classUSER *pSourAVT, tagITEM  *pITEM, classUSER *pDestAVT, short nTradeSlotNO );

	// 창고에 보관 찾기
	bool When_DEPOSIT  ( classUSER *pSourAVT, tagITEM  *pITEM );
	bool When_WithDRAW ( classUSER *pSourAVT, tagITEM  *pITEM );
#endif

	// 제조 관련
#ifdef	__NEW_LOG
	// @param:: btMakeOrBreak: 0:제조, 1:분리
	//          btSucOrFail: 0:성공, 1:실패
	bool When_CreateOrDestroyITEM ( classUSER *pSourAVT, tagITEM *pOutItem, tagITEM *pUseItem, short nUseCNT, BYTE btMakeOrBreak, BYTE btSucOrFail );
#else
	bool When_CreatedITEM	( classUSER *pSourAVT, tagITEM *pITEM );
	bool When_DestroyedITEM ( classUSER *pSourAVT, tagITEM *pITEM );
#endif

	bool When_DieBY( CObjCHAR *pKillOBJ, classUSER *pDeadAVT );

	// 레벨업
	bool When_LevelUP ( classUSER *pSourAVT, int iGetEXP );

	// 퀘스트...
#ifdef	__NEW_LOG
	// @param:: btLogType: 0:부여, 1:수행, 2:포기
	bool When_QuestLOG ( classUSER *pSourAVT, int iQuestIDX, BYTE btLogType );
#else
	bool When_RecvQUEST		( classUSER *pSourAVT, int iQuestIDX );
	bool When_DoneQUEST		( classUSER *pSourAVT, int iQuestIDX );
	bool When_GiveUpQUEST	( classUSER *pSourAVT, int iQuestIDX );
	bool When_ChangedQUEST	( classUSER *pSourAVT, int iOldQuestIDX, int iNewQuestIDX );
#endif

	// 스킬...
	bool When_LearnSKILL	( classUSER *pSourAVT,	short nSkillIDX );
#ifndef	__NEW_LOG
	bool When_LevelUpSKILL	( classUSER *pSourAVT,	short nSkillIDX );
#endif

	// 재밍 관련
	bool When_UpgradeITEM	( classUSER *pSourAVT,  tagITEM *pEquipITEM, BYTE btBeforeGrade, BYTE btResult );
	bool When_GemmingITEM	( classUSER *pSourAVT,	tagITEM *pEquipITEM, tagITEM *pJewelITEM, BYTE btGemming, BYTE btSuccess );
#ifndef	__NEW_LOG
	bool When_UngemmingITEM	( classUSER *pSourAVT,	tagITEM *pEquipITEM, tagITEM *pJewelITEM );
	bool When_BreakupITEM	( classUSER *pSourAVT,	tagITEM *pITEM ); 
#endif

	// 조합 관련
	bool When_ChangeUNION	( classUSER *pSourAVT, int iBeforeUnion, int iNewUnion );
	bool When_PointUNION	( classUSER *pSourAVT, int iCurUnion, int iCurPoint, int iNewPoint );

	// 치트 관련
	bool When_CheatCODE		( classUSER *pSourAVT,	char *szCode );

	bool When_ItemHACKING	( classUSER *pSourAVT, tagITEM *pHackITEM, char *szLogType );

	// 백업 로그.
	bool When_BackUP		( classUSER *pSourAVT,  char *szBackUpType );

	// 클랜 로그..
	bool When_gs_CLAN		( classUSER *pSourAVT, char *szLogType, BYTE btSucType );

	bool When_ChangeABILITY	( classUSER * pSourAVT , BYTE btAbilityTYPE, short nUsedBPOINT );


} ;
extern GS_CThreadLOG *g_pThreadLOG;

//-------------------------------------------------------------------------------------------------
#endif
