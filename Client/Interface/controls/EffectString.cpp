#include "stdafx.h"
#include ".\effectstring.h"
#include "../util/localizing.h"
#include "../Game.h"
CEffectString::CEffectString(void)
{
	m_color_red		= 0xff;
	m_color_green	= 0xff;
	m_color_blue	= 0xff;
	m_color_alpha	= 0xff;
}

CEffectString::~CEffectString(void)
{

}

void CEffectString::SetColor( BYTE red, BYTE green, BYTE blue )
{
	m_color_red		= red;
	m_color_green	= green;
	m_color_blue	= blue;
}

bool CEffectString::SetString( int font, char* message, POINT draw_position, int display_width, DWORD display_time )
{
	assert( message );
	assert( font >= 0 && font < MAX_FONT );
	if( message == NULL ) return false;
	if( font < 0 || font >= MAX_FONT )	return false;

	m_parser.Split( font, message, display_width, CLocalizing::GetSingleton().GetCurrentCodePageNO() );
	m_time_set		= g_GameDATA.GetGameTime();
	m_font			= font;
	m_draw_position = draw_position;

	assert( display_time >= 5000 );
	
	if( display_time < 5000 )
		m_time_display = 5000;
	else
		m_time_display = display_time;

	return true;
}

/**
* Fade In/Out 기능의 글자색깔 변화는 현재 아웃라인폰트에 적용시 그림자가 남게 되어서 안됨 , 주석처리함
* 추후에 엔진에서 그림자의 색깔까지 바꿀수 있도록 수정되면 적용
*
* @Author		최종진
* @Date			2005/9/6
*/
int	CEffectString::Update()
{
	//if end return -1;
	DWORD curr_time = g_GameDATA.GetGameTime();

	if( curr_time > m_time_set + m_time_display )
		return -1;

	DWORD run_time = curr_time - m_time_set;
	DWORD change_alpha_time = 2000;

	//if( run_time < change_alpha_time )///시작한지 1초 이내라면
	//	m_color_alpha = (BYTE)( 0xff * run_time / change_alpha_time );
	//else if( m_time_display - run_time < change_alpha_time ) ///끝나기 까지 1초이내라면 )
	//	m_color_alpha = (BYTE)( 0xff * ( m_time_display - run_time ) / change_alpha_time );
	//else
		m_color_alpha = 0xff;

	//_RPT1( _CRT_WARN, "String Effect's Alpha is %x \n", m_color_alpha );
	return 0;
}

void CEffectString::Draw()
{
	if( int line_count = m_parser.GetLineCount() )
	{
		const char* message = NULL;
		SIZE	size;	

		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_draw_position.x, (float)m_draw_position.y,0.0f);
		::setTransformSprite( mat );
		int offset_y = 0;
		for( int i = 0 ; i < line_count; ++i )
		{
			if( message = m_parser.GetString(i) )
			{
				drawFont( g_GameDATA.m_hFONT[ m_font ], true, 0, offset_y, D3DCOLOR_ARGB( m_color_alpha, m_color_red, m_color_green, m_color_blue), message );
				size = getFontTextExtent( g_GameDATA.m_hFONT[ m_font ], message );
				offset_y += size.cy + size.cy / 3;
				//_RPT1( _CRT_WARN, "String Effect's message is %s \n", message );
			}
		}
	}
}

void CEffectString::SetType( int type )
{
	m_type = type;
}

int CEffectString::GetType()
{
	return m_type;
}

