
#include "stdAFX.h"
#include <assert.h>
#include "CAS_GUMS.h"


#define	__GUMS_LOG


#define	AS_TIMER_GUMS_CONN			1
#define AS_TIMER_GUMS_ALIVE			2

#define	RECONNECT_TIME_TICK			20000		// 20 sec
#define	SEND_ALIVE_TIME_TICK		30000		// 30 sec


AS_gumSOCKET* AS_gumSOCKET::m_pInstance = NULL;

/*
	로그인시 ::
		11번은 성공이구여.
		12 ~ 는 실패에 대한 이유입니다ㅣ.
			12는 아이디나 패스워드가 틀렸을 경우
			13은 게임할 권한이 없는 사용자
			14는 결제한 시간을 다 썼거나 정액제의 경우 기간이 지났을 경우
			15 이미 로그인 한 사용자의 경우..
			18 일일 사용시간 (하루에 사용할 수 있는 시간)이 넘었을 때 오는 메시지..
			19 하루에 사용할 수 있는 기간(예 12시 ~ 2시)에서 벗어 났을 때 오는 메시지..

	경고메시지는 50번대..
	끊으라는 신호는 60번대.

	프리 로그인 요청시 GUMS에서 오는 메세지는 111,112,113
	사용자 시간 만료시 GUMS에서 오는 메세지는 52, 62

	남은 시간요구 : 40 요청 -> 응답 41/42
*/

//-------------------------------------------------------------------------------------------------
DWORD AS_gumSOCKET::GetPlayingFLAG( eGumsNATION eNation, int iPayType )
{
	DWORD dwPlayFlag = 0;
	switch( eNation ) {
		case GUMS_KOREA :	// 한국은...
			dwPlayFlag = PLAY_FLAG_KOREA_DEFAULT;
			switch( iPayType ) {
				case BILLING_MSG_PAY_FAP	:
				case BILLING_MSG_PAY_GU		:
					dwPlayFlag |= PLAY_FLAG_EXTRA_STOCK | PLAY_FLAG_EXTRA_CHAR;
					break;
#ifdef	__PHILIPPINE
				default:
					dwPlayFlag |= PLAY_FLAG_EXTRA_CHAR;
#endif
			}
			break;
		case GUMS_JAPAN :	// 일본은...
			// 일본 iPayType은 PLAY_FLAG와 같다 !!!
			dwPlayFlag = iPayType;
			break;
	} 
	return dwPlayFlag;
}


//-------------------------------------------------------------------------------------------------
#ifdef	__SHO_LS_LIB
//-------------------------------------------------------------------------------------------------
	#include "CLS_Client.h"
	bool AS_gumSOCKET::RecvGums_LoginSUC (eGumsNATION eNation, char *szAccount, char *szGumsPKET, char *szUserTAG, int iPayType)	
	{
		int iSocketIDX = atoi( szUserTAG );
	    CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( iSocketIDX );
	    if ( pClient && !strcmpi( szAccount, pClient->m_Account.Get() ) ) {
			BYTE btType;
			switch( eNation ) {
				case GUMS_KOREA :	// 한국은...
					//pClient->m_dwPayFLAG = 0x01 << iPayType;
					btType = RESULT_LOGIN_REPLY_KOREA_OK;
					break;
				case GUMS_JAPAN :	// 일본은...
					btType = RESULT_LOGIN_REPLY_JAPAN_OK;
					break;
					
			} 
			pClient->Send_lsv_LOGIN_REPLY( btType /* RESULT_LOGIN_REPLY_OK */, iPayType );

			pClient->m_dwPayFLAG = this->GetPlayingFLAG( eNation, iPayType );
			pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		}

		return true;
	}
	bool AS_gumSOCKET::RecvGums_LoginOnlyFREE(int iMsgTYPE, char *szAccount, char *szUserTAG)
	{
		// 프리섭 로그인... 프리섭 리스트만 전송..
		int iSocketIDX = atoi( szUserTAG );
	    CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( iSocketIDX );
	    if ( pClient && !strcmpi( szAccount, pClient->m_Account.Get() ) ) {
			BYTE btType;
			eGumsNATION eNation = Gums_NATION( iMsgTYPE );
			switch( eNation ) {
				case GUMS_KOREA :	// 한국은...
					btType = RESULT_LOGIN_REPLY_KOREA_OK;
					break;
				case GUMS_JAPAN :	// 일본은...
					btType = RESULT_LOGIN_REPLY_JAPAN_OK;
					break;
			} 
			pClient->Send_lsv_LOGIN_REPLY( btType/* RESULT_LOGIN_REPLY_OK */, 0 );

			pClient->m_bFreeServerOnly = true;
			pClient->m_dwPayFLAG = PLAY_FLAG_KOREA_DEFAULT;// this->GetPlayingFLAG( eNation, iPayType );
			pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
		}

		return true;
	}
	bool AS_gumSOCKET::RecvGums_LogInFailedNoRIGHT (int iMsgTYPE, char *szAccount, char *szUserTAG)
	{
		int iSocketIDX = atoi( szUserTAG );
	    CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( iSocketIDX );
	    if ( pClient && !strcmpi( szAccount, pClient->m_Account.Get() ) ) {
			pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_NEED_CHARGE, 0 );
		}

		return true;
	}
	bool AS_gumSOCKET::RecvGums_LogInFailedOutOfIP (int iMsgTYPE, char *szAccount, char *szUserTAG)
	{		
		int iSocketIDX = atoi( szUserTAG );
	    CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( iSocketIDX );
	    if ( pClient && !strcmpi( szAccount, pClient->m_Account.Get() ) ) {
			pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OUT_OF_IP, 0 );
		}

		return true;
	}
#else
//-------------------------------------------------------------------------------------------------
	#include "CAS_Account.h"
	bool AS_gumSOCKET::RecvGums_LoginOnlyFREE(int iMsgTYPE, char *szAccount, char *szUserTAG)	
	{		
		return true;	
	}

	// 게임할 권한이 없는 사용자
	bool AS_gumSOCKET::RecvGums_LogInFailedNoRIGHT (int iMsgTYPE, char *szUserID, char *szUserTAG)		
	{		
		return g_pListACCOUNT->Gums_KickOutUSER( szUserID, BILLING_MSG_KICKOUT_NO_RIGHT );
	}
	bool AS_gumSOCKET::RecvGums_LogInFailedOneDayEXPIRED (int iMsgTYPE, char *szUserID)
	{		
		return g_pListACCOUNT->Gums_KickOutUSER( szUserID, BILLING_MSG_KICKOUT_TIME_EXPIRED );
	}
	bool AS_gumSOCKET::RecvGums_LogInFailedAlreadyLOGIN (int iMsgTYPE, char *szUserID)
	{		
		return g_pListACCOUNT->Gums_KickOutUSER( szUserID, BILLING_MSG_KICKOUT_ALREADY_LOGIN );
	}
	bool AS_gumSOCKET::RecvGums_LogInFailedOutOfIP (int iMsgTYPE, char *szUserID, char *szUserTAG)
	{		
		return g_pListACCOUNT->Gums_KickOutUSER( szUserID, BILLING_MSG_KICKOUT_OUT_OF_IP );
	}
	bool AS_gumSOCKET::RecvGums_LogInFailedKickedUSER (int iMsgTYPE, char *szUserID)
	{	// 기존 사용자 접속을 끊었다.
		return g_pListACCOUNT->Gums_KickOutUSER( szUserID, BILLING_MSG_KICKOUT_EXIST_ACCOUNT );
	}
	bool AS_gumSOCKET::RecvGums_LogInFailedTimeEXPIRED (int iMsgTYPE, char *szUserID, char *szString )
	{
		return g_pListACCOUNT->Gums_KickOutUSER( szUserID, BILLING_MSG_KICKOUT_TIME_EXPIRED );
	}


	// 프리 로그인 실패
	bool AS_gumSOCKET::RecvGums_PreLoginFAILED (int iMsgTYPE, char *szAccount)					{		return true;	}

	bool AS_gumSOCKET::RecvGums_LoginSUC (eGumsNATION eNation, char *szAccount, char *szGumsPKET, char *szUserTAG, int iPayType)
	{
		//	국가마다 iPayType의 값이 틀리다...
		DWORD dwPlayFlag = this->GetPlayingFLAG( eNation, iPayType );
		WORD wMsgType;
		switch( eNation ) {
			case GUMS_KOREA :
				wMsgType = EXT_BILLING_MSG_PAY_KOREA;
				break;
			case GUMS_JAPAN	:
				wMsgType = EXT_BILLING_MSG_PAY_JAPAN;
				break;
		}

		return g_pListACCOUNT->Gums_LoginSUC( wMsgType, szAccount, iPayType, dwPlayFlag );
	}

	bool AS_gumSOCKET::RecvGums_TimeOUT (int iMsgTYPE, char *szAccount)
	{		
		// KEEP ALIVE...
		return g_pListACCOUNT->Gums_TimeOUT( szAccount );
	}
	bool AS_gumSOCKET::RecvGums_TimeALERT  (int iMsgTYPE, char *szAccount, char *szRemainMin)
	{
		// 채널 섭에 남은 시간 보냄...
		return g_pListACCOUNT->Gums_Message( BILLING_MSG_TYPE_TIME_ALERT, szAccount, szRemainMin );
	}
	bool AS_gumSOCKET::RecvGums_TimeALERT_EXT( int iMsgTYPE, char *szAccount, char *szRemainMin, DWORD dwWarnPlayFlag )
	{
		return g_pListACCOUNT->Gums_Message_EXT( EXT_BILLING_MSG_TYPE_TIME_ALERT, szAccount, szRemainMin, dwWarnPlayFlag );
	}

	bool AS_gumSOCKET::RecvGums_SetExpireDATEorTIME( int iMsgTYPE, char *szAccount, char *szMsg )
	{
		if ( GUMS041_REPLY_EXPIRE_DAY == iMsgTYPE ) {
			// 정액 사용자의 종료일자를 돌려준다
			return g_pListACCOUNT->Gums_Message( BILLING_MSG_TYPE_SET_DATE, szAccount, szMsg );
		} 
		// 종량 사용자의 남은 시간을 돌려준다.
		return g_pListACCOUNT->Gums_Message( BILLING_MSG_TYPE_SET_TIME, szAccount, szMsg );
	}
	bool AS_gumSOCKET::RecvGums_SetExpireDATEorTIME_EXT( int iMsgTYPE, char *szAccount, char *szMsg )
	{
		if ( GUMS1041_REPLY_EXPIRE_DAY == iMsgTYPE ) {
			// 정액 사용자의 종료일자를 돌려준다
			return g_pListACCOUNT->Gums_Message_EXT( EXT_BILLING_MSG_TYPE_SET_DATE, szAccount, szMsg, 0 );
		} 
		// 종량 사용자의 남은 시간을 돌려준다.
		return g_pListACCOUNT->Gums_Message_EXT( EXT_BILLING_MSG_TYPE_SET_TIME, szAccount, szMsg, 0 );
	}
	

	// 사용 기간 만료
	bool AS_gumSOCKET::RecvGums_TimeEXPIRED (int iMsgTYPE, char *szUserID, char *szString )
	{
		return g_pListACCOUNT->Gums_KickOutUSER( szUserID, BILLING_MSG_KICKOUT_TIME_EXPIRED );	// RecvGums_TimeEXPIRED
	}
	bool AS_gumSOCKET::RecvGums_TimeEXPIRED_EXT (int iMsgTYPE, char *szUserID, DWORD dwDelPlayFlag )
	{
		return g_pListACCOUNT->Gums_Message_EXT( EXT_BILLING_MSG_TYPE_TIME_EXPIRED, szUserID, NULL, dwDelPlayFlag );
	}

	bool AS_gumSOCKET::RecvGums_DupUserID (int iMsgTYPE, char *szUserID)
	{
		return g_pListACCOUNT->Gums_KickOutUSER( szUserID, BILLING_MSG_KICKOUT_DUP_LOGIN );	// RecvGums_DupUserID
	}
#endif


//-------------------------------------------------------------------------------------------------
VOID CALLBACK AS_TimerProc (HWND hwnd/* handle to window */, UINT uMsg/* WM_TIMER message */, UINT_PTR idEvent/* timer identifier */, DWORD dwTime/* current system time */ ) 
{
	AS_gumSOCKET *pInstance = AS_gumSOCKET::GetInstance();

	switch( idEvent ) {
		case AS_TIMER_GUMS_CONN :
		{
			if ( pInstance )
				pInstance->Connect ();
			break;
		}

		case AS_TIMER_GUMS_ALIVE :
		{
			if ( pInstance ) {
				pInstance->Send_ServerKeepAliveMessage ();
			}
			break;
		}
	} ;
}


//-------------------------------------------------------------------------------------------------
bool ASGUM_Socket::WndPROC( WPARAM wParam, LPARAM lParam )
{
	AS_gumSOCKET *pInstance = AS_gumSOCKET::GetInstance();
	if ( !pInstance )
		return false;
		
	return pInstance->Proc_SocketMSG( wParam, lParam );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
AS_gumSOCKET::AS_gumSOCKET() // : m_csHEAD( 4000 )
{
	assert( NULL == m_pInstance );

	m_pRecvPket = (t_PACKET*) new char[ MAX_PACKET_SIZE ];

	m_pSendAliveTimer = NULL;
	m_pReconnectTimer = NULL;
	m_bTryCONN = false;
	m_bGumsINIT = false;

	m_pInstance = this;

	SYSTEMTIME ST;
	::GetLocalTime( &ST );
	char szLogFile[ MAX_PATH ];
#ifdef	__SHO_LS_LIB
	sprintf( szLogFile, "LSgums%d_%d_%d.log", ST.wYear, ST.wMonth, ST.wDay );
#else
	sprintf( szLogFile, "ASgums%d_%d_%d.log", ST.wYear, ST.wMonth, ST.wDay );
#endif

#ifdef	__GUMS_LOG
	m_fpLOG = fopen( szLogFile, "a");
	this->Log( true, "      << Log start >>");
#else
	m_fpLOG = NULL;
#endif
}

AS_gumSOCKET::~AS_gumSOCKET ()
{
	if ( m_pInstance ) {
		if ( m_fpLOG )
			fclose( m_fpLOG );

		m_pInstance = NULL;

		SAFE_DELETE( m_pSendAliveTimer );
		SAFE_DELETE( m_pReconnectTimer );
		SAFE_DELETE_ARRAY( m_pRecvPket );
	}
}
void AS_gumSOCKET::Log( bool bRecv, char *szLog ) 
{
	static SYSTEMTIME ST;
	if ( m_fpLOG ) {
		::GetLocalTime( &ST );

		this->m_csLOG.Lock ();
			fprintf( m_fpLOG, "%s [%d-%d-%d, %d:%d:%d] %s\n", bRecv?"Recv":"Send", ST.wYear, ST.wMonth, ST.wDay, ST.wHour, ST.wMinute, ST.wSecond, szLog );
		this->m_csLOG.Unlock ();
	}
}

//-------------------------------------------------------------------------------------------------
void AS_gumSOCKET::Init (HWND hWND, char *szGumServerIP, int iGumServerPort, UINT uiSocketMSG)
{
//	m_iGameID = 1;
	m_iCpID = 1;

	m_hMainWND = hWND;
	m_GumServerIP.Set( szGumServerIP );
	m_GumServerPORT = iGumServerPort;
	m_uiSocketMSG = uiSocketMSG;

	if ( NULL == m_pReconnectTimer )
		m_pReconnectTimer = new CTimer( m_hMainWND, AS_TIMER_GUMS_CONN,  RECONNECT_TIME_TICK, (TIMERPROC)AS_TimerProc );

	if ( NULL == m_pSendAliveTimer )
		m_pSendAliveTimer = new CTimer( m_hMainWND, AS_TIMER_GUMS_ALIVE, SEND_ALIVE_TIME_TICK, (TIMERPROC)AS_TimerProc );
}

//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Connect()
{
	if ( !m_bGumsINIT ) {
		// m_bGumsINIT = false;
		return m_SockGUM.Connect( m_hMainWND, m_GumServerIP.Get(), m_GumServerPORT, m_uiSocketMSG);
	}
	return true;
}
void AS_gumSOCKET::Disconnect ()
{
	m_bGumsINIT = false;

	if ( m_pReconnectTimer )
		m_pReconnectTimer->Stop ();

//	this->Lock ();
	{
		m_SockGUM.Close ();
	}
//	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Send_GumsPACKET (int iMsgType, char *szString, int iSeqNO, bool bCheckInit)
{
	if ( bCheckInit && !this->m_bGumsINIT )
		return false;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	char szGumHeader[ 32 ];
//	m_csHEAD.Lock ();
	{
		DWORD dwSeqNO;
		if ( iSeqNO ) {
			dwSeqNO = (DWORD)iSeqNO;
		} else {
			dwSeqNO = (DWORD)::InterlockedIncrement( &this->m_lGumsSqlNO );
		}
		sprintf( szGumHeader, "GUMP/1.2\t%u\x1d%d\t", dwSeqNO, iMsgType );

		pCPacket->m_HEADER.m_wType	= 0xffff;
		pCPacket->m_HEADER.m_nSize	= sizeof( t_PACKETHEADER );
		
		pCPacket->AppendData( szGumHeader, (short)( strlen(szGumHeader) ) );	// 스트링으로 붙이면 \0이 붙어서 안됨...	
	}
//	m_csHEAD.Unlock ();

	pCPacket->AppendString( szString );
	m_SockGUM.Packet_Register2SendQ( pCPacket );

#ifdef	__SHO_LS_LIB
	g_LOG.CS_ODS( 0xffff, "LS SEND:: %s\n", (char*)&pCPacket->m_pDATA[ sizeof(t_PACKETHEADER) ] );
#else
	g_LOG.CS_ODS( 0xffff, "AS SEND:: %s\n", szString );
#endif

	this->Log( false, (char*)&pCPacket->m_pDATA[ sizeof(t_PACKETHEADER) ] );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}
//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Send_IntializedMessage (char *szServerName, int iServerGroupNumber, int iServerNumber, int iZoneNumber, int iGumsKeepAlive, int iUserKeepAlive )
{
	char szGumBody[ 256 ];
	// GUMPHeader#MsgType/ServerName/ServerGroupNumber/ServerNumber/ZoneNumber/
	sprintf( szGumBody, "%s\t%d\t%d\t%d\t%d\t%d\t", szServerName, iServerGroupNumber, iServerNumber, iZoneNumber, iGumsKeepAlive, iUserKeepAlive );
	return this->Send_GumsPACKET( GUMS001_REQ_INITIALIZED, szGumBody, 0, false );
}
//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Send_ServerKeepAliveMessage ()
{
	// GUMPHeader#MsgType/heart_beat/
	return this->Send_GumsPACKET( GUMS002_REQ_SERVER_KEEP_ALIVE, "heart_beat\t" );
}
//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Send_RequestLOGIN (bool bKickAlreadyLogin, int iGameID, char *szUserID, char *szPassword, char *szRealIP, char *szVirtualIP)
{
	char szGumBody[ 256 ];
	// 로그인 요청 GUMPHeader#MsgType/Disconnected/GameId/UserID/CpID/UserPwd/UserRealIp/UserVirtualIp/
	sprintf( szGumBody, "%d\t%d\t%s\t%d\t%s\t%s\t%s\t", bKickAlreadyLogin,
					iGameID, szUserID, m_iCpID, szPassword, szRealIP, szVirtualIP ? szVirtualIP : szRealIP);
	return this->Send_GumsPACKET( GUMS010_REQ_LOGIN, szGumBody );
}
//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Send_RequestLOGOUT (int iGameID, char *szUserID)
{
	char szGumBody[ 256 ];
	// 로그아웃 요청 GUMPHeader#MsgType/GameId/UserID/CpID/
	sprintf( szGumBody, "%d\t%s\t%d\t", iGameID, szUserID, m_iCpID );
	return this->Send_GumsPACKET( GUMS020_REQ_LOGOUT, szGumBody );
}
//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Send_UserKeepAliveMessage (int iGameID, char *szUserID)
{
	char szGumBody[ 256 ];
	// GUMPHeader#MsgType/GameId/UserID/CpID/
	sprintf( szGumBody, "%d\t%s\t%d\t", iGameID, szUserID, m_iCpID );
	return this->Send_GumsPACKET( GUMS030_REQ_USER_KEEP_ALIVE, szGumBody );
}
//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Send_RequestReaminTime (int iGameID, char *szUserID)
{
	char szGumBody[ 256 ];
	// GUMPHeader#MsgType/GameId/UserID/CpID/
	sprintf( szGumBody, "%d\t%s\t%d\t", iGameID, szUserID, m_iCpID );
	return this->Send_GumsPACKET( GUMS040_REQ_REMAIN_TIME, szGumBody );
}
//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Send_PreLogin( int iGameID, char *szUserID, char *szRealIP, char *szVirtualIP, int iUserTAG )
{
	char szGumBody[ 256 ];
	// GUMPHeader#MsgType/GameId/UserID/CpID/UserRealIp/UserVirtualIp/
	sprintf( szGumBody, "%d\t%s\t%d\t%s\t%s\t", iGameID, szUserID, m_iCpID, szRealIP, szVirtualIP ? szVirtualIP : szRealIP );
	return this->Send_GumsPACKET( GUMS110_REQ_PRELOGIN_TYPE, szGumBody, iUserTAG );
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
int AS_gumSOCKET::GetPayType( char *szPayType )
{
	static struct {
		int   m_iPayType;
		char *m_szPayType;
	} sPayType[] = {
		{	-1,						NULL	},
		{	BILLING_MSG_PAY_FU,		"FU"	},
		{	BILLING_MSG_PAY_FA,		"FA"	},
		{	BILLING_MSG_PAY_FAP,	"FAP"	},
		{	BILLING_MSG_PAY_FQ,		"FQ"	},
		{	BILLING_MSG_PAY_GU,		"GU"	},
		{	BILLING_MSG_PAY_GQ,		"GQ"	},
		{	BILLING_MSG_PAY_IQ,		"IQ"	},
		{	0,			NULL	}
	} ;

	for (int iT=1; sPayType[iT].m_szPayType; iT++)
		if ( 0 == strcmp( szPayType, sPayType[iT].m_szPayType ) )
			return sPayType[iT].m_iPayType;
	return -1;
}

//-------------------------------------------------------------------------------------------------

DWORD AS_gumSOCKET::GetPayFlag( char *szPayType )
{
/*
	0	Free Play 	캐릭작성, 채팅
	1	Entry	배틀, 베이식 필드
	2	Community	친구목록, 채팅방
	3	Trade	거래, 상점
	4	Stock Space	뱅크
	5	Extra Stock	추가뱅크 스페이스
	6	Starship Pass	행성간 이동
	7	Dungeon Adventure	던젼 필드
	8	Extra Character	작성가능 캐릭 추가
*/
	DWORD dwPayFlag = 0;
	
	// Free Play 통과~
	for (short nI=1; nI<9 && szPayType[ nI ]; nI++) {
		if ( '0' != szPayType[ nI ] ) {
			// dwPayFlag |= ( 1 << (8-1+nI) );
			dwPayFlag |= ( 1 << (nI-1) );
		}
	}
	return dwPayFlag;
}


//-------------------------------------------------------------------------------------------------
void AS_gumSOCKET::Recv_GumsPACKET (char *szString, int iStrLen)
{
	//static const char *szRecvGumHeader = "GUMPHeader\x1d";
	static char *pHeaderDelimiter="\x1d";
	static char *pTabDelimiters = "\t";
	char *pGumVersion;

	CStrVAR ST;
	char *pSeqID, *pMsgType, *pGameID, *pUserID, *pCpID, *pLast;


	pGumVersion = ST.GetTokenFirst (szString, pTabDelimiters );
	if ( !pGumVersion ) {
		g_LOG.CS_ODS( 0xffff, "Invalid GUM Version::: %s", szString );
		return;
	}
	pSeqID = ST.GetTokenNext( pHeaderDelimiter );
	if ( !pSeqID ) {
		g_LOG.CS_ODS( 0xffff, "Invalid GUM SeqID ::: %s", szString );
		return;
	}

	pMsgType = ST.GetTokenNext( pTabDelimiters );
	if ( !pMsgType ) {
		g_LOG.CS_ODS( 0xffff, "Invalid GUM Body::: %s", szString );
		return;
	}

	this->Log( true, szString );

	int iMsgTYPE = atoi( pMsgType );

	if ( GUMS100_REPLY_INIT_SUCCESS == iMsgTYPE ) {
		// GUMS 초기화 성공 응답
		this->m_bGumsINIT = true;

		g_LOG.CS_ODS( 0xffff, "Recved GUM Intialized MSG[ Type:%d,  %s ]\n", iMsgTYPE, szString );
#ifndef	__SHO_LS_LIB
		// 계정 서버일경우.... GUMS가 내려갔다가 다시 올라온 상황일수 있으니까...
		// 기존 계정리스트를 모두 다시 로긴시킨다...
		g_pListACCOUNT->Gums_Connected ();
#endif
		return;
	}

	pGameID = ST.GetTokenNext( pTabDelimiters );
	pUserID = ST.GetTokenNext( pTabDelimiters );
	pCpID   = ST.GetTokenNext( pTabDelimiters );
	if ( !pGameID || !pUserID || !pCpID ) {
		g_LOG.CS_ODS( 0xffff, "Invalid GUM Token[ %s ]::: MsgType:%s, GamdID:%s, UserID:%s, CpID:%s\n", szString, pMsgType, pGameID, pUserID, pCpID );
		return;
	}

	pLast   = ST.GetTokenNext( pTabDelimiters );
	switch( iMsgTYPE ) {
		default :
			g_LOG.CS_ODS( 0xffff, "Unknow GUM MsgType[ %s ]::: Type: %d, GamdID:%s, UserID:%s, CpID:%s\n", szString, iMsgTYPE, pGameID, pUserID, pCpID );
			return;

		// Response Message TYPE 1	:: pLast == PlayType
		case 	GUMS011_REPLY_LOGIN_SUCCESS		:
		case 	GUMS111_REPLY_FREE_LOGIN_SUCCESS	:
		case	GUMS1011_REPLY_LOGIN_SUCCESS		:
		case	GUMS1111_REPLY_FREE_LOGIN_SUCCESS	:
		{
			if ( pLast == NULL ) {
				g_LOG.CS_ODS( 0xffff, "ERROR in GUMS  :: Type:%d, PayType == NULL[ %s ]\n", iMsgTYPE, szString );
				return;
			}
			eGumsNATION eNation = Gums_NATION( iMsgTYPE );
			switch( eNation ) {
				case GUMS_JAPAN :		// JAPAN !!! 
				{
					DWORD dwPayFlag = ( NULL == pLast ) ? PLAY_FLAG_KOREA_DEFAULT : GetPayFlag( pLast );
					this->RecvGums_LoginSUC( eNation, pUserID, szString, pSeqID, dwPayFlag );
					return;
				}
				case GUMS_KOREA :
				{
					int iPayType = ( NULL == pLast ) ? BILLING_MSG_PAY_FU : GetPayType( pLast );
					switch( iPayType ) {
						case BILLING_MSG_PAY_FU	 :	// FU	로그인 된 사용자는 무료 아이디 사용자임을 나타낸다.
						case BILLING_MSG_PAY_FA  :	// FA	로그인 된 사용자는 개인정액 사용자임을 나타낸다.
						case BILLING_MSG_PAY_FAP :	// FAP	로그인 된 사용자는 개인정액 사용자(플레티넘)임을 나타낸다.
						case BILLING_MSG_PAY_FQ	 :	// FQ	로그인 된 사용자는 개인정량 사용자임을 나타낸다.
						case BILLING_MSG_PAY_GU  :	// GU	로그인 된 사용자는 게임방 사용자임을 나타낸다.
						case BILLING_MSG_PAY_GQ  :	// GQ	게임방에서 로그인을 시도하였으나 게임방의 과금기간(시간)이 만료되어 개인정량으로 로그인 되었음을 나타낸다.
						case BILLING_MSG_PAY_IQ  :	// IQ	게임방에서 로그인을 시도하였으나 이미 계약된 IP 숫자를 모두 사용하고 있어 개인정량으로 로그인 되었음을 나타낸다.
							this->RecvGums_LoginSUC( eNation, pUserID, szString, pSeqID, iPayType );
							return;
						default		 :
							;
							// assert( "Unknown pay type.." && 0 );
					}
					break;
				}
			}
			// 아래 공짜 서버 사용만 할수 있다..
			//g_LOG.CS_ODS( 0xffff, "%d login success:: %s \n", iMsgTYPE, szString );
			//return;
		}
		case 	GUMS112_REPLY_FREE_SRV_ONLY			:	// 프리 로그인 실패
		case	GUMS1112_REPLY_FREE_SRV_ONLY		:
			// g_LOG.CS_ODS( 0xffff, "%d allow only free server :: %s \n", iMsgTYPE, szString );
			this->RecvGums_LoginOnlyFREE( iMsgTYPE, pUserID, pSeqID );
			return;

		// Response Message type 2	:: pLast == NULL
		case 	GUMS012_REPLY_LOGIN_FAILED_INVALID_ID_OR_PW		:	// 사용자의 ID나 PW가 틀린경우
			g_LOG.CS_ODS( 0xffff, "%d invalid id or password :: %s \n", iMsgTYPE, szString );
		case 	GUMS013_REPLY_LOGIN_FAILED_NO_RIGHT				:	// 게임할 권한이 없는 사용자
		case	GUMS113_REPLY_LOGIN_FAILED			:
			this->RecvGums_LogInFailedNoRIGHT( iMsgTYPE, pUserID, pSeqID );
			return;

		case 	GUMS016_REPLY_LOGIN_FAILED_IP_FULL_USED			:	// 결제한 IP의 수량을 모두 사용중인 경우(겜방)
		case	GUMS116_REPLY_FREE_LOGIN_FAILED_IP_FULL_USED	:
			this->RecvGums_LogInFailedOutOfIP( iMsgTYPE, pUserID, pSeqID );
			return;

#ifndef	__SHO_LS_LIB
		case 	GUMS014_REPLY_LOGIN_FAILED_TIME_EXPIRED			:	// 사용 기간 만료
			this->RecvGums_LogInFailedTimeEXPIRED( iMsgTYPE, pUserID, szString );
			return;

		case 	GUMS018_REPLY_LOGIN_FAILED_ONE_DAY_EXPIRED		:	// 일일 사용 기간이 만료된 사용자
		case 	GUMS019_REPLY_LOGIN_FAILED_ONE_DAY_EXPIRED2		:	// 일일 사용 기간이 만료된 사용자
			this->RecvGums_LogInFailedOneDayEXPIRED ( iMsgTYPE, pUserID );
			return;

		case 	GUMS015_REPLY_LOGIN_FAILED_ALREADY_LOGIN			:	// 이미 로그인 한 사용자
			this->RecvGums_LogInFailedAlreadyLOGIN ( iMsgTYPE, pUserID );
			return;

		case 	GUMS017_REPLY_LOGIN_FAILED_KICKED_USER			:	// 접속 해지 로그인 요청시 기존 사용자 로그 아웃됐다고 통보 받음..
			// g_LOG.CS_ODS( 0xffff, "%d kickout from new login :: %s \n", iMsgTYPE, szString );
			this->RecvGums_LogInFailedKickedUSER( iMsgTYPE, pUserID );
			return;

		case 	GUMS021_REPLY_LOGOUT_SUCCESS		:	// 사용자 로그아웃 성공
			// g_LOG.CS_ODS( 0xffff, "%d logout success :: %s \n", iMsgTYPE, szString );
			// 로그아웃 요청에 의한것이므로 걍 무시~~~
			return;
		case	GUMS022_REPLY_LOGOUT_FAILED			:	// 로그아웃 실패
			// 로그아웃 요청에 의해 전송 받은것...
			// g_LOG.CS_ODS( 0xffff, "%d logout failed:: %s \n", iMsgTYPE, szString );
			return;

		case 	GUMS088_REPLY_DO_INIT_FIRST			:	// Connection이 초기화 되지 않았을 경우 MsgType 이 88 
		case 	GUMS099_REPLY_INVALID_MSG_FORMAT	:	// 메세지 형식이 틀렸을 경우
		case 	GUMS044_REPLY_NETWORK_IS_NOT_GOOD	:	// 네트웍 사용상황이 좋지 않아 메시지 처리가 불가능.
			break;

		// Response Message type 3	:: pLast == Remain Time
		case 	GUMS041_REPLY_EXPIRE_DAY			:	// 정액 사용자의 종료일자를 돌려준다
		case 	GUMS042_REPLY_EXPIRE_DAY2			:	// 종량 사용자의 남은 시간을 돌려준다.
		{
			//종료일자 형식 : YYYYMMDDhhmm
			//[YYYY : 년, MM : 월, DD : 일, hh : 시(24시간 기준), mm(분)]
			//GUMP/1.2/1#41/1/subsub/1/200309101025/

			// GUMP/1.2/1#42/1/subsub/1/122/
			if ( NULL == pLast ) {
				g_LOG.CS_ODS( 0xffff, "DATE or TIME == NULL :: Recv GUMS %d _REPLY_EXPIRE_DAY:: %s, %s\n", iMsgTYPE, pUserID, szString );
				return;
			}
			this->RecvGums_SetExpireDATEorTIME( iMsgTYPE, pUserID, pLast );
			return;
		}

		case	GUMS1041_REPLY_EXPIRE_DAY			:
		case	GUMS1042_REPLY_EXPIRE_DAY2			:
		{
			// GUMP/1.2/1#41/1/subsub/1/1/200609101025/4/200609251025
			// Entry 요금제의 만료시간은 2006년 9월 10일 10:25 이며 Stock Space 요금제의 만료시간은 2006년 9월 25일 10:25이다.
			char szTmp1[256], szTmp2[256] = { 0, } ;
			char *pDate = ST.GetTokenNext( pTabDelimiters );
			while( pLast && pDate ) {
				sprintf( szTmp1, "%s\t%s\t", pLast, pDate );
				strcat( szTmp2, szTmp1 );
				pLast = ST.GetTokenNext( pTabDelimiters );
				pDate = ST.GetTokenNext( pTabDelimiters );
			}
			int iStrLen = strlen( szTmp2 );
			if ( iStrLen > 1 ) {
				szTmp2[ iStrLen-1 ] = 0;
				this->RecvGums_SetExpireDATEorTIME_EXT( iMsgTYPE, pUserID, szTmp2 /* &szString[ xxxxx ] */ /* pLast */ );
			}
			return;
		}

		//
		// Callback Messages		:: pLast == Time(남은시간(분)을 나타낸다)
		//
		// 1. 경고 메세지
		case 	GUMS051_REPLY_DAY_EXPIRED_ALERT		:	// (1)	기간 만료 경고(정액) :: 결제한 사용기간이 만료되기 몇 분전에 Game Server에게 전달되는 메시지다. MsgType 이 51
		case 	GUMS052_REPLY_ONE_DAY_TIME_OVER		:	// (2)	일일 사용시간 한도 초과 경고:: 일일 사용시간 한도 - 예를 들면 하루 1시간 사용가능 - 를 초과하기 몇 분전에 Game Server에게 전달되는 메시지다. MsgType 이 52
		case 	GUMS053_REPLY_ONE_DAY_TIME_OVER2	:	// (3)	일일 사용기간 범위 초과 경고:: 일일 사용기간 - 예를 들면 18:00 ~ 20:00 시 - 을 벗어나기 몇 분전에 Game Server에게 전달되는 메시지다. MsgType 이 53
		case 	GUMS054_REPLY_TIME_EXPIRED_ALERT	:	// (4)	사용시간(정량) 초과 경고 :: 정량제 사용시간 만료 몇 분전에 Game Server에게 전달되는 메시지다. MsgType 이 54
			// g_LOG.CS_ODS( 0xffff, "%d time out alert:: %s \n", iMsgTYPE, szString );
			if ( pLast )
				this->RecvGums_TimeALERT( iMsgTYPE, pUserID, pLast );
			return;

		case	GUMS1051_REPLY_DAY_EXPIRED_ALERT	:
		case	GUMS1052_REPLY_ONE_DAY_TIME_OVER	:
		case	GUMS1053_REPLY_ONE_DAY_TIME_OVER2	:
		case	GUMS1054_REPLY_TIME_EXPIRED_ALERT	:
		{
			if ( pLast ) {
				char *szFlags = ST.GetTokenNext( pTabDelimiters );
				if ( szFlags ) {
					DWORD dwWarnPlayFlag = this->GetPayFlag( szFlags );
					this->RecvGums_TimeALERT_EXT( iMsgTYPE, pUserID, pLast, dwWarnPlayFlag );
				}
			}
			return;
		}

		// 2. 접속 해제 요청 메세지
		case 	GUMS061_REPLY_DAY_EXPIRED_DIS		:	// (1)	기간 만료 접속 해제 메시지(정액) :: 결제한 사용기간이 만료되어 사용자의 접속을 해제하라는 메시지이다. MsgType 이 61
		case 	GUMS062_REPLY_ONE_DAY_EXPIRED_DIS	:	// (2)	일일 사용시간 한도 초과 접속 해제 메시지 :: 일일 사용시간 한도가 초과되어 사용자의 접속을 해제하라는 메시지이다. MsgType 이 62
		case 	GUMS063_REPLY_ONE_DAY_EXPIRED_DIS2	:	// (3)	일일 사용기간 범위 초과 접속 해제 메시지 :: 일일 사용기간 범위가 초과되어 사용자의 접속을 해제하라는 메시지이다. MsgType 이 63
		case 	GUMS066_REPLY_TIME_EXPIRED_DIS		:	// 사용시간(정량) 만료 접속 해제 메시지
			// g_LOG.CS_ODS( 0xffff, "%d time expired :: %s \n", iMsgTYPE, szString );
			this->RecvGums_TimeEXPIRED( iMsgTYPE, pUserID, szString );
			return;

		case 	GUMS1061_REPLY_DAY_EXPIRED_DIS		:	// 사용기간(정액) 만료 접속 해제 메세지
		case 	GUMS1062_REPLY_ONE_DAY_EXPIRED_DIS	:	// 일일 사용시간 한도 초과 접속 해제 메시지
		case 	GUMS1063_REPLY_ONE_DAY_EXPIRED_DIS2 :	// 일일 한도시간 한도 초과 접속 해제 메시지
		case	GUMS1066_REPLY_TIME_EXPIRED_DIS		:	
		{
			char *szFlag = pLast;
			if ( pLast ) {
				DWORD dwDelPlayFlag = this->GetPayFlag( pLast );
				this->RecvGums_TimeEXPIRED_EXT( iMsgTYPE, pUserID, dwDelPlayFlag );
			}
			return;
		}

		case 	GUMS064_REPLY_DUP_USERID_DIS		:	// (4)	다른 사용자의 요구로 접속 해제 메시지 :: 같은 아이디로 다른 사용자가 로그인을 하면서 이미 로그인 되어 있던 기존 사용자를 로그 아웃 시킬 때 기존 사용자에게 전달되는 메시지이다. MsgType 이 64
		case	GUMS1064_REPLY_DUP_USERID_DIS		:
			g_LOG.CS_ODS( 0xffff, "%d time expired or duplicated ID :: %s \n", iMsgTYPE, szString );
			this->RecvGums_DupUserID( iMsgTYPE, pUserID );
			return;

		case 	GUMS065_REPLY_USER_NOT_FOUND		:	// 사용자 Live 신호가 왔을때 사용자가 메모리 내에 없을 경우 보내주는 메시지
		case	GUMS1065_REPLY_USER_NOT_FOUND		:
			// 로그인 서버에는 있는 껌서버에 없덴다
			// 이경우는 gums와 접속이 끊긴후 재접 됐을때... 
			// 기존 접속자 정보를 보내면 gums에서 감당해 내지 못하기 때문에...
			// 현재 계정 응답온 계정이 있는지 조사해서 있으면 로그인 요청...
			if ( this->RecvGums_TimeOUT( iMsgTYPE, pUserID ) /* g_pListACCOUNT->RecvGums_TimeOUT( pUserID ) */ ) {
				;// g_LOG.CS_ODS( 0xffff, "%d user Request relogin :: %s \n", iMsgTYPE, szString );
			} else
				g_LOG.CS_ODS( 0xffff, "TimeOut:: %d user not found:: %s \n", iMsgTYPE, szString );
			return;
#endif
	}

	g_LOG.CS_ODS( 0xffff, "!!!! Recved GUM Msg[ %s ]::: Type: %d, GamdID:%s, UserID:%s, CpID:%s, Last:%s\n", szString, iMsgTYPE, pGameID, pUserID, pCpID, pLast );
}

//-------------------------------------------------------------------------------------------------
bool AS_gumSOCKET::Proc_SocketMSG (WPARAM wParam, LPARAM lParam)
{
	int nErrorCode = WSAGETSELECTERROR(lParam);
	switch ( WSAGETSELECTEVENT(lParam) ) {
		case FD_READ:
		{
			m_SockGUM.OnReceive( nErrorCode );

			// 받은 패킷 처리..
			while( m_SockGUM.Peek_Packet( m_pRecvPket, true ) ) {
				// LogString( LOG_DEBUG, "Handle LS Packet: Type[ 0x%x ], Size[ %d ]\n", m_pRecvPket->m_HEADER.m_wType, m_pRecvPket->m_HEADER.m_nSize);
				if ( 0x0ffff == m_pRecvPket->m_HEADER.m_wType ) {
					// m_pRecvPket->m_pDATA[ m_pRecvPket->m_HEADER.m_nSize ] = 0;
					short nOffset = sizeof( t_PACKETHEADER );
					char *szString = Packet_GetStringPtr( m_pRecvPket, nOffset );
#ifdef	__SHO_LS_LIB
					g_LOG.CS_ODS( 0xffff, "LS RECV:: %s\n", szString );
#else
					g_LOG.CS_ODS( 0xffff, "AS RECV:: %s\n", szString );
#endif

					this->Recv_GumsPACKET( szString, m_pRecvPket->m_HEADER.m_nSize-sizeof( t_PACKETHEADER ) );
				} else {
					g_LOG.CS_ODS( 0xffff, "ERROR !!!! :: Undefined Account server packet... Type[ 0x%x ], Size[ %d ]\n", m_pRecvPket->m_HEADER.m_wType, m_pRecvPket->m_HEADER.m_nSize);
				}
			}
			break;
		}
		case FD_WRITE:
			m_SockGUM.OnSend ( nErrorCode );
			break;

		case FD_CONNECT: 
		{
			m_SockGUM.OnConnect ( nErrorCode );
			if ( !nErrorCode ) {
				g_LOG.CS_ODS( 0xffff, "Connected to GUMS server \n");

				m_lGumsSqlNO = 0;
				m_bTryCONN = false;

				if ( m_pReconnectTimer )
					m_pReconnectTimer->Stop ();

				m_pSendAliveTimer->Start ();

				// iMsgType, szServerName, iServerGroupNumber, iServerNumber, iZoneNumber
				// 라이브 6분설정... : 게임에서 5분마나 온다.

			#ifdef	__SHO_LS_LIB
				this->Send_IntializedMessage( "roseON_LS", 1, 1, 1, 3, 10 );
			#else
				this->Send_IntializedMessage( "roseON_AS", 1, 2, 1, 3, 10 );
			#endif
 			} else {
				m_pSendAliveTimer->Stop ();

				if ( m_pReconnectTimer )
					m_pReconnectTimer->Start ();
				if ( !m_bTryCONN ) {
					m_bTryCONN = true;
					g_LOG.CS_ODS( 0xffff, "Connect failed to GUMS server \n");
				}
			}
			break;
		}
		case FD_CLOSE:		// Close()함수를 호출해서 종료될때는 발생 안한다.
		{
			m_bGumsINIT = false;

			m_SockGUM.OnClose ( nErrorCode );

			g_LOG.CS_ODS( 0xffff, "Disconnected form GUMS server \n");

			if ( m_pReconnectTimer )
				m_pReconnectTimer->Start ();
		}
	}

	return true;
}
