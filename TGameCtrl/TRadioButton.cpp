#include "StdAfx.h"
#include ".\tradiobutton.h"
#include "TRadioBox.h"
#include "TControlMgr.h"
#include "ITSound.h"
#include "ITFont.h"
CTRadioButton::CTRadioButton(void)
{
	m_bGhost	= FALSE;
	m_pDraw		= NULL;
	m_pRadioBox = NULL;
	m_iButtonState = BS_NORMAL;
	m_iDisableSoundID = 0;
	m_pFontImpl		  = NULL;
}

CTRadioButton::~CTRadioButton(void)
{
}

bool	CTRadioButton::Create(int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID,int iModuleID )
{
	m_iModuleID			= iModuleID;
	m_iNormalGraphicID	= iNormalGraphicID;
	m_iOverGraphicID	= iOverGraphicID;
	m_iDownGraphicID	= iDownGraphicID;
	
	m_pDraw				= CTControlMgr::GetInstance()->GetDrawMgr();
	m_pFontImpl			= CTControlMgr::GetInstance()->GetFontMgr();

	Init( CTRL_RADIOBUTTON,iScrX, iScrY, iWidth, iHeight );
	return true;
}

unsigned int CTRadioButton::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( m_bGhost ) return 0;
	if(!IsVision() ) return 0;

	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };
	//영역안에 있는지 체크
	if(!IsInside( ptMouse.x, ptMouse.y )) {
		//m_iButtonState = BS_NORMAL;
		return 0;
	}


	if( !IsEnable() )
	{
		if( uiMsg == WM_LBUTTONDOWN && m_iDisableSoundID > 0 )
			CTControlMgr::GetInstance()->GetSoundMgr()->Playsound( m_iDisableSoundID );

		return 0;
	}

	switch(uiMsg)
	{
	case	WM_LBUTTONDOWN:
		{
			m_iButtonState = BS_CLICKED;
			if( m_pRadioBox )
				m_pRadioBox->EventChangeSelect( GetControlID());
		}
		return m_iControlID;
	case	WM_LBUTTONUP:
		{
			if( m_iButtonState == BS_CLICKED )
				return m_iControlID;
		}
	}
	return 0;
}

void CTRadioButton::Draw()
{
	if( m_bGhost ) return;
	if( !IsVision()) return ;

	if( m_pDraw )
	{
		DWORD  dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);
		//if( !IsEnable() )
		//{
		//	m_pDraw->Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iNormalGraphicID );
		//}
		//else
		{
			switch( m_iButtonState )
			{
			case BS_CLICKED:
				dwColor = D3DCOLOR_ARGB(255,255, 255, 0);
				m_pDraw->Draw( m_sPosition.x, m_sPosition.y,m_iModuleID, m_iDownGraphicID );
				break;
			case BS_MOUSEOVER:
				m_pDraw->Draw( m_sPosition.x, m_sPosition.y,m_iModuleID, m_iOverGraphicID );
				break;
			default:
				m_pDraw->Draw( m_sPosition.x, m_sPosition.y,m_iModuleID, m_iNormalGraphicID );
				break;
			}
		}

		if( !m_text.empty() )
		{
			m_text.set_color( dwColor );		
			m_text.draw();
		}
	}
}

void CTRadioButton::Update( POINT ptMouse )
{
	if( m_bGhost ) return;
	if( !IsVision()) return ;

	if( !IsInside( ptMouse.x , ptMouse.y  ))
	{
		if( m_iButtonState == BS_MOUSEOVER )
			m_iButtonState = BS_NORMAL;
	}
	else
	{
		if( m_iButtonState != BS_CLICKED )
			m_iButtonState = BS_MOUSEOVER;
	}
}
void CTRadioButton::SetText( char* szText )
{
	if( m_bGhost ) return;

	if( szText == NULL )
	{
		m_text.clear();
	}
	else
	{
		RECT rect = { 5,5, m_iWidth-5, m_iHeight };
		m_text.set_string( szText, rect, 0 );
	}
}

void CTRadioButton::SetButtonState( int iState )
{ 
	if( m_bGhost ) return;

	m_iButtonState = iState; 
	if( m_pRadioBox && m_iButtonState == BS_CLICKED )
		m_pRadioBox->EventChangeSelect( GetControlID());

}
CSinglelineString&	CTRadioButton::GetText()
{ 
	return m_text; 
}

void CTRadioButton::SetSoundDisableID( int i )
{ 
	m_iDisableSoundID = i; 
}

void CTRadioButton::SetGhost()
{
	m_bGhost = true;
}