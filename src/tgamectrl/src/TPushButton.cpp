#include "StdAfx.h"
#include ".\tpushbutton.h"
#include "TControlMgr.h"
#include "ITSound.h"
CTPushButton::CTPushButton(void)
{
	m_pDraw = NULL;
	m_iButtonState = BS_NORMAL;
	m_iOverSoundID = 0;
	m_iClickSoundID = 0;
}

CTPushButton::~CTPushButton(void)
{
}

bool	CTPushButton::Create(int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID )
{
	m_iModuleID			= iModuleID;
	m_iNormalGraphicID = iNormalGraphicID;
	m_iOverGraphicID   = iOverGraphicID;
	m_iDownGraphicID   = iDownGraphicID;
	m_pDraw			   = CTControlMgr::GetInstance()->GetDrawMgr();
	Init( CTRL_PUSHBUTTON,iScrX, iScrY, iWidth, iHeight );
	return true;
}

unsigned int CTPushButton::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() || !IsEnable() ) return 0;


	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };

	//영역안에 있는지 체크
	if(!IsInside( ptMouse.x, ptMouse.y)) {
		return 0;
	}
	
	switch(uiMsg)
	{
	case	WM_LBUTTONDOWN:
		{
			if( m_iClickSoundID > 0 && m_iButtonState != BS_CLICKED )
				CTControlMgr::GetInstance()->GetSoundMgr()->Playsound( m_iClickSoundID );

			switch(m_iButtonState )
			{
			case BS_CLICKED:
				m_iButtonState = BS_NORMAL;
				return m_iControlID;
			case BS_NORMAL:
				m_iButtonState = BS_CLICKED;
				return m_iControlID;
			default:
				break;
			}

		}
		return uiMsg;
	case	WM_LBUTTONUP:
		{
			break;
		}
	}
	return 0;
}

void CTPushButton::Draw()
{
	if( !IsVision()) return ;

	if( m_pDraw )
	{
		if( !IsEnable() )
		{
			m_pDraw->Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iNormalGraphicID );
		}
		else
		{
			switch( m_iButtonState )
			{
			case BS_CLICKED:
				m_pDraw->Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iDownGraphicID );
				break;
			case BS_MOUSEOVER:
				m_pDraw->Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iOverGraphicID );
				break;
			default:
				m_pDraw->Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iNormalGraphicID );
				break;
			}
		}
	}
}


void CTPushButton::Update( POINT ptMouse )
{
	if( !IsVision() || !IsEnable() ) return ;

	if( IsInside( ptMouse.x , ptMouse.y  ))
	{
		SetProcessMouseOver( true );
	}

}
void CTPushButton::SetText( char* szText )
{
	if( szText == NULL )
		m_stText.clear();
	else
		m_stText = szText;
}


