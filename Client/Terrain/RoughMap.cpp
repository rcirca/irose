#include "stdafx.h"
#include ".\roughMap.h"
#include "IO_Terrain.h"
#include "../Interface/it_Mgr.h"


CRoughMap::CRoughMap(void)
{
	m_hTerrainBlock		= NULL;
	
	/// 높이맵 정보
	iWidth				= 0;
	iHeight				= 0;

	m_pHeight			= NULL;

	m_iDetailLevel		= 4;
}

CRoughMap::~CRoughMap(void)
{
}

void CRoughMap::SetWorldOffset( float fX, float fY )
{
	m_fWorldOffsetX = fX;
	m_fWorldOffsetY = fY;
}

void CRoughMap::SetDetailLevel( int iLevel )
{
	m_iDetailLevel = iLevel;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 높이맵과 매핑 텍스쳐 로드
//----------------------------------------------------------------------------------------------------

bool CRoughMap::Load( char* szMapName, int iZoneMapX, int iZoneMapY )
{
	m_fWorldOffsetX = iZoneMapX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS;
	m_fWorldOffsetY = iZoneMapY * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS;

	///////////////////////////////////////////////////////////////////////////////
	/// HeightsMap info file
	///////////////////////////////////////////////////////////////////////////////
	char *szFullPathName = CStr::Printf ("%s.HIM", szMapName);

	/// Load file
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szFullPathName ) == false )	
	{	
		/// 원래 없을 수도 있다.
		//char *szStr = CStr::Printf ("File [%s] open error ", szFullPathName );
		//g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
		return false;
	}	


	int iWidth = 0, iHeight = 0;
	pFileSystem->ReadInt32( &iWidth );	// iWidth = patch cnt * 4 + 1
	pFileSystem->ReadInt32( &iHeight );

	// appended 03. 07. 25
	int iPatchGridCnt = 0;
	float fOnePatchSize = 0;
	pFileSystem->ReadInt32( &iPatchGridCnt );
	pFileSystem->ReadFloat( &fOnePatchSize );
	

	int iW, iH;
	float fHeight;
	
	/////
	if( iWidth != VERTEX_COUNT_PER_MAP_AXIS )
	{
		iWidth = VERTEX_COUNT_PER_MAP_AXIS;
		g_itMGR.OpenMsgBox( "유효하지 않은 맵 데이터" );
	}

	if( iHeight != VERTEX_COUNT_PER_MAP_AXIS )
	{
		iHeight = VERTEX_COUNT_PER_MAP_AXIS;
		g_itMGR.OpenMsgBox( "유효하지 않은 맵 데이터" );
	}

	m_pHeight = new float [ VERTEX_COUNT_PER_MAP_AXIS * VERTEX_COUNT_PER_MAP_AXIS ];
	for (iH=0; iH<iHeight; iH++) 
	{			
		for (iW=0; iW<iWidth; iW++) 
		{
			pFileSystem->ReadFloat( &fHeight );
			m_pHeight[ iH * VERTEX_COUNT_PER_MAP_AXIS + iW ] = fHeight;// * 0.01f * nGRID_SIZE;
		}
	}

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );


	if( LoadEngineObject( szMapName, iZoneMapX, iZoneMapY ) == false )
		return false;

	InsertToEngine();

	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 맵 프리
//----------------------------------------------------------------------------------------------------

void CRoughMap::Free( )
{
	RemoveFromEngine ();
	ReleaseEngineObject();	

	if ( m_pHeight ) 
	{		
		SAFE_DELETE_ARRAY( m_pHeight );
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CRoughMap::LoadEngineObject( char* szMapName, int iZoneMapX, int iZoneMapY )
{
	char *szFullPathName = CStr::Printf ("%sPlanMap.dds", szMapName );	
	
	/// Load file
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->IsExist( szFullPathName ) == false )	
	{
		szFullPathName = "3DData\\Terrain\\default_light.dds";
	}

/*ZZ_DLL
HNODE loadTerrainBlockRough (
	ZSTRING pTerrainBlockName,
	float fMinMax[2][3],
	int iWidth,
	float * pHeightList,
	ZSTRING pTexturePath,
	HNODE hRoughTerrainShader,
	HNODE hLight
);	*/	
	
	char* pBlockName = CStr::Printf ("%s_Terrain_Block", szMapName );
	if( pBlockName == NULL )
		return false;

	float fMapSize = PATCH_COUNT_PER_MAP_AXIS*fPATCH_SIZE;
	float fMinMax[2][3] = { { m_fWorldOffsetX, m_fWorldOffsetY, -10000.0f },
							{ m_fWorldOffsetX + fMapSize, m_fWorldOffsetY + fMapSize, 10000.0f } };

	m_hTerrainBlock = ::loadTerrainBlockRough( pBlockName,
											fMinMax,										
											VERTEX_COUNT_PER_MAP_AXIS,
											m_pHeight,
											szFullPathName,											
											g_GameDATA.m_hShader_terrain_Rough,
											g_GameDATA.m_hLight );
	if( m_hTerrainBlock == NULL )
	{
		g_pCApp->ErrorBOX( "loadTerrainBlockExt Error", "ERROR" );
		return false;
	}	
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CRoughMap::ReleaseEngineObject( )
{
	if( m_hTerrainBlock )
	{
		::unloadTerrainBlock ( m_hTerrainBlock );
		m_hTerrainBlock = NULL;
	}	
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CRoughMap::InsertToEngine()
{
	if( m_hTerrainBlock )
		::insertToScene( m_hTerrainBlock );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CRoughMap::RemoveFromEngine()
{
	if( m_hTerrainBlock )
		::removeFromScene( m_hTerrainBlock );
}