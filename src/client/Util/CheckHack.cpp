#include "stdafx.h"
#include ".\checkhack.h"
//#include "TRose.h"
/*
TCheck_Init g_fpTRInit	= NULL;	
TCheck_Free g_fpTRFree	= NULL;
HMODULE		g_hChkLIB	= NULL;

/// for singleton
CCheckHack checkHack;

bool __stdcall CallBackFindHack ( char *szFileName )
{
	// return true면 해킹툴 프로세스 강제 종료..
	if ( IDOK == ::MessageBox( NULL, "동시에 실행할수 없는 프로세스가 실행중입니다.", szFileName, MB_OK ) )
		return true;
	

	// 해킹툴 종료 안됨 :: 게임 프로세스를 종료..
	return false;
}


CCheckHack::CCheckHack(void)
{
	m_bCanStartAPP = false;	
}

CCheckHack::~CCheckHack(void)
{		
}

bool CCheckHack::CanStartApp()
{
	return m_bCanStartAPP;
}

bool CCheckHack::Check()
{
	DWORD dwVersion = ::GetVersion();
	if ( dwVersion < 0x80000000 ) 
	{
 		g_hChkLIB = ::LoadLibrary( "TCheck.dll" );
 		if ( g_hChkLIB ) 
		{
 			g_fpTRInit = (TCheck_Init)::GetProcAddress( g_hChkLIB, "TRose_Init" );
 			g_fpTRFree = (TCheck_Free)::GetProcAddress( g_hChkLIB, "TRose_Free" );
 			if ( g_fpTRInit && g_fpTRFree )
			{
 				g_fpTRInit ( 5, CallBackFindHack );
			}
 			else 
			{
 				::MessageBox( NULL, "invalid TCheck.dll", "...", MB_OK );
 				return false;	// 게임 종료
 			}
 		} else 
		{
 			::MessageBox( NULL, "TCheck.dll not found", "...", MB_OK );
 			return false;		// 게임 종료
 		}
 	} else 
	{
 		// NT platform 이 아니면 무시
 		///::MessageBox( NULL, "not active on noe NT platfrom..", "...", MB_OK );
		return true;
 	}

    return true;
}

void CCheckHack::Release()
{
	if ( g_fpTRFree )
 		g_fpTRFree ();
 	if ( g_hChkLIB )
 		::FreeLibrary( g_hChkLIB );
}
*/