#ifndef _WIDE_TERRAIN_
#define _WIDE_TERRAIN_


#include "RoughMap.h"
#include <list>
#include <string>

struct ROUGHMAP_DATA
{	
	bool				m_bLoaded;
	std::string			m_strMapFileName;
	CRoughMap			m_RoughMap;	
};

//----------------------------------------------------------------------------------------------------
/// class CWideTerrain
/// 광대역 지형관리를 위한 클래스
//----------------------------------------------------------------------------------------------------

class CWideTerrain
{
	CStrVAR							m_ZoneDIR;
	/// 맵파일 이름 임시 저장소..
	CStrVAR							m_MapFileName;
	/// roughmap 풀
	std::list< ROUGHMAP_DATA* >	m_RoughMapVec;


	/// 현재 중심맵
	short							m_nCenterMapXIDX;
	short							m_nCenterMapYIDX;

public:
	CWideTerrain(void);
	~CWideTerrain(void);

	void			SetZoneDir( char* pZoneDir );
	char*			GetMapFILE( short nMapX, short nMapY );

	ROUGHMAP_DATA*  GetEmptyRoughMap();
	ROUGHMAP_DATA*	FindLoadedMAP( char* szMapFile );

	void			AddRoughMap( short nCenterMapXIDX, short nCenterMapYIDX, WORD wUpdateFLAG );
	void			SubRoughMap( WORD wUpdateFLAG );
};


#endif //_WIDE_TERRAIN_
