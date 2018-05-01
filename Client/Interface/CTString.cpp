#include "stdafx.h"
#include ".\ctstring.h"

CTString::CTString(void)
{
}

CTString::~CTString(void)
{
}
void CTString::SetString( const char* pszTxt )
{ 
	m_strText = pszTxt; 
}
void CTString::SetColor( DWORD dwColor )
{ 
	m_dwColor = dwColor; 
}
void CTString::SetFormat( UINT uFormat )
{ 
	m_uFormat = uFormat; 
}

void CTString::SetFont( HNODE hFont )
{ 
	m_hFont = hFont; 
}

void CTString::Draw( RECT& rcDraw )
{
	drawFont( m_hFont, true, &rcDraw, m_dwColor, m_uFormat, m_strText.c_str() );
}
