
#include "stdAFX.h"

#include "SHO_LS_LIB.h"

#include "CLS_Account.h"
#include "CLS_Client.h"
#include "CLS_Server.h"
#include "CLS_SqlTHREAD.h"
#include "CurlTHREAD.h"
#include "blockLIST.h"
#include "CAS_Gums.h"
#ifdef	__TAIWAN
#include "BS_Taiwan.h"
#endif


#define PACKET_SEED 0x6648495
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------

//CBS_Taiwan	 *g_pTwainBS=NULL;
AS_gumSOCKET *g_pSockGUM=NULL;

SHO_LS *SHO_LS::m_pInstance=NULL;

CLS_AccountLIST *g_pListJOIN = NULL;    // HASHKEY = CStr::GetHASH( szAccount )
CLS_AccountLIST *g_pListWAIT = NULL;    // HASHKEY = dwLoginServerSocketID
CLS_AccountLIST *g_pListBKDR = NULL;

CLS_SqlTHREAD   *g_pThreadSQL = NULL;
CurlTHREAD      *g_pThreadSTATUS=NULL;

CLS_ListCLIENT  *g_pListCLIENT;
CLS_ListSERVER  *g_pListSERVER;

CPacketCODEC	*g_pPacketCODEC;


classListBLOCK<tagBlockDATA>    *g_pListBlackIP=NULL;
classListBLOCK<tagBlockDATA>    *g_pListBlackACCOUNT=NULL;

classListBLOCK<tagBlockDATA>    *g_pListServerIP=NULL;

//-------------------------------------------------------------------------------------------------

#define	DEF_CLIENT_POOL_SIZE	8192		// 동시 접속 대기 소켓
#define	INC_CLIENT_POOL_SIZE	1024

#define	DEF_SERVER_POOL_SIZE	256
#define	INC_SERVER_POOL_SIZE	128


#define	DEF_RECV_IO_POOL_SIZE		( DEF_CLIENT_POOL_SIZE + INC_CLIENT_POOL_SIZE )
#define	INC_RECV_IO_POOL_SIZE		( INC_CLIENT_POOL_SIZE )

#define	DEF_SEND_IO_POOL_SIZE		( DEF_CLIENT_POOL_SIZE + INC_CLIENT_POOL_SIZE )
#define	INC_SEND_IO_POOL_SIZE		( INC_CLIENT_POOL_SIZE )

#define	DEF_PACKET_POOL_SIZE		( DEF_RECV_IO_POOL_SIZE + DEF_SEND_IO_POOL_SIZE )
#define	INC_PACKET_POOL_SIZE		( INC_CLIENT_POOL_SIZE )


#define	JOIN_HASH_TABLE_SIZE		4096
#define WAIT_HASH_TABLE_SIZE		512


//-------------------------------------------------------------------------------------------------
void WriteLOG (char *szMSG)
{
	// 정말로 중요한 메세지만...
	SHO_LS::ExeAPI()->WriteLOG( szMSG );
}

#define	LS_TIMER_CHECK_WAIT_LIST	1
#define	LS_TICK_CHECK_WAIT_LIST		1000

static DWORD s_dwTick=0;
VOID CALLBACK LS_TimerProc (HWND hwnd/* handle to window */, UINT uMsg/* WM_TIMER message */, UINT_PTR idEvent/* timer identifier */, DWORD dwTime/* current system time */ ) 
{
	if ( LS_TIMER_CHECK_WAIT_LIST == idEvent ) {
		if ( ++s_dwTick % 2 )
			g_pListWAIT->Delete_IdleACCOUNT( 90 );	// 서버 이동하는데 30초 이상 걸리면 짤러~
		else
			g_pListCLIENT->Delete_IdleSOCKET();
	}
}

//-------------------------------------------------------------------------------------------------
#include "../SHO_LS.ver"
DWORD GetServerBuildNO ()
{
	return BUILD_NUM;
}
DWORD g_dwStartTIME=0;
DWORD GetServerStartTIME ()
{
	return g_dwStartTIME;
}

SHO_LS::SHO_LS (EXE_LS_API *pExeAPI)
{
#if ( _WIN32_WINNT < 0x0500 ) || !defined( __SERVER )
	COMPILE_TIME_ASSERT( 0 );
#endif
	m_pExeAPI = pExeAPI;

	m_pTIMER = NULL;
}

void LoadBlockIP ()
{
	FILE *fp;
	fp = fopen( "IP_BLOCK.txt", "rt" );
	if ( !fp )
		return;

	int iRet;
	char szFromIP[ 64 ], szToIP[ 64 ];
	do {
		iRet = fscanf( fp, "%s %s", szFromIP, szToIP );
		if ( 2 == iRet ) {
			// printf("%s --> %s \n", szFromIP, szToIP );
			g_pListCLIENT->Add_RefuseIP( szFromIP, szToIP );
		} 

	} while( iRet != EOF );

	fclose( fp );
}

void SHO_LS::SystemINIT( HINSTANCE hInstance )
{
	::Sleep( 500 );

	g_pPacketCODEC = new CPacketCODEC;
	//g_pPacketCODEC->Default ();
	g_pPacketCODEC->Init( PACKET_SEED );

	CPoolPACKET::Instance (DEF_PACKET_POOL_SIZE,	INC_PACKET_POOL_SIZE );
	CPoolRECVIO::Instance (DEF_RECV_IO_POOL_SIZE,	INC_RECV_IO_POOL_SIZE);
	CPoolSENDIO::Instance (DEF_SEND_IO_POOL_SIZE,	INC_SEND_IO_POOL_SIZE);

	g_pListCLIENT = new CLS_ListCLIENT( DEF_CLIENT_POOL_SIZE, INC_CLIENT_POOL_SIZE );
	g_pListSERVER = new CLS_ListSERVER( DEF_SERVER_POOL_SIZE, INC_SERVER_POOL_SIZE );

	LoadBlockIP ();

    g_pListBlackIP = new classListBLOCK<tagBlockDATA>( DEF_BLACK_IP_HASHTABLE_SIZE );
    g_pListBlackACCOUNT = new classListBLOCK<tagBlockDATA> ( DEF_BLACK_NAME_HASHTABLE_SIZE );

    g_pListJOIN = new CLS_AccountLIST( "JOIN", JOIN_HASH_TABLE_SIZE );
    g_pListWAIT = new CLS_AccountLIST( "WAIT", WAIT_HASH_TABLE_SIZE );
	g_pListBKDR = new CLS_AccountLIST( "", WAIT_HASH_TABLE_SIZE );

	m_iClientListenPortNO = 0;
	m_iServerListenPortNO = 0;

	#define	WM_GUMSOCK_MSG	( WM_SOCKETWND_MSG+0 )
	CSocketWND *pSockWND = CSocketWND::InitInstance( hInstance, 1 );

#ifdef	__TAIWAN
	g_LOG.CS_ODS(0xffff, "TAWAIN version \n");
	g_LOG.CS_ODS(0xffff, "TAWAIN version \n");
	g_LOG.CS_ODS(0xffff, "TAWAIN version \n");
	g_LOG.CS_ODS(0xffff, "TAWAIN version \n");
	g_LOG.CS_ODS(0xffff, "TAWAIN version \n");
	g_LOG.CS_ODS(0xffff, "TAWAIN version \n");
#elif defined( USE_MSSQL )
	g_LOG.CS_ODS(0xffff, "MS-SQL version \n");
	g_LOG.CS_ODS(0xffff, "MS-SQL version \n");
	g_LOG.CS_ODS(0xffff, "MS-SQL version \n");
	g_LOG.CS_ODS(0xffff, "MS-SQL version \n");
	g_LOG.CS_ODS(0xffff, "MS-SQL version \n");
	g_LOG.CS_ODS(0xffff, "MS-SQL version \n");
#elif defined( USE_ORACLE_DB )
	g_LOG.CS_ODS(0xffff, "ORACLE version \n");
	g_LOG.CS_ODS(0xffff, "ORACLE version \n");
	g_LOG.CS_ODS(0xffff, "ORACLE version \n");
	g_LOG.CS_ODS(0xffff, "ORACLE version \n");
	g_LOG.CS_ODS(0xffff, "ORACLE version \n");
	g_LOG.CS_ODS(0xffff, "ORACLE version \n");
#else
	g_LOG.CS_ODS(0xffff, "My SQL version \n");
	g_LOG.CS_ODS(0xffff, "My SQL version \n");
	g_LOG.CS_ODS(0xffff, "My SQL version \n");
	g_LOG.CS_ODS(0xffff, "My SQL version \n");
	g_LOG.CS_ODS(0xffff, "My SQL version \n");
	g_LOG.CS_ODS(0xffff, "My SQL version \n");
#endif
}


SHO_LS::~SHO_LS ()
{
	SAFE_DELETE( m_pTIMER );

    if ( g_pThreadSQL ) {
        g_pThreadSQL->Destroy ();
        g_pThreadSQL = NULL;
    }

	SAFE_DELETE( g_pListJOIN );
	SAFE_DELETE( g_pListWAIT );
	SAFE_DELETE( g_pListBKDR );

    SAFE_DELETE( g_pListBlackACCOUNT );
    SAFE_DELETE( g_pListBlackIP );

	SAFE_DELETE( g_pListCLIENT );
	SAFE_DELETE( g_pListSERVER );

	SAFE_DELETE( g_pSockGUM );
	if ( CSocketWND::GetInstance() )
		CSocketWND::GetInstance()->Destroy();

//	CBS_Taiwan::Destroy();
//	g_pTwainBS = NULL;

	CPoolRECVIO::Destroy ();
	CPoolSENDIO::Destroy ();
	CPoolPACKET::Destroy ();

	SAFE_DELETE( g_pPacketCODEC );
}


//-------------------------------------------------------------------------------------------------
bool SHO_LS::StartClientSOCKET (int iClientListenPort, int iLimitUserCNT, BYTE btMD5[32])
{
	if ( m_iClientListenPortNO )
		return false;

	::CopyMemory( m_btMD5, btMD5, 32 );

	m_iClientListenPortNO = iClientListenPort;
	g_pListCLIENT->Active( m_iClientListenPortNO, 65535, 3*60 );	// 동시 접속 소켓 갯수 65535. 접속유지 체크 3분
	this->SetLimitUserCNT ( iLimitUserCNT );

	return true;
}

void SHO_LS::CloseClientSOCKET ()
{
	if ( m_iClientListenPortNO ) {
		g_pListCLIENT->Shutdown ();
		m_iClientListenPortNO = 0;
	}
}



bool SHO_LS::StartServerSOCKET (HWND hMainWND, char *szDBServerIP, int iServerListenPort, DWORD dwLoginRight, char *szGumsIP, int iGumsPORT, bool bShowOnlyWS)
{
	if ( m_iServerListenPortNO )
		return false;

#ifdef	__TAIWAN
	if ( szGumsIP ) {
		g_pTwainBS = CBS_Taiwan::InitInstance(
							szGumsIP,	// "203.74.37.20", 
							iGumsPORT,	// 7888,
							0/* iServerID */ );
	}
#endif

	m_bShowOnlyWS = bShowOnlyWS;
    if ( NULL == g_pThreadSQL ) {
#ifndef	USE_ORACLE_DB
		char *szDBName  = "SHO";
        char *szDBUser	= "icarus";
        char *szPassword= "1111";
#else
		char *szDBName  = "SEVEN_ORA";
        char *szDBUser	= "seven";
        char *szPassword= "tpqmsgkcm";
#endif

        g_pThreadSQL = CLS_SqlTHREAD::Instance ();

#ifndef	__TAIWAN
	#ifndef	USE_ORACLE_DB
			if ( !g_pThreadSQL->Connect( USE_MY_SQL, (char*)szDBServerIP, szDBUser, szPassword, szDBName, 32, 1024*8) ) {
	#else
			if ( !g_pThreadSQL->Connect( USE_ODBC, szDBServerIP, szDBUser, szPassword, szDBName, 32, 1024*4) ) {
	#endif
				g_LOG.CS_ODS (0xffff, "sql connect failed ...");
				g_pThreadSQL->Destroy ();
				g_pThreadSQL = NULL;
				return false;
			}
#endif

		g_pThreadSQL->Resume ();
		g_pThreadSQL->m_bCheckLogIN  = true;//CheckBoxLogIN->Checked;
		g_pThreadSQL->m_dwCheckRIGHT = dwLoginRight;//StrToInt( EditLogInLevel->Text );
	}

#ifndef	__TAIWAN
	if ( szGumsIP && CSocketWND::GetInstance() ) {
		// 테스트용 n-cash 서버 :: 211.232.109.160
		g_pSockGUM = new AS_gumSOCKET;
		CSocketWND::GetInstance()->AddSocket( &g_pSockGUM->m_SockGUM, WM_GUMSOCK_MSG );

		g_pSockGUM->Init ( CSocketWND::GetInstance()->GetWindowHandle(), 
							szGumsIP,	// "192.168.20.204",	// "211.232.109.160",
							iGumsPORT,	// 20000, 
							WM_GUMSOCK_MSG );
		g_pSockGUM->Connect ();
	}
#endif

	g_dwStartTIME = classTIME::GetCurrentAbsSecond();

	m_iServerListenPortNO = iServerListenPort;
	g_pListSERVER->Active( m_iServerListenPortNO, 1024, 60 );	// 동시 접속 소켓 갯수 1024. 접속유지 체크 1분

	m_pTIMER = new CTimer( hMainWND, LS_TIMER_CHECK_WAIT_LIST, LS_TICK_CHECK_WAIT_LIST, (TIMERPROC)LS_TimerProc );
	m_pTIMER->Start ();

	return true;
}

void SHO_LS::Shutdown ()
{
	CloseClientSOCKET ();
	if ( m_iServerListenPortNO ) {
		SAFE_DELETE( m_pTIMER );

		g_pListSERVER->Shutdown ();
		m_iServerListenPortNO = 0;
	}
}

//-------------------------------------------------------------------------------------------------
void SHO_LS::Send_ANNOUNCE( void *pServer ,char *szAnnounceMsg )
{
	if ( g_pListSERVER )
		g_pListSERVER->Send_lsv_ANNOUNCE_CHAT( pServer, szAnnounceMsg );
}

//-------------------------------------------------------------------------------------------------
void SHO_LS::SetLoginRIGHT( DWORD dwLoginRight )
{
	if ( g_pThreadSQL )
		g_pThreadSQL->m_dwCheckRIGHT = dwLoginRight;
}

void SHO_LS::SetLimitUserCNT( int iLimitUserCNT )
{
	if ( g_pListCLIENT )
		g_pListCLIENT->SetLimitUserCNT( iLimitUserCNT );
}

//-------------------------------------------------------------------------------------------------
