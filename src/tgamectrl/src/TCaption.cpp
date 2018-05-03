#include "StdAfx.h"
#include ".\tcaption.h"
#include "TImage.h"
#include "TButton.h"
#include "TControlMgr.h"
#include "ITFont.h"


CTCaption::CTCaption(void)
{
	m_bClicked = false;
	m_pIcon			= NULL;
	m_pImage		= NULL;
	m_pCloseButton	= NULL;
	m_pIconButton	= NULL;
}

CTCaption::~CTCaption(void)
{
	if( m_pIcon )
	{
		delete m_pIcon;
		m_pIcon = NULL;
	}
	if( m_pImage )
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	if( m_pCloseButton )
	{
		delete m_pCloseButton;
		m_pCloseButton = NULL;
	}

	if( m_pIconButton )
	{
		delete m_pIconButton;
		m_pIconButton = NULL;
	}
}
bool CTCaption::Create(int iScrX, int iScrY, int iWidth, int iHeight)
{
	Init( CTRL_CAPTION, iScrX, iScrY, iWidth, iHeight );
	return true;
}
///CAPTION의 경우 다른 컨트롤들이 캡션위에 있을수 있다. 그러므로 항상 메세지 처리를 계속하도록
/// 0을 리턴한다.
unsigned int CTCaption::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision()) return 0;
	
	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };
//	if( !IsInside( ptMouse.x, ptMouse.y ))
//	{
//		m_bClicked = false;
//		return 0;
//	}
	if( m_pCloseButton && m_pCloseButton->Process( uiMsg, wParam, lParam ))
		return m_pCloseButton->GetControlID();

	if( m_pIconButton && m_pIconButton->Process( uiMsg, wParam, lParam))
		return m_pIconButton->GetControlID();


	switch(uiMsg)
	{
	case	WM_LBUTTONDOWN:
		{

			if( IsInside( ptMouse.x, ptMouse.y ) )
			{
				CWinCtrl::SetMouseExclusiveCtrl( this );			
				m_bClicked = true;
			}
		}
		return 0;
//		return uiMsg;///
	case	WM_LBUTTONUP:
		{
			m_bClicked = false;
		}
//		return uiMsg;
		return 0;///CAPTION
	}
	return 0;
}

void CTCaption::Update(  POINT ptMouse )
{
	if( !IsVision()) return;

	if( !IsInside( ptMouse.x, ptMouse.y ) )
		m_bClicked = false;
}

void CTCaption::Draw()
{
	if( m_pImage )
		m_pImage->Draw();

	if( m_pIcon )
		m_pIcon->Draw();

	if( m_pCloseButton )
		m_pCloseButton->Draw();

	if( m_pIconButton )
		m_pIconButton->Draw();


	if( !m_strTitle.empty() )
	{
		ITFont* pFontMgr =	CTControlMgr::GetInstance()->GetFontMgr();
		pFontMgr->SetTransformSprite( (float) m_sPosition.x, (float) m_sPosition.y );
		pFontMgr->Draw( 0, true, m_sPosition.x, m_sPosition.y, D3DCOLOR_ARGB(255,255,255,255), m_strTitle.c_str() );
	}
}

void CTCaption::MoveWindow(POINT pt)
{
	CWinCtrl::MoveWindow( pt );
	if( m_pImage )
		m_pImage->MoveWindow( m_sPosition );

	if( m_pIcon )
		m_pIcon->MoveWindow( m_sPosition );

	if( m_pCloseButton )
		m_pCloseButton->MoveWindow( m_sPosition );

	if( m_pIconButton )
		m_pIconButton->MoveWindow( m_sPosition );
}

void CTCaption::Hide()
{
	m_bClicked = false;
	CWinCtrl::Hide();
}

bool CTCaption::IsClicked()
{ 
	return m_bClicked; 
}

void CTCaption::SetString( const char* pszString )
{
	m_strTitle = pszString;
}
//*------------------------------------------------*/
/// 
//*------------------------------------------------*/
void CTCaption::SetIcon( CTImage* pImage )
{
	if( m_pIcon )
		delete m_pIcon;
	m_pIcon = pImage;
	m_pIcon->SetControlID( IID_IMAGE_ICON );
}
//*------------------------------------------------*/
///
//*------------------------------------------------*/
void CTCaption::SetImage( CTImage* pImage )
{
	if( m_pImage )
		delete m_pImage;
	m_pImage = pImage;
	m_pImage->SetControlID( IID_IMAGE_BG );

}
void CTCaption::SetCloseButton( CTButton* pButton )
{
	if( m_pCloseButton )
		delete m_pCloseButton;

	m_pCloseButton = pButton;
	m_pCloseButton->SetControlID( IID_BTN_CLOSE );
}

void CTCaption::SetIconButton( CTButton* pButton )
{
	if( m_pIconButton )
		delete m_pIconButton;

	m_pIconButton = pButton;
	m_pIconButton->SetControlID( IID_BTN_CLOSE );
}
