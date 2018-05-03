#include "StdAfx.h"
#include ".\tbutton.h"
#include "TControlMgr.h"
#include "ITSound.h"
#include "TDialog.h"
#include <MMSystem.h>
//#include "CToolTip.h"
#include <crtdbg.h>
CTButton::CTButton(void)
{
	m_pDraw = NULL;
	m_iButtonState = BS_NORMAL;
	m_iOverSoundID = 0;
	m_iClickSoundID = 0;
	
	m_iNormalGraphicID	= 0;	
	m_iOverGraphicID	= 0;
	m_iDownGraphicID	= 0;
	m_iDisableGrahicID	= 0;
	m_iBlink			= 0;
	m_iBlinkTime		= 500;
	m_bNoImage			= false;
	m_iSourceNormalGraphicID = 0;
	m_iBlinkGraphicID		 = 0;
	m_dwPrevUpdateTime		 = 0;
	m_textcolor				= D3DCOLOR_ARGB(255, 0, 0, 0);
}

CTButton::~CTButton(void)
{
}

bool	CTButton::Create(int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID )
{
	m_iModuleID			= iModuleID;
	m_iNormalGraphicID = iNormalGraphicID;
	m_iOverGraphicID   = iOverGraphicID;

	m_iSourceNormalGraphicID = m_iNormalGraphicID;


	m_iDownGraphicID   = iDownGraphicID;
	m_pDraw			   = CTControlMgr::GetInstance()->GetDrawMgr();
	Init( CTRL_BUTTON,iScrX, iScrY, iWidth, iHeight );
	m_bNoImage		   = false;
	return true;
}

unsigned int CTButton::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() || !IsEnable() ) return 0;


	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };

	//영역안에 있는지 체크
	if(!IsInside( ptMouse.x, ptMouse.y)) {
//		m_iButtonState = BS_NORMAL;
		return 0;
	}
	
	switch(uiMsg)
	{
	case	WM_LBUTTONDOWN:
		{
			if( m_iClickSoundID > 0 && m_iButtonState != BS_CLICKED )
				CTControlMgr::GetInstance()->GetSoundMgr()->Playsound( m_iClickSoundID );

			m_iButtonState = BS_CLICKED;
		}
		return m_iControlID;
	case	WM_LBUTTONUP:
		{

			if(m_iButtonState==BS_CLICKED) 
			{
				m_iButtonState = BS_MOUSEOVER;
				return m_iControlID;
			}
		}
	}
	return 0;
}

void CTButton::Draw()
{
	if( !IsVision()) return ;

	if( m_pDraw && !m_bNoImage)
	{
		if( !IsEnable() )
		{
			if( m_iDisableGrahicID )
				m_pDraw->Draw( m_sPosition.x, m_sPosition.y, m_iModuleID, m_iDisableGrahicID );
			else
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

		if( !m_text.empty() )
		{
			m_text.set_color( m_textcolor );
			m_text.draw();
		}
	}
}

void CTButton::Update( POINT ptMouse )
{
	if( !IsVision() ) return ;

	if( m_iBlink )
	{
		DWORD dwCurrTime = timeGetTime();

		if( dwCurrTime - m_dwPrevUpdateTime > m_iBlinkTime )
		{
			int iTempGID		= m_iNormalGraphicID;
			m_iNormalGraphicID  = m_iBlinkGraphicID;
			m_iBlinkGraphicID   = iTempGID;
			m_dwPrevUpdateTime  = dwCurrTime;
		}
	}
	///Parent Dlg과 있고 MouseOver를 처리한 Dialog가 있을경우
	if( m_pParent && CTDialog::GetProcessMouseOverDialog() )
	{
		if( (CWinCtrl*)CTDialog::GetProcessMouseOverDialog() != m_pParent )
			return;
	}

	if( !IsInside( ptMouse.x , ptMouse.y  ))
	{
		if( IsEnable() && m_iButtonState == BS_CLICKED || m_iButtonState == BS_MOUSEOVER )
			m_iButtonState = BS_NORMAL;
	}
	else
	{
		if( m_iButtonState != BS_MOUSEOVER && m_iButtonState != BS_CLICKED && !IsExclusive() && !IsProcessMouseOver() )
		{
			///Disalbe상태에서도 동작하자
			SetProcessMouseOverCtrl( this );
			SetProcessMouseOver( true );
			if( IsEnable() )
			{
				m_iButtonState = BS_MOUSEOVER;
				if( m_iBlink )
					SetBlink( 0 ,0 );

				if( m_iOverSoundID > 0 )
					CTControlMgr::GetInstance()->GetSoundMgr()->Playsound( m_iOverSoundID );
			}
		}

		if( m_iButtonState == BS_MOUSEOVER )
		{
			SetProcessMouseOver( true );
			SetProcessMouseOverCtrl( this );
		}
	}
}

void CTButton::SetText( char* szText )
{
	if( szText == NULL )
	{
		m_text.clear();
	}
	else
	{
		RECT rect = { 5, 3, m_iWidth-5, m_iHeight };
		m_text.set_string( szText, rect, 0 );
		m_text.set_format( DT_CENTER );
	}
}

CSinglelineString&	CTButton::GetText()
{ 
	return m_text; 
}

void CTButton::SetNoImage()
{
	m_bNoImage = true;
}

void CTButton::SetBlink( int iBlink , int iBlinkTime )
{
	m_dwPrevUpdateTime = timeGetTime();
	m_iBlink = iBlink;
	m_iBlinkTime = iBlinkTime;

	if( m_iBlink == 0 )
	{
		if( m_iNormalGraphicID != m_iSourceNormalGraphicID )
		{
			int tmp_gid = m_iNormalGraphicID;
			m_iNormalGraphicID = m_iSourceNormalGraphicID;
			m_iBlinkGraphicID  = tmp_gid;
		}
	}

	if( m_iBlinkGraphicID <= 0 )
		m_iBlinkGraphicID  = m_iOverGraphicID;
}


void CTButton::SetBlinkGid( int iGid )
{
	m_iBlinkGraphicID = iGid;
}

void CTButton::SetTextColor( DWORD color )
{
	m_textcolor = color;
}
