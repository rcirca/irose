#include "stdafx.h"
#include ".\clanmark.h"
#include "../../CTDrawImpl.h"

CClanMark::CClanMark(void)
{
	m_iGraphicID = 0;
	m_iModuleID  = 0;
	m_iWidth	= 20;
	m_iHeight	= 20;
}

CClanMark::~CClanMark(void)
{

}

void CClanMark::Draw()
{
	g_DrawImpl.Draw( (int)m_sPosition.x, (int)m_sPosition.y, m_iModuleID, m_iGraphicID );
}

void CClanMark::SetGraphicID( int iID )
{
	m_iGraphicID = iID;
}

void CClanMark::SetModuleID( int iID )
{
	m_iModuleID = iID;
}

unsigned CClanMark::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( uiMsg == WM_LBUTTONDOWN && IsInside( LOWORD( lParam ), HIWORD( lParam )) )
		return GetControlID();

	return 0;
}