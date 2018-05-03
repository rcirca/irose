#include "stdafx.h"
#include "CLoading.h"
#include "Game.h"

CLoading _Loading;

CLoading::CLoading()
{
	m_bInitLoadingImageManager	= false;
	m_hTexture					= NULL;
}

CLoading::~CLoading()
{
	// Destruct
}

bool CLoading::InitLoadingImageManager()
{
	if( m_LoadingImageManager.LoadImageTable( "3DData\\STB\\LIST_LOADING.STB" ) )
	{
		m_bInitLoadingImageManager = true;
		return true;
	}
	return false;
}

bool CLoading::LoadTexture( int iZoneNo, int iPlanetNO )
{
	if( !m_bInitLoadingImageManager )
	{
		if( InitLoadingImageManager() == false )
		{			
			return false;
		}
	}

	if( m_hTexture )
		UnloadTexture();

	setDelayedLoad(0);

	if( iZoneNo == 9999 )
	{	
			m_hTexture = loadTexture ( "3DData\\Control\\Res\\Loading.dds", 
									"3DData\\Control\\Res\\Loading.dds",
									1, 
									0 );
	}else
	{
		//------------------------------------------------------------------------------
		/// 2005/04/27 여러가지 로딩이미지를 상황에 맞게 출력하도록 수정
		//------------------------------------------------------------------------------
		m_hTexture = m_LoadingImageManager.GetLoadingImage( iZoneNo, iPlanetNO );
		if( m_hTexture == 0 )
		{
			m_hTexture = loadTexture ( "3DData\\Control\\Res\\Loading.dds", 
									"3DData\\Control\\Res\\Loading.dds",
									1, 
									0 );
		}
	}

	setDelayedLoad(1);
	if( m_hTexture == NULL )
	{
#ifdef _DEBUG
			g_pCApp->ErrorBOX( "Loading image load failed !!", "File open error" );
#endif
			//실패한 이유를 적어준다..
			return false;
	}    

	return true;
}

void CLoading::UnloadTexture()
{
	if( m_hTexture != NULL )
	{
		unloadTexture( m_hTexture );
		m_hTexture = NULL;
	}
}

void CLoading::Draw()
{
	if( m_hTexture == NULL )
		return;

	// Transform	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, 0,	0, 0.0f );
	int iWidth, iHeight;
	
	::getTextureSize( m_hTexture, iWidth, iHeight );
	
	if( iWidth != 0 && iHeight != 0 )
		D3DXMatrixScaling( &mat, (float)g_pCApp->GetWIDTH()/(float)iWidth, 
		(float)g_pCApp->GetHEIGHT()/(float)iHeight, 1.0f );
	
	::setTransformSprite( mat );
	::drawSprite( m_hTexture, 
					NULL,
					NULL,
					&D3DXVECTOR3( 0, 0, 0 ),								
					D3DCOLOR_RGBA( 255, 255, 255, 255 ) );

}

