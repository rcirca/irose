#include "StdAfx.h"
#include ".\ctooltip.h"
#include "WinCtrl.h"
#include "ITFont.h"
#include "TImage.h"
#include "TControlMgr.h"
#include "TImage.h"
CToolTip::CToolTip(void)
{

	m_iFont		  = 0;			
	m_iMarginLeft = 0;		
	m_iMarginTop  = 0;		
	m_iLineSpace  = 0;		
	m_iMaxLength  = 10;
	m_dwColor = 0;
	
	m_pImageTop		= NULL;
	m_pImageMiddle  = NULL;
	m_pImageBottom  = NULL;
}

CToolTip::~CToolTip(void)
{

}

void	CToolTip::Draw( POINT pt )
{
	DrawTopImage( pt );
	DrawBody( pt );
	DrawBottomImage( pt );
}


void	CToolTip::DrawTopImage( POINT pt )
{
	if( m_pImageTop )
	{
		m_pImageTop->SetPosition( pt );
		m_pImageTop->Draw();
	}
}

void	CToolTip::DrawBody( POINT pt )
{
	int iLineCount = m_splitMsg.GetLineCount();
	if( iLineCount <= 0 )
		return;

	int iFontHeight = 1;
	ITFont* pFont = CTControlMgr::GetInstance()->GetFontMgr();
	if( pFont )
		iFontHeight = pFont->GetFontHeight( m_iFont );

	POINT ptNew = {pt.x + m_iMarginLeft, pt.y + m_iMarginTop };
	
	for( int i = 0; i < iLineCount; ++i)
	{
		DrawText( m_iFont, ptNew , m_dwColor, m_splitMsg.GetString( i ) );			
		ptNew.y += iFontHeight + m_iLineSpace;
	}
}

void	CToolTip::DrawText(int iFont, POINT pt , D3DCOLOR color, const char* szText )
{
	if( m_pImageMiddle )
	{
		m_pImageMiddle->SetPosition( pt );
		m_pImageMiddle->Draw();
	}

	ITFont* pFont = CTControlMgr::GetInstance()->GetFontMgr();
	if( pFont )
		pFont->Draw( iFont , true , pt.x, pt.y , color,  szText );
}

void	CToolTip::DrawBottomImage( POINT pt )
{
	if( m_pImageBottom )
	{
		m_pImageBottom->SetPosition( pt );
		m_pImageTop->Draw();
	}
}
void	CToolTip::SetImageTop( CTImage* pImage ){ m_pImageTop = pImage; pImage->Show();}
void	CToolTip::SetImageMiddle( CTImage* pImage ){ m_pImageMiddle = pImage;pImage->Show(); }
void	CToolTip::SetImageBottom( CTImage* pImage ){ m_pImageBottom = pImage;pImage->Show(); }

void	CToolTip::SetText( char* szText )
{
	if( szText == NULL )
	{
		m_stMsg.clear();
		m_splitMsg.Clear();
	}
	else
	{
		m_stMsg		= szText;
		m_splitMsg.DoSplit( szText, m_iMaxLength ,CTControlMgr::GetInstance()->GetCodePage());
	}
}