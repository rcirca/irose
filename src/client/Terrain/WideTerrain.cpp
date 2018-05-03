#include "stdafx.h"
#include ".\wideterrain.h"
#include "IO_TERRAIN.h"
#include "../CClientStorage.h"


static tPOINT16 s_WideSecAdjPos[  25 ] = 
{
	{ -2, -2 }, { -1, -2 }, { 0, -2 }, { 1, -2 }, { 2, -2 },	// 0  ~ 4
	{ -2, -1 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { 2, -1 },	// 5  ~ 9
	{ -2,  0 }, { -1,  0 }, { 0,  0 }, { 1,  0 }, { 2,  0 },	// 10 ~ 14
	{ -2,  1 }, { -1,  1 }, { 0,  1 }, { 1,  1 }, { 2,  1 },	// 15 ~ 19
	{ -2,  2 }, { -1,  2 }, { 0,  2 }, { 1,  2 }, { 2,  2 },	// 20 ~ 24
/*
	0	1	2	3	4
	5	6	7	8	9
	10	11	12	13	14
	15	16	17	18	19
	20	21	22	23	24
*/

} ;

//static char	s_WideAddSecIdx[ 11 ][ 27 ] = 
//{
//	{	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 12, -1 },	// 0x00		all
//	{	0,  5, 10, 15, 20, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x01		left
//	{	4,  9, 14, 19, 24, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x02		right
//	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x03	
//	{	0,  1,  2,  3,  4, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x04		up
//	{	0,  1,  2,  3,  4,  5, 10, 15, 20, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x05		left & up
//	{	0,  1,  2,  3,  4,  9, 14, 19, 24, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x06		right & up	
//	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x07
//	{  20, 21, 22, 23, 24, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x08		down
//	{   0,  5, 10, 15, 20, 21, 22, 23, 24, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x09		left & down
//	{	4,  9, 14, 19, 20, 21, 22, 23, 24, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x0a		right & down	
//} ;
//
//static char s_WideSubSecIdx[ 11 ][ 27 ] = 
//{
//	{	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 12, -1 },	// 0x00		all
//	{	4,  9, 14, 19, 24, -1  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x01		left	
//	{	0,  5, 10, 15, 20, -1  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x02		right
//	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x03	
//	{  20, 21, 22, 23, 24, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x04		up
//	{	4,  9, 14, 19, 20, 21, 22, 23, 24, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x05		left & up
//	{   0,  5, 10, 15, 20, 21, 22, 23, 24, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x06		right & up
//	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x07
//	{	0,  1,  2,  3,  4, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x08		down
//	{	0,  1,  2,  3,  4,  9, 14, 19, 24, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x09		left & down
//	{	0,  1,  2,  3,  4,  5, 10, 15, 20, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x0a		right & down	
//} ;

static char	s_WideAddSecIdx[ 11 ][ 26 ] = 
{
	{	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, -1 },	// 0x00		all
	{	0,  5, 10, 15, 20, 13  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x01		left
	{	4,  9, 14, 19, 24, 11  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x02		right
	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x03	
	{	0,  1,  2,  3,  4, 17, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x04		up
	{	0,  1,  2,  3,  4,  5, 10, 15, 20, 18, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x05		left & up
	{	0,  1,  2,  3,  4,  9, 14, 19, 24, 16, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x06		right & up	
	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x07
	{  20, 21, 22, 23, 24,  7, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x08		down
	{   0,  5, 10, 15, 20, 21, 22, 23, 24,  6, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x09		left & down
	{	4,  9, 14, 19, 20, 21, 22, 23, 24,  8, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x0a		right & down	
} ;

static char s_WideSubSecIdx[ 11 ][ 26 ] = 
{
	{	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, -1 },	// 0x00		all
	{	4,  9, 14, 19, 24, 11  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x01		left	
	{	0,  5, 10, 15, 20, 13  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x02		right
	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x03	
	{  20, 21, 22, 23, 24,  7, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x04		up
	{	4,  9, 14, 19, 20, 21, 22, 23, 24,  8, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x05		left & up
	{   0,  5, 10, 15, 20, 21, 22, 23, 24,  6, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x06		right & up
	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x07
	{	0,  1,  2,  3,  4, 17, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x08		down
	{	0,  1,  2,  3,  4,  9, 14, 19, 24, 16, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x09		left & down
	{	0,  1,  2,  3,  4,  5, 10, 15, 20, 18, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x0a		right & down	
} ;

//static char	s_WideAddSecIdx[ 11 ][ 17 ] = 
//{
//	{	0,  1,  2,  3,  4,  5,  9, 10, 14, 15, 19, 20, 21, 22, 23, 24, -1 },	// 0x00		all
//	{	0,  5, 10, 15, 20,  9, 14, 19, -1,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x01		left
//	{	4,  9, 14, 19, 24,  5, 10, 15, -1,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x02		right
//	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x03	
//	{	0,  1,  2,  3,  4, 21, 22, 23, -1,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x04		up
//	{	0,  1,  2,  3,  4,  5, 10, 15, 20, 14, 19, 22, 23, 24, -1,  0, -1 },	// 0x05		left & up
//	{	0,  1,  2,  3,  4,  9, 14, 19, 24, 10, 15, 20, 21, 22, -1,  0, -1 },	// 0x06		right & up	
//	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x07
//	{  20, 21, 22, 23, 24,  1,  2,  3, -1,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x08		down
//	{	0,  5, 10, 15, 20, 21, 22, 23, 24,  2,  3,  4,  9, 14, -1,  0, -1 },	// 0x09		left & down
//	{	4,  9, 14, 19, 20, 21, 22, 23, 24,  0,  1,  2,  5, 10, -1,  0, -1 },	// 0x0a		right & down	
//} ;
//
//static char s_WideSubSecIdx[ 11 ][ 17 ] = 
//{
//	{	0,  1,  2,  3,  4,  5,  9, 10, 14, 15, 19, 20, 21, 22, 23, 24, -1 },	// 0x00		all
//	{	4,  9, 14, 19, 24,  5, 10, 15, -1,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x01		left	
//	{	0,  5, 10, 15, 20,  9, 14, 19, -1,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x02		right
//	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x03	
//	{  20, 21, 22, 23, 24,  1,  2,  3, -1,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x04		up
//	{	4,  9, 14, 19, 20, 21, 22, 23, 24,  0,  1,  2,  5, 10, -1,  0, -1 },	// 0x05		left & up
//	{	0,  5, 10, 15, 20, 21, 22, 23, 24,  2,  3,  4,  9, 14, -1,  0, -1 },	// 0x06		right & up
//	{  -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x07
//	{	0,  1,  2,  3,  4, 21, 22, 23, -1,  0,  0,  0,  0,  0,  0,  0, -1 },	// 0x08		down
//	{	0,  1,  2,  3,  4,  9, 14, 19, 24, 10, 15, 20, 21, 22, -1,  0, -1 },	// 0x09		left & down
//	{	0,  1,  2,  3,  4,  5, 10, 15, 20, 14, 19, 22, 23, 24, -1,  0, -1 },	// 0x0a		right & down	
//} ;


CWideTerrain::CWideTerrain(void)
{
	m_nCenterMapXIDX = 0;
	m_nCenterMapYIDX = 0;
}

CWideTerrain::~CWideTerrain(void)
{
}

void CWideTerrain::SetZoneDir( char* pZoneDir )
{
	m_ZoneDIR.Set( pZoneDir );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 주어진 위치로 맵이름을 구합니다.
//----------------------------------------------------------------------------------------------------

char* CWideTerrain::GetMapFILE( short nMapX, short nMapY )
{
	if ( nMapX < 0 || nMapX >= MAP_COUNT_PER_ZONE_AXIS )
		return NULL;
	if ( nMapY < 0 || nMapY >= MAP_COUNT_PER_ZONE_AXIS )
		return NULL;

	char *szMapFile = CStr::Printf ("%s%d_%d", m_ZoneDIR.Get(), nMapX, MAP_COUNT_PER_ZONE_AXIS-nMapY);
	m_MapFileName.Set( szMapFile );

	return m_MapFileName.Get();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 맵 풀에서 비어있는 데이터 노드가 있는지 찾는다.
///        없다면 새로 만들어서 풀에 집어 넣어라( 예약된 개수보다 많아질 경우겠지.. )
//----------------------------------------------------------------------------------------------------

ROUGHMAP_DATA*  CWideTerrain::GetEmptyRoughMap()
{
	ROUGHMAP_DATA* pMapData = NULL;

	std::list< ROUGHMAP_DATA* >::iterator		begin = m_RoughMapVec.begin();
	for( ; begin != m_RoughMapVec.end() ; ++begin )
	{
		pMapData = *begin;
		
		if( pMapData->m_bLoaded == false )
			return pMapData;		
	}

	/// 현재 비어있는 노드가 없다면
	pMapData = new ROUGHMAP_DATA;
	pMapData->m_bLoaded = false;

	m_RoughMapVec.push_back( pMapData );

	return pMapData;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 이미 로드된 맵인지 확인합니다.
//----------------------------------------------------------------------------------------------------

ROUGHMAP_DATA*	CWideTerrain::FindLoadedMAP( char* szMapFile )
{
	if( szMapFile == NULL )
		return NULL;

	char* strMapName = NULL;

	std::list< ROUGHMAP_DATA* >::iterator		begin = m_RoughMapVec.begin();
	for( ; begin != m_RoughMapVec.end() ; ++begin )
	{
		ROUGHMAP_DATA* pMapData = *begin;
		strMapName = (char*)( pMapData->m_strMapFileName.c_str() );

		if( strMapName == NULL )
			continue;

		if( strcmp( strMapName, szMapFile ) == 0 )
			return pMapData;
	}

	return NULL;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  적절한 RoughMap 을 추가한다.
//----------------------------------------------------------------------------------------------------

void CWideTerrain::AddRoughMap( short nCenterMapXIDX, short nCenterMapYIDX, WORD wUpdateFLAG )
{
	if( g_ClientStorage.GetVideoPerformance() < 5 )
		return;

	m_nCenterMapXIDX = nCenterMapXIDX;
	m_nCenterMapYIDX = nCenterMapYIDX;

	short nZoneMapXIDX			= 0;
	short nZoneMapYIDX			= 0;

	char* szMapFile				= NULL;
	ROUGHMAP_DATA*	pMapData	= NULL;

	for ( short nI=0; s_WideAddSecIdx[ wUpdateFLAG ][ nI ] >= 0; nI++ ) 
	{
		nZoneMapXIDX = nCenterMapXIDX + s_WideSecAdjPos[ s_WideAddSecIdx[ wUpdateFLAG ][ nI ] ].m_nX;
		nZoneMapYIDX = nCenterMapYIDX + s_WideSecAdjPos[ s_WideAddSecIdx[ wUpdateFLAG ][ nI ] ].m_nY;

		if ( nZoneMapXIDX >= 0 && nZoneMapXIDX < MAP_COUNT_PER_ZONE_AXIS &&
				nZoneMapYIDX >= 0 && nZoneMapYIDX < MAP_COUNT_PER_ZONE_AXIS ) 
		{
			szMapFile = GetMapFILE( nZoneMapXIDX, nZoneMapYIDX );
			if ( szMapFile ) 
			{
				pMapData = this->FindLoadedMAP ( szMapFile );			
				/// 맵이 로드되지 않았다면..
				if ( pMapData == NULL ) 
				{				
					pMapData = this->GetEmptyRoughMap ();
					if ( pMapData ) 
					{						
						if ( szMapFile ) 
						{		
							if( pMapData->m_RoughMap.Load( szMapFile, nZoneMapXIDX, nZoneMapYIDX ) )
							{
								pMapData->m_strMapFileName = std::string( szMapFile );
								pMapData->m_bLoaded = true;

								LogString ( LOG_NORMAL, "Wide AddMAP: (%d, %d) --> ( %d, %d ) \n", s_WideSecAdjPos[ s_WideAddSecIdx[ wUpdateFLAG ][ nI ] ].m_nX, s_WideSecAdjPos[ s_WideAddSecIdx[ wUpdateFLAG ][ nI ] ].m_nY, nZoneMapXIDX, nZoneMapYIDX);
							}else
								LogString ( LOG_NORMAL, "FAILED Wide AddMAP: (%d, %d) --> ( %d, %d ) \n", s_WideSecAdjPos[ s_WideAddSecIdx[ wUpdateFLAG ][ nI ] ].m_nX, s_WideSecAdjPos[ s_WideAddSecIdx[ wUpdateFLAG ][ nI ] ].m_nY, nZoneMapXIDX, nZoneMapYIDX);
						} 
					}
					else
					{
						assert( 0 && "GetEmptyMAP() returns invalid map[ AddMap ]" );
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Expired된 맵들을 내린다.
//----------------------------------------------------------------------------------------------------

void CWideTerrain::SubRoughMap( WORD wUpdateFLAG )
{
	short nI					= 0;
	short nZoneMapXIDX			= 0;
	short nZoneMapYIDX			= 0;

	char* szMapFile				= NULL;
	ROUGHMAP_DATA *pMapData		= NULL;

	for (nI=0; s_WideSubSecIdx[ wUpdateFLAG ][ nI ]>=0; nI++) 
	{
		nZoneMapXIDX = m_nCenterMapXIDX + s_WideSecAdjPos[ s_WideSubSecIdx[ wUpdateFLAG ][ nI ] ].m_nX;
		nZoneMapYIDX = m_nCenterMapYIDX + s_WideSecAdjPos[ s_WideSubSecIdx[ wUpdateFLAG ][ nI ] ].m_nY;

		if ( nZoneMapXIDX >= 0 && nZoneMapXIDX < MAP_COUNT_PER_ZONE_AXIS &&
			 nZoneMapYIDX >= 0 && nZoneMapYIDX < MAP_COUNT_PER_ZONE_AXIS ) 
		{

			szMapFile = GetMapFILE( nZoneMapXIDX, nZoneMapYIDX );
			if ( szMapFile ) 
			{
				pMapData = this->FindLoadedMAP ( szMapFile );
				if ( pMapData ) 
				{
					LogString (LOG_NORMAL, "	Wide FreeRoughMAP: (%d, %d) --> ( %d, %d )\n", 
											s_WideSecAdjPos[ s_WideSubSecIdx[ wUpdateFLAG ][ nI ] ].m_nX, 
											s_WideSecAdjPos[ s_WideSubSecIdx[ wUpdateFLAG ][ nI ] ].m_nY, 
											nZoneMapXIDX, nZoneMapYIDX);
					pMapData->m_RoughMap.Free ();

					pMapData->m_bLoaded = false;
				}			
			}
		}
	}	
}