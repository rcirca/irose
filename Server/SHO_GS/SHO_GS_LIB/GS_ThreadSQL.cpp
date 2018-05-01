/*
	$Header: /7HeartsOnline/Server/SHO_GS/Sho_gs_lib/GS_ThreadSQL.cpp 117   05-08-25 5:24p Icarus $
*/
#include "LIB_gsMAIN.h"
#include "CSLList.h"
#include "classTIME.h"
#include "GS_ThreadSQL.h"
#include "GS_ThreadLOG.h"
#include "GS_ListUSER.h"
#include "GS_SocketLSV.h"
#include "GS_SocketASV.h"
#include "ZoneLIST.h"
#include "CThreadGUILD.h"

#define	MAX_CHAR_PER_USER	5
#define	DELETE_CHAR_WAIT_TIME  (5*60)	//	7일 24시간 60분 60초

enum LOGINTBL_COL_IDX {
	LGNTBL_USERINFO = 0,
	LGNTBL_ACCOUNT,
	LGNTBL_PASSWORD,
	LGNTBL_LAST_CONNECT,
	LGNTBL_ENABLE,
	LGNTBL_REG_DATE
} ;

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

enum BANKTBL_COL_IDX {
	BANKTBL_ACCOUNT = 0,
	BANKTBL_ITEMS,
	BANKTBL_REWARD,
	BANKTLB_PASSWORD
} ;

enum MEMOTBL_COL_IDX {
	MEMOTBL_MEMOID = 0,
	MEMOTBL_DATE,
	MEMOTBL_NAME,
	MEMOTBL_FROM,
	MEMOTBL_MEMO
} ;

//-------------------------------------------------------------------------------------------------
// suspend mode로 시작, spinlock설정...
GS_CThreadSQL::GS_CThreadSQL () : CSqlTHREAD( true ), m_csUserLIST( 4000 ), m_TmpSTR( 512 )
{
	COMPILE_TIME_ASSERT( 4 == sizeof( void* ) );

#ifdef __KCHS_BATTLECART__ // __OLD_DATA_COMPATIBLE__
	COMPILE_TIME_ASSERT( MAX_RIDING_PART			== 5 );
#else
	COMPILE_TIME_ASSERT( MAX_RIDING_PART			== 4 );
#endif
	COMPILE_TIME_ASSERT( sizeof( tagBasicETC )		<= 96	);	// db field size
	COMPILE_TIME_ASSERT( sizeof( tagBasicINFO )		<= 32	);
	COMPILE_TIME_ASSERT( sizeof( tagBasicAbility )	<= 48	);
#ifdef FRAROSE
	COMPILE_TIME_ASSERT( sizeof( tagGrowAbility )	<= 394	);
#else
	COMPILE_TIME_ASSERT( sizeof( tagGrowAbility )	<= 384	);
#endif
	COMPILE_TIME_ASSERT( sizeof( tagSkillAbility )	<= 240	);
	COMPILE_TIME_ASSERT( sizeof( CInventory )		<= 2048 );
	COMPILE_TIME_ASSERT( sizeof( tagBankData )		<= 2576 );
	COMPILE_TIME_ASSERT( sizeof( tagQuestData )		< 1000 );

	COMPILE_TIME_ASSERT( sizeof( tagITEM )			== (6+8) );
#ifdef __KCHS_BATTLECART__ // __OLD_DATA_COMPATIBLE__
	COMPILE_TIME_ASSERT( sizeof( CInventory )		== (140*14+8) );	// 1954
#else
	COMPILE_TIME_ASSERT( sizeof( CInventory )		== (139*14+8) );	// 1954
#endif

	COMPILE_TIME_ASSERT( sizeof(m_sBE)		== sizeof( tagBasicETC		) );
	COMPILE_TIME_ASSERT( sizeof(m_sBI)		== sizeof( tagBasicINFO		) );
#ifdef FRAROSE
	COMPILE_TIME_ASSERT( sizeof(m_sGB)		== MAX_GRAW_ABILITY_BUFF );	//sizeof( tagGrowAbility	) );
#else
	COMPILE_TIME_ASSERT( sizeof(m_sGB)		== MAX_GRAW_ABILITY_BUFF );	//sizeof( tagGrowAbility	) );
#endif
	COMPILE_TIME_ASSERT( sizeof(m_sSA)		== sizeof( tagSkillAbility	) );
	COMPILE_TIME_ASSERT( sizeof(m_sQD)		== sizeof( tagQuestData		) );
	COMPILE_TIME_ASSERT( sizeof(tagHotICON) == sizeof( WORD				) );

	m_bWaiting = false;
	m_sGB.Init ();
	m_sSA.Init ();
	m_sQD.Init ();
	m_HotICON.Init ();
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
			assert( AVATAR_ZONE( nR ) > 0 );

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

__fastcall GS_CThreadSQL::~GS_CThreadSQL()
{
	SAFE_DELETE_ARRAY( m_pDefaultBA );
	SAFE_DELETE_ARRAY( m_pDefaultBE );
	SAFE_DELETE_ARRAY( m_pDefaultINV );
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Sql_TEST ()
{
	DWORD dwCurAbsSEC = classTIME::GetCurrentAbsSecond();

	// UPDATE tblGS_AVATAR SET ZoneNO=xxx WHERE Name='icarus_test';
	char *pCharName="icarus";

/*
Create Procedure spGetImage
@id int
As
SELECT ImageId, ImageDescription, ImagePath 
FROM tblImages
WHERE ImageId = @id

"{ ? = CALL dbo.spGetImage;1 (?) }"
*/
	::SQLCloseCursor( ( (classODBC*)this->m_pSQL )->m_hSTMT1 );

	long lReturn=-1;
	SDWORD cbParm1=SQL_NTS;
	if ( !( (classODBC*)this->m_pSQL )->SetParam_long  ( 1, lReturn, cbParm1 ) ) 
	{
		g_LOG.CS_ODS(LOG_NORMAL, "shit~~~~~ %s \n", m_pSQL->GetERROR() );
	}
//	this->m_pSQL->BindPARAM( 1, (BYTE*)pCharName,			strlen(pCharName)		);
	
//	if ( this->m_pSQL->MakeQuery("{call sho_test( ? ) }" ) < 0 ) {
	if ( !this->m_pSQL->QuerySQL("{?=call sho_test( \'%s\' ) }", pCharName ) ) {
//	if ( this->m_pSQL->ExecSQL("{ call sho_test( %s ) }", pCharName ) < 0 ) {
		g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR:: %s \n", m_pSQL->GetERROR() );
	}

/*
	while( this->m_pSQL->GetNextRECORD() ) {
		g_LOG.CS_ODS(LOG_NORMAL, "%d  call result:: %s, %s \n", lReturn, m_pSQL->GetStrPTR(0), m_pSQL->GetStrPTR(1) );
	}
*/
	::SQLMoreResults( ( (classODBC*)this->m_pSQL )->m_hSTMT1 ); // != SQL_NO_DATA


	int iii = lReturn;

	g_LOG.CS_ODS(LOG_NORMAL, "RESULT:: %d   \n", lReturn );

/*
	this->m_pSQL->MakeQuery("UPDATE tblGS_AVATAR SET dwDelTIME=",
													MQ_PARAM_INT,   dwCurAbsSEC,
				MQ_PARAM_ADDSTR, "WHERE txtNAME=",	MQ_PARAM_STR,	pCharName,	MQ_PARAM_END);
	if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
		// 고치기 실패 !!!
		g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR:: %s \n", m_pSQL->GetERROR() );
		return true;
	}
*/

	if ( this->m_pSQL->GetRecordCNT() != 1 ) {
		// 고칠 레코드가 없다.
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Add_SqlPacketWithACCOUNT (classUSER *pUSER, t_PACKET *pPacket)
{
	return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX, pUSER->Get_ACCOUNT(), (BYTE*)pPacket, pPacket->m_HEADER.m_nSize);
}
bool GS_CThreadSQL::Add_SqlPacketWithAVATAR (classUSER *pUSER, t_PACKET *pPacket)
{
	return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX, pUSER->Get_NAME(), (BYTE*)pPacket, pPacket->m_HEADER.m_nSize);
}


bool GS_CThreadSQL::IO_ZoneDATA (CZoneTHREAD *pZONE, bool bSave)
{
	sql_ZONE_DATA *pPacket;

	if ( bSave ) {
		// 존이 뽀개지면서 호출될경우 처리시 존의 데이타를 참조하면 안됨...
		// 패킷에 복사된 생성...
		pPacket = (sql_ZONE_DATA*) new BYTE[ sizeof(sql_ZONE_DATA) + sizeof(tagECONOMY) ];
		pPacket->m_nDataSIZE = sizeof( tagECONOMY );
		::CopyMemory( pPacket->m_btZoneDATA, pZONE->m_Economy.m_pEconomy, pPacket->m_nDataSIZE );
		pPacket->m_btDataTYPE = SQL_ZONE_DATA_ECONOMY_SAVE;
	} else {
		pPacket = new sql_ZONE_DATA;
		pPacket->m_nDataSIZE = 0;
		pPacket->m_btDataTYPE = SQL_ZONE_DATA_ECONOMY_LOAD;
	}
	pPacket->m_wType = SQL_ZONE_DATA;
	pPacket->m_nSize = sizeof( sql_ZONE_DATA ) + pPacket->m_nDataSIZE;

/*
	m_TmpSTR.Printf ("%s_EC_Zone%d_%s", CLIB_GameSRV::GetInstance()->GetServerName(), pZONE->Get_ZoneNO(), pZONE->Get_NAME() );
	CSqlTHREAD::Add_SqlPACKET(pZONE->Get_ZoneNO(), m_TmpSTR.Get(), (BYTE*)pPacket, pPacket->m_nSize);
*/
	CSqlTHREAD::Add_SqlPACKET(pZONE->Get_ZoneNO(), pZONE->Get_NAME(), (BYTE*)pPacket, pPacket->m_nSize);

	SAFE_DELETE_ARRAY( pPacket );
	return true;
}

/*
#define	ZONE_VAR_NPCOBJ			"%s_NPC_%s"
#define ZONE_VAR_EVENTOBJ		"%s_EVT_%s"
*/

bool GS_CThreadSQL::IO_NpcObjDATA (CObjNPC *pObjNPC, bool bSave)
{
	sql_ZONE_DATA *pPacket;

	pPacket = (sql_ZONE_DATA*) new BYTE[ sizeof(sql_ZONE_DATA) + sizeof(tagObjVAR) ];

	pPacket->m_nDataSIZE = sizeof( tagObjVAR );

	pPacket->m_wType = SQL_ZONE_DATA;
	pPacket->m_nSize = sizeof( sql_ZONE_DATA ) + pPacket->m_nDataSIZE;

	pPacket->m_btDataTYPE = ( bSave ) ? SQL_ZONE_DATA_NPCOBJ_SAVE : SQL_ZONE_DATA_NPCOBJ_LOAD;
	::CopyMemory( pPacket->m_btZoneDATA, pObjNPC->m_pVAR, pPacket->m_nDataSIZE );

	m_TmpSTR.Printf ("%s_NPC_%s", CLIB_GameSRV::GetInstance()->GetServerName(), pObjNPC->Get_NAME() );

	CSqlTHREAD::Add_SqlPACKET( pObjNPC->Get_CharNO(), m_TmpSTR.Get(), (BYTE*)pPacket, pPacket->m_nSize );

	SAFE_DELETE_ARRAY( pPacket );
	return true;
}
bool GS_CThreadSQL::IO_EventObjDATA ( CObjEVENT *pObjEVENT, bool bSave)
{
	sql_ZONE_DATA *pPacket;

	pPacket = (sql_ZONE_DATA*) new BYTE[ sizeof(sql_ZONE_DATA) + sizeof(tagObjVAR) ];

	pPacket->m_nDataSIZE = sizeof( tagObjVAR );

	pPacket->m_wType = SQL_ZONE_DATA;
	pPacket->m_nSize = sizeof( sql_ZONE_DATA ) + pPacket->m_nDataSIZE;

	pPacket->m_btDataTYPE = ( bSave ) ? SQL_ZONE_DATA_EVENTOBJ_SAVE : SQL_ZONE_DATA_EVENTOBJ_LOAD;
	::CopyMemory( pPacket->m_btZoneDATA, pObjEVENT->m_pVAR, pPacket->m_nDataSIZE );

	m_TmpSTR.Printf ("%s_EVT_%s", CLIB_GameSRV::GetInstance()->GetServerName(), pObjEVENT->Get_NAME() );

	CSqlTHREAD::Add_SqlPACKET(pObjEVENT->Get_ID(), m_TmpSTR.Get(), (BYTE*)pPacket, pPacket->m_nSize);
	SAFE_DELETE_ARRAY( pPacket );
	return true;
}
bool GS_CThreadSQL::Save_WorldVAR ( BYTE *pVarBUFF, short nBuffLEN )
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

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Add_BackUpUSER (classUSER *pUSER, BYTE btLogOutMODE)
{
	if ( pUSER->m_btLogOutMODE != btLogOutMODE ) {
		// 저장할 필요 없다.
		return true;
	}

	m_csUserLIST.Lock ();
	{
		classDLLNODE< tagSqlUSER > *pNewNODE;
		pNewNODE = new classDLLNODE< tagSqlUSER >;

		// bLogOut이 아닌 경우가 이미 등록 되어 있는 경우가 있어
		// pUSER->m_bLogOut으로 판단하면 안된다.
		pNewNODE->DATA.m_btLogOutMODE = btLogOutMODE;
		pNewNODE->DATA.m_pUSER		  = pUSER;
		m_AddUserLIST.AppendNode( pNewNODE );
	}
	m_csUserLIST.Unlock ();

	m_pEVENT->SetEvent ();

	return true;
}

//-------------------------------------------------------------------------------------------------
#define	BEGINNER_ZONE_LEVEL		5
#define	BEGINNER_ZONE_NO		20
bool GS_CThreadSQL::UpdateUserRECORD (classUSER *pUSER)
{
	// update character DB !!!
	// "UPDATE tblGS_AVATAR SET nZoneNO=xxx, binBasicI=xx, binBasicA=xx WHERE txtNAME=xxx;"
	m_sBE.m_btCharRACE = (BYTE)pUSER->m_nCharRACE;

	if ( pUSER->Get_HP() <= 0 ) {
		// 존번호가 없으면 ???
		// 죽었으면 저장된 부활장소에서 ...
		if ( pUSER->m_nZoneNO == BEGINNER_ZONE_NO ) {
			m_sBE.m_nZoneNO   = BEGINNER_ZONE_NO;
			m_sBE.m_PosSTART  = pUSER->m_PosCUR;
		} else {
			m_sBE.m_nZoneNO	  = pUSER->m_nReviveZoneNO;
			m_sBE.m_PosSTART  = pUSER->m_PosREVIVE;
		}
	} else {
		m_sBE.m_nZoneNO   = pUSER->m_nZoneNO;
		m_sBE.m_PosSTART  = pUSER->m_PosCUR;
	}

	// 부활 장소...
	m_sBE.m_nReviveZoneNO = pUSER->m_nReviveZoneNO;
	m_sBE.m_PosREVIVE	  = pUSER->m_PosREVIVE;
	m_sBE.m_btCharSlotNO  = pUSER->m_btPlatinumCHAR;

	if ( m_sBE.m_nZoneNO < 0 || m_sBE.m_nReviveZoneNO < 0 ) {
		g_LOG.CS_ODS( 0xffff, "**** Invalid ZoneNO [ %s ] Race: %d, Zone: %d ReviveZone: %d \n",
				pUSER->Get_NAME(),
				m_sBE.m_btCharRACE,
				m_sBE.m_nZoneNO, 
				m_sBE.m_nReviveZoneNO );
	}

	::CopyMemory( m_sBE.m_PartITEM, pUSER->m_PartITEM,	sizeof(tagPartITEM)*MAX_BODY_PART);
	::CopyMemory( m_sBE.m_RideITEM, pUSER->m_RideITEM,	sizeof(tagPartITEM)*MAX_RIDING_PART);

	pUSER->m_BasicINFO.m_cFaceIDX = (char)pUSER->m_PartITEM[ BODY_PART_FACE ].m_nItemNo;
	pUSER->m_BasicINFO.m_cHairIDX = (char)pUSER->m_PartITEM[ BODY_PART_HAIR ].m_nItemNo;

	this->m_pSQL->BindPARAM( 1, (BYTE*)&this->m_sBE,			sizeof( tagBasicETC )		);
	this->m_pSQL->BindPARAM( 2, (BYTE*)&pUSER->m_BasicINFO,		sizeof( tagBasicINFO )		);
	this->m_pSQL->BindPARAM( 3, (BYTE*)&pUSER->m_BasicAbility,	sizeof( tagBasicAbility )	);
	this->m_pSQL->BindPARAM( 4, (BYTE*)&pUSER->m_GrowAbility,	sizeof( tagGrowAbility )	);
	this->m_pSQL->BindPARAM( 5, (BYTE*)&pUSER->m_Skills,		sizeof( tagSkillAbility )	);
	this->m_pSQL->BindPARAM( 6, (BYTE*)&pUSER->m_Inventory,		sizeof( CInventory )		);
	this->m_pSQL->BindPARAM( 7, (BYTE*)&pUSER->m_Quests,		sizeof( tagQuestData )		);
	this->m_pSQL->BindPARAM( 8, (BYTE*)&pUSER->m_HotICONS,		sizeof( CHotICONS )			);
	this->m_pSQL->BindPARAM( 9, (BYTE*)&pUSER->m_WishLIST,		sizeof( tagWishLIST )		);

#ifdef __KCHS_BATTLECART__  // MQ_PARAM_INT16,		DATA_VER_2,
	this->m_pSQL->MakeQuery( "UPDATE tblGS_AVATAR SET binBasicE=",
												MQ_PARAM_BINDIDX,	1,
			MQ_PARAM_ADDSTR, ",binBasicI=",		MQ_PARAM_BINDIDX,	2,
			MQ_PARAM_ADDSTR, ",binBasicA=",		MQ_PARAM_BINDIDX,	3,
			MQ_PARAM_ADDSTR, ",binGrowA=",		MQ_PARAM_BINDIDX,	4,
			MQ_PARAM_ADDSTR, ",binSkillA=",		MQ_PARAM_BINDIDX,	5,
			MQ_PARAM_ADDSTR, ",blobINV=",		MQ_PARAM_BINDIDX,	6,
			MQ_PARAM_ADDSTR, ",blobQUEST=",		MQ_PARAM_BINDIDX,	7,
			MQ_PARAM_ADDSTR, ",binHotICON=",	MQ_PARAM_BINDIDX,	8,
			MQ_PARAM_ADDSTR, ",binWishLIST=",	MQ_PARAM_BINDIDX,	9,
			MQ_PARAM_ADDSTR, ",btLEVEL=",		MQ_PARAM_INT,		pUSER->m_GrowAbility.m_nLevel,
			MQ_PARAM_ADDSTR, ",intMoney=",		MQ_PARAM_INT64,		pUSER->GetCur_MONEY(),
			MQ_PARAM_ADDSTR, ",intJOB=",		MQ_PARAM_INT,		pUSER->m_BasicINFO.m_nClass,
			MQ_PARAM_ADDSTR, ",dwRegTIME=",		MQ_PARAM_INT,		this->m_dwCurTIME,
			MQ_PARAM_ADDSTR, ",dwPartyIDX=",	MQ_PARAM_INT,		pUSER->GetPARTY() ? pUSER->m_pPartyBUFF->m_wPartyWSID : 0,
			MQ_PARAM_ADDSTR, ",dwItemSN=",		MQ_PARAM_INT,		pUSER->m_dwItemSN,
			MQ_PARAM_ADDSTR, ",intDataVER=",	MQ_PARAM_INT16,		DATA_VER_2,
			MQ_PARAM_ADDSTR, "WHERE txtNAME=",	MQ_PARAM_STR,		pUSER->Get_NAME(),
												MQ_PARAM_END );
#else
	this->m_pSQL->MakeQuery( "UPDATE tblGS_AVATAR SET binBasicE=",
												MQ_PARAM_BINDIDX,	1,
			MQ_PARAM_ADDSTR, ",binBasicI=",		MQ_PARAM_BINDIDX,	2,
			MQ_PARAM_ADDSTR, ",binBasicA=",		MQ_PARAM_BINDIDX,	3,
			MQ_PARAM_ADDSTR, ",binGrowA=",		MQ_PARAM_BINDIDX,	4,
			MQ_PARAM_ADDSTR, ",binSkillA=",		MQ_PARAM_BINDIDX,	5,
			MQ_PARAM_ADDSTR, ",blobINV=",		MQ_PARAM_BINDIDX,	6,
			MQ_PARAM_ADDSTR, ",blobQUEST=",		MQ_PARAM_BINDIDX,	7,
			MQ_PARAM_ADDSTR, ",binHotICON=",	MQ_PARAM_BINDIDX,	8,
			MQ_PARAM_ADDSTR, ",binWishLIST=",	MQ_PARAM_BINDIDX,	9,
			MQ_PARAM_ADDSTR, ",btLEVEL=",		MQ_PARAM_INT,		pUSER->m_GrowAbility.m_nLevel,
			MQ_PARAM_ADDSTR, ",intMoney=",		MQ_PARAM_INT64,		pUSER->GetCur_MONEY(),
			MQ_PARAM_ADDSTR, ",intJOB=",		MQ_PARAM_INT,		pUSER->m_BasicINFO.m_nClass,
			MQ_PARAM_ADDSTR, ",dwRegTIME=",		MQ_PARAM_INT,		this->m_dwCurTIME,
			MQ_PARAM_ADDSTR, ",dwPartyIDX=",	MQ_PARAM_INT,		pUSER->GetPARTY() ? pUSER->m_pPartyBUFF->m_wPartyWSID : 0,
			MQ_PARAM_ADDSTR, ",dwItemSN=",		MQ_PARAM_INT,		pUSER->m_dwItemSN,
			MQ_PARAM_ADDSTR, "WHERE txtNAME=",	MQ_PARAM_STR,		pUSER->Get_RNAME(),
												MQ_PARAM_END );
#endif

	if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
		// 고치기 실패 !!!
		// log ...
		g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE Char:%s %s \n", pUSER->Get_NAME(), m_pSQL->GetERROR() );
	} else {
		// 케릭 백업된 시간 로그 남기기..
		g_pThreadLOG->When_BackUP( pUSER, "CHAR" );
	}

	if ( BANK_CHANGED != pUSER->m_btBankData )
		return true;
	
	// 창고(은행) 데이타 기록...
	this->m_pSQL->BindPARAM( 1, (BYTE*)&pUSER->m_Bank,		sizeof( tagBankData )		);
	this->m_pSQL->MakeQuery( "UPDATE tblGS_BANK SET blobITEMS=",
													MQ_PARAM_BINDIDX,	1,
			MQ_PARAM_ADDSTR, "WHERE txtACCOUNT=",	MQ_PARAM_STR,	pUSER->Get_ACCOUNT(),
													MQ_PARAM_END );
	if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
		// 고치기 실패 !!!
		// log ...
		g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE Bank:%s %s \n", pUSER->Get_ACCOUNT(), m_pSQL->GetERROR() );
	} else {
		pUSER->m_btBankData = BANK_LOADED;
		// 창고 백업된 시간 로그 남기기..
		g_pThreadLOG->When_BackUP( pUSER, "BANK" );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool __fastcall GS_CThreadSQL::ConvertBasicETC ()
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
					MQ_PARAM_ADDSTR, ",intDataVER=",	MQ_PARAM_INT,		1,
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
void GS_CThreadSQL::Clear_LoginTABLE ()
{
	//	GS_LogIN테이블에서 현재서버 ip인 레코드 모두 삭제...
	if ( 0 == ::Get_ServerLangTYPE() ) {
		if ( this->m_pSQL->ExecSQL( "DELETE FROM tblGS_LogIN;" ) < 1 ) {
			// 오류 또는 만들어진것이 없다.
			g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR in Clear_LoginTABLE:: %s \n", m_pSQL->GetERROR() );
		}
	}
}
void GS_CThreadSQL::Add_LoginACCOUNT (char *szAccount)
{
	if ( 0 == ::Get_ServerLangTYPE() ) {
		if ( this->m_pSQL->ExecSQL("INSERT tblGS_LogIN (txtACCOUNT, txtServerIP) VALUES(\'%s\',\'%s\');", szAccount, CLIB_GameSRV::GetInstance()->GetServerIP() ) < 1 ) {
			// 오류 또는 만들어진것이 없다.
			g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR in Add_LoginACCOUNT(%s):: %s \n", szAccount, m_pSQL->GetERROR() );
		}
	}
}
void GS_CThreadSQL::Sub_LoginACCOUNT (char *szAccount)
{
	if ( 0 == ::Get_ServerLangTYPE() && NULL != szAccount ) {
		if ( this->m_pSQL->ExecSQL("DELETE FROM tblGS_LogIN WHERE txtACCOUNT=\'%s\';", szAccount ) < 1 ) {
			// 오류 또는 만들어진것이 없다.
			g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR in Sub_LoginACCOUNT(%s):: %s \n", szAccount, m_pSQL->GetERROR() );
		}
	}
}

void GS_CThreadSQL::Execute ()
{
//	this->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );	// Priority 1 point above the priority class
	this->SetPriority( THREAD_PRIORITY_HIGHEST );		// Priority 2 point above the priority class

	CDLList< tagQueryDATA >::tagNODE *pSqlNODE;
	classDLLNODE< tagSqlUSER   > *pUsrNODE;

	g_LOG.CS_ODS( 0xffff, ">  > >> GS_CThreadSQL::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

	this->ConvertBasicETC();

	this->Clear_LoginTABLE ();

    while( TRUE ) {
		if ( !this->Terminated ) {
			m_bWaiting = true;
			m_pEVENT->WaitFor( INFINITE );
			m_bWaiting = false;
		} else {
			int iCnt1, iCnt2;
			this->m_CS.Lock ();
				iCnt1 = m_AddPACKET.GetNodeCount();
			this->m_CS.Unlock ();

			this->m_csUserLIST.Lock ();
				iCnt2 = m_AddUserLIST.GetNodeCount ();
			this->m_csUserLIST.Unlock ();
			if ( iCnt1 <= 0 && iCnt2 <= 0 ) {
				m_bWaiting = true;
				break;
			}
		}

		// 게임 로그 저장..
		this->Proc_QuerySTRING ();

		this->m_CS.Lock ();
		m_RunPACKET.AppendNodeList( &m_AddPACKET );
		m_AddPACKET.Init ();
		m_pEVENT->ResetEvent ();
		this->m_CS.Unlock ();

		m_dwCurTIME = classTIME::GetCurrentAbsSecond();
		for( pSqlNODE = m_RunPACKET.GetHeadNode(); pSqlNODE; ) {
			if ( Run_SqlPACKET( &pSqlNODE->m_VALUE ) )
				pSqlNODE = this->Del_SqlPACKET( pSqlNODE );
			else
				pSqlNODE = m_RunPACKET.GetNextNode ( pSqlNODE );
		}

		m_csUserLIST.Lock ();
		m_RunUserLIST.AppendNodeList( &m_AddUserLIST );
		m_AddUserLIST.Init ();
		m_csUserLIST.Unlock ();

		while ( pUsrNODE = m_RunUserLIST.GetHeadNode() ) {
			if ( pUsrNODE->DATA.m_pUSER->Get_NAME() ) {
			/*
				if ( pUsrNODE->DATA.m_btLogOutMODE ) {
					LogString (0xffff, "		%d UPDATE %s:%s DATA \n", pUsrNODE->DATA.m_btLogOutMODE, pUsrNODE->DATA.m_pUSER->Get_ACCOUNT(), pUsrNODE->DATA.m_pUSER->Get_NAME() );
				}
			*/
				this->UpdateUserRECORD( pUsrNODE->DATA.m_pUSER );
			}
			
			if ( pUsrNODE->DATA.m_btLogOutMODE ) {
				if ( pUsrNODE->DATA.m_pUSER->Get_ACCOUNT() ) {
					// 로그인 서버에 로그아웃 or 서버 이동 정보 전송.
					g_pSockASV->Send_zas_SUB_ACCOUNT( pUsrNODE->DATA.m_pUSER->Get_ACCOUNT(), 
									pUsrNODE->DATA.m_btLogOutMODE, 
									pUsrNODE->DATA.m_pUSER->Get_LEVEL(), 
									pUsrNODE->DATA.m_pUSER->m_nZoneNO);
					switch( pUsrNODE->DATA.m_btLogOutMODE ) {
					/*
						case LOGOUT_MODE_WARP		:
							// g_pThreadLOG->Whne_LogWARP( pUsrNODE->DATA.m_pUSER );
							g_pSockLSV->Send_gsv_WARP_USER( pUsrNODE->DATA.m_pUSER );
							#pragma COMPILE_TIME_MSG( "TODO:: 이상태에서 뻑나서 접속이 끊기면 로그인 서버가 알수 있나???" )
							break;
					*/
						case LOGOUT_MODE_CHARLIST	:
							// 파티는 탈퇴되서 온다...
							#ifdef	__NEW_LOG
								g_pThreadLOG->When_LogInOrOut ( pUsrNODE->DATA.m_pUSER, NEWLOG_LOGOUT );
							#else
								g_pThreadLOG->When_LogOUT( pUsrNODE->DATA.m_pUSER );
							#endif

							// GS_LogIN 테이블에 계정삭제....
							this->Sub_LoginACCOUNT( pUsrNODE->DATA.m_pUSER->Get_ACCOUNT() );

							g_pSockLSV->Send_gsv_CHANGE_CHAR( pUsrNODE->DATA.m_pUSER );
							break;

						default :
							if ( pUsrNODE->DATA.m_pUSER->GetPARTY() ) {
								// 비정상 종료...
								pUsrNODE->DATA.m_pUSER->m_pPartyBUFF->Lock ();	// 2004. 9. 19 새벽 7시10분 :: 빽섭..
								pUsrNODE->DATA.m_pUSER->m_pPartyBUFF->OnDisconnect( pUsrNODE->DATA.m_pUSER );
								pUsrNODE->DATA.m_pUSER->m_pPartyBUFF->Unlock ();
							}

							#ifdef	__NEW_LOG
								g_pThreadLOG->When_LogInOrOut ( pUsrNODE->DATA.m_pUSER, NEWLOG_LOGOUT );
							#else
								g_pThreadLOG->When_LogOUT( pUsrNODE->DATA.m_pUSER );
							#endif
							g_pSockLSV->Send_zws_SUB_ACCOUNT( pUsrNODE->DATA.m_pUSER->m_dwWSID, pUsrNODE->DATA.m_pUSER->Get_ACCOUNT() );

							// GS_LogIN 테이블에 계정삭제....
							this->Sub_LoginACCOUNT( pUsrNODE->DATA.m_pUSER->Get_ACCOUNT() );
					}
				}

			#ifdef	__INC_WORLD
				// 개인서버일 경우 워프, 케릭터 리스트는 접속 종료 안함.
				//if ( LOGOUT_MODE_LEFT == pUsrNODE->DATA.m_btLogOutMODE )
			#endif
				{
					// 모든 classUSER()는 GS_CThreadSQL::Execute 삭제된다...
					g_pUserLIST->FreeClientSOCKET( pUsrNODE->DATA.m_pUSER );
				}
			}

			m_RunUserLIST.DeleteNFree( pUsrNODE );
		}
	}

	this->Clear_LoginTABLE ();

	int iCnt = m_AddPACKET.GetNodeCount();
	assert( iCnt == 0 );

	g_LOG.CS_ODS( 0xffff, "<  < << GS_CThreadSQL::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Run_SqlPACKET( tagQueryDATA *pSqlPACKET )
{
	switch( pSqlPACKET->m_pPacket->m_wType ) {
#ifdef	__INC_WORLD
		case CLI_CHAR_LIST :
			Proc_cli_CHAR_LIST	( pSqlPACKET );
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
		//case CLI_CLAN_COMMAND :
		//	Proc_cli_CLAN_COMMAND( pSqlPACKET );
		//	break;
#endif
		case CLI_MALL_ITEM_REQ :
			Proc_cli_MALL_ITEM_REQ( pSqlPACKET );
			break;

		case CLI_SELECT_CHAR :
			Proc_cli_SELECT_CHAR( pSqlPACKET );
			break;

		case CLI_BANK_LIST_REQ :
			Proc_cli_BANK_LIST_REQ( pSqlPACKET );
			break;

		case SQL_ZONE_DATA :
		{
			sql_ZONE_DATA *pSqlZONE = (sql_ZONE_DATA*)pSqlPACKET->m_pPacket;
			switch( pSqlZONE->m_btDataTYPE ) {
				case SQL_ZONE_DATA_NPCOBJ_LOAD	:
				case SQL_ZONE_DATA_EVENTOBJ_LOAD:	
					Proc_LOAD_OBJVAR( pSqlPACKET );
					break;

				case SQL_ZONE_DATA_ECONOMY_LOAD :		// 존 쓰레드가 아래 함수안에서 시작된다.
					Proc_LOAD_ZONE_DATA( pSqlPACKET->m_iTAG );
					break;

			#ifndef	__INC_WORLD
				case SQL_ZONE_DATA_ECONOMY_SAVE :
					Proc_SAVE_ZONE_DATA( pSqlPACKET->m_iTAG, pSqlZONE );
					break;

				case SQL_ZONE_DATA_NPCOBJ_SAVE	:
				case SQL_ZONE_DATA_EVENTOBJ_SAVE:	
					Proc_SAVE_OBJVAR( pSqlPACKET );
					break;

				case SQL_ZONE_DATA_WORLDVAR_SAVE :
					Proc_SAVE_WORLDVAR( pSqlZONE );
					break;
			#endif
			}
			break;
		}

		default :
			g_LOG.CS_ODS( 0xffff, "Undefined sql packet Type: %x, Size: %d \n", pSqlPACKET->m_pPacket->m_wType, pSqlPACKET->m_pPacket->m_nSize);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
struct tagDelCHAR {
	DWORD	m_dwDBID;
	CStrVAR	m_Name;
} ;
bool GS_CThreadSQL::Proc_cli_CHAR_LIST( tagQueryDATA *pSqlPACKET )
{
	if ( !this->m_pSQL->QuerySQL( "{call ws_GetCharLIST(\'%s\')}", pSqlPACKET->m_Name.Get() ) ) {
/*
	this->m_pSQL->MakeQuery( "SELECT txtNAME, binBasicE, binBasicI, binGrowA, dwDelTIME FROM tblGS_AVATAR WHERE txtACCOUNT=", 
							MQ_PARAM_STR, pSqlPACKET->m_Name.Get(), 
							MQ_PARAM_END);
	if ( !this->m_pSQL->QuerySQLBuffer() ) {
*/
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
		short nC = 0;
		//CSLList< CStrVAR >  DelList;
		//CSLList< CStrVAR >::tagNODE *pNode;
		CSLList< tagDelCHAR >  DelList;
		CSLList< tagDelCHAR >::tagNODE *pNode;
		DWORD dwDelSEC, dwCurAbsSEC = this->m_dwCurTIME;
		do { 
			if ( this->m_pSQL->GetStrPTR( 0 ) ) {
				szCharName = this->m_pSQL->GetStrPTR ( 0 );
				dwDelSEC = this->m_pSQL->GetInteger( 4 );
				if ( dwDelSEC && dwCurAbsSEC >= dwDelSEC ) {
					// 삭제...
					//pNode = new CSLList< CStrVAR >::tagNODE;
					//pNode->m_VALUE.Set( szCharName );
					pNode = new CSLList< tagDelCHAR >::tagNODE;
					pNode->m_VALUE.m_Name.Set( szCharName );
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
					sCHAR.m_btCharRACE	= pBE->m_btCharRACE;
					sCHAR.m_nJOB		= pBI->m_nClass;
					sCHAR.m_nLEVEL		= pGA->m_nLevel;
					sCHAR.m_dwRemainSEC = dwDelSEC;
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
					pCPacket->AppendData (&sCHAR,	sizeof( tagCHARINFO ) );
					pCPacket->AppendData (pBE->m_PartITEM, sizeof(tagPartITEM)*MAX_BODY_PART );

					pCPacket->m_wsv_CHAR_LIST.m_btCharCNT ++;
				}
			}
		} while( this->m_pSQL->GetNextRECORD() && ++nC<MAX_CHAR_PER_USER ) ;

		g_pUserLIST->SendPacketToSocketIDX( pSqlPACKET->m_iTAG, pCPacket );

		//pNode = DelList.GetHeadNode ();
		//while( pNode ) {
		//	if ( this->m_pSQL->ExecSQL("DELETE FROM tblGS_AVATAR WHERE txtNAME=\'%s\'", pNode->m_VALUE.Get() ) < 1 ) {
		//		// 오류 또는 삭제된것이 없다.
		//		g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR(DELETE_CHAR:%s):: %s \n", pNode->m_VALUE.Get(), m_pSQL->GetERROR() );
		//	}
		//	DelList.DeleteHeadNFree ();
		//	pNode = DelList.GetHeadNode ();
		//} ;
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
		g_pUserLIST->SendPacketToSocketIDX( pSqlPACKET->m_iTAG, pCPacket );
	}
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Proc_cli_SELECT_CHAR( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	#define	MAX_AVATAR_NAME	20
	short nOffset=sizeof(cli_SELECT_CHAR), nOutStrLen;
	char *pCharName=Packet_GetStringPtr( pPacket, nOffset, nOutStrLen );
	if ( !pCharName || nOutStrLen < 4 ) {
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: CharName == NULL \n" );
		return false;
	}

	if ( nOutStrLen > MAX_AVATAR_NAME) {
		g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_SELECT_CHAR:: CharName == '%s'\n", pCharName );
		return false;
	}

	// "SELECT * FROM tblGS_AVATAR WHERE txtNAME='xxxx';"
	if ( !this->m_pSQL->QuerySQL( "{call gs_SelectCHAR(\'%s\')}", pCharName ) ) {
/*
	this->m_pSQL->MakeQuery( "SELECT * FROM tblGS_AVATAR WHERE txtNAME=", 
							MQ_PARAM_STR, pCharName, 
							MQ_PARAM_END);
	if ( !this->m_pSQL->QuerySQLBuffer() ) {
//*/
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		return false;
	}

	if ( !this->m_pSQL->GetNextRECORD() ) {
		// 케릭터 없다.
		g_LOG.CS_ODS(LOG_NORMAL, "Char[ %s ] not found ...\n", pCharName );
		return false;
	}

	bool bResult;
	classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( pUSER ) {
		classPACKET *pCPacket = Packet_AllocNLock ();
		if ( pCPacket ) {
			// 정상 처리...
			// Get_ACCOUNT == this->m_pSQL->GetStrPTR( Account ) ???
#ifdef __KCHS_BATTLECART__
			short nDataVER = this->m_pSQL->GetInteger16( AVTTBL_DATA_VER );
#endif
			pUSER->m_dwDBID = this->m_pSQL->GetInteger( AVTTBL_CHARID );

			pUSER->Set_NAME( pCharName );
			pUSER->Set_RNAME( pCharName );
			g_pUserLIST->Add_CHAR( pUSER );

			tagBasicETC *pBE;
			pBE = (tagBasicETC*)this->m_pSQL->GetDataPTR( AVTTBL_BASIC_ETC );

			pUSER->m_nCharRACE		= pBE->m_btCharRACE;
#ifdef __KCHS_BATTLECART__ // __OLD_DATA_COMPATIBLE__
			if( nDataVER < DATA_VER_2 )
			{
				pUSER->m_btPlatinumCHAR = *((BYTE *)(&pBE->m_RideITEM[ RIDE_PART_ARMS ]));
			}
			else
				pUSER->m_btPlatinumCHAR = pBE->m_btCharSlotNO;
#else
			pUSER->m_btPlatinumCHAR = pBE->m_btCharSlotNO;
#endif

			CZoneTHREAD *pZONE = g_pZoneLIST->GetZONE( pBE->m_nZoneNO );
			if ( pZONE ) {
				pUSER->m_nZoneNO   = pBE->m_nZoneNO;

				if ( pBE->m_PosSTART.x == 0.f || pBE->m_PosSTART.y == 0.f ) {
					/// 샷다운시 존이 모두 삭제되어 가까운 부활 위치를 못찾아 0으로 셋팅됏다 ... 2.26
					pBE->m_PosSTART = pZONE->Get_StartRevivePOS ();
				} else {
					// 기본 현재 위치의 가장 가까운 부활 장소로...
					pBE->m_PosSTART = pZONE->Get_RevivePOS( pBE->m_PosSTART );
					pBE->m_PosSTART.x += ( RANDOM(1001) - 500 );	// 랜덤 5미터..
					pBE->m_PosSTART.y += ( RANDOM(1001) - 500 );
				}

				pUSER->m_PosCUR    = pBE->m_PosSTART;
			} else {
				// 혹시 버그로 인해 존이 삭제 되었을경우..
				pUSER->m_nZoneNO	  = AVATAR_ZONE( pBE->m_btCharRACE );
				pZONE = g_pZoneLIST->GetZONE( pUSER->m_nZoneNO );
				if( !pZONE ) // 존이 없을 경우 디폴트로 존 1을...
				{
					pUSER->m_nZoneNO = 1;
					pZONE = g_pZoneLIST->GetZONE( pUSER->m_nZoneNO );
				}
				pUSER->m_PosCUR		  = pZONE->Get_StartPOS();
			}

			
			/// 2005. 08. 24 :: 임시로 클랜필드에서 접속할경우 2번존(주논폴리스) 부활위치로 강제 이동
			if ( pUSER->m_nZoneNO >= 11 && pUSER->m_nZoneNO <= 13 ) {
				int iJunonPolice = 2;
				CZoneTHREAD *pZone2 = g_pZoneLIST->GetZONE( iJunonPolice );
				pUSER->m_nZoneNO = pZone2->Get_ZoneNO();
				pUSER->m_PosCUR = pZone2->Get_StartRevivePOS();
			} // if ( pUSER->m_nZoneNO >= 11 && pUSER->m_nZoneNO <= 13 )


			// 일정 주기로 사용자 정보 저장하기 위해서..
			pUSER->m_dwBackUpTIME = pZONE->GetCurrentTIME();

			if ( pBE->m_nReviveZoneNO <= 0 ) {
				// 역시 버그로 인해 부활존이 없으면...
				pUSER->m_nReviveZoneNO = m_pDefaultBE[ pBE->m_btCharRACE ].m_nReviveZoneNO;
				pUSER->m_PosREVIVE     = m_pDefaultBE[ pBE->m_btCharRACE ].m_PosREVIVE;
			} else {
				pUSER->m_nReviveZoneNO = pBE->m_nReviveZoneNO;
				pUSER->m_PosREVIVE     = pBE->m_PosREVIVE;
			}

			::CopyMemory( pUSER->m_PartITEM, pBE->m_PartITEM, sizeof(tagPartITEM)*MAX_BODY_PART	);

#ifdef __KCHS_BATTLECART__ // __OLD_DATA_COMPATIBLE__
			if( nDataVER < DATA_VER_2 )
			{
				::CopyMemory( pUSER->m_RideITEM, pBE->m_RideITEM, sizeof(tagPartITEM)*(MAX_RIDING_PART-1));
				ZeroMemory( &pUSER->m_RideITEM[ RIDE_PART_ARMS ], sizeof(tagPartITEM));
			}
			else
				::CopyMemory( pUSER->m_RideITEM, pBE->m_RideITEM, sizeof(tagPartITEM)*MAX_RIDING_PART );
#else
			::CopyMemory( pUSER->m_RideITEM, pBE->m_RideITEM, sizeof(tagPartITEM)*MAX_RIDING_PART );
#endif

			BYTE *pDATA;
			pDATA = this->m_pSQL->GetDataPTR( AVTTBL_BASIC_INFO );
			::CopyMemory( &pUSER->m_BasicINFO,		pDATA,	sizeof( tagBasicINFO ) );
			
			pDATA = this->m_pSQL->GetDataPTR( AVTTBL_BASIC_ABILITY );
			::CopyMemory( &pUSER->m_BasicAbility,	pDATA,	sizeof( tagBasicAbility ) );

			pDATA = this->m_pSQL->GetDataPTR( AVTTBL_GROW_ABILITY );
			::CopyMemory( &pUSER->m_GrowAbility,	pDATA,	sizeof( tagGrowAbility ) );
#ifdef	__KCHS_BATTLECART__
			if( pUSER->GetCur_PatHP() > PAT_COOLTIME ) // 이렇다는 얘기는 0 - 1 한 잘못 된 데이터
				pUSER->SetCur_PatHP( 0 );
#endif
			// skill
			pDATA = this->m_pSQL->GetDataPTR( AVTTBL_SKILL_ABILITY );
			::CopyMemory( &pUSER->m_Skills,			pDATA,	sizeof( tagSkillAbility ) );

			// quest 
			pDATA = this->m_pSQL->GetDataPTR( AVTTBL_QUEST );
			if ( pDATA ) {
				::CopyMemory( &pUSER->m_Quests,			pDATA,	sizeof( tagQuestData ) );
				pUSER->m_Quests.CheckExpiredTIME();
			} else
				pUSER->m_Quests.Init ();

			// inventory...
			pDATA = this->m_pSQL->GetDataPTR( AVTTBL_INVENTORY );
			::CopyMemory( &pUSER->m_Inventory,		pDATA,	sizeof( CInventory ) );

			// wish list
			pDATA = this->m_pSQL->GetDataPTR( AVTTBL_WISHLIST );
			if ( pDATA )
				::CopyMemory( &pUSER->m_WishLIST,		pDATA,	sizeof( tagWishLIST ) );
			else
				pUSER->m_WishLIST.Init ();

			pUSER->m_i64StartMoney = pUSER->GetCur_MONEY();		// 돈 변화량을 체크하기 위해서...

			// 잘못된 아이템일 경우 삭제...
			tagITEM *pITEM;
			short nI;
			for (nI=EQUIP_IDX_NULL+1; nI<MAX_EQUIP_IDX; nI++) {
				pITEM = &pUSER->m_Inventory.m_ItemLIST[ nI ];
				if ( !pITEM->GetHEADER() )
					continue;
				if ( !pITEM->IsEquipITEM() || !pITEM->IsValidITEM() ) {
					// 이상한 아이템이다...
					g_pThreadLOG->When_ItemHACKING( pUSER, pITEM, "ItemHACK" );
					pITEM->Clear ();
					continue;
				}
			}

			for (nI=EQUIP_IDX_FACE_ITEM; nI<INVENTORY_TOTAL_SIZE; nI++) {
				pITEM = &pUSER->m_Inventory.m_ItemLIST[ nI ];
				if ( !pITEM->GetHEADER() )
					continue;

				if ( !pITEM->GetTYPE() || pITEM->GetTYPE() > ITEM_TYPE_RIDE_PART ) {
					pITEM->Clear ();
					continue;
				}

				// 아이템 해킹 케릭인지 조사...
				if ( pITEM->IsEnableDupCNT() ) {
					if ( pITEM->GetQuantity() > MAX_DUP_ITEM_QUANTITY ) {
						g_pThreadLOG->When_ItemHACKING( pUSER, pITEM, "ItemHACK" );
						pUSER->m_Inventory.m_i64Money = 0;
						pITEM->Clear ();
						continue;
					}
				} else 
				if ( pITEM->GetOption() ) {
					if ( pITEM->GetOption() >= g_TblGEMITEM.m_nDataCnt ) {
						// 창고로 이동시 잘못됐던 버그 아이템이다...
						pITEM->m_nGEM_OP = 0;
					}
				}
				if ( pITEM->GetItemNO() >= g_pTblSTBs[ pITEM->GetTYPE() ]->m_nDataCnt ) {
					pITEM->Clear ();
					continue;
				}
			}

			pDATA = this->m_pSQL->GetDataPTR( AVTTBL_HOTICON );
			if ( pDATA )
				::CopyMemory( &pUSER->m_HotICONS,		pDATA,	sizeof( CHotICONS ) );
			else
				pUSER->m_HotICONS.Init ();

			COMPILE_TIME_ASSERT( sizeof( CHotICONS ) == sizeof(tagHotICON) * MAX_HOT_ICONS );

			DWORD dwPassMIN, dwDisMIN;
			/*
			dwPassMIN = (DWORD) this->m_pSQL->GetInteger( AVTTBL_RIGHT );
			if ( dwPassMIN >= pUSER->m_dwRIGHT )
				pUSER->m_dwRIGHT = dwPassMIN;
			*/
			dwDisMIN  = (DWORD)this->m_pSQL->GetInteger( AVTTBL_REG_TIME );
			if ( this->m_dwCurTIME > dwDisMIN ) {
				dwPassMIN = (DWORD)( ( this->m_dwCurTIME - dwDisMIN ) / 60 );
				//if ( dwPassMIN >= ( MAX_STAMINA / 17 ) ) {
				//	pUSER->SetCur_STAMINA( MAX_STAMINA );
				//} else 
				{
					int iStamina = pUSER->GetCur_STAMINA () + ( 2 * dwPassMIN );
					pUSER->SetCur_STAMINA( iStamina>=MAX_STAMINA ? MAX_STAMINA : iStamina );
				}
			} else {
				dwPassMIN = 0;
				if ( pUSER->GetCur_STAMINA() > MAX_STAMINA )
					pUSER->SetCur_STAMINA( MAX_STAMINA );
			}

			// 스킬 데이타에서 패시브 스킬에서 얻은 능력치들을 정리...
			pUSER->InitPassiveSkill ();

			// 패킷을 보내기 전에 능력치를 계산해야 할듯...
			// 아래 함수 실행중 클라이언트에서 패킷이 도착해서
			// 진행되어 질수 있다... 운이 나쁘면 ㅡ,.ㅡ;
			pUSER->UpdateAbility ();
			if ( pUSER->Get_HP() <= 0 ) {
				// 죽었다면 30% hp
				pUSER->Set_HP( 3*pUSER->Get_MaxHP() / 10 );
			} else
			if ( pUSER->Get_HP() > pUSER->Get_MaxHP() )
				pUSER->Set_HP( pUSER->Get_MaxHP() );

			pUSER->Set_ShotITEM ();			// GS_CThreadSQL::Proc_cli_SELECT_CHAR

			if ( pPacket->m_cli_SELECT_CHAR.m_btCharNO )
			{
				// 클라이언트에게 정보를 보내야 하는가 ???
				pCPacket->m_HEADER.m_wType = GSV_SELECT_CHAR;
				pCPacket->m_HEADER.m_nSize = sizeof( gsv_SELECT_CHAR );

				pCPacket->m_gsv_SELECT_CHAR.m_btCharRACE	= pBE->m_btCharRACE;
				pCPacket->m_gsv_SELECT_CHAR.m_nZoneNO       = pUSER->m_nZoneNO;
				pCPacket->m_gsv_SELECT_CHAR.m_PosSTART      = pUSER->m_PosCUR;
				pCPacket->m_gsv_SELECT_CHAR.m_nReviveZoneNO = pUSER->m_nReviveZoneNO;
				pCPacket->m_gsv_SELECT_CHAR.m_dwUniqueTAG	= pUSER->m_dwDBID;

				::CopyMemory(  pCPacket->m_gsv_SELECT_CHAR.m_PartITEM,		pUSER->m_PartITEM,			sizeof(tagPartITEM)*MAX_BODY_PART );

				::CopyMemory( &pCPacket->m_gsv_SELECT_CHAR.m_BasicINFO,		&pUSER->m_BasicINFO,		sizeof( tagBasicINFO ) );
				::CopyMemory( &pCPacket->m_gsv_SELECT_CHAR.m_BasicAbility,	&pUSER->m_BasicAbility,		sizeof( tagBasicAbility ) );

				#pragma COMPILE_TIME_MSG( "********** 여기서는 tagGrowAbility도 크기가 바뀌었다 ..." )
				::CopyMemory( &pCPacket->m_gsv_SELECT_CHAR.m_GrowAbility,	&pUSER->m_GrowAbility,		sizeof( tagGrowAbility ) );

				::CopyMemory( &pCPacket->m_gsv_SELECT_CHAR.m_Skill,			&pUSER->m_Skills,			sizeof( tagSkillAbility ) );
				::CopyMemory( &pCPacket->m_gsv_SELECT_CHAR.m_HotICONS,		&pUSER->m_HotICONS,			sizeof( CHotICONS ) );
				pCPacket->AppendString ( pUSER->Get_NAME() );
				COMPILE_TIME_ASSERT( sizeof( gsv_SELECT_CHAR ) < MAX_PACKET_SIZE );
				pUSER->SendPacket( pCPacket );

				pUSER->Send_gsv_INVENTORYnQUEST_DATA ();

				// 파티번호가 있냐...
				unsigned int iPartyIDX = (unsigned int)this->m_pSQL->GetInteger( AVTTBL_PARTY_IDX );
				if ( iPartyIDX && dwPassMIN < 7 ) {		// 튕기고 5분안에 재접이면(서번6분), 클라이언트에서 파장이 5분되면 자동 강퇴요청 해야됨
					g_pPartyBUFF->OnConnect( iPartyIDX, pUSER );
				}

				// 아이템 시리얼번호 초기값 갱신...
				DWORD dwDBItemSN = this->m_pSQL->GetInteger( AVTTBL_ITEM_SN );
				if ( this->m_dwCurTIME >= dwDBItemSN )
					pUSER->m_dwItemSN = this->m_dwCurTIME;
				else
					pUSER->m_dwItemSN = dwDBItemSN;
			} else {
				// 존에 참가하라는 패킷 전송...
				pUSER->m_bRunMODE   = pPacket->m_cli_SELECT_CHAR.m_btRunMODE;
				pUSER->m_btRideMODE = pPacket->m_cli_SELECT_CHAR.m_btRideMODE;
				pUSER->Send_gsv_TELEPORT_REPLY( pUSER->m_PosCUR, pUSER->m_nZoneNO );
			}

			Packet_ReleaseNUnlock( pCPacket );

			pUSER->m_Bank.Init ();
			pUSER->m_btBankData = BANK_UNLOADED;

			pUSER->m_dwLoginTIME = this->m_dwCurTIME;

			// GS_LogIN 테이블에...계정 등록...
			this->Add_LoginACCOUNT( pUSER->Get_ACCOUNT() );

			#ifdef	__NEW_LOG
				g_pThreadLOG->When_LogInOrOut( pUSER, NEWLOG_LOGIN );
			#else
				g_pThreadLOG->When_LogIN( pUSER );
			#endif

			bResult = true;
		} else {
			// 패킷 할당 실패...
			bResult = false;
		}
	} else {
		// 접속 끊겼다.
		bResult = false;
	}	
	return bResult;
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
bool GS_CThreadSQL::Proc_cli_CREATE_CHAR( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	short nOffset=sizeof(cli_CREATE_CHAR);
	char *pCharName=Packet_GetStringPtr( pPacket, nOffset );
	if ( !pCharName ) {
		return false;
	}

	if ( pPacket->m_cli_CREATE_CHAR.m_btCharRACE >= m_nDefaultDataCNT ) {
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return true;
	}

	short nDefRACE = pPacket->m_cli_CREATE_CHAR.m_btCharRACE;
	CZoneTHREAD *pZONE = g_pZoneLIST->GetZONE( AVATAR_ZONE( nDefRACE ) );
	if ( !pZONE ) {
		g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_CREATE_CHAR:: Invalid Zone %d, Race: %d \n", AVATAR_ZONE( nDefRACE ), nDefRACE );
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return true;
	}

	// "SELECT * FROM tblGS_AVATAR WHERE Name='xxxx';"
	this->m_pSQL->MakeQuery( "SELECT intCharID FROM tblGS_AVATAR WHERE txtNAME=", 
							MQ_PARAM_STR, pCharName,
							MQ_PARAM_END );
	if ( !this->m_pSQL->QuerySQLBuffer () ) {
		// ???
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		return false;
	}

	if ( m_pSQL->GetNextRECORD() ) {
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
		return false;
	}
	
	if ( m_pSQL->GetNextRECORD() && m_pSQL->GetInteger( 0 ) >= MAX_CHAR_PER_USER ) {
		// 더이상 못만든다.
		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_NO_MORE_SLOT );
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
	m_pDefaultBE[ nDefRACE ].m_btCharRACE = (BYTE)nDefRACE;
	m_pDefaultBE[ nDefRACE ].m_nZoneNO    = BEGINNER_ZONE;
	//m_pDefaultBE[ nDefRACE ].m_PosSTART   = g_ZoneLIST.Get_StartPOS( nZoneIDX );
	m_pDefaultBE[ nDefRACE ].m_PosSTART   = s_BeginnerPOS[ wPosBeginner ];
	
	// 초기 부활장소 지정...
	short nDefReviveZoneNO = IsTAIWAN() ? BEGINNER_ZONE : ADVENTURE_ZONE;

	pZONE = g_pZoneLIST->GetZONE( nDefReviveZoneNO );
	m_pDefaultBE[ nDefRACE ].m_nReviveZoneNO = pZONE->Get_ZoneNO();
	m_pDefaultBE[ nDefRACE ].m_PosREVIVE     = pZONE->Get_StartRevivePOS();

	// 사용자 선택에의해 변화되는 값들...
	m_sBI.Init ( pPacket->m_cli_CREATE_CHAR.m_cBoneSTONE, pPacket->m_cli_CREATE_CHAR.m_cFaceIDX, pPacket->m_cli_CREATE_CHAR.m_cHairIDX );

//	m_pDefaultBE[ nDefRACE ].m_nPartItemIDX[ BODY_PART_FACE		] = m_sBI.m_cFaceIDX;
//	m_pDefaultBE[ nDefRACE ].m_nPartItemIDX[ BODY_PART_HAIR		] = m_sBI.m_cHairIDX;
	m_pDefaultBE[ nDefRACE ].m_PartITEM[ BODY_PART_FACE	].m_nItemNo = m_sBI.m_cFaceIDX;
	m_pDefaultBE[ nDefRACE ].m_PartITEM[ BODY_PART_HAIR	].m_nItemNo = m_sBI.m_cHairIDX;

	this->m_pSQL->BindPARAM( 1, (BYTE*)&m_pDefaultBE[ nDefRACE ],	sizeof(tagBasicETC) );
	this->m_pSQL->BindPARAM( 2, (BYTE*)&m_sBI,						sizeof(m_sBI) );
	this->m_pSQL->BindPARAM( 3, (BYTE*)&m_pDefaultBA[ nDefRACE ],	sizeof(tagBasicAbility) );
	this->m_pSQL->BindPARAM( 4, (BYTE*)&m_sGB,						sizeof(tagGrowAbility) );
	this->m_pSQL->BindPARAM( 5, (BYTE*)&m_sSA,						sizeof(m_sSA) );
	this->m_pSQL->BindPARAM( 6, (BYTE*)&m_pDefaultINV[ nDefRACE ],	sizeof(CInventory) );
	this->m_pSQL->BindPARAM( 7, (BYTE*)&m_sQD,						sizeof(m_sQD) );
	this->m_pSQL->BindPARAM( 8, (BYTE*)&m_HotICON,					sizeof(m_HotICON) );

	// "INSERT tblGS_AVATAR (Account, Name, ZoneNO, BasicINFO) VALUSE( xxx, xxx, xxx, xxx );"
#ifdef __KCHS_BATTLECART__ // __OLD_DATA_COMPATIBLE__
	// short nNewDataVER = DATA_VER_2;
	// this->m_pSQL->BindPARAM( 9, (BYTE*)&nNewDataVER,				sizeof(short) );
	this->m_pSQL->MakeQuery("INSERT tblGS_AVATAR (txtACCOUNT, txtNAME, intDataVER, binBasicE, binBasicI, binBasicA, binGrowA, binSkillA, blobINV, blobQUEST, binHotICON ) VALUES(",
											MQ_PARAM_STR,		pSqlPACKET->m_Name.Get(),
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		pCharName,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_INT16,		DATA_VER_2,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	1,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	2,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	3,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	4,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	5,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	6,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	7,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	8,
				MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END);
#else
	this->m_pSQL->MakeQuery("INSERT tblGS_AVATAR (txtACCOUNT, txtNAME, binBasicE, binBasicI, binBasicA, binGrowA, binSkillA, blobINV, blobQUEST, binHotICON) VALUES(",
											MQ_PARAM_STR,		pSqlPACKET->m_Name.Get(),
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		pCharName,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	1,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	2,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	3,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	4,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	5,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	6,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	7,
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	8,
				MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END);
#endif
	if ( this->m_pSQL->ExecSQLBuffer() < 1 ) {
		// 오류 또는 만들어진것이 없다.
		g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR:: %s \n", m_pSQL->GetERROR() );

		g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_FAILED );
		return true;
	}

	classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( pUSER ) {
#ifdef	__NEW_LOG
		g_pThreadLOG->When_CharacterLOG( pUSER, pCharName, NEWLOG_NEW_CHAR );
#else
		g_pThreadLOG->When_CreateCHAR( pUSER, pCharName );
#endif
	}

	// 만들어 졌다..
	g_pUserLIST->Send_wsv_CREATE_CHAR( pSqlPACKET->m_iTAG, RESULT_CREATE_CHAR_OK );
	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Proc_cli_DELETE_CHAR( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	short nOffset=sizeof(cli_DELETE_CHAR);
	char *pCharName=Packet_GetStringPtr( pPacket, nOffset );
	if ( !pCharName || !pSqlPACKET->m_Name.Get() ) {
		return false;
	}

	DWORD dwCurAbsSEC=0, dwReaminSEC=0;
	if ( pPacket->m_cli_DELETE_CHAR.m_bDelete ) {
		// 삭제 대기
		dwCurAbsSEC = classTIME::GetCurrentAbsSecond() + DELETE_CHAR_WAIT_TIME;
		dwReaminSEC = DELETE_CHAR_WAIT_TIME;
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
		g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR:: %s \n", m_pSQL->GetERROR() );
		return true;
	}
	*/
	classUSER *pFindUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
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
bool GS_CThreadSQL::Proc_cli_BANK_LIST_REQ( tagQueryDATA *pSqlPACKET )
{
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	if ( BANK_REQ_CHANGE_PASSWORD == pPacket->m_cli_BANK_LIST_REQ.m_btREQ ) {
		// 창고 비번 변경...
		if ( pPacket->m_HEADER.m_nSize > sizeof( cli_BANK_LIST_REQ ) ) {
			short nOffset=sizeof( cli_BANK_LIST_REQ );
			char *szPassWD = Packet_GetStringPtr( pPacket, nOffset );
			if ( szPassWD && this->m_pSQL->ExecSQL("UPDATE tblGS_AVATAR SET txtPASSWORD=\'%s\' WHERE txtACCOUNT=\'%s\'", szPassWD, pSqlPACKET->m_Name.Get() ) < 1 ) {
				classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
				if ( pUSER ) {
					pUSER->Send_gsv_BANK_LIST_REPLY( BANK_REPLY_CHANGED_PASSWORD );
				}
			}
		} else {
			if ( this->m_pSQL->ExecSQL("UPDATE tblGS_AVATAR SET txtPASSWORD=NULL WHERE txtACCOUNT=\'%s\'", pSqlPACKET->m_Name.Get() ) < 1 ) {
				classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
				if ( pUSER ) {
					pUSER->Send_gsv_BANK_LIST_REPLY( BANK_REPLY_CLEARED_PASSWORD );
				}
			}
		}
		return true;
	}

	if ( !this->m_pSQL->QuerySQL( "{call gs_SelectBANK(\'%s\')}", pSqlPACKET->m_Name.Get() ) ) {
/*
	this->m_pSQL->MakeQuery( "SELECT * FROM tblGS_BANK WHERE txtACCOUNT=", 
							MQ_PARAM_STR, pSqlPACKET->m_Name.Get(),
							MQ_PARAM_END);
	if ( !this->m_pSQL->QuerySQLBuffer() ) {
*/
		// ???
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		return false;
	}

	if ( !this->m_pSQL->GetNextRECORD() ) {
		// LogString(LOG_NORMAL, "Create [ %s ] bank...\n", pSqlPACKET->m_Name.Get() );

		this->m_pSQL->BindPARAM( 1, (BYTE*)&this->m_sEmptyBANK,		sizeof(tagBankData) );

		// "INSERT tblGS_AVATAR (Account, Name, ZoneNO, BasicINFO) VALUSE( xxx, xxx, xxx, xxx );"
		this->m_pSQL->MakeQuery("INSERT tblGS_BANK (txtACCOUNT, blobITEMS) VALUES(",
												MQ_PARAM_STR,		pSqlPACKET->m_Name.Get(),
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	1,
					MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END);
		if ( this->m_pSQL->ExecSQLBuffer() < 1 ) {
			// 만들기 실패 !!!
			g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: INSERT Bank:%s : %s \n", pSqlPACKET->m_Name.Get(), m_pSQL->GetERROR() );
			return true;
		}

		classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
		if ( !pUSER || !pUSER->Get_ACCOUNT() )
			return false;
		
		return pUSER->Send_gsv_BANK_ITEM_LIST (true);
	}

	classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( !pUSER || !pUSER->Get_ACCOUNT() )
		return false;

	BYTE *pDATA;
	pDATA = this->m_pSQL->GetDataPTR( BANKTBL_ITEMS );
	if ( pDATA ) {
		::CopyMemory( &pUSER->m_Bank,	pDATA,	sizeof( tagBankData ) );

		char *szPassword;
		szPassword = this->m_pSQL->GetStrPTR( BANKTLB_PASSWORD );
		if ( szPassword ) {
			// 비밀번호 체크..
			pUSER->m_BankPASSWORD.Set( szPassword );
			if ( pPacket->m_HEADER.m_nSize > sizeof( cli_BANK_LIST_REQ ) ) {
				short nOffset=sizeof( cli_BANK_LIST_REQ );
				char *szPassWD = Packet_GetStringPtr( pPacket, nOffset );
				if ( !szPassWD || strcmp( szPassword, szPassWD ) )
					return pUSER->Send_gsv_BANK_LIST_REPLY( BANK_REPLY_INVALID_PASSWORD );
			} else {
				// 비번 필요...
				return pUSER->Send_gsv_BANK_LIST_REPLY( BANK_REPLY_NEED_PASSWORD );
			}
		}

		int iRewardMoney = this->m_pSQL->GetInteger( BANKTBL_REWARD );
		if ( iRewardMoney ) {
			// 보상금이 있다...
			this->m_pSQL->MakeQuery( "UPDATE tblGS_BANK SET intREWARD=",
															MQ_PARAM_INT,   0,
					MQ_PARAM_ADDSTR, "WHERE txtACCOUNT=",	MQ_PARAM_STR,	pUSER->Get_ACCOUNT(),
															MQ_PARAM_END );
			if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
				// 고치기 실패 !!!
				g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE Bank money :%s %s \n", pUSER->Get_ACCOUNT(), m_pSQL->GetERROR() );
			} else {
				// 상금 더함.
				pUSER->Add_MoneyNSend( iRewardMoney, GSV_REWARD_MONEY );
			}
		}
	}

	return pUSER->Send_gsv_BANK_ITEM_LIST ();
}
/*
bool GS_CThreadSQL::Proc_cli_CHANGE_BANK_PASSWORD( tagQueryDATA *pSqlPACKET )
{
	xxxx
}
*/
//-------------------------------------------------------------------------------------------------
#define WSVAR_TBL_BLOB		2
#define	ZONE_VAR_ECONOMY	"%s_EC_Zone%d"

bool GS_CThreadSQL::Proc_LOAD_ZONE_DATA( int iZoneNO )
{
	CZoneTHREAD *pZONE = g_pZoneLIST->GetZONE( iZoneNO );
	assert( pZONE );

	m_TmpSTR.Printf (ZONE_VAR_ECONOMY, CLIB_GameSRV::GetInstance()->GetServerName(), iZoneNO );

	this->m_pSQL->MakeQuery( "SELECT * FROM tblWS_VAR WHERE txtNAME=", 
							MQ_PARAM_STR, m_TmpSTR.Get(),
							MQ_PARAM_END);
	if ( !this->m_pSQL->QuerySQLBuffer() ) {
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		return false;
	}

	if ( !this->m_pSQL->GetNextRECORD() ) {
		// insert !!!
		this->m_pSQL->BindPARAM( 1, pZONE->m_Economy.m_pEconomy,	sizeof(tagECONOMY) );

		this->m_pSQL->MakeQuery("INSERT tblWS_VAR (txtNAME, dateUPDATE, binDATA) VALUES(",
												MQ_PARAM_STR,		m_TmpSTR.Get(),
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		g_pThreadLOG->GetCurDateTimeSTR(),
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	1,
					MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END);
		if ( this->m_pSQL->ExecSQLBuffer() < 1 ) {
			g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: INSERT %s : %s \n", m_TmpSTR.Get(), m_pSQL->GetERROR() );
			return true;
		}
	} else {
		BYTE *pDATA = this->m_pSQL->GetDataPTR( WSVAR_TBL_BLOB );

		::CopyMemory( pZONE->m_Economy.m_pEconomy, pDATA,	sizeof( tagECONOMY ) );
	}

	// Start Zone THREAD !!!!
	pZONE->Resume ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Proc_SAVE_ZONE_DATA( int iZoneNO, sql_ZONE_DATA *pSqlZONE )
{
	m_TmpSTR.Printf (ZONE_VAR_ECONOMY, CLIB_GameSRV::GetInstance()->GetServerName(), iZoneNO );

	this->m_pSQL->BindPARAM( 1, pSqlZONE->m_btZoneDATA,	pSqlZONE->m_nDataSIZE );

	this->m_pSQL->MakeQuery( "UPDATE tblWS_VAR SET dateUPDATE=",
												MQ_PARAM_STR,		g_pThreadLOG->GetCurDateTimeSTR(),
			MQ_PARAM_ADDSTR, ",binDATA=",		MQ_PARAM_BINDIDX,	1,
			MQ_PARAM_ADDSTR, "WHERE txtNAME=",	MQ_PARAM_STR,		m_TmpSTR.Get(),
												MQ_PARAM_END );
	if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
		// 고치기 실패 !!!
		g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE %s %s \n", m_TmpSTR.Get(), m_pSQL->GetERROR() );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Proc_LOAD_OBJVAR( tagQueryDATA *pSqlPACKET )
{
	sql_ZONE_DATA *pSqlZONE = (sql_ZONE_DATA*)pSqlPACKET->m_pPacket;
/*
	if ( pSqlZONE->m_btDataTYPE == SQL_ZONE_DATA_NPCOBJ_LOAD )
		m_TmpSTR.Printf( ZONE_VAR_NPCOBJ, pSqlPACKET->m_Name.Get() );
	else
		m_TmpSTR.Printf( ZONE_VAR_EVENTOBJ, pSqlPACKET->m_Name.Get() );
*/
	this->m_pSQL->MakeQuery( "SELECT * FROM tblWS_VAR WHERE txtNAME=", 
							MQ_PARAM_STR, pSqlPACKET->m_Name.Get() /* m_TmpSTR.Get() */,
							MQ_PARAM_END);
	if ( !this->m_pSQL->QuerySQLBuffer() ) {
		g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", m_pSQL->GetERROR() );
		g_LOG.CS_ODS(LOG_NORMAL, "      %s\n", this->m_pSQL->GetQueryBuffPTR() );
		return false;
	}

	if ( !this->m_pSQL->GetNextRECORD() ) {
		// insert !!!
		this->m_pSQL->BindPARAM( 1, (BYTE*)pSqlZONE->m_btZoneDATA,	pSqlZONE->m_nDataSIZE );

		this->m_pSQL->MakeQuery("INSERT tblWS_VAR (txtNAME, dateUPDATE, binDATA) VALUES(",
												MQ_PARAM_STR,		pSqlPACKET->m_Name.Get() /* m_TmpSTR.Get() */,
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,		g_pThreadLOG->GetCurDateTimeSTR(),
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	1,
					MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END);
		if ( this->m_pSQL->ExecSQLBuffer() < 1 ) {
			g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: INSERT %s : %s \n", pSqlPACKET->m_Name.Get()/* m_TmpSTR.Get() */, m_pSQL->GetERROR() );
			return true;
		}
	} else {
		BYTE *pDATA = this->m_pSQL->GetDataPTR( WSVAR_TBL_BLOB );

		switch( pSqlZONE->m_btDataTYPE ) {
			case SQL_ZONE_DATA_NPCOBJ_LOAD   :
				g_pZoneLIST->Init_NpcObjVAR( pSqlPACKET->m_iTAG, pDATA );
				break;
			case SQL_ZONE_DATA_EVENTOBJ_LOAD :
				g_pZoneLIST->Init_EventObjVAR( pSqlPACKET->m_iTAG, pDATA );
				break;
		} 
	}

	return true;
}

bool GS_CThreadSQL::Proc_SAVE_OBJVAR( tagQueryDATA *pSqlPACKET )
{
	sql_ZONE_DATA *pSqlZONE = (sql_ZONE_DATA*)pSqlPACKET->m_pPacket;
/*
	if ( pSqlZONE->m_btDataTYPE == SQL_ZONE_DATA_NPCOBJ_SAVE )
		m_TmpSTR.Printf( ZONE_VAR_NPCOBJ, pSqlPACKET->m_Name.Get() );
	else
		m_TmpSTR.Printf( ZONE_VAR_EVENTOBJ, pSqlPACKET->m_Name.Get() );
*/
	this->m_pSQL->BindPARAM( 1, (BYTE*)pSqlZONE->m_btZoneDATA,	pSqlZONE->m_nDataSIZE );

	this->m_pSQL->MakeQuery( "UPDATE tblWS_VAR SET dateUPDATE=",
												MQ_PARAM_STR,		g_pThreadLOG->GetCurDateTimeSTR(),
			MQ_PARAM_ADDSTR, ",binDATA=",		MQ_PARAM_BINDIDX,	1,
			MQ_PARAM_ADDSTR, "WHERE txtNAME=",	MQ_PARAM_STR,		pSqlPACKET->m_Name.Get()/* m_TmpSTR.Get() */,
												MQ_PARAM_END );
	if ( this->m_pSQL->ExecSQLBuffer() < 0 ) {
		// 고치기 실패 !!!
		g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE %s %s \n", pSqlPACKET->m_Name.Get()/* m_TmpSTR.Get() */, m_pSQL->GetERROR() );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Load_WORLDVAR (BYTE *pVarBUFF, short nBuffLEN)
{
//	this->m_pSQL->MakeQuery( "SELECT binDATA FROM tblWS_VAR WHERE txtNAME=", 
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

bool GS_CThreadSQL::Proc_SAVE_WORLDVAR( sql_ZONE_DATA *pSqlZONE )
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
bool GS_CThreadSQL::Proc_cli_MEMO( tagQueryDATA *pSqlPACKET )
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
			if ( !this->m_pSQL->QuerySQL( "{call ws_GetMEMO(\'%s\')}", pSqlPACKET->m_Name.Get() ) ) {
			/*
			if ( !this->m_pSQL->QuerySQL("SELECT TOP 5 dwDATE, txtFROM, txtMEMO FROM tblWS_MEMO WHERE txtNAME=\'%s\' ORDER BY dwDATE ASC", pSqlPACKET->m_Name.Get() ) ) {
			*/
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
					g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR:: %s \n", m_pSQL->GetERROR() );
					return true;
				}
			} // else 쪽지 없다.
			break;
		}
		case MEMO_REQ_SEND	  :
		{
			/*
				jeddli에게 온 메시지 갯수 구하기. 조건 xxxx는 tblgs_avatar에 존재해야 함
				=========================================================================================================
				SELECT count(*) FROM tblws_memo LEFT JOIN tblgs_avatar ON tblws_memo.txtNAME=tblgs_avatar.txtNAME 
				WHERE tblgs_avatar.txtNAME='xxxx';
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
			if ( !szMemo || strlen(szTargetCHAR) < 4 ) {
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
//bool GS_CThreadSQL::Proc_cli_CLAN_COMMAND( tagQueryDATA *pSqlPACKET )
//{
//	return true;
//}

//-------------------------------------------------------------------------------------------------
bool GS_CThreadSQL::Proc_cli_MALL_ITEM_REQ	( tagQueryDATA *pSqlPACKET )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( NULL == pCPacket )
		return false;
	classUSER *pUSER = (classUSER*)g_pUserLIST->GetSOCKET( pSqlPACKET->m_iTAG );
	if ( NULL == pUSER )
		return false;

	// 몰 아이템을 선물할 케릭터가 존재 하는가 ???
	t_PACKET *pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

	short nOffset = sizeof( cli_MALL_ITEM_REQ );
	char *szCharName = Packet_GetStringPtr( pPacket, nOffset );

	pCPacket->m_HEADER.m_wType = GSV_MALL_ITEM_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_MALL_ITEM_REPLY );

	if ( this->m_pSQL->QuerySQL( "{call gs_GetACCOUNT(\'%s\')}", szCharName ) ) {
		if ( this->m_pSQL->GetNextRECORD() ) {
			// 찾았다..
			char *szAccount = this->m_pSQL->GetStrPTR( 0 );
			// 자기 자신의 계정이면 안됨...
			if ( strcmp(szAccount, pUSER->Get_ACCOUNT() ) ) {
				pUSER->m_MALL.m_DestACCOUNT.Set( szAccount );
				pUSER->m_MALL.m_DestCHAR.Set( szCharName );
				pUSER->m_MALL.m_HashDestCHAR = ::StrToHashKey ( szCharName );

				pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = REPLY_MALL_ITEM_CHECK_CHAR_FOUND;
			} else 
				pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = REPLY_MALL_ITEM_CHECK_CHAR_INVALID;
		} else {
			// 못찾았다..
			pCPacket->m_gsv_MALL_ITEM_REPLY.m_btReplyTYPE = REPLY_MALL_ITEM_CHECK_CHAR_NONE;
		}

		pUSER->Send_Start( pCPacket );
	}

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
