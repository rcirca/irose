/*
	**	모든 클라이언트의 접속을 유지한다.
		1. 클라이언트의 접속후 케릭터 선택 처리
		1. 케릭 선택후 존 서버로 이동.
		1. 길드챗 처리
		1. 귓말 처리
		1. 친구관리

	**	모든 존 서버의 접속을 유지
		1. 존별 IP/PORT 관리
*/
#include "stdafx.h"
#include "SHO_WS_DLL.h"
#include "SHO_WS_LIB.h"

#pragma	comment	(lib, "WS2_32.LIB")


//---------------------------------------------------------------------------------------
static int s_iCalledCnt = 0;
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
			if ( s_iCalledCnt < 2 ) {
				Sleep( 300 );
				s_iCalledCnt ++;
			}

		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


SHO_WS *g_GSApp=NULL;

//---------------------------------------------------------------------------------------
DLL_WS_API bool __stdcall WS_Init (HINSTANCE hInstance, char *szBaseDataDIR, int iLangType, EXE_WS_API *pAPI)
{
	if ( NULL == g_GSApp ) {
		g_GSApp = g_GSApp->InitInstance( hInstance, szBaseDataDIR, pAPI, iLangType );
	}

	return true;
}

//---------------------------------------------------------------------------------------
DLL_WS_API void __stdcall WS_Free ()
{
	if ( g_GSApp )  {
		g_GSApp->Shutdown ();
		g_GSApp->Destroy ();
		g_GSApp = NULL;
	}
}


//---------------------------------------------------------------------------------------
DLL_WS_API bool __stdcall WS_ConnectDB (char *szServerIP, char *szDBName, char *szDBUser, char *szDBPw, char *szLogDBUser, char *szLogDBPw)
{
	#define	DB_USER			"icarus"
	#define	DB_PASSWORD		"1111"
	char *szUser, *szPw;

	szUser = ( szDBUser ) ? szDBUser : DB_USER;
	szPw   = ( szDBPw   ) ? szDBPw   : DB_PASSWORD;

	if ( !g_GSApp->ConnectDB(szServerIP, szDBName, szUser, szPw, szLogDBUser, szLogDBPw ) ) {
		return false;
	}

	return true;
}


//---------------------------------------------------------------------------------------
DLL_WS_API bool __stdcall WS_Start(HWND hWnd,
						  char *szWorldNAME,
						  char *szLoginServerIP, int iLoginServerPORT,
						  char *szLogServerIP,	 int iLogServerPORT,
						  int   iZoneListenPORT, int iUserListenPORT, bool bBlockCreateCHAR)
{
	return g_GSApp->Start( hWnd, 
				szLoginServerIP,	iLoginServerPORT,
				szLogServerIP,		iLogServerPORT,
				szWorldNAME,
				iZoneListenPORT, 
				iUserListenPORT, bBlockCreateCHAR);
}

DLL_WS_API void __stdcall WS_Active( bool bActive )
{
    g_GSApp->Active( bActive );
}

//---------------------------------------------------------------------------------------
DLL_WS_API void __stdcall WS_CloseAllUSER ()
{
}

//---------------------------------------------------------------------------------------
DLL_WS_API void __stdcall WS_Shutdown ()
{
	if ( g_GSApp )  {
		g_GSApp->Shutdown ();
	}
}

//---------------------------------------------------------------------------------------
DLL_WS_API void __stdcall WS_AnnounceChat( char *szMsg )
{
	if ( g_GSApp )
		g_GSApp->Send_ANNOUNCE( szMsg );
}


DLL_WS_API void __stdcall WS_StartCLI_SOCKET ()
{
	if ( g_GSApp )
		g_GSApp->StartCLI_SOCKET ();
}

DLL_WS_API void __stdcall WS_ShutdownCLI_SOCKET ()
{
	if ( g_GSApp )
		g_GSApp->ShutdownCLI_SOCKET ();
}

DLL_WS_API void __stdcall WS_ToggleChannelActive(int iChannelNo)
{
	if ( g_GSApp )
		g_GSApp->ToggleChannelActive( iChannelNo );
}

DLL_WS_API void __stdcall WS_DisconnectChannelServer(int iChannelNo)
{
	// 채널 서버 디버깅중에 여기서 채널 서버를 삭제하면.... 로그인은 할수 있지만
	// 케릭터는 업데이트 되지 않는다...
	if ( g_GSApp )
		g_GSApp->DeleteChannelServer( iChannelNo );
}

//---------------------------------------------------------------------------------------
