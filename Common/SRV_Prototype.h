#ifndef	__SRV_PROTOTYPE_H
#define	__SRV_PROTOTYPE_H
//-------------------------------------------------------------------------------------------------

#define	MAX_PARTY_BUFF			8192	//16384
#define	MAX_PARTY_MEMBERS		7
#define	MAX_PARTY_LEVEL			50

// 월드 서버 최대 접속 가능자
#define	MAX_WORLD_USER_UBFF			65535
// 존 서버 최대 접속 가능자.
#define	MAX_ZONE_USER_BUFF			8192

#define	MAX_CHANNEL_SERVER			(16+1)	// 0번 채널 제외


#define	MAX_CLAN_LEVEL		7
enum {
	GPOS_PENALTY	= 0,	// 길드챗 금지
	GPOS_JUNIOR		= 1,	// "junior"
	GPOS_SENIOR		= 2,	// "senior"
	GPOS_RENDER		= 3,	// "veteran"
	GPOS_COMMANDER	= 4,	// "commander"
	GPOS_SUB_MASTER	= 5,	// "deputy-master"
	GPOS_MASTER		= 6		// "master"
} ;

//-------------------------------------------------------------------------------------------------
					
//-------------------------------------------------------------------------------------------------
// #define	__SKIP_WS_PARTY

#define	MON_SERVER_LIST_REQ			0x01f0
#define	WLS_SERVER_LIST_REPLY		0x01f0
#define	GSV_SERVER_LIST_REPLY		0x01f1

#define	MON_SERVER_STATUS_REQ		0x01f2
#define	WLS_SERVER_STATUS_REPLY		0x01f2
#define	GSV_SERVER_STATUS_REPLY		0x01f3

#define	MON_SERVER_ANNOUNCE			0x01f4

/// 온라인 툴 - 게임서버 간 패킷
#define OST_SERVER_USERLOGOUT		0x01f5	// 유저 강제로그아웃
#define GSV_SERVER_USERLOGOUT_REPLY	0x01f5

#define OST_SERVER_ZONEINFO			0x01f6	// 지역별 정보. 존번호, 존이름, 유저수
#define GSV_SERVER_ZONEINFO_REPLY	0x01f6

#define OST_SERVER_USERINFO			0x01f7	// 유저정보 , 현재 좌표, IP
#define GSV_SERVER_USERINFO_REPLY	0x01f7	

#define OST_SERVER_CHGUSER			0x01f8	// 유저의 상태세팅 ( 현재는 채팅 금지 )
#define GSV_SERVER_CHGUSER_REPLY	0x01f8

#define OST_SERVER_MOVECHR			0x01f9	// 캐릭터를 이동시킴
#define GSV_SERVER_MOVECHR_REPLY	0x01f9

#define OST_SERVER_ZONEANNOUNCE		0x01fa	// 존별 공지
#define GSV_SERVER_ZONEANNOUNCE_REPLY	0x01fa

#define OST_SERVER_IPSEARCH			0x01fb	// IP로 검색
#define GSV_SERVER_IPSEARCH_REPLY	0x01fb
/// ----------------------------------------------------
/// < Admin Tool >	- Raven
/// ----------------------------------------------------
#define	BKD_SERVER_MAKEACCO			0x01fc	// Makes a new user account
#define	BKD_SERVER_MAKEACCO_REPLY	0x01fc	// Replys if the account was created 

#define BKD_SERVER_MODACCO			0x01fd	// Modifys an account
#define BKD_SERVER_LISTACCO			0x01fe	// Lists Accounts on the server
/// ----------------------------------------------------
/// </ Admin Tool >
/// ----------------------------------------------------

#define	LSV_CHECK_ALIVE				0x0200
#define GSV_CHECK_ALIVE             0x0200

#define	ZWS_SERVER_INFO				0x0201

#define	GSV_ADD_ZONE_LIST			0x0202
#define	GSV_DEL_ZONE				0x0203

#define	WSV_DEL_ZONE				0x0204
#define	GSV_WARP_USER				0x0205		// 월드 서버에 사용자 워프 한다고 통보...

#define	GSV_CHEAT_REQ				0x0206		// GSV->WSV
#define	WSV_CHEAT_REQ				0x0207		// WSV->GSV

#define	GSV_CHANGE_CHAR				0x0208

#define	LSV_ANNOUNCE_CHAT			0x0210

#define	ZWS_CONFIRM_ACCOUNT_REQ		0x0211
#define	WLS_CONFIRM_ACCOUNT_REPLY	0x0211

#define	ZWS_ADD_ACCOUNT				0x0212
#define	ZWS_SUB_ACCOUNT				0x0213

#define	ZAS_SUB_ACCOUNT				ZWS_SUB_ACCOUNT

#define	WLS_KICK_ACCOUNT			0x0214		// 중복 접속이나 해킹...

#define	GSV_BLOCK					0x0215

#define	SRV_ACTIVE_MODE				0x0216

#define	WLS_CHANNEL_LIST			0x0217

#define	WLS_ACCOUNT_LIST			0x0218
#define	ZWS_ACCOUNT_LIST			0x0218

#define	SRV_USER_LIMIT				0x0219

#define	ZAS_SERVER_TYPE				0x021a
#define	ZAS_MESSAGE					0x021b
#define	ZAS_KICK_ACCOUNT			0x021c
#define	ZAS_CHECK_ACCOUNT			0x021d

#define	ZAS_MESSAGE_EXT				0x021e

#define	GSV_LOG_DB_INFO				0x0220
#define	GSV_LOG_DB_SQL				0x0221

#define	ZWS_CREATE_CLAN				0x0230		// 클랜 생성
#define	ZWS_SET_USER_CLAN			0x0231		// 사용자 클랜 설정.
#define	ZWS_DEL_USER_CLAN			0x0232		// 사용자 클랜 삭제.

#define	GSV_ADJ_CLAN_VAR			0x0233
#define	WSV_SET_CLAN_VAR			0x0233

/*
#define GSV_LEVEL_UP				0x0200
#define	WSV_PARTY_CMD				0x0201
*/

#define	GSV_SET_WORLD_VAR			0x02fa		// 월드 변수 설정
#define	WSV_SET_WORLD_VAR			0x02fa

/*
#define	GSV_DO_TRIGGER_NCPOBJ		0x02fb		// 트리거 실행 명령
#define	WSV_DO_TRIGGER_NPCOBJ		0x02fb

#define	GSV_DO_TRIGGER_EVENTOBJ		0x02fc
#define	WSV_DO_TRIGGER_EVENTOBJ		0x02fc

#define	GSV_SET_NPCOBJ_VAR			0x02fd		// 변수 설정
#define	WSV_SET_NPCOBJ_VAR			0x02fd

#define	GSV_SET_EVENTOBJ_VAR		0x02fe
#define	WSV_SET_EVENTOBJ_VAR		0x02fe
*/

#define	SQL_ZONE_DATA				0x02ff		// DB에서 존 데이타 I/O..


//-------------------------------------------------------------------------------------------------
/*
struct tag_SERVER_INFO {
	// WORD	 m_wPortNO;
	// DWORD m_dwRandomSEED;
	// szServerIP[]
	// szServerNAME[]
} ;
struct tag_ZONE_INFO {
	// short nZoneNo
	// szZoneName[]
} ;
*/

struct bkd_MAKE_ACCOUNT : public t_PACKETHEADER {
	WORD	m_wRight;
	union {
		BYTE	m_MD5Password[ 32 ];
		DWORD	m_dwMD5[ 8 ];
	} ;
} ;

struct bkd_MOD_ACCOUNT : public t_PACKETHEADER {
	WORD	m_wRight;
	union {
		BYTE	m_MD5Password[ 32 ];
		DWORD	m_dwMD5[ 8 ];
	} ;
} ;

struct lsv_SERVER_MAKE_ACCOUNT_REPLY : public t_PACKETHEADER {
	DWORD	m_dwTIME;
} ;

struct srv_SERVER_LIST_REPLY : public t_PACKETHEADER {
	short	m_nServerCNT;
	DWORD	m_dwStartTIME;
	DWORD	m_dwBuildNO;
} ;

struct mon_SERVER_STATUS_REQ : public t_PACKETHEADER {
	DWORD	m_dwTIME;
} ;

// 로그인/월드 서버 정보
struct wls_SERVER_STATUS_REPLY : public t_PACKETHEADER {
	DWORD	m_dwTIME;
	short	m_nServerCNT;
	int		m_iUserCNT;
} ;

// 채널 서버 정보
struct tag_ZONE_STATUS {
	short	m_nZoneNO;
	short	m_nUserCNT;
} ;
struct gsv_SERVER_STATUS_REPLY : public t_PACKETHEADER {
	DWORD			m_dwTIME;
	short			m_nZoneCNT;
	tag_ZONE_STATUS	m_STATUS[1];
} ;


//-------------------------------------------------------------------------------------------------
struct gsv_LOG_DB_INFO : public t_PACKETHEADER {
	// char *db_ip
	// char *db_name
	// char *db_account
	// char *db_password
} ;
struct gsv_LOG_SQL : public t_PACKETHEADER {
	union {
		char	m_cQUERY[1];
		BYTE	m_btQUERY[1];
	} ;
} ;

#define	CLI_STRESS_TEST				0x040ff

// CLI->SRV
struct cli_STRESS_TEST : public t_PACKETHEADER {
	DWORD	m_dwPacketNO;
} ;


// LS->GS
struct lsv_CHECK_GSV : public t_PACKETHEADER {
} ;

// GS->WS / WS->LS
struct zws_SERVER_INFO : public t_PACKETHEADER {
	WORD			m_wPortNO;
	DWORD			m_dwSeed;
	tagCHANNEL_SRV	m_Channel;
	// szServerName[]
	// szServerIP[]
} ;


// WS->LS
struct srv_ACTIVE_MODE : public t_PACKETHEADER {
	bool	m_bActive;
} ;

struct srv_USER_LIMIT : public t_PACKETHEADER {
	DWORD	m_dwUserLIMIT;
} ;


struct wls_CHANNEL_LIST : public t_PACKETHEADER {
	BYTE	m_btChannelCNT;
	/*
	tagCHANNEL_SRV[]
	*/
} ;


struct tag_WLS_ACCOUNT {
	DWORD	m_dwLSID;
	DWORD	m_dwLoginTIME;
	char	m_szAccount[0];
} ;
struct wls_ACCOUNT_LIST: public t_PACKETHEADER {
	short	m_nAccountCNT;
	/*
		tag_WLS_ACCOUNT[ m_nAccountCNT ];
	*/
} ;
struct tag_ZWS_ACCOUNT {
	DWORD	m_dwLSID;
	DWORD	m_dwGSID;
	DWORD	m_dwLoginTIME;
	char	m_szAccount[0];
	//char	m_szChar[0];
} ;
struct zws_ACCOUNT_LIST : public t_PACKETHEADER {
	short	m_nAccountCNT;
	/*
		tag_ZWS_ACCOUNT[ m_nAccountCNT ];
	*/
} ;


struct zas_SERVER_TYPE : public t_PACKETHEADER {
	BYTE	m_btServerTYPE;
} ;


// @btTYPE은 gsv_BILLING_MESSAGE과 동일...
struct zas_MESSAGE : public t_PACKETHEADER {
	BYTE	m_btTYPE;
	// char m_szAccount[]
	// char m_szMsg[]
} ;


// gsv_BILLING_MESSAGE_EXT와 같으나 gsv_BILLING_MESSAGE_EXT의 szMsg앞에 szAccount가 온다
struct zas_MESSAGE_EXT : public gsv_BILLING_MESSAGE_EXT {
	// char m_szAccount[]
	// char m_szMsg[]
} ;


struct zas_KICK_ACCOUNT : public t_PACKETHEADER {
	BYTE	m_btWHY;
} ;

#define	ZAS_CHECK_DISCONNECT	0x01		// GUMS에서 timeout 걸렸다... 채널 섭에서 접속여부 체크..
#define	ZAS_CHECK_RELOGIN		0x02
struct zas_CHECK_ACCOUNT : public t_PACKETHEADER {
	BYTE	m_btType;
	// char m_szAccount[];
} ;


// GS->WS / WS->LS
struct zws_CONFIRM_ACCOUNT_REQ : public t_PACKETHEADER {
	DWORD	m_dwServerID;
	DWORD	m_dwClientID;
    DWORD	m_dwMD5Password[ 8 ];		// 32 bytes
} ;	



struct tagWSPartyINFO {
	char	m_cMemberCNT;
	BYTE	m_btPartyLEV;
	int		m_iPartyEXP;
} ;
// WS->GS / LS->WS
struct wls_CONFIRM_ACCOUNT_REPLY : public t_PACKETHEADER {
	BYTE		m_btResult;
	DWORD		m_dwLSID;
	DWORD		m_dwWSID;
	DWORD		m_dwGSID;
	DWORD		m_dwLoginTIME;
	DWORD		m_dwRIGHT;
	DWORD		m_dwPayFLAG;

	BYTE		m_btChannelNO;
	// szAccount[]
} ;
#define	RESULT_CONFIRM_ACCOUNT_OK					RESULT_JOIN_SERVER_OK
#define	RESULT_CONFIRM_ACCOUNT_FAILED				RESULT_JOIN_SERVER_FAILED
#define	RESULT_CONFIRM_ACCOUNT_TIME_OUT				RESULT_JOIN_SERVER_TIME_OUT		
#define	RESULT_CONFIRM_ACCOUNT_INVALID_PASSWORD		RESULT_JOIN_SERVER_INVALID_PASSWORD
#define	RESULT_CONFIRM_ACCOUNT_ALREADY_LOGGEDIN		RESULT_JOIN_SERVER_ALREADY_LOGGEDIN

// GS->WS / WS->LS
struct zws_ADD_ACCOUNT : public t_PACKETHEADER {
	// szAccount[]
} ;
struct zws_SUB_ACCOUNT : public t_PACKETHEADER {
	// szAccount[]
} ;

struct zas_SUB_ACCOUNT : public t_PACKETHEADER {
	BYTE	m_btWhy;
	short	m_nLevel;
	short	m_nZoneNo;
	// szAccount[]
} ;

// LS->WS / WS->GS
struct wls_KICK_ACCOUNT : public t_PACKETHEADER {
	// char  szAccount[]
} ;

// GSV->WSV / WSV->GSV
struct srv_CHEAT : public t_PACKETHEADER {
	DWORD		m_dwReqUSER;
	DWORD		m_dwReplyUSER;
	short		m_nZoneNO;
	tPOINTF		m_PosCUR;
//	char		m_szCheatCODE[1];
} ;

// GS->LS
struct gsv_BLOCK : public t_PACKETHEADER {
	BYTE	m_btTargetTYPE;
	WORD	m_wAttackTYPE;
	/*
		char szIP[]
			or 
		char szACCOUNT[]
	*/
} ;

// LS->GS
struct lsv_CHECK_ACCOUNT : public t_PACKETHEADER {
	// szAccount[]
} ;


/*
struct gsv_ADD_ZONE_LIST : public t_PACKETHEADER {
	short	m_nZoneCNT;
	short	m_nZoneIDX[0];
} ;
struct gsv_DEL_ZONE : public t_PACKETHEADER {
	short	m_nZoneIDX;
} ;

struct wsv_DEL_ZONE	: public t_PACKETHEADER {
	short	m_nZoneIDX;
} ;
*/

/*
	존 워프 처리 순서..
	1. CLI => GSV 워프 요청
	2. 로컬 / 원격 판단
	   a. 로컬일 경우 
		  . send_gsv_teleport_reply 전송 
		  . 끝
	   b. 원격일 경우
	      . DB에 케릭터 기록 
		  . WSV에 워프 전송
		  . WSV => CLI에 move_server 전송
		  . CLI => GSV 접속
		  . GSV => send_join_zone 전송.
		  . 끝.
*/
/*
// 워프시 유지할 값들...
struct gsv_WARP_USER : public t_PACKETHEADER {
	DWORD		m_dwWSID;
	short		m_nZoneNO;
	BYTE		m_btRunMODE;
	BYTE		m_btRideMODE;
	union {
		DWORD	m_dwIngStatusFLAG;
		BYTE	m_IngSTATUS[1];			// 진행 상태값이 있을경우... 즉... 0 != m_dwIngStatusFLAG
	} ;
} ;
*/

//-------------------------------------------------------------------------------------------------
struct gsv_CHANGE_CHAR : public t_PACKETHEADER {
	/*
	DWORD		m_dwWSID;
	t_HASHKEY	m_HashACCOUNT;
	*/
	char	m_szAccount[0];
} ;


//-------------------------------------------------------------------------------------------------
struct tag_ZWS_CLAN_PACKET : public t_PACKETHEADER {
	DWORD		m_dwGSID;
	t_HASHKEY	m_HashCHAR;
} ;
struct zws_CREATE_CLAN : public tag_ZWS_CLAN_PACKET {
} ;
struct zws_DEL_USER_CLAN : public tag_ZWS_CLAN_PACKET {
} ;
struct zws_SET_USER_CLAN : public tag_ZWS_CLAN_PACKET {
	tag_MY_CLAN m_ClanINFO;
//	char		m_szClanNAME[0];
} ;


// 클랜 명령 패킷과 함께 사용됨....
#define	CLVAR_INC_LEV			0x01
#define	CLVAR_ADD_SCORE			0x02
#define	CLVAR_ADD_ZULY			0x03
#define	CLVAR_ADD_SKILL			0x04
#define	CLVAR_DEL_SKILL			0x05

struct gsv_ADJ_CLAN_VAR : public t_PACKETHEADER {
	BYTE	m_btCMD;
	DWORD	m_dwClanID;
	BYTE	m_btVarType;
	union {
		int		m_iAdjValue;
		__int64	m_biResult;
	} ;
} ;
struct wsv_SET_CLAN_VAR : public t_PACKETHEADER {
	BYTE	m_btVarType;
	union {
		BYTE	m_ClanBIN[1];
		int		m_iClanLEV;
		int		m_iClanSCORE;
		__int64	m_biClanMONEY;
		int		m_iUserCONT;
	} ;
} ;


/*
//-------------------------------------------------------------------------------------------------
struct gsv_LEVEL_UP : public t_PACKETHEADER {
	BYTE		m_btOP;
	WORD		m_wWSID;
	short		m_nLevel;
	int			m_iEXP;
} ;
#define	LEVELUP_OP_USER			0x01
#define LEVELUP_OP_PARTY		0x02

struct wsv_PARTY_CMD : public t_PACKETHEADER {
	BYTE		m_btOP;
	WORD		m_wPartyWSID;
	union {
		WORD	m_wGSIDs[ 2 ];			// 파티 생성시.. 만든넘, 대상
		struct {
			WORD	m_wGSID;
			short	m_nAvgLEV;
		} ;
	} ;
} ;
#define PARTY_OP_CREATE			0x01	// 파티 생성
#define PARTY_OP_DESTROY		0x02	// 파티 뽀개라~
#define	PARTY_OP_JOIN			0x03	// 파티원이 참가 했다.
#define	PARTY_OP_LEFT			0x04	// 파티원이 나갔다.
#define	PARTY_OP_USER_LEVELUP	0x05	// 파티원이 렙업했다.
*/

//-------------------------------------------------------------------------------------------------
struct srv_SET_WORLD_VAR : public t_PACKETHEADER {
	short		m_nVarIDX;			// 음수일경우 전체
	short		m_nValue[1];
} ;

/*
//-------------------------------------------------------------------------------------------------
struct srv_DO_TRIGGER_NCPOBJ : public t_PACKETHEADER {
	short		m_nNpcIDX;
	t_HASHKEY	m_HashTRIGGER;
} ;

struct srv_DO_TRIGGER_EVENTOBJ : public t_PACKETHEADER {
	t_HASHKEY	m_HashNAME;
	t_HASHKEY	m_HashTRIGGER;
} ;

//-------------------------------------------------------------------------------------------------
struct srv_SET_NPCOBJ_VAR : public t_PACKETHEADER {
	short		m_nNpcIDX;			// 음수일 경우 전체 데이타
	// short	m_nVAR[ xxx ];
} ;

struct srv_SET_EVENTOBJ_VAR : public t_PACKETHEADER {
	t_HASHKEY	m_HashNAME;
	// short	m_nVAR[ xxx ];
} ;
*/

//-------------------------------------------------------------------------------------------------
#define	SQL_ZONE_DATA_ECONOMY_LOAD		0x00
#define	SQL_ZONE_DATA_ECONOMY_SAVE		0x01
#define	SQL_ZONE_DATA_NPCOBJ_LOAD		0x02
#define	SQL_ZONE_DATA_NPCOBJ_SAVE		0x03
#define	SQL_ZONE_DATA_EVENTOBJ_LOAD		0x04
#define	SQL_ZONE_DATA_EVENTOBJ_SAVE		0x05
#define	SQL_ZONE_DATA_WORLDVAR_SAVE		0x06
struct sql_ZONE_DATA : public t_PACKETHEADER {
	BYTE	m_btDataTYPE;
	short	m_nDataSIZE;
	BYTE	m_btZoneDATA[0];
} ;


//-------------------------------------------------------------------------------------------------
/// 유저 강제로그아웃
struct ost_SERVER_USERLOGOUT : public t_PACKETHEADER
{
	// char [] (계정명)
};
struct gsv_SERVER_USERLOGOUT_REPLY : public t_PACKETHEADER
{
	BYTE		m_btRESULT;	// 1 : 로그아웃 , 0 : 실패
	// char [] (캐릭명, NULL 포함)
	// char [] (계정명, NULL 포함)
};

struct ost_ZoneINFO
{
	short	m_nZoneNO;
	short	m_nUserCNT;
};
/// 지역별 정보. 존번호, 존이름, 유저수
struct ost_SERVER_ZONEINFO : public t_PACKETHEADER
{
};
struct gsv_SERVER_ZONEINFO_REPLY : public t_PACKETHEADER
{
	short			m_nZoneCOUNT;
	// ost_ZoneINFO	m_ZoneINFO[  ];
};

/// 유저정보 , 현재 좌표, IP
struct ost_SERVER_USERINFO : public t_PACKETHEADER
{
	// char [] ( 계정명 ) AppendString 으로 붙일 것
};
struct gsv_SERVER_USERINFO_REPLY : public t_PACKETHEADER
{
	BYTE			m_btEXIST;
	short			m_nZoneNO;
	short			m_nX;
	short			m_nY;
	unsigned long	m_ulIP;
	// char [] ( 캐릭터명 ) , char [] ( 계정명 )
};

#define		OST_CHGUSER_BLOCKCHAT			0x000000001		// 채팅금지
// 유저의 상태세팅 ( 현재는 채팅 금지 )
struct ost_SERVER_CHGUSER : public t_PACKETHEADER
{
	DWORD		m_dwSTATUS;		// 적용할 항목
	DWORD		m_dwCMD;		// 적용할 항목에 대한 On(1), Onff(0)
	// char [] ( 계정명 ) AppendString 으로 붙일 것
};
struct gsv_SERVER_CHGUSER_REPLY : public t_PACKETHEADER
{
	DWORD		m_dwSTATUS;		// 적용된 항목
	DWORD		m_dwCMD;		// 적용된 항목에 대한 상태 값
	// char [] ( 캐릭터명 ) , char [] ( 계정명 )
};

// 캐릭터를 이동시킴
struct ost_SERVER_MOVECHR : public t_PACKETHEADER
{
	short		m_nZoneNO;
	short		m_nX;
	short		m_nY;
	// 계정명
};

struct gsv_SERVER_MOVECHR_REPLY : public t_PACKETHEADER
{
	BYTE		m_btMOVED;	// 1 : 이동성공, 0 : 이동실패
	// 캐릭터명
	// 계정명
};

struct ost_SERVER_ZONEANNOUNCE : public t_PACKETHEADER
{
	short		m_nZoneNO;
	// 메시지
	// 보내는 사람 계정
};

struct gsv_SERVER_ZONEANNOUNCE_REPLY : public t_PACKETHEADER
{
	short		m_nZoneNO;
	// 메시지
};

// 아이피로 검색
struct ost_SERVER_IPSEARCH : public t_PACKETHEADER
{
	// 아이피 , AppendString 된 문자열. "211." 일 경우 211로 시작하는 모든 ip를 검색
};

struct gsv_SERVER_IPSEARCH_REPLY : public t_PACKETHEADER
{
	short		m_nCOUNT;	// 갯수
	// 캐릭명1, 계정명1, 캐릭명2, 계정명2, ...
};
//-------------------------------------------------------------------------------------------------

#endif