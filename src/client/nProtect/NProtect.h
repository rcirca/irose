/******************************************************************************
  class	CNProtectSys 

  desc:  nProtect 처리하는 클래스 

  Date:  2005. 05. 19일 

  작성자:  박 지호.

  설정파일:  GameGuard.des, RoseOnlinePH.ini 파일들을 게임 실행파일과 
             같은 디렉토리에 복사한다. 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/
#ifndef		__NPROTECT_SYS__
#define		__NPROTECT_SYS__

#define		MAX_BUF_LEN			200


//홍근 : 디버깅시 게임가드 실행 시키지 않는다.
#ifdef _DEBUG

#define	NO_GAMEGUARD

#endif



//nProtect 라이브러리 설정 
#include "NPGameLib.h"
#include "npkcrypt.h"

#pragma comment(lib,"../nProtect/NPGameLib.lib")
//#pragma comment(lib,"../nProtect/npkcrypt.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Winmm.lib")









class CNProtectSys
{

	BOOL	m_IsGameExit;				//게w임종료 유무 
	BOOL	m_IsCreate;					//protect 생성유무 
	BOOL	m_IsUse;					//protect 사용유무   

	DWORD	m_curTick,m_oldTick;		//검사 tick 카운터 
	DWORD	m_delayTick;
	
	GG_AUTH_DATA	m_AuthData;			//인증 구조체 

public:
	TCHAR	m_HackString[MAX_BUF_LEN];	//해킹 메세지 버퍼 


public:
	CNProtectSys();
	~CNProtectSys();

	void  Release(void);
	void  Permission_Country(void);
	BOOL  InitProtect(void);
	

	void  Set_hWnd(HWND hWnd);
	void  Set_UserID(const char* userID);

	void  Check_nProtectSafe(DWORD ckTick=0);
	void  Auth_FromServer(WORD nType,GG_AUTH_DATA* pAuthData=NULL);
	bool  _NPGameMonCallback(DWORD dwMsg,DWORD dwArg);	

	
	GG_AUTH_DATA GetAuthData(void)	{  return m_AuthData;		}
	BOOL& IsCreate(void)			{  return m_IsCreate;		}
	BOOL& IsGetUse(void)			{  return m_IsUse;			}
	BOOL  IsGameExit(void)			{  return m_IsGameExit;	}

	void  SetDelayTick(DWORD t)		{  m_delayTick = t;			}
	TCHAR* GetHacking_Msg(void)		{  return m_HackString;	}
 

};
 



bool CALLBACK NPGameMonCallback(DWORD dwMsg,DWORD dwArg);		//nProtec안에서 사용하는 윈도우 함수 
void GetWindowsErrorMsg(DWORD dwError);							



extern CNProtectSys		m_nProtectSys;		
extern CNPGameLib		npgl;				




//
//
///********************************************************************************
//
// Class CNProtectKC   : 키보드 암호화를 처리하는 클래스 
//
// 설정파일:   npkcrypt, npkcrypt.dll, npkcrypt.vxd, npkcusb, npkpdb.dll
//             파일들을 게임 실행파일과 같은 디렉토리에 복사한다. 
//********************************************************************************/
//class CNProtectKC
//{
//
//	BOOL m_IsCreate;
//	BOOL m_IsUse;
//
//	HKCRYPT	m_hKCrypt;				//키크립트 핸들  
//	APPCOMPATFLAG	m_apl;			//호환성 플래그 
//
//public:
//	
//
//
//
//public:
//	CNProtectKC();
//	~CNProtectKC();
//	
//	void Release(void);
//	BOOL InitKeyCrypt(void);
//	void CloseHandle(void);
//	BOOL ChekAppCompatFlag(char* szGameFileName);
//
//	
//	BOOL& IsCreate(void)			{  return m_IsCreate;		}
//	BOOL& IsGetUse(void)			{  return m_IsUse;			}
//
//};
//
//
//
//
//extern CNProtectKC	m_npKeyCrypt;
//


#endif