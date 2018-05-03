/******************************************************************************
  class	CNProtectSys 

  desc:  nProtect 처리하는 클래스 

  Date:  2005. 05. 19일 

  작성자:  박 지호.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++**/
#include "stdafx.h"
#include "nProtect.h"
#include "../Country.h"
#include "../Network/CNetwork.h"
 

CNProtectSys	m_nProtectSys;


#if defined(_EU)
	CNPGameLib npgl("RoseOnlineUK"); //nProtect 전역 변수 
#else
	CNPGameLib npgl("RoseOnlinePH"); 
#endif 
        
    

/********************************************************************************
    Func: NPGameMonCallback()

	Desc: nProtect 에서 사용하는 윈도우 함수  
********************************************************************************/
bool CALLBACK NPGameMonCallback(DWORD dwMsg,DWORD dwArg)
{
	
	return m_nProtectSys._NPGameMonCallback(dwMsg,dwArg);

}


/********************************************************************************
    Func: GetWindowsErrorMsg()

	Desc: DWORD 의 에러코드를 문자열로 변경해 메세지 창을 오픈한다. 
********************************************************************************/
void GetWindowsErrorMsg(DWORD dwError)
{

	LPVOID lpMsg = NULL;


	if(!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
					  NULL,dwError,MAKELANGID(LANG_ENGLISH,SUBLANG_DEFAULT),(LPTSTR)lpMsg,0,
					  NULL))
		return;


	MessageBox(NULL,(LPTSTR)lpMsg,"System Error",MB_OK);

}





/********************************************************************************
    Func: CNProtectSys()

	Desc: 생성자 
********************************************************************************/
CNProtectSys::CNProtectSys()
{
	
	m_IsGameExit	= FALSE;
	m_IsCreate		= FALSE;
	m_IsUse			= TRUE;

	m_curTick	= GetTickCount();
	m_oldTick	= 0;
	m_delayTick = 7000;

	ZeroMemory(m_HackString,sizeof(TCHAR)*MAX_BUF_LEN);

}


/********************************************************************************
    Func: ~CNProtectSys()

	Desc: 소멸자 
********************************************************************************/
CNProtectSys::~CNProtectSys()
{
	
	Release();

}


/********************************************************************************
    Func: Release()

	Desc: 메모리 해재
********************************************************************************/
void CNProtectSys::Release(void)
{

	ZeroMemory(m_HackString,sizeof(TCHAR)*MAX_BUF_LEN);

	m_IsCreate = FALSE;

}


/********************************************************************************
    Func: Permission_Country()

	Desc: 허용 가능한 국가인지 체크 한다.  

	현재( 2005/5/20) 필리핀에서만 동작하도록 수정
********************************************************************************/
void CNProtectSys::Permission_Country(void)
{

	//국가  체크 후 결과를 세팅한다. 
	if(CCountry::GetSingleton().IsPhilipplines() ||
		CCountry::GetSingleton().IsEU())
		m_IsUse = TRUE;
	else
		m_IsUse = FALSE;
}



/********************************************************************************
    Func: InitProtect()

	Desc: nProtect  초기화 
********************************************************************************/
BOOL CNProtectSys::InitProtect(void)
{

	//허용한 국가인지 체크한다. 
	Permission_Country();


	if(!m_IsUse)
		return TRUE;

	if(m_IsCreate)
		Release();

    //nProtect 초기화 
	DWORD	dwResult		= npgl.Init();
	LPCSTR	lpszMsg			= NULL;
	TCHAR	rtitleMsg[256]	= { 0 };
	

	//성공이라면...
	if(dwResult == NPGAMEMON_SUCCESS)
	{
		m_IsCreate = TRUE;
		return m_IsCreate;
	}


////////////////////////////////////////////////////////////////////////////////
	//에러 메세지 처리  :
	// !! 문자열 정렬을 만지지 마세요!! 창에서 라인이 깨집니다 !!
////////////////////////////////////////////////////////////////////////////////
	switch(dwResult)
	{
		case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
		case NPGAMEMON_ERROR_NFOUND_GG:
		case NPGAMEMON_ERROR_AUTH_INI:
		case NPGAMEMON_ERROR_NFOUND_INI:
			{
				lpszMsg ="GameGuard file does not exist or corrupted.\nPlease try installing GameGuard setup file.";
				break;
			}

		case NPGAMEMON_ERROR_CRYPTOAPI:
			{
				lpszMsg ="Some system files of Windows are corrupted.\nPlease try reinstalling Internet Explorer(IE).";
				break;
			}

		case NPGAMEMON_ERROR_EXECUTE:
			{
				lpszMsg = "Fail to run GameGuard. Please try installing GameGuard setup file again.";
				break;
			}


		case NPGAMEMON_ERROR_ILLEGAL_PRG:
			{
				lpszMsg = "Prohibited program was found.\nPlease end unnecessary programs and try again.";
				break;
			}

		case NPGMUP_ERROR_ABORT:
			{
				lpszMsg = "Fail to connect GameGuard update server.\nPlease try again  after a while, or check the settings of internet or persona firewall.";
				break;
			}
		
		case NPGMUP_ERROR_CONNECT:
		case NPGMUP_ERROR_DOWNCFG:
			{		
				lpszMsg =  "Fail to complete GameGuard updating.\nCheck virus first or adjust settings of PC management if any.";
				break;
			}

		case NPGMUP_ERROR_AUTH:
			{	
				lpszMsg =  "Fail to complete GameGuard updating.\nCheck virus first, or adjust settings of PC management if any.";
				break;
			}

		default:
			{
				lpszMsg = "GameGuard execution error occurred.\nPlease send all files  with extension of '.erl' in GameGuard\nfolder located in the game's  folder to game@inca.co.kr.";
				break;
			}
			
	}
////////////////////////////////////////////////////////////////////////////////


	//에러 메세지 박스 출력 
	wsprintf(rtitleMsg,"GameGuard execution error: %lu",dwResult);
	MessageBox(NULL,lpszMsg,rtitleMsg,MB_OK);

	return m_IsCreate;

}


/********************************************************************************
    Func: Set_hWnd()

	Desc: nProtect에 핸들을 설정한다. 
********************************************************************************/
void CNProtectSys::Set_hWnd(HWND hWnd)
{

	if(m_IsUse)
		npgl.SetHwnd(hWnd);
		
}


/********************************************************************************
    Func: Set_UserID()

	Desc: nProtect에 유저 ID를 삽입한다. . 
********************************************************************************/
void CNProtectSys::Set_UserID(const char* userID)
{

	if(m_IsUse && (userID != NULL))
		npgl.Send(userID);
	
}


/********************************************************************************
    Func: _NPGameMonCallback(DWORD dwMsg,DWORD dwArg)

	Desc: nProtect 윈도우 메세지 처리 함수 
********************************************************************************/
bool CNProtectSys::_NPGameMonCallback(DWORD dwMsg,DWORD dwArg)
{

	if(!m_IsUse)
		return TRUE;


	//문자열 초기화 
	ZeroMemory(m_HackString,sizeof(TCHAR)*MAX_BUF_LEN);


	switch(dwMsg)
	{
		case NPGAMEMON_COMM_ERROR:
		case NPGAMEMON_COMM_CLOSE:
			{	m_IsGameExit = TRUE;	return false;		}

		case NPGAMEMON_INIT_ERROR:
			{
				wsprintf(m_HackString,"GameGuard Initialize Error: %lu",dwArg);
				m_IsGameExit = TRUE; 
				break;
			}

		case NPGAMEMON_SPEEDHACK:
			{
				wsprintf(m_HackString,"GameGuard Initialize Error: %lu",dwArg);
				m_IsGameExit = TRUE; 
				break;
			}

		case NPGAMEMON_GAMEHACK_KILLED:
			{
				wsprintf(m_HackString,"Hacking Tool is Found!: %lu",dwArg);
				m_IsGameExit = TRUE; 
				break;
			}
		
		case NPGAMEMON_GAMEHACK_DETECT:
			{
				wsprintf(m_HackString,"Hacking Tool is Found!: %lu",dwArg);
				m_IsGameExit = TRUE; 
				break;
			}
	
		case NPGAMEMON_GAMEHACK_DOUBT:
			{
				wsprintf(m_HackString,"Game or GameGuard was Altered.: %lu",dwArg);
				m_IsGameExit = TRUE; 
				break;
			}

		case NPGAMEMON_CHECK_CSAUTH2:
			{
				memcpy(&m_AuthData,(PVOID)dwArg,sizeof(GG_AUTH_DATA));
				g_pNet->Send_AuthMsg();
				m_IsGameExit = FALSE;
				break;
			}

	}
	

	//게임이 종료 되어야 한다면  nProtect에 FALSE를 넘긴다. 
	if(m_IsGameExit)
		return FALSE;


	return true;

}


/********************************************************************************
    Func: Check_nProtectSafe();

	Desc: 일정 Tick 동안 nProtect가 유효한지 검사한다. 
********************************************************************************/
void CNProtectSys::	Check_nProtectSafe(DWORD ckTick)
{   

	if(!m_IsUse)
		return;

	//인자로 tick을 받았다면 대입 
	if(ckTick != 0)
		m_delayTick = ckTick;

	m_curTick = GetTickCount();

	DWORD tempTick = (m_curTick - m_oldTick);


	//일정 딜레이틱이 되었다면 검사 
	if(tempTick >= m_delayTick)
	{
		npgl.Check();
		m_oldTick = m_curTick;
	}

}


/********************************************************************************
    Func: Auth_FromServer();

	Desc: 게임서버로 부터 nProtect 인증을 받는다. 
********************************************************************************/
void  CNProtectSys::Auth_FromServer(WORD nType,GG_AUTH_DATA* pAuthData)
{

	if(!m_IsUse)
		return;

	//AUTH_MODULE_nPROTECT == 241
	if((nType != 241) || (pAuthData == NULL))
		return;
	
	npgl.Auth2(pAuthData);
	
}




//
///********************************************************************************
//
// Class CNProtectKC   : 키보드 암호화를 처리하는 클래스 
//
//********************************************************************************/
//
//
//CNProtectKC	m_npKeyCrypt;
///********************************************************************************
//    Func: CNProtectKC()
//
//	Desc: 생성자 
//********************************************************************************/
//CNProtectKC::CNProtectKC()
//{
//	
//	m_IsCreate		= FALSE;
//	m_IsUse			= TRUE;
//
//	m_hKCrypt		= 0;	
//
//}
//
//
///********************************************************************************
//    Func: ~CNProtectKC()
//
//	Desc: 소멸자 
//********************************************************************************/
//CNProtectKC::~CNProtectKC()
//{
//
//	Release();
//
//}
//
//
///********************************************************************************
//    Func: Release()
//
//	Desc: 메모리 해제 
//********************************************************************************/
//void CNProtectKC::Release(void)
//{
//
//	if(!m_IsCreate)
//		return;
//
//	CloseHandle();
//
//	m_IsCreate = FALSE;
//
//}
//
//
///********************************************************************************
//    Func: InitKeyCrypt()
//
//	Desc: 초기화 
//********************************************************************************/
//BOOL CNProtectKC::InitKeyCrypt(void)
//{
//
//
////만약 nProtect 실행 된다면 활성화 
//#ifndef NO_GAMEGUARD
//	  m_IsUse = TRUE;
//#else
//	  m_IsUse = FALSE;
//#endif
//
//	//사용하지 않는다면 리턴 
//	if(!m_IsUse)
//		return TRUE;
//
//
//	Release();
//
//	//DLL PATH 설정 
//	TCHAR szFileName[MAX_PATH] = {0};
//	GetModuleFileName(NULL, szFileName, sizeof(szFileName));
//
//
//	//호환성 설정 
//	if(!ChekAppCompatFlag(szFileName))
//		return FALSE;
//
//
//	char *ptr = strrchr(szFileName,'\\');
//		if(ptr != NULL)
//			*ptr = 0;
//
//	//리소스 PATH 설정 
//	NPKSetDrvPath(szFileName);
//	
//	//NPK Open
//	m_hKCrypt = NPKOpenDriver();
//	
///////////////////////////////////////////////////////////////////////////////////////////
////	nProtect 에서 권고 하는 에러처리문 소스 참고 
///////////////////////////////////////////////////////////////////////////////////////////
//		if(m_hKCrypt <= 0)
//		{	
//			DWORD	dwError = GetLastError();
//			
//			//NPK 표준에러 
//			if((dwError & NPK_ERROR_BASE) == NPK_ERROR_BASE)
//			{
//				
//				LPCTSTR	eMsg[] = 
//				{
//					{ "Adminustrator로 로그인 하시기 바랍니다." },
//					{ "nProtect KeyCrypt 버전이 맞지 않습니다.\n재부팅 후 실행 시켜 주세요."},
//				 	{ "WinXP 호환성 모드를 사용하고 계십니다.\n호환성 모드를 제거 하였습니다. 다시 실행해 주세요" }  
//				};
//				
//				LPCTSTR msg = NULL;	
//			
//
//				switch(dwError)
//				{
//					//Admin 계정이 아닐때 시스템 재시작시 Admin권한이 없어도 강제로 실행함.
//					case NPK_ERROR_NOTADMIN:
//						{
//							NPKLoadAtStartup(TRUE);
//							msg = eMsg[0];
//							break;
//						}
//					
//					//드라이버 버전이 다름 
//					case NPK_ERROR_DRIVERVERSION:
//						{	msg = eMsg[1];	break; }
//
//					//호환성 모드 제거 
//					case NPK_ERROR_VERIFYVERSION:
//						{
//							//GetModuleFileName(NULL,szFileName,sizeof(szFileName));
//							//NPKSetAppCompatFlag(szFileName,m_apl);
//							msg = eMsg[2];	break;
//						}
//				}
//				
//                MessageBox(NULL,msg,"nProtect KeyCrypt",MB_OK);
//				
//			}
//			//시스템 에러 
//			else
//				GetWindowsErrorMsg(dwError);
//
//			return FALSE;
//		}
/////////////////////////////////////////////////////////////////////////////////////////////
//	
//	NPKLoadAtStartup(TRUE);
//
//	m_IsCreate = TRUE;
//
//	return m_IsCreate;
//
//
//}
//
//
///********************************************************************************
//    Func: CloseHandle()
//
//	Desc: KeyCrypt 핸들을 닫는다. 
//********************************************************************************/
//void CNProtectKC::CloseHandle(void)
//{
//
//	if(!m_IsUse)
//		return;
//
//	if(!m_IsCreate)
//		return;
//
//	NPKCloseDriver(m_hKCrypt);
//	
//}
//
//
///********************************************************************************
//    Func: ChekAppCompatFlag()
//
//	Desc: 호환성을 체크 후 이상 있다면 게임을 종료한다. 
//********************************************************************************/
//BOOL CNProtectKC::ChekAppCompatFlag(char* szGameFileName)
//{
//
//	//호환성에서 "apcNone"이 아니면 설정 후 재 시작 
//	if(NPKGetAppCompatFlag(szGameFileName) != apcfNone)
//    {
//		
//		NPKSetAppCompatFlag(szGameFileName,apcfNone);
//
//		MessageBox(NULL,"종료 후 다시 재실행 해주세요","nProtect KeyCrypt",MB_OK);
//		//PostQuitMessage(0);
//		return FALSE;
//	}
//
//	return TRUE;
//
//}


/********************************************************************************
    Func:

	Desc:
********************************************************************************/

/********************************************************************************
    Func:

	Desc:
********************************************************************************/

/********************************************************************************
    Func:

	Desc:
********************************************************************************/