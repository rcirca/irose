#include "StdAfx.h"
#include ".\actionevent.h"

CActionEvent::CActionEvent()
{
	m_pSource	= 0;
	m_uiID		= 0;
}
CActionEvent::CActionEvent( CWinCtrl* pSource, unsigned uiID  , unsigned wParam ,unsigned lParam )
{
	m_pSource	= pSource;
	m_uiID		= uiID;
	m_uiWParam  = wParam;
	m_uiLParam	= lParam;
}

CActionEvent::~CActionEvent(void)
{
 
}

CWinCtrl* CActionEvent::GetSource()
{
	return m_pSource;
}

unsigned  CActionEvent::GetID()
{
	return m_uiID;
}

void CActionEvent::SetSource( CWinCtrl* pSource )
{
	m_pSource = pSource;
}

void CActionEvent::SetID( unsigned uiID )
{
	m_uiID = uiID;
}

unsigned CActionEvent::GetWParam()
{
	return m_uiWParam;
}

unsigned CActionEvent::GetLParam()
{
	return m_uiLParam;
}

void CActionEvent::SetWParam( unsigned wParam )
{
	m_uiWParam = wParam;
}

void CActionEvent::SetLParam( unsigned lParam )
{
	m_uiLParam = lParam;
}