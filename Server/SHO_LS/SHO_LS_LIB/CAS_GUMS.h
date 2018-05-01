#ifndef	__CAS_GUMS_H
#define	__CAS_GUMS_H
#include "CClientSOCKET.h"
#include "CSocketWND.h"
#include "classSYNCOBJ.h"
#include "classTIME.h"
//-------------------------------------------------------------------------------------------------

enum eGumsNATION {
	GUMS_KOREA = 0,
	GUMS_JAPAN
} ;



#define	GUMS001_REQ_INITIALIZED								1
#define	GUMS002_REQ_SERVER_KEEP_ALIVE						2
#define	GUMS010_REQ_LOGIN									10
#define	GUMS020_REQ_LOGOUT									20
#define	GUMS030_REQ_USER_KEEP_ALIVE							30
#define	GUMS040_REQ_REMAIN_TIME								40
#define	GUMS110_REQ_PRELOGIN_TYPE							110

// Response Message type 1 
#define	GUMS011_REPLY_LOGIN_SUCCESS							11
#define	GUMS111_REPLY_FREE_LOGIN_SUCCESS					111		// 본섭/프리섭 로그인 가능

// Response Message type 1 - JAPAN
#define	GUMS1011_REPLY_LOGIN_SUCCESS						1011	// *
#define	GUMS1111_REPLY_FREE_LOGIN_SUCCESS					1111	// * 본섭/프리섭 로그인 가능



// Response Message type 2
#define	GUMS012_REPLY_LOGIN_FAILED_INVALID_ID_OR_PW			12		// 사용자의 ID나 PW가 틀린경우
#define	GUMS013_REPLY_LOGIN_FAILED_NO_RIGHT					13		// 게임할 권한이 없는 사용자
#define	GUMS014_REPLY_LOGIN_FAILED_TIME_EXPIRED				14		// 사용 기간 만료

#define	GUMS018_REPLY_LOGIN_FAILED_ONE_DAY_EXPIRED			18		// 일일 사용 기간이 만료된 사용자
#define	GUMS019_REPLY_LOGIN_FAILED_ONE_DAY_EXPIRED2			19		// 일일 사용 기간이 만료된 사용자

#define	GUMS015_REPLY_LOGIN_FAILED_ALREADY_LOGIN			15		// 이미 로그인 한 사용자라고 알려주면서 로그인이 안되는거구여..
#define	GUMS016_REPLY_LOGIN_FAILED_IP_FULL_USED				16		// 결제한 IP의 수량을 모두 사용중인 경우(겜방)
#define	GUMS116_REPLY_FREE_LOGIN_FAILED_IP_FULL_USED		116		// 결제한 IP의 수량을 모두 사용중인 경우(겜방)

#define	GUMS017_REPLY_LOGIN_FAILED_KICKED_USER				17		// 먼저 로그인 되어 있는 사용자를 로그아웃 시켰다고 알려주고 로그인이 안되는 겁니다.

#define	GUMS021_REPLY_LOGOUT_SUCCESS						21		// 사용자 로그아웃 성공
#define	GUMS022_REPLY_LOGOUT_FAILED							22		// 사용자 로그아웃 실패

#define	GUMS088_REPLY_DO_INIT_FIRST							88		// 컨텍션에 대한 초기화 실행이 되어있지 않아 정상적인 메시지 처리 불가
#define	GUMS099_REPLY_INVALID_MSG_FORMAT					99		// 메세지 형식이 틀렸을 경우

#define	GUMS044_REPLY_NETWORK_IS_NOT_GOOD					44		// 네트웍 사용상황이 좋지 않아 메시지 처리가 불가능.

#define	GUMS100_REPLY_INIT_SUCCESS							100		// 초기화 성공
#define	GUMS112_REPLY_FREE_SRV_ONLY							112		// 프리섭만 로긴 가능...

#define	GUMS1112_REPLY_FREE_SRV_ONLY						1112	// 본섭/프리섭 로그인 가능


#define	GUMS113_REPLY_LOGIN_FAILED							113		// 본섭/프리섭 모두 로긴 불가..

// Response Message type 3
#define	GUMS041_REPLY_EXPIRE_DAY							41		// 해당 사용자의 종료일자를 돌려준다
#define	GUMS042_REPLY_EXPIRE_DAY2							42		// 해당 사용자의 남은 시간을 돌려준다.

// Response Message type 3 - JAPAN
#define	GUMS1041_REPLY_EXPIRE_DAY							1041	// 해당 사용자의 종료일자를 돌려준다
#define	GUMS1042_REPLY_EXPIRE_DAY2							1042	// 해당 사용자의 남은 시간을 돌려준다.

// Callback Messages
#define	GUMS051_REPLY_DAY_EXPIRED_ALERT						51		// 사용시간(정액) 만료 경고 메시지
#define	GUMS052_REPLY_ONE_DAY_TIME_OVER						52		// 일일 사용시간 한도 초과 경고 메세지
#define	GUMS053_REPLY_ONE_DAY_TIME_OVER2					53		// 일일 사용시간 한도 초과 경고 메세지
#define	GUMS054_REPLY_TIME_EXPIRED_ALERT					54		// 사용시간(정량) 만료 경고 메세지

// Callback Messages - JAPAN
#define	GUMS1051_REPLY_DAY_EXPIRED_ALERT					1051	// * 사용시간(정액) 만료 경고 메시지
#define	GUMS1052_REPLY_ONE_DAY_TIME_OVER					1052	// * 일일 사용시간 한도 초과 경고 메세지
#define	GUMS1053_REPLY_ONE_DAY_TIME_OVER2					1053	// * 일일 사용시간 한도 초과 경고 메세지
#define	GUMS1054_REPLY_TIME_EXPIRED_ALERT					1054	// * 사용시간(정량) 만료 경고 메세지


#define	GUMS061_REPLY_DAY_EXPIRED_DIS						61		// 사용기간(정액) 만료 접속 해제 메세지
#define	GUMS062_REPLY_ONE_DAY_EXPIRED_DIS					62		// 일일 사용시간 한도 초과 접속 해제 메시지
#define	GUMS063_REPLY_ONE_DAY_EXPIRED_DIS2					63		// 일일 한도시간 한도 초과 접속 해제 메시지
#define	GUMS064_REPLY_DUP_USERID_DIS						64		// 다른 사용자의 요구로 접속 해제
#define	GUMS065_REPLY_USER_NOT_FOUND						65		// 사용자 Live 신호가 왔을때 사용자가 메모리 내에 없을 경우 보내주는 메시지
#define	GUMS066_REPLY_TIME_EXPIRED_DIS						66		// 사용시간(정량) 만료 접속 해제 메시지


#define	GUMS1061_REPLY_DAY_EXPIRED_DIS						1061	// * 사용기간(정액) 만료 접속 해제 메세지
#define	GUMS1062_REPLY_ONE_DAY_EXPIRED_DIS					1062	// * 일일 사용시간 한도 초과 접속 해제 메시지
#define	GUMS1063_REPLY_ONE_DAY_EXPIRED_DIS2					1063	// * 일일 한도시간 한도 초과 접속 해제 메시지
#define	GUMS1064_REPLY_DUP_USERID_DIS						1064	// * 다른 사용자의 요구로 접속 해제
#define	GUMS1065_REPLY_USER_NOT_FOUND						1065	// * 사용자 Live 신호가 왔을때 사용자가 메모리 내에 없을 경우 보내주는 메시지
#define	GUMS1066_REPLY_TIME_EXPIRED_DIS						1066	// * 사용시간(정량) 만료 접속 해제 메시지



class ASGUM_Socket : public CClientSOCKET {
public :
	bool WndPROC ( WPARAM wParam, LPARAM lParam );
} ;

class AS_gumSOCKET 
{
public :
	ASGUM_Socket	m_SockGUM;

private:
	CTimer		*m_pReconnectTimer;
	CTimer		*m_pSendAliveTimer;

	bool		 m_bTryCONN;
	bool		 m_bGumsINIT;

	t_PACKET	*m_pRecvPket;

	HWND		 m_hMainWND;
	UINT		 m_uiSocketMSG;
	CStrVAR		 m_GumServerIP;
	int			 m_GumServerPORT;

	// iGameID	:: GUMS서버에서 구분하는 Gamd ID	무조건 1	Gum 서버 1대에 여러개인 서비스 할때...
	// 로즈에선...본섭은 1, 하루 일정시간 즐길수 있는 무료섭은 2 !!!
	// iCpID	:: 회원 관리가 분리된 경우 업체 ID	무조건 1
//	int			 m_iGameID;
	int			 m_iCpID;

	long		 m_lGumsSqlNO;
	//char		 m_szGumHeader[ 32 ];
	//CCriticalSection	m_csHEAD;
	//	char		 m_szGumBody[ 512 ];
//	CCriticalSection	m_csBODY;

	DWORD	GetPlayingFLAG( eGumsNATION eNation, int iPayType );

	int		GetPayType( char *szPayType );
	DWORD	GetPayFlag( char *szPayType );

	eGumsNATION	Gums_NATION( int iMsgType )
	{
		if ( iMsgType > 1000 ) {
			return GUMS_JAPAN;
		}
		return GUMS_KOREA;
	}

	CCriticalSection	 m_csLOG;
	FILE				*m_fpLOG;
	void Log( bool bRecv, char *szLog );

	static AS_gumSOCKET *m_pInstance;

public :
	AS_gumSOCKET ();
	~AS_gumSOCKET ();

	static AS_gumSOCKET *GetInstance ()		{	return m_pInstance;		}

	void Init (HWND hWND, char *szGumServerIP, int iGumServerPort, UINT uiSocketMSG);

	bool Connect ();
	void Disconnect ();

	bool Send_GumsPACKET (int iMsgType, char *szString, int iSeqNO=0, bool bCheckInit=true);
	void Recv_GumsPACKET (char *szString, int iStrLen);
	bool Proc_SocketMSG (WPARAM wParam, LPARAM lParam);


	bool Send_IntializedMessage (char *szServerName, int iServerGroupNumber, int iServerNumber, int iZoneNumber, int iGumsKeepAlive, int iUserKeepAlive );
	bool Send_ServerKeepAliveMessage ();
	bool Send_UserKeepAliveMessage (int iGameID, char *szUserID);
	bool Send_RequestReaminTime (int iGameID, char *szUserID);

	bool Send_PreLogin (int iGameID, char *szUserID, char *szRealIP, char *szVirtualIP=NULL, int iUserTAG=0 );

	bool Send_RequestLOGIN (bool bKickAlreadyLogin, int iGameID, char *szUserID, char *szPassword, char *szRealIP, char *szVirtualIP=NULL);
	bool Send_RequestLOGOUT (int iGameID, char *szUserID);


	bool RecvGums_LoginSUC (eGumsNATION eNation, char *szAccount, char *szGumsPKET, char *szUserTAG, int iPayType);
	bool RecvGums_LogInFailedNoRIGHT (int iMsgTYPE, char *szAccount, char *szUserTAG);

	bool RecvGums_LoginOnlyFREE(int iMsgTYPE, char *szAccount, char *szUserTAG);
	bool RecvGums_LogInFailedOutOfIP (int iMsgTYPE, char *szAccount, char *szUserTAG);

#ifndef	__SHO_LS_LIB
	bool RecvGums_TimeOUT  (int iMsgTYPE, char *szAccount);

	bool RecvGums_PreLoginFAILED (int iMsgTYPE, char *szAccount);

	bool RecvGums_LogInFailedKickedUSER (int iMsgTYPE, char *szUserID);
	bool RecvGums_LogInFailedTimeEXPIRED (int iMsgTYPE, char *szAccount);
	bool RecvGums_LogInFailedOneDayEXPIRED (int iMsgTYPE, char *szAccount);
	bool RecvGums_LogInFailedAlreadyLOGIN (int iMsgTYPE, char *szAccount);
	bool RecvGums_LogInFailedTimeEXPIRED (int iMsgTYPE, char *szAccount, char *szString );

	bool RecvGums_TimeALERT(int iMsgTYPE, char *szAccount, char *szRemainMin);
	bool RecvGums_TimeALERT_EXT( int iMsgTYPE, char *szAccount, char *szRemainMin, DWORD dwWarnPlayFlag );

	bool RecvGums_TimeEXPIRED (int iMsgTYPE, char *szAccount, char *szString );
	bool RecvGums_TimeEXPIRED_EXT (int iMsgTYPE, char *szUserID, DWORD dwDelPlayFlag );

	bool RecvGums_DupUserID (int iMsgTYPE, char *szAccount);

	bool RecvGums_SetExpireDATEorTIME( int iMsgTYPE, char *szAccount, char *szMsg );
	bool RecvGums_SetExpireDATEorTIME_EXT( int iMsgTYPE, char *szAccount, char *szMsg );
#endif
} ;

//-------------------------------------------------------------------------------------------------
#endif