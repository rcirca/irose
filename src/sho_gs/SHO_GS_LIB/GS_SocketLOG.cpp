
#include "LIB_gsMAIN.h"
#include "GS_SocketLOG.h"
#include "GS_ThreadZONE.h"
#include "IO_Quest.h"

#ifndef	__CDB_SOCKET
	#include "GS_ThreadSQL.h"
#endif
/*
enum e_LogTYPE {
	GSLOG_LOGIN=0,
	GSLOG_LOGOUT,

	GSLOG_DEADBY,		// 죽음

	GSLOG_PICK,			// 아이템을 줍기
	GSLOG_DROP,			// 아이템을 바닥에 떨굼
	GSLOG_DELETED,		// 필드 아이템이 사라짐

	GSLOG_CREATED,		// 아이템 제작해서 생성됨.
	GSLOG_DESTROYED,	// 아이템 제작시 실패해서 소멸됨.

	GSLOG_SELL,			// 아이템을 판매
	GSLOG_USE,			// 아이템을 사용해서 소멸됨

	GSLOG_GIVE,			// 아이템을 건네줌
	GSLOG_RECV,			// 아이템을 받음

	GSLOG_DEPOSIT,		// 창고에 보관
	GSLOG_WITHDRAW,		// 창고에서 찾음
} ;
*/

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_ERROR ( classUSER *pUSER, char *szFile, int iLine, char *szDesc )
{
	/*
	txtIP
	txtACCOUNT
	txtCHAR
	txtFILE
	intLINE
	txtDESC
	*/
	m_csSQL.Lock ();
	{
		// 시간, 케릭, 동작, 위치, IP
		this->m_QuerySTR.Printf("INSERT tblGS_ERROR ( dateREG, txtIP, txtACCOUNT, txtCHAR, txtFILE, intLINE, txtDESC ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%d,\'%s\' );",
				this->GetCurDateTimeSTR(),
				pUSER->Get_IP(),
				pUSER->Get_ACCOUNT(),
				pUSER->Get_NAME() ? pUSER->Get_NAME() : "????",

				szFile,
				iLine,
				szDesc 
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_LogIN ( classUSER *pUSER )
{
/*	LogString( LOG_NORMAL, "GSV_SELECT_CHAR( Zone:%d, ObjNO:%d, %s:%s, LEV:%d )\n", 
			pUSER->m_nZoneNO, 
			pUSER->Get_INDEX(),
			pUSER->Get_ACCOUNT(),
			pUSER->Get_NAME(),
			pUSER->Get_LEVEL() );
*/
	m_csSQL.Lock ();
	{
		// 시간, 케릭, 동작, 위치, IP
		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtOBJECT, txtSBJIP, txtACTION, txtLOC ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pUSER->Get_NAME(),
				pUSER->Get_ACCOUNT(),
				pUSER->Get_IP(),

				"LogIN",
				this->GetLocationSTR( pUSER->m_nZoneNO, pUSER->m_PosCUR )
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_LogOUT ( classUSER *pUSER )
{
	m_csSQL.Lock ();
	{
		SYSTEMTIME stIn;

		classTIME::AbsSecondToSystem( pUSER->m_dwLoginTIME, stIn );

		this->m_ItemSTR.Printf("LogIn Time[%d-%d-%d,%d:%d:%d]", stIn.wYear, stIn.wMonth, stIn.wDay, stIn.wHour, stIn.wMinute, stIn.wSecond );

		if ( 0 == pUSER->m_nZoneNO ) {
			m_PosSTR.Printf( "lobby :: Account: %s", pUSER->Get_ACCOUNT() );
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtOBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					"not selected",
					pUSER->Get_ACCOUNT(),
					pUSER->Get_IP(),

					"LogOUT",
					m_PosSTR.Get(),
					this->m_ItemSTR.Get()
					);
		} else {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtOBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pUSER->Get_NAME(),
					pUSER->Get_ACCOUNT(),
					pUSER->Get_IP(),

					"LogOUT",
					this->GetLocationSTR( pUSER->m_nZoneNO, pUSER->m_PosCUR ),
					this->m_ItemSTR.Get()
					);
		}

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_CreateCHAR ( classUSER *pSourAVT, char *szCharNAME )
{
	m_csSQL.Lock ();
	{
		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				szCharNAME,
				pSourAVT->Get_IP(),

				"AddCHAR",
				pSourAVT->Get_ACCOUNT()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_DeleteCHAR ( classUSER *pSourAVT, char *szCharNAME )
{
	m_csSQL.Lock ();
	{
		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				szCharNAME,
				pSourAVT->Get_IP(),

				"DelCHAR",
				pSourAVT->Get_ACCOUNT()
				);
		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
char *GS_logSOCKET::MakeItemDESC( CStrVAR *pStrVAR, tagITEM *pITEM, bool bCreated )
{
	#define	CHECK_ITEM_PRICE	1000
	#define	CHECK_ITEM_MONEY	100000
	if ( pITEM->IsEquipITEM() ) {
		// 장비
		if ( pITEM->IsCreated() ||	// 제조된 아이템이냐 ???
			 ITEM_BASE_PRICE( pITEM->GetTYPE(), pITEM->GetItemNO() ) >= CHECK_ITEM_PRICE )	// 비싼 일반 장비.
		{
			pStrVAR->Printf("[%d:%d]%s, G:%d,D:%d,L:%d, O:%d/%s S:%d:, A:%d", 
					pITEM->GetTYPE(), pITEM->GetItemNO(), 
					ITEM_NAME( pITEM->GetTYPE(), pITEM->GetItemNO() ), 

					pITEM->GetGrade(), pITEM->GetDurability(), pITEM->GetLife(),

					pITEM->GetGemNO(), GEMITEM_NAME( pITEM->GetGemNO() ),
					pITEM->HasSocket(), pITEM->IsAppraisal() );
		} else
			return NULL;
	} else 
	if ( ITEM_TYPE_MONEY == pITEM->GetTYPE() ) {
		// 돈
		if ( pITEM->GetMoney() < CHECK_ITEM_MONEY )
			return NULL;

		pStrVAR->Printf("돈 : %d원", pITEM->GetMoney() );
	} else {
		// 소모/기타
		if ( pITEM->GetQuantity() < 10 )
			return NULL;

		pStrVAR->Printf("[%d:%d]%s : %d개", pITEM->GetTYPE(), pITEM->GetItemNO(), ITEM_NAME( pITEM->GetTYPE(), pITEM->GetItemNO() ), pITEM->GetQuantity() );
	}

	return pStrVAR->Get();
}
char *GS_logSOCKET::GetItemINFO( tagITEM *pITEM, bool bCreated )
{
	return this->MakeItemDESC( &m_ItemSTR, pITEM, bCreated );
}

bool GS_logSOCKET::When_PickITEM ( classUSER *pSourAVT, CObjITEM *pItemOBJ )
{
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( &pItemOBJ->m_ITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"Pick",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pItemOBJ->m_PosCUR ),
					this->m_ItemSTR.Get(),

					pItemOBJ->m_bDropperIsUSER ? pItemOBJ->m_DropperNAME.Get() : "none",
					pItemOBJ->m_bDropperIsUSER ? pItemOBJ->m_DropperIP.Get()   : "none"
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}
bool GS_logSOCKET::When_DropITEM ( classUSER *pSourAVT, CObjITEM *pItemOBJ )
{
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( &pItemOBJ->m_ITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"Drop",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pItemOBJ->m_PosCUR ),
					this->m_ItemSTR.Get()
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}
bool GS_logSOCKET::When_DeletedITEM ( CObjITEM *pItemOBJ )
{
	// 사용자가 버린 필드 아이템 사라짐.
	assert( pItemOBJ->m_bDropperIsUSER );

	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( &pItemOBJ->m_ITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtACTION, txtLOC, txtITEM, txtSUBJECT, txtSBJIP ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),

					"TimeOUT",
					this->GetLocationSTR( pItemOBJ->GetZONE()->Get_ZoneNO(), pItemOBJ->m_PosCUR ),
					this->m_ItemSTR.Get(),

					pItemOBJ->m_DropperNAME.Get(),
					pItemOBJ->m_DropperIP.Get()
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}


//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_Sell2STORE ( classUSER *pSourAVT, tagITEM *pITEM, CObjCHAR *pDestCHR )
{
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( pITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"Sell2Store",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
					this->m_ItemSTR.Get(),

					pDestCHR->Get_NAME(),
					pDestCHR->IsUSER() ? ((classUSER*)pDestCHR)->Get_IP() : "NPC"
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_P2PTRADE ( classUSER *pSourAVT, tagITEM *pITEM, classUSER *pDestAVT, DWORD dwTradeMoney )
{
	// pSour가 pITEM을 pDest에게 dwTradeMoney에 팔았다.
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( pITEM ) ) {
			this->m_DescSTR.Printf("%s가 %I32u원에 판매", pSourAVT->Get_NAME(), dwTradeMoney );

			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP, txtDESC ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"p2pTRADE",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
					this->m_ItemSTR.Get(),

					pDestAVT->Get_NAME(),
					pDestAVT->Get_IP(),

					this->m_DescSTR.Get ()
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}


//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_GiveITEM ( classUSER *pSourAVT, tagITEM *pITEM, classUSER *pDestAVT )
{
	// pSour가 pDest에게 줬다.
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( pITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"Give",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
					this->m_ItemSTR.Get(),

					pDestAVT->Get_NAME(),
					pDestAVT->Get_IP()
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_RecvITEM ( classUSER *pSourAVT, tagITEM *pITEM, classUSER *pDestAVT )
{
	// pSour가 pDest로 부터 받았다.
/*
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( pITEM ) ) {
			m_dwBuffLEN = this->m_pSQL->MakeQuery("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtOBJECT, txtOBJIP ) VALUES (",
												MQ_PARAM_STR,	this->GetCurDateTimeSTR(),
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	pSourAVT->Get_NAME(),
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	pSourAVT->Get_IP(),

					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	"Recv",
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	this->m_ItemSTR.Get(),

					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	pDestAVT->Get_NAME(),
					MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	pDestAVT->Get_IP(),

					MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END );

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();
*/
	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_DEPOSIT ( classUSER *pSourAVT, tagITEM *pITEM )
{
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( pITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"Deposit",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
					this->m_ItemSTR.Get()
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_WithDRAW ( classUSER *pSourAVT, tagITEM *pITEM )
{
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( pITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"WithDRAW",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
					this->m_ItemSTR.Get()
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
// 제조시 생성된 아이템
bool GS_logSOCKET::When_CreatedITEM ( classUSER *pSourAVT, tagITEM *pITEM )
{
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( pITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"Created",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
					this->m_ItemSTR.Get()
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
// 제조시 소모된 아이템
bool GS_logSOCKET::When_DestroyedITEM ( classUSER *pSourAVT, tagITEM *pITEM )
{
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( pITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"Destroy",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
					this->m_ItemSTR.Get()
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool GS_logSOCKET::When_DieBY ( CObjCHAR *pKillOBJ, classUSER *pDeadAVT )
{
	m_csSQL.Lock ();
	{
		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtOBJECT, txtOBJIP ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' )",
				this->GetCurDateTimeSTR(),
				pKillOBJ->Get_NAME(),
				pKillOBJ->IsUSER() ? ((classUSER*)pKillOBJ)->Get_IP() : "NPC",

				"DieBY",
				this->GetLocationSTR( pDeadAVT->m_nZoneNO, pDeadAVT->m_PosCUR ),

				pDeadAVT->Get_NAME(),
				pDeadAVT->Get_IP()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
// 레벨업
bool GS_logSOCKET::When_LevelUP ( classUSER *pSourAVT, int iGetEXP  )
{
	// LogString(LOG_NORMAL, ">>> %s LEVEL UP to %d,  CurEXP: %d, GetEXP: %d\n", pSourAVT->Get_NAME(), pSourAVT->Get_LEVEL(), pSourAVT->m_GrowAbility.m_lEXP, iGetEXP);

	m_csSQL.Lock ();
	{
		this->m_ItemSTR.Printf ("LEVEL UP to %d, CurEXP: %d, GetEXP: %d", pSourAVT->Get_LEVEL(), pSourAVT->m_GrowAbility.m_lEXP, iGetEXP);

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				"LevelUP",
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
// 스킬...
bool GS_logSOCKET::When_LearnSKILL ( classUSER *pSourAVT, short nSkillIDX )
{
	m_csSQL.Lock ();
	{
		this->m_ItemSTR.Printf ("Skill learn %s, LEV: %d ", SKILL_NAME( nSkillIDX ), SKILL_LEVEL( nSkillIDX ));

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				"GetSKILL",
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}
bool GS_logSOCKET::When_LevelUpSKILL ( classUSER *pSourAVT,	short nSkillIDX )
{
	m_csSQL.Lock ();
	{
		this->m_ItemSTR.Printf ("Skill LevelUP %s, LEV: %d ", SKILL_NAME( nSkillIDX ), SKILL_LEVEL( nSkillIDX ));

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				"UpSKILL",
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}


//-------------------------------------------------------------------------------------------------
// 퀘스트...
bool GS_logSOCKET::When_RecvQUEST ( classUSER *pSourAVT, int iQuestIDX )
{
	m_csSQL.Lock ();
	{
		this->m_ItemSTR.Printf( "Quest Recv %d:%s", iQuestIDX, QUEST_NAME( iQuestIDX ) );

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				"QstRECV",
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}
bool GS_logSOCKET::When_DoneQUEST ( classUSER *pSourAVT, t_HASHKEY hQuestHASH )
{
	CQuestTRIGGER *pQuestTRIGGER = g_QuestList.GetQuest( hQuestHASH );

	m_csSQL.Lock ();
	{
		this->m_ItemSTR.Printf( "Quest Done %d:%s", hQuestHASH, pQuestTRIGGER->m_Name.Get() );

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				"QstDONE",
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

bool GS_logSOCKET::When_GiveUpQUEST	( classUSER *pSourAVT, int iQuestIDX )
{
	m_csSQL.Lock ();
	{
		this->m_ItemSTR.Printf( "Quest GiveUP %d:%s", iQuestIDX, QUEST_NAME( iQuestIDX ) );

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				"QstGiveUP",
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

bool GS_logSOCKET::When_GemmingITEM	( classUSER *pSourAVT,	tagITEM *pEquipITEM, tagITEM *pJewelITEM )
{
	m_csSQL.Lock ();
	{
		m_ItemSTR.Printf("[%d:%d]%s, G:%d,D:%d,L:%d, O:%d/%s S:%d:, A:%d", 
				pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO(), 
				ITEM_NAME( pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO() ), 
				pEquipITEM->GetGrade(), pEquipITEM->GetDurability(), pEquipITEM->GetLife(),
				pEquipITEM->GetGemNO(), GEMITEM_NAME( pEquipITEM->GetGemNO() ),
				pEquipITEM->HasSocket(), pEquipITEM->IsAppraisal() );

		m_DescSTR.Printf("[%d:%d]%s", pJewelITEM->GetTYPE(), pJewelITEM->GetItemNO(), ITEM_NAME( pJewelITEM->GetTYPE(), pJewelITEM->GetItemNO() ) );

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtDESC ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				"Gemming",
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get(),
				this->m_DescSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

bool GS_logSOCKET::When_UngemmingITEM	( classUSER *pSourAVT,	tagITEM *pEquipITEM, tagITEM *pJewelITEM )
{
	m_csSQL.Lock ();
	{
		m_ItemSTR.Printf("[%d:%d]%s, G:%d,D:%d,L:%d, O:%d/%s S:%d:, A:%d", 
				pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO(), 
				ITEM_NAME( pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO() ), 
				pEquipITEM->GetGrade(), pEquipITEM->GetDurability(), pEquipITEM->GetLife(),
				pEquipITEM->GetGemNO(), GEMITEM_NAME( pEquipITEM->GetGemNO() ),
				pEquipITEM->HasSocket(), pEquipITEM->IsAppraisal() );

		if ( NULL == pJewelITEM ) {
			// 분리 실패 ..보석 삭제됨.
			m_DescSTR.Printf("Failed, Jewel item cleared");
		} else {
			m_DescSTR.Printf("[%d:%d]%s", pJewelITEM->GetTYPE(), pJewelITEM->GetItemNO(), ITEM_NAME( pJewelITEM->GetTYPE(), pJewelITEM->GetItemNO() ) );
		}

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM, txtDESC ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				"Ungemming",
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get(),
				this->m_DescSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

bool GS_logSOCKET::When_BreakupITEM	( classUSER *pSourAVT, tagITEM *pITEM )
{
	m_csSQL.Lock ();
	{
		if ( this->GetItemINFO( pITEM ) ) {
			this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
					this->GetCurDateTimeSTR(),
					pSourAVT->Get_NAME(),
					pSourAVT->Get_IP(),

					"BreakUp",
					this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
					this->m_ItemSTR.Get()
					);

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}

bool GS_logSOCKET::When_UpgradeITEM ( classUSER *pSourAVT, tagITEM *pEquipITEM, BYTE btBeforeGrade, bool bSuccess )
{
	m_csSQL.Lock ();
	{
		m_ItemSTR.Printf("[%d:%d]%s, D:%d,L:%d, O:%d/%s S:%d:, A:%d, G:%d=>%d", 
				pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO(), 
				ITEM_NAME( pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO() ), 
				pEquipITEM->GetDurability(), pEquipITEM->GetLife(),
				pEquipITEM->GetGemNO(), GEMITEM_NAME( pEquipITEM->GetGemNO() ),
				pEquipITEM->HasSocket(), pEquipITEM->IsAppraisal(),
				btBeforeGrade, pEquipITEM->GetGrade() );

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				bSuccess ? "Upgrade_SUC" : "Upgrade_FAILED",

				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

bool GS_logSOCKET::When_CheatCODE ( classUSER *pSourAVT, char *szCode )
{
	m_csSQL.Lock ();
	{
		this->m_ItemSTR.Printf( "Account:%s[ %s ]", pSourAVT->Get_ACCOUNT(), szCode );

		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtLOC, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				"CheatCODE",
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
GS_logSOCKET::GS_logSOCKET (bool bUseMySQL) : m_PosSTR(128), m_ItemSTR(256), m_QuerySTR( 1024 ), m_DescSTR( 256 )
#ifdef	__CDB_SOCKET
												, CDB_Socket( bUseMySQL ) 
#endif
{
}

//-------------------------------------------------------------------------------------------------
#ifndef	__CDB_SOCKET
void GS_logSOCKET::Send_gsv_LOG_SQL()
{
	g_pThreadSQL->Add_QueryString( this->m_QuerySTR.Get() );
}
#endif
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
