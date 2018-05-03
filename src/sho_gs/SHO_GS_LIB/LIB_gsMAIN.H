#ifndef	__LIB_GAMEMAIN_H
#define	__LIB_GAMEMAIN_H
#include "CGS_API.h"
//-------------------------------------------------------------------------------------------------

#ifndef	_WINDOWS_
	#define	_WIN32_WINNT	0x0500
#endif

#if !defined(_WIN32_WINNT) || ( _WIN32_WINNT < 0x0500 )
	#error >>> ERROR :: _WIN32_WINNT >= 0x0500 !!!
#endif

#define	__SHO_GS

//  #define	__INC_WORLD
//	#define	__USE_TRACE
#define ENABLE_CHATROOM		// Enables the chatroom functions
//#define FRAROSE

#ifdef FRAROSE
	#define __KCHS_BATTLECART__
#endif

//#define	__PHILIPPINES			// 필리핀 전용 서버
//#define __N_PROTECT				// nProtect 용

#include "LIB_Util.h"

#include <windows.h>
#include <wchar.h>
#include <crtdbg.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include "md5.h"
#include "CVector.h"
#include "DataTYPE.h"
#include "NET_Prototype.h"
#include "DLLIST.h"
#include "SLLIST.h"
#include "classLOG.h"
#include "classSTR.h"
#include "classUTIL.h"
#include "classHASH.h"
#include "classHTTP.h"
#include "classINDEX.h"
#include "classIOCP.h"
#include "classTIME.h"
#include "classTRACE.h"
#include "classSYNCOBJ.h"
#include "CClientSOCKET.h"

#include "ETC_Math.h"

#include "IO_STB.h"
#include "OBJECT.h"

//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \file	LIB_gsMAIN.h
 * \class	CLIB_GameSRV
 * \author	wookSang.Jo
 * \brief	gs 전역 싱글톤 클래스
 */
class CLIB_GameSRV {
private  :
	static CLIB_GameSRV	*m_pInstance;
	EXE_GS_API *m_pExeAPI;

	HWND		m_hMainWND;				///< 윈도우 핸들
	CTimer	   *m_pWorldTIMER;			///< 월드 타이머
	bool	   *m_pCheckedLocalZONE;

	CStrVAR	m_BaseDataDIR;				///< 게임 데이타 폴더
	CStrVAR	m_ServerNAME;				///< 설정된 서버 이름
	CStrVAR m_ServerIP;					///< 서버 IP
	int		m_iListenPortNO;			///< 사용자 접속 포트번호

	CStrVAR	m_DBServerIP;				///< 디비 서버 IP
	CStrVAR	m_DBName;					///< 디비 네임
	CStrVAR	m_DBUser;					///< 디비 사용자
	CStrVAR	m_DBPassword;				///< 디비 사용자 비밀번호
	CStrVAR	m_LogUser;
	CStrVAR	m_LogPW;

	CStrVAR m_AccountServerIP;			///< sho_as에 접속할 IP
	int		m_iAccountServerPORT;		///< sho_as에 접속할 port no

	CStrVAR m_LoginServerIP;			///< sho_ws에 접속할 IP
	int		m_iLoginServerPORT;			///< sho_ws에 접속할 port no

	DWORD	m_dwRandomSEED;

	BYTE	m_btChannelNO;				///< 동작중인 채널 번호
	BYTE	m_btLowAGE;					///< 연령 접속 제한 최저 나이
	BYTE	m_btHighAGE;				///< 연령 접속 제한 최고 나이
	DWORD	m_dwUserLIMIT;				///< 최대 접속 가능한 인원수

	DWORD	m_dwMaxLevel;
	DWORD	m_dwMaxStats;

	int		m_iLangTYPE;				///< 국가 코드
	bool	m_bTestServer;				///< 테스트 서버인가 ?

	CLIB_GameSRV ( EXE_GS_API *pExeAPI );
	virtual ~CLIB_GameSRV ();
	void SystemINIT( HINSTANCE hInstance, char *szBaseDataDIR, int iLangType );


	bool CheckSTB_UseITEM ();
	bool CheckSTB_NPC ();
	bool CheckSTB_DropITEM ();
	bool CheckSTB_ItemRateTYPE ();
	bool CheckSTB_Motion ();
	bool CheckSTB_GemITEM ();
	bool CheckSTB_ListPRODUCT ();

	bool Load_BasicDATA ();
	void Free_BasicDATA ();

	bool ConnectToLSV ();
	void DisconnectFromLSV ();

	bool ConnectToASV ();
	void DisconnectFromASV ();

	bool ConnectToLOG ();
	void DisconnectFromLOG ();

	void TranslateNameWithDescKey	( STBDATA *pOri, char *szStbFile, int iNameCol );
	void TranslateNameWithColoumKey ( STBDATA *pOri, char *szStbFile, int iLangCol, int iNameCol, int iDescCol );

	friend VOID CALLBACK GS_TimerProc ( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

public   :
	bool ConnectSERVER( char *szDBServerIP,		char *szDBName,
						char *szDBUser,			char *szDBPW,
						char *szLogUser,		char *szLogPW,
						char *szMallUser,		char *szMallPW,
						char *szLoginServerIP,	int iLoginServerPort,
						char *szLogServerIP,	int iLogServerPortNO );

	bool Start( HWND hMainWND, char *szServerName, char *szClientListenIP, int iClientListenPort, BYTE btChannelNO, BYTE btLowAge, BYTE btHighAge );
	void Shutdown ();

	short InitLocalZone( bool bAllActive );
	bool  CheckZoneToLocal(short nZoneNO, bool bChecked);
	char* GetZoneName( short nZoneNO );
	bool  IsLocalCheckedZONE( short nZoneNO )	{	return m_pCheckedLocalZONE[nZoneNO];	}	

	char* GetServerName()						{	return m_ServerNAME.Get();	}
	char* GetServerIP()							{	return m_ServerIP.Get();	}
	int	  GetListenPort()						{	return m_iListenPortNO;		}
	DWORD GetRandomSeed()						{	return m_dwRandomSEED;		}
	DWORD GetMaxStats()							{	return m_dwMaxStats;		}
	DWORD GetMaxLevel()							{	return m_dwMaxLevel;		}

	BYTE  GetChannelNO ()						{	return m_btChannelNO;		}
	BYTE  GetLowAGE ()							{	return m_btLowAGE;			}
	BYTE  GetHighAGE ()							{	return m_btHighAGE;			}
	bool  IsTestServer()						{	return m_bTestServer;		}

	int	  GetLangTYPE()							{	return m_iLangTYPE;			}

	void  Send_ANNOUNCE( short nZoneNO, char *szMsg );
	void  Set_UserLIMIT( DWORD dwUserLimit );
	DWORD Get_UserLIMIT()						{	return m_dwUserLIMIT;		}
	
	static EXE_GS_API *ExeAPI()					{	return m_pInstance->m_pExeAPI;	}
	static CLIB_GameSRV *GetInstance ()			{	return m_pInstance;	}
	static CLIB_GameSRV *InitInstance( HINSTANCE hInstance, char *szBaseDataDIR, EXE_GS_API *pExeAPI, int iLangType )
	{
		if ( NULL == m_pInstance ) {
			m_pInstance = new CLIB_GameSRV( pExeAPI );
			_ASSERT( m_pInstance );
			m_pInstance->SystemINIT( hInstance, szBaseDataDIR, iLangType );
		}
		return m_pInstance;
	}
	void Destroy ()								{	SAFE_DELETE( m_pInstance );	}
} ;

#define	GS_TIMER_LSV			1
#define	GS_TIMER_LOG			2
#define	GS_TIMER_WORLD_TIME		3
#define	GS_TIMER_ASV			4

#define	RECONNECT_TIME_TICK		10000	// 10 sec

extern VOID CALLBACK GS_TimerProc ( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

extern void Save_AllUSER ();

extern DWORD GetServerBuildNO ();
extern DWORD GetServerStartTIME ();

inline short Get_ServerChannelNO()
{
	return CLIB_GameSRV::GetInstance()->GetChannelNO();
}
inline int	 Get_ServerLangTYPE()
{
	return CLIB_GameSRV::GetInstance()->GetLangTYPE();
}

inline DWORD Get_ServerMaxStats()
{
	return CLIB_GameSRV::GetInstance()->GetMaxStats();
}
inline DWORD Get_ServerMaxLevel()
{
	return CLIB_GameSRV::GetInstance()->GetMaxLevel();
}

//-------------------------------------------------------------------------------------------------
#endif
