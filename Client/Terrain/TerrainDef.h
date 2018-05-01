#ifndef _TERRAIN_DEF_
#define _TERRAIN_DEF_


//-------------------------------------------------------------------------------------------------

#define	MAGNIFICATION_RATE			1


#define	MAP_COUNT_PER_ZONE_AXIS		64


#define	MAX_VERTEX_COUNT_PER_PATCH_AXIS		33

#define	GRID_COUNT_PER_PATCH_AXIS	g_pTerrain->GridCountPerPatchAXIS()
#define	GRID_COUNT_PER_MAP_AXIS		( GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS )
#define	VERTEX_COUNT_PER_MAP_AXIS	( PATCH_COUNT_PER_MAP_AXIS * GRID_COUNT_PER_PATCH_AXIS + 1 )

#define	PATCH_COUNT_PER_MAP_AXIS	16

#define	iMAP_SIZE					g_pTerrain->iMap_SIZE()
#define	fPATCH_SIZE					g_pTerrain->fPatch_SIZE()
#define	nPATCH_SIZE					g_pTerrain->nPatch_SIZE()

#define	fGRID_SIZE					g_pTerrain->fGrid_SIZE()
#define	nGRID_SIZE					g_pTerrain->nGrid_SIZE()

#define MANAGED_MAP_WIDTH			3	/// 한번에 관리되는 맵 개수( 가로크기 )

#endif //_TERRAIN_DEF_