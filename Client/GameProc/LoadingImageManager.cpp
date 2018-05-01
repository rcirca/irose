#include "stdafx.h"
#include ".\loadingimagemanager.h"

#include <algorithm>

CLoadingImageManager::CLoadingImageManager(void)
{
	m_bDisplayEventLoadingImage = false;
}

CLoadingImageManager::~CLoadingImageManager(void)
{
	/// 행성별 로딩 이미지 테이블
	m_LoadingImageTableByEvent.clear();

	/// 행성별 로딩 이미지 테이블
	m_LoadingImageTableByPlanet.clear();    

	/// 존별 로딩 이미지 테이블
	m_LoadingImageTableByZone.clear();
}

//------------------------------------------------------------------------
/// List_Loading.sTB 로 부터 이미지 테이블을 생성한다.
//------------------------------------------------------------------------
bool CLoadingImageManager::LoadImageTable( char* strSTBName )
{
	CGameSTB fSTB;
	int iImageZoneNO = 0;

	if ( fSTB.Open ( strSTBName ) ) 
	{
		int iImageCNT = fSTB.GetRowCount();

		/// 이벤트용 로딩 이미지를 출력해야하는가?
		int iIsEventLoading = fSTB.GetInteger( 1, 0 );
		if( iIsEventLoading == 0 && fSTB.GetString( 0, 0 ) )
		{
			m_bDisplayEventLoadingImage = true;
		}
		
		
		for ( int i=0 ; i<iImageCNT; i++) 
		{			
			iImageZoneNO = fSTB.GetInteger( 1, i );
			const char* szFileName = fSTB.GetString( 0, i );
			if ( szFileName ) 
			{
				/// 이벤트용 출력 이미지라면..
				if( m_bDisplayEventLoadingImage )
				{
					m_LoadingImageTableByEvent.push_back( szFileName );
				}else
				{
					/// 행성
					if( iImageZoneNO > 500 )
					{

						m_LoadingImageTableByPlanet.insert( std::make_pair< int, std::string >( iImageZoneNO - 500,  szFileName ) );
					}else
					{
						/// 존
						m_LoadingImageTableByZone.insert( std::make_pair< int, std::string >( iImageZoneNO,  szFileName ) );
					}
				}
			}else
			{
				assert( 0 && "Invalid loading image zone NO" );
			}
		}
		fSTB.Close ();

		return true;
	}

	return false;
}

//------------------------------------------------------------------------
/// 해당 존에서 출력해야할 로딩이미지를 판단해서 리턴한다.
//------------------------------------------------------------------------
HNODE CLoadingImageManager::GetLoadingImage( int iZoneNO, int iPlanetNO )
{
	HNODE hTexture = 0;

	/// 이벤트용 이미지를 출력해야하는가?
	if( m_bDisplayEventLoadingImage )
	{
		int iImageCount = m_LoadingImageTableByEvent.size();
		int iRefNO = RANDOM( iImageCount );

		hTexture = ::loadTexture(	m_LoadingImageTableByEvent[ iRefNO ].c_str(), 
									m_LoadingImageTableByEvent[ iRefNO ].c_str(), 
									1,
									0 );

		return hTexture;
	}
	
	/// 먼저 존테이블에 있는지 찾는다.
	if( m_LoadingImageTableByZone.find( iZoneNO ) != m_LoadingImageTableByZone.end() )
	{
		hTexture = GetLoadingImageFromTable( m_LoadingImageTableByZone, iZoneNO );
		return hTexture;
	}

	/// 존테이블에 없다면 행성 테이블을 찾는다.
	if( m_LoadingImageTableByPlanet.find( iPlanetNO ) != m_LoadingImageTableByPlanet.end() )
	{
		hTexture = GetLoadingImageFromTable( m_LoadingImageTableByPlanet, iPlanetNO );
		return hTexture;
	}

	return 0;
}

HNODE CLoadingImageManager::GetLoadingImageFromTable( std::multimap< int, std::string >& imageTable, int iKey )
{
	std::multimap< int, std::string >::iterator itorLow, itorUpper;

	itorLow		= imageTable.lower_bound( iKey );
	itorUpper	= imageTable.upper_bound( iKey );

	int iDistance = std::distance( itorLow, itorUpper );
	if( iDistance > 1 )
	{
		int iRefNO = RANDOM( iDistance );
		std::advance( itorLow, iRefNO );			
	}

	HNODE hTexture = ::loadTexture( itorLow->second.c_str(), itorLow->second.c_str(), 1, 0 ); 
	return hTexture;
}

//------------------------------------------------------------------------
///
//------------------------------------------------------------------------
void CLoadingImageManager::ReleaseTexture( HNODE hTexture )
{
	if( hTexture )
	{
		::unloadTexture( hTexture );
		hTexture = 0;
	}
}