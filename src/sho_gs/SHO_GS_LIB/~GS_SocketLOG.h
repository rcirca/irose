#ifndef	__GS_SOCKETLOG_H
#define	__GS_SOCKETLOG_H
#include "classSYNCOBJ.h"
#include "classTIME.h"
#include "classMYSQL.h"
#include "classODBC.h"
//-------------------------------------------------------------------------------------------------

// #define	__CDB_SOCKET


#ifdef	__CDB_SOCKET
	#include "CDB_Socket.h"
#endif

class GS_logSOCKET
#ifdef	__CDB_SOCKET
					: public CDB_Socket
#endif
{
public :
	GS_logSOCKET (bool bUseMySQL);
	~GS_logSOCKET ()	{	;	}

private :
#ifdef	__CDB_SOCKET
		char *GetServerNAME ()
		{
			return CLIB_GameSRV::GetInstance()->GetServerName();
		}
#endif

	CStrVAR		m_PosSTR;
	char		*GetLocationSTR(short nZoneNO, tPOINTF &PosCUR )
	{
		m_PosSTR.Printf( "%d:%s(%6.0f,%6.0f)", nZoneNO, ZONE_NAME( nZoneNO ), PosCUR.x, PosCUR.y );
		return m_PosSTR.Get();
	}

	CStrVAR		m_QuerySTR;
	CStrVAR		m_ItemSTR;
	CStrVAR		m_DescSTR;
	char		*MakeItemDESC( CStrVAR *pStrVAR, tagITEM *pITEM, bool bCreated );
	char		*GetItemINFO( tagITEM *pITEM, bool bCreated=false );

public :

#ifndef	__CDB_SOCKET
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
		bool Connect ()		{	return true;	}
		void Disconnect ()	{	;				}

		void Send_gsv_LOG_SQL();
#endif
	// 시스템 오류...
	bool When_ERROR ( classUSER *pUSER, char *szFile, int iLine, char *szDesc );

	// 로그인/아웃
	bool When_LogIN  ( classUSER *pUSER );
	bool When_LogOUT ( classUSER *pUSER );

	// 케릭 생성/삭제
	bool When_CreateCHAR ( classUSER *pSourAVT, char *szCharNAME );
	bool When_DeleteCHAR ( classUSER *pSourAVT, char *szCharNAME );

	// 줍거나 버리기
	bool When_PickITEM		( classUSER *pSourAVT, CObjITEM *pItemOBJ );
	bool When_DropITEM		( classUSER *pSourAVT, CObjITEM *pItemOBJ );
	bool When_DeletedITEM	( CObjITEM *pItemOBJ );

	// 팔거나 주기
	bool When_Sell2STORE ( classUSER *pSourAVT, tagITEM *pITEM, CObjCHAR  *pDestCHR );
	bool When_P2PTRADE   ( classUSER *pSourAVT, tagITEM *pITEM, classUSER *pDestAVT, DWORD dwTradeMoney );

	bool When_GiveITEM ( classUSER *pSourAVT, tagITEM  *pITEM, classUSER *pDestAVT );
	bool When_RecvITEM ( classUSER *pSourAVT, tagITEM  *pITEM, classUSER *pDestAVT );

	// 창고에 보관 찾기
	bool When_DEPOSIT  ( classUSER *pSourAVT, tagITEM  *pITEM );
	bool When_WithDRAW ( classUSER *pSourAVT, tagITEM  *pITEM );

	// 제조 관련
	bool When_CreatedITEM	( classUSER *pSourAVT, tagITEM *pITEM );
	bool When_DestroyedITEM ( classUSER *pSourAVT, tagITEM *pITEM );

	bool When_DieBY( CObjCHAR *pKillOBJ, classUSER *pDeadAVT );

	// 레벨업
	bool When_LevelUP ( classUSER *pSourAVT, int iGetEXP );

	// 퀘스트...
	bool When_RecvQUEST		( classUSER *pSourAVT, int iQuestIDX );
	bool When_DoneQUEST		( classUSER *pSourAVT, t_HASHKEY hQuestHASH );
	bool When_GiveUpQUEST	( classUSER *pSourAVT, int iQuestIDX );

	// 스킬...
	bool When_LearnSKILL	( classUSER *pSourAVT,	short nSkillIDX );
	bool When_LevelUpSKILL	( classUSER *pSourAVT,	short nSkillIDX );

	// 재밍 관련
	bool When_GemmingITEM	( classUSER *pSourAVT,	tagITEM *pEquipITEM, tagITEM *pJewelITEM );
	bool When_UngemmingITEM	( classUSER *pSourAVT,	tagITEM *pEquipITEM, tagITEM *pJewelITEM );
	bool When_UpgradeITEM	( classUSER *pSourAVT,  tagITEM *pEquipITEM, BYTE btBeforeGrade, bool bSuccess );
	bool When_BreakupITEM	( classUSER *pSourAVT,	tagITEM *pITEM ); 

	// 치트 관련
	bool When_CheatCODE		( classUSER *pSourAVT,	char *szCode );
} ;
extern GS_logSOCKET	*g_pSockLOG;

//-------------------------------------------------------------------------------------------------
#endif
