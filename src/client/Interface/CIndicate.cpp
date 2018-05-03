#include "stdafx.h"
#include ".\cindicate.h"
#include "../Game.h"
CIndicate::CIndicate(void)
{
	m_strTitle = "TITLE";
	m_strSubTitle = "SUBTITLE";
	m_Descriptions.clear();
	m_dwShowTime			=	0;
	m_dwDisplayTime			=	5000;
}

CIndicate::~CIndicate(void)
{
	m_Descriptions.clear();
}

void CIndicate::Draw()
{
	if( !IsVision() ) return;
	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, 0, 0, 0 );
	::setTransformSprite( mat );

	RECT rcTitle = { 0, 
		g_pCApp->GetHEIGHT() / 6,
		g_pCApp->GetWIDTH(),
		g_pCApp->GetHEIGHT() / 6 + m_iTitleHeight
	};

	drawFont ( g_GameDATA.m_hFONT[ FONT_TITLE ], true, &rcTitle, g_dwWHITE, DT_CENTER , m_strTitle.c_str() );
	

	RECT rcDrawDesc = { 0, 
		g_pCApp->GetHEIGHT() / 3, 
		g_pCApp->GetWIDTH(), 
		g_pCApp->GetHEIGHT() / 3 + m_iLineHeight 
	};

	std::list< string >::iterator iter;
	for( iter = m_Descriptions.begin(); iter != m_Descriptions.end(); ++iter )
	{
		drawFont ( g_GameDATA.m_hFONT[ FONT_DESCRIPTION ], true, &rcDrawDesc, g_dwWHITE, DT_CENTER , iter->c_str() );

		rcDrawDesc.top   += m_iLineHeight + m_iLineSpace;
		rcDrawDesc.bottom = rcDrawDesc.top + m_iLineHeight;
	}
}

void CIndicate::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	if( (g_GameDATA.GetTime() - m_dwShowTime) > m_dwDisplayTime )
		Hide();
}
void CIndicate::Show()
{
	CWinCtrl::Show();
	m_dwShowTime = g_GameDATA.GetTime();
}

void CIndicate::Hide()
{
	CWinCtrl::Hide();
}

void CIndicate::SetTitle( const char* pszTitle, const char* pszSubTitle )
{
	assert( pszTitle );
	if( pszTitle == NULL ) return;

	assert( !IsVision() );
	m_strTitle = pszTitle;

	SIZE size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_TITLE ], pszTitle );
	m_iTitleHeight		= size.cy;

//	if( pszSubTitle == NULL ) return;
}


void CIndicate::AddDescription( const char* pszDescription )
{
	assert( !IsVision() );
	assert( pszDescription );
	SIZE size = getFontTextExtent( g_GameDATA.m_hFONT[ FONT_DESCRIPTION ], pszDescription );
	if( pszDescription )
	{
		m_Descriptions.push_back( pszDescription );
		m_iLineSpace			=  size.cy / 4;
		m_iLineHeight			=  size.cy;
	}
}

void CIndicate::ResetDescription()
{
	assert( !IsVision() );
	m_Descriptions.clear();
}
