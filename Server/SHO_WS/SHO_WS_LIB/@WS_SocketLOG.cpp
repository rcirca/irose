
#include "stdAFX.h"
#include "WS_SocketLOG.h"
#include "CWS_Client.h"

#ifndef	__CDB_SOCKET
	#include "WS_ThreadSQL.h"
#endif

//-------------------------------------------------------------------------------------------------
#define	KEY_WORLD_VAR		"%s_@VAR"
#define	KEY_WORLD_NPC		"%s_@NPC"

bool WS_logSOCKET::Insert_WorldVAR ()
{
	/*
	m_csSQL.Lock ();
	{
		// 서버 이름@VAR
		this->m_TmpSTR.Printf (KEY_WORLD_VAR, SHO_WS::GetInstance()->GetServerName() );

		this->m_pSQL->BindPARAM( 1, (BYTE*)&this->m_sBE,	sizeof( tagBasicETC )		);

		m_dwBuffLEN = this->m_pSQL->MakeQuery("INSERT tblWS_VAR ( txtNAME, binDATA ) VALUES (",
											MQ_PARAM_STR,		this->m_TmpSTR.Get(),
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_BINDIDX,	1,
				MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END );

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();
	*/
	return true;
}

bool WS_logSOCKET::Update_WorldVAR ()
{
	m_csSQL.Lock ();
	{
	/*
		// 서버 이름@VAR
		this->m_TmpSTR.Printf ("%s_@VAR", SHO_WS::GetInstance()->GetServerName() );
		m_dwBuffLEN = this->m_pSQL->MakeQuery("INSERT tblWS_VAR ( txtNAME, binDATA ) VALUES (",
											MQ_PARAM_STR,	this->GetCurDateTimeSTR(),
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	pSourAVT->Get_NAME(),
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	pSourAVT->Get_IP(),

				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	"GetSKILL",
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				MQ_PARAM_ADDSTR, ","	,	MQ_PARAM_STR,	this->m_ItemSTR.Get(),

				MQ_PARAM_ADDSTR, ");"	,	MQ_PARAM_END );

		this->Send_gsv_LOG_SQL();
	*/
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool WS_logSOCKET::Insert_NpcVAR ()
{
	return true;
}
bool WS_logSOCKET::Update_NpcVAR ()
{
	return true;
}


//-------------------------------------------------------------------------------------------------
bool WS_logSOCKET::When_CreateCHAR ( CWS_Client *pSourAVT, char *szCharNAME )
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

		this->Send_wsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}


//-------------------------------------------------------------------------------------------------
bool WS_logSOCKET::When_DeleteCHAR ( CWS_Client *pSourAVT, char *szCharNAME )
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

		this->Send_wsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}


//-------------------------------------------------------------------------------------------------
WS_logSOCKET::WS_logSOCKET (bool bUseMySQL) : m_QuerySTR(1024) // , m_TmpSTR( 256 )
#ifdef	__CDB_SOCKET
											  ,CDB_Socket( bUseMySQL )
#endif
{
	;
}
WS_logSOCKET::~WS_logSOCKET ()
{
	;
}

#ifndef	__CDB_SOCKET
void WS_logSOCKET::Send_wsv_LOG_SQL()
{
	g_pThreadSQL->Add_QueryString( this->m_QuerySTR.Get() );
}
#endif

//-------------------------------------------------------------------------------------------------
