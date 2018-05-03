/*
	$Header: /Client/IO_Terrain.cpp 294   05-10-27 10:48a Choo0219 $
*/
#include "stdAFX.h"
#include "Game.h"
#include "CObjFIXED.h"
#include "ObjFixedEvent.h"
#include "IO_Terrain.h"
#include "IO_Event.h"
#include "CCamera.h"
#include "System/CGame.h"
#include "interface\\it_MGR.h"
#include "Network\CNetwork.h"
#include "Util\\VFSManager.h"
#include "interface/dlgs/CMinimapDlg.h"

#include "Sound/MusicMgr.h"
#include "CClientStorage.h"

#include "GameProc/BGMManager.h"


#define	VIEW_PATCH_RANGE			8


#define MOVE_UPDATE_ALL			0x00
#define MOVE_UPDATE_LEFT		0x01
#define MOVE_UPDATE_RIGHT		0x02
#define MOVE_UPDATE_UP			0x04
#define MOVE_UPDATE_DOWN		0x08
#define MOVE_UPDATE_LU			( MOVE_UPDATE_LEFT  | MOVE_UPDATE_UP	)	// 0x05
#define MOVE_UPDATE_RU			( MOVE_UPDATE_RIGHT | MOVE_UPDATE_UP	)	// 0x06
#define MOVE_UPDATE_LD			( MOVE_UPDATE_LEFT  | MOVE_UPDATE_DOWN	)	// 0x09
#define MOVE_UPDATE_RD			( MOVE_UPDATE_RIGHT | MOVE_UPDATE_DOWN  )	// 0x0a		10


static tPOINT16 s_SecAdjPos[  9 ] = {
	{ -1, -1 }, { 0, -1 }, { 1, -1 },		// 0 ~ 2
	{ -1,  0 }, { 0,  0 }, { 1,  0 },		// 3 ~ 5
	{ -1,  1 }, { 0,  1 }, { 1,  1 }		// 6 ~ 8
} ;

static char	s_AddSecIdx[ 11 ][ 10 ] = {
	{	0, 1, 2, 3, 4, 5, 6, 7, 8,-1 },	// 0x00		all
//	{	4, -1, 7, 8,-1, 5, 6, 7, 8,-1 },	// 0x00		all
	{	0, 3, 6,-1, 0, 0, 0, 0, 0, 0 },	// 0x01		left
	{	2, 5, 8,-1, 0, 0, 0, 0, 0, 0 },	// 0x02		right
	{  -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	// 0x03
	{   0, 1, 2,-1, 0, 0, 0, 0, 0, 0 },	// 0x04		up
	{   0, 3, 6, 1, 2,-1, 0, 0, 0, 0 },	// 0x05		left & up
	{	2, 5, 8, 0, 1,-1, 0, 0, 0, 0 },	// 0x06		right & up
	{  -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	// 0x07
	{	6, 7, 8,-1, 0, 0, 0, 0, 0, 0 },	// 0x08		down
	{	0, 3, 6, 7, 8,-1, 0, 0, 0, 0 },	// 0x09		left & down
	{	2, 5, 8, 6, 7,-1, 0, 0, 0, 0 }	// 0x0a		right & down
} ;

static char s_SubSecIdx[ 11 ][ 10 ] = {
	{	0, 1, 2, 3, 4, 5, 6, 7, 8,-1 },	// 0x00		all
	{	2, 5, 8,-1, 0, 0, 0, 0, 0, 0 },	// 0x01		right
	{	0, 3, 6,-1, 0, 0, 0, 0, 0, 0 },	// 0x02		left
	{  -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	// 0x03
	{	6, 7, 8,-1, 0, 0, 0, 0, 0, 0 },	// 0x04		down
	{	2, 5, 8, 6, 7,-1, 0, 0, 0, 0 },	// 0x05		right & down
	{	0, 3, 6, 7, 8,-1, 0, 0, 0, 0 },	// 0x06		left & down
	{  -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	// 0x07
	{   0, 1, 2,-1, 0, 0, 0, 0, 0, 0 },	// 0x08		up
	{	2, 5, 8, 0, 1,-1, 0, 0, 0, 0 },	// 0x09		right & up
	{   0, 3, 6, 1, 2,-1, 0, 0, 0, 0 } 	// 0x0a		left & up
} ;




int CTERRAIN::m_RegistedPatchCnt = 0;








CQuadPatchManager::CQuadPatchManager(void)
{
  	InitializeQuadPatch();
}

CQuadPatchManager::~CQuadPatchManager(void)
{
  int i;
  
  for(i=0;i<86;i+=1)
  m_ExPatchList[i].ClearList();
  

}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  Top node에서 QuadTree구조 초기화 시작 
//----------------------------------------------------------------------------------------------------
void CQuadPatchManager::InitializeQuadPatch()
{
  InputQuadPatchElement(0,0,16,0,0);
  
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  CQuadPatchManager의 QuadTree구조의 초기화 
//----------------------------------------------------------------------------------------------------
void CQuadPatchManager::InputQuadPatchElement(short index, short level,int iquad_size, short nMapX, short nMapY) 
{
   
	if(level>=4)
    return;

	int next_iquad_size,next_level;
	next_iquad_size=iquad_size/2;
	next_level=level+1;

	m_quadPATCH[index].level=level;
	m_quadPATCH[index].iquad_size=iquad_size;
	m_quadPATCH[index].nQuadXFromMap=nMapX;
    m_quadPATCH[index].nQuadYFromMap=nMapY;
    m_quadPATCH[index].index_x[0]=nMapX;
    m_quadPATCH[index].index_y[0]=nMapY;
    m_quadPATCH[index].index_x[1]=nMapX+iquad_size-1;
    m_quadPATCH[index].index_y[1]=nMapY+iquad_size-1;
	
	InputQuadPatchElement(index*4+1, next_level, next_iquad_size, nMapX, nMapY);
    InputQuadPatchElement(index*4+2, next_level, next_iquad_size, nMapX+next_iquad_size, nMapY);
    InputQuadPatchElement(index*4+3, next_level, next_iquad_size, nMapX+next_iquad_size, nMapY+next_iquad_size);
    InputQuadPatchElement(index*4+4, next_level, next_iquad_size, nMapX, nMapY+next_iquad_size);
  
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief QuadTree구조에 CMAP* 의 정보값을 입력 
///        실제 맵의 좌표를 보고 각 노드들의 바운딩박스의 좌료를 실제 월드 좌표로 변환.
//----------------------------------------------------------------------------------------------------
void CQuadPatchManager::LoadQuadPatch(CMAP *pMap)
{
   
    int i,buffer_lengthX, buffer_lengthY;
	    
    m_pMap=pMap;
		
	buffer_lengthX=PATCH_COUNT_PER_MAP_AXIS* m_pMap->m_nZoneMapXIDX;
	buffer_lengthY=PATCH_COUNT_PER_MAP_AXIS* m_pMap->m_nZoneMapYIDX;
	
	for(i=0;i<85;i+=1)
	{
	    m_quadPATCH[i].fquad_size=m_quadPATCH[i].iquad_size*fPATCH_SIZE;
        m_quadPATCH[i].nQuadXFromW=buffer_lengthX+m_quadPATCH[i].nQuadXFromMap;
        m_quadPATCH[i].nQuadYFromW=buffer_lengthY+m_quadPATCH[i].nQuadYFromMap;
        m_quadPATCH[i].aabb.x[0]=m_quadPATCH[i].nQuadXFromW*fPATCH_SIZE;
		m_quadPATCH[i].aabb.x[1]=m_quadPATCH[i].aabb.x[0]+m_quadPATCH[i].fquad_size;
        m_quadPATCH[i].aabb.y[0]=m_quadPATCH[i].nQuadYFromW*fPATCH_SIZE;
		m_quadPATCH[i].aabb.y[1]=m_quadPATCH[i].aabb.y[0]+m_quadPATCH[i].fquad_size;
   	
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief   Top node에서 View Frsutum Culling을 시작 
//----------------------------------------------------------------------------------------------------

void CQuadPatchManager::CalculateViewFrustumCulling()
{
               
    m_nPATCH=0;
    m_nExPATCH=0;
	CalculateQuadPatchCulling(0);
    InsertMemoQuadExPatch2(85);
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  QuadTree의 View Frsutum Culling 
//----------------------------------------------------------------------------------------------------
void CQuadPatchManager::CalculateQuadPatchCulling(short index)
{
   
	short test_type;
	
	test_type=1;
     
    switch(ViewCullingFunc(&m_quadPATCH[index].aabb))
	{
	case 0:
        InsertMemoQuadPatch(index);
 	    InsertMemoQuadExPatch(index);
		break;
	case 1:
				
		InsertMemoQuadExPatch2(index);
		
		if(m_quadPATCH[index].level>=3)
           CalculateBottomNode(index);
		else
		{
	       CalculateQuadPatchCulling(4*index+1);
           CalculateQuadPatchCulling(4*index+2);
           CalculateQuadPatchCulling(4*index+3);
		   CalculateQuadPatchCulling(4*index+4);
		}

	}



}





//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  View Frsutum안쪽의 Patch를 얻음 
//----------------------------------------------------------------------------------------------------
void CQuadPatchManager::InsertMemoQuadPatch(short index)
{
   
   int i,j;
	
   for( i = m_quadPATCH[index].index_y[0] ; i <= m_quadPATCH[index].index_y[1] ; i+=1)
   {
	   for( j = m_quadPATCH[index].index_x[0] ; j <= m_quadPATCH[index].index_x[1] ; j+=1)
	   {
	     m_ppPATCH[m_nPATCH]=m_pMap->GetPATCH(j,i);
		 m_nPATCH+=1;
	   }

   }

  
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  QuadTree의 Bottom node를 처리
//----------------------------------------------------------------------------------------------------
void CQuadPatchManager::CalculateBottomNode(short index)
{
   
   int i,j;
   CMAP_PATCH *pPatch;
   short patch_type;
   
   for( i = m_quadPATCH[index].index_y[0] ; i <= m_quadPATCH[index].index_y[1] ; i+=1)
   {
	   for( j = m_quadPATCH[index].index_x[0] ; j <= m_quadPATCH[index].index_x[1] ; j+=1)
	   {
           pPatch=m_pMap->GetPATCH(j,i);
		   patch_type=ViewCullingFunc(&pPatch->m_aabb);
	         
			 if(patch_type==1 || patch_type==0)
			 {
 	           m_ppPATCH[m_nPATCH]=pPatch;
		       m_nPATCH+=1;
 
			 }
 
	   }

   }

  
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  AABB Box의 View Frsutum Culing 
//----------------------------------------------------------------------------------------------------
short CQuadPatchManager::ViewCullingFunc(aabbBox* box)
{
	///if( g_GameDATA.m_bJustObjectLoadMode )
		return 0;

   int i;
   float buffer_vector[3],buffer_vector2[3];


   for(i=0; i<4; i++) 
   { 
    buffer_vector[0] = m_ViewFrsutumEq[i][0] < 0 ? box->x[1] : box->x[0]; 
    buffer_vector[1] = m_ViewFrsutumEq[i][1] < 0 ? box->y[1] : box->y[0]; 
    buffer_vector[2] = m_ViewFrsutumEq[i][2] < 0 ? box->z[1] : box->z[0]; 

    buffer_vector2[0]=m_ViewFrsutumEq[i][0];
    buffer_vector2[1]=m_ViewFrsutumEq[i][1];
    buffer_vector2[2]=m_ViewFrsutumEq[i][2];

	
	if (((buffer_vector[0]*buffer_vector2[0]+buffer_vector[1]*buffer_vector2[1]+buffer_vector[2]*buffer_vector2[2])+m_ViewFrsutumEq[i][3]) > 0) 
        return 2;     //  View Frsutum 바깥에 있는 경우
   } 

   
   for(i=0; i<4; i++) 
   { 
    buffer_vector[0] = m_ViewFrsutumEq[i][0] > 0 ? box->x[1] : box->x[0]; 
    buffer_vector[1] = m_ViewFrsutumEq[i][1] > 0 ? box->y[1] : box->y[0]; 
    buffer_vector[2] = m_ViewFrsutumEq[i][2] > 0 ? box->z[1] : box->z[0]; 

    buffer_vector2[0]=m_ViewFrsutumEq[i][0];
    buffer_vector2[1]=m_ViewFrsutumEq[i][1];
    buffer_vector2[2]=m_ViewFrsutumEq[i][2];

	
	if (((buffer_vector[0]*buffer_vector2[0]+buffer_vector[1]*buffer_vector2[1]+buffer_vector[2]*buffer_vector2[2])+m_ViewFrsutumEq[i][3]) > 0) 
        return 1;            //  View Frsutum에 걸쳐 있는 경우  
   } 

 return 0;                  //  View Frsutum 안쪽에 있는 경우  
}  

short CQuadPatchManager::ViewCullingFunc2(D3DVECTOR *min,D3DVECTOR *max)
{
   

   int i;
   float buffer_vector[3],buffer_vector2[3];


   for(i=0; i<4; i++) 
   { 
    buffer_vector[0] = m_ViewFrsutumEq[i][0] < 0 ? max->x : min->x; 
    buffer_vector[1] = m_ViewFrsutumEq[i][1] < 0 ? max->y : min->y; 
    buffer_vector[2] = m_ViewFrsutumEq[i][2] < 0 ? max->z : min->z; 

    buffer_vector2[0]=m_ViewFrsutumEq[i][0];
    buffer_vector2[1]=m_ViewFrsutumEq[i][1];
    buffer_vector2[2]=m_ViewFrsutumEq[i][2];

	
	if (((buffer_vector[0]*buffer_vector2[0]+buffer_vector[1]*buffer_vector2[1]+buffer_vector[2]*buffer_vector2[2])+m_ViewFrsutumEq[i][3]) > 0) 
        return 2;     //  View Frsutum 바깥에 있는 경우
   } 

   
   for(i=0; i<4; i++) 
   { 
    buffer_vector[0] = m_ViewFrsutumEq[i][0] > 0 ? max->x : min->x; 
    buffer_vector[1] = m_ViewFrsutumEq[i][1] > 0 ? max->y : min->y; 
    buffer_vector[2] = m_ViewFrsutumEq[i][2] > 0 ? max->z : min->z; 

    buffer_vector2[0]=m_ViewFrsutumEq[i][0];
    buffer_vector2[1]=m_ViewFrsutumEq[i][1];
    buffer_vector2[2]=m_ViewFrsutumEq[i][2];

	
	if (((buffer_vector[0]*buffer_vector2[0]+buffer_vector[1]*buffer_vector2[1]+buffer_vector[2]*buffer_vector2[2])+m_ViewFrsutumEq[i][3]) > 0) 
        return 1;            //  View Frsutum에 걸쳐 있는 경우  
   } 

 return 0;                  //  View Frsutum 안쪽에 있는 경우  
}  


bool CQuadPatchManager::IntersectRayOnOff(aabbBox* box)
{
    float vMin[3], vMax[3];
     
	vMin[0] = box->x[0]; vMin[1] = box->y[0]; vMin[2] =box->z[0];
    vMax[0] = box->x[1]; vMax[1] = box->y[1]; vMax[2] =box->z[1];

	return false;//::intersectRayAABB(vMin, vMax, m_RayPosition[0], m_RayPosition[1], m_RayPosition[2], m_RayDirection[0], m_RayDirection[1], m_RayDirection[2]); 
}


void CQuadPatchManager::SetExPatchTotal()
{

  int i;

  for(i=0;i<86;i+=1)
  m_ExPatchList[i].ClearList();
  
  
  for(i=21;i<85;i+=1)
  SetExPatchBottom(i);


}

void CQuadPatchManager::SetExPatchBottom(int index)
{

   int i,j;
   CMAP_PATCH *pPatch;
   classDLLNODE<PatchIndex> *pNODE; 

   for( i = m_quadPATCH[index].index_y[0] ; i <= m_quadPATCH[index].index_y[1] ; i+=1)
   {
	   for( j = m_quadPATCH[index].index_x[0] ; j <= m_quadPATCH[index].index_x[1] ; j+=1)
	   {
           pPatch=m_pMap->GetPATCH(j,i);
		   if(pPatch->ExPatchEnable)
		   {
				if((m_quadPATCH[index].aabb.x[0]> pPatch->m_AABBMin.x) || (m_quadPATCH[index].aabb.x[1]< pPatch->m_AABBMax.x) || 
				(m_quadPATCH[index].aabb.y[0]> pPatch->m_AABBMin.y) || (m_quadPATCH[index].aabb.y[1]< pPatch->m_AABBMax.y))
				{
		          SetExPatch(index%4==0 ? index/4-1 : index/4,pPatch,j,i);
				}
				else
				{
				  
				  pNODE = m_ExPatchList[index].AllocNAppend();
				  pNODE->DATA.x=j;
				  pNODE->DATA.y=i;

				
				}
		   }
           
	   }

   }


}

void CQuadPatchManager::SetExPatch(int index,CMAP_PATCH *pPatch,int patch_x,int patch_y)
{

   int parent_index;
   classDLLNODE<PatchIndex> *pNODE; 

   if((m_quadPATCH[index].aabb.x[0] > pPatch->m_AABBMin.x) || (m_quadPATCH[index].aabb.x[1] < pPatch->m_AABBMax.x) || 
   (m_quadPATCH[index].aabb.y[0] > pPatch->m_AABBMin.y) || (m_quadPATCH[index].aabb.y[1] < pPatch->m_AABBMax.y))
   { 
	 if(index>0)
	 SetExPatch(index%4==0 ? index/4-1 : index/4,pPatch,patch_x,patch_y);
	 else
	 {
      pNODE = m_ExPatchList[85].AllocNAppend();
      pNODE->DATA.x=patch_x;
	  pNODE->DATA.y=patch_y;
	 
	 }
    
   }
   else
   {
	  
     pNODE = m_ExPatchList[index].AllocNAppend();
     pNODE->DATA.x=patch_x;
	 pNODE->DATA.y=patch_y;
   }

}


void CQuadPatchManager::InsertMemoQuadExPatch(short index)
{
   
 
   if(m_ExPatchList[index].GetNodeCount()>0)
   {
      classDLLNODE<PatchIndex> *pNODE; 
	
      pNODE= m_ExPatchList[index].GetHeadNode();

	  while(pNODE)
	  {
         m_ppExPATCH[m_nExPATCH]=m_pMap->GetPATCH(pNODE->DATA.x,pNODE->DATA.y);
		 m_nExPATCH+=1;

        pNODE = m_ExPatchList[index].GetNextNode(pNODE);
	  }


   }
   
}

void CQuadPatchManager::InsertMemoQuadExPatch2(short index)
{
   	
   CMAP_PATCH *pPatch;
   
   if(m_ExPatchList[index].GetNodeCount()>0)
   {
      classDLLNODE<PatchIndex> *pNODE; 
	
      pNODE= m_ExPatchList[index].GetHeadNode();

	  while(pNODE)
	  {
          pPatch=m_pMap->GetPATCH(pNODE->DATA.x,pNODE->DATA.y);
		 

		  if(ViewCullingFunc2(&pPatch->m_AABBMin,&pPatch->m_AABBMax)!=2)
		  {
		    m_ppExPATCH[m_nExPATCH]=m_pMap->GetPATCH(pNODE->DATA.x,pNODE->DATA.y);
		    m_nExPATCH+=1;
		  }
         pNODE = m_ExPatchList[index].GetNextNode(pNODE);
	  }


   }
   

}


void CQuadPatchManager::GetViewFrustumEq(float viewEquation[6][4])
{
   int i;
   
   for(i=0;i<6;i+=1)
   {
     
     m_ViewFrsutumEq[i][0]=viewEquation[i][0];
     m_ViewFrsutumEq[i][1]=viewEquation[i][1];
     m_ViewFrsutumEq[i][2]=viewEquation[i][2];
     m_ViewFrsutumEq[i][3]=viewEquation[i][3];
   }


}


void CQuadPatchManager::CalculateRayCulling(short index)
{
   
  
    if(IntersectRayOnOff(&m_quadPATCH[index].aabb))
	{
		if(m_quadPATCH[index].level>=3)
			CalculateRayCullingBottomNode(index);
		else
		{
	        CalculateRayCulling(4*index+1);
            CalculateRayCulling(4*index+2);
			CalculateRayCulling(4*index+3);
			CalculateRayCulling(4*index+4);
		}
    
		

	}
}
void CQuadPatchManager::CalculateRayCullingBottomNode(short index)
{

   int i,j;
   CMAP_PATCH *pPatch;
   short patch_type;
   
   for( i = m_quadPATCH[index].index_y[0] ; i <= m_quadPATCH[index].index_y[1] ; i+=1)
   {
	   for( j = m_quadPATCH[index].index_x[0] ; j <= m_quadPATCH[index].index_x[1] ; j+=1)
	   {
           pPatch=m_pMap->GetPATCH(j,i);
		   if(IntersectRayOnOff(&pPatch->m_aabb))
	       {
	           m_ppPickPATCH[m_nPickPATCH]=pPatch;
		       m_nPickPATCH+=1;
           }
 	   }

   }
}

void CQuadPatchManager::CalculatePickingPATCH()
{
               
   	m_nPickPATCH=0;
	CalculateRayCulling(0);
  
}

void CQuadPatchManager::GetPickingRay(float Position[3], float Direction[3])
{

     m_RayPosition[0] = Position[0]; m_RayPosition[1] = Position[1]; m_RayPosition[2] = Position[2];
     m_RayDirection[0] = Direction[0]; m_RayDirection[1] = Direction[1]; m_RayDirection[2] = Direction[2];
	
}

//-------------------------------------------------------------------------------------------------

float CMAP_PATCH::m_fHEIGHT[ MAX_VERTEX_COUNT_PER_PATCH_AXIS * MAX_VERTEX_COUNT_PER_PATCH_AXIS ];
int*  CMAP::m_pObjectIndex[ LUMP_MAP_MAX ];

//-------------------------------------------------------------------------------------------------
CMAP_PATCH::CMAP_PATCH ()
{	
	m_pMAP = NULL;
	m_PreDrawingType=-1;
	InitMember();
}

void CMAP_PATCH::InitMember()
{
	m_wLastViewFRAME = 0;

	m_hTerrainBlock = NULL;

	m_nTileNO = 0;
	m_nDnTileTEX = 0;;
	m_nUpTileTEX = 0;	

	m_bIsRegistered = false;
	m_bIsVisible = false;

	m_FixObjLIST.ClearList();
	m_SoundLIST.ClearList();
	m_EffectLIST.ClearList();
	m_CylinderLIST.ClearList();
}

bool CMAP_PATCH::IsIntersect (float &fCurMinDist, D3DXVECTOR3 &PosOUT)
{
	float fDistance;
	D3DXVECTOR3 PosPICK;
	D3DXVECTOR3 RayOrig;
	D3DXVECTOR3 RayDir;
	
	CGame::GetInstance().GetRayOrig(RayOrig);
	CGame::GetInstance().GetRayDir(RayDir);
	
	if( m_hTerrainBlock )
	{
		if ( ::intersectRay(m_hTerrainBlock,
					RayOrig.x,	RayOrig.y,	RayOrig.z,
					RayDir.x,	RayDir.y,	RayDir.z,
					&PosPICK.x,				&PosPICK.y,				&PosPICK.z,				&fDistance) ) {
			if ( fDistance < fCurMinDist ) {
				fCurMinDist = fDistance;
				PosOUT = PosPICK;
				return true;
			}
		}
	}

	return false;

}

bool CMAP_PATCH::IsIntersect_test (float &fCurMinDist, D3DXVECTOR3 &PosOUT, float Position[3], float Direction[3])
{
	
   float fDistance;
	D3DXVECTOR3 PosPICK;
	D3DXVECTOR3 RayOrig(Position[0], Position[1], Position[2]);
	D3DXVECTOR3 RayDir(Direction[0], Direction[1] ,Direction[2]);
	
	
	
	if( m_hTerrainBlock )
	{
		if ( ::intersectRay(m_hTerrainBlock,
					RayOrig.x,	RayOrig.y,	RayOrig.z,
					RayDir.x,	RayDir.y,	RayDir.z,
					&PosPICK.x,				&PosPICK.y,				&PosPICK.z,				&fDistance) ) {
			if ( fDistance < fCurMinDist ) {
				fCurMinDist = fDistance;
				PosOUT = PosPICK;
				return true;
			}
		}
	}

	return false;

}


void CMAP_PATCH::SetPOSITION (CMAP *pMAP, short nPatchXFromMap, short nPatchYFromMap)
{
	m_pMAP = pMAP;
	m_nPatchXFromMAP = nPatchXFromMap;
	m_nPatchYFromMAP = nPatchYFromMap;

    m_aabb.x[0]=nPatchXFromMap*fPATCH_SIZE+iMAP_SIZE*pMAP->m_nZoneMapXIDX;
	m_aabb.x[1]=m_aabb.x[0]+fPATCH_SIZE;

    m_aabb.y[0]=nPatchYFromMap*fPATCH_SIZE+iMAP_SIZE*pMAP->m_nZoneMapYIDX;
	m_aabb.y[1]=m_aabb.y[0]+fPATCH_SIZE;

    m_AABBMin.x=m_AABBMin.y=m_AABBMin.z=100000000.0f;
    m_AABBMax.x=m_AABBMax.y=m_AABBMax.z=-100000000.0f;
    
    ExPatchEnable=FALSE;
    ObjectOnOff=FALSE;

}

void CMAP_PATCH::SetMATERIAL (short nTileNO)
{
	m_nTileNO    = nTileNO;
	m_nDnTileTEX = TILETBL_LAYER1( nTileNO ) + TILETBL_DNOFF( nTileNO );
	m_nUpTileTEX = TILETBL_LAYER2( nTileNO ) + TILETBL_UPOFF( nTileNO );
}
bool CMAP_PATCH::Register (short nWorldX, short nWorldY)
{	
	///
	/// 하늘 연출등에서 지형블락은 로드를 하지 않아야 할 모드에서
	///
	if( !g_GameDATA.m_bJustObjectLoadMode )
	{
		HNODE hDnMat = g_pTerrain->GetMATERIAL( m_nDnTileTEX );
		HNODE hUpMat = g_pTerrain->GetMATERIAL( m_nUpTileTEX );

		char *szName;

		_ASSERT( hDnMat );
		_ASSERT( hUpMat );

		/// 꽉찬 타일이라면 윗타일은 빼준다.
		///if( hDnMat == hUpMat )
		///	hUpMat = 0;

		szName = CStr::Printf( "@tP%dx%d", nWorldX, nWorldY );

		m_hTerrainBlock = ::loadTerrainBlockEx( szName,
						nWorldX*fPATCH_SIZE, (1+nWorldY)*fPATCH_SIZE,
						GRID_COUNT_PER_PATCH_AXIS,
						1, TILETBL_PUTTYPE(m_nTileNO),
						(float*)m_fHEIGHT,
						m_aabb.z,
						iMAP_SIZE,
						nPATCH_SIZE,
						hDnMat,
						hUpMat,
						m_pMAP->LightMAP(),
						g_GameDATA.m_hShader_terrain,
						g_GameDATA.m_hLight);
		
		if ( m_hTerrainBlock ) {
			::setCollisionLevel( m_hTerrainBlock, 4 );
		}

		m_bIsRegistered = true;
	}


	return true;
} 
bool CMAP_PATCH::RegisterToNZIN (void)
{
	if ( m_bIsRegistered ) return false;

	_ASSERT( GRID_COUNT_PER_PATCH_AXIS+1 < MAX_VERTEX_COUNT_PER_PATCH_AXIS );

	short nX, nY;
	for (nY=0; nY<=GRID_COUNT_PER_PATCH_AXIS; nY++)
		for (nX=0; nX<=GRID_COUNT_PER_PATCH_AXIS; nX++) {
			m_fHEIGHT[ ( GRID_COUNT_PER_PATCH_AXIS-nY) * (GRID_COUNT_PER_PATCH_AXIS+1) + nX ] =
			//m_fHEIGHT[ GRID_COUNT_PER_PATCH_AXIS-nY ][ nX ] = 
				m_pMAP->GetVertexHEIGHT( m_nPatchXFromMAP*GRID_COUNT_PER_PATCH_AXIS+nX, m_nPatchYFromMAP*GRID_COUNT_PER_PATCH_AXIS+nY );
		}

	return this->Register( 
		this->m_nPatchXFromMAP+PATCH_COUNT_PER_MAP_AXIS* m_pMAP->m_nZoneMapXIDX, 
		this->m_nPatchYFromMAP+PATCH_COUNT_PER_MAP_AXIS* m_pMAP->m_nZoneMapYIDX );
}

/// 04/4/25
/// 패치에 이펙트를 등록할때 리스트에 추가하지 않았음
/// 그런데 지울때는 이펙트 리스트에서 지워주기를 바랬음
/// 그래서 다른 존으로 이동후에 이펙트들이 남았음..
/// !!! 
/// @bug m_bIsRegistered는 insertToScene 할때 세팅된다. 그러나 오브젝트 리스트라던지
///      기타 오브젝트는 맵 로딩시 등록된다. 결국 짝이 않맞다.
bool CMAP_PATCH::UnregisterFromNZIN (void)
{
	if( m_bIsRegistered )
	{
		this->RemoveFromScene ();	// UnregisterFromNZIN

		///
		/// 하늘 연출등에서 지형블락은 로드를 하지 않아야 할 모드에서
		///
		if( !g_GameDATA.m_bJustObjectLoadMode )
		{
			assert( m_hTerrainBlock != 0 );
			if( m_hTerrainBlock )
			{
				::unloadTerrainBlock ( m_hTerrainBlock );
				m_hTerrainBlock = 0;
			}
		}
	}
	
	///if (!m_bIsRegistered) return true; // 추가된 코드

	classDLLNODE< tagMAPEFT > *pEftNode;

	pEftNode = m_EffectLIST.GetHeadNode ();

	while( pEftNode ) 
	{
		//g_pEffectLIST->Del_EFFECT( pEftNode->DATA.m_pEffect );
		if( pEftNode->DATA.m_pEffect )
		{
			delete pEftNode->DATA.m_pEffect;
			pEftNode->DATA.m_pEffect = NULL;
		}else
			assert( 0 && "pEftNode->DATA.m_pEffect = NULL" );

		m_EffectLIST.DeleteNFree( pEftNode );
		pEftNode = m_EffectLIST.GetHeadNode ();
	}

	classDLLNODE< tagMAPSND > *pSndNode;
	pSndNode = m_SoundLIST.GetHeadNode ();

	while( pSndNode ) {
		g_pSoundLIST->SubSoundFile(pSndNode->DATA.m_HashKEY );
		m_SoundLIST.DeleteNFree( pSndNode );
		pSndNode = m_SoundLIST.GetHeadNode ();
	}

	classDLLNODE< CGameOBJ* > *pObjNode;
	pObjNode = m_FixObjLIST.GetHeadNode ();
	while( pObjNode ) 
	{
		g_pObjMGR->Del_Object( pObjNode->DATA );
		m_FixObjLIST.DeleteNFree( pObjNode );
		pObjNode = m_FixObjLIST.GetHeadNode ();	

		CTERRAIN::m_RegistedPatchCnt--;
	}	

	///
	m_CylinderLIST.ClearList();
	
	
	m_bIsRegistered = false;
	
	return true;
}

void CMAP_PATCH::InsertToScene (void)
{		
	if ( !m_bIsVisible ) 
	{
		RegisterToNZIN ();

		///
		/// 하늘 연출등에서 지형블락은 로드를 하지 않아야 할 모드에서
		///
		if( !g_GameDATA.m_bJustObjectLoadMode )
		{
			assert( m_hTerrainBlock != 0 );
			if( m_hTerrainBlock )
				::insertToScene  ( m_hTerrainBlock );	// CMAP_PATCH::InsertToScene
		}


		m_bIsVisible = true;

		classDLLNODE< CGameOBJ* > *pObjNode;
		pObjNode = m_FixObjLIST.GetHeadNode ();
		while( pObjNode ) 
		{
			pObjNode->DATA->InsertToScene ();
			pObjNode = m_FixObjLIST.GetNextNode( pObjNode );
		}


		classDLLNODE< tagMAPEFT > *pEftNode;
		pEftNode = m_EffectLIST.GetHeadNode ();
		while( pEftNode ) 
		{
			if ( !pEftNode->DATA.m_pEffect ) 
			{
				pEftNode->DATA.m_pEffect = g_pEffectLIST->Add_EFFECT ( pEftNode->DATA.m_HashKEY );
				if ( pEftNode->DATA.m_pEffect )
				{
					pEftNode->DATA.m_pEffect->SetPOSITION( pEftNode->DATA.m_Positon );
					pEftNode->DATA.m_pEffect->InsertToScene ();
				}else
				{
					assert( 0 && "Insert to scene a terrain effect is failed" );
				}
			}else
				pEftNode->DATA.m_pEffect->InsertToScene();

			pEftNode = m_EffectLIST.GetNextNode( pEftNode );
		}
	}
}

/// 04/4/25
/// 패치에 이펙트를 등록할때 리스트에 추가하지 않았음
/// 그런데 지울때는 이펙트 리스트에서 지워주기를 바랬음
/// 그래서 다른 존으로 이동후에 이펙트들이 남았음..
/// 04/4/26 이건 m_pEffect->RemoveFromScene(); 했어야 했는데 지웠었군.. 그래서.. UnRegister 할때 assert에 걸렸다.
void CMAP_PATCH::RemoveFromScene (void)
{	
	if ( m_bIsVisible ) 
	{
		///
		/// 하늘 연출등에서 지형블락은 로드를 하지 않아야 할 모드에서
		///
		if( !g_GameDATA.m_bJustObjectLoadMode )
		{
			if( m_hTerrainBlock )
			{
				::removeFromScene( m_hTerrainBlock );	// CMAP_PATCH::RemoveFromScene 				
			}
		}

		m_bIsVisible = false;

		classDLLNODE< CGameOBJ* > *pObjNode;
		pObjNode = m_FixObjLIST.GetHeadNode ();
		while( pObjNode ) 
		{
			pObjNode->DATA->RemoveFromScene ();
			pObjNode = m_FixObjLIST.GetNextNode( pObjNode );
		}

		classDLLNODE< tagMAPEFT > *pEftNode;
		pEftNode = m_EffectLIST.GetHeadNode ();
		while( pEftNode ) 
		{
			///g_pEffectLIST->Del_EFFECT( pEftNode->DATA.m_pEffect );
			pEftNode->DATA.m_pEffect->RemoveFromScene();
			/*if( pEftNode->DATA.m_pEffect )
			{
				delete pEftNode->DATA.m_pEffect;
				pEftNode->DATA.m_pEffect = NULL;
			}else
				assert( 0 && "pEftNode->DATA.m_pEffect = NULL" );*/
			pEftNode = m_EffectLIST.GetNextNode( pEftNode );
		}
	}
}


//-------------------------------------------------------------------------------------------------
/// @todo 사운드 유효거리에 따른 처리...
void CMAP_PATCH::PlaySOUND ()
{
	classDLLNODE< tagMAPSND > *pSndNode;
	pSndNode = m_SoundLIST.GetHeadNode ();

	DWORD dwTime = g_GameDATA.GetGameTime();
	while( pSndNode ) 
	{
		//pSndNode->DATA.m_uiRange;

		if ( dwTime - pSndNode->DATA.m_dwLastPlayTIME > pSndNode->DATA.m_uiInterval ) 
		{
			int iVolume = g_pSoundLIST->GetVolume ();// * xxx / pSndNode->DATA.m_uiRange;
			// int iPan = ???;

			//g_pSoundLIST->KEY_PlaySound( pSndNode->DATA.m_HashKEY, iVolume );
			const float fMaxStart = 200.0f;
			const float fMaxEnd = 50000.0f;
			const float fMinStart = 100.0f;
			float fMinDistance, fMaxDistance = 100.0f * pSndNode->DATA.m_uiRange; // 단위: 현재 미터

			if ( fMaxDistance < fMaxStart ) fMaxDistance = fMaxStart;
			if ( fMaxDistance > fMaxEnd ) fMaxDistance = fMaxEnd;
			fMinDistance = fMaxDistance / 10;
			if (fMinDistance < fMinStart) fMinDistance = fMinStart;

			//LogString(LOG_DEBUG, "Range = %d, Min(%f), Max(%f)\n", pSndNode->DATA.m_uiRange, fMinDistance, fMaxDistance);

			g_pSoundLIST->KEY_PlaySound3D( pSndNode->DATA.m_HashKEY, pSndNode->DATA.m_Position, iVolume, 0, 0, fMinDistance, fMaxDistance );
			pSndNode->DATA.m_dwLastPlayTIME = dwTime;
		}

		pSndNode = m_SoundLIST.GetNextNode( pSndNode );
	}
}

bool CMAP_PATCH::AddObject ( CGameOBJ *pObject, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale )
{		
	CTERRAIN::m_RegistedPatchCnt++;
	m_FixObjLIST.AllocNAppend( pObject );

	return MakeAABBFromObject( pObject, Position, Rotate, Scale );
}

bool CMAP_PATCH::MakeAABBFromObject( CGameOBJ *pObject, D3DVECTOR &Position, D3DXQUATERNION &Rotate, D3DVECTOR &Scale )
{
	float fMin[3] = { 1000000.0f, 1000000.0f, 1000000.0f };
	float fMax[3] = { -1000000.0f, -1000000.0f, -1000000.0f };

	CMODEL<CFixedPART>* pModel = NULL;

	/// 각 타입별로 해당 모델데이터를 기반으로 바운딩박스 정보를 얻어온다.
	switch( pObject->Get_TYPE() )
	{
		case OBJ_MORPH:
			{

			}
			break;
		case OBJ_GROUND:
			{
				CObjTREE *pTree = (CObjTREE *)pObject;
				pModel = g_DATA.m_ModelDECO.GetMODEL( pTree->m_nTreeIdx );				
			}
			break;
		case OBJ_CNST:
			{
				CObjCNST *pCnst = (CObjCNST *)pObject;
				pModel = g_DATA.m_ModelCNST.GetMODEL( pCnst->m_nCnstIdx );	
				
			}
			break;
		case OBJ_EVENTOBJECT:
			{
				CObjFixedEvent *pCnst = (CObjFixedEvent *)pObject;
				pModel = g_DATA.m_ModelEvent.GetMODEL( pCnst->m_nTreeIdx );				
			}
			break;
	}

	if( pModel )
	{
		
		D3DXMATRIX t_m,r_m,s_m,object_m;

		D3DXMatrixTranslation(&t_m,Position.x,Position.y,Position.z); 
		D3DXMatrixRotationQuaternion (&r_m,&Rotate);
		D3DXMatrixScaling(&s_m,Scale.x,Scale.y,Scale.z);

        object_m=( s_m * r_m * t_m );
  
		TransformOBB2AABB(pModel->m_BBMin,pModel->m_BBMax,&object_m,fMin,fMax);

        if(fMin[0]<m_AABBMin.x)
        m_AABBMin.x=fMin[0];
        if(fMin[1]<m_AABBMin.y)
        m_AABBMin.y=fMin[1];
        if(fMin[2]<m_AABBMin.z)
        m_AABBMin.z=fMin[2];

        if(fMax[0]>m_AABBMax.x)
        m_AABBMax.x=fMax[0];
        if(fMax[1]>m_AABBMax.y)
        m_AABBMax.y=fMax[1];
        if(fMax[2]>m_AABBMax.z)
        m_AABBMax.z=fMax[2];
  
        ObjectOnOff=TRUE;

	}

	
	/// 위에서 구해진 오브젝트의 바운딩박스 정보로 패치의 바운딩 박스 정보를 갱신..

	/// 만약 패치의 초기 바운딩 박스가 변경된다면
	return true;
}


void CMAP_PATCH::TransformOBB2AABB(float *obb_min,float *obb_max,D3DXMATRIX *object_m,float *bb_min,float *bb_max)
{

        bb_min[0]=object_m->_11*(object_m->_11 < 0 ? obb_max[0] : obb_min[0])+
	      		  object_m->_21*(object_m->_21 < 0 ? obb_max[1] : obb_min[1])+
				  object_m->_31*(object_m->_31 < 0 ? obb_max[2]: obb_min[2]);

		bb_min[1]=object_m->_12*(object_m->_12 < 0 ? obb_max[0] : obb_min[0])+
				  object_m->_22*(object_m->_22 < 0 ? obb_max[1] : obb_min[1])+
				  object_m->_32*(object_m->_32 < 0 ? obb_max[2]: obb_min[2]);
	   
		bb_min[2]=object_m->_13*(object_m->_13 < 0 ? obb_max[0] : obb_min[0])+
				  object_m->_23*(object_m->_23 < 0 ? obb_max[1] : obb_min[1])+
				  object_m->_33*(object_m->_33 < 0 ? obb_max[2]: obb_min[2]);

		bb_max[0]=object_m->_11*(object_m->_11 > 0 ? obb_max[0] : obb_min[0])+
				  object_m->_21*(object_m->_21 > 0 ? obb_max[1] : obb_min[1])+
				  object_m->_31*(object_m->_31 > 0 ? obb_max[2]: obb_min[2]);

		bb_max[1]=object_m->_12*(object_m->_12 > 0 ? obb_max[0] : obb_min[0])+
				  object_m->_22*(object_m->_22 > 0 ? obb_max[1] : obb_min[1])+
				  object_m->_32*(object_m->_32 > 0 ? obb_max[2]: obb_min[2]);
	   
		bb_max[2]=object_m->_13*(object_m->_13 > 0 ? obb_max[0] : obb_min[0])+
				  object_m->_23*(object_m->_23 > 0 ? obb_max[1] : obb_min[1])+
				  object_m->_33*(object_m->_33 > 0 ? obb_max[2]: obb_min[2]);


		bb_min[0]+=object_m->_41;
		bb_max[0]+=object_m->_41;

		bb_min[1]+=object_m->_42;
		bb_max[1]+=object_m->_42;

		bb_min[2]+=object_m->_43;
		bb_max[2]+=object_m->_43;
	
}

void CMAP_PATCH::CompareSizePath2Obj()
{
    
      if(m_aabb.x[0]>m_AABBMin.x)
      ExPatchEnable=TRUE;
      else if(m_aabb.y[0]>m_AABBMin.y)
      ExPatchEnable=TRUE;
//	  else if(m_aabb.z[0]>m_AABBMin.z)
//    ExPatchEnable=TRUE;
   
      if(m_aabb.x[1]<m_AABBMax.x)
      ExPatchEnable=TRUE;
      else if(m_aabb.y[1]<m_AABBMax.y)
      ExPatchEnable=TRUE;
//	  else if(m_aabb.z[1]<m_AABBMax.z)
//    ExPatchEnable=TRUE;
   
 
  
//	ExPatchEnable=TRUE;


}

//-------------------------------------------------------------------------------------------------
CMAP::CMAP ()
{
#ifdef	__VIRTUAL_SERVER
	m_pRegenNODE = new classDLLNODE< CRegenAREA >;
	g_pTerrain->Add_RegenAREA( m_pRegenNODE );
#endif

	m_nUseMODE = MAP_EMPTY;
	m_hLightMAP=NULL;
	m_ppHeight = NULL;

	m_nZoneMapXIDX = 0;
	m_nZoneMapYIDX = 0;

	// 추가된 코드 by zho
	m_nXPatchCnt = 0;
	m_nYPatchCnt = 0;

	ZeroMemory( m_PATCH, sizeof( m_PATCH ) );
}
CMAP::~CMAP ()
{
#ifdef	__VIRTUAL_SERVER
	g_pTerrain->Del_RegenAREA( m_pRegenNODE );
#endif
	ClearFromNZIN ();
	g_pObjMGR->RemoveNpcInfo( this );
}

void CMAP::AddEffect(D3DVECTOR &Position, char *szEffectFile)
{
	short nPatchX, nPatchY;

	nPatchX = (short)( Position.x - ( m_nZoneMapXIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );
	nPatchY = (short)( Position.y - ( m_nZoneMapYIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );

	tagMAPEFT sMapEFT;

//	sMapEFT.m_EffectFile.Set( szEffectFile );
	sMapEFT.m_HashKEY = g_pEffectLIST->Add_EffectFILE( szEffectFile );
	sMapEFT.m_pEffect = NULL;
	sMapEFT.m_Positon = Position;

	m_PATCH[ nPatchY ][ nPatchX ].m_EffectLIST.AllocNAppend( sMapEFT );
}

//-------------------------------------------------------------------------------------------------
void CMAP::AddSound (D3DVECTOR &Position, char *szSoundFile, unsigned int uiInterval, unsigned int uiRange)
{
	short nPatchX, nPatchY;

	nPatchX = (short)( Position.x - ( m_nZoneMapXIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );
	nPatchY = (short)( Position.y - ( m_nZoneMapYIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );

	// 지형 효과 사운드....
	tagMAPSND sMapSND;

	sMapSND.m_HashKEY        = g_pSoundLIST->AddSoundFile( szSoundFile, 1 );
	sMapSND.m_Position		 = Position;
	sMapSND.m_dwLastPlayTIME = 0;
	sMapSND.m_uiInterval     = uiInterval;
	sMapSND.m_uiRange		 = uiRange;
	
	m_PATCH[ nPatchY ][ nPatchX ].m_SoundLIST.AllocNAppend( sMapSND );
}

//-------------------------------------------------------------------------------------------------
/// 맵에선 31_31에 의 1,1 에 등록되었던 패치라도 여기서 실제 오브젝트 위치로.. 게임 좌표계로 변환후의
/// 패치에 새로 등록한다.( 새로운 좌표축 구성과 오브젝트 배치와의 비밀이 여기에.. 상당히 복잡하다. )
bool CMAP::AddObject ( int iObjectIndex, D3DXVECTOR3 &Position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
										D3DXQUATERNION &Rotate = D3DXQUATERNION( 1.0f, 1.0f, 1.0f, 1.0f ), 
										D3DXVECTOR3 &Scale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f )  )
{
	CGameOBJ *pObject = g_pObjMGR->Get_OBJECT( iObjectIndex );
	if ( pObject ) {
		short nPatchX, nPatchY;

		nPatchX = (short)( pObject->Get_CurPOS().x - ( m_nZoneMapXIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );
		nPatchY = (short)( pObject->Get_CurPOS().y - ( m_nZoneMapYIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );

		if ( nPatchX < 0 || nPatchX >= PATCH_COUNT_PER_MAP_AXIS )
			return false;
		if ( nPatchY < 0 || nPatchY >= PATCH_COUNT_PER_MAP_AXIS )
			return false;

		
		
		/// If get cylinder infomation, insert cylinder info to patch.
		/// 
		/// if tree..
		if( pObject->IsA( OBJ_GROUND ) )
		{
			CMODEL<CFixedPART>* pModel = NULL;
			CObjTREE *pTree = (CObjTREE *)pObject;
			pModel = g_DATA.m_ModelDECO.GetMODEL( pTree->m_nTreeIdx );	

			if( pModel->m_iCylinderRadius != 0 )
			{
				D3DXVECTOR2 vMax, vMin;
				D3DXVECTOR4 bufferPosition1, bufferPosition2;
				short nPatchMin[2], nPatchMax[2];

				tagCYLINDERINFO CylinderInfo;
				
				D3DXMATRIX t_m,r_m,s_m,object_m;
	            D3DXMatrixTranslation(&t_m,Position.x,Position.y,Position.z); 
		        D3DXMatrixRotationQuaternion (&r_m,&Rotate);
		        D3DXMatrixScaling(&s_m,Scale.x,Scale.y,Scale.z);
                object_m=( s_m * r_m * t_m );
                
                bufferPosition1.x = pModel->m_iCylinderX; bufferPosition1.y = pModel->m_iCylinderY; bufferPosition1.z = 0.0f;	bufferPosition1.w=1.0f;
                D3DXVec4Transform (&bufferPosition2, &bufferPosition1, &object_m); 				
				
				CylinderInfo.m_fRadius = Scale.x*pModel->m_iCylinderRadius;
				CylinderInfo.m_Position = (D3DXVECTOR3)bufferPosition2;
                
				vMax.x = CylinderInfo.m_Position.x + pModel->m_iCylinderRadius;
				vMax.y = CylinderInfo.m_Position.y + pModel->m_iCylinderRadius;
    	        vMin.x = CylinderInfo.m_Position.x - pModel->m_iCylinderRadius;
				vMin.y = CylinderInfo.m_Position.y - pModel->m_iCylinderRadius;

			    nPatchMin[0] = (short)( vMin.x - ( m_nZoneMapXIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );	
                nPatchMin[1] = (short)( vMin.y - ( m_nZoneMapYIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );	
                nPatchMax[0] = (short)( vMax.x - ( m_nZoneMapXIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );	
                nPatchMax[1] = (short)( vMax.y - ( m_nZoneMapYIDX * nGRID_SIZE * GRID_COUNT_PER_MAP_AXIS ) ) / ( nGRID_SIZE*GRID_COUNT_PER_PATCH_AXIS );	
                      
                
                if(nPatchMin[0] < 0 )
					nPatchMin[0] = 0;
				if(nPatchMin[1] < 0)
					nPatchMin[1] = 0;
                if(nPatchMax[0] >= PATCH_COUNT_PER_MAP_AXIS)
					nPatchMax[0] = PATCH_COUNT_PER_MAP_AXIS-1;
				if(nPatchMax[1] >= PATCH_COUNT_PER_MAP_AXIS)
					nPatchMax[1] = PATCH_COUNT_PER_MAP_AXIS-1;
				
				for( int i = nPatchMin[0]; i <= nPatchMax[0]; i+=1)
				{
					for( int j = nPatchMin[1]; j <= nPatchMax[1]; j+=1)
						m_PATCH[j][i].m_CylinderLIST.AllocNAppend( CylinderInfo );
				}

			}
		}
			
	
		
		
		
		if( m_PATCH[ nPatchY ][ nPatchX ].AddObject( pObject, Position, Rotate, Scale ) )
		{
			/// 패치의 바운딩 박스가 변경되었다.
			if( std::find( m_PatchUpdateList.begin(), m_PatchUpdateList.end(), &m_PATCH[ nPatchY ][ nPatchX ] ) != m_PatchUpdateList.end() )
				m_PatchUpdateList.push_back( &m_PATCH[ nPatchY ][ nPatchX ] );
		}
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
void CMAP::MappingToZONE (short nXFrom3x3, short nYFrom3x3)
{
	short nViewPatchX = nXFrom3x3 * PATCH_COUNT_PER_MAP_AXIS;
	short nViewPatchY = nYFrom3x3 * PATCH_COUNT_PER_MAP_AXIS;

	LogString (LOG_DEBUG, "MappingToZONE::( %d, %d ) << ( %d, %d ) \n", nViewPatchX, nViewPatchY, nXFrom3x3, nYFrom3x3);

	int iH, iW;
	if ( this->IsUsing() ) {
		for (iH=0; iH<m_nYPatchCnt; iH++)
			for (iW=0; iW<m_nXPatchCnt; iW++) {
				g_pTerrain->SetPATCH( nViewPatchX+iW, nViewPatchY+iH, &m_PATCH[ iH ][ iW ] );
			}
	} else {
		for (iH=0; iH<m_nYPatchCnt; iH++)
			for (iW=0; iW<m_nXPatchCnt; iW++) {
				g_pTerrain->SetPATCH( nViewPatchX+iW, nViewPatchY+iH, NULL );
			}
	}
}

//-------------------------------------------------------------------------------------------------
// 단순 지형 높이를 얻는다.
float CMAP::GetHEIGHT (float fWorldX, float fWorldY)
{
	float fHeights[4];
	float fWeightHoriz, fWeightVert;
	GetHeightsByCoordinates( fWorldX, fWorldY, fHeights, &fWeightHoriz, &fWeightVert );

	float fInterpHorizTop, fInterpHorizBottom;
	fInterpHorizTop    = fHeights[0]*(1.0f-fWeightHoriz) + fHeights[1]*fWeightHoriz;
	fInterpHorizBottom = fHeights[2]*(1.0f-fWeightHoriz) + fHeights[3]*fWeightHoriz;

	return (fInterpHorizTop * (1.0f-fWeightVert) + fInterpHorizBottom * fWeightVert);
}

/// 단순 지형 기울기를 얻는다.
float CMAP::GetSlope (float fWorldX, float fWorldY)
{
	float fHeights[4];
	
	GetHeightsByCoordinates( fWorldX, fWorldY, fHeights );

	float fMaxHeight = fHeights[0];
	fMaxHeight = (fHeights[1] > fMaxHeight) ? fHeights[1] : fMaxHeight;
	fMaxHeight = (fHeights[2] > fMaxHeight) ? fHeights[2] : fMaxHeight;
	fMaxHeight = (fHeights[3] > fMaxHeight) ? fHeights[3] : fMaxHeight;

	float fMinHeight = fHeights[0];
	fMinHeight = (fHeights[1] < fMinHeight) ? fHeights[1] : fMinHeight;
	fMinHeight = (fHeights[2] < fMinHeight) ? fHeights[2] : fMinHeight;
	fMinHeight = (fHeights[3] < fMinHeight) ? fHeights[3] : fMinHeight;

	float fSlope = (fMaxHeight - fMinHeight) / fGRID_SIZE;

	return fSlope;
}

//void CMAP::GetNormal (float fWorldX, float fWorldY, D3DXVECTOR3& vNormal)
//{
//	float fHeights[4];
//	float fGridSize(fGRID_SIZE);
//
//	GetHeightsByCoordinates( fWorldX, fWorldY, fHeights );
//
//	D3DXVECTOR3 vRight(fGridSize, 0.0f, ( fHeights[1] - fHeights[0] ));
//	D3DXVECTOR3 vFront(0.0f, fGridSize, ( fHeights[2] - fHeights[0] ));
//	
//	D3DXVec3Normalize( &vRight, &D3DXVECTOR3(vRight) );
//	D3DXVec3Normalize( &vFront, &D3DXVECTOR3(vFront) );
//
//	::D3DXVec3Cross( &vNormal, &vRight, &vFront );
//}

void CMAP::GetNormal (float fWorldX, float fWorldY, D3DXVECTOR3& vNormal)
{
   float fHeights[4];
   float fGridSize(fGRID_SIZE);

   GetHeightsByCoordinates( fWorldX, fWorldY, fHeights );
   vNormal.x=(fHeights[0]-fHeights[1])/fGRID_SIZE;   // X방향으로 편미분 (Z 방향 '+'기준)
   vNormal.y=(fHeights[0]-fHeights[2])/fGRID_SIZE;   // Y방향으로 편미분 (Z 방향 '+'기준)
   vNormal.z=1.0f;

   D3DXVec3Normalize( &vNormal, &D3DXVECTOR3(vNormal) );  // Unit Vector
}

//-------------------------------------------------------------------------------------------------
void CMAP::ReadMapINFO( CFileSystem* pFileSystem, long lOffset)
{
	// 읽을 필요 없다.
	/*
	fseek(fp, lOffset, SEEK_SET);

	int iMapXPos, iMapYPos;
	fread (&iMapXPos, sizeof(int), 1, fp);
	fread (&iMapYPos, sizeof(int), 1, fp);

	int iZoneXPos, iZoneYPos;
	fread (&iZoneXPos, sizeof(int), 1, fp);
	fread (&iZoneYPos, sizeof(int), 1, fp);

	float ftmp;		// D3DMATRIX WorldTM;
	for (int i=0; i<16; i++)
		fread (&ftmp, sizeof(float), 1, fp);

	//	read null terninated string ...
	char *szStr;
	szStr = CStr::ReadString (fp, true);
	*/
}

//-------------------------------------------------------------------------------------------------
void CMAP::ReadOceanINFO ( CFileSystem* pFileSystem, long lOffset)
{
	if ( !g_DATA.Get_OceanMAT() )
		return;

	pFileSystem->Seek( lOffset, FILE_POS_SET );	

	float fSize, fSX, fSY, fSZ, fEX, fEY, fEZ, fW, fH;
	int	  iCount;

	pFileSystem->ReadFloat( &fSize );
	pFileSystem->ReadInt32( &iCount );

	HNODE hOceanBlock;
	for (int iC=0; iC<iCount; iC++) 
	{
		pFileSystem->ReadFloat3( &fSX, &fSZ, &fSY );	/// start 		
		pFileSystem->ReadFloat3( &fEX, &fEZ, &fEY );	/// end		
		

		fSX += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );
		fSY += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );
		fEX += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );
		fEY += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );
		fW = ( fEX-fSX );
		fH = ( fEY-fSY );

		hOceanBlock = ::loadOceanBlock( CStr::Printf("@OB%dx%d:%d", m_nZoneMapXIDX, m_nZoneMapYIDX, iC),
							fSX, fSY, fSZ,
							fW, fH,
							(int)(fW/fSize), 
							(int)(fH/fSize),
							g_DATA.Get_OceanMAT(),									// material
							g_GameDATA.m_hLight );						// light
		if ( hOceanBlock ) 
		{
			::setMaterialBlendTypeCustom( g_DATA.Get_OceanMAT(), 5, 2, 1 );

			//::setCollisionLevel( hOceanBlock, 0 );
			m_OceanLIST.AllocNAppend( hOceanBlock );
			::setOceanBlockInterval( hOceanBlock, 100 );
			::insertToScene( hOceanBlock );
		}		
	}
}

//-------------------------------------------------------------------------------------------------
void CMAP::ReadObjINFO( CFileSystem* pFileSystem, long lOffset, int iLumpType)
{
	pFileSystem->Seek( lOffset, FILE_POS_SET);	

	int iObjCNT, iObjType, iObjID, iMapXPos, iMapYPos;
	D3DXQUATERNION	Rotate;
	D3DVECTOR		Position, Scale;

	Rotate.w = 1; Rotate.x = 0; Rotate.y = 0; Rotate.z = 0; // 초기화

	pFileSystem->ReadInt32( &iObjCNT );

	this->m_pObjectIndex[ iLumpType ] = new int[ iObjCNT ];
	::ZeroMemory( m_pObjectIndex[ iLumpType ], sizeof(int)*iObjCNT );


	int  iEventID, iObjectIndex = 0;
	char *szName=CStr::GetString ();
	BYTE cNameLen;

	short nEventID = 0;
	short nWarpID = 0;

	for (int iC=0; iC<iObjCNT; iC++) 
	{
		pFileSystem->ReadByte( &cNameLen );
		if ( cNameLen > 0 ) 
		{
			pFileSystem->Read( szName, sizeof(char) * cNameLen );
			szName[ cNameLen ] = 0;
		
		} else 
		{
			szName[ 0 ] = 0;		
		}

		//pFileSystem->ReadInt32 ( &iEventID );
		pFileSystem->ReadInt16 ( &nWarpID );
		pFileSystem->ReadInt16 ( &nEventID );		
		iEventID = nEventID;

		pFileSystem->ReadInt32 (&iObjType );
		pFileSystem->ReadInt32 (&iObjID );
		pFileSystem->ReadInt32 (&iMapXPos );
		pFileSystem->ReadInt32 (&iMapYPos );

		pFileSystem->Read (&Rotate,		sizeof(D3DXQUATERNION) );
		pFileSystem->Read (&Position,	sizeof(D3DVECTOR) );
		pFileSystem->Read (&Scale,		sizeof(D3DVECTOR) );

		Position.x *= MAGNIFICATION_RATE;
		Position.y *= MAGNIFICATION_RATE;

		/// 맵정보는 32,32( MAP_COUNT_PER_ZONE_AXIS, MAP_COUNT_PER_ZONE_AXIS ) 에 원점이 위치한다.
		/// 그러나 게임은 0,0 의 왼쪽 아래 코너가 원점이다.
		/// 따라서 변환
		int iOneMapWidth = ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS );
		Position.x += ( MAP_COUNT_PER_ZONE_AXIS/2 * iOneMapWidth + (iOneMapWidth / 2) );
		Position.y += ( MAP_COUNT_PER_ZONE_AXIS/2 * iOneMapWidth + (iOneMapWidth / 2) );

		switch( iLumpType ) 
		{
			case LUMP_TERRAIN_MORPH  :
			{		
				iObjectIndex = g_pObjMGR->Add_MORPHER ( iObjID, Position, Rotate, Scale );
				if ( !this->AddObject( iObjectIndex ) ) 
				{
					g_pObjMGR->Del_Object( iObjectIndex );
					LogString (LOG_DEBUG, "TERRAIN CNST ERROR:: ID: %d \n", iObjID);
				}
				continue;
			}
			case LUMP_TERRAIN_REGEN  :
			{
#ifdef	__VIRTUAL_SERVER
				CRegenPOINT *pNewPOINT = new CRegenPOINT( this, Position.x, Position.y );
				m_pRegenNODE->DATA.m_LIST.AllocNAppend( pNewPOINT );

				pNewPOINT->Load( pFileSystem );
#endif
				continue;
			}
			case LUMP_TERRAIN_WATER  :
			{
				/*
				int iWidth, iHeight, iW, iH;

				fread( &iWidth,		sizeof(int),	1,	fp);
				fread( &iHeight,	sizeof(int),	1,	fp);

				float fHeight;
				BYTE  btHasWater;
				for (iH=0; iH<iHeight; iH++)
					for (iW=0; iW<iWidth; iW++) {
						fread( &btHasWater,	sizeof(BYTE),	1,	fp);
						fread( &fHeight,	sizeof(float),	1,	fp);
					}
				*/
				continue;
			}
			case LUMP_TERRAIN_EFFECT :
			{
				pFileSystem->ReadByte ( &cNameLen );
				pFileSystem->Read ( szName, sizeof(char) * cNameLen );	// sound file name ..		
				szName[ cNameLen ] = 0;

				this->AddEffect (Position, szName);
				continue;
			}
			case LUMP_TERRAIN_SOUND :
			{
				int iInterval, iRange;
				pFileSystem->ReadByte (&cNameLen );
				pFileSystem->Read (szName, sizeof( char) * cNameLen );	// sound file name ..		
				szName[ cNameLen ] = 0;

				pFileSystem->ReadInt32 (&iRange );	// cm
				pFileSystem->ReadInt32 (&iInterval );	// interval.. sec

				this->AddSound (Position, szName, iInterval*1000, iRange);
				continue;
			}

			case LUMP_TERRAIN_OBJECT :
			{				
				iObjectIndex = g_pObjMGR->Add_GndTREE( iObjID, Position, Rotate, Scale );
				if ( !this->AddObject( iObjectIndex, D3DXVECTOR3( Position ), Rotate, D3DXVECTOR3( Scale ) ) ) 
				{
					g_pObjMGR->Del_Object( iObjectIndex );
					LogString (LOG_DEBUG, "TERRAIN Object ERROR:: ID: %d \n", iObjID);
				}
				break;
			}
			case LUMP_TERRAIN_CNST :
			{				
				iObjectIndex = g_pObjMGR->Add_GndCNST( iObjID, Position, Rotate, Scale );
				if ( !this->AddObject( iObjectIndex, D3DXVECTOR3( Position ), Rotate, D3DXVECTOR3( Scale ) ) ) 
				{
					g_pObjMGR->Del_Object( iObjectIndex );
					LogString (LOG_DEBUG, "TERRAIN CNST ERROR:: ID: %d \n", iObjID);
				}
				break;
			}

			case LUMP_TERRAIN_MOB :
			{
				short nQuestIDX;
				int iAI;
				pFileSystem->ReadInt32 (&iAI );
				
				// ai file name..
				// quest id !!!
				pFileSystem->ReadByte (&cNameLen );
				pFileSystem->Read (szName, sizeof(char) * cNameLen );	// sound file name ..		
				szName[ cNameLen ] = 0;

				if ( strlen( szName ) ) 
				{
					nQuestIDX = g_TblEVENT.GetRowIndex( szName );
					/*
					if ( nIndex ) {
						HashEVENT = g_pEventLIST->Add_EVENT( QUEST_FILE_FILENAME( nIndex ) );
						if ( HashEVENT ) {
							LogString (LOG_DEBUG, "Event File[ %s ]\n", szName);
						}
					}
					*/
				} else 
					nQuestIDX = 0;

				if ( NPC_TYPE( iObjID ) == 999 ) 
				{
					g_pObjMGR->AddNpcInfo( this, iObjID, Position );
				}
			#ifdef	__VIRTUAL_SERVER
				if ( NPC_TYPE( iObjID ) == 999 ) 
				{
					float fModelDIR = quaternionToModelDirection( Rotate.w, Rotate.x, Rotate.y, Rotate.z );
					iObjectIndex = g_pObjMGR->Add_NpcCHAR( 0, iObjID, Position, nQuestIDX, fModelDIR );
					CObjCHAR *pObj = g_pObjMGR->Get_CharOBJ( iObjectIndex, true );
					/*
					if ( pObj )
						::setQuaternion( pObj->GetZMODEL(), Rotate.w, Rotate.x, Rotate.y, Rotate.z );
					*/
				} else {
					// iObjectIndex = g_pObjMGR->Add_MobCHAR( 0, iObjID, Position, nQuestIDX );
					continue;
				}

				if ( !iObjectIndex ) {
					LogString (LOG_DEBUG, "TERRAIN MOB ERROR:: ID: %d \n", iObjID);
					continue;
				}
			#endif
				continue;
			}

			/// Warp object
			case LUMP_TERRAIN_WARP:
				{
					iObjectIndex = g_pObjMGR->Add_WARP ( nWarpID, Position, Rotate, Scale );
					if ( !this->AddObject( iObjectIndex ) ) 
					{
						g_pObjMGR->Del_Object( iObjectIndex );
						LogString (LOG_DEBUG, "TERRAIN WARP ERROR:: ID: %d \n", iObjID);
					}

					continue;
				}
				break;
				/// Warp object
			case LUMP_TERRAIN_COLLISION:
				{
					iObjectIndex = g_pObjMGR->Add_CollisionBox( iEventID, Position, Rotate, Scale );
					if ( !this->AddObject( iObjectIndex ) ) 
					{
						g_pObjMGR->Del_Object( iObjectIndex );
						LogString (LOG_DEBUG, "TERRAIN COLLISION ERROR:: ID: %d \n", iObjID);
					}

					continue;
				}
				break;

			case LUMP_TERRAIN_EVENT_OBJECT:
				{
					char strBuf[1024];
					/// Trigger name
					pFileSystem->ReadPascalString( strBuf, sizeof( strBuf ) );					
					std::string strTriggerName( strBuf );

					/// Confile name
					pFileSystem->ReadPascalString( strBuf, sizeof( strBuf ) );
					std::string strConfileName( strBuf );
					

					iObjectIndex = g_pObjMGR->Add_FixedEvent( iEventID, iObjID, 
																strTriggerName.c_str(),
																strConfileName.c_str(),
																Position, Rotate, Scale );
					if ( !this->AddObject( iObjectIndex, D3DXVECTOR3( Position ), Rotate, D3DXVECTOR3( Scale ) ) ) 
					{
						g_pObjMGR->Del_Object( iObjectIndex );
						LogString (LOG_DEBUG, "TERRAIN Object ERROR:: ID: %d \n", iObjID);
					}					

					break;
				}
				break;

			default :
			{
				LogString (LOG_DEBUG, "Invalid map object type : %d \n", iLumpType );
				continue;
			}
		}
		this->m_pObjectIndex[ iLumpType ][ iC ] = iObjectIndex;

		LogString (LOG_DEBUG, "Object:: Map[%02d,%02d], Type:%d, ID: %d, Pos[%d,%d] \n", m_nZoneMapXIDX, m_nZoneMapYIDX, iObjType, iObjID, iMapXPos, iMapYPos);
	}
}

//-------------------------------------------------------------------------------------------------
bool CMAP::LoadLightMapINFO (int iLumpType, char *szLightFileINF, char *szPath)
{
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szLightFileINF ) == false )	
	{		
		char *szStr = CStr::Printf ("File [%s] open error ", szLightFileINF );
		//g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
		return false;
	}
	
	
	int iObjCNT, iPartCNT, iObjIDX, iPartIDX;
	BYTE btStrLEN;

	pFileSystem->ReadInt32( &iObjCNT );
	for (int iO=0; iO < iObjCNT ; iO++ ) 
	{
		pFileSystem->ReadInt32( &iPartCNT );
		pFileSystem->ReadInt32( &iObjIDX );
		iObjIDX --;	// 1 base ..
		if( iObjIDX < 0 )
		{
			pFileSystem->CloseFile();
			(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

			return false;
		}

		CObjFIXED *pObject;
		pObject = (CObjFIXED*)g_pObjMGR->Get_OBJECT( m_pObjectIndex[ iLumpType ][ iObjIDX ] );

		char *pMapFILE;
		for( int iP=0; iP < iPartCNT ; iP++ ) 
		{
			pFileSystem->ReadByte( &btStrLEN );
			pFileSystem->Seek( btStrLEN, FILE_POS_CUR );
			pFileSystem->ReadInt32( &iPartIDX );

			pMapFILE = CStr::GetString ();
			pFileSystem->ReadByte( &btStrLEN );
			pFileSystem->Read( pMapFILE, sizeof(char) * btStrLEN );
			pMapFILE[ btStrLEN ] = 0;

			pFileSystem->Seek( sizeof(int), FILE_POS_CUR ); // skiip light map index			

			int iPixelPerOBJ;
			int iObjPerWIDTH;
			int iObjPosInMAP;
			pFileSystem->ReadInt32( &iPixelPerOBJ );
			pFileSystem->ReadInt32( &iObjPerWIDTH );
			pFileSystem->ReadInt32( &iObjPosInMAP );

			if ( NULL != pObject ) 
			{
				
				pObject->SetLightMap( iPartIDX,
						CStr::Printf ("%s\\LightMap\\%s", szPath, pMapFILE), 
						iObjPosInMAP % iObjPerWIDTH, 
						iObjPosInMAP / iObjPerWIDTH, 
						iObjPerWIDTH, 
						iObjPerWIDTH);
			}
		}
	}

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

	return true;
}

//-------------------------------------------------------------------------------------------------
CMAP*CMAP::Load (char *szFileName, short nZoneMapXIDX, short nZoneMapYIDX)
{
	char *szFullPathName;
	
	m_nZoneMapXIDX = nZoneMapXIDX;
	m_nZoneMapYIDX = nZoneMapYIDX;


	///////////////////////////////////////////////////////////////////////////////
	/// HeightsMap info file
	///////////////////////////////////////////////////////////////////////////////

	szFullPathName = CStr::Printf ("%s.HIM", szFileName);

	/// Load file
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szFullPathName ) == false )	
	{	
		/// 원래 없을 수도 있다.
		//char *szStr = CStr::Printf ("File [%s] open error ", szFullPathName );
		//g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
		return false;
	}	

	szFullPathName = CStr::Printf ("%s\\%d_%d_PlaneLightingMap.dds", szFileName, nZoneMapXIDX, MAP_COUNT_PER_ZONE_AXIS-nZoneMapYIDX );

	if ( (CVFSManager::GetSingleton()).IsExistFile( szFullPathName ) == false )	
	{
		szFullPathName = CStr::Printf ("%s\\%d_%d_PlaneLightingMap.tga", szFileName, nZoneMapXIDX, MAP_COUNT_PER_ZONE_AXIS-nZoneMapYIDX );

		if ( (CVFSManager::GetSingleton()).IsExistFile( szFullPathName ) == false )		
			szFullPathName = CStr::Printf("3DDATA\\TERRAIN\\default_light.dds");
	}
	m_hLightMAP = ::loadColormapMaterial(CStr::Printf("%dx%d_%s", nZoneMapXIDX, nZoneMapYIDX, szFullPathName), g_GameDATA.m_hShader_lightmap, szFullPathName );
	if ( m_hLightMAP ) 
	{
		;
		//LogString (LOG_DEBUG, "Load Lightmap[ %s / %s ] \n", szFullPathName, szFullPathName );
	} else {
		LogString (LOG_DEBUG, "ERROR:: Load Lightmap:: %s \n", szFullPathName);
	}


	int iWidth = 0, iHeight = 0;
	pFileSystem->ReadInt32( &iWidth );	// iWidth = patch cnt * 4 + 1
	pFileSystem->ReadInt32( &iHeight );

	// appended 03. 07. 25
	int iPatchGridCnt = 0;
	float fOnePatchSize = 0;
	pFileSystem->ReadInt32( &iPatchGridCnt );
	pFileSystem->ReadFloat( &fOnePatchSize );

	m_nXPatchCnt = ( iWidth-1 ) / iPatchGridCnt;
	m_nYPatchCnt = ( iHeight-1) / iPatchGridCnt;

	int iW, iH;
	float fHeight;

	_ASSERT( iHeight == VERTEX_COUNT_PER_MAP_AXIS );
	_ASSERT( iWidth == VERTEX_COUNT_PER_MAP_AXIS );

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

	this->m_ppHeight = new float* [ VERTEX_COUNT_PER_MAP_AXIS ];
	for (iH=iHeight-1; iH>=0; iH--) 
	{
		this->m_ppHeight[ iH ] = new float [ VERTEX_COUNT_PER_MAP_AXIS ];

		for (iW=0; iW<iWidth; iW++) 
		{
			pFileSystem->ReadFloat( &fHeight );
			this->m_ppHeight[ iH ][ iW ] = fHeight;// * 0.01f * nGRID_SIZE;
		}
	}

    char buffer_name[30];
    pFileSystem->ReadPascalString(buffer_name,4);
        
	int nPatch, nQuadPatch;
	
	nPatch=0;
	nQuadPatch=0;
   
    pFileSystem->ReadInt32( &nPatch);	
      
	iHeight=iWidth=PATCH_COUNT_PER_MAP_AXIS;

    for (iH=0; iH< iHeight;iH+=1) 
	{
		
		for (iW=0; iW<iWidth; iW+=1) 
		{
			pFileSystem->ReadFloat( &m_PATCH[iH][iW].m_aabb.z[1]);
		    pFileSystem->ReadFloat( &m_PATCH[iH][iW].m_aabb.z[0]);
		
		}
	}


    pFileSystem->ReadInt32( &nQuadPatch);	
      
	
    for (iH=0; iH< nQuadPatch;iH+=1) 
	{
		
	 	pFileSystem->ReadFloat( &m_QuadManager.m_quadPATCH[iH].aabb.z[1]);
	    pFileSystem->ReadFloat( &m_QuadManager.m_quadPATCH[iH].aabb.z[0]);
		
	}
	


	pFileSystem->CloseFile();
	

	///////////////////////////////////////////////////////////////////////////////
	/// Tile info file
	///////////////////////////////////////////////////////////////////////////////

	szFullPathName = CStr::Printf ( "%s.TIL", szFileName );

	//pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szFullPathName ) == false )	
	{		
		char *szStr = CStr::Printf ("File [%s] open error ", szFullPathName );
		g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
		return false;
	}	
	
	BYTE btBrushNO, btTileIDX, btTileSET;
	int  iTileNO;

	pFileSystem->ReadInt32( &iWidth );	// iWidth = patch cnt 
	pFileSystem->ReadInt32( &iHeight );

	///////////////////////////////////////////////////////////////////////////////
	/// Tile info file -> height_max,height_min
	///////////////////////////////////////////////////////////////////////////////
	
	
	
	
	
	if( iWidth != PATCH_COUNT_PER_MAP_AXIS )
	{
		iWidth = PATCH_COUNT_PER_MAP_AXIS;
		g_itMGR.OpenMsgBox( "유효하지 않은 맵 데이터" );
	}

	if( iHeight != PATCH_COUNT_PER_MAP_AXIS )
	{
		iHeight = PATCH_COUNT_PER_MAP_AXIS;
		g_itMGR.OpenMsgBox( "유효하지 않은 맵 데이터" );
	}

	for (iH=iHeight-1; iH>=0; iH--)
	{
		for (iW=0; iW<iWidth; iW++) 
		{
			pFileSystem->ReadByte( &btBrushNO );
			pFileSystem->ReadByte( &btTileIDX );
			pFileSystem->ReadByte( &btTileSET );
			pFileSystem->ReadInt32( &iTileNO );

			m_PATCH[ iH ][ iW ].SetMATERIAL ( iTileNO );
		}
	}

	for (iH=0; iH<iHeight; iH++) 
	{
		for (iW=0; iW<iWidth; iW++) 
		{
			m_PATCH[ iH ][ iW ].SetPOSITION (this, iW, iH);
		}
	}

	pFileSystem->CloseFile();


	///////////////////////////////////////////////////////////////////////////////
	/// Map info file
	///////////////////////////////////////////////////////////////////////////////

	szFullPathName = CStr::Printf ("%s.IFO", szFileName);

	//pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szFullPathName ) == false )	
	{		
		char *szStr = CStr::Printf ("File [%s] open error ", szFullPathName );
		g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
		return false;
	}	

	int  iLumpCNT, iType;

	for (iH=0; iH<LUMP_MAP_MAX; iH++)
		this->m_pObjectIndex[ iH ] = NULL;

	int  iOffset;
	long lCurPtr;
	pFileSystem->ReadInt32( &iLumpCNT );
	for (iH=0; iH<iLumpCNT; iH++) 
	{
		pFileSystem->ReadInt32( &iType );
		pFileSystem->ReadInt32( &iOffset );

		lCurPtr = pFileSystem->Tell();

		switch( iType ) 
		{
			case LUMP_MAPINFO :
				ReadMapINFO( pFileSystem, iOffset );
				break;
			case LUMP_TERRAIN_OCEAN :
				ReadOceanINFO ( pFileSystem, iOffset );
				break;

			case LUMP_TERRAIN_WATER :
			case LUMP_TERRAIN_REGEN :
			case LUMP_TERRAIN_EFFECT :
			case LUMP_TERRAIN_SOUND :
			case LUMP_TERRAIN_OBJECT :
			case LUMP_TERRAIN_MOB  :
			case LUMP_TERRAIN_CNST :
			case LUMP_TERRAIN_MORPH :
			case LUMP_TERRAIN_WARP:
			case LUMP_TERRAIN_COLLISION:
			case LUMP_TERRAIN_EVENT_OBJECT:
				ReadObjINFO( pFileSystem, iOffset, iType );
			default :
				break;
		}

		pFileSystem->Seek( lCurPtr, FILE_POS_SET );
	}
	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

	//load construct light map ...
	szFullPathName = CStr::Printf ("%s\\LightMap\\BuildingLightMapData.lit", szFileName);
	LoadLightMapINFO( LUMP_TERRAIN_CNST, szFullPathName, szFileName );

	//	load object light map ...
	szFullPathName = CStr::Printf ("%s\\LightMap\\ObjectLightMapData.lit", szFileName);
	LoadLightMapINFO( LUMP_TERRAIN_OBJECT, szFullPathName, szFileName );

	for (iH=0; iH<LUMP_MAP_MAX; iH++)
		SAFE_DELETE_ARRAY( this->m_pObjectIndex[ iH ] );

	m_nUseMODE = MAP_USING;

	LogString (LOG_DEBUG, ">>>>>>> load MAP :: %d, %d \n", m_nZoneMapXIDX, m_nZoneMapYIDX );

	
	//Load quadManager
	m_QuadManager.LoadQuadPatch(this);
    
    CompareSizePath2ObjAll();
    m_QuadManager.SetExPatchTotal();
	return this;
}

void CMAP::Free (void)
{	
	m_nUseMODE = MAP_DIRTY;

	ClearFromNZIN ();

	ZeroMemory( m_PATCH, sizeof( m_PATCH ) );
}

void CMAP::ClearFromNZIN (void)
{
	if (this->IsEmpty()) return; // 추가된 코드 by zho

	LogString (LOG_DEBUG, "<<<<<<< Clear MAP :: %d, %d \n", m_nZoneMapXIDX, m_nZoneMapYIDX );

#ifdef	__VIRTUAL_SERVER
	classDLLNODE< CRegenPOINT* > *pPointNODE;
	pPointNODE = m_pRegenNODE->DATA.m_LIST.GetHeadNode ();
	while( pPointNODE ) {
		m_pRegenNODE->DATA.m_LIST.DeleteNFree( pPointNODE );
		pPointNODE = m_pRegenNODE->DATA.m_LIST.GetHeadNode ();
	}
#endif

	/// Free ocean list
	classDLLNODE< HNODE > *pOceanNode;
	pOceanNode = m_OceanLIST.GetHeadNode ();
	while( pOceanNode ) {
		::removeFromScene( pOceanNode->DATA );
		::unloadOceanBlock ( pOceanNode->DATA );
		m_OceanLIST.DeleteNFree( pOceanNode );
		pOceanNode = m_OceanLIST.GetHeadNode ();
	}
	
	/// Free ocean matrial list
	classDLLNODE< HNODE > *pOceanMatNode;
	pOceanMatNode = m_OceanMatLIST.GetHeadNode ();
	while( pOceanMatNode ) {
		::removeFromScene( pOceanMatNode->DATA );
		::unloadMaterial( pOceanMatNode->DATA );
		m_OceanMatLIST.DeleteNFree( pOceanMatNode );
		pOceanMatNode = m_OceanMatLIST.GetHeadNode ();
	}

	/// Lightmap
	if ( m_hLightMAP ) {
		::unloadMaterial( m_hLightMAP );
		m_hLightMAP = NULL;
	}

	short nZ;
	for (nZ=0; nZ<m_nYPatchCnt; nZ++)
		for (short nX=0; nX<m_nXPatchCnt; nX++) {
			m_PATCH[ nZ ][ nX ].UnregisterFromNZIN ();
		}
	if ( m_ppHeight ) {
		for (nZ=0; nZ<VERTEX_COUNT_PER_MAP_AXIS; nZ++)
			SAFE_DELETE_ARRAY( m_ppHeight[ nZ ] );
		SAFE_DELETE_ARRAY( m_ppHeight );
	}



	m_nUseMODE = MAP_EMPTY;
}

POINT CMAP::GetMapIndexFromWorld( float fWorldX, float fWorldY )
{
	POINT ptMapIndex = { 0, 0 };

	int nGridSize(nGRID_SIZE);
	float fGridSize(fGRID_SIZE);

	int iHoriz( int(fWorldX) / ( nGridSize * GRID_COUNT_PER_PATCH_AXIS ) );
	int iVerti( int(fWorldY) / ( nGridSize * GRID_COUNT_PER_PATCH_AXIS ) );	

	iHoriz %= ( PATCH_COUNT_PER_MAP_AXIS );
	iVerti %= ( PATCH_COUNT_PER_MAP_AXIS );
	
	ptMapIndex.x = iHoriz;
	ptMapIndex.y = iVerti;

	return ptMapIndex;
}


void CMAP::GetHeightsByCoordinates (float fWorldX, float fWorldY, float fHeights[4], float * pfWeightHoriz, float * pfWeightVert)
{
	int nGridSize(nGRID_SIZE);
	float fGridSize(fGRID_SIZE);

	int iHoriz( int(fWorldX) / nGridSize );
	int iVerti( int(fWorldY) / nGridSize );

	if (pfWeightHoriz) *pfWeightHoriz = ( ( fWorldX - iHoriz*nGridSize ) / fGridSize );
	if (pfWeightVert) *pfWeightVert = ( ( fWorldY - iVerti*nGridSize ) / fGridSize );

	iHoriz %= ( VERTEX_COUNT_PER_MAP_AXIS-1 );
	iVerti %= ( VERTEX_COUNT_PER_MAP_AXIS-1 );

	// 0:lefttop, 1:righttop, 2:leftbottom, 3:rightbottom. in editor frame(x-z plane)
	// 0:leftbottom, 1:rightbottom, 2:lefttop, 3:righttop. in engine frame(x-y plane)
	fHeights[ 0 ] = m_ppHeight[ iVerti		][ iHoriz		];
	fHeights[ 1 ] = m_ppHeight[ iVerti		][ iHoriz + 1	];
	fHeights[ 2 ] = m_ppHeight[ iVerti + 1	][ iHoriz		];
	fHeights[ 3 ] = m_ppHeight[ iVerti + 1	][ iHoriz + 1	];
}

///
/// 현재 월드 위치로 해당 패치를 구하기
///
CMAP_PATCH*	CMAP::GetPatchFromWorld( float fWorldX, float fWorldY )
{
	POINT ptMapIndex = GetMapIndexFromWorld( fWorldX, fWorldY );
	
	return &( m_PATCH[ ptMapIndex.y ][ ptMapIndex.x ] );
}



void CMAP::CompareSizePath2ObjAll()
{

  int i,j;

  for(i=0;i<PATCH_COUNT_PER_MAP_AXIS;i+=1)
  {
	  for(j=0;j<PATCH_COUNT_PER_MAP_AXIS;j+=1)
	  {
        
          if(m_PATCH[i][j].ObjectOnOff)
		  m_PATCH[i][j].CompareSizePath2Obj();

	  }

  }
    
}














//-------------------------------------------------------------------------------------------------
CTERRAIN* CTERRAIN::m_pInstance = NULL;
//-------------------------------------------------------------------------------------------------
CTERRAIN::CTERRAIN ()
{	
	m_iObjCreateOrder = 0;	
	::ZeroMemory( m_pMapBUFFER, sizeof(CMAP*)*MAX_MAP_BUFFER);
	::ZeroMemory( m_pMAPS,sizeof(CMAP*)*MAP_COUNT_PER_ZONE_AXIS*MAP_COUNT_PER_ZONE_AXIS);
	m_pTILE = NULL;
	m_ppTileTYPE = NULL;

	m_pEventPOS = NULL;
	m_iEventPosCNT = 0;
}

CTERRAIN* CTERRAIN::Instance ()
{
	if ( m_pInstance == NULL ) {
		m_pInstance = new CTERRAIN;
	}

	return m_pInstance;
}


tPOINTF CTERRAIN::Get_EventPOS (char *szName)
{
	tPOINTF OutPOS;

	for (int iC=0; iC<m_iEventPosCNT; iC++) {
		if ( !strcmpi(m_pEventPOS[ iC ].m_Name.Get(), szName ) ) {
			OutPOS.x = m_pEventPOS[ iC ].m_Position.x;
			OutPOS.y = m_pEventPOS[ iC ].m_Position.y;

			LogString (LOG_DEBUG, "Pos[ %s ] = ( %f, %f ) \n", szName, OutPOS.x, OutPOS.y );

			return OutPOS;
		}
	}

	OutPOS.x = 0;
	OutPOS.y = 0;

	LogString (LOG_DEBUG, "Pos[ %s ] not found \n", szName );

	return OutPOS;
}
tPOINTF CTERRAIN::Get_StartPOS ()
{
	return Get_EventPOS( ZONE_START_POS(m_nZoneNO) );
}
tPOINTF CTERRAIN::Get_RevivePOS ()
{
	return Get_EventPOS( ZONE_REVIVE_POS(m_nZoneNO) );
}


void CTERRAIN::Add_RegenAREA( classDLLNODE< CRegenAREA > *pNode)
{
	m_RegenLIST.AppendNode( pNode );
}
void CTERRAIN::Del_RegenAREA( classDLLNODE< CRegenAREA > *pNode)
{
	m_RegenLIST.DeleteNode( pNode );
}
void CTERRAIN::Proc_RegenAREA (void)
{
	classDLLNODE< CRegenAREA > *pNode;
	pNode = m_RegenLIST.GetHeadNode ();
	while( pNode ) {
		pNode->DATA.Proc ();
		pNode = m_RegenLIST.GetNextNode( pNode );
	}
}
void CTERRAIN::Proc_Ecomony (void)
{
#ifdef	__VIRTUAL_SERVER
	m_Economy.Proc( g_GameDATA.GetGameTime() );
#endif
}

//-------------------------------------------------------------------------------------------------
HNODE CTERRAIN::GetLIGHTMAP(char *szLightMapFile)
{
	if ( (CVFSManager::GetSingleton()).IsExistFile( szLightMapFile ) == false )	
	{
		szLightMapFile = CStr::Printf("3DDATA\\TERRAIN\\Zone\\default_light.dds");
	}

	t_HASHKEY uiHashKey;
	uiHashKey = CStr::GetHASH(szLightMapFile);
	HNODE hLightMap = m_pTILE->KEY_HNODE(uiHashKey);
	if ( NULL == hLightMap ) {
		uiHashKey = m_pTILE->Add_MATERIAL (szLightMapFile, 0, 0, 0, true);
		hLightMap = m_pTILE->KEY_HNODE( uiHashKey );
	}

	return hLightMap;
}

//-------------------------------------------------------------------------------------------------
char*CTERRAIN::GetMapFILE(short nMapX, short nMapY)
{
	if ( nMapX < 0 || nMapX >= MAP_COUNT_PER_ZONE_AXIS )
		return NULL;
	if ( nMapY < 0 || nMapY >= MAP_COUNT_PER_ZONE_AXIS )
		return NULL;

	char *szMapFile = CStr::Printf ("%s%d_%d", m_ZoneDIR.Get(), nMapX, MAP_COUNT_PER_ZONE_AXIS-nMapY);
	m_MapFileName.Set( szMapFile );

	return m_MapFileName.Get();
}

//-------------------------------------------------------------------------------------------------
void CTERRAIN::ReadZoneINFO( CFileSystem* pFileSystem, long lOffset )
{
	int iValue;
	pFileSystem->Seek( lOffset,	FILE_POS_SET );
	pFileSystem->Seek( sizeof(int),	FILE_POS_CUR );
	
	pFileSystem->ReadInt32( &iValue );						// witdh
	LogString (LOG_DEBUG, "Width:: %d ... \n", iValue);

	pFileSystem->ReadInt32( &iValue );		// height
	LogString (LOG_DEBUG, "Height:: %d ... \n", iValue);


	// appended 03. 07. 25
	pFileSystem->ReadInt32( &iValue );	// patch grid count
	pFileSystem->ReadFloat( &m_fGridSIZE );
	m_nGridPerPATCH = iValue;

	m_fPatchSIZE = m_fGridSIZE * m_nGridPerPATCH;
	m_nGridSIZE  = (short)m_fGridSIZE;
	m_nPatchSIZE = (short)m_fPatchSIZE;

	// iMapSize = 16000, iBlockSize = 1000
	m_iMapSIZE = ( m_nPatchSIZE * 16 );

	pFileSystem->ReadInt32( &iValue );	// start pos
	m_nCenterMapXIDX = iValue;

	pFileSystem->ReadInt32( &iValue );	// start pos
	m_nCenterMapYIDX = iValue;

	LogString (LOG_DEBUG, "Start (X:%d, Y:%d) \n", m_nCenterMapXIDX, m_nCenterMapYIDX);
}

void CTERRAIN::ReadEventObjINFO ( CFileSystem* pFileSystem, long lOffset)
{
	pFileSystem->Seek( lOffset, FILE_POS_SET );
	
	BYTE btStrLen;
	char *szStr=CStr::GetString ();
	int iEventCNT;

	pFileSystem->ReadInt32( &iEventCNT );

	m_iEventPosCNT = iEventCNT;
	if ( m_iEventPosCNT > 0 ) 
	{
		m_pEventPOS = new tagEVENT_POS[ m_iEventPosCNT ];
	}

	float fTmp;
	for (int iC=0; iC<iEventCNT; iC++) 
	{
		pFileSystem->Read( &m_pEventPOS[ iC ].m_Position, sizeof(D3DVECTOR) );

		// exchange y <--> z
		fTmp = m_pEventPOS[ iC ].m_Position.z;
		m_pEventPOS[ iC ].m_Position.z = m_pEventPOS[ iC ].m_Position.y;
		m_pEventPOS[ iC ].m_Position.y = fTmp;

		m_pEventPOS[ iC ].m_Position.x *= MAGNIFICATION_RATE;
		m_pEventPOS[ iC ].m_Position.y *= MAGNIFICATION_RATE;

		m_pEventPOS[ iC ].m_Position.x += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );
		m_pEventPOS[ iC ].m_Position.y += (MAP_COUNT_PER_ZONE_AXIS/2 * ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) + ( nGRID_SIZE * GRID_COUNT_PER_PATCH_AXIS * PATCH_COUNT_PER_MAP_AXIS ) / 2 );

		pFileSystem->ReadByte( &btStrLen );

		_ASSERT( btStrLen > 0 );

		pFileSystem->Read( szStr, sizeof(char) * btStrLen );
		szStr[ btStrLen ] = 0;

		m_pEventPOS[ iC ].m_Name.Set( szStr );

		LogString (LOG_DEBUG, "Event Object:: %d / %s[%d] \n", iC, szStr, btStrLen);
	}
}

void CTERRAIN::ReadTileINFO( CFileSystem* pFileSystem, long lOffset )
{
	m_pTILE = new CMatLIST( NAME_TERRAIN_MAT );

	pFileSystem->Seek( lOffset, FILE_POS_SET );
	
	BYTE btStrLen;
	char *szStr=CStr::GetString ();
	int iTileCNT;

	pFileSystem->ReadInt32( &iTileCNT );

	tagMATERIAL *pMat;
	m_pTILE->Alloc_FILES( iTileCNT );

	for (int iC=0; iC<iTileCNT; iC++) 
	{
		pFileSystem->ReadByte( &btStrLen );
		pFileSystem->Read( szStr, sizeof(char) * btStrLen );
		szStr[ btStrLen ] = 0;

		LogString (LOG_DEBUG, "Tile Texture :: %d / %s[%d] \n", iC, szStr, btStrLen);

		pMat = new tagMATERIAL;
		pMat->m_nAlpha  = 1;
		pMat->m_nTwoSide= 0;
		pMat->m_hShader	= g_GameDATA.m_hShader_terrain;
		pMat->m_RealFileName.Set( szStr );
		m_pTILE->Add_FILE(szStr, iC, pMat, false, true );
	}
}

void CTERRAIN::ReadTileTYPE( CFileSystem* pFileSystem, long lOffset )
{
	pFileSystem->Seek( lOffset, FILE_POS_SET );

	int iValue;
	pFileSystem->ReadInt32( &m_iTileTypeCNT );

	m_ppTileTYPE = new short* [ m_iTileTypeCNT ];
	for (int iC=0; iC<m_iTileTypeCNT; iC++) 
	{
		m_ppTileTYPE[ iC ] = new short[ 7 ];

		for (int iX=0; iX<7; iX++) 
		{
			pFileSystem->ReadInt32( &iValue );
			m_ppTileTYPE[ iC ][ iX ] = iValue;
		}
	}
}

bool CTERRAIN::ReadECONOMY( CFileSystem* pFileSystem, long lOffset )
{
	pFileSystem->Seek( lOffset, FILE_POS_SET );
	
	BYTE btStrLen;
	char *pStr=CStr::GetString();

	// zone name
	pFileSystem->ReadByte( &btStrLen );
	pFileSystem->Read( pStr, sizeof( char) * btStrLen );
	pStr[ btStrLen ] = 0;
	m_ZoneNAME.Set( pStr );

	pFileSystem->ReadInt32( &m_iIsDungeon );

	// background music ...
	pFileSystem->ReadByte( &btStrLen );
	pFileSystem->Read( pStr, sizeof( char) * btStrLen );
	pStr[ btStrLen ] = 0;
	m_MusicFile.Set( pStr );

	// background model ...
	pFileSystem->ReadByte( &btStrLen );
	pFileSystem->Read( pStr, sizeof( char) * btStrLen );
	pStr[ btStrLen ] = 0;
	m_SkyNAME.Set( pStr );

#ifdef	__VIRTUAL_SERVER
	m_Economy.Load( pFileSystem );
#endif

	return true;
}

bool CTERRAIN::LoadZONE(short nZoneNO,bool bPlayBGM )
{
	::setDelayedLoad( false );

	g_pEventLIST->Clear ();
	this->FreeZONE ();

	if( bPlayBGM )
	{
		if( CMusicMgr::GetSingleton().bIsReady() )
		{
			///CBGMManager::GetSingleton().EndTransition();

			CMusicMgr::GetSingleton().Play ( ZONE_BG_MUSIC_DAY( nZoneNO ) );
			CMusicMgr::GetSingleton().SetVolume( g_ClientStorage.GetBgmVolumeByIndex( g_ClientStorage.GetBgmVolumeIndex() ) );
		}
	}

	
	g_DATA.FreeZoneData();
	g_DATA.LoadZoneData( nZoneNO );

	/// 라이트맵 블랜딩 오퍼레이션 세팅
	int iBlendOP = SKY_LIGHTMAP_BLEND( ZONE_BG_IMAGE( nZoneNO ) );
	::setLightmapBlendStyle( iBlendOP );


	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( ZONE_FILE( nZoneNO ) ) == false )	
	{		
		char *szStr = CStr::Printf ("File [%s] open error ", ZONE_FILE( nZoneNO ) );
		g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
		return false;
	}
	

	m_nZoneNO = nZoneNO;

	int iCount, iType, iOffset;

	iCount = CUtil::ExtractFileName( NULL, ZONE_FILE( m_nZoneNO ) );
	if ( iCount > 0 ) 
	{
		char *pStr=CStr::GetString ();
		strncpy( pStr, ZONE_FILE( nZoneNO ), iCount );
		pStr[ iCount ] = 0;
		m_ZoneDIR.Set( pStr );
		m_WideTerrain.SetZoneDir( pStr );
		LogString (LOG_DEBUG, "zone folder :: %s \n", pStr );
	}

	pFileSystem->ReadInt32( &iCount );
	for (int iC=0; iC<iCount; iC++) 
	{
		pFileSystem->ReadInt32( &iType );
		pFileSystem->ReadInt32( &iOffset );

		long lCurPtr = pFileSystem->Tell();

		switch( iType ) 
		{
			case LUMP_ZONE_INFO :
				ReadZoneINFO( pFileSystem, iOffset );
				break;
			case LUMP_EVENT_OBJECT :
				ReadEventObjINFO( pFileSystem, iOffset );
				break;
			case LUMP_ZONE_TILE :
				ReadTileINFO( pFileSystem, iOffset );
				break;
			case LUMP_TILE_TYPE :
				ReadTileTYPE( pFileSystem, iOffset );
				break;
			case LUMP_ECONOMY :
				ReadECONOMY ( pFileSystem, iOffset );
				break;
		}

		pFileSystem->Seek( lCurPtr, FILE_POS_SET );		
	}

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );
	

	short nX, nY;
	for (nX=0; nX<MAX_MAP_BUFFER; nX++) 
	{
		m_pMapBUFFER[ nX ] = new CMAP;
	}

	for (nY=0; nY<MAP_COUNT_PER_ZONE_AXIS; nY++)
		for (nX=0; nX<MAP_COUNT_PER_ZONE_AXIS; nX++) 
		{
			m_pMAPS[ nY ][ nX ] = NULL;
		}

	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 모든 존 정보 제거..
//----------------------------------------------------------------------------------------------------

void CTERRAIN::FreeZONE ()
{

	this->Clear_VisiblePatch ();
	this->SubMAP( MOVE_UPDATE_ALL );
	m_WideTerrain.SubRoughMap( MOVE_UPDATE_ALL );



	SAFE_DELETE_ARRAY( m_pEventPOS );

	if ( m_ppTileTYPE ) {
		for (int iC=0; iC<m_iTileTypeCNT; iC++) {
			SAFE_DELETE_ARRAY( m_ppTileTYPE[ iC ] );
		}
		SAFE_DELETE_ARRAY( m_ppTileTYPE );
		m_iTileTypeCNT = 0;
	}

	if ( m_pTILE ) {
		m_pTILE->Free ();
		SAFE_DELETE( m_pTILE );
	}

	for (short nI=0; nI<MAX_MAP_BUFFER; nI++) {
		SAFE_DELETE( m_pMapBUFFER[ nI ] );
	}

	if ( g_pAVATAR )
		g_pObjMGR->Clear( g_pAVATAR->Get_INDEX() );
	else
		g_pObjMGR->Clear( 0 );


	CMinimapDLG* pDlg = g_itMGR.GetMinimapDLG();
	if( pDlg )
		pDlg->FreeMinimap();
}


//-------------------------------------------------------------------------------------------------
bool CTERRAIN::InitZONE (float fCenterX, float fCenterY)
{
	this->SubMAP( MOVE_UPDATE_ALL );


	float fCenterZ;
	if ( fCenterX == 0 && fCenterY == 0 ) 
	{
		m_nCenterMapXIDX = 32;
		m_nCenterMapYIDX = 32;

		fCenterX = m_nCenterMapXIDX*16*fPATCH_SIZE  + 8*fPATCH_SIZE;
		fCenterY = m_nCenterMapYIDX*16*fPATCH_SIZE  + 8*fPATCH_SIZE;
	}

	m_nCenterMapXIDX = (short)( fCenterX / (16*fPATCH_SIZE) );
	m_nCenterMapYIDX = (short)( fCenterY / (16*fPATCH_SIZE) );


	this->AddMAP( m_nCenterMapXIDX, m_nCenterMapYIDX, MOVE_UPDATE_ALL, true, true );
	m_WideTerrain.AddRoughMap( m_nCenterMapXIDX, m_nCenterMapYIDX, MOVE_UPDATE_ALL );
	this->m_wViewFRAME = 0;

    
	fCenterZ = g_pTerrain->GetHeight(fCenterX, fCenterY) + 150;	


	this->ReOrginazationPatch( m_nCenterMapXIDX, m_nCenterMapYIDX );
	

	return true;
}


//-------------------------------------------------------------------------------------------------
CMAP_PATCH* CTERRAIN::GetPATCH(int iZonePatchX, int iZonePatchY)
{
	CMAP *pMAP;

	pMAP = m_pMAPS[ iZonePatchY/PATCH_COUNT_PER_MAP_AXIS ][ iZonePatchX/PATCH_COUNT_PER_MAP_AXIS ];
	if ( pMAP )
		return pMAP->GetPATCH( iZonePatchX%PATCH_COUNT_PER_MAP_AXIS, iZonePatchY%PATCH_COUNT_PER_MAP_AXIS );

	return NULL;
}


/// 단순 지형 높이만 계산(오브젝트 높이는 제외)
/// @pNotFound 지형 패치를 찾을 수 없으면 false
/// @return 계산된 높이. 패치를 못 찾았을 시에는 0
float CTERRAIN::GetHeight (float fWorldX, float fWorldY, bool * pNotFound)
{
	CMAP *pMAP = GetMapPtrByWorldCoordinates( fWorldX, fWorldY );
	if ( !pMAP ) {
		if (pNotFound) {
			*pNotFound = true;
		}
		return 0.0f;
	}
	if (pNotFound) {
		*pNotFound = false;
	}
    return pMAP->GetHEIGHT (fWorldX, fWorldY);
}

float CTERRAIN::GetHeightTop (float fWorldX, float fWorldY, bool * pNotFound)
{
	float fTerrainHeight = GetHeight( fWorldX, fWorldY, pNotFound );

	float fHeight = getWorldObjectHeightInScene( fWorldX, fWorldY, fTerrainHeight );

	return (fHeight > fTerrainHeight) ? fHeight : fTerrainHeight; // 지형 높이와 오브젝트 높이 중 더 높은 것 선택
}

CMAP* CTERRAIN::GetMapPtrByWorldCoordinates (float fWorldX, float fWorldY)
{
	short nZoneMapX, nZoneMapY;
	short nZonePatchX, nZonePatchY;

	nZonePatchX = (short) (fWorldX / (GRID_COUNT_PER_PATCH_AXIS * nGRID_SIZE ) );
	nZonePatchY = (short) (fWorldY / (GRID_COUNT_PER_PATCH_AXIS * nGRID_SIZE ) );

	nZoneMapX   = nZonePatchX / PATCH_COUNT_PER_MAP_AXIS;
	nZoneMapY   = nZonePatchY / PATCH_COUNT_PER_MAP_AXIS;

	_ASSERT( nZonePatchX >= 0 );
	_ASSERT( nZonePatchY >= 0 );

	if( nZoneMapX >= MAP_COUNT_PER_ZONE_AXIS || nZoneMapY >= MAP_COUNT_PER_ZONE_AXIS )
	{
		_ASSERT( 0 && "nZoneMapX >= MAP_COUNT_PER_ZONE_AXIS || nZoneMapY >= MAP_COUNT_PER_ZONE_AXIS" );
		return NULL;
	}

	CMAP *pMAP;
	pMAP = m_pMAPS[ nZoneMapY ][ nZoneMapX ];
	return pMAP;
}

float CTERRAIN::GetSlope (float fWorldX, float fWorldY, bool * pNotFound)
{
	CMAP * pMAP = GetMapPtrByWorldCoordinates( fWorldX, fWorldY );

	if ( !pMAP ) {
		if (pNotFound) {
			*pNotFound = true;
		}
		return 0.0f;
	}
	if (pNotFound) {
		*pNotFound = false;
	}
    return pMAP->GetSlope(fWorldX, fWorldY);
}

bool CTERRAIN::GetNormal (float fWorldX, float fWorldY, D3DXVECTOR3& vNormal)
{
	CMAP * pMAP = GetMapPtrByWorldCoordinates( fWorldX, fWorldY );

	if ( !pMAP ) {
		return false;
	}
    pMAP->GetNormal(fWorldX, fWorldY, vNormal);
	return true;
}


//-------------------------------------------------------------------------------------------------
float CTERRAIN::Pick_POSITION (D3DXVECTOR3 &PosPICK)
{
	return m_PatchManager.Pick_POSITION( PosPICK );
}

//-------------------------------------------------------------------------------------------------
float CTERRAIN::Pick_POSITION_Test (D3DXVECTOR3 &PosPICK)
{
	m_PatchManager.CalculatePickingPATCH();
	
	
	return 0.0f; //m_PatchManager.Pick_POSITION( PosPICK );
}

//----------------------------------------------------------------------------------------------------
/// 월드에서 오브젝트를 선택한다.
///	m_CnstLIST 에는 캐릭터를 제외한, 건물, 나무, 풀, 아이템 등이 들어있고
/// m_CharLIST 에는 캐릭터 들이 들어 있지..
//----------------------------------------------------------------------------------------------------

short CTERRAIN::Pick_NearObject( D3DXVECTOR3 &PosPICK, float &fDistance )
{
	CGameOBJ*	pObj			= NULL;
	HNODE		hNode			= 0;
	HNODE		hUserData		= 0;	
	int			iPickObject		= 0;

	D3DVECTOR vecCenter			= g_pAVATAR->Get_CurPOS();
	int iNodeCnt				= ::collectBySphere( (float *)( &vecCenter ), 1000 );

	float		fCurDis			= g_fMaxDistance;
	fDistance					= g_fMaxDistance;

	for( int i = 0; i < iNodeCnt; i++ )
	{
		hNode = ::getCollectNode( i );
		hUserData = ::getUserData( hNode );

		if( hUserData )
		{
			pObj = reinterpret_cast< CGameOBJ* >( hUserData );

			switch( pObj->Get_TYPE() )
			{				
				case OBJ_USER:
					break;
				case OBJ_AVATAR:
				case OBJ_NPC:
				case OBJ_MOB:
				case OBJ_CART:
				case OBJ_CGEAR:
					{
						if( pObj->IsIntersect( fCurDis ) )
						{
							PosPICK = pObj->Get_PickPOSITION();
							if ( fCurDis < fDistance ) 
							{
								fDistance = fCurDis;
								iPickObject = pObj->Get_INDEX();
							}
						}
					}
					break;
				case OBJ_ITEM:			
				case OBJ_GROUND:
				case OBJ_CNST:
					{
						if( pObj->IsIntersectForCamera( fCurDis ) )
						{
							PosPICK = pObj->Get_PickPOSITION();
							if ( fCurDis < fDistance ) 
							{
								fDistance = fCurDis;
								iPickObject = pObj->Get_INDEX();
							}
						}
					}
					break;
					
			}
		}
	}

	return iPickObject;
}

//----------------------------------------------------------------------------------------------------
/// 월드에서 오브젝트를 선택한다.
///	m_CnstLIST 에는 캐릭터를 제외한, 건물, 나무, 풀, 아이템 등이 들어있고
/// m_CharLIST 에는 캐릭터 들이 들어 있지..
//----------------------------------------------------------------------------------------------------

short CTERRAIN::Pick_OBJECT (D3DXVECTOR3 &PosPICK, float &fDistance)
{
//	PosPICK.x;
//	PosPICK.y;

	D3DXVECTOR3 PosTMP;
	float fCurDis = g_fMaxDistance;
	fDistance = g_fMaxDistance;
	int iPickObject = 0;
	classDLLNODE< int > *pNODE;

	//----------------------------------------------------------------------------------------------------
	/// 먼저 정적 객체들..
	//----------------------------------------------------------------------------------------------------
	pNODE = g_pObjMGR->m_CnstLIST.GetHeadNode ();
	while( pNODE ) 
	{
		_ASSERT( pNODE->DATA >= 0 && pNODE->DATA < MAX_CLIENT_OBJECTS );
		if( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ] != NULL )
		{
			if( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->IsInViewFrustum() )
			{
				if ( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->IsIntersectForCamera(fCurDis) )
				{
					PosPICK = g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->Get_PickPOSITION ();
					if ( fCurDis < fDistance ) {
						fDistance = fCurDis;
						iPickObject = pNODE->DATA;
					}
				}		
			}
		} else
		{
			_ASSERT( 0 && "g_pObjMGR->m_pOBJECTS[ pNODE->DATA ] == NULL" );
		}

		pNODE = g_pObjMGR->m_CnstLIST.GetNextNode( pNODE );
	}

	//----------------------------------------------------------------------------------------------------
	/// 아이템 객체들..
	//----------------------------------------------------------------------------------------------------
	pNODE = g_pObjMGR->m_ItemLIST.GetHeadNode ();
	while( pNODE ) 
	{
		_ASSERT( pNODE->DATA >= 0 && pNODE->DATA < MAX_CLIENT_OBJECTS );
		if( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ] != NULL )
		{
			if( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->IsInViewFrustum() )
			{
				if ( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->IsIntersectForCamera(fCurDis) )
				{
					PosPICK = g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->Get_PickPOSITION ();
					if ( fCurDis < fDistance ) {
						fDistance = fCurDis;
						iPickObject = pNODE->DATA;
					}
				}		
			}
		} else
		{
			_ASSERT( 0 && "g_pObjMGR->m_pOBJECTS[ pNODE->DATA ] == NULL" );
		}

		pNODE = g_pObjMGR->m_ItemLIST.GetNextNode( pNODE );
	}

	//----------------------------------------------------------------------------------------------------
	// 캐릭터들..
	//----------------------------------------------------------------------------------------------------
	pNODE = g_pObjMGR->m_CharLIST.GetHeadNode ();
	while( pNODE ) 
	{

//=====================================================================================
//2005 6. 9  최종진	/ 박지호		
		CObjCHAR* pChar = (CObjCHAR*)(g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]);
		if(::getVisibility( pChar->GetZMODEL() ) == 0)
		{	pNODE = g_pObjMGR->m_CharLIST.GetNextNode( pNODE );  continue; }

//박지호 :: 카트는 피킹을 하지않는다. 
		if(pChar->Get_TYPE() == OBJ_CART)
		{	pNODE = g_pObjMGR->m_CharLIST.GetNextNode( pNODE );  continue; }
//=====================================================================================
		
		_ASSERT( pNODE->DATA >= 0 && pNODE->DATA < MAX_CLIENT_OBJECTS );
		if ( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->Get_TYPE() != OBJ_USER ) 
		{
			if( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->IsInViewFrustum() )
			{
				// 자기 아바타는 클릭되지 않도록...
				if ( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->IsIntersect( fCurDis ) ) 
				{
					PosPICK = g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->Get_PickPOSITION ();
					if ( fCurDis < fDistance ) {
						fDistance = fCurDis;
						iPickObject = pNODE->DATA;
					}
				}

				/// 현재 내가 아바타 일경우에..
				if( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ]->IsA( OBJ_AVATAR ) )
				{
					CObjAVT* pAvt = (CObjAVT*)( g_pObjMGR->m_pOBJECTS[ pNODE->DATA ] );
					if( pAvt->IsIntersectAccessory( fCurDis ) )
					{
						assert( pAvt->m_pObjPersonalStore );
						PosPICK = pAvt->m_pObjPersonalStore->Get_PickPOSITION();
						if ( fCurDis < fDistance ) 
						{
							fDistance = fCurDis;
							iPickObject = pNODE->DATA;
						}
					}
				}
			}
		}

		pNODE = g_pObjMGR->m_CharLIST.GetNextNode( pNODE );
	}

	return iPickObject;
}

//-------------------------------------------------------------------------------------------------
CMAP*CTERRAIN::GetEmptyMAP ()
{
	for (short nI=0; nI<MAX_MAP_BUFFER; nI++) 
	{
		if ( m_pMapBUFFER[ nI ]->IsEmpty () )
			return m_pMapBUFFER[ nI ];
	}

	/// Dirty맵을 번저 정리후에..
	FreeDirtyMAP();

	/// 비어있는 맵이 없을경우 DirtyMapList를 뒤진다.
	/// 제일 먼저 추가된 노드를 꺼내온다. ( 
	
	if( m_DirtyMapList.empty() )
		return NULL;

	CMAP* pMap = m_DirtyMapList.front();
	m_DirtyMapList.pop_front();

	///assert ( pMap->IsDirty () );	
	{	
		return pMap;
	}

	return NULL;
}
CMAP*CTERRAIN::FindLoadedMAP (short nZoneMapXIDX, short nZoneMapYIDX)
{
	for (short nI=0; nI<MAX_MAP_BUFFER; nI++) {
		if ( m_pMapBUFFER[ nI ]->IsEmpty () )
			continue;
	
		if ( m_pMapBUFFER[ nI ]->m_nZoneMapXIDX == nZoneMapXIDX &&
			 m_pMapBUFFER[ nI ]->m_nZoneMapYIDX == nZoneMapYIDX )
			return m_pMapBUFFER[ nI ];
	}

	return NULL;
}
void CTERRAIN::FreeDirtyMAP ()
{
	for (short nI=0; nI<MAX_MAP_BUFFER; nI++) 
	{
		if ( m_pMapBUFFER[ nI ] && m_pMapBUFFER[ nI ]->IsDirty () ) 
		{
			m_pMapBUFFER[ nI ]->ClearFromNZIN ();
			return;
		}
	}
}

CMAP*CTERRAIN::GetMapPTR (short nZoneMapX, short nZoneMapY)
{
	if ( nZoneMapX < 0 || nZoneMapX >= MAP_COUNT_PER_ZONE_AXIS )	return NULL;
	if ( nZoneMapY < 0 || nZoneMapY >= MAP_COUNT_PER_ZONE_AXIS )	return NULL;

#ifdef	_DEBUG
	if ( NULL == m_pMAPS[ nZoneMapY ][ nZoneMapX ] ) {
		LogString (LOG_DEBUG, " GetMapPTR:: Map[ %d, %d ] is NULL Filename( %s ) !!!!\n", nZoneMapX, nZoneMapY, this->GetMapFILE(nZoneMapX, nZoneMapY));
	}
#endif

	return m_pMAPS[ nZoneMapY ][ nZoneMapX ];
}
void CTERRAIN::SetMapPTR (short nZoneMapX, short nZoneMapY, CMAP* pMAP)
{
	m_pMAPS[ nZoneMapY ][ nZoneMapX ] = pMAP;

	if ( m_pMAPS[ nZoneMapY ][ nZoneMapX ] ) 
	{
		pMAP->SetUseMODE( MAP_USING );
			LogString (LOG_DEBUG, " SetMapPTR:: Map[ %d, %d ] !!!!\n", nZoneMapX, nZoneMapY);
	} 
#ifdef	_DEBUG
	else {
		LogString (LOG_DEBUG, " SetMapPTR:: Map[ %d, %d ] is NULL Filename !!!!\n", nZoneMapX, nZoneMapY );
	}
#endif
}

//-------------------------------------------------------------------------------------------------
///
/// 업데이트 대상인 맵을 뺀다..
///
void CTERRAIN::SubMAP (WORD wUpdateFLAG)
{
	short nI, nZoneMapXIDX, nZoneMapYIDX;
	CMAP *pMAP;

	for (nI=0; s_SubSecIdx[ wUpdateFLAG ][ nI ]>=0; nI++) {
		nZoneMapXIDX = m_nCenterMapXIDX + s_SecAdjPos[ s_SubSecIdx[ wUpdateFLAG ][ nI ] ].m_nX;
		nZoneMapYIDX = m_nCenterMapYIDX + s_SecAdjPos[ s_SubSecIdx[ wUpdateFLAG ][ nI ] ].m_nY;

		if ( nZoneMapXIDX >= 0 && nZoneMapXIDX < MAP_COUNT_PER_ZONE_AXIS &&
			 nZoneMapYIDX >= 0 && nZoneMapYIDX < MAP_COUNT_PER_ZONE_AXIS ) {

			pMAP= m_pMAPS[ nZoneMapYIDX ][ nZoneMapXIDX ];
			if ( pMAP ) 
			{
				LogString (LOG_DEBUG, "		FreeMAP: (%d, %d) --> ( %d, %d )\n", s_SecAdjPos[ s_SubSecIdx[ wUpdateFLAG ][ nI ] ].m_nX, s_SecAdjPos[ s_SubSecIdx[ wUpdateFLAG ][ nI ] ].m_nY, nZoneMapXIDX, nZoneMapYIDX);
				pMAP->Free ();				
		
				m_DirtyMapList.push_back( pMAP );

				/// 점진적 로딩 리스트에 들어있다면 빼라...
				{
					tagLOAD_ONE_MAP_DATA* pOneMapData = NULL;

					std::list< tagLOAD_ONE_MAP_DATA* >::iterator begin = m_LoadOneMapData.begin();

//					for( ; begin != m_LoadOneMapData.begin(); ++begin )
					for( ; begin != m_LoadOneMapData.end();  )///<= 2004 / 2 / 2 : nAvy 수정
					{
						pOneMapData = *begin;

						short nZoneXIDX = pOneMapData->m_nCenterX + s_SecAdjPos[ pOneMapData->m_nUpdateIndex ].m_nX;
						short nZoneYIDY = pOneMapData->m_nCenterY + s_SecAdjPos[ pOneMapData->m_nUpdateIndex ].m_nY;

						/// 지워야 되는데 점진적 로딩 리스트에 있다면 뺀다..
						if( nZoneXIDX == nZoneMapXIDX && nZoneYIDY == nZoneMapYIDX )
						{
							begin = m_LoadOneMapData.erase( begin );

							delete pOneMapData;
							pOneMapData = NULL;
						}
						else				///<= 2004 / 2/ 2 : nAvy 추가
						{					///if( --begin == end() )일경우 erase 하고 ++begin하면 부적절한 iter가 된다. 
							++begin;		///
						}					///
					}									
				}


			}
			m_pMAPS[ nZoneMapYIDX ][ nZoneMapXIDX ] = NULL;
		}
	}	
}

///
/// 이미 로딩해야될 리스트에 있는가?
///
tagLOAD_ONE_MAP_DATA* CTERRAIN::GetLodingMapFromList( short nCenterMapXIDX, short nCenterMapYIDX, WORD wUpdateIndex )
{
	short nZoneMapXIDX = nCenterMapXIDX + s_SecAdjPos[ wUpdateIndex ].m_nX;
	short nZoneMapYIDX = nCenterMapYIDX + s_SecAdjPos[ wUpdateIndex ].m_nY;	

	std::list< tagLOAD_ONE_MAP_DATA* >::iterator begin = m_LoadOneMapData.begin();

	tagLOAD_ONE_MAP_DATA* pOneMapData = NULL;

	for( ; begin != m_LoadOneMapData.end() ; ++begin )
	{
		pOneMapData = *begin;

		short nZoneX = pOneMapData->m_nCenterX + s_SecAdjPos[ pOneMapData->m_nUpdateIndex ].m_nX;
		short nZoneY = pOneMapData->m_nCenterY + s_SecAdjPos[ pOneMapData->m_nUpdateIndex ].m_nY;

		/// 이미 리스트에 있다면..
		if( nZoneX == nZoneMapXIDX && nZoneY == nZoneMapYIDX )
			return pOneMapData;
	}

	return NULL;
}

///
/// 한번에 하나의 맵추가..
///
bool CTERRAIN::AddOneMap(short nCenterMapXIDX,short nCenterMapYIDX,WORD wUpdateIndex)
{
	short nZoneMapXIDX, nZoneMapYIDX;
	CMAP *pMAP;
	char *szMapFile;

	nZoneMapXIDX = nCenterMapXIDX + s_SecAdjPos[ wUpdateIndex ].m_nX;
	nZoneMapYIDX = nCenterMapYIDX + s_SecAdjPos[ wUpdateIndex ].m_nY;	

	if ( nZoneMapXIDX >= 0 && nZoneMapXIDX < MAP_COUNT_PER_ZONE_AXIS &&
		 nZoneMapYIDX >= 0 && nZoneMapYIDX < MAP_COUNT_PER_ZONE_AXIS ) 
	{

		LogString (LOG_DEBUG, "Add One MAP: (%d, %d) --> ( %d, %d ) \n", s_SecAdjPos[ wUpdateIndex ].m_nX, 
																s_SecAdjPos[ wUpdateIndex ].m_nY, 
																nZoneMapXIDX, nZoneMapYIDX );

		pMAP = this->FindLoadedMAP (nZoneMapXIDX, nZoneMapYIDX);
		/// 이미 로드된 맵이라면..
		if ( pMAP ) 
		{
			this->SetMapPTR(nZoneMapXIDX, nZoneMapYIDX, pMAP );
		} else 
		{
			pMAP = this->GetEmptyMAP ();
			if ( pMAP ) 
			{
				szMapFile = GetMapFILE( nZoneMapXIDX, nZoneMapYIDX );
				if ( szMapFile ) 
				{
					this->SetMapPTR(nZoneMapXIDX, nZoneMapYIDX, pMAP->Load( szMapFile, nZoneMapXIDX, nZoneMapYIDX) );					
				} else {
					// 맵없다.
					LogString (LOG_DEBUG, "Map not found ... \n" );
				}
			}else
			{
				assert( 0 && "GetEmptyMAP() returns invalid map[ AddOneMap ]" );
			}
		}
	}
	
	return true;
}

///
/// 업데이트 대상인 맵을 더한다..
///
bool CTERRAIN::AddMAP (short nCenterMapXIDX, short nCenterMapYIDX, WORD wUpdateFLAG, bool bAllUpdate, bool bImmediateLoad )
{
	short nI, nZoneMapXIDX, nZoneMapYIDX;
	CMAP *pMAP;
	char *szMapFile;

	if( !bImmediateLoad )
	{
		tagLOAD_ONE_MAP_DATA* pOneMapData = NULL;
		for (nI=0; s_AddSecIdx[ wUpdateFLAG ][ nI ]>=0; nI++) 
		{			
			/// 이미 그 맵을 로딩해야할 리스트에 넣었다면 그 데이터를 갱신하고 리턴..
			tagLOAD_ONE_MAP_DATA* pOneMapData = GetLodingMapFromList( nCenterMapXIDX, nCenterMapYIDX, s_AddSecIdx[ wUpdateFLAG ][ nI ] );
			if( pOneMapData )
			{				
				pOneMapData->m_nCenterX = nCenterMapXIDX;
				pOneMapData->m_nCenterY = nCenterMapYIDX;
				pOneMapData->m_nUpdateIndex = s_AddSecIdx[ wUpdateFLAG ][ nI ];
				pOneMapData->m_bImmediateLoad = bImmediateLoad;		

				
			}else
			{
				pOneMapData = new tagLOAD_ONE_MAP_DATA;
				pOneMapData->m_nCenterX = nCenterMapXIDX;
				pOneMapData->m_nCenterY = nCenterMapYIDX;
				pOneMapData->m_nUpdateIndex = s_AddSecIdx[ wUpdateFLAG ][ nI ];
				pOneMapData->m_bImmediateLoad = bImmediateLoad;

				short nZoneX = pOneMapData->m_nCenterX + s_SecAdjPos[ pOneMapData->m_nUpdateIndex ].m_nX;
				short nZoneY = pOneMapData->m_nCenterY + s_SecAdjPos[ pOneMapData->m_nUpdateIndex ].m_nY;

				LogString (LOG_DEBUG, "LoadOneMapData: [ %d, %d ] (%d, %d) --> ( %d, %d ) \n", 
									nCenterMapXIDX, nCenterMapYIDX,
									s_SecAdjPos[ pOneMapData->m_nUpdateIndex ].m_nX, 
									s_SecAdjPos[ pOneMapData->m_nUpdateIndex ].m_nY, 
									nZoneX, nZoneY );

				m_LoadOneMapData.push_back( pOneMapData );
			}
		}
	}else
	/// 다이렉트 로딩
	{	

		for (nI=0; s_AddSecIdx[ wUpdateFLAG ][ nI ]>=0; nI++) {
			nZoneMapXIDX = nCenterMapXIDX + s_SecAdjPos[ s_AddSecIdx[ wUpdateFLAG ][ nI ] ].m_nX;
			nZoneMapYIDX = nCenterMapYIDX + s_SecAdjPos[ s_AddSecIdx[ wUpdateFLAG ][ nI ] ].m_nY;

			if ( nZoneMapXIDX >= 0 && nZoneMapXIDX < MAP_COUNT_PER_ZONE_AXIS &&
				 nZoneMapYIDX >= 0 && nZoneMapYIDX < MAP_COUNT_PER_ZONE_AXIS ) {

				LogString ( LOG_DEBUG, "AddMAP: (%d, %d) --> ( %d, %d ) \n", s_SecAdjPos[ s_AddSecIdx[ wUpdateFLAG ][ nI ] ].m_nX, s_SecAdjPos[ s_AddSecIdx[ wUpdateFLAG ][ nI ] ].m_nY, nZoneMapXIDX, nZoneMapYIDX);

				pMAP = this->FindLoadedMAP (nZoneMapXIDX, nZoneMapYIDX);
				/// 이미 로드된 맵이라면..
				if ( pMAP ) {
					this->SetMapPTR(nZoneMapXIDX, nZoneMapYIDX, pMAP );
				} else {
					pMAP = this->GetEmptyMAP ();
					if ( pMAP ) {
						szMapFile = GetMapFILE( nZoneMapXIDX, nZoneMapYIDX );
						if ( szMapFile ) 
						{							
							this->SetMapPTR(nZoneMapXIDX, nZoneMapYIDX, pMAP->Load( szMapFile, nZoneMapXIDX, nZoneMapYIDX) );
							if ( !bAllUpdate )
								return false;
						} else {
							// 맵없다.
							LogString (LOG_DEBUG, "Map not found ... \n" );
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

	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 
//----------------------------------------------------------------------------------------------------

void CTERRAIN::Insert_VisiblePatch (CMAP_PATCH *pPATCH)
{
	m_PatchManager.Insert_VisiblePatch( pPATCH );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CTERRAIN::Delete_UnvisiblePatch (void)
{
	m_PatchManager.Delete_UnvisiblePatch( );
}


void CTERRAIN::Clear_VisiblePatch (void)
{
	m_PatchManager.Clear_VisiblePatch();
}


//-------------------------------------------------------------------------------------------------
/// CTERRAIN::
/// @param fWorldX
/// @param fWorldY
/// @brief pOneMapData->m_bImmediateLoad 에 따라 setDelayedLoad 함수를 설정한다.( 04/4/29 )
///			매프레임 카메라의 위치에 따라 맵을 갱신할지를 결정한다.
//-------------------------------------------------------------------------------------------------

void CTERRAIN::SetCenterPosition (float fWorldX, float fWorldY)
{
	static int iCheckFrameCount = 0;
	iCheckFrameCount++;

	///// 패치 재구성	
	bool	   bPatchOrganizing =false;	

	FreeDirtyMAP ();

	//----------------------------------------------------------------------------------------------------	
	/// @brief Delayed load 가 세팅되었을경우.. 맵 로딩 큐가 비어 있지 않다면.. 먼저 그맵을 읽는다.
	//----------------------------------------------------------------------------------------------------
	if( !m_LoadOneMapData.empty() && (( iCheckFrameCount % 5 ) == 0))
	{
		tagLOAD_ONE_MAP_DATA* pOneMapData = NULL;
		
		pOneMapData = m_LoadOneMapData.front();
		m_LoadOneMapData.pop_front();

		if( pOneMapData->m_bImmediateLoad )
		{
			/// Loading mode change
			::setDelayedLoad( false );
		}else
			::setDelayedLoad( true );

		AddOneMap( pOneMapData->m_nCenterX, pOneMapData->m_nCenterY, pOneMapData->m_nUpdateIndex );

		::setDelayedLoad( true );

		
		delete pOneMapData;
		pOneMapData = NULL;		

		/// 패치 재구성
		bPatchOrganizing = true;
	}	


	g_GameDATA.m_PosCENTER.x = fWorldX;
	g_GameDATA.m_PosCENTER.y = fWorldY;


	WORD  wUpdateFLAG = 0;
	short nZoneMapX, nZoneMapY;
	short nZonePatchX, nZonePatchY;

	//										4						250
	int iWorldX=(int)fWorldX;
	int iWorldY=(int)fWorldY;
	nZonePatchX = (short) (fWorldX / (GRID_COUNT_PER_PATCH_AXIS * nGRID_SIZE ) );
	nZonePatchY = (short) (fWorldY / (GRID_COUNT_PER_PATCH_AXIS * nGRID_SIZE ) );
	nZoneMapX   = nZonePatchX / PATCH_COUNT_PER_MAP_AXIS;
	nZoneMapY   = nZonePatchY / PATCH_COUNT_PER_MAP_AXIS;

	g_GameDATA.m_PosPATCH.x = nZonePatchX % PATCH_COUNT_PER_MAP_AXIS;
	g_GameDATA.m_PosPATCH.y = nZonePatchY % PATCH_COUNT_PER_MAP_AXIS;


	if ( nZoneMapX < m_nCenterMapXIDX ) 
	{
		wUpdateFLAG |= MOVE_UPDATE_LEFT;
	} else
	if ( nZoneMapX > m_nCenterMapXIDX ) 
	{
		wUpdateFLAG |= MOVE_UPDATE_RIGHT;
	}

	if ( nZoneMapY < m_nCenterMapYIDX ) 
	{
		wUpdateFLAG |= MOVE_UPDATE_UP;
	} else 
	if ( nZoneMapY > m_nCenterMapYIDX ) 
	{
		wUpdateFLAG |= MOVE_UPDATE_DOWN;
	}


	short nMappingX, nMappingY;
	nMappingX = PATCH_COUNT_PER_MAP_AXIS + nZonePatchX - ( nZoneMapX*PATCH_COUNT_PER_MAP_AXIS);
	nMappingY = PATCH_COUNT_PER_MAP_AXIS + nZonePatchY - ( nZoneMapY*PATCH_COUNT_PER_MAP_AXIS);


	if ( wUpdateFLAG ) 
	{
		LogString (LOG_DEBUG, "\n=====================================================\n" );

		LogString (LOG_DEBUG, "(%d,%d)-->(%d,%d)    mapping ( %d, %d )\n", 
			m_nCenterMapXIDX, m_nCenterMapYIDX,
			nZoneMapX, nZoneMapY,
			nMappingX, nMappingY);

		LogString (LOG_DEBUG, "=====================================================\n\n" );


		//----------------------------------------------------------------------------------------------------
		/// Sub map
		//----------------------------------------------------------------------------------------------------
		SubMAP( wUpdateFLAG );		
		m_WideTerrain.SubRoughMap( wUpdateFLAG );

		m_nCenterMapXIDX = nZoneMapX;
		m_nCenterMapYIDX = nZoneMapY;

		//----------------------------------------------------------------------------------------------------
		/// Add map
		//----------------------------------------------------------------------------------------------------
		AddMAP( nZoneMapX, nZoneMapY, wUpdateFLAG, true, false );		
		m_WideTerrain.AddRoughMap( nZoneMapX, nZoneMapY, wUpdateFLAG );

		/// 패치 재구성
		bPatchOrganizing = true;
	}		

///*	----------------------------------------------------------------------------------------------------------
	
	/// 패치 재구성
	//if( bPatchOrganizing )
	//{
	//	/// 현재 위치를 기반으로 재구성된 맵들에 기반해서 패치풀을 재구성한다.
	//	ReOrginazationPatch( nZoneMapX, nZoneMapY );		
	//}
	//
	//	
	//Update_VisiblePatch( nMappingX, nMappingY );	
	
// 일단 제외	   
//*/   -----------------------------------------------------------------------------------------------------------    

	
	if( true )
	{
	  m_PatchManager.ClearAllQuadPatchManager();

      
	  UpdatePatchManager( nZoneMapX, nZoneMapY );
      InsertCameraPatch(fWorldX,fWorldY);
	  
	  m_PatchManager.Update_VisiblePatchManager();
/*	  D3DXVECTOR3 vec(0.0f,0.0f,0.0f);
	  Pick_POSITION_Test(vec);  */               //test 12_15
	}
		/// 엔진에 등록될 패치 업데이트
	

//	m_PatchManager.DrawCollisionCylinder();
	

/*-----------------------------------------------------------------------------------------------------*/ //test 1-5
/*
	short buffer_index[2];
	CMAP *pMap;
    float vMin[3],vMax[3];
    aabbBox *aabb; 

	for(int aa=0; aa<3 ; aa+=1)
	{
    	for(int bb=0; bb<3; bb+=1)
		{
			buffer_index[0] = nZoneMapX-1+aa;
			buffer_index[1] = nZoneMapY-1+bb;

			
			if(m_pMAPS[ buffer_index[1] ][ buffer_index[0] ]!=NULL)
			{
				if(m_pMAPS[ buffer_index[1] ][ buffer_index[0] ]->IsUsing())
				{
					pMap = m_pMAPS[ buffer_index[1] ][ buffer_index[0] ];
					aabb = &pMap->m_QuadManager.m_quadPATCH[0].aabb;
					vMin[0] = aabb->x[0]; vMin[1] = aabb->y[0]; vMin[2] = aabb->z[0];
					vMax[0] = aabb->x[1]; vMax[1] = aabb->y[1];	vMax[2] = aabb->z[1];

					::InputSceneAABB(vMin,vMax,0);
     			}
			}
		}
	}

/*----------------------------------------------------------------------------------------------------*/ //test 1-5


	m_PatchManager.IncreaseViewFrame();
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 위치를 기반으로 재구성된 맵들에 기반해서 패치풀을 재구성한다.
//----------------------------------------------------------------------------------------------------

void CTERRAIN::ReOrginazationPatch( short nZoneMapX, short nZoneMapY )
{
	short nX, nZ;
	short nZoneMapXIDX = 0, nZoneMapYIDX = 0;
	CMAP*	pMap = NULL;

	
	for (nZ=-1; nZ<=1; nZ++) 
	{
		nZoneMapYIDX = nZ + nZoneMapY;
		for (nX=-1; nX<=1; nX++) 
		{
			nZoneMapXIDX = nX + nZoneMapX;

			pMap = this->GetMapPTR (nZoneMapXIDX, nZoneMapYIDX);
			this->Lock ();

			if ( pMap ) 
			{
				pMap->MappingToZONE( 1+nX, 1+nZ );
			} else {
				short nViewPatchX = (1+nX) * PATCH_COUNT_PER_MAP_AXIS;
				short nViewPatchY = (1+nZ) * PATCH_COUNT_PER_MAP_AXIS;

				LogString (LOG_DEBUG, "NULL MappingToZONE::( %d, %d ) \n", nViewPatchX, nViewPatchY);

				int iH, iW;
				for (iH=0; iH<PATCH_COUNT_PER_MAP_AXIS; iH++)
				{
					for (iW=0; iW<PATCH_COUNT_PER_MAP_AXIS; iW++)
					{
						this->SetPATCH( nViewPatchX+iW, nViewPatchY+iH, NULL );
					}
				}
			}
			this->Unlock ();
		}
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 위치에 기반해서 엔진에 등록된 패치들을 갱신한다.
//----------------------------------------------------------------------------------------------------

void CTERRAIN::Update_VisiblePatch( short nMappingX, short nMappingY )
{
	this->Lock ();	
	m_PatchManager.Update_VisiblePatch( nMappingX, nMappingY );
	this->Unlock ();	
}


///
/// 현재 월드 위치로 해당 패치를 구하기
///
CMAP_PATCH*	CTERRAIN::GetPatchFromWorld( float fWorldX, float fWorldY )
{
	int iWorldX=(int)fWorldX;
	int iWorldY=(int)fWorldY;
	int iZonePatchX = (short) (fWorldX / (GRID_COUNT_PER_PATCH_AXIS * nGRID_SIZE ) );
	int iZonePatchY = (short) (fWorldY / (GRID_COUNT_PER_PATCH_AXIS * nGRID_SIZE ) );
	int iZoneMapX   = iZonePatchX / PATCH_COUNT_PER_MAP_AXIS;
	int iZoneMapY   = iZonePatchY / PATCH_COUNT_PER_MAP_AXIS;

	CMAP*	pMap = this->GetMapPTR ( iZoneMapX, iZoneMapY );
	if( pMap == NULL )
		return NULL;

	return pMap->GetPatchFromWorld( fWorldX, fWorldY );
}




void CTERRAIN::UpdatePatchManager(short nCenterMapXIDX, short nCenterMapYIDX)
{

    short nI, nZoneMapXIDX, nZoneMapYIDX, buffer_X,buffer_Y;
	
	for (nI=0; s_AddSecIdx[ MOVE_UPDATE_ALL ][ nI ]>=0; nI++) {
			
		
		    buffer_X=s_SecAdjPos[ s_AddSecIdx[ MOVE_UPDATE_ALL ][ nI ] ].m_nX;
			buffer_Y=s_SecAdjPos[ s_AddSecIdx[ MOVE_UPDATE_ALL ][ nI ] ].m_nY;
		
		    nZoneMapXIDX = nCenterMapXIDX + buffer_X;
			nZoneMapYIDX = nCenterMapYIDX + buffer_Y;

			if ( nZoneMapXIDX >= 0 && nZoneMapXIDX < MAP_COUNT_PER_ZONE_AXIS &&
				 nZoneMapYIDX >= 0 && nZoneMapYIDX < MAP_COUNT_PER_ZONE_AXIS ) {

					 if(m_pMAPS[nZoneMapYIDX][nZoneMapXIDX]!=NULL){
						 if(m_pMAPS[nZoneMapYIDX][nZoneMapXIDX]->IsUsing()){
                         m_PatchManager.m_ppQuadPatchManager[buffer_Y+1][buffer_X+1]=(m_pMAPS[nZoneMapYIDX][nZoneMapXIDX]->GetQaudManager());
					     m_PatchManager.m_isUse[buffer_Y+1][buffer_X+1]=TRUE;
						 }
					 }
					 else{
                         m_PatchManager.m_ppQuadPatchManager[buffer_Y+1][buffer_X+1]=NULL;
				         m_PatchManager.m_isUse[buffer_Y+1][buffer_X+1]=FALSE;
					 }
				 
				 }
			else{
                 
				m_PatchManager.m_ppQuadPatchManager[buffer_Y+1][buffer_X+1]=NULL;
                m_PatchManager.m_isUse[buffer_Y+1][buffer_X+1]=FALSE;
			
			}
	
	
	}
	

}

void CTERRAIN::InsertCameraPatch(float xChar,float yChar)
{

    int nIndex,nX,nY,map_center[2],patch_center[2],patch_position[2];	
    float position[3];
    HNODE camera_node;
	  
   	camera_node=g_pCamera->GetZHANDLE();
    getCameraEye(camera_node,position);

	patch_center[0]=(int)(xChar/1000.0f);
	patch_center[1]=(int)(yChar/1000.0f);

	for ( nIndex=g_pCRange->GetStartIndex(0); nIndex<g_pCRange->GetStartIndex( 4 ); nIndex++) 
	{
		nX = patch_center[0] + g_pCRange->m_pPOINTS[ nIndex ].x;
		nY = patch_center[1] + g_pCRange->m_pPOINTS[ nIndex ].y;

	    map_center[0]=nX/16;
		map_center[1]=nY/16;
   
        if(m_pMAPS[map_center[1]][map_center[0]]!=NULL && m_pMAPS[map_center[1]][map_center[0]]->IsUsing())
		{
          patch_position[0]=nX-16*map_center[0];
		  patch_position[1]=nY-16*map_center[1];

          m_PatchManager.InsertCameraPatch(m_pMAPS[map_center[1]][map_center[0]]->GetPATCH(patch_position[0],patch_position[1]));
 		
		}
		 
	}	
	

	/*
	
	patch_center[0]=(int)(position[0]/1000.0f);
	patch_center[1]=(int)(position[1]/1000.0f);

	for ( nIndex=g_pCRange->GetStartIndex(0); nIndex<g_pCRange->GetStartIndex( 3 ); nIndex++) 
	{
		nX = patch_center[0] + g_pCRange->m_pPOINTS[ nIndex ].x;
		nY = patch_center[1] + g_pCRange->m_pPOINTS[ nIndex ].y;

	    map_center[0]=nX/16;
		map_center[1]=nY/16;
   
        if(m_pMAPS[map_center[1]][map_center[0]]!=NULL && m_pMAPS[map_center[1]][map_center[0]]->IsUsing())
		{
          patch_position[0]=nX-16*map_center[0];
		  patch_position[1]=nY-16*map_center[1];

          m_PatchManager.InsertCameraPatch(m_pMAPS[map_center[1]][map_center[0]]->GetPATCH(patch_position[0],patch_position[1]));
 		
		}
		 
	}	

*/
	
}


//----------------------------------------------------------------------------------------------------	
/// @brief Wide terrain
//----------------------------------------------------------------------------------------------------
void CTERRAIN::SubWideMap( WORD wUpdateFLAG )
{
	m_WideTerrain.SubRoughMap( wUpdateFLAG );
}


bool CTERRAIN::IsAgitZone()
{
	if( ZONE_PVP_STATE( g_pTerrain->GetZoneNO() ) == 11 )
		return true;

	return false;
}

bool CTERRAIN::IsPVPZone()
{
	switch( ZONE_PVP_STATE( g_pTerrain->GetZoneNO() ) )
	{
		case 0:
			return false;
		case 1:
		case 2:
			return true;
		case 11:
			return false;
		default:
			return false;
	}
	return false;
}
//-------------------------------------------------------------------------------------------------



