#ifndef	__SHO_WS_DLL_H
#define	__SHO_WS_DLL_H
#include "../SHO_WS_LIB/CWS_API.h"
//---------------------------------------------------------------------------------------

#ifdef SHO_WS_DLL_EXPORTS
#define DLL_WS_API extern "C" __declspec(dllexport)
#else
#define DLL_WS_API extern "C" __declspec(dllimport)
#endif



#ifndef	SHO_WS_DLL_EXPORTS
	typedef int  (__stdcall *fpWS_Init) (HINSTANCE hInstance);
	typedef void (__stdcall *fpWS_Free) ();
#endif



DLL_WS_API bool __stdcall WS_Init (HINSTANCE hInstance, char *szBaseDataDIR, int iLangType, EXE_WS_API *pAPI);
DLL_WS_API void __stdcall WS_Free ();

DLL_WS_API bool __stdcall WS_ConnectDB (char *szServerIP, char *szDBName, char *szDBUser, char *szDBPw, char *szLogDBUser, char *szLogDBPw);

DLL_WS_API bool __stdcall WS_Start (HWND hWnd,
						  char *szWorldNAME,
                          char *szLoginServerIP, int iLoginServerPORT,
						  char *szLogServerIP,	 int iLogServerPORT,
						  int   iZoneListenPORT, int iUserListenPORT, bool bBlockCreateCHAR);

DLL_WS_API void __stdcall WS_Active( bool bActive );

DLL_WS_API void __stdcall WS_CloseAllUSER ();

DLL_WS_API void __stdcall WS_Shutdown ();

DLL_WS_API void __stdcall WS_AnnounceChat(char *szMsg);


DLL_WS_API void __stdcall WS_StartCLI_SOCKET ();
DLL_WS_API void __stdcall WS_ShutdownCLI_SOCKET ();

DLL_WS_API void __stdcall WS_ToggleChannelActive(int iChannelNo);
DLL_WS_API void __stdcall WS_DisconnectChannelServer(int iChannelNo);

//---------------------------------------------------------------------------------------
#endif