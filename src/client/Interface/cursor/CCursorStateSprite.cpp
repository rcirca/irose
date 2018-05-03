#include "stdafx.h"
#include ".\ccursorstatesprite.h"
#include "CCursor.h"
#include "../CTDrawImpl.h"
CCursorStateSprite::CCursorStateSprite(void)
{
	m_iState = CCursor::STATE_SPRITE;
	setDelayedLoad(0);
	m_hTexture = loadTexture ( "3DData\\Control\\Res\\Cursor.dds", 
								"3DData\\Control\\Res\\Cursor.dds",
								1, 
								0 );
	setDelayedLoad(1);
	assert( m_hTexture );
}

CCursorStateSprite::~CCursorStateSprite(void)
{
	if( m_hTexture )
	{
		unloadTexture( m_hTexture );
		m_hTexture = NULL;
	}
}

void CCursorStateSprite::Draw( POINT ptMouse )
{
	if( m_hTexture == NULL )
		return;


	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)ptMouse.x,	(float)ptMouse.y, 0.0f );
	
	::setTransformSprite( mat );
	::drawSprite( m_hTexture, 
					NULL,
					NULL,
					&D3DXVECTOR3( 0, 0, 0 ),								
					D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
}

int  CCursorStateSprite::SetCursorType( int iType )
{ 
	return 1; 
}

void CCursorStateSprite::Enter()
{
//	showCursor( NULL );
}

void CCursorStateSprite::Leave()
{
}
