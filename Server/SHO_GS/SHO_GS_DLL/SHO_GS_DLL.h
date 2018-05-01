#ifndef	__DLL_GAMESERVER_H
#define	__DLL_GAMESERVER_H
#include "../SHO_GS_LIB/CGS_API.h"
//---------------------------------------------------------------------------------------

#ifdef SHO_GS_DLL_EXPORTS
#define DLL_GS_API extern "C" __declspec(dllexport)
#else
#define DLL_GS_API extern "C" __declspec(dllimport)
#endif

//---------------------------------------------------------------------------------------

DLL_GS_API int	__stdcall GS_BuildNO ();

DLL_GS_API bool __stdcall GS_Init (HINSTANCE hInstance, char *szBaseDataDIR, int iLangType, EXE_GS_API *pAPI);
DLL_GS_API void __stdcall GS_Free ();

DLL_GS_API short __stdcall GS_InitZONE( bool bAllActive );
DLL_GS_API bool  __stdcall GS_CheckZoneACTIVE( short nZoneNO, bool bCheck );
DLL_GS_API char* __stdcall GS_ZoneName(short nZoneNO);

DLL_GS_API bool __stdcall GS_ConnectSERVER( char *szDBServerIP,		char *szDBName,
											char *szDBUser,			char *szDBPW,
											char *szLogUser,		char *szLogPW,
											char *szMallUser,		char *szMallPW,
											char *szLoginServerIP,	int iLoginServerPort,
											char *szLogServerIP,	int iLogServerPort );

DLL_GS_API bool __stdcall GS_Start (  HWND hMainWND, char *szServerName, char *szClientListIP, int iClientListenPort, int iChannelNO, int iLowAge, int iHighAge );

DLL_GS_API void __stdcall GS_Shutdown ();

DLL_GS_API void __stdcall GS_AnnounceChat( short nZoneNO, char *szMsg );
DLL_GS_API void	__stdcall GS_SetUserLIMIT( DWORD dwUserLimit );

// DLL_GS_API  LONG	__stdcall GS_Execption( EXCEPTION_POINTERS* pExPtrs );

//---------------------------------------------------------------------------------------
#endif