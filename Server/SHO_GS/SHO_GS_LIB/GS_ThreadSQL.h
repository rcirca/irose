/**
 * \ingroup SHO_GS
 * \file	GS_ThreadSQL.h
 * \brief	게임 디비서버와 연결,통신하는 클래스
 */
#ifndef	__GS_THREADSQL_H
#define	__GS_THREADSQL_H
#include "CSqlTHREAD.h"
//-------------------------------------------------------------------------------------------------

#pragma pack (push, 1)

#define DATA_VER_2		2 // 배틀카트

struct tagBasicETC {
	tPOINTF		m_PosSTART;
	BYTE		m_btCharRACE;
	short		m_nZoneNO;

	tPOINTF		m_PosREVIVE;
	short		m_nReviveZoneNO;

	tagPartITEM	m_PartITEM[ MAX_BODY_PART	];
	tagPartITEM	m_RideITEM[ MAX_RIDING_PART	];

	BYTE		m_btCharSlotNO;

	void Init ()
	{
		m_btCharSlotNO = 0;
		m_PosSTART.x = 0;
		m_PosSTART.y = 0;
		m_nZoneNO    = -1;

		::ZeroMemory( m_PartITEM,	sizeof(tagPartITEM)*MAX_BODY_PART	);
		::ZeroMemory( m_RideITEM,	sizeof(tagPartITEM)*MAX_RIDING_PART	);
	}
	void SetPartITEM( short nPartIdx, tagITEM &sITEM )
	{
		m_PartITEM[ nPartIdx ].m_nItemNo	= sITEM.GetItemNO();
		m_PartITEM[ nPartIdx ].m_nGEM_OP	= sITEM.GetGemNO();
		m_PartITEM[ nPartIdx ].m_cGrade		= sITEM.GetGrade();
		m_PartITEM[ nPartIdx ].m_bHasSocket	= sITEM.m_bHasSocket;
	}
} ;

#pragma pack (pop)

struct tagSqlUSER {
	BYTE		m_btLogOutMODE;
	classUSER*	m_pUSER;
} ;
#ifdef FRAROSE
#define	MAX_GRAW_ABILITY_BUFF	394
#else
#define	MAX_GRAW_ABILITY_BUFF	384
#endif
struct tagGrowA_BUFF {
	union {
		tagGrowAbility	m_sGA;
		BYTE			m_btBUFF[ MAX_GRAW_ABILITY_BUFF ];
	} ;
	void Init ()
	{
		::ZeroMemory( m_btBUFF, MAX_GRAW_ABILITY_BUFF );
		m_sGA.Init ();
	}
} ;

/**
 * \ingroup SHO_GS_LIB
 * \class	GS_CThreadSQL
 * \author	wookSang.Jo
 * \brief	케릭터 데이타를 디비서버에 읽고 쓰는 쓰래드 클래스
 *			디비연결및 처리 클래스인 CSqlTHREAD를 상속 받음
 */
class GS_CThreadSQL : public CSqlTHREAD
{
private:
	bool __fastcall ConvertBasicETC ();
	bool	m_bWaiting;

private:
	CCriticalSection			m_csUserLIST;
	classDLLIST< tagSqlUSER >	m_AddUserLIST;
	classDLLIST< tagSqlUSER >	m_RunUserLIST;
//	classDLLIST< classUSER >	m_AddUserLIST;
//	classDLLIST< classUSER >	m_RunUserLIST;

	short				 m_nDefaultDataCNT;
	tagBasicETC			*m_pDefaultBE;
	CInventory			*m_pDefaultINV;
	tagBasicAbility		*m_pDefaultBA;

	tagBasicETC			m_sBE;
	tagBasicINFO		m_sBI;
	tagGrowA_BUFF		m_sGB;
	tagSkillAbility		m_sSA;
	tagQuestData		m_sQD;
	CHotICONS			m_HotICON;
	tagBankData			m_sEmptyBANK;

	short				m_nTmpWEIGHT;
	CStrVAR				m_TmpSTR;
	DWORD				m_dwCurTIME;

	// inherit virtual function from CSqlTHREAD...
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET );

	bool Proc_LOAD_ZONE_DATA	( int iZoneNO );
	bool Proc_SAVE_ZONE_DATA	( int iZoneNO, sql_ZONE_DATA *pSqlZONE );

	bool Proc_LOAD_OBJVAR( tagQueryDATA *pSqlPACKET );
	bool Proc_SAVE_OBJVAR( tagQueryDATA *pSqlPACKET );

	bool Proc_SAVE_WORLDVAR( sql_ZONE_DATA *pSqlZONE );

	bool Proc_cli_MEMO( tagQueryDATA *pSqlPACKET );

	bool Proc_cli_CHAR_LIST		( tagQueryDATA *pSqlPACKET );
	bool Proc_cli_SELECT_CHAR	( tagQueryDATA *pSqlPACKET );
	bool Proc_cli_CREATE_CHAR	( tagQueryDATA *pSqlPACKET );
	bool Proc_cli_DELETE_CHAR	( tagQueryDATA *pSqlPACKET );
	bool Proc_cli_BANK_LIST_REQ	( tagQueryDATA *pSqlPACKET );

//	bool Proc_cli_CLAN_COMMAND	( tagQueryDATA *pSqlPACKET );
	bool Proc_cli_MALL_ITEM_REQ	( tagQueryDATA *pSqlPACKET );

	void Clear_LoginTABLE ();
	void Add_LoginACCOUNT (char *szAccount);
	void Sub_LoginACCOUNT (char *szAccount);

	void Execute ();

public :
	GS_CThreadSQL ();
	virtual ~GS_CThreadSQL ();

	void Set_EVENT ()		{	m_pEVENT->SetEvent ();				}

	bool IO_ZoneDATA (CZoneTHREAD *pZONE, bool bSave=false);
	bool IO_NpcObjDATA	(CObjNPC *pObjNPC, bool bSave=false);
	bool IO_EventObjDATA(CObjEVENT *pObjEVENT, bool bSave=false);
	bool Save_WorldVAR ( BYTE *pVarBUFF, short nBuffLEN );
	bool Load_WORLDVAR ( BYTE *pVarBUFF, short nBuffLEN );

	bool Add_SqlPacketWithACCOUNT (classUSER *pUSER, t_PACKET *pPacket);
	bool Add_SqlPacketWithAVATAR  (classUSER *pUSER, t_PACKET *pPacket);
	bool Add_BackUpUSER (classUSER *pUSER, BYTE btLogOutMODE=0);
	bool Sql_TEST ();

	bool IsWaiting()		{	return m_bWaiting;						}
	int  WaitUserCNT()		{	return m_AddUserLIST.GetNodeCount();	}

	bool UpdateUserRECORD (classUSER *pUSER);
} ;
extern GS_CThreadSQL *g_pThreadSQL;

//-------------------------------------------------------------------------------------------------
#endif