#include "StdAfx.h"
#include ".\tcheckbox.h"
#include "TControlMgr.h"
CTCheckBox::CTCheckBox(void)
{
	m_iCheckState = CBS_UNCHECK;
	m_iModuleID = -1;
	m_iCheckImageID = -1;
	m_iUncheckImageID = -1;
}

CTCheckBox::~CTCheckBox(void)
{

}

bool CTCheckBox::Create(int iScrX, int iScrY, int iWidth, int iHeight , int iModuleID, int iCheckImageID, int iUncheckImageID )
{
	m_iModuleID			= iModuleID;
	m_iCheckImageID		= iCheckImageID;
	m_iUncheckImageID	= iUncheckImageID;

	m_pDraw			   = CTControlMgr::GetInstance()->GetDrawMgr();

	Init( CTRL_CHECKBOX,iScrX, iScrY, iWidth, iHeight );
	return true;
}

unsigned CTCheckBox::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() || !IsEnable() ) return 0;
	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };

	if(!IsInside( ptMouse.x, ptMouse.y)) return 0;
	
	switch(uiMsg)
	{
	case	WM_LBUTTONDOWN:
		{
			if( m_iCheckState == CBS_UNCHECK )
				m_iCheckState = CBS_CHECK;
			else
				m_iCheckState = CBS_UNCHECK;
		}
		return m_iControlID;
	default:
		break;
	}
	return 0;
}

void CTCheckBox::Draw()
{
	if( !IsVision() ) return;

	if( m_pDraw )
	{
		switch( m_iCheckState )
		{
		case CBS_UNCHECK:
			if( m_iUncheckImageID >= 0 )
				m_pDraw->Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iUncheckImageID );
			break;
		case CBS_CHECK:
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iCheckImageID );
			break;
		default:
			break;
		}
	}
}

bool CTCheckBox::IsCheck()
{
	return (m_iCheckState==CBS_UNCHECK)?false:true;
}

void CTCheckBox::SetCheck()
{
	m_iCheckState = CBS_CHECK;
}
void CTCheckBox::SetUncheck()
{
	m_iCheckState = CBS_UNCHECK;
}
