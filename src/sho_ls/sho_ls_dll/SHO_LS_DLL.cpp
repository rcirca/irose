// SHO_LS_DLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SHO_LS_DLL.h"
#include "../SHO_LS_LIB/SHO_LS_LIB.h"

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

//-------------------------------------------------------------------------------------------------

SHO_LS	*g_pLS = NULL;

//-------------------------------------------------------------------------------------------------
DLL_LS_API bool __stdcall LS_Init( HINSTANCE hInstance, EXE_LS_API *pExeAPI )
{
	if ( g_pLS == NULL ) {
		g_pLS = SHO_LS::InitInstance( hInstance, pExeAPI );
	}

	return ( NULL != g_pLS );
}
DLL_LS_API void __stdcall LS_Free ()
{
	if ( g_pLS )  {
		g_pLS->Shutdown ();
		g_pLS->Destroy ();
		g_pLS = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
DLL_LS_API bool __stdcall LS_StartServerSOCKET(HWND hWnd, char *szDBServerIP, int iServerListenPort, DWORD dwLoginRight, char *szGumsIP, int iGumsPORT, bool bShowOnlyWS )
{
	if ( NULL == g_pLS )
		return false;

	return g_pLS->StartServerSOCKET( hWnd, szDBServerIP, iServerListenPort, dwLoginRight, szGumsIP, iGumsPORT, bShowOnlyWS );
}


//-------------------------------------------------------------------------------------------------
DLL_LS_API bool __stdcall LS_StartClientSOCKET(int iClientListenPort, int iLimitUserCNT, BYTE btMD5[32])
{
	if ( NULL == g_pLS )
		return false;

	return g_pLS->StartClientSOCKET( iClientListenPort, iLimitUserCNT, btMD5 );
}


//-------------------------------------------------------------------------------------------------
DLL_LS_API void __stdcall LS_CloseClientSOCKET ()
{
	g_pLS->CloseClientSOCKET ();
}

//-------------------------------------------------------------------------------------------------
DLL_LS_API void __stdcall LS_Shutdown ()
{
	if ( g_pLS ) {
		g_pLS->Shutdown ();
	}
}

//-------------------------------------------------------------------------------------------------
DLL_LS_API void __stdcall LS_AnnounceChat(void *pServer, char *szAnnounceMsg)
{
	/*
	if ( g_pLS ) {
		g_pLS->SendAnnounce( szAnnounceMsg );
	}
	*/
}

DLL_LS_API void __stdcall LS_SetLoginRIGHT( DWORD dwLoginRight )
{
	if ( g_pLS ) 
		g_pLS->SetLoginRIGHT( dwLoginRight );
}

DLL_LS_API void __stdcall LS_SetLimitUserCNT( int iLimitUserCNT )
{
	if ( g_pLS && iLimitUserCNT >= 0 ) {
		g_pLS->SetLimitUserCNT( iLimitUserCNT );
	}
}

//-------------------------------------------------------------------------------------------------
