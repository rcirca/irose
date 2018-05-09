
#include "stdAFX.h"
#include "CWS_Server.h"
#include "CWS_Client.h"
#include "SHO_WS_LIB.h"
#include "WS_SocketLSV.h"
#include "WS_ThreadSQL.h"
#include "WS_ZoneLIST.h"
#include "CThreadLOG.h"
#include "IO_STB.h"
#include "IO_PAT.h"
#include "CWS_Party.h"

#include "CChatROOM.h"
#include "CThreadMSGR.h"
#include "CThreadGUILD.h"
#include "IO_Skill.h"

//-------------------------------------------------------------------------------------------------

STBDATA			 g_TblHAIR;
STBDATA			 g_TblFACE;
STBDATA			 g_TblARMOR;
STBDATA			 g_TblGAUNTLET;
STBDATA			 g_TblBOOTS;
STBDATA			 g_TblHELMET;
STBDATA			 g_TblWEAPON;
STBDATA			 g_TblSUBWPN;

STBDATA			 g_TblFACEITEM;
STBDATA			 g_TblUSEITEM;
STBDATA			 g_TblBACKITEM;
STBDATA			 g_TblGEMITEM;
STBDATA			 g_TblJEWELITEM;
STBDATA			 g_TblNATUAL;
STBDATA			 g_TblQUESTITEM;

/*
STBDATA			 g_TblEFFECT;
STBDATA			 g_TblNPC;
STBDATA			 g_TblAniTYPE;
STBDATA			 g_TblPRODUCT;
STBDATA			 g_TblDropITEM;
STBDATA          g_TblStore;
*/

STBDATA			 g_TblAVATAR;		// 아바타 초기 설정 데이타..
STBDATA	         g_TblWARP;
STBDATA			 g_TblZONE;
STBDATA			*g_pTblSTBs[ ITEM_TYPE_RIDE_PART+1 ];

//#define	BASE_DATA_DIR	"..\\..\\sho\\"
#define	BASE_DATA_DIR	m_BaseDataDIR.Get()	//	"..\\..\\sho\\"

WS_ZoneLIST		g_ZoneLIST;

//-------------------------------------------------------------------------------------------------

SHO_WS *SHO_WS::m_pInstance=NULL;

CWS_ThreadSQL	*g_pThreadSQL = NULL;
CThreadLOG		*g_pThreadLOG = NULL;

CWS_ListSERVER	*g_pListSERVER = NULL;
CWS_ListCLIENT  *g_pUserLIST = NULL;

WS_lsvSOCKET	*g_pSockLSV = NULL;
//WS_logSOCKET	*g_pSockLOG = NULL;

CPacketCODEC	*g_pPacketCODEC;

CChatRoomLIST	*g_pChatROOMs=NULL;
//CWSPartyBUFF	*g_pPartyBUFF=NULL;

CThreadMSGR		*g_pThreadMSGR  = NULL;
CThreadGUILD	*g_pThreadGUILD = NULL;

//-------------------------------------------------------------------------------------------------

#define	DEF_CLIENT_POOL_SIZE	0x7fff		// 32767	동시 접속 대기 소켓
#define	INC_CLIENT_POOL_SIZE	0xfff		// 4095

#define	DEF_SERVER_POOL_SIZE	64
#define	INC_SERVER_POOL_SIZE	16


#define	DEF_RECV_IO_POOL_SIZE		( DEF_CLIENT_POOL_SIZE + INC_CLIENT_POOL_SIZE )
#define	INC_RECV_IO_POOL_SIZE		( INC_CLIENT_POOL_SIZE )

#define	DEF_SEND_IO_POOL_SIZE		( DEF_CLIENT_POOL_SIZE + INC_CLIENT_POOL_SIZE )
#define	INC_SEND_IO_POOL_SIZE		( INC_CLIENT_POOL_SIZE )

#define	DEF_PACKET_POOL_SIZE		( DEF_RECV_IO_POOL_SIZE + DEF_SEND_IO_POOL_SIZE )
#define	INC_PACKET_POOL_SIZE		( INC_CLIENT_POOL_SIZE )


//-------------------------------------------------------------------------------------------------
#define	WM_LSVSOCK_MSG	( WM_SOCKETWND_MSG+0 )
#define	WM_LOGSOCK_MSG	( WM_SOCKETWND_MSG+1 )


#define	MAX_CHAT_ROOMS	150

/*
bool LSV_SocketMSG(WPARAM wParam, LPARAM lParam)
{
	return g_pSockLSV->Proc_SocketMSG( wParam, lParam );
}
*/

void WriteLOG (char *szMSG)
{
	SHO_WS::ExeAPI()->WriteLOG( szMSG );
}
void SetTotalUserCNT( int iUserCNT )
{
	SHO_WS::ExeAPI()->SetUserCNT( iUserCNT );
}


bool SHO_WS::ConnectToLOG ()
{
//	return g_pSockLOG->Connect();
	return true;
}
void SHO_WS::DisconnectFromLOG ()
{
//	g_pSockLOG->Disconnect ();
}
bool SHO_WS::ConnectToLSV ()
{
	return g_pSockLSV->Connect();
}
void SHO_WS::DisconnectFromLSV ()
{
	g_pSockLSV->Disconnect ();
}

#define	USE_MY_SQL_AGENT	0

#include "../SHO_WS.ver"
DWORD GetServerBuildNO ()
{
	return BUILD_NUM;
}
DWORD g_dwStartTIME=0;
DWORD GetServerStartTIME ()
{
	return g_dwStartTIME;
}

//#define	USE_BUG_GUARD
#ifdef	USE_BUG_GUARD
	void SetExceptionReport (DWORD dwVer, char *szAppName, char *szErrFile );
	#pragma comment ( lib, "BugGuard.lib" )
#endif

//-------------------------------------------------------------------------------------------------



#define	LANG_KOR			0
#define	LANG_ENG			1
#define	LANG_JPN			2
#define	LANG_TWAIN			3
#define	LANG_CHINA			4
#define LANG_PHILIPPINE		5
#define LANG_USA			6
#define LANG_EUROPE			7

bool IsEUROPE ()
{
	return ( LANG_EUROPE == ::Get_ServerLangTYPE() );
}

bool IsUSA ()
{
	return ( LANG_USA == ::Get_ServerLangTYPE() );
}

bool IsJAPAN ()
{
	return ( LANG_JPN == ::Get_ServerLangTYPE() );
}
bool IsOnlyTAIWAN()
{
	return ( LANG_TWAIN == ::Get_ServerLangTYPE() );
}
bool IsTAIWAN ()
{
	/*
	switch( ::Get_ServerLangTYPE() ) {
		case LANG_ENG :
		case LANG_TWAIN :
		case LANG_PHILIPPINE :
			return true;
	} 
	//*/
	return true;
}


SHO_WS::SHO_WS( EXE_WS_API *pExeAPI )
{
#ifdef	USE_BUG_GUARD
	SetExceptionReport( GetServerBuildNO() , "SHO_WS_DLL", "SHO_WS_DLL.err" );
#endif

//	COMPILE_TIME_ASSERT( sizeof(cli_MESSENGER) == ( sizeof(t_PACKETHEADER)+1 ) );
//	int iSize = sizeof(cli_MESSENGER);

#if ( _WIN32_WINNT < 0x0500 ) || !defined( __SERVER )
	COMPILE_TIME_ASSERT( 0 );
#endif
	this->m_pExeAPI = pExeAPI;

	CSOCKET::Init ();
	CStr::Init ();
}
void SHO_WS::SystemINIT( HINSTANCE hInstance, char *szBaseDataDIR, int iLangType  )
{
	if ( iLangType < 0 )
		iLangType = 0;
	m_iLangTYPE = iLangType;

	m_BaseDataDIR.Alloc( strlen(szBaseDataDIR) + 3 );
	m_BaseDataDIR.Printf("%s\\", szBaseDataDIR );

	g_pSockLSV = new WS_lsvSOCKET;
//	g_pSockLOG = new WS_logSOCKET( USE_MY_SQL_AGENT );

	g_pPacketCODEC = new CPacketCODEC;

	// 로그인 서버에 접속할 클라이언트 소켓 생성...
	CSocketWND *pSockWND = CSocketWND::InitInstance( hInstance, 2 );
	if ( pSockWND ) {
		// pSockWND->AddSocketMsgProc( WM_LSVSOCK_MSG, LSV_SocketMSG );
		pSockWND->AddSocket( &g_pSockLSV->m_SockLSV, WM_LSVSOCK_MSG );
	#ifdef	__CDB_SOCKET
		pSockWND->AddSocket( g_pSockLOG, WM_LOGSOCK_MSG );
	#endif
	}

//	g_pPartyBUFF  = new CWSPartyBUFF;

	g_pUserLIST = new CWS_ListCLIENT( DEF_CLIENT_POOL_SIZE, INC_CLIENT_POOL_SIZE );
	g_pListSERVER = new CWS_ListSERVER( DEF_SERVER_POOL_SIZE, INC_SERVER_POOL_SIZE );

	CPoolPACKET::Instance (DEF_PACKET_POOL_SIZE,	INC_PACKET_POOL_SIZE );
	CPoolRECVIO::Instance (DEF_RECV_IO_POOL_SIZE,	INC_RECV_IO_POOL_SIZE);
	CPoolSENDIO::Instance (DEF_SEND_IO_POOL_SIZE,	INC_SEND_IO_POOL_SIZE);

	Load_BasicDATA ();

	m_pWorldTIMER = NULL;
}

SHO_WS::~SHO_WS ()
{
	if ( g_pThreadSQL ) {
		g_pThreadSQL->Free ();
		SAFE_DELETE( g_pThreadSQL );
	}
	if ( g_pThreadLOG ) {
		g_pThreadLOG->Free ();
		SAFE_DELETE( g_pThreadLOG );
	}

	if ( g_pThreadMSGR ) {
		g_pThreadMSGR->Free ();
		SAFE_DELETE( g_pThreadMSGR );
	}
	if ( g_pThreadGUILD ) {
		g_pThreadGUILD->Free ();
		SAFE_DELETE( g_pThreadGUILD );
	}

	SAFE_DELETE( g_pUserLIST );
	SAFE_DELETE( g_pListSERVER );

//	SAFE_DELETE( g_pPartyBUFF );


	CPoolRECVIO::Destroy ();
	CPoolSENDIO::Destroy ();
	CPoolPACKET::Destroy ();

	SAFE_DELETE( g_pPacketCODEC );

//	SAFE_DELETE( g_pSockLOG );
	SAFE_DELETE( g_pSockLSV );

	if ( CSocketWND::GetInstance() )
		CSocketWND::GetInstance()->Destroy();

    CStr::Free ();
	CSOCKET::Free ();
}


//-------------------------------------------------------------------------------------------------
// 서버 초기화
// 1. 디비 서버 접속
// 2. 존 서버 접속 허용
// 3. 사용자 접속 허용..
// 4. 로그인 서버 접속
bool SHO_WS::ConnectDB( char *szDBServerIP, 
						char *szDBName, 
						char *szDBUser, char *szDBPassword,
						char *szLogDBUser, char *szLogDBPw )
{
	if ( NULL == this->GetInstance() )
		return false;

	m_DBServerIP.Set( szDBServerIP );
	m_DBName.Set( szDBName );
	m_DBUser.Set( szDBUser );
	m_DBPassword.Set( szDBPassword );

	if ( '?' == szLogDBUser[0] ) {
		m_LogDBUser.Set( szDBUser );
		m_LogDBPassword.Set( szDBPassword );
	} else {
		m_LogDBUser.Set( szLogDBUser );
		m_LogDBPassword.Set( szLogDBPw );
	}

	g_pThreadSQL = new CWS_ThreadSQL;	// suspend 모드로 시작됨.
	if ( !g_pThreadSQL->Connect( USE_MY_SQL_AGENT ? USE_MY_SQL:USE_ODBC, (char*)szDBServerIP, szDBUser, szDBPassword, szDBName, 32, 1024*8) ) {
		return false;
	}
	g_pThreadSQL->Resume ();


	CStrVAR stLogODBC;
	stLogODBC.Set( "SHO_LOG" );
	g_pThreadLOG = new CThreadLOG;
	if ( !g_pThreadLOG->Connect( USE_ODBC, szDBServerIP, m_LogDBUser.Get(), m_LogDBPassword.Get(), stLogODBC.Get(), 32, 1024*8) ) {
		return false;
	}
	g_pThreadLOG->Resume ();


	g_pThreadMSGR = new CThreadMSGR( 16384, 1024 );
	if ( !g_pThreadMSGR->Connect( USE_MY_SQL_AGENT ? USE_MY_SQL:USE_ODBC, (char*)szDBServerIP, szDBUser, szDBPassword, szDBName, 32, 1024*8) ) {
		return false;
	}
	g_pThreadMSGR->Resume ();


	g_pThreadGUILD = new CThreadGUILD( 8192, 512 );
	if ( !g_pThreadGUILD->Connect( USE_MY_SQL_AGENT ? USE_MY_SQL:USE_ODBC, (char*)szDBServerIP, szDBUser, szDBPassword, szDBName, 32, 1024*8) ) {
		return false;
	}
	g_pThreadGUILD->Resume ();


	g_pThreadSQL->Load_WORLDVAR( g_ZoneLIST.m_pVAR, sizeof( tagWorldVAR ) );

	return true;
}


//-------------------------------------------------------------------------------------------------
bool SHO_WS::Start (HWND hMainWND,
					char *szLoginServerIP,	int iLoginServerPort,
					char *szLogServerIP,	int iLogServerPortNO,
					char *szWorldName,		int iServerListenPortNO, int iClientListenPortNO, bool bBlockCreateCHAR )
{
	g_dwStartTIME = classTIME::GetCurrentAbsSecond();
	m_hMainWND = hMainWND;

	m_bBlockCreateCHAR = bBlockCreateCHAR;
	m_iClientListenPortNO = iClientListenPortNO;
	m_iServerListenPortNO = iServerListenPortNO;

	m_ServerNAME.Set( szWorldName );
	m_ServerIP.Set( szLoginServerIP );

	g_pChatROOMs = new CChatRoomLIST( MAX_CHAT_ROOMS );

	g_pUserLIST->Active( m_iClientListenPortNO, MAX_WORLD_USER_UBFF, 5*60 );	// 5분 대기
	g_pListSERVER->Active( m_iServerListenPortNO, 512, 3*60 );					// 3분 대기

	m_dwRandomSEED = ::timeGetTime();
	g_pPacketCODEC->Init( m_dwRandomSEED );


#ifdef	__CDB_SOCKET
	g_pSockLOG->Init ( CSocketWND::GetInstance()->GetWindowHandle(), 
						szLogServerIP, 
						iLogServerPortNO, 
						WM_LOGSOCK_MSG, 
						m_DBName.Get(), m_DBUser.Get(), m_DBPassword.Get(),
						WS_TIMER_LOG, RECONNECT_TIME_TICK, (TIMERPROC)WS_TimerProc );
#endif
	this->ConnectToLOG ();

	g_pSockLSV->Init ( CSocketWND::GetInstance()->GetWindowHandle(), szLoginServerIP, iLoginServerPort, WM_LSVSOCK_MSG, true );
	this->ConnectToLSV ();

	m_pWorldTIMER = new CTimer( m_hMainWND, WS_TIMER_WORLD_TIME, WORLD_TIME_TICK, (TIMERPROC)WS_TimerProc );
	m_pWorldTIMER->Start ();

	return true;
}

//-------------------------------------------------------------------------------------------------
void SHO_WS::Active (bool bActive)
{
	g_pSockLSV->Send_srv_ACTIVE_MODE( bActive );
}

//-------------------------------------------------------------------------------------------------
void SHO_WS::Shutdown ()
{
	SAFE_DELETE( m_pWorldTIMER );

	DisconnectFromLSV ();

	g_pUserLIST->Shutdown ();
	g_pListSERVER->Shutdown ();

	SAFE_DELETE( g_pChatROOMs );	// 제일 마지막에...
}

//-------------------------------------------------------------------------------------------------
void SHO_WS::StartCLI_SOCKET ()
{
	g_pUserLIST->Active( m_iClientListenPortNO, MAX_WORLD_USER_UBFF, 5*60 );	// 5분 대기
}

void SHO_WS::ShutdownCLI_SOCKET ()
{
	g_pUserLIST->Shutdown ();
}


//-------------------------------------------------------------------------------------------------
void SHO_WS::TranslateNameWithDescKey( STBDATA *pOri, char *szStbFile, int iLangCol )
{
	int iKeyCol = 0;

	STBDATA	tmpTBL;
	// 아이템 이름만 읽음 :: 이름 컬럼은 1, 3, 5 ... 설명 컬럼은 2, 4, 6 ...
	if ( !tmpTBL.LoadWSTB( true, CStr::Printf("%s%s", BASE_DATA_DIR, szStbFile), iKeyCol, iLangCol, -1 ) )
		return;
	STBDATA *pLang = &tmpTBL;

	char *szKey;
	int iIdx;
	for (int iL=0; iL<pOri->m_nDataCnt; iL++ ) {
		szKey = pOri->m_ppDESC[ iL ];
		if ( !szKey )
			continue;

		iIdx = pLang->GetRowIndex( szKey );
		if ( iIdx && pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN() ) {
			// LogString(0xffff," %s :: %d : %s/%s\n", szKey, iIdx, pOri->m_ppNAME[ iL ], pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );
			SAFE_DELETE( pOri->m_ppNAME[ iL ] );
			pOri->m_ppNAME[ iL ] = new char[ pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN()+1 ];
			strcpy( pOri->m_ppNAME[ iL ], pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );
			pOri->m_ppNAME[ pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN() ] = 0;
		}
	}

	tmpTBL.Free();
}

void SHO_WS::TranslateNameWithColoumKey ( STBDATA *pOri, char *szStbFile, int iLangCol, int iNameCol, int iDescCol )
{
	int iKeyCol = 0;

	STBDATA	tmpTBL;

	// 아이템 이름만 읽음 :: 이름 컬럼은 1, 3, 5 ... 설명 컬럼은 2, 4, 6 ...
	if ( !tmpTBL.LoadWSTB( true, CStr::Printf("%s%s", BASE_DATA_DIR, szStbFile), iKeyCol, iLangCol, -1 ) )
		return;
	STBDATA *pLang = &tmpTBL;

	char *szKey;
	int iIdx;
	for (int iL=1; iL<pOri->m_nDataCnt; iL++ ) {
		szKey = pOri->m_ppVALUE[ iL ][ iDescCol ].GetSTR();
		if ( !szKey )
			continue;

		iIdx = pLang->GetRowIndex( szKey );
		if ( iIdx && pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN() ) {
			// LogString(0xffff," %s :: %d : %s/%s\n", szKey, iIdx, pOri->m_ppVALUE[ iL ][ iNameCol ].GetSTR(), pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );
			pOri->m_ppVALUE[ iL ][ iNameCol ].SetVALUE( pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );
		}
	}

	tmpTBL.Free();
}

//-------------------------------------------------------------------------------------------------
bool SHO_WS::Load_BasicDATA ()
{
	g_TblHAIR.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Hair.STB"),		true, false );
	g_TblFACE.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Face.STB"),		true, false );
	g_TblARMOR.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Body.STB"),		true, true  );

	g_TblGAUNTLET.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Arms.STB"),		true, true  );
	g_TblBOOTS.Load		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Foot.STB"),		true, true  );
	g_TblHELMET.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Cap.STB"),			true, true  );

	g_TblWEAPON.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Weapon.STB"),		true, true  );
	g_TblSUBWPN.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SUBWPN.STB"),		true, true  );

//	g_TblPRODUCT.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_PRODUCT.STB"),	    true, false );
	g_TblNATUAL.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_NATURAL.STB"),		true, true	);

	g_TblFACEITEM.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_FACEITEM.STB"),	true, true	);
	g_TblUSEITEM.Load   ( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_USEITEM.STB"),		true, true	);
	g_TblBACKITEM.Load  ( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_BACK.STB"),		true, true	);
	g_TblGEMITEM.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_JEMITEM.STB"),		true, true	);
	g_TblJEWELITEM.Load ( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_JEWEL.STB"),	    true, true	);
	g_TblQUESTITEM.Load ( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_QUESTITEM.STB"),	true, true  );

	if ( !g_SkillList.LoadSkillTable( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SKILL.STB") ) )
		return false;

	if ( !g_PatITEM.LoadPatITEM( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_PAT.STB") ) )
		return false;

	g_pTblSTBs[ ITEM_TYPE_FACE_ITEM ] = &g_TblFACEITEM;
	g_pTblSTBs[ ITEM_TYPE_HELMET	] = &g_TblHELMET;
	g_pTblSTBs[ ITEM_TYPE_ARMOR		] =	&g_TblARMOR;
	g_pTblSTBs[ ITEM_TYPE_GAUNTLET	] = &g_TblGAUNTLET;
	g_pTblSTBs[ ITEM_TYPE_BOOTS		] = &g_TblBOOTS;
	g_pTblSTBs[ ITEM_TYPE_KNAPSACK	] = &g_TblBACKITEM;
	g_pTblSTBs[ ITEM_TYPE_JEWEL		] = &g_TblJEWELITEM;
	g_pTblSTBs[ ITEM_TYPE_WEAPON	] = &g_TblWEAPON;
	g_pTblSTBs[ ITEM_TYPE_SUBWPN	] = &g_TblSUBWPN;
	g_pTblSTBs[ ITEM_TYPE_USE		] = &g_TblUSEITEM;
	g_pTblSTBs[ ITEM_TYPE_GEM		] = &g_TblGEMITEM;
	g_pTblSTBs[ ITEM_TYPE_NATURAL	] = &g_TblNATUAL;
	g_pTblSTBs[ ITEM_TYPE_QUEST		] = &g_TblQUESTITEM;
	g_pTblSTBs[ ITEM_TYPE_RIDE_PART ] = &g_PatITEM.m_ItemDATA;
	
	// 아이템 이름만 읽음 :: 이름 컬럼은 1, 3, 5 ... 설명 컬럼은 2, 4, 6 ...
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_FACE_ITEM  ], "Language\\LIST_FACEITEM_s.STB",	1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_HELMET	 ], "Language\\LIST_CAP_s.STB",			1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_ARMOR		 ], "Language\\LIST_BODY_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_GAUNTLET	 ], "Language\\LIST_ARMS_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_BOOTS		 ], "Language\\LIST_FOOT_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_KNAPSACK	 ], "Language\\LIST_BACK_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_JEWEL		 ], "Language\\LIST_JEWEL_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_WEAPON	 ], "Language\\LIST_WEAPON_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_SUBWPN	 ], "Language\\LIST_SUBWPN_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_USE		 ], "Language\\LIST_USEITEM_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_GEM		 ], "Language\\LIST_JEMITEM_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_NATURAL	 ], "Language\\LIST_NATURAL_s.STB",		1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_QUEST		 ], "Language\\LIST_QUESTITEM_s.STB",	1+2*m_iLangTYPE );
	this->TranslateNameWithDescKey( g_pTblSTBs[ ITEM_TYPE_RIDE_PART  ], "Language\\LIST_PAT_s.STB",			1+2*m_iLangTYPE );

	g_TblAVATAR.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\INIT_AVATAR.STB"),		false,	false);
	g_TblWARP.Load2		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\WARP.STB"),				true,	false);

	g_TblZONE.Load2		( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_ZONE.STB"),		true,	false);
	g_ZoneLIST.Init (BASE_DATA_DIR);

	return true;
}
void SHO_WS::Free_BasicDATA ()
{
	// STBDATA는 자동 풀림..
	g_SkillList.Free ();
	g_PatITEM.Free ();
}
void SHO_WS::Send_ANNOUNCE(char *szMsg )
{
	g_pListSERVER->Send_gsv_ANNOUNCE_CHAT( szMsg, NULL );
}

//-------------------------------------------------------------------------------------------------
void SHO_WS::ToggleChannelActive (int iChannelNo)
{
	g_pListSERVER->Set_ChannelACTIVE( iChannelNo, false, true );
}

void SHO_WS::DeleteChannelServer (int iChannelNo)
{
	g_pListSERVER->Del_ChannelSERVER( iChannelNo );
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
VOID CALLBACK WS_TimerProc (HWND hwnd/* handle to window */, UINT uMsg/* WM_TIMER message */, UINT_PTR idEvent/* timer identifier */, DWORD dwTime/* current system time */ ) 
{
	switch( idEvent ) {
		case WS_TIMER_LSV :
		{
			SHO_WS *pGS = SHO_WS::GetInstance();
			if ( pGS ) {
				pGS->ConnectToLSV ();
			}
			break;
		}

		case WS_TIMER_LOG :
		{
			SHO_WS *pGS = SHO_WS::GetInstance();
			if ( pGS ) {
				pGS->ConnectToLOG ();
			}
			break;
		}

		case WS_TIMER_WORLD_TIME :
		{
			SHO_WS *pWS = SHO_WS::GetInstance();
			if ( NULL == pWS )
				return;
			g_ZoneLIST.Inc_WorldTIME (true);

			switch( g_ZoneLIST.m_dwAccTIME % 6 ) {
				case 0 : case 3 :		// 30초에 한번씩 체크...	
					g_pUserLIST->Check_SocketALIVE ();
					break;
				case 1 : // case 4 :
					g_pUserLIST->CloseIdleSCOKET( 150*1000 );		// 2분 30초
					break;
			}
			break;
		}
	} ;
}

//-------------------------------------------------------------------------------------------------
