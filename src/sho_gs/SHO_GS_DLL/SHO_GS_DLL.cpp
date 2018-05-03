// DLL_GameSERVER.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SHO_GS_DLL.h"
#include "LIB_gsMAIN.h"

//---------------------------------------------------------------------------------------
static int s_iCalledCnt = 0;
BOOL APIENTRY DllMain( HANDLE /*hModule*/, 
                       DWORD  ul_reason_for_call, 
                       LPVOID /*lpReserved*/
					 )
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

//---------------------------------------------------------------------------------------
/*
typedef int  (__stdcall *fpGS_Init) ();
typedef void (__stdcall *fpGS_Free) ();

fpGS_Init	GS_Init;
fpGS_Free	GS_Free;

HMODULE hGSModule;

hGSModule = ::LoadLibrary( "SHO_GS.DLL" );
if ( NULL == hGSModule ) return false;

GS_Init = (fpGS_Init) ::GetProcAddress( hGSModule, "GS_Init" );
if ( !GS_Init ) return false;

GS_Free = (fpGS_Free) ::GetProcAddress( hGSModule, "GS_Free" );
if ( !GS_Free ) return false;
	;

BOOL ::FreeLibrary( hGSModule );
*/

CLIB_GameSRV *g_pServer;

//LONG __stdcall MyUnhandledExceptionFilter ( EXCEPTION_POINTERS* pExPtrs);
//
//DLL_GS_API  LONG	__stdcall GS_Execption( EXCEPTION_POINTERS* pExPtrs )
//{
//	return MyUnhandledExceptionFilter( pExPtrs );
//}

//---------------------------------------------------------------------------------------
DLL_GS_API int	__stdcall GS_BuildNO ()
{
	return 0;
}

//---------------------------------------------------------------------------------------
DLL_GS_API bool __stdcall GS_Init (HINSTANCE hInstance, char *szBaseDataDIR, int iLangType, EXE_GS_API *pAPI)
{
	/*CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            (LPTHREAD_START_ROUTINE)CLIB_GameSRV::AuthHWID,       // thread function name
            0,          // argument to thread function 
            0,                      // use default creation flags 
            0);   // returns the thread identifier */
	//CLIB_GameSRV::AuthHWID();
	g_pServer = CLIB_GameSRV::InitInstance( hInstance, szBaseDataDIR, pAPI, iLangType );
	return ( NULL != g_pServer );
}

//---------------------------------------------------------------------------------------
DLL_GS_API void __stdcall GS_Free ()
{
	if ( g_pServer )
		g_pServer->Destroy ();
}

DLL_GS_API short __stdcall GS_InitZONE( bool bAllActive )
{
	if ( g_pServer )
		return g_pServer->InitLocalZone( bAllActive );
	return -1;
}
DLL_GS_API bool __stdcall GS_CheckZoneACTIVE( short nZoneNO, bool bCheck )
{
	if ( g_pServer )
		return g_pServer->CheckZoneToLocal( nZoneNO, bCheck );

	return false;
}
DLL_GS_API char* __stdcall GS_ZoneName(short nZoneNO)
{
	return g_pServer->GetZoneName( nZoneNO );
}

//---------------------------------------------------------------------------------------
DLL_GS_API bool __stdcall GS_ConnectSERVER(	char *szDBServerIP,		char *szDBName,
											char *szDBUser,			char *szDBPW,
											char *szLogUser,		char *szLogPW,
											char *szMallUser,		char *szMallPW,
											char *szLoginServerIP,	int iLoginServerPort,
											char *szLogServerIP,	int iLogServerPort )
{
	g_pServer = CLIB_GameSRV::GetInstance ();
	if ( NULL == g_pServer )
		return false;

	return g_pServer->ConnectSERVER(szDBServerIP,	szDBName, 
									szDBUser,		szDBPW,
									szLogUser,		szLogPW,
									szMallUser,		szMallPW,
									szLoginServerIP,iLoginServerPort,
									szLogServerIP,	iLogServerPort );
}

//---------------------------------------------------------------------------------------
DLL_GS_API bool __stdcall GS_Start( HWND hMainWND, char *szServerName, char *szClientListIP, int iClientListenPort, int iChannelNO, int iLowAge, int iHighAge )
{
	g_pServer = CLIB_GameSRV::GetInstance ();
	if ( NULL == g_pServer )
		return false;

	return g_pServer->Start( hMainWND, szServerName, 
						szClientListIP, iClientListenPort, 
						iChannelNO, iLowAge, iHighAge );
}
DLL_GS_API void __stdcall GS_Shutdown ()
{
	if ( g_pServer )
		g_pServer->Shutdown ();
}

//---------------------------------------------------------------------------------------
DLL_GS_API void __stdcall GS_AnnounceChat( short nZoneNO, char *szMsg )
{
	if ( g_pServer )
		g_pServer->Send_ANNOUNCE( nZoneNO, szMsg );
}

//---------------------------------------------------------------------------------------
DLL_GS_API void	__stdcall GS_SetUserLIMIT( DWORD dwUserLimit )
{
	if ( g_pServer )
		g_pServer->Set_UserLIMIT( dwUserLimit );
}

//---------------------------------------------------------------------------------------
