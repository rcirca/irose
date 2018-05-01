#include "StdAfx.h"
#include ".\singlelinestring.h"
#include "TControlMgr.h"
#include "ITFont.h"
CSinglelineString::CSinglelineString(void)
{
	m_color  = D3DCOLOR_ARGB( 255, 255, 255, 255);
	m_format = 0;
}

CSinglelineString::~CSinglelineString(void)
{
}

void	CSinglelineString::set_string( const char* msg, const RECT& rect , int ifont )
{
	_ASSERT( msg );
	if( msg == NULL ) return;

	clear();
	m_msg			= msg;
	m_font			= ifont;

	set_rect( rect );
}

void	CSinglelineString::set_rect( const RECT& rect )
{
	m_rect = rect;

	ITFont*	pFontMgr = CTControlMgr::GetInstance()->GetFontMgr();

	_ASSERT( pFontMgr );;
	SIZE size = pFontMgr->GetFontTextExtent( m_font, m_msg.c_str() );

	bool end_ellipsis = ( size.cx > ( m_rect.right - m_rect.left)  )?true:false;

	WORD codepage = (WORD)CTControlMgr::GetInstance()->GetCodePage();
	if( m_end_ellipsis != end_ellipsis )
	{
		m_ellipsis_msg.clear();
		m_end_ellipsis = end_ellipsis;
		if( m_end_ellipsis )
		{
			std::string temp_string;
			SIZE		size_temp_string;
			SIZE		size_ellipsis_msg;
			
			const char* ellipsis = "..";
			SIZE size_ellipsis = pFontMgr->GetFontTextExtent( m_font, ellipsis );
			
			LPCSTR	curr = m_msg.c_str();
			LPSTR	next = CharNextExA( codepage, curr,  0 );

			while( curr != next )
			{
				temp_string.clear();
				for( ; curr != next; ++curr )
					temp_string.push_back( *curr );
				
				size_temp_string  = pFontMgr->GetFontTextExtent( m_font, temp_string.c_str() );
				size_ellipsis_msg = pFontMgr->GetFontTextExtent( m_font, m_ellipsis_msg.c_str() );
				if( size_temp_string.cx + size_ellipsis_msg.cx + size_ellipsis.cx >= m_rect.right - m_rect.left )
					break;

				m_ellipsis_msg.append( temp_string );
				next = CharNextExA( codepage, curr, 0 );
			}

			m_ellipsis_msg.append( ellipsis );
		}
	}
}

const RECT&	CSinglelineString::get_rect()
{
	return m_rect;
}

void	CSinglelineString::draw()
{
	if( m_end_ellipsis && !m_ellipsis_msg.empty() )
		CTControlMgr::GetInstance()->GetFontMgr()->Draw( m_font, true, &m_rect, m_color, 0, m_ellipsis_msg.c_str() );
	else
		CTControlMgr::GetInstance()->GetFontMgr()->Draw( m_font, true, &m_rect, m_color, m_format, m_msg.c_str() );

}

bool	CSinglelineString::is_ellipsis()
{
	return m_end_ellipsis;
}

const char* CSinglelineString::get_string()
{
	return m_msg.c_str();
}

void CSinglelineString::set_color( DWORD color )
{
	m_color = color;
}

bool CSinglelineString::empty()
{
	return m_msg.empty();
}

void CSinglelineString::clear()
{
	m_msg.clear();
	m_ellipsis_msg.clear();
	m_end_ellipsis = false;
}

void CSinglelineString::set_format( DWORD format )
{
	m_format = format;
}