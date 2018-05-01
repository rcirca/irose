#include "stdafx.h"
#include ".\cinfo.h"
#include "IO_ImageRes.h"
#include "CTDrawImpl.h"
#include "../../TGameCtrl/ResourceMgr.h"
CInfo::CInfo(void)
{
	Clear();
}

CInfo::~CInfo(void)
{
}
void CInfo::Draw(){
	///배경 이미지 그리기 
	///Width는 g_DrawImpl시에 Size를 주고
	///Height는 Text그리기 전에 그려주자.

	RECT rcDraw = { 2, 2, m_iWidth, 17 };

	std::vector< CTString >::iterator iter;
	
	int iPosX = m_ptPosition.x;
	int iPosY = m_ptPosition.y;

	//if( !m_strTitle.empty() )
	//{
	//	DrawLineImage( iPosX , iPosY , m_iWidth );
	//	if( int iStringCount = m_Title.GetStringCount() )
	//	{
	//		CJString* pString = m_Title.GetString( 0 );
	//		pString->Draw( 2, 2, false );
	//		iPosY += pString->GetStringHeight() + 4;
	//	}
	//}
	for( iter = m_listString.begin(); iter != m_listString.end(); ++iter )
	{
		DrawLineImage( iPosX , iPosY , m_iWidth );
		iter->Draw( rcDraw );
		iPosY += 17;
	}
}

bool CInfo::IsEmpty()
{
	return m_listString.empty();
}

void CInfo::DrawLineImage( int x, int y, int iWidth )
{
	m_iLineImageNo = CResourceMgr::GetInstance()->GetImageNID(IMAGE_RES_UI, "ID_BLACK_PANEL"  );
	g_DrawImpl.Draw( x, y, iWidth, IMAGE_RES_UI, m_iLineImageNo , D3DCOLOR_ARGB( 180,255,255,255) );
}

void CInfo::Clear()
{
	m_iLineImageNo = CResourceMgr::GetInstance()->GetImageNID(IMAGE_RES_UI, "ID_BLACK_PANEL"  );
	m_listString.clear();
//	m_strTitle.clear();
	
	m_iHeight = 0;
	
	m_uMaxSizeString	= 0;
	m_iWidth			= 0;

	m_ptPosition.x		= 0;
	m_ptPosition.y		= 0;
}

void CInfo::SetPosition( POINT pt )
{ 
	m_ptPosition = pt; 
	AdjustPosition();
}	
int	 CInfo::GetWidth(){ return m_iWidth; }
int  CInfo::GetHeight(){ return m_iHeight; }
int	 CInfo::GetMaxSizeString(){ return m_uMaxSizeString; }
void CInfo::AddString( CTString& TString )
{
	AddString( TString.m_strText.c_str(), TString.m_dwColor, TString.m_uFormat );
}

void CInfo::AddString( const char* pszTxt, DWORD color, HNODE hFont, UINT uFormat )
{
	if( pszTxt == NULL )
	{
		Clear();		
		return;
	}

	SIZE sizeString = getFontTextExtent( hFont, pszTxt );

	m_iHeight += 18;//sizeString.cy + 5;

	if( m_iWidth < sizeString.cx + 5)
		m_iWidth = sizeString.cx + 5;

	if( strlen( pszTxt ) > m_uMaxSizeString )
		m_uMaxSizeString = strlen( pszTxt );


	CTString String;
	String.SetColor( color );
	String.SetString( pszTxt );
	String.SetFormat( uFormat );
	String.SetFont( hFont );
	m_listString.push_back( String );	
	AdjustPosition();
}

void CInfo::AdjustPosition()
{
	if( m_ptPosition.x < 0 )
		m_ptPosition.x = 0;
	if( m_ptPosition.y < 0 )
		m_ptPosition.y = 0;

	if( m_ptPosition.x + m_iWidth > g_pCApp->GetWIDTH() )
		m_ptPosition.x = g_pCApp->GetWIDTH() - m_iWidth;

	if( m_ptPosition.y + m_iHeight  > g_pCApp->GetHEIGHT() )
		m_ptPosition.y = g_pCApp->GetHEIGHT() - m_iHeight ;

	//if( !m_strTitle.empty() )
	//	SetTitle( (char*)m_strTitle.c_str() ); 
}

//void CInfo::SetTitle( char* pszTitle  )
//{
//	assert( pszTitle );
//	if( pszTitle )
//	{
//		m_strTitle = pszTitle;
//		m_Title.SetDefaultColor( g_dwYELLOW );
//		m_Title.SetDefaultFont( FONT_NORMAL_BOLD );
//		int iStringWidthOnelinePrint = m_Title.SetString( pszTitle, m_iWidth );
//		if( m_iWidth < iStringWidthOnelinePrint )
//			m_iWidth = iStringWidthOnelinePrint;
//	}
//}