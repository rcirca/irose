#ifndef	__SHO_LS_DLL_H
#define	__SHO_LS_DLL_H
#include "../SHO_LS_LIB/CLS_API.h"
//-------------------------------------------------------------------------------------------------

#ifdef SHO_LS_DLL_EXPORTS
#define DLL_LS_API extern "C" __declspec(dllexport)
#else
#define DLL_LS_API extern "C" __declspec(dllimport)
#endif


DLL_LS_API bool __stdcall LS_Init ( HINSTANCE hInstance, EXE_LS_API *pExeAPI );
DLL_LS_API void __stdcall LS_Free ();

DLL_LS_API bool __stdcall LS_StartServerSOCKET(HWND hWnd, char *szDBServerIP, int iServerListenPort, DWORD dwLoginRight, char *szGumsIP, int iGumsPORT, bool bShowOnlyWS );
DLL_LS_API bool __stdcall LS_StartClientSOCKET(int iClientListenPort, int iLimitUserCNT, BYTE btMD5[32]);
DLL_LS_API void __stdcall LS_CloseClientSOCKET ();
DLL_LS_API void __stdcall LS_Shutdown ();

DLL_LS_API void __stdcall LS_AnnounceChat(void *pServer, char *szAnnounceMsg);

DLL_LS_API void __stdcall LS_SetLoginRIGHT( DWORD dwLoginRight );
DLL_LS_API void __stdcall LS_SetLimitUserCNT( int iLimitUserCNT );

//-------------------------------------------------------------------------------------------------
#endif
