/*
	$Header: /Client/IO_Terrain.h 100   05-01-26 1:54p Navy $
*/
#ifndef	__IO_TERRAIN_H
#define	__IO_TERRAIN_H
#include "IO_Basic.h"
#include "IO_Material.h"
#include "IO_Effect.h"
#include "Common\IO_STB.h"
#include "Sound\IO_Sound.h"
#include "Object.h"
#include "Common\CRegenAREA.h"
#include "Common\CEconomy.h"

#include "Util\\CFileSystem.h"
#include "Terrain\WideTerrain.h"
#include "Terrain\PatchManager.h"
#include "Terrain/TerrainDef.h"



#include <list>



class CQuadPatchManager;
class CMAP_PATCH;
class CMAP;
class CTERRAIN;


//----------------------------------------------------------------------------------------------------
/// struct tagMAPSND
//----------------------------------------------------------------------------------------------------

struct tagMAPSND 
{
	t_HASHKEY		m_HashKEY;
	D3DVECTOR		m_Position;
	DWORD			m_dwLastPlayTIME;
	unsigned int	m_uiInterval;
	unsigned int	m_uiRange;
} ;

//----------------------------------------------------------------------------------------------------
/// struct tagMAPEFT
//----------------------------------------------------------------------------------------------------

struct tagMAPEFT 
{
//	CStrVAR			m_EffectFile;
	t_HASHKEY		m_HashKEY;
	CEffect		   *m_pEffect;
	D3DXVECTOR3		m_Positon;
} ;

//----------------------------------------------------------------------------------------------------
/// Cylinder information for mob collision
//----------------------------------------------------------------------------------------------------

struct tagCYLINDERINFO
{
	D3DXVECTOR3		m_Position;
	float			m_fRadius;
};



struct aabbBox{

    float x[2];
	float y[2];
	float z[2];

};


struct QuadPatch{

        short  level;
        short  index_x[2];
		short  index_y[2];
		short  iquad_size;
		float  fquad_size;
		float  nQuadXFromW,nQuadYFromW;
		int    nQuadXFromMap,nQuadYFromMap;
		aabbBox aabb;

};

//----------------------------------------------------------------------------------------------------
///
/// class CQuadPatchManager
///
//----------------------------------------------------------------------------------------------------

struct PatchIndex{

     int x,y;

};


class CQuadPatchManager{

    
   public  :

     CMAP *m_pMap;
	   
	 QuadPatch m_quadPATCH[85];
     classDLLIST<PatchIndex> m_ExPatchList[86];
	 CMAP_PATCH* m_ppPATCH[ PATCH_COUNT_PER_MAP_AXIS*PATCH_COUNT_PER_MAP_AXIS ];
	 CMAP_PATCH* m_ppExPATCH[ PATCH_COUNT_PER_MAP_AXIS*PATCH_COUNT_PER_MAP_AXIS ];
     CMAP_PATCH* m_ppPickPATCH[ PATCH_COUNT_PER_MAP_AXIS*PATCH_COUNT_PER_MAP_AXIS ];	

	 short m_nPATCH,m_nExPATCH,m_nPickPATCH;
	   
	 float m_ViewFrsutumEq[6][4];
	 
	 float m_RayPosition[3];
	 float m_RayDirection[3];
	  
	 CQuadPatchManager(void);
     ~CQuadPatchManager(void);
     void InitializeQuadPatch();
     void InputQuadPatchElement(short index, short level,int iquad_size, short nMapX, short nMapY);
     void LoadQuadPatch(CMAP *pMap);
     void CalculateViewFrustumCulling();
     void CalculateQuadPatchCulling(short index);
     void InsertMemoQuadPatch(short index);
     void CalculateBottomNode(short index);
     short ViewCullingFunc(aabbBox* box);
	 short ViewCullingFunc2(D3DVECTOR *min,D3DVECTOR *max);
	 void SetExPatch(int index,CMAP_PATCH *pPatch,int patch_x,int patch_y);
	 void SetExPatchBottom(int index);
     void SetExPatchTotal();
     void InsertMemoQuadExPatch(short index);
	 void InsertMemoQuadExPatch2(short index);
	 void CalculateQaudExPatchCullingEx();
	 void GetViewFrustumEq(float viewEquation[6][4]);
	 void CalculateRayCulling(short index);
	 bool IntersectRayOnOff(aabbBox* box);
     void CalculateRayCullingBottomNode(short index);
	 void CalculatePickingPATCH();
	 void GetPickingRay(float Position[3], float Direction[3]);
};



//----------------------------------------------------------------------------------------------------
///
/// class CMAP_PATCH
///
//----------------------------------------------------------------------------------------------------

class CMAP_PATCH {
private :
	static	float	m_fHEIGHT[ MAX_VERTEX_COUNT_PER_PATCH_AXIS * MAX_VERTEX_COUNT_PER_PATCH_AXIS ];

	HNODE	m_hTerrainBlock;

	short	m_nTileNO;
	short	m_nDnTileTEX;
	short	m_nUpTileTEX;

	CMAP   *m_pMAP;
	

	bool	m_bIsRegistered;
	bool	m_bIsVisible;

	bool	Register (short nWorldX, short nWorldY);

public  :
	classDLLIST< CGameOBJ*	> m_FixObjLIST;
	classDLLIST< tagMAPSND	> m_SoundLIST;
	classDLLIST< tagMAPEFT  > m_EffectLIST;
	classDLLIST< tagCYLINDERINFO  > m_CylinderLIST;



	short	m_nPatchXFromMAP;
	short	m_nPatchYFromMAP;


	
	/// AABB
	D3DVECTOR	m_AABBMin;
	D3DVECTOR	m_AABBMax;



	CMAP_PATCH ();

	void	InitMember();

	HNODE	GetVISIBLE ()			{	return m_hTerrainBlock;		}
	CMAP   *GetParentMAP (void)		{	return m_pMAP;			}

	void	SetMATERIAL (short nTileNO);
	void	SetPOSITION (CMAP *pMAP, short nPatchXFromMap, short nPatchYFromMap);
	bool	RegisterToNZIN (void);
	bool	UnregisterFromNZIN (void);

	WORD	m_wLastViewFRAME;

	void	InsertToScene ();
	void	RemoveFromScene ();

	bool	IsIntersect (float &fCurMinDist, D3DXVECTOR3 &PosOUT);
	bool    IsIntersect_test(float &fCurMinDist, D3DXVECTOR3 &PosOUT,float Position[3], float Direction[3]);
	void	PlaySOUND ();
	bool	AddObject ( CGameOBJ *pObject, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale );
//	float	GetHEIGHT (float fWorldX, float fWorldY, HNODE hCheckNODE);

	int		GetUpTileNO(){ return m_nUpTileTEX; }

	
	aabbBox   m_aabb;
	int   m_DrawingType;                          //  추가
    int   m_PreDrawingType;                       //  추가 
    bool  ExPatchEnable; 
    bool  ObjectOnOff;

	bool	MakeAABBFromObject( CGameOBJ * pObject, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale );
    void    TransformOBB2AABB(float *obb_min,float *obb_max,D3DXMATRIX *object_m,float *bb_min,float *bb_max);
    void    CompareSizePath2Obj();

} ;


//-------------------------------------------------------------------------------------------------
enum MAP_LUMP_TYPE {
	LUMP_MAPINFO = 0,
	LUMP_TERRAIN_OBJECT,
	LUMP_TERRAIN_MOB,
	LUMP_TERRAIN_CNST,
	LUMP_TERRAIN_SOUND,
	LUMP_TERRAIN_EFFECT,
	LUMP_TERRAIN_MORPH,
	LUMP_TERRAIN_WATER,
	LUMP_TERRAIN_REGEN,
	LUMP_TERRAIN_OCEAN,
	LUMP_TERRAIN_WARP,
	LUMP_TERRAIN_COLLISION,
	LUMP_TERRAIN_EVENT_OBJECT,
	LUMP_MAP_MAX
} ;
enum {
	MAP_EMPTY = 0,
	MAP_USING,
	MAP_DIRTY
} ;


//----------------------------------------------------------------------------------------------------
///
/// Map class. One map container..
///
//----------------------------------------------------------------------------------------------------

class CMAP {
private :
	static int	   *m_pObjectIndex[ LUMP_MAP_MAX ];
	HNODE			m_hLightMAP;
	short			m_nUseMODE;
	short			m_nXPatchCnt;
	short			m_nYPatchCnt;

	CMAP_PATCH		m_PATCH[ PATCH_COUNT_PER_MAP_AXIS ][ PATCH_COUNT_PER_MAP_AXIS ];
	                                                          //   추가

	float		  **m_ppHeight;//[ VERTEX_COUNT_PER_MAP_AXIS ][ VERTEX_COUNT_PER_MAP_AXIS ];

	float           m_fPatchHeightMN[PATCH_COUNT_PER_MAP_AXIS*PATCH_COUNT_PER_MAP_AXIS][2];     //    추가
	float           m_fQuadPatchHeightMN[85][2];                                                //    추가
	
	
	classDLLIST< HNODE >	m_OceanLIST;
	classDLLIST< HNODE >	m_OceanMatLIST;

	void	ReadMapINFO		(CFileSystem* pFileSystem, long lOffset);
	void	ReadOceanINFO	(CFileSystem* pFileSystem, long lOffset);
	void	ReadObjINFO		(CFileSystem* pFileSystem, long lOffset, int iType);
	bool	LoadLightMapINFO(int iLumpType, char *szLightFileINF, char *szPath);

	void	GetHeightsByCoordinates (float fWorldX, float fWorldY, float fHeights[4], float * pfWeightHoriz = NULL, float * pfWeightVert = NULL);	
	POINT	GetMapIndexFromWorld( float fWorldX, float fWorldY );
	
public  :
	CMAP ();
	~CMAP ();

	short	m_nZoneMapXIDX;
	short	m_nZoneMapYIDX;

    
	void	SetUseMODE(short nMode)		{	m_nUseMODE = nMode;				}
	bool	IsEmpty ()					{	return (m_nUseMODE==MAP_EMPTY);	}
	bool	IsUsing ()					{	return (m_nUseMODE==MAP_USING);	}
	bool	IsDirty ()					{	return (m_nUseMODE==MAP_DIRTY);	}

	HNODE	LightMAP()		{	return m_hLightMAP;				}

	CMAP*	Load (char *szFileName, short nZoneMapXIDX, short nZoneMapYIDX);
	void	Free (void);
	void	ClearFromNZIN (void);

	float	GetVertexHEIGHT (short nMapX, short nMapY)		{	return m_ppHeight[ nMapY ][ nMapX ];	}

	float	GetHEIGHT (float fWorldX, float fWorldY);
	float	GetSlope (float fWorldX, float fWorldY);
	void	GetNormal (float fWorldX, float fWorldY, D3DXVECTOR3& vNormal);
	void	MappingToZONE (short nXFrom3x3, short nYFrom3x3);

#ifdef	__VIRTUAL_SERVER
	classDLLNODE< CRegenAREA >	*m_pRegenNODE;

	int		RegenCHAR (CRegenPOINT *pRegenPOINT, int iCharIDX, float fXPos, float fYPos);
#endif

	CMAP_PATCH *GetPATCH(int iX, int iY)	{	return &m_PATCH[ iY ][ iX ];	}
	CQuadPatchManager *GetQaudManager()     {   return &m_QuadManager;          }
	bool	AddObject (int iObjectIndex, D3DXVECTOR3 &Position, D3DXQUATERNION &Rotate, D3DXVECTOR3 &Scale );
	void	AddSound (D3DVECTOR &Position, char *szSoundFile, unsigned int uiInterval, unsigned uiRange);
	void	AddEffect(D3DVECTOR &Position, char *szEffectFile);

public:
	///
	/// 현재 월드 위치로 해당 패치를 구하기
	///	
	CMAP_PATCH*		GetPatchFromWorld( float fWorldX, float fWorldY );
    CQuadPatchManager m_QuadManager;   

    void CompareSizePath2ObjAll();


	///
	/// 맵 로딩후 쿼드트리 재구성이 필요한 패치들
	///	
	std::list< CMAP_PATCH* >	m_PatchUpdateList;
	//CChatBox* pBox = NULL;

	//std::list< CChatBox* >::iterator begin = m_ActiveChatBoxList.begin();

	//for(; begin != m_ActiveChatBoxList.end() ; ++begin )
	//{
	//	pBox = *begin;

	//	/// 이미 다른 말중이라면.. 갱신..
	//	if( iCharIndex == pBox->GetChatIndex() )
	//	{
	//		pBox->SetMember( iCharIndex, Msg, Color );			
	//		return;
	//	}
	//}
	/// 다사용후에는 m_patchUpdateList.clear()
} ;


//-------------------------------------------------------------------------------------------------

#define	TILETBL_LAYER1(I)				g_pTerrain->GetTileTBL(0,I)
#define	TILETBL_LAYER2(I)				g_pTerrain->GetTileTBL(1,I)
#define	TILETBL_DNOFF(I)				g_pTerrain->GetTileTBL(2,I)
#define	TILETBL_UPOFF(I)				g_pTerrain->GetTileTBL(3,I)
#define	TILETBL_ISBLENDING(I)			g_pTerrain->GetTileTBL(4,I)
#define	TILETBL_PUTTYPE(I)				g_pTerrain->GetTileTBL(5,I)


enum ZONE_LUMP_TYPE
{
	LUMP_ZONE_INFO = 0,
	LUMP_EVENT_OBJECT,
	LUMP_ZONE_TILE,
	LUMP_TILE_TYPE,
	LUMP_ECONOMY,
	LUMP_ZONE_MAX
} ;

struct tagEVENT_POS {
	D3DVECTOR	m_Position;
	CStrVAR		m_Name;
} ;

/// 한프레임에 하나의 맵만을 읽기위한 구조.
struct tagLOAD_ONE_MAP_DATA
{
	short		m_nCenterX;
	short		m_nCenterY;
	short		m_nUpdateIndex;
	bool		m_bImmediateLoad;
};



//----------------------------------------------------------------------------------------------------
///
/// class CTERRAIN
/// 전체 지형을 관리합니다. 9장의 실제 맵을 읽어 들이고 패치들을 엔진에 등록하거나 갱신합니다.
///
//----------------------------------------------------------------------------------------------------

#define	MAX_MAP_BUFFER	( 3*3+6 )
class CTERRAIN 
{

private :	
	CRITICAL_SECTION	 m_csZONE;	

	/// 개선된 광대역 지형관리를 위한 클래스..
	CWideTerrain		m_WideTerrain;

	/// 패치 관리를 위한 클래스


	short		m_nZoneNO;
	CStrVAR		m_ZoneDIR;
	CStrVAR		m_ZoneNAME;
	CStrVAR		m_MusicFile;
	CStrVAR		m_SkyNAME;
	CStrVAR		m_MapFileName;
	int			m_iIsDungeon;

//	STBDATA		m_TileTBL;
	CMatLIST   *m_pTILE;
	int			m_iTileTypeCNT;
	short	  **m_ppTileTYPE;

	short		m_nGridPerPATCH;
	int			m_iMapSIZE;
	float		m_fPatchSIZE;
	short		m_nPatchSIZE;

	float		m_fGridSIZE;
	short		m_nGridSIZE;

	WORD        m_wViewFRAME;
	
	std::list< CMAP* >		m_DirtyMapList;
	CMAP	   *m_pMapBUFFER[ MAX_MAP_BUFFER ];
	CMAP	   *m_pMAPS  [ MAP_COUNT_PER_ZONE_AXIS    ][ MAP_COUNT_PER_ZONE_AXIS ];
    

	classDLLIST< CRegenAREA  >	m_RegenLIST;

	short	m_nCenterMapXIDX;
	short	m_nCenterMapYIDX;

	tagEVENT_POS*m_pEventPOS;
	int			 m_iEventPosCNT;

	void	ReadZoneINFO (CFileSystem* pFileSystem, long lOffset);
	void	ReadEventObjINFO (CFileSystem* pFileSystem, long lOffset);
	void	ReadTileINFO( CFileSystem* pFileSystem, long lOffset );
	void	ReadTileTYPE( CFileSystem* pFileSystem, long lOffset );
	bool	ReadECONOMY( CFileSystem* pFileSystem, long lOffset );

	char*	GetMapFILE(short nMapX, short nMapY);

	CMAP*	GetEmptyMAP ();
	CMAP*	FindLoadedMAP (short nZoneMapXIDX, short nZoneMapYIDX);
	void	FreeDirtyMAP ();

	/// 한번에는 하나의 맵만 읽기 위한 ....
	std::list< tagLOAD_ONE_MAP_DATA* >	m_LoadOneMapData;
	tagLOAD_ONE_MAP_DATA* GetLodingMapFromList( short nCenterMapXIDX, short nCenterMapYIDX, WORD wUpdateIndex );
	bool	AddOneMap( short nCenterMapXIDX, short nCenterMapYIDX, WORD wUpdateIndex );

	bool	AddMAP (short nZoneMapX, short nZoneMapY, WORD wUpdateFLAG, bool bAllUpdate=true, bool bImmediateLoad = false );
	void	SubMAP (WORD wUpdateFLAG);
	CMAP*	GetMapPTR (short nZoneMapX, short nZoneMapY);
	void	SetMapPTR (short nZoneMapX, short nZoneMapY, CMAP* pMAP);

	void	ReOrginazationPatch( short nZoneMapX, short nZoneMapY );
	void	Update_VisiblePatch( short nMappingX, short nMappingY );
	void	Insert_VisiblePatch (CMAP_PATCH *pPATCH);
	void	Clear_VisiblePatch (void);
	void	Delete_UnvisiblePatch (void);
	
	static CTERRAIN	*m_pInstance;

	CMAP* GetMapPtrByWorldCoordinates (float fWorldX, float fWorldY);

	/// Data loading mode change
	///bool	m_bLoadImmediate;


	CTERRAIN ();
	~CTERRAIN ()		{	if ( m_pInstance )	m_pInstance->FreeZONE();	}

public  :
	
	CPatchManager		m_PatchManager;
	
	CEconomy			m_Economy;
	int					m_iObjCreateOrder;

	static CTERRAIN *Instance ();
	void	Destroy ()	{	SAFE_DELETE( m_pInstance );				}


	void	Lock ()		{	;	}
	void	Unlock ()	{	;	}


	/// 2004/3/31:인자 추가 nAvy
	///임시로 로그인화면에서 사용되는 Zone Load시에는 해당 BGM을 Play하지 않기 위하여 bPlayBGM 추가
	bool	LoadZONE (short nZoneNO,bool bPlayBGM = true);
	void	FreeZONE ();
	bool	InitZONE (float fCenterX, float fCenterY);

	short	GridCountPerPatchAXIS()	{	return m_nGridPerPATCH;		}
	int		iMap_SIZE()				{	return m_iMapSIZE;			}
	float	fPatch_SIZE()			{	return m_fPatchSIZE;		}
	short	nPatch_SIZE()			{	return m_nPatchSIZE;		}

	float	fGrid_SIZE()			{	return m_fGridSIZE;			}
	short	nGrid_SIZE()			{	return m_nGridSIZE;			}

	void	Add_RegenAREA( classDLLNODE< CRegenAREA > *pNode);
	void	Del_RegenAREA( classDLLNODE< CRegenAREA > *pNode);
	void	Proc_RegenAREA (void);
	void	Proc_Ecomony (void);

	tPOINTF	Get_EventPOS( char *szName );
	tPOINTF Get_StartPOS ();
	tPOINTF Get_RevivePOS ();

	int		GetVisiblePatchCount ()				{	return m_PatchManager.GetVisiblePatchCount();		}

//	short	GetTileTBL(short nX, short nY)		{	return m_TileTBL.m_ppDATA[ nY ][ nX ];	}
	short	GetTileTBL(short nX, short nY)		{	return m_ppTileTYPE[ nY ][ nX ];		}
	HNODE	GetMATERIAL(short nIndex)			{	return m_pTILE->IDX_HNODE( nIndex );	}
	HNODE	GetLIGHTMAP(char *szFullPathName);

	void		SetPATCH (int iX, int iY, CMAP_PATCH *pPATCH)	{	m_PatchManager.SetPATCH( iX, iY, pPATCH );	}
	CMAP_PATCH* GetPATCH (int iZoneX, int iZoneY);


	void	SetCenterPosition (float fWorldX, float fWorldY);
	//float	GetHEIGHT (float fWorldX, float fWorldY, bool &bStepOnObject, HNODE hCheckNODE=NULL);
	//float GetTerrainHeight (float fWorldX, float fWorldY, bool& bNotFound);

	/// 지형의 단순 높이를 리턴함.
	/// @bNotFound 해당 좌표에 지형이 세팅되어 있지 않은 경우에는 FALSE 값이 설정.
	float GetHeight (float fWorldX, float fWorldY, bool * pNotFound = NULL);

	/// 현재 좌표의 가장 높은 오브젝트의 위치를 반환.
	/// 오브젝트가 하나도 없는 경우에는 pNotFound 가 TRUE이며, 지형 높이가 리턴됨.
	/// 오브젝트의 높이가 지형보다 낮은 경우에는 지형의 높이가 선택됨.
	/// @bNotFound 해당 좌표에 지형이 세팅되어 있지 않은 경우에는 TRUE 값이 설정.
	float GetHeightTop (float fWorldX, float fWorldY, bool * pNotFound = NULL);

	/// 현재 좌표의 지형에 대한 기울기를 반환. 0(평면)에서 1(45도)사이의 값.
	/// @bNotFound 해당 좌표에 지형이 세팅되어 있지 않은 경우에는 TRUE 값이 설정.
	float GetSlope (float fWorldX, float fWorldY, bool * bNotFound);

	/// 현재 좌표의 지형에 대한 노말 벡터를 계산.
	/// @vNormal 정규화된 노말 벡터. 평면이라면, (0, 0, 1)
	/// @return 해당 좌표에 대한 지형이 세팅되어 있지 않은 경우에는 false 를 리턴.
	bool GetNormal (float fWorldX, float fWorldY, D3DXVECTOR3& vNormal);

	float	Pick_POSITION (D3DXVECTOR3 &PosPICK);
	float   Pick_POSITION_Test (D3DXVECTOR3 &PosPICK);
	short	Pick_NearObject( D3DXVECTOR3 &PosPICK, float &fDistance );
	short	Pick_OBJECT (D3DXVECTOR3 &PosPICK, float &fDistance);	

	/// Get/Set loading mode
	///void	SetLoadingMode( bool bImmediateMode ){ m_bLoadImmediate = bImmediateMode; };
	///bool	IsLoadingImmediateMode( ){ return m_bLoadImmediate; };


	int		GetZoneNO(){ return m_nZoneNO; }
	bool    IsPVPZone();
	bool	IsAgitZone();

	///
	/// 현재 월드 위치로 해당 패치를 구하기
	///
	CMAP_PATCH*	GetPatchFromWorld( float fWorldX, float fWorldY );

    void UpdatePatchManager(short nCenterMapXIDX, short nCenterMapYIDX);
	void InsertCameraPatch(float xChar,float yChar);



	//----------------------------------------------------------------------------------------------------	
	/// @brief Wide terrain
	//----------------------------------------------------------------------------------------------------
	void SubWideMap( WORD wUpdateFLAG );



	/// 현재 등록된 패치 개수
    static int		m_RegistedPatchCnt;

} ;

extern CTERRAIN	*g_pTerrain;

//-------------------------------------------------------------------------------------------------
#endif
