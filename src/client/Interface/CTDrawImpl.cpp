#include "stdafx.h"
#include "CTDrawImpl.h"
#include "IO_ImageRes.h"

CTDrawImpl g_DrawImpl;

CTDrawImpl::CTDrawImpl()
{
	// Construct
}


CTDrawImpl::~CTDrawImpl()
{
	// Destruct
}

/// 2D version
void CTDrawImpl::Draw( int iX, int iY, int iModuleID, int iGraphicID )
{
	Draw( iX, iY, iModuleID, iGraphicID, D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
}

void CTDrawImpl::Draw( int iX, int iY, int iModuleID, int iGraphicID, D3DCOLOR color )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo failed" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo  && "Get SpriteInfo failed");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;

	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed");
		return;
	}
	
	// Transform	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)iX, (float)iY, 0 );
	::setTransformSprite( mat );

	::drawSprite( hTexture, 
				&(pSpriteInfo->m_Rect),
				NULL,
				&D3DXVECTOR3( 0, 0 , 0 ),								
				color );
}

void CTDrawImpl::Draw( int iX, int iY, int iWidth, int iModuleID, int iGraphicID, D3DCOLOR color )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo failed" );
		return;
	}
	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo && "Get SpriteInfo failed" );
		return;
	}
	HNODE hTexture = pTextureInfo->m_Texture;

	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed.." );
		return;
	}
	
	// Transform	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)iX, (float)iY, 0 );

	RECT rcDraw = pSpriteInfo->m_Rect;
	rcDraw.right = rcDraw.left + iWidth;	

	::setTransformSprite( mat );

	::drawSprite( hTexture, 
				&(rcDraw),
				NULL,
				&D3DXVECTOR3( 0, 0 , 0 ),								
				color );

}

void CTDrawImpl::Draw( int iX, int iY, int iWidth, int iModuleID, int iGraphicID )
{
	Draw( iX, iY, iWidth, iModuleID, iGraphicID, D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
}



/// 3D version
void CTDrawImpl::Draw( float iX, float iY, float iZ, int iModuleID, int iGraphicID , D3DCOLOR color)
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo fail" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo && "Get SpriteInfo fail");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;
	
	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed.." );
		return;
	}

	// Transform	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, iX, iY, iZ );
	
	::setTransformSprite( mat );

	::drawSprite( hTexture, 
				&(pSpriteInfo->m_Rect),
				NULL,
				&D3DXVECTOR3( 0, 0 , 0 ),								
				color );
}

void CTDrawImpl::Draw( float iX, float iY, float iZ, int iModuleID, int iGraphicID )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo fail" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo && "Get SpriteInfo fail");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;
	
	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed.." );
		return;
	}

	// Transform	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, iX, iY, iZ );
	
	::setTransformSprite( mat );

	::drawSprite( hTexture, 
				&(pSpriteInfo->m_Rect),
				NULL,
				&D3DXVECTOR3( 0, 0 , 0 ),								
				D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
}

void CTDrawImpl::DrawRotateY( float fX, float fY, float fZ , float fRotateY, int iModuleID, int iGraphicID )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo fail" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo && "Get SpriteInfo fail");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;
	
	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed.." );
		return;
	}

	D3DXMATRIX matPos, mat;
	D3DXMatrixRotationY( &mat, fRotateY );
	D3DXMatrixTranslation( &matPos, fX, fY, fZ );
	D3DXMatrixMultiply( &mat, &mat, &matPos );
	::setTransformSprite( mat);

	::drawSprite( hTexture, 
				&(pSpriteInfo->m_Rect),
				NULL,
				&D3DXVECTOR3( 0, 0 , 0 ),								
				D3DCOLOR_ARGB(255,255,255,255) );
}

void CTDrawImpl::Draw( float iX, float iY, float iZ,int iWidth, int iModuleID, int iGraphicID )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );

	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed");
		return;
	}
	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );
    
    if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo failed");
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo && "Get SpriteInfo failed" );
		return;
	}
	
	HNODE hTexture = pTextureInfo->m_Texture;
	
	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed.." );
		return;
	}

	// Transform	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, iX, iY, iZ );

	RECT rcDraw = pSpriteInfo->m_Rect;
	rcDraw.right = rcDraw.left + iWidth;

	::setTransformSprite( mat );

	::drawSprite( hTexture, 
				&(rcDraw),
				NULL,
				&D3DXVECTOR3( 0, 0 , 0 ),								
				D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
}

void CTDrawImpl::Draw( int iX, int iY, int iModuleID, int iGraphicID, float fScaleWidth , float fScaleHeight , D3DCOLOR color )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo failed" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo  && "Get SpriteInfo failed");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;

	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed");
		return;
	}
	
	// Transform	
	D3DXMATRIX mat;	

	D3DXVECTOR2 scale(fScaleWidth, fScaleHeight );
	D3DXVECTOR2 pos( (float)iX,(float)iY );
	D3DXMatrixTransformation2D(&mat, NULL, NULL, &scale, NULL, NULL, &pos);

	::setTransformSprite( mat );

	::drawSprite( hTexture, 
				&( pSpriteInfo->m_Rect ),
				NULL,
				&D3DXVECTOR3( 0, 0 , 0 ),								
				color );

	D3DXMatrixIdentity( &mat );
	::setTransformSprite( mat );
}

void CTDrawImpl::Draw( int iX, int iY, int iModuleID, int iGraphicID, int iWidth, float fScaleWidth , float fScaleHeight , D3DCOLOR color )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo failed" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo  && "Get SpriteInfo failed");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;

	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed");
		return;
	}

	// Transform	
	D3DXMATRIX mat;	

	D3DXVECTOR2 scale(fScaleWidth, fScaleHeight );
	D3DXVECTOR2 pos( (float)iX,(float)iY );
	D3DXMatrixTransformation2D(&mat, NULL, NULL, &scale, NULL, NULL, &pos);

	::setTransformSprite( mat );

	RECT rcDraw = pSpriteInfo->m_Rect;
	rcDraw.right = rcDraw.left + iWidth;

	::drawSprite( hTexture, 
		&( rcDraw ),
		NULL,
		&D3DXVECTOR3( 0, 0 , 0 ),								
		color );
}

void CTDrawImpl::DrawFit( int iX, int iY, int iModuleID, int iGraphicID, int iWidth, int iHeight, D3DCOLOR color )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo failed" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo  && "Get SpriteInfo failed");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;

	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed");
		return;
	}

	// Transform	
	D3DXMATRIX mat;	
	float fScaleWidth = (float)iWidth / (pSpriteInfo->m_Rect.right - pSpriteInfo->m_Rect.left);
	float fScaleHeight = (float)iHeight / (pSpriteInfo->m_Rect.bottom - pSpriteInfo->m_Rect.top);
	

	D3DXVECTOR2 scale(fScaleWidth, fScaleHeight );
	D3DXVECTOR2 pos( (float)iX,(float)iY );
	D3DXMatrixTransformation2D(&mat, NULL, NULL, &scale, NULL, NULL, &pos);

	::setTransformSprite( mat );

	::drawSprite( hTexture, 
		&( pSpriteInfo->m_Rect ),
		NULL,
		&D3DXVECTOR3( 0, 0 , 0 ),								
		color );
}

void CTDrawImpl::DrawFitW( int iX, int iY, int iModuleID, int iGraphicID, int iWidth, D3DCOLOR color )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo failed" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo  && "Get SpriteInfo failed");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;

	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed");
		return;
	}

	// Transform	
	D3DXMATRIX mat;	
	float fScaleWidth = (float)iWidth / (pSpriteInfo->m_Rect.right - pSpriteInfo->m_Rect.left);

	D3DXVECTOR2 scale(fScaleWidth, 1 );
	D3DXVECTOR2 pos( (float)iX,(float)iY );
	D3DXMatrixTransformation2D(&mat, NULL, NULL, &scale, NULL, NULL, &pos);

	::setTransformSprite( mat );

	::drawSprite( hTexture, 
		&( pSpriteInfo->m_Rect ),
		NULL,
		&D3DXVECTOR3( 0, 0 , 0 ),								
		color );
}

void CTDrawImpl::DrawFitH( int iX, int iY, int iModuleID, int iGraphicID, int iHeight, D3DCOLOR color )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo failed" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo  && "Get SpriteInfo failed");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;

	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed");
		return;
	}

	// Transform	
	D3DXMATRIX mat;	
	float fScaleHeight = (float)iHeight / (pSpriteInfo->m_Rect.bottom - pSpriteInfo->m_Rect.top);


	D3DXVECTOR2 scale( 1, fScaleHeight );
	D3DXVECTOR2 pos( (float)iX,(float)iY );
	D3DXMatrixTransformation2D(&mat, NULL, NULL, &scale, NULL, NULL, &pos);

	::setTransformSprite( mat );

	::drawSprite( hTexture, 
		&( pSpriteInfo->m_Rect ),
		NULL,
		&D3DXVECTOR3( 0, 0 , 0 ),								
		color );
}
//------------------------------------------------------------------------------------------------------------
/// @param color : 배경 아이콘 칼라
/// @param coveColor : 게이지 컬러
/// @brief 진행 게이지가 있는 아이콘
//------------------------------------------------------------------------------------------------------------
void CTDrawImpl::DrawCover( int iX, int iY, int iModuleID, int iGraphicID, D3DCOLOR color, D3DCOLOR coverColor, float value )
{
	CImageRes* pImageRes = (CImageResManager::GetSingleton()).GetImageRes( iModuleID );
	if( pImageRes == NULL )
	{
		assert( pImageRes && "Get ImageRes failed" );
		return;
	}

	stTexture* pTextureInfo = pImageRes->GetTexture( iGraphicID );
	stSprite* pSpriteInfo = pImageRes->GetSprite( iGraphicID );

	if( pTextureInfo == NULL )
	{
		assert( pTextureInfo && "Get TextureInfo fail" );
		return;
	}

	if( pSpriteInfo == NULL )
	{
		assert( pSpriteInfo && "Get SpriteInfo fail");
		return;
	}

	HNODE hTexture = pTextureInfo->m_Texture;
	
	if( hTexture == NULL )
	{
		assert( hTexture && "Get Texture failed.." );
		return;
	}

	// Transform	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, 0, 0, 0 );	
	::setTransformSprite( mat );

	::drawSpriteCover( hTexture, 
				&(pSpriteInfo->m_Rect),
				NULL,
				&D3DXVECTOR3( iX, iY , 0 ),								
				color,
				coverColor,
				value );

	D3DXMatrixTranslation( &mat, iX, iY, 0 );	
	::setTransformSprite( mat );
}

int CTDrawImpl::GetScreenWidth()
{
	return getScreenWidth();
}

int CTDrawImpl::GetScreenHeight()
{
	return getScreenHeight();
}