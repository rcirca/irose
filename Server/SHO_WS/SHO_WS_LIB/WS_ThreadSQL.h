#ifndef	__WS_THREADSQL_H
#define	__WS_THREADSQL_H
#include "CSqlTHREAD.h"
//-------------------------------------------------------------------------------------------------

#pragma pack (push, 1)
struct tagBasicETC3 {
	tPOINTF m_PosSTART;
	BYTE	m_btCharRACE;
	short	m_nZoneNO;

	short	m_nPartItemIDX[ MAX_BODY_PART	];
	short	m_nRideItemIDX[ MAX_RIDING_PART ];

	tPOINTF	m_PosREVIVE;
	short	m_nReviveZoneNO;

	void Init ()
	{
		m_PosSTART.x = 0;
		m_PosSTART.y = 0;
		m_nZoneNO    = -1;
		::ZeroMemory( m_nPartItemIDX,	sizeof(short)*MAX_BODY_PART		);
		::ZeroMemory( m_nRideItemIDX,	sizeof(short)*MAX_RIDING_PART	);
	}
	void SetPartITEM( short nPartIdx, tagITEM &sITEM )
	{
		m_nPartItemIDX[ nPartIdx ] = sITEM.GetItemNO();
	}
} ;

struct tagBasicETC {
	tPOINTF		m_PosSTART;
	BYTE		m_btCharRACE;
	short		m_nZoneNO;
/*
	short	m_nPartItemIDX[ MAX_BODY_PART	];
	short	m_nRideItemIDX[ MAX_RIDING_PART ];
*/
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
		m_PartITEM[ nPartIdx ].m_nItemNo	 = sITEM.GetItemNO();
		m_PartITEM[ nPartIdx ].m_nGEM_OP	 = sITEM.GetGemNO();
		m_PartITEM[ nPartIdx ].m_cGrade	 = sITEM.GetGrade();
		m_PartITEM[ nPartIdx ].m_bHasSocket= sITEM.m_bHasSocket;
	}
} ;


#pragma pack (pop)


class CWS_Client;
struct tagSqlUSER {
	bool		m_bLogOut;			
	CWS_Client*	m_pUSER;
} ;

class CWS_ThreadSQL : public CSqlTHREAD
{
private:
	bool __fastcall ConvertBasicETC ();

private:
	short				 m_nDefaultDataCNT;
	tagBasicETC			*m_pDefaultBE;
	CInventory			*m_pDefaultINV;
	tagBasicAbility		*m_pDefaultBA;

	tagBasicETC			m_sBE;
	tagBasicINFO		m_sBI;
	tagGrowAbility		m_sGA;
	tagSkillAbility		m_sSA;
//	tagQuestData		m_sQD;
//	CHotICONS			m_HotICON;
	tagBankData			m_sEmptyBANK;


	// inherit virtual function from CSqlTHREAD...
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET );
	void Execute ();


	bool Proc_cli_CHAR_LIST		( tagQueryDATA *pSqlPACKET );
	bool Proc_cli_SELECT_CHAR	( tagQueryDATA *pSqlPACKET );
	bool Proc_cli_CREATE_CHAR	( tagQueryDATA *pSqlPACKET );
	bool Proc_cli_DELETE_CHAR	( tagQueryDATA *pSqlPACKET );

	bool Proc_SAVE_WORLDVAR( sql_ZONE_DATA *pSqlZONE );
	bool Proc_cli_MEMO( tagQueryDATA *pSqlPACKET );

public :
	CWS_ThreadSQL ();
	virtual __fastcall ~CWS_ThreadSQL ();

	void Set_EVENT ()		{	m_pEVENT->SetEvent ();				}

	bool Add_SqlPacketWithACCOUNT (CWS_Client *pUSER, t_PACKET *pPacket);
	bool Add_SqlPacketWithAVATAR  (CWS_Client *pUSER, t_PACKET *pPacket);

	bool Load_WORLDVAR (BYTE *pVarBUFF, short nBuffLEN);
	bool Save_WorldVAR (BYTE *pVarBUFF, short nBuffLEN);
} ;
extern CWS_ThreadSQL *g_pThreadSQL;

//-------------------------------------------------------------------------------------------------
#endif
