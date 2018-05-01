#include "StdAfx.h"
#include ".\jstring.h"
#include "TControlMgr.h"
#include "ITFont.h"
#include <crtdbg.h>
CJString::CJString(void)
{
	m_iMaxHeight = 0;
}

CJString::~CJString(void)
{

}

void CJString::Clear()
{
	m_iMaxHeight = 0;
	m_Letters.clear();
}

void CJString::AddString( const char* pszString, SIZE sizeString, int iFont, DWORD dwColor )
{
	CJLetter Letter;
	Letter.SetLetter( pszString );
	Letter.SetSize( sizeString );
	Letter.SetColor( dwColor );
	Letter.SetFont( iFont );
	if( m_iMaxHeight < sizeString.cy )
		m_iMaxHeight = sizeString.cy;
	m_Letters.push_back( Letter );
}


//----------------------------------------------------------------------------------------
/// @param y :  bTramsform == true 일경우에는 offset_y에 더하지 않는다.
///				bTransform == false 일경우에는 라인간격으로도 사용된다.( 버그성 코드 - 추후 수정되어야 한다. 2005_6_14_23:55)
//----------------------------------------------------------------------------------------
void CJString::Draw( int x, int y, bool bTransform ,DWORD force_text_color)
{
	ITFont* IFont = CTControlMgr::GetInstance()->GetFontMgr();
	if( bTransform )
		IFont->SetTransformSprite( (float)x, (float)y, 0 );

	std::list< CJLetter >::iterator iter;
	POINT ptOffset = { x };

	for( iter = m_Letters.begin(); iter != m_Letters.end(); ++iter )
	{
		ptOffset.y = m_iMaxHeight - iter->GetSize().cy;
		if( !bTransform )
			ptOffset.y += y;

		if( force_text_color )
			IFont->Draw( iter->GetFont(), 1, ptOffset.x, ptOffset.y, force_text_color, iter->GetLetter() );
		else
			IFont->Draw( iter->GetFont(), 1, ptOffset.x, ptOffset.y, iter->GetColor(), iter->GetLetter() );

		ptOffset.x += iter->GetSize().cx;
	}
}

int CJString::GetStringHeight()
{
	return m_iMaxHeight;
}

bool CJString::IsEmpty()
{
	return m_Letters.empty();
}
