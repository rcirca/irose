#include "StdAfx.h"
#include ".\jletter.h"
#include <crtdbg.h>
CJLetter::CJLetter()
{
	m_dwColor = D3DCOLOR_ARGB( 0xFF, 0xFF, 0xFF, 0xFF);
	m_iFont   = 0;
}

void	CJLetter::SetLetter( const char* pszLetter )
{
	_ASSERT( pszLetter );
	if( pszLetter )
		m_Letter = pszLetter;
}

void	CJLetter::SetSize( SIZE& size )
{
	m_Size = size;
}

void	CJLetter::SetFont( int iFont )
{
	m_iFont = iFont;
}

void	CJLetter::SetColor( DWORD dwColor )
{
	m_dwColor = dwColor;
}

const SIZE&  CJLetter::GetSize()
{
	return m_Size;
}

int		CJLetter::GetFont()
{
	return m_iFont;
}

DWORD   CJLetter::GetColor()
{
	return m_dwColor;
}

const char* CJLetter::GetLetter()
{
	return m_Letter.c_str();
}