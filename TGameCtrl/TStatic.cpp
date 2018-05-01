#include "StdAfx.h"
#include ".\tstatic.h"
#include "TControlMgr.h"
#include "TImage.h"
CTStatic::CTStatic(void)
{
	m_pImage	= NULL;
	m_pFontImpl = CTControlMgr::GetInstance()->GetFontMgr();
	m_dwColor	= D3DCOLOR_ARGB(255,255,255,255);
	m_iFont		= 0;
}

CTStatic::~CTStatic(void)
{
	if( m_pImage )
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

bool CTStatic::Create(int iScrX, int iScrY, int iWidth, int iHeight)
{
	CTStatic::CTStatic();
	Init( CTRL_STATIC,iScrX, iScrY, iWidth, iHeight );
	return true;
}

unsigned int CTStatic::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	return 0;
}

void CTStatic::Update( POINT ptMouse )
{

}

void CTStatic::Draw()
{
	if( !IsVision() ) return;
	if( m_pImage )
	{
		m_pImage->Draw( m_sPosition );
		m_pFontImpl->Draw( GetFont(), true, m_sPosition.x + m_pImage->GetWidth(), m_sPosition.y, m_dwColor, m_strText.c_str());
	}
	else
	{
		m_pFontImpl->Draw( GetFont(), true, m_sPosition.x, m_sPosition.y, m_dwColor, m_strText.c_str());
	}
}

void CTStatic::SetString( const char* szString )
{
	if( szString == NULL )
		return;
	int iSize = (int)strlen( szString ) + 1;
	char* szTemp = new char[ iSize ];
	strncpy( szTemp, szString, iSize );
	m_strText = szTemp;
	delete []szTemp;
}
const char*	CTStatic::toString()
{
	return m_strText.c_str();
}

void CTStatic::SetImage( CTImage* pImage )
{ 
	m_pImage = pImage; 
}
void CTStatic::SetColor( DWORD dwColor )
{ 
	m_dwColor = dwColor; 
}

void CTStatic::SetFont( int iFont )
{
	m_iFont = iFont;
}

int CTStatic::GetFont()
{
	return m_iFont;
}