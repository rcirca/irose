#ifndef _LOADING_IMAGE_MANAGER_
#define _LOADING_IMAGE_MANAGER_

#include <map>
#include <vector>

/**
* 각 존별 혹은 행성별.. 상황에 따른 로딩이미지 교체 관리..
*
* @Author		이동현
* @Date			2005/9/5
*/ 

class CLoadingImageManager
{
public:
	CLoadingImageManager(void);
	~CLoadingImageManager(void);

	bool	LoadImageTable( char* strSTBName );
	HNODE	GetLoadingImage( int iZoneNO, int iPlanetNO );
	void	ReleaseTexture( HNODE hTexture );

private:

	/// 이벤트용 로딩 이미지를 출력해야하는가..
	bool	m_bDisplayEventLoadingImage;


	/// 행성별 로딩 이미지 테이블
	std::vector< std::string >			m_LoadingImageTableByEvent;
	/// 행성별 로딩 이미지 테이블
	std::multimap< int, std::string >	m_LoadingImageTableByPlanet;    
	/// 존별 로딩 이미지 테이블
	std::multimap< int, std::string >	m_LoadingImageTableByZone;



	HNODE	GetLoadingImageFromTable( std::multimap< int, std::string >& imageTable, int iKey );
};

#endif //_LOADING_IMAGE_MANAGER_