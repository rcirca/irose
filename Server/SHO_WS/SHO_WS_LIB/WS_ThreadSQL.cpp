
#include "stdAFX.h"
#include "IO_STB.h"
#include "CWS_Client.h"
#include "WS_ThreadSQL.h"
#include "WS_ZoneLIST.h"
#include "CThreadLOG.h"
#include "CThreadGUILD.h"

#ifdef __EUROPE // Oct. 6 2005 추가 (권형근)
	#define	MAX_CHAR_PER_USER			3
#else
	#define	MAX_CHAR_PER_USER			5
#endif

//#define	MAX_CREATE_CHAR_PER_USER	3
#define	MAX_CREATE_CHAR_PER_USER		MAX_CHAR_PER_USER
#define	MAX_AVATAR_NAME	20

#define	DELETE_CHAR_WAIT_TIME		(60*60)		//	60분

#define DATA_VER_2					2

enum AVTTBL_COL_IDX {
	AVTTBL_CHARID = 0,
	AVTTBL_ACCOUNT,
	AVTTBL_NAME = 2,
	AVTTBL_LEVEL,
	AVTTBL_MONEY,
	AVTTBL_RIGHT,
	AVTTBL_BASIC_ETC	= 6,
	AVTTBL_BASIC_INFO,
	AVTTBL_BASIC_ABILITY,
	AVTTBL_GROW_ABILITY,
	AVTTBL_SKILL_ABILITY,
	AVTTBL_QUEST		= 11,
	AVTTBL_INVENTORY,
	AVTTBL_HOTICON,
	AVTTBL_DELETE_TIME,
	AVTTBL_WISHLIST,
	AVTTBL_OPTION		= 16,
	AVTTBL_JOB_NO,
	AVTTBL_REG_TIME,
	AVTTBL_PARTY_IDX,
	AVTTBL_ITEM_SN,
	AVTTBL_DATA_VER
} ;


//-------------------------------------------------------------------------------------------------
CWS_ThreadSQL::CWS_ThreadSQL () : CSqlTHREAD( true )
{
	COMPILE_TIME_ASSERT( sizeof( tagGrowAbility )	<= 384	);
#ifdef __KCHS_BATTLECART__ 
	COMPILE_TIME_ASSERT( sizeof( tagGrowAbility )	== 383	);
	COMPILE_TIME_ASSERT( sizeof( CInventory )		== (140*14+8) );	// 1954
	COMPILE_TIME_ASSERT( MAX_RIDING_PART			== 5 );
#else
	COMPILE_TIME_ASSERT( sizeof( tagGrowAbility )	== 383	);
	COMPILE_TIME_ASSERT( sizeof( CInventory )		== (139*14+8) );	// 1954
	COMPILE_TIME_ASSERT( MAX_RIDING_PART			== 4 );
#endif

	m_sGA.Init ();
	m_sSA.Init ();
//	m_sQD.Init ();
//	m_HotICON.Init ();
	m_sEmptyBANK.Init ();

	m_pDefaultBE = NULL;
	m_pDefaultINV = NULL;

	m_nDefaultDataCNT = g_TblAVATAR.m_nDataCnt;
	if ( m_nDefaultDataCNT > 0 ) {
		m_pDefaultBE  = new tagBasicETC		[ m_nDefaultDataCNT ];
		m_pDefaultINV = new CInventory		[ m_nDefaultDataCNT ];
		m_pDefaultBA  = new tagBasicAbility	[ m_nDefaultDataCNT ];

		short nR, nJ;
		for (nR=0; nR<m_nDefaultDataCNT; nR++) {
			m_pDefaultBA[ nR ].m_nSTR	= AVATAR_STR( nR );
			m_pDefaultBA[ nR ].m_nDEX	= AVATAR_DEX( nR );
			m_pDefaultBA[ nR ].m_nINT	= AVATAR_INT( nR );
			m_pDefaultBA[ nR ].m_nCON	= AVATAR_CON( nR );
			m_pDefaultBA[ nR ].m_nCHARM = AVATAR_CHARM( nR );
			m_pDefaultBA[ nR ].m_nSENSE = AVATAR_SENSE( nR );

			m_pDefaultBE[ nR ].Init ();
			m_pDefaultBE[ nR ].m_nZoneNO = AVATAR_ZONE( nR );

			_ASSERT( m_pDefaultBE[ nR ].m_nZoneNO > 0 );

			m_pDefaultINV[ nR ].Clear ();
			m_pDefaultINV[ nR ].m_i64Money = AVATAR_MONEY( nR );

			// 초기 장작 아이템...
			m_pDefaultINV[ nR ].SetInventory( EQUIP_IDX_FACE_ITEM,	AVATAR_FACEITEM(nR) );
			m_pDefaultINV[ nR ].SetInventory( EQUIP_IDX_HELMET,		AVATAR_HELMET(nR)	);
			m_pDefaultINV[ nR ].SetInventory( EQUIP_IDX_ARMOR,		AVATAR_ARMOR(nR)	);
			m_pDefaultINV[ nR ].SetInventory( EQUIP_IDX_KNAPSACK,	AVATAR_BACKITEM(nR) );
			m_pDefaultINV[ nR ].SetInventory( EQUIP_IDX_GAUNTLET,	AVATAR_GAUNTLET(nR) );
			m_pDefaultINV[ nR ].SetInventory( EQUIP_IDX_BOOTS,		AVATAR_BOOTS(nR)	);
			m_pDefaultINV[ nR ].SetInventory( EQUIP_IDX_WEAPON_R,	AVATAR_WEAPON(nR)	);
			m_pDefaultINV[ nR ].SetInventory( EQUIP_IDX_WEAPON_L,	AVATAR_SUBWPN(nR)	);

			// 초기 장비 아이템
			for (nJ=0; nJ<10; nJ++)
				m_pDefaultINV[ nR ].SetInventory( ( INV_WEAPON * INVENTORY_PAGE_SIZE ) + nJ+MAX_EQUIP_IDX,	AVATAR_ITEM_WEAPON(nR,nJ) );

			// 초기 소모 아이템
			for (nJ=0; nJ<5; nJ++) {
				m_pDefaultINV[ nR ].SetInventory( ( INV_USE * INVENTORY_PAGE_SIZE ) + nJ+MAX_EQUIP_IDX, AVATAR_ITEM_USE(nR,nJ), AVATAR_ITEM_USECNT(nR,nJ) );
			}

			// 초기 기타 아이템
			for (nJ=0; nJ<5; nJ++) {
				m_pDefaultINV[ nR ].SetInventory( ( INV_ETC * INVENTORY_PAGE_SIZE ) + nJ+MAX_EQUIP_IDX, AVATAR_ITEM_ETC(nR,nJ), AVATAR_ITEM_ETCCNT(nR,nJ) );
			}

		//	m_pDefaultBE[ nR ].m_nPartItemIDX[ BODY_PART_FACE		] = m_pDefaultINV[ nR ].m_ItemEQUIP[ nI ].m_nItemNo;
		//	m_pDefaultBE[ nR ].m_nPartItemIDX[ BODY_PART_HAIR		] = m_pDefaultINV[ nR ].m_ItemEQUIP[ nI ].m_nItemNo;
			m_pDefaultBE[ nR ].SetPartITEM( BODY_PART_HELMET	, m_pDefaultINV[ nR ].m_ItemEQUIP[ EQUIP_IDX_HELMET		]  );
			m_pDefaultBE[ nR ].SetPartITEM( BODY_PART_ARMOR		, m_pDefaultINV[ nR ].m_ItemEQUIP[ EQUIP_IDX_ARMOR		]  );
			m_pDefaultBE[ nR ].SetPartITEM( BODY_PART_GAUNTLET	, m_pDefaultINV[ nR ].m_ItemEQUIP[ EQUIP_IDX_GAUNTLET	]  );
			m_pDefaultBE[ nR ].SetPartITEM( BODY_PART_BOOTS		, m_pDefaultINV[ nR ].m_ItemEQUIP[ EQUIP_IDX_BOOTS		]  );
			m_pDefaultBE[ nR ].SetPartITEM( BODY_PART_FACE_ITEM	, m_pDefaultINV[ nR ].m_ItemEQUIP[ EQUIP_IDX_FACE_ITEM	]  );
			m_pDefaultBE[ nR ].SetPartITEM( BODY_PART_KNAPSACK	, m_pDefaultINV[ nR ].m_ItemEQUIP[ EQUIP_IDX_KNAPSACK	]  );
			m_pDefaultBE[ nR ].SetPartITEM( BODY_PART_WEAPON_R	, m_pDefaultINV[ nR ].m_ItemEQUIP[ EQUIP_IDX_WEAPON_R	]  );
			m_pDefaultBE[ nR ].SetPartITEM( BODY_PART_WEAPON_L	, m_pDefaultINV[ nR ].m_ItemEQUIP[ EQUIP_IDX_WEAPON_L	]  );
		}
	}
}
__fastcall CWS_ThreadSQL::~CWS_ThreadSQL()
{
	SAFE_DELETE_ARRAY( m_pDefaultBA );
	SAFE_DELETE_ARRAY( m_pDefaultBE );
	SAFE_DELETE_ARRAY( m_pDefaultINV );
}



//-------------------------------------------------------------------------------------------------
bool __fastcall CWS_ThreadSQL::ConvertBasicETC ()
{
/*
	short nI;
	tagBasicETC3 *pOldBE;
	tagBasicETC   sNewBE;
	int iCharID, iRecCNT=0;

	while( true ) 
	{
		this->m_pSQL->MakeQuery( "SELECT TOP 1 intCharID, binBasicE FROM tblGS_AVATAR WHERE intDataVER=0 ", //and txtACCOUNT=\'icarus3\'", 
								MQ_PARAM_END);
		if ( !this->m_pSQL->QuerySQLBuffer() ) {
			g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
			return false;
		}

		if ( !this->m_pSQL->GetNextRECORD() ) {
			return false;
		}

		iCharID = this->m_pSQL->GetInteger( 0 );
		pOldBE = (tagBasicETC3*)this->m_pSQL->GetDataPTR( 1 );

		assert( iCharID && pOldBE );

		sNewBE.Init ();
		sNewBE.m_btCharRACE		= pOldBE->m_btCharRACE;
		sNewBE.m_nReviveZoneNO	= pOldBE->m_nReviveZoneNO;

		sNewBE.m_PosSTART		= pOldBE->m_PosSTART	;
		sNewBE.m_btCharRACE		= pOldBE->m_btCharRACE	;
		sNewBE.m_nZoneNO		= pOldBE->m_nZoneNO	;
		sNewBE.m_PosREVIVE		= pOldBE->m_PosREVIVE	;
		sNewBE.m_nReviveZoneNO	= pOldBE->m_nReviveZoneNO;

		for (nI=0; nI<MAX_BODY_PART; nI++) {
			sNewBE.m_PartITEM[ nI ].m_nItemNo = pOldBE->m_nPartItemIDX[ nI ];
		}
		for (nI=0; nI<MAX_RIDING_PART; nI++) {
			sNewBE.m_RideITEM[ nI ].m_nItemNo = pOldBE->m_nRideItemIDX[ nI ];
		}

		this->m_pSQL->BindPARAM( 1, (BYTE*)&sNewBE,		sizeof( tagBasicETC )	);
		this->m_pSQL->MakeQuery( "UPDATE tblGS_AVATAR SET binBasicE=",
														MQ_PARAM_BINDIDX,	1,
					MQ_PARAM_ADDSTR, ",intDataVER=",	MQ_PARAM_INT,		2,
					MQ_PARAM_ADDSTR, "WHERE intCharID=",MQ_PARAM_INT,	iCharID,
														MQ_PARAM_END );
		if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
			// 고치기 실패 !!!
			// log ...
			g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE binBasicE: %d, \n", iCharID, m_pSQL->GetERROR() );
		} else
			iRecCNT ++;
	}

	g_LOG.CS_ODS(LOG_NORMAL, "Complete ConvertBasicETC, %d records\n", iRecCNT);
*/
	return true;
}


//-------------------------------------------------------------------------------------------------
void CWS_ThreadSQL::Execute ()
{
	this->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );	// Priority 1 point above the priority class

	CDLList< tagQueryDATA >::tagNODE *pSqlNODE;

	g_LOG.CS_ODS( 0xffff, ">  > >> CWS_ThreadSQL::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

	this->ConvertBasicETC();

    while( TRUE ) {
		if ( !this->Terminated ) {
			m_pEVENT->WaitFor( INFINITE );
		} else {
			int iReaminCNT;
			this->m_CS.Lock ();
				iReaminCNT = m_AddPACKET.GetNodeCount();
			this->m_CS.Unlock ();

			if ( iReaminCNT <= 0 )
				break;
		}

		this->m_CS.Lock ();
		m_RunPACKET.AppendNodeList( &m_AddPACKET );
		m_AddPACKET.Init ();
		m_pEVENT->ResetEvent ();
		this->m_CS.Unlock ();

		for( pSqlNODE = m_RunPACKET.GetHeadNode(); pSqlNODE; ) {
			if ( Run_SqlPACKET( &pSqlNODE->m_VALUE ) )
				pSqlNODE = this->Del_SqlPACKET( pSqlNODE );
			else
				pSqlNODE = m_RunPACKET.GetNextNode ( pSqlNODE );
		}
	}

	int iCnt = m_AddPACKET.GetNodeCount();
	_ASSERT( iCnt == 0 );

	g_LOG.CS_ODS( 0xffff, "<<<< CWS_ThreadSQL::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}
//-------------------------------------------------------------------------------------------------
bool CWS_ThreadSQL::Add_SqlPacketWithACCOUNT (CWS_Client *pUSER, t_PACKET *pPacket)
{
	return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX, pUSER->Get_ACCOUNT(), (BYTE*)pPacket, pPacket->m_HEADER.m_nSize);
}
bool CWS_ThreadSQL::Add_SqlPacketWithAVATAR (CWS_Client *pUSER, t_PACKET *pPacket)
{
	return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX, pUSER->Get_NAME(), (BYTE*)pPacket, pPacket->m_HEADER.m_nSize);
}

//-------------------------------------------------------------------------------------------------
bool CWS_ThreadSQL::Run_SqlPACKET( tagQueryDATA *pSqlPACKET )
{
	switch( pSqlPACKET->m_pPacket->m_wType ) {
		case CLI_CHAR_LIST :
			Proc_cli_CHAR_LIST	( pSqlPACKET );
			break;

		case CLI_SELECT_CHAR :
			Proc_cli_SELECT_CHAR( pSqlPACKET );
			break;

		case CLI_CREATE_CHAR :
			Proc_cli_CREATE_CHAR( pSqlPACKET );
			break;

		case CLI_DELETE_CHAR :
			Proc_cli_DELETE_CHAR( pSqlPACKET );
			break;

		case CLI_MEMO :
			Proc_cli_MEMO( pSqlPACKET );
			break;

		case SQL_ZONE_DATA :
		{
			sql_ZONE_DATA *pSqlZONE = (sql_ZONE_DATA*)pSqlPACKET->m_pPacket;
			switch( pSqlZONE->m_btDataTYPE ) {
				case SQL_ZONE_DATA_WORLDVAR_SAVE :
					Proc_SAVE_WORLDVAR( pSqlZONE );
					break;
			}
			break;
		}

		default :
			g_LOG.CS_ODS( 0xffff, "Undefined sql packet Type: %x, Size: %d \n", pSqlPACKET->m_pPacket->m_wType, pSqlPACKET->m_pPacket->m_nSize);
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct tagDelCHAR {
	DWORD	m_dwDBID;
	CStrVAR	m_Name;
} ;
bool CWS_ThreadSQL::Proc_cli_CHAR_LIST( tagQueryDATA *pSqlPACKET )
{
/*
	//주의 !!! 프로지져를 쓸려면 프로시져에 intCharID를 추가해야 한다.
	// if ( !this->m_pSQL->QuerySQL( "{call ws_GetCharLIST(\'%s\')}", pSqlPACKET->m_Name.Get() ) ) {
	this->m_pSQL->MakeQuery( "SELECT txtNAME, binBasicE, binBasicI, binGrowA, dwDelTIME, intCharID FROM tblGS_AVATAR WHERE txtACCOUNT=", 
							MQ_PARAM_STR, pSqlPACKET->m_Name.Get(), 
							MQ_PARAM_END);
*/
	this->m_pSQL->MakeQuery( "SELECT txtNAME, binBasicE, binBasicI, binGrowA, dwDelTIME, intDataVER FROM tblGS_AVATAR WHERE txtACCOUNT=", 
							MQ_PARAM_STR, pSqlPACKET->m_Name.Get(), 
							MQ_PARAM_END);

	if ( !this->m_pSQL->QuerySQLBuffer() ) {
		// ???
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		return false;
	}
	if ( !this->m_pSQL->QuerySQLBuffer() ) {
		// ???
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		return false;
	}

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = WSV_CHAR_LIST;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_CHAR_LIST );
	pCPacket->m_wsv_CHAR_LIST.m_btCharCNT = 0;

	if ( this->m_pSQL->GetNextRECORD() ) {
		tagBasicINFO	*pBI;
		tagBasicETC		*pBE;
		tagGrowAbility  *pGA;
		tagCHARINFO sCHAR;
		char *szCharName;
		short nC = 0, nPlatinumCharCNT=0;
		CSLList< tagDelCHAR >  DelList;
		CSLList< tagDelCHAR >::tagNODE *pNode;
		DWORD dwDelSEC, dwCurAbsSEC = classTIME::GetCurrentAbsSecond();
		do {
			if ( this->m_pSQL->GetStrPTR( 0 ) ) {
				szCharName = this->m_pSQL->GetStrPTR ( 0 );
				dwDelSEC = this->m_pSQL->GetInteger( 4 );

				if ( dwDelSEC && dwCurAbsSEC >= dwDelSEC ) {
					// 삭제...
					pNode = new CSLList< tagDelCHAR >::tagNODE;
					pNode->m_VALUE.m_Name.Set( szCharName );
					//pNode->m_VALUE.m_dwDBID = this->m_pSQL->GetInteger( 5 );
					DelList.AppendNode (pNode);
				} else {
					if ( dwDelSEC ) {
						// 삭제 대기...
						dwDelSEC -= dwCurAbsSEC;
					}

					pCPacket->AppendString( szCharName );

					pBE = (tagBasicETC   *)this->m_pSQL->GetDataPTR( 1 );
					pBI = (tagBasicINFO  *)this->m_pSQL->GetDataPTR( 2 );
					pGA = (tagGrowAbility*)this->m_pSQL->GetDataPTR( 3 );
#ifdef __KCHS_BATTLECART__
					short nDataVER = this->m_pSQL->GetInteger16( 5 );
#endif
					sCHAR.m_btCharRACE		= pBE->m_btCharRACE;
					sCHAR.m_nJOB			= pBI->m_nClass;
					sCHAR.m_nLEVEL			= pGA->m_nLevel;
					sCHAR.m_dwRemainSEC		= dwDelSEC;
#ifdef	__INC_PLATINUM
	#ifdef __KCHS_BATTLECART__
					if( nDataVER < DATA_VER_2 )
						sCHAR.m_btIsPlatinumCHAR = *((BYTE *)(&pBE->m_RideITEM[ RIDE_PART_ARMS ]));
					else
						sCHAR.m_btIsPlatinumCHAR = pBE->m_btCharSlotNO;
	#else
						sCHAR.m_btIsPlatinumCHAR = pBE->m_btCharSlotNO;
	#endif
#endif
					if ( pBE->m_btCharSlotNO )
						nPlatinumCharCNT++;
					pCPacket->AppendData (&sCHAR,	sizeof( tagCHARINFO ) );
					pCPacket->AppendData (pBE->m_PartITEM, sizeof(tagPartITEM)*MAX_BODY_PART );

					pCPacket->m_wsv_CHAR_LIST.m_btCharCNT ++;
				}
			}
		} while( this->m_pSQL->GetNextRECORD() && ++nC<MAX_CHAR_PER_USER );

		if ( g_pUserLIST->SendPacketToSocketIDX( pSqlPACKET->m_iTAG, pCPacket ) ) {
			CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
			if ( pFindUSER ) {
				pFindUSER->m_nPlatinumCharCNT = nPlatinumCharCNT;
			}
		}

		pNode = DelList.GetHeadNode ();
		while( pNode ) {
			//if ( this->m_pSQL->ExecSQL("DELETE FROM tblGS_AVATAR WHERE txtNAME=\'%s\'", pNode->m_VALUE.m_Name.Get() ) < 1 ) {
			//	// 오류 또는 삭제된것이 없다.
			//	g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR(DELETE_CHAR:%s):: %s \n", pNode->m_VALUE.m_Name.Get(), m_pSQL->GetERROR() );
			//}

			long   iResultSP=-99;
			SDWORD cbSize1=SQL_NTS;
			this->m_pSQL->SetParam_long( 1, iResultSP, cbSize1 );

			#define	SP_DeleteCHAR	"{?=call ws_CharDELETE(\'%s\')}"
			if ( this->m_pSQL->QuerySQL( SP_DeleteCHAR, pNode->m_VALUE.m_Name.Get() ) ) {
				while ( this->m_pSQL->GetMoreRESULT() ) {
					if ( this->m_pSQL->BindRESULT() ) {
						if ( this->m_pSQL->GetNextRECORD() ) {
							;
						}
					}
				}
				if ( 0 != iResultSP ) {
					// 디비프로시져 삭제 실패...
					g_LOG.CS_ODS(LOG_NORMAL, "SP Return ERROR Code:%d (ws_DeleteCHAR:%s):: %s \n", iResultSP, pNode->m_VALUE.m_Name.Get(), m_pSQL->GetERROR() );
				}
			} else {
				// 삭제 실패.;
				g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR(ws_DeleteCHAR:%s):: %s \n", pNode->m_VALUE.m_Name.Get(), m_pSQL->GetERROR() );
			}


			DelList.DeleteHeadNFree ();
			pNode = DelList.GetHeadNode ();
		} ;
	} else {
		// 이 서버에는 등록된 케릭터가 없다.
		CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
		if ( pFindUSER ) {
			pFindUSER->m_nPlatinumCharCNT = 0;
		}
		g_pUserLIST->SendPacketToSocketIDX( pSqlPACKET->m_iTAG, pCPacket );
	}
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CWS_ThreadSQL::Proc_cli_SELECT_CHAR( tagQueryDATA *pSqlPACKET )
{
	/*
		존 번호에 해당되는 존서버로 이동하라고 ...
	*/
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	short nOffset=sizeof(cli_SELECT_CHAR), nOutStrLen;
	char *pCharName = Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( !pCharName) {
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: CharName == NULL \n" );
		return false;
	}

	if ( nOutStrLen > MAX_AVATAR_NAME) {
		g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_SELECT_CHAR:: CharName == '%s'\n", pCharName );
		return false;
	}

	{
		bool m_Injected = false;
		for(int i=0; i<strlen(pCharName); i++)
		{
			if(!m_Injected)
			{
				if(pCharName[i] == '\'')
				{
					if(pCharName[i+1] == ';')
					{
						g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_SELECT_CHAR: 715 SQL Injection Recv'd and filtered\n");
						m_Injected = true;
						pCharName[i] = '\0';
					}
				}
			}
			else
			{
				pCharName[i] = 0x00;
			}
		}
	}

	// 케릭터의 소유자인지 판단...
	this->m_pSQL->MakeQuery( "SELECT txtACCOUNT, binBasicE, intCharID, intDataVER FROM tblGS_AVATAR WHERE txtNAME=", 
							MQ_PARAM_STR, pCharName, 
							MQ_PARAM_END);
	if ( !this->m_pSQL->QuerySQLBuffer() ) {
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		return false;
	}
	if ( !this->m_pSQL->GetNextRECORD() ) {
		// 케릭터 없다.
		g_LOG.CS_ODS(LOG_NORMAL, "Char[ %s ] not found ...\n", pCharName );
		return false;
	}

	char *szCharOWNER = this->m_pSQL->GetStrPTR( 0 );
	tagBasicETC *pBE  = (tagBasicETC*)this->m_pSQL->GetDataPTR( 1 );
	short nDataVER = m_pSQL->GetInteger16( 3 );

	CWS_Client *pUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( pUSER ) {
#ifdef __KCHS_BATTLECART__
		BYTE btCharSlotNO = nDataVER < DATA_VER_2 ? *((BYTE *)(&pBE->m_RideITEM[ 4 ])) : pBE->m_btCharSlotNO ;
#else
		BYTE btCharSlotNO = pBE->m_btCharSlotNO;
#endif
		if ( btCharSlotNO ) {
			// 플레티넘 케릭이면 ???
			if ( 0 == ( pUSER->m_dwPayFLAG & PLAY_FLAG_EXTRA_CHAR ) ) {
				// 뭐냐 ???
				pUSER->CloseSocket ();
				return false;
			}
		}
		pUSER->m_dwDBID		= this->m_pSQL->GetInteger( 2 );
		pUSER->ClanINIT();

		short nZoneNO = pBE->m_nZoneNO;
		if ( !strcmpi( pUSER->Get_ACCOUNT(), szCharOWNER ) ) {
			// 존이 0 이면 ...디폴트 존으로..
			g_pUserLIST->Add_CHAR( pUSER, pCharName );
			if ( pUSER->Send_wsv_MOVE_SERVER( (0==nZoneNO)? AVATAR_ZONE( pBE->m_btCharRACE ) : nZoneNO ) ) 
			{
				// 도착한 쪽지 체크...
				this->m_pSQL->MakeQuery( "SELECT Count(*) FROM tblWS_MEMO WHERE txtNAME=", 
										MQ_PARAM_STR, pCharName,
										MQ_PARAM_END );
				if ( !this->m_pSQL->QuerySQLBuffer () ) {
					g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
					return true;
				}
				
				if ( m_pSQL->GetNextRECORD() && m_pSQL->GetInteger( 0 ) > 0 ) {
					// 쪽지 갯수...
					g_pUserLIST->Send_wsv_MEMO( pSqlPACKET->m_iTAG, MEMO_REQ_RECEIVED_CNT, m_pSQL->GetInteger( 0 ) );
				}
			}
		}
	} // else 접속 끊겼다.

	return false;
}

#define	MAX_BEGINNER_POS	5
tPOINTF	s_BeginnerPOS[ MAX_BEGINNER_POS ] = {
	{	530500,	539500	},
	{	568700,	520222	},
	{	568000,	473400	},
	{	512100,	469900	},
	{	499900,	515600	}
} ;

#define	BEGINNER_ZONE	20
#define	ADVENTURE_ZONE	22

//-------------------------------------------------------------------------------------------------
extern bool IsTAIWAN();
bool CWS_ThreadSQL::Proc_cli_CREATE_CHAR( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	short nOffset=sizeof(cli_CREATE_CHAR), nOutStrLen;
	char *pCharName=Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( NULL == pCharName || nOutStrLen < 4) {
		// 클라이언트 버그로 엄청긴 이름이 전달되어 오는 경우있다.
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return false;
	}

	if ( nOutStrLen > MAX_AVATAR_NAME) {
		g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_CREATE_CHAR:: CharName == '%s'\n", pCharName );
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return false;
	}

	{
		bool m_Injected = false;
		for(int i=0; i<strlen(pCharName); i++)
		{
			if(!m_Injected)
			{
				if(pCharName[i] == '\'')
				{
					if(pCharName[i+1] == ';')
					{
						g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_CREATE_CHAR: 713 SQL injection recv'd and filtered\n");
						m_Injected = true;
						pCharName[i] = '\0';
					}
				}
			}
			else
			{
				pCharName[i] = 0x00;
			}
		}
	}

	if ( pPacket->m_cli_CREATE_CHAR.m_btCharRACE >= m_nDefaultDataCNT ) {
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return false;
	}

	short nDefRACE = pPacket->m_cli_CREATE_CHAR.m_btCharRACE;
	short nZoneIDX = AVATAR_ZONE( nDefRACE );
/*
	if ( !g_ZoneLIST.IsActive( nZoneIDX ) ) {
		g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_CREATE_CHAR:: Invalid Zone %d, Race: %d \n", m_pDefaultBE[ nDefRACE ].m_nZoneNO, nDefRACE );
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return true;
	}
*/
	// "SELECT * FROM tblGS_AVATAR WHERE Name='xxxx';"
	this->m_pSQL->MakeQuery( "SELECT intCharID FROM tblGS_AVATAR WHERE txtNAME=", 
							MQ_PARAM_STR, pCharName,
							MQ_PARAM_END );
	if ( !this->m_pSQL->QuerySQLBuffer () ) {
		// ???
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return false;
	}

	if ( this->m_pSQL->GetNextRECORD () ) {
		// 이미 만들어져 있는 이름이다.
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_DUP_NAME );
		return true;
	}

	this->m_pSQL->MakeQuery( "SELECT Count(*) FROM tblGS_AVATAR WHERE txtACCOUNT=", 
							MQ_PARAM_STR, pSqlPACKET->m_Name.Get(),
							MQ_PARAM_END );
	if ( !this->m_pSQL->QuerySQLBuffer () ) {
		// ???
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return false;
	}
	BYTE btCharSlotNO = 0;
	if ( m_pSQL->GetNextRECORD() ) {
		int iTotalCharCnt = m_pSQL->GetInteger( 0 ); 
		CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
		if ( pFindUSER ) {
			int iNormalCharCnt = iTotalCharCnt - pFindUSER->m_nPlatinumCharCNT;
			if ( pFindUSER->m_dwPayFLAG & PLAY_FLAG_EXTRA_CHAR ) {
				// 최대 5개
				if ( iTotalCharCnt >= 2+MAX_CREATE_CHAR_PER_USER )	{ // 최대 5개
					// 더이상 못만든다.
					g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_NO_MORE_SLOT );
					return true;
				}
				if ( iNormalCharCnt >= MAX_CREATE_CHAR_PER_USER ) { 
					// 플레티넘 케릭으로 생성
					btCharSlotNO = (BYTE)iNormalCharCnt;
				} // else 노말 케릭으루 생성...
			} else
			if ( iNormalCharCnt >= MAX_CREATE_CHAR_PER_USER )	{ // 최대 3개
				// 더이상 못만든다.
				g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_NO_MORE_SLOT );
				return true;
			}
		} else
			return true;
	}

	// 시작 위치..
	WORD wPosBeginner;
	if ( IsTAIWAN() ) {
		wPosBeginner = 0;// 강제로 북쪽~~~
	} else {
		wPosBeginner = pPacket->m_cli_CREATE_CHAR.m_nZoneNO;
		if ( wPosBeginner >= MAX_BEGINNER_POS ) {
			wPosBeginner = RANDOM(MAX_BEGINNER_POS);
		}
	}
	
	// 만들자 !!!
	m_pDefaultBE[ nDefRACE ].m_btCharSlotNO = btCharSlotNO;
	m_pDefaultBE[ nDefRACE ].m_btCharRACE = (BYTE)nDefRACE;

	m_pDefaultBE[ nDefRACE ].m_nZoneNO    = BEGINNER_ZONE;
	//m_pDefaultBE[ nDefRACE ].m_PosSTART   = g_ZoneLIST.Get_StartPOS( nZoneIDX );
	m_pDefaultBE[ nDefRACE ].m_PosSTART   = s_BeginnerPOS[ wPosBeginner ];
	
	// 초기 부활장소 지정...
	short nDefReviveZoneNO = IsTAIWAN() ? BEGINNER_ZONE : ADVENTURE_ZONE;

	m_pDefaultBE[ nDefRACE ].m_nReviveZoneNO = nDefReviveZoneNO;
	m_pDefaultBE[ nDefRACE ].m_PosREVIVE     = g_ZoneLIST.Get_StartRevivePOS( nDefReviveZoneNO );

	// 사용자 선택에의해 변화되는 값들...
	m_sBI.Init ( pPacket->m_cli_CREATE_CHAR.m_cBoneSTONE, pPacket->m_cli_CREATE_CHAR.m_cFaceIDX, pPacket->m_cli_CREATE_CHAR.m_cHairIDX );

	m_pDefaultBE[ nDefRACE ].m_PartITEM[ BODY_PART_FACE ].m_nItemNo = m_sBI.m_cFaceIDX;
	m_pDefaultBE[ nDefRACE ].m_PartITEM[ BODY_PART_HAIR	].m_nItemNo = m_sBI.m_cHairIDX;

	this->m_pSQL->BindPARAM( 1, (BYTE*)&m_pDefaultBE[ nDefRACE ],	sizeof(tagBasicETC) );
	this->m_pSQL->BindPARAM( 2, (BYTE*)&m_sBI,						sizeof(m_sBI) );
	this->m_pSQL->BindPARAM( 3, (BYTE*)&m_pDefaultBA[ nDefRACE ],	sizeof(tagBasicAbility) );
	this->m_pSQL->BindPARAM( 4, (BYTE*)&m_sGA,						sizeof(m_sGA) );
	this->m_pSQL->BindPARAM( 5, (BYTE*)&m_sSA,						sizeof(m_sSA) );
	this->m_pSQL->BindPARAM( 6, (BYTE*)&m_pDefaultINV[ nDefRACE ],	sizeof(CInventory) );

#ifdef __KCHS_BATTLECART__ // __OLD_DATA_COMPATIBLE__
	this->m_pSQL->MakeQuery("INSERT tblGS_AVATAR (txtACCOUNT, txtNAME, intDataVER, binBasicE, binBasicI, binBasicA, binGrowA, binSkillA, blobINV ) VALUES(",
											MQ_PARAM_STR,		pSqlPACKET->m_Name.Get(),
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		pCharName,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_INT16,		DATA_VER_2,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	1,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	2,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	3,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	4,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	5,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	6,
				MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END);
#else
	this->m_pSQL->MakeQuery("INSERT tblGS_AVATAR (txtACCOUNT, txtNAME, binBasicE, binBasicI, binBasicA, binGrowA, binSkillA, blobINV) VALUES(",
											MQ_PARAM_STR,		pSqlPACKET->m_Name.Get(),
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		pCharName,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	1,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	2,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	3,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	4,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	5,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	6,
				MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END);
#endif
	if ( this->m_pSQL->ExecSQLBuffer() < 1 ) {
		// 오류 또는 만들어진것이 없다.
		g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR(CREATE_CHAR:%s):: %s \n", pCharName, m_pSQL->GetERROR() );
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return true;
	}

	// 만들어 졌다..
	CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( pFindUSER ) {
#ifdef	__NEW_LOG
		g_pThreadLOG->When_CharacterLOG( pFindUSER, pCharName, NEWLOG_NEW_CHAR );
#else
		g_pThreadLOG->When_CreateCHAR( pFindUSER, pCharName );
#endif
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_OK, btCharSlotNO );
	} 

	return true;
}
/* ------------------------------------------------------------------------
 * 작성자 : 조욱상
 * Email  : icarus@trigger.co.kr
 * Date   : 
 * 설  명 : 아바타 삭제 및 삭제 대기처리
 * 수  정 :
		이름 : 권형근
		날짜 : Sep. 28 2005
		설명 : 미국의 경우 아바타 삭제 요청시 24시간 이후 삭제됨.
		내용 :	dwDelWaitTime = IsOnlyTAIWAN() ? DELETE_CHAR_WAIT_TIME*12 : DELETE_CHAR_WAIT_TIME;
				=> dwDelWaitTime = IsOnlyTAIWAN() ? DELETE_CHAR_WAIT_TIME*12 : IsUSA() ? DELETE_CHAR_WAIT_TIME*24 : DELETE_CHAR_WAIT_TIME;
 * ------------------------------------------------------------------------ */
extern bool IsOnlyTAIWAN();
extern bool IsUSA();
bool CWS_ThreadSQL::Proc_cli_DELETE_CHAR( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	short nOffset=sizeof(cli_DELETE_CHAR), nOutStrLen;
	char *pCharName=Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( !pCharName || !pSqlPACKET->m_Name.Get() ) {
		return false;
	}

	if ( nOutStrLen > MAX_AVATAR_NAME) {
		g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_SELECT_CHAR:: CharName == '%s'\n", pCharName );
		return false;
	}

	{
		bool m_Injected = false;
		for(int i=0; i<strlen(pCharName); i++)
		{
			if(!m_Injected)
			{
				if(pCharName[i] == '\'')
				{
					if(pCharName[i+1] == ';')
					{
						g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_DELETE_CHAR: 714 SQL Injection Recv'd and filtered\n");
						m_Injected = true;
						pCharName[i] = '\0';
					}
				}
			}
			else
			{
				pCharName[i] = 0x00;
			}
		}
	}

	DWORD dwCurAbsSEC=0, dwReaminSEC=0;

	if ( pPacket->m_cli_DELETE_CHAR.m_bDelete ) {
		// 삭제 대기
		DWORD dwDelWaitTime;
		
		dwDelWaitTime = IsOnlyTAIWAN() ? DELETE_CHAR_WAIT_TIME*12 : IsUSA() ? DELETE_CHAR_WAIT_TIME*24 : DELETE_CHAR_WAIT_TIME;
		dwCurAbsSEC = classTIME::GetCurrentAbsSecond() + dwDelWaitTime;
		dwReaminSEC = dwDelWaitTime;
	}

	if ( this->m_pSQL->QuerySQL( "{call ws_ClanCharGET(\'%s\')}", pCharName ) ) {
		if ( this->m_pSQL->GetNextRECORD() ) {
			// 클랜 있다.
			int iClanPOS = this->m_pSQL->GetInteger(2);
			if ( iClanPOS >= GPOS_MASTER ) {
				classUSER *pFindUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
				classPACKET *pCPacket = Packet_AllocNLock ();
				if ( pFindUSER && pCPacket ) {
					pCPacket->m_HEADER.m_wType = WSV_DELETE_CHAR;
					pCPacket->m_HEADER.m_nSize = sizeof( wsv_DELETE_CHAR );

					pCPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME = 0xffffffff;
					pCPacket->AppendString ( pCharName );
					pFindUSER->Send_Start( pCPacket );

					Packet_ReleaseNUnlock( pCPacket );
				}
				return true;
			}
		}
	}


	if ( this->m_pSQL->ExecSQL("UPDATE tblGS_AVATAR SET dwDelTIME=%u WHERE txtACCOUNT=\'%s\' AND txtNAME=\'%s\'", dwCurAbsSEC, pSqlPACKET->m_Name.Get(), pCharName ) < 1 ) {
		// 오류 또는 삭제된것이 없다.
		g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR:: %s \n", m_pSQL->GetERROR() );
	}
	/*
	this->m_pSQL->MakeQuery("DELETE FROM tblGS_AVATAR WHERE txtACCOUNT=",
													MQ_PARAM_STR,	pSqlPACKET->m_Name.Get(),	// account
			MQ_PARAM_ADDSTR,	" AND txtNAME=",	MQ_PARAM_STR,	pCharName,
			MQ_PARAM_END );
	if ( this->m_pSQL->ExecSQLBuffer() < 1 ) {
		// 오류 또는 삭제된것이 없다.
		g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR(DELETE_CHAR:%s):: %s \n", pCharName, m_pSQL->GetERROR() );
		return true;
	}
	*/

#pragma COMPILE_TIME_MSG( "등록된 모든 메모장 삭제할곳..." )

	CWS_Client *pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( pFindUSER ) {
#ifdef	__NEW_LOG
		g_pThreadLOG->When_CharacterLOG( pFindUSER, pCharName, NEWLOG_DEL_START_CHAR );
#else
		g_pThreadLOG->When_DeleteCHAR( pFindUSER, pCharName );
#endif

		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			pCPacket->m_HEADER.m_wType = WSV_DELETE_CHAR;
			pCPacket->m_HEADER.m_nSize = sizeof( wsv_DELETE_CHAR );

			pCPacket->m_wsv_DELETE_CHAR.m_dwDelRemainTIME = dwReaminSEC;
			pCPacket->AppendString ( pCharName );

			pFindUSER->Send_Start( pCPacket );

			Packet_ReleaseNUnlock( pCPacket );
		}
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
#define WSVAR_TBL_BLOB		2
bool CWS_ThreadSQL::Load_WORLDVAR (BYTE *pVarBUFF, short nBuffLEN)
{
	this->m_pSQL->MakeQuery( "SELECT * FROM tblWS_VAR WHERE txtNAME=", 
							MQ_PARAM_STR, WORLD_VAR,
							MQ_PARAM_END);
	if ( !this->m_pSQL->QuerySQLBuffer() ) {
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		return false;
	}

	if ( !this->m_pSQL->GetNextRECORD() ) {
		// insert !!!
		this->m_pSQL->BindPARAM( 1, pVarBUFF,	nBuffLEN );

		this->m_pSQL->MakeQuery("INSERT tblWS_VAR (txtNAME, dateUPDATE, binDATA) VALUES(",
												MQ_PARAM_STR,		WORLD_VAR,
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		g_pThreadLOG->GetCurDateTimeSTR(),
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	1,
					MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END);
		if ( this->m_pSQL->ExecSQLBuffer() < 1 ) {
			g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: INSERT %s : %s \n", WORLD_VAR, m_pSQL->GetERROR() );
			return true;
		}
	} else {
		BYTE *pDATA = this->m_pSQL->GetDataPTR( WSVAR_TBL_BLOB );

		::CopyMemory( pVarBUFF, pDATA, nBuffLEN );
	}

	return true;
}

bool CWS_ThreadSQL::Save_WorldVAR ( BYTE *pVarBUFF, short nBuffLEN )
{
	sql_ZONE_DATA *pPacket;

	pPacket = (sql_ZONE_DATA*) new BYTE[ sizeof(sql_ZONE_DATA) + nBuffLEN ];

	pPacket->m_nDataSIZE = nBuffLEN;

	pPacket->m_wType = SQL_ZONE_DATA;
	pPacket->m_nSize = sizeof( sql_ZONE_DATA ) + pPacket->m_nDataSIZE;

	pPacket->m_btDataTYPE = SQL_ZONE_DATA_WORLDVAR_SAVE;
	::CopyMemory( pPacket->m_btZoneDATA, pVarBUFF, pPacket->m_nDataSIZE );

	CSqlTHREAD::Add_SqlPACKET(0, WORLD_VAR, (BYTE*)pPacket, pPacket->m_nSize);
	SAFE_DELETE_ARRAY( pPacket );

	return true;
}

bool CWS_ThreadSQL::Proc_SAVE_WORLDVAR( sql_ZONE_DATA *pSqlZONE )
{
	this->m_pSQL->BindPARAM( 1, (BYTE*)pSqlZONE->m_btZoneDATA,	pSqlZONE->m_nDataSIZE );

	this->m_pSQL->MakeQuery( "UPDATE tblWS_VAR SET dateUPDATE=",
												MQ_PARAM_STR,		g_pThreadLOG->GetCurDateTimeSTR(),
			MQ_PARAM_ADDSTR, ",binDATA=",		MQ_PARAM_BINDIDX,	1,
			MQ_PARAM_ADDSTR, "WHERE txtNAME=",	MQ_PARAM_STR,		WORLD_VAR,
												MQ_PARAM_END );
	if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
		// 고치기 실패 !!!
		g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE %s %s \n", WORLD_VAR, m_pSQL->GetERROR() );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CWS_ThreadSQL::Proc_cli_MEMO( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	switch( pPacket->m_cli_MEMO.m_btTYPE )
	{
		case MEMO_REQ_RECEIVED_CNT :
		{
			if ( !this->m_pSQL->QuerySQL( "SELECT Count(*) FROM tblWS_MEMO WHERE txtNAME=\'%s\';", pSqlPACKET->m_Name.Get() ) ) {
				g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
				return false;
			}
			g_pUserLIST->Send_wsv_MEMO( pSqlPACKET->m_iTAG, MEMO_REPLY_RECEIVED_CNT, m_pSQL->GetInteger( 0 ) );
			return true;
		}

		case MEMO_REQ_CONTENTS	  :
		{
			// 한번에 5개의 쪽지 읽음
			if ( !this->m_pSQL->QuerySQL("SELECT TOP 5 dwDATE, txtFROM, txtMEMO FROM tblWS_MEMO WHERE txtNAME=\'%s\' ORDER BY dwDATE ASC", pSqlPACKET->m_Name.Get() ) ) {
				g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
				return false;
			}
			// EX: delete top 2 from `tblgs_error` where txtACCOUNT='gmsho004' order by dateREG ASC
			if ( this->m_pSQL->GetNextRECORD() ) {
				classPACKET *pCPacket = Packet_AllocNLock ();
				if ( !pCPacket )
					return false;

				pCPacket->m_HEADER.m_wType = WSV_MEMO;
				pCPacket->m_HEADER.m_nSize = sizeof( wsv_MEMO );
				pCPacket->m_wsv_MEMO.m_btTYPE = MEMO_REPLY_CONTENTS;

				DWORD dwDate, *pDW;
				char *szFrom, *szMemo;
				int iMemoCNT=0;
				do {
					iMemoCNT ++;
					dwDate = (DWORD)this->m_pSQL->GetInteger(0);
					szFrom = this->m_pSQL->GetStrPTR( 1 );
					szMemo = this->m_pSQL->GetStrPTR( 2, false );

					pDW = (DWORD*)( &pCPacket->m_pDATA[ pCPacket->m_HEADER.m_nSize ] );
					pCPacket->m_HEADER.m_nSize += 4;
					*pDW = dwDate;
					pCPacket->AppendString( szFrom );
					pCPacket->AppendString( szMemo );

					if ( pCPacket->m_HEADER.m_nSize > MAX_PACKET_SIZE-270 ) {
						// 꽉찼다... 전송
						g_pUserLIST->SendPacketToSocketIDX( pSqlPACKET->m_iTAG, pCPacket );
						Packet_ReleaseNUnlock( pCPacket );

						pCPacket = Packet_AllocNLock ();
						if ( !pCPacket )
							return false;

						pCPacket->m_HEADER.m_wType = WSV_MEMO;
						pCPacket->m_HEADER.m_nSize = sizeof( wsv_MEMO );
						pCPacket->m_wsv_MEMO.m_btTYPE = MEMO_REPLY_CONTENTS;
					}
				} while( this->m_pSQL->GetNextRECORD() );

				g_pUserLIST->SendPacketToSocketIDX( pSqlPACKET->m_iTAG, pCPacket );
				Packet_ReleaseNUnlock( pCPacket );

				/*
				DELETE FROM tblWS_MEMO WHERE (intSN IN (SELECT TOP 2 intSN FROM tblWS_MEMO WHERE txtNAME = 'navi' ORDER BY dwDATE ASC))
				*/
				
				if ( this->m_pSQL->ExecSQL( "DELETE FROM tblWS_MEMO WHERE (intSN IN (SELECT TOP %d intSN FROM tblWS_MEMO WHERE txtNAME=\'%s\' ORDER BY dwDATE ASC))",
							iMemoCNT, pSqlPACKET->m_Name.Get() ) < 1 ) {
					// 오류 또는 삭제된것이 없다.
					g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR(DELETE_MEMO):: %s \n", m_pSQL->GetERROR() );
					return true;
				}
			} // else 쪽지 없다.
			break;
		}
		case MEMO_REQ_SEND	  :
		{
			/*
				jeddli에게 온 메시지 갯수 구하기. 조건 jeddli는 tblgs_avatar에 존재해야 함
				=========================================================================================================
				SELECT count(*) FROM tblws_memo LEFT JOIN tblgs_avatar ON tblws_memo.txtNAME=tblgs_avatar.txtNAME 
				WHERE tblgs_avatar.txtNAME='jeddli';
			*/
			short nOffset = sizeof(cli_MEMO);
			char *szTargetCHAR;

			szTargetCHAR = Packet_GetStringPtr( pPacket, nOffset );
			if ( !szTargetCHAR || strlen(szTargetCHAR) < 1 ) {
				// 잘못된 케릭 이름
				g_pUserLIST->Send_wsv_MEMO( pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_INVALID_TARGET );
				return true;
			}
			char *szMemo = Packet_GetStringPtr( pPacket, nOffset );
			if ( !szMemo || strlen(szTargetCHAR) < 2 ) {
				// 쪽지 내용 오류.
				g_pUserLIST->Send_wsv_MEMO( pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_INVALID_CONTENT );
				return true;
			}

			#define	OPTION_REFUSE_JJOKJI	0x00000001
			// 대상 케릭의 쪽지 수신 거부 여부...
			if ( !this->m_pSQL->QuerySQL( "SELECT dwOPTION FROM tblGS_AVATAR WHERE txtNAME=\'%s\';", szTargetCHAR ) ) {
				g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
				return false;
			}

			if ( !m_pSQL->GetNextRECORD() ) {
				// 보낼 대상 케릭 없다.
				g_pUserLIST->Send_wsv_MEMO( pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_NOT_EXIST );
				return true;
			}
			if ( m_pSQL->GetInteger( 0 ) & OPTION_REFUSE_JJOKJI ) {
				// 거부 상태
				g_pUserLIST->Send_wsv_MEMO( pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_REFUSED );
				return true;
			}

			#define	MAX_RECV_MEMO_CNT	50
			// 대상 케릭이 몇개의 보관된 쪽지가 있냐?
			if ( !this->m_pSQL->QuerySQL( "SELECT Count(*) FROM tblWS_MEMO WHERE txtNAME=\'%s\';", szTargetCHAR ) ) {
				g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
				return false;
			}
			if ( m_pSQL->GetNextRECORD() && m_pSQL->GetInteger( 0 ) > MAX_RECV_MEMO_CNT ) {
				// MAX_RECV_MEMO_CNT 개 이상의 쪽지를 보유 하고 있다면...
				g_pUserLIST->Send_wsv_MEMO( pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_FULL_MEMO );
				return true;
			}

			// 쪽지 저장..
			DWORD dwCurAbsSEC = classTIME::GetCurrentAbsSecond();
			this->m_pSQL->MakeQuery("INSERT tblWS_MEMO (dwDATE, txtNAME, txtFROM, txtMEMO) VALUES(",
													MQ_PARAM_INT,		dwCurAbsSEC,	
						MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		szTargetCHAR,
						MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		pSqlPACKET->m_Name.Get(),
						MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		szMemo,
						MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END);
			if ( this->m_pSQL->ExecSQLBuffer() < 1 ) {
				// 만들기 실패 !!!
				g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: INSERT MEMO:%s : %s \n", pSqlPACKET->m_Name.Get(), m_pSQL->GetERROR() );
				return true;
			}

			g_pUserLIST->Send_wsv_MEMO( pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_OK );
			return true;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
