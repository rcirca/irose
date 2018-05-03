#include "stdafx.h"
#include ".\reloadprocess.h"
#include "../Game.h"
#include "../Interface/it_MGR.h"
#include "../Interface/CTDrawImpl.h"

CReloadProcess::CReloadProcess(void)
{
}

CReloadProcess::~CReloadProcess(void)
{	
}

bool CReloadProcess::LoadReloadTexture()
{
	return true;
}

void CReloadProcess::Draw( int iX, int iY, int iModuleID, int iGraphicID, float fProcessRate )
{
	//sprintf( g_MsgBuf, "%d", 100 - (int)( fProcessRate * 100 ) );
	//drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 2,2 ,g_dwBLACK, g_MsgBuf );
	CTDrawImpl* pDrawObj = g_itMGR.GetDrawImplPtr();
	pDrawObj->DrawCover( iX,
						 iY,
						iModuleID, 
						iGraphicID,
						D3DCOLOR_RGBA( 255, 255, 255, 255 ),
						D3DCOLOR_RGBA(   0,   0,   0, 110 ),
						fProcessRate );
}
