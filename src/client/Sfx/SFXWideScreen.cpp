#include "stdafx.h"
#include ".\sfxwidescreen.h"

CSFXWideScreen::CSFXWideScreen(void)
{
	m_iType = SFX_WIDE_SCREEN;

	m_hSFXTexture = 0;

	m_iWidth = 0;
	m_iHeight = 0;
}

CSFXWideScreen::~CSFXWideScreen(void)
{
}

void CSFXWideScreen::SetScreenSize( int iWidth, int iHeight )
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
}

bool CSFXWideScreen::Init()
{
	//if( m_hSFXTexture )
	//{
	//	::unloadTexture( m_hSFXTexture );
	//}

	//m_hSFXTexture = ::loadTexture( "3Ddata\\CUTSCENE\\WARP01\\screen_black01.dds", "3Ddata\\CUTSCENE\\WARP01\\screen_black01.dds", 1, 0 );
	//if( m_hSFXTexture == 0 )
	//{
	//	#ifdef _DEBUG
	//		g_pCApp->ErrorBOX( "CSFXWideScreen load failed !!", "File open error" );
	//	#endif
	//		//실패한 이유를 적어준다..
	//	return false;
	//}


	///
	/// 엔진의 뷰포트 변환기능으로 와이드 스크린효과..
	///
	PlayWideScreen( 9.0f / 16.0f );

	return true;
}

void CSFXWideScreen::Release()
{
	/*if( m_hSFXTexture )
	{
		::unloadTexture( m_hSFXTexture );
		m_hSFXTexture = 0;
	}*/

	///
	/// 엔진의 뷰포트 변환기능으로 와이드 스크린효과..
	///
	StopWideScreen();
}

void CSFXWideScreen::Update()
{
}

void CSFXWideScreen::Draw()
{
	//if( m_iWidth == 0 || m_iHeight == 0 )
	//	return;

	//assert( m_hSFXTexture != 0 );
	//if( m_hSFXTexture == 0 )
	//	return;

	////// Transform
	//D3DXMATRIX mat;		
	//D3DXMatrixTranslation( &mat, 100, 
	//							100 , 0.0f );

	//D3DXMatrixScaling( &mat, m_iWidth / 2, 50.0f, 1.0f );

	//::setTransformSprite( mat );
	//::drawSprite(	m_hSFXTexture, 
	//				NULL,
	//				NULL,
	//				&D3DXVECTOR3( 0, 0, 0 ),								
	//				D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
	
}