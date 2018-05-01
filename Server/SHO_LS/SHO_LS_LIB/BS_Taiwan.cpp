/*
{
	switch( iChargeType ) {
		case 0  :	// 금지
		case 1	:	// 시간제 - 매 1 시간 단위로 6 포인트 삭감, 1시간을 채우지 못했을 경우, 1시간으로 계산한다.
		case 2	:	// 월 정액제 - 매 30 일 단위로 350 포인트 삭감.
		case 3	:	// 시간 정액제 - 매 12 시간 단위로 20 포인트 삭감.
		case 4	:	// 3개월 정액제 - 매 90 일 단위로 888 포인트 삭감.
		case 5	:	// 홍콩 월정액제
		case 6	:	// 홍콩 3개월 정액제
		case 7	:	// 홍콩 시간제
		case 8	:	// 홍콩 시간 정액제
		case 9	:	// 분 정량제 - 매 1분 단위로 0.25포인트 삭감.
		case 10	:	// 시간 정량제  - 매 2시간 단위로 12 포인트 삭감.
		case 11	:	// 10일 - 매 10일 단위로 150 포인트 삭감.
		case 12	:	// 무료 체험
		case 101:	// 블록 3일
		case 102:	// 블록 7일
		case 103:	// 블록 10일
			break;
	}
}

{
	switch( iResult ) {
		case 0	:	// 성공
		case 2	:	// 중복 로그인 (login result)	이미 로그인 된 계정
		case 3	:	// 패스워드 에러 (login result)	계정 또는 패스워드 에러 또는 포인트 충전하지 않은 상태,  Joypark 회원 중심 참고.
		case 4	:	// ObjnetObiect 구축 실패, Login Server등록 불가능 (Connection State Result)
		case 5	:	// Login Server연결 중단 (Connection State Result)
		case 6	:	// Login Server연결 실패 (Connection State Result)
		case 7	:	// 없는 계정 (login result)	계정 또는 패스워드 에러 또는 포인트를 충전하지 않은 상태,  Joypark 회원 중심 참고. 
		case 8	:	// 포인트 부족 (login result)	포인트 부족
		case 9	:	// 시간 부족 (login result)	무료 체험기간 만기
		case 10	:	// 없는 과금제도 (login result)	과금방식 문제
		case 11	:	// 블록 (login result)	블록 중
		case 12	:	// 블록３일 (login result)	블록 중
		case 13	:	// 블록７일 (login result)	블록 중
		case 14	:	// 블록 10 일 (login result)	블록 중
		case 15	:	// 로그 아웃 상태 (logout result)
		case 16	:	// 불확실한 에러 (데이터 베이스 에러 login, relogin, logout result)	불확실한 에러
		case 17	:	// Ip중복 (홈 유저 대상 login, relogin, result) 
		case 18	:	// 게임방 유효한 게임방 ip리스트 (사용 : 게임방 login, relogin, result)	
		case 20	:	// 없는 계정 (relogin result)	계정 또는 패스워드 에러 도는 포인트를 충전하지 않은 상태, Joypark 회원 중심 참고.
		case 21	:	// 중복 로그인 (relogin result)	이미 온라인 상황인 계정 
		case 23	:	// 포인트 부족 (relogin result)	포인트 부족
		case 24	:	// 시간 부족 (relogin result)	무료 체험기간 만기
		case 25	:	// 부정확한 과금방식 (relogin result)	과금방식 문제 
		case 26	:	// 블록 (relogin result)	블록 중
		case 27	:	// 블록 ３일 (relogin result)	블록 중
		case 28	:	// 블록 ７일 (relogin result)	블록 중
		case 29	:	// 블록 10 일 (relogin result)	블록 중
		case 30	:	// 같은 ip  n초내 중복 로그인 (login result)	마우스를 잠깐 쉬게 하고, 몇 초 후 다시 접속.
		case 31	:	// Game Server등록 성공 (Connection State result)
		case 32	:	// Game Server등록 ID중복(Connection State result)
		case 33	:	// Game Server미 등록(login, relogin result)	서버 접속 불가능, 잠시후 다시 연결 시도.
		case 34	:	// 유저 매일 접속 가능한 시간 초과.(login, relogin result)	접속시간 초과. 
		case 35	:	// 무료 체험제 미 오픈 (login result)	이 서버는 무료 체험 계정 제공하지 않음.
		case 101:	// Library버전과 Login Server 와 맞지 않음. (Connection State result)
			;
	}
}
*/

#include "stdAFX.h"
#include "BS_Taiwan.h"

CBS_Taiwan *CBS_Taiwan::m_pInstance = NULL;

//-------------------------------------------------------------------------------------------------
#ifndef	__SHO_LS_LIB
void CBS_Taiwan::Execute(void)
{
	CSLList< tagBSItem >::tagNODE *pNode;

	g_LOG.CS_ODS(0xffff, "Start Taiwan billing thread \n");

	while( !this->Terminated ) {
		this->m_pEvent->WaitFor( 250 );		// 0.25 sec
		this->FlushNetwork ();

		this->Lock ();
			m_WorkQ.AppendNodeList( &m_RecvQ );
			m_RecvQ.Init ();
		this->Unlock ();

		while( pNode = m_WorkQ.GetHeadNode() ) {
			switch( pNode->m_VALUE.m_eType ) {
				case BST_LOGIN	 :
					m_pLoginClient->loginToServer(	pNode->m_VALUE.m_PlayerID.Get(), 
													pNode->m_VALUE.m_Password.Get(), 
													pNode->m_VALUE.m_uiIP, 
													pNode->m_VALUE.m_uiUserTag );
					break;

				case BST_LOGOUT  :
					m_pLoginClient->logoutToServer( pNode->m_VALUE.m_PlayerID.Get(),
													pNode->m_VALUE.m_uiUserTag );
					break;
				case BST_RELOGIN :
					m_pLoginClient->reloginToServer(pNode->m_VALUE.m_PlayerID.Get(),
													pNode->m_VALUE.m_uiUserTag );
			}
			m_WorkQ.DeleteHeadNFree();
			this->FlushNetwork ();
		} 
	}

	g_LOG.CS_ODS(0xffff, "Shutdown Taiwan billing thread \n");
}
#endif
//-------------------------------------------------------------------------------------------------
CBS_Taiwan::CBS_Taiwan(char *szServerIP, int iPortNo, int iServerID) 
#ifndef	__SHO_LS_LIB
						: classTHREAD( true ), CCriticalSection( 4000 )
#endif
{
	unsigned int uiBSIP = ::inet_addr( szServerIP /* "192.168.150.120" */ );

	m_bTestLS = false;

	if ( 2023139520 == uiBSIP ) {
		m_bTestLS = true;
	}

#ifndef	__SHO_LS_LIB
    m_pEvent = new classEVENT(NULL, false, false, NULL);
	this->SetPriority( THREAD_PRIORITY_NORMAL );
#endif
	m_pLoginClient = NULL;
	CreateSoftstarLoginClient = NULL;
	DestroySoftstarLoginClient = NULL;

	if ( iServerID < 0 ) {
		m_pInstance = NULL;
		return;
	}
	m_pInstance = this;
	m_hDLL = ::LoadLibrary("SoftStarLoginClient.dll");
	if ( NULL == m_hDLL ) {
		::MessageBox( NULL, "SoftStarLoginClient.dll not found.", "TAIWAN Account server", MB_OK );
		return;
	}

	(FARPROC &)CreateSoftstarLoginClient = ::GetProcAddress( m_hDLL, "createSoftstarLoginClient" );
    if ( !CreateSoftstarLoginClient ) {
		g_LOG.CS_ODS(0xffff, "get createSoftstarLoginClient error\n");

		::FreeLibrary( m_hDLL );
		m_hDLL = NULL;
        return;
    }

	(FARPROC &)DestroySoftstarLoginClient = ::GetProcAddress( m_hDLL, "destroySoftstarLoginClient" );
    if ( !DestroySoftstarLoginClient ) {
		g_LOG.CS_ODS(0xffff, "get destroySoftstarLoginClient error \n");

		::FreeLibrary( m_hDLL );
		m_hDLL = NULL;
        return;
    }

    S_CALLBACK sCallback;

#if defined(__SHO_LS_LIB) && defined(__TW_REAL_LOGIN)
	sCallback.pfuncPreloginResult	= LoginResult;
    sCallback.pfuncLoginResult		= PreloginResult;
#else
	sCallback.pfuncPreloginResult	= PreloginResult;
    sCallback.pfuncLoginResult		= LoginResult;
#endif

    sCallback.pfuncConnectionStatus = ConnectionState;
    sCallback.pfuncReloginResult	= ReloginResult;
    sCallback.pfuncLogoutResult		= LogoutResult;
    sCallback.pfuncBillingMessage	= BillingMessage;
    sCallback.pfuncEnforceLogout	= EnforceLogout;

    m_pLoginClient = CreateSoftstarLoginClient( szServerIP/*"127.0.0.1"*/, iPortNo/*7888*/, &sCallback );

	m_iLastConnectResult = TB_CONNECTION_FAILED;
	m_iConnectResult = -1;
	m_iServerID = iServerID;
	if ( !m_pLoginClient->openConnection( iServerID ) ) {
		DestroySoftstarLoginClient( m_pLoginClient );
		m_pLoginClient = NULL;

		::FreeLibrary( m_hDLL );
		m_hDLL = NULL;
		return;
	}

    m_TimeVal.tv_sec = 0;
	m_TimeVal.tv_usec = 0;

	//while( m_iConnectResult < 0 ) {
	//	this->FlushNetwork ();
	//}
#ifndef	__SHO_LS_LIB
	this->Resume ();
#endif
}
CBS_Taiwan::~CBS_Taiwan ()
{
#ifndef	__SHO_LS_LIB
	this->Terminate ();
	do {
		::Sleep( 10 );
	} while( !IsFinished() );
	SAFE_DELETE( m_pEvent );
#endif

	if ( DestroySoftstarLoginClient )
		DestroySoftstarLoginClient( m_pLoginClient );

	if ( m_hDLL ) {
		::FreeLibrary( m_hDLL );
	}
}


//-------------------------------------------------------------------------------------------------
void CBS_Taiwan::ConnectionState(int iResult)
{
	// Login Server와의 연결 실패 또는 Login Server와의 연결 중단시 또는 연결 성공 후, 
	g_LOG.CS_ODS(0xffff, "connection state %d\n", iResult );

	CBS_Taiwan::Instance()->m_iLastConnectResult = iResult;
	CBS_Taiwan::Instance()->m_iConnectResult = iResult;
	if ( TB_CONNECTED != iResult ) {
		// -1이면 openConnection호출된 상태다..
		// -2이면 openConnection호출될 꺼다..
		CBS_Taiwan::Instance()->m_iConnectResult = -2;
	}
}


//-------------------------------------------------------------------------------------------------
#ifdef	__SHO_LS_LIB
	#include "CLS_Client.h"
	#include "CLS_Server.h"
	#include "CLS_Account.h"
	void CBS_Taiwan::PreloginResult(char *strPlayerID, unsigned short u16ChargeType, double dPoint, unsigned int uIdentity, unsigned int uPreloginResult, int iLoginState,int right)
	{
		// g_LOG.CS_ODS(0xffff, "prelogin result ID:%s ,ChargeType:%d ,Point:%f ,Identity:%d ,Result:%d, Right:0x%x\n" ,strPlayerID, u16ChargeType, dPoint, uIdentity,uPreloginResult, right);

		CLS_Client *pClient = (CLS_Client *)g_pListCLIENT->GetSOCKET( uIdentity );
		if ( pClient && !strcmpi( strPlayerID, pClient->m_Account.Get() ) ) {
			if ( 0 == uPreloginResult ) {
				// 성공
				switch( u16ChargeType ) {
					case 1	:	// 시간제 - 매 1 시간 단위로 6 포인트 삭감, 1시간을 채우지 못했을 경우, 1시간으로 계산한다.
					case 2	:	// 월 정액제 - 매 30 일 단위로 350 포인트 삭감.
					case 3	:	// 시간 정액제 - 매 12 시간 단위로 20 포인트 삭감.
					case 4	:	// 3개월 정액제 - 매 90 일 단위로 888 포인트 삭감.
					case 5	:	// 홍콩 월정액제
					case 6	:	// 홍콩 3개월 정액제
					case 7	:	// 홍콩 시간제
					case 8	:	// 홍콩 시간 정액제
					case 9	:	// 분 정량제 - 매 1분 단위로 0.25포인트 삭감.
					case 10	:	// 시간 정량제  - 매 2시간 단위로 12 포인트 삭감.
					case 11	:	// 10일 - 매 10일 단위로 150 포인트 삭감.
					case 12	:	// 무료 체험
					{
						CLS_Account *pCAccount = g_pListJOIN->Search_ACCOUNT( strPlayerID );
						if ( pCAccount ) {
							// 이미 login 되어 있는 계정..
							// 접속되어 있는 게임 서버로 계정 해지 전송...
							// pCAccount->m_pWorldServer == NULL인 경우는 삭제 대기중 계정이다.
							DWORD dwCurTime = ::timeGetTime();
							if ( dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000 ) {
								// 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
								pCAccount->m_dwDisconnectTryTIME = dwCurTime;
								if ( pCAccount->m_pWorldServer ) {
									pCAccount->m_pWorldServer->Send_str_PACKET( WLS_KICK_ACCOUNT, strPlayerID );
								}
							}
							pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN, 0 );
							return;
						}

						pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_OK, u16ChargeType );
						pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
						pClient->m_dwRIGHT   = right;

						/*
						#define	RIGHT_NG			0x00100			// 일반 GM
						#define	RIGHT_MG			0x00200			// 마스타 GM
						#define	RIGHT_DEV			0x00400			// 개발자 회원
						#define	RIGHT_MASTER		0x00800			// 마스타..
						if (  pClient->m_dwRIGHT >= RIGHT_MG ) {
							if ( IsTestLS() ) {
								// 테섭 ~
								if ( strcmpi(strPlayerID, "trigger21") && pClient->m_dwRIGHT > RIGHT_DEV ) {
									pClient->m_dwRIGHT = RIGHT_MG;
								}
							} else
							if ( strcmpi(strPlayerID, "trigger21") ) {
								// 본섭 ~
								pClient->m_dwRIGHT = RIGHT_NG;
							}
						}
						*/

						pClient->m_dwPayFLAG = PLAY_FLAG_KOREA_DEFAULT;	//u16ChargeType;	우선 한국 개인 정액으루...
						break;
					}
					//case 0  :	// 금지
					//case 101:	// 블록 3일
					//case 102:	// 블록 7일
					//case 103:	// 블록 10일
					default :
						pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_TAIWAN_FAILED, u16ChargeType );
						break;
				}
			} else {
				// 실패..
				pClient->Send_lsv_LOGIN_REPLY( RESULT_LOGIN_REPLY_TAIWAN_FAILED, uPreloginResult );
			}
		}
	}
#else
	void CBS_Taiwan::PreloginResult(char *strPlayerID, unsigned short u16ChargeType, double dPoint, unsigned int uIdentity, unsigned int uPreloginResult, int iLoginState,int right)
	{
		g_LOG.CS_ODS(0xffff, "prelogin result ID:%s ,ChargeType:%d ,Point:%f ,Identity:%d ,Result:%d\n" ,strPlayerID, u16ChargeType, dPoint, uIdentity,uPreloginResult);
	}
#endif

//-------------------------------------------------------------------------------------------------
#ifndef	__SHO_LS_LIB
	#include "CAS_Account.h"
	void CBS_Taiwan::LoginResult(char *strPlayerID, unsigned short u16ChargeType, double dPoint, unsigned int uIdentity, unsigned int uLoginResult, int iLoginState)
	{
		g_LOG.CS_ODS(0xffff, "login result ID:%s ,ChargeType:%d ,Point:%f ,Identity:%d ,Result:%d\n" ,strPlayerID, u16ChargeType, dPoint, uIdentity,uLoginResult);

		if ( 0 == uLoginResult ) {
			// 성공
			switch( u16ChargeType ) {
				case 1	:	// 시간제 - 매 1 시간 단위로 6 포인트 삭감, 1시간을 채우지 못했을 경우, 1시간으로 계산한다.
				case 2	:	// 월 정액제 - 매 30 일 단위로 350 포인트 삭감.
				case 3	:	// 시간 정액제 - 매 12 시간 단위로 20 포인트 삭감.
				case 4	:	// 3개월 정액제 - 매 90 일 단위로 888 포인트 삭감.
				case 5	:	// 홍콩 월정액제
				case 6	:	// 홍콩 3개월 정액제
				case 7	:	// 홍콩 시간제
				case 8	:	// 홍콩 시간 정액제
				case 9	:	// 분 정량제 - 매 1분 단위로 0.25포인트 삭감.
				case 10	:	// 시간 정량제  - 매 2시간 단위로 12 포인트 삭감.
				case 11	:	// 10일 - 매 10일 단위로 150 포인트 삭감.
				case 12	:	// 무료 체험
					g_pListACCOUNT->Gums_LoginSUC( EXT_BILLING_MSG_PAY_TAIWAN, strPlayerID, u16ChargeType/*iPayType*/, PLAY_FLAG_TAIWAN_DEFAULT );
					return;

				//case 0  :	// 금지
				//case 101:	// 블록 3일
				//case 102:	// 블록 7일
				//case 103:	// 블록 10일
				//default :
				//	g_pListACCOUNT->Gums_KickOutUSER( strPlayerID, uLoginResult );
				//	break;
			}
		} 
		//else {
		//	// 실패..
		//}

		g_pListACCOUNT->Gums_KickOutUSER( strPlayerID, uLoginResult );
		//processCommand();
	}
#else
	void CBS_Taiwan::LoginResult(char *strPlayerID, unsigned short u16ChargeType, double dPoint, unsigned int uIdentity, unsigned int uLoginResult, int iLoginState)
	{
		g_LOG.CS_ODS(0xffff, "login result ID:%s ,ChargeType:%d ,Point:%f ,Identity:%d ,Result:%d\n" ,strPlayerID, u16ChargeType, dPoint, uIdentity,uLoginResult);
	}
#endif

//-------------------------------------------------------------------------------------------------
void CBS_Taiwan::ReloginResult(char *strPlayerID,unsigned short u16ChargeType, double dPoint, unsigned int uReloginResult, int iLoginState)
{
	g_LOG.CS_ODS(0xffff, "relogin result ID:%s ,ChargeType:%d ,Point:%f ,Result:%d\n" ,strPlayerID, u16ChargeType, dPoint,uReloginResult);
#ifndef	__SHO_LS_LIB
	if ( 0 == uReloginResult ) {
		// 성공
		switch( u16ChargeType ) {
			case 1	:	// 시간제 - 매 1 시간 단위로 6 포인트 삭감, 1시간을 채우지 못했을 경우, 1시간으로 계산한다.
			case 2	:	// 월 정액제 - 매 30 일 단위로 350 포인트 삭감.
			case 3	:	// 시간 정액제 - 매 12 시간 단위로 20 포인트 삭감.
			case 4	:	// 3개월 정액제 - 매 90 일 단위로 888 포인트 삭감.
			case 5	:	// 홍콩 월정액제
			case 6	:	// 홍콩 3개월 정액제
			case 7	:	// 홍콩 시간제
			case 8	:	// 홍콩 시간 정액제
			case 9	:	// 분 정량제 - 매 1분 단위로 0.25포인트 삭감.
			case 10	:	// 시간 정량제  - 매 2시간 단위로 12 포인트 삭감.
			case 11	:	// 10일 - 매 10일 단위로 150 포인트 삭감.
			case 12	:	// 무료 체험
				return;
		}
	} 
	
	g_pListACCOUNT->Gums_KickOutUSER( strPlayerID, uReloginResult );
#endif
	//processCommand();
}
//-------------------------------------------------------------------------------------------------
void CBS_Taiwan::LogoutResult(char *strPlayerID, unsigned int uIdentity, unsigned int uLogoutResult)
{
	// uIdentity == LSID
	g_LOG.CS_ODS(0xffff, "logout result ID:%s ,Identity:%d ,Result:%d\n" ,strPlayerID, uIdentity,uLogoutResult);

#ifndef	__SHO_LS_LIB
	// 할거 없자나...
	;
	;
	;
#endif
	//processCommand();
}
//-------------------------------------------------------------------------------------------------
void CBS_Taiwan::BillingMessage(char *strPlayerID, char *strBillingMessage, unsigned int uIdentity)
{
	g_LOG.CS_ODS(0xffff, " %s\n" , strBillingMessage);

#ifndef	__SHO_LS_LIB
	// 클라이언트에 전송
	g_pListACCOUNT->Gums_Message( BILLING_MSG_TYPE_TAIWAN, strPlayerID, strBillingMessage );
#endif
}
//-------------------------------------------------------------------------------------------------
void CBS_Taiwan::EnforceLogout(int iPlayerNo, char *strPlayerID)
{
	// iPlayerNo 무시해도 됨 !!!
	g_LOG.CS_ODS(0xffff, "enforce logout Player NO:%d ,ID:%s", &iPlayerNo, strPlayerID);

	CBS_Taiwan::Instance()->m_pLoginClient->logoutToServer( strPlayerID,1 );

#ifndef	__SHO_LS_LIB
	// 과금 시간 만료...
	g_pListACCOUNT->Gums_KickOutUSER( strPlayerID, BILLING_MSG_KICKOUT_TAIWAN );
#endif
	//processCommand();
}
//-------------------------------------------------------------------------------------------------
/*
void processCommand()
{
	char login_str[] = "login";
	char relogin_str[] = "relogin";
	char logout_str[] = "logout";
	char quit_str[] = "quit";

	char *command;
	command = new char[255];
    printf("enter command(login,relogin,logout,quit):");
	scanf("%s",command);
    if( strcmp(quit_str,command)==0)
		quitflag = true;
    else if(strcmp(login_str,command)==0)
		login();
	else if(strcmp(relogin_str,command)==0)
		relogin();
	else if(strcmp(logout_str,command)==0)
		logout();
	else
	{
	    printf("error command\n");
        processCommand();
	}
}
*/
//-------------------------------------------------------------------------------------------------


