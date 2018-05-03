#include "stdafx.h"
#include ".\patchmanager.h"
#include "../Game.h"
#include "../IO_TERRAIN.h"
#include "CCamera.h"
#include "CClientStorage.h"
//#include "../../../engine/include/zz_interface.h"



CPatchManager::CPatchManager(void)
{
   
   lod_onoff=FALSE;	
   CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();

   if(pFileSystem->OpenFile("3DData\\Terrain\\LODType\\PatchType.lod", OPEN_READ_BIN))
   {

     int i,j;
	 char name[50];

	 pFileSystem->ReadStringByNull(name);
    
	 for(i=0;i<31;i+=1)
	 {
		for(j=0;j<31;j+=1)
		pFileSystem->ReadInt32(&patch_map[i][j]);	
          
	 }
     
	 pFileSystem->CloseFile();

   }

   (CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );
}

CPatchManager::~CPatchManager(void)
{
}

void CPatchManager::SetPATCH (int iX, int iY, CMAP_PATCH *pPATCH)
{
	m_ppPATCH[ iY ][ iX ] = pPATCH;
}




//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Visible patch리스트 갱신
//----------------------------------------------------------------------------------------------------

void CPatchManager::Update_VisiblePatch( short nMappingX, short nMappingY )
{
	short nIndex, nY, nX;	

	/// 실제로 엔진에서 사용될 패치들을 등록..
	for ( nIndex=g_pCRange->GetStartIndex(0); nIndex<g_pCRange->GetStartIndex( 22 ); nIndex++) 
	{
		nX = nMappingX + g_pCRange->m_pPOINTS[ nIndex ].x;
		nY = nMappingY + g_pCRange->m_pPOINTS[ nIndex ].y;

		if ( nX<0 || nX>=PATCH_COUNT_PER_MAP_AXIS*3 ||
			 nY<0 || nY>=PATCH_COUNT_PER_MAP_AXIS*3 ) continue;

		if ( m_ppPATCH[ nY ][ nX ] ) 
		{
			this->Insert_VisiblePatch( m_ppPATCH[ nY ][ nX ] );
			m_ppPATCH[ nY ][ nX ]->PlaySOUND ();
		}
	}	

//	Delete_UnvisiblePatch ();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Visible patch 엔진에 등록
//----------------------------------------------------------------------------------------------------
void CPatchManager::Insert_VisiblePatch (CMAP_PATCH *pPATCH)
{
	if ( pPATCH->m_wLastViewFRAME == 0 ) 
	{
		pPATCH->InsertToScene ();
		m_PatchLIST.AllocNAppend( pPATCH );
	}
	pPATCH->m_wLastViewFRAME = this->m_wViewFRAME;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 모든 Visible 제거
//----------------------------------------------------------------------------------------------------

void CPatchManager::Clear_VisiblePatch (void)
{
	classDLLNODE< CMAP_PATCH* > *pCurNode;

	pCurNode = m_PatchLIST.GetHeadNode ();
	while( pCurNode ) 
	{
		
		pCurNode->DATA->m_PreDrawingType=-1;
		pCurNode->DATA->RemoveFromScene ();
		m_PatchLIST.DeleteNFree( pCurNode );

		pCurNode = m_PatchLIST.GetHeadNode ();
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 유효하지 않은 patch 제거( time expire )
//----------------------------------------------------------------------------------------------------

void CPatchManager::Delete_UnvisiblePatch (void)
{
	classDLLNODE< CMAP_PATCH* > *pCurNode, *pDelNode;

	pCurNode = m_PatchLIST.GetHeadNode ();
	while( pCurNode ) {
		if ( pCurNode->DATA->m_wLastViewFRAME != this->m_wViewFRAME ) 
		{
			pCurNode->DATA->m_PreDrawingType=-1;
			pCurNode->DATA->RemoveFromScene ();
			pCurNode->DATA->m_wLastViewFRAME = 0;

			//pCurNode->DATA->InitMember();

 			pDelNode = pCurNode;
			pCurNode = m_PatchLIST.GetNextNode( pCurNode );
			m_PatchLIST.DeleteNFree( pDelNode );
			continue;
		}

		pCurNode = m_PatchLIST.GetNextNode( pCurNode );
	}	
    
	m_nDrawingPatch=m_PatchLIST.GetNodeCount(); 

}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재 피킹된 패치, 위치를 구한다.
//----------------------------------------------------------------------------------------------------

float CPatchManager::Pick_POSITION( D3DXVECTOR3 &PosPICK )
{
	// 속도 상향을 위해서 패치를 선별적으로 선택해서 조사해야...
	classDLLNODE< CMAP_PATCH* > *pCurNode;
    
	float fMinDis=-1, fCurDis=g_fMaxDistance;
	pCurNode = m_PatchLIST.GetHeadNode ();
	while( pCurNode ) 
	{
		if ( pCurNode->DATA->IsIntersect(fCurDis, PosPICK) ) {
			fMinDis = fCurDis;
		}

		pCurNode = m_PatchLIST.GetNextNode( pCurNode );
	    
	}

	return fMinDis;
}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  PatchManager에 참조된 *m_ppQuadPatchManager[3][3]을 모두 Clear함
//----------------------------------------------------------------------------------------------------

void CPatchManager::ClearAllQuadPatchManager()
{
   
	int i, j;

	m_nSubPATCH=0;
    m_nCameraPatch=0;

	for(i = 0; i < 3; i+=1)
	{
		for( j= 0; j < 3 ; j+=1)
		{

			m_ppQuadPatchManager[i][j]=NULL;

		}
	}



}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  PatchManager에 (y,x)위치의 *m_ppQuadPatchManager[y][x]를 참조  
//----------------------------------------------------------------------------------------------------
void CPatchManager::AddQuadPatchManager(CMAP *pMap,short y,short x)
{
     
	m_ppQuadPatchManager[y][x]=pMap->GetQaudManager();

}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief  참조된 각 *m_ppQuadPatchManager의 CMAP이 유용하면 ViewFrustumCulling을 해서 VisiblePatch를 얻음 
//----------------------------------------------------------------------------------------------------
void CPatchManager::CalculateViewFrustumCulling()
{
    int i,j;
	HNODE camera_node;
	float buffer_viewFrustumEq[6][4],buffer_pos[3];
	float buffer_length,camera_length;
    camera_node=g_pCamera->GetZHANDLE();
	
	getCameraViewfrustum(camera_node,buffer_viewFrustumEq);
    getCameraEye(camera_node,buffer_pos);

	t_OptionVideo option;
	g_ClientStorage.GetVideoOption(option);
//	g_ClientStorage.ApplyCameraOption( option.iCamera );
	
	buffer_length=buffer_viewFrustumEq[1][0]*buffer_pos[0]+buffer_viewFrustumEq[1][1]*buffer_pos[1]+buffer_viewFrustumEq[1][2]*buffer_pos[2]+buffer_viewFrustumEq[1][3];
    camera_length=CAMERA_MAX_RANGE( option.iCamera+ZONE_CAMERA_TYPE(g_pTerrain->GetZoneNO()) )*100.0f;
    buffer_viewFrustumEq[1][3]+=(-buffer_length-camera_length);

	for( i = 0; i < 3; i+=1){
       
		for( j = 0; j < 3 ; j+=1){
          
            if(m_isUse[i][j])
			{
              m_ppQuadPatchManager[i][j]->GetViewFrustumEq(buffer_viewFrustumEq);
			  m_ppQuadPatchManager[i][j]->CalculateViewFrustumCulling();
		      AddVisiblePatch(m_ppQuadPatchManager[i][j]);
			}

		}

	}




//          test  12_10
/*	
	classDLLNODE< CMAP_PATCH* > *pCurNode;
    float min[3],float max[3];
	CMAP_PATCH *pt;

	pCurNode = m_PatchLIST.GetHeadNode ();
	::ResetSceneAABB();
	while( pCurNode ) 
	{
		pt = pCurNode->DATA;
		min[0] = pt->m_aabb.x[0]; min[1] = pt->m_aabb.y[0]; min[2] = pt->m_aabb.z[0];
        max[0] = pt->m_aabb.x[1]; max[1] = pt->m_aabb.y[1]; max[2] = pt->m_aabb.z[1];
		::InputSceneAABB(min, max, 0);
         
		pCurNode = m_PatchLIST.GetNextNode( pCurNode );
	    
	}
	
	
	pCurNode = m_PatchLIST.GetHeadNode ();
	::ResetSceneAABB();
	while( pCurNode ) 
	{
		pt = pCurNode->DATA;
		min[0] = pt->m_aabb.x[0]; min[1] = pt->m_aabb.y[0]; min[2] = pt->m_aabb.z[0];
        max[0] = pt->m_aabb.x[1]; max[1] = pt->m_aabb.y[1]; max[2] = pt->m_aabb.z[1];
		::InputSceneAABB(min, max, 0);
         
		pCurNode = m_PatchLIST.GetNextNode( pCurNode );
	    
	}
	
*/

}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 참조된 각 *m_ppQuadPatchManager의 VisiblePatch를 CPatchManager에 합병합
//----------------------------------------------------------------------------------------------------
void CPatchManager::AddVisiblePatch(CQuadPatchManager* pQuadPatch)
{
     int i;

     for(i = 0; i < pQuadPatch->m_nPATCH ; i+=1)
	 {

        m_ppSubPATCH[m_nSubPATCH]=pQuadPatch->m_ppPATCH[i];
		m_nSubPATCH+=1;
 
     }
     
	 for(i = 0; i < pQuadPatch->m_nExPATCH ; i+=1)
	 {

        m_ppSubPATCH[m_nSubPATCH]=pQuadPatch->m_ppExPATCH[i];
		m_nSubPATCH+=1;
 
     }
      
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief VisiblePatch를 엔진에 등록
//----------------------------------------------------------------------------------------------------
void CPatchManager::Update_VisiblePatchManager()
{
	short i;	

	/// 실제로 엔진에서 사용될 패치들을 등록..
	

	CalculateViewFrustumCulling();

	for(i=0;i<m_nSubPATCH;i+=1){
    
		this->Insert_VisiblePatch( m_ppSubPATCH[i]);
  	    m_ppSubPATCH[i]->PlaySOUND();
	}

	
	for(i=0;i<m_nCameraPatch;i+=1){
    
		this->Insert_VisiblePatch( m_CameraPATCH[i]);
  	    m_CameraPATCH[i]->PlaySOUND();
	}

	
	SetPatchType();
	ExecutePatchTpye();   
		
	Delete_UnvisiblePatch ();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief VisiblePatch의 Drawing Type을 정함
//----------------------------------------------------------------------------------------------------
void CPatchManager::SetPatchType()
{


    int i,buffer_x,buffer_y,patch_center[2];	
    float position[3];
    HNODE camera_node;
	  
   	
    for(i=0;i<m_nCameraPatch;i+=1)
	{

       m_CameraPATCH[i]->m_DrawingType=0; 
		
	}
	
	camera_node=g_pCamera->GetZHANDLE();
    getCameraEye(camera_node,position);

	patch_center[0]=(int)(position[0]/1000.0f);
	patch_center[1]=(int)(position[1]/1000.0f);
				
	for(i=0;i<m_nSubPATCH;i+=1)
	{
    
		buffer_y=(int)(m_ppSubPATCH[i]->m_aabb.y[0]/1000.0f)-patch_center[1]+15;;
		buffer_x=(int)(m_ppSubPATCH[i]->m_aabb.x[0]/1000.0f)-patch_center[0]+15;;
			  
		if(lod_onoff)
		{
			if(0<=buffer_x && buffer_x<31 && 0<=buffer_y && buffer_y<31)
			m_ppSubPATCH[i]->m_DrawingType=patch_map[buffer_y][buffer_x];
			else
			m_ppSubPATCH[i]->m_DrawingType=10;
		}
		else
		{
		    m_ppSubPATCH[i]->m_DrawingType=0;
		}
	}


}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief VisiblePatch의 Drawing Type을 엔진에 알려줌
//----------------------------------------------------------------------------------------------------
void CPatchManager::ExecutePatchTpye()
{

    int i;

	
    for(i=0;i<m_nSubPATCH;i+=1)
	{
    
	   if(m_ppSubPATCH[i]->m_PreDrawingType!=m_ppSubPATCH[i]->m_DrawingType)
  	
		///::setTerrainBlockIndexOrder( m_ppSubPATCH[i]->GetVISIBLE(), m_ppSubPATCH[i]->m_DrawingType );
		
		 
	    m_ppSubPATCH[i]->m_PreDrawingType=m_ppSubPATCH[i]->m_DrawingType;
	} 


    for(i=0;i<m_nCameraPatch;i+=1)
	{

       	if( m_CameraPATCH[i]->m_DrawingType==0)
		{
		 ///::setTerrainBlockIndexOrder( m_CameraPATCH[i]->GetVISIBLE(), 0);
	     m_CameraPATCH[i]->m_PreDrawingType=0;   
	    }
	}
	

}



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief VisiblePatch중 카메라 주위의 Patch를 얻음  
//----------------------------------------------------------------------------------------------------
void CPatchManager::InsertCameraPatch(CMAP_PATCH* pPatch)
{
   
    m_CameraPATCH[m_nCameraPatch]=pPatch;
	m_nCameraPatch+=1; 
	                           

}


void CPatchManager::TestDrawAABB()
{

   int i,j,k;
   CMAP_PATCH *patch;
   float buffer_min[3],buffer_max[3];
   
   for(i=0;i<3;i+=1)
   {
	   for(j=0;j<3;j+=1)
	   {
		   
		   if(m_ppQuadPatchManager[i][j]!=NULL)
		   {
				for(k=0;k<m_ppQuadPatchManager[i][j]->m_nExPATCH;k+=1)
				{

					patch=m_ppQuadPatchManager[i][j]->m_ppExPATCH[k];
					buffer_min[0]=patch->m_AABBMin.x;buffer_min[1]=patch->m_AABBMin.y;buffer_min[2]=patch->m_AABBMin.z;
					buffer_max[0]=patch->m_AABBMax.x;buffer_max[1]=patch->m_AABBMax.y;buffer_max[2]=patch->m_AABBMax.z;

				//	 drawAABB(buffer_min,buffer_max,D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f));

				}
		   }
	   }
    }

}

void CPatchManager::AddPickPatch(CQuadPatchManager* pQuadPatch)
{
     int i;

     for(i = 0; i < pQuadPatch->m_nPickPATCH ; i+=1)
	 {

        m_PickPATCH[m_nPickPATCH]=pQuadPatch->m_ppPickPATCH[i];
		m_nPickPATCH+=1;
 
     }
     
}

void CPatchManager::CalculatePickingPATCH()
{

	int i,j;
    static float seta = 1.570796f,  phi = 0.0f;
	static bool PositiveOnOff = true;
	float Position[3], Direction[3];
	float Position2[3];
    
	D3DXVECTOR3 Pos;
    if(g_pAVATAR != NULL)
	{
	
		Pos = g_pAVATAR->Get_CurPOS();
		Position[0] = Pos.x; Position[1] = Pos.y; Position[2] = Pos.z + 100.0f;
		
		Direction[0] = sinf(seta)*cosf(phi); Direction[1] = sinf(seta)*sinf(phi); Direction[2] = cosf(seta);

		Position2[0] = Position[0] + 100000.0f * Direction[0];
		Position2[1] = Position[1] + 100000.0f * Direction[1];
		Position2[2] = Position[2] + 100000.0f * Direction[2];
	
		
		::ResetSceneLine();
		::InputSceneLine(Position, Position2);

		
		
		phi += 3.141592f/100.0f;
		if( phi > (3.141592f * 2.0f))
		phi = 0.0f;
        
		if(PositiveOnOff)
		{
			seta += 3.141592f/1000.0f;
		    if(seta > (3.141592f * 0.5f))
			{
				seta = 3.141592f * 0.5f;
                PositiveOnOff = false;
			}
		}
		else
		{   
			seta -= 3.141592f/1000.0f;
		    if(seta < (3.141592f * 0.35f))
			{
				seta = 3.141592f * 0.35f;
                PositiveOnOff = true;
			}

		}


		m_nPickPATCH = 0;
		
		for( i = 0; i < 3; i+=1){
			
			for( j = 0; j < 3 ; j+=1){
				
				if(m_isUse[i][j])
				{
					m_ppQuadPatchManager[i][j]->GetPickingRay(Position, Direction);
					m_ppQuadPatchManager[i][j]->CalculatePickingPATCH();
					AddPickPatch(m_ppQuadPatchManager[i][j]);
				}

			}

		}

	    
		float vMin[3], vMax[3];

		
		for(i = 0; i < m_nPickPATCH; i+=1)
		{
			vMin [0] = m_PickPATCH[i]->m_aabb.x[0];
			vMin [1] = m_PickPATCH[i]->m_aabb.y[0];
			vMin [2] = m_PickPATCH[i]->m_aabb.z[0];
			vMax [0] = m_PickPATCH[i]->m_aabb.x[1];
			vMax [1] = m_PickPATCH[i]->m_aabb.y[1];
			vMax [2] = m_PickPATCH[i]->m_aabb.z[1]; 
			
			::InputSceneAABB(vMin,vMax,0);

		}

	}
}

/// for mob collision test
void CPatchManager::DrawCollisionCylinder()
{
	classDLLNODE< CMAP_PATCH* > *pCurNode;

	pCurNode = m_PatchLIST.GetHeadNode ();
	while( pCurNode ) 
	{	
		{
			classDLLNODE< tagCYLINDERINFO > *pCurPatchNode = pCurNode->DATA->m_CylinderLIST.GetHeadNode();
			while( pCurPatchNode )
			{
				InputSceneCylinder( pCurPatchNode->DATA.m_Position.x,
									pCurPatchNode->DATA.m_Position.y,
									pCurPatchNode->DATA.m_Position.z,
									1000.0,
									pCurPatchNode->DATA.m_fRadius );

				pCurPatchNode = pCurNode->DATA->m_CylinderLIST.GetNextNode( pCurPatchNode );
			
			
				float vMin[3], vMax[3];
				CMAP_PATCH *pPatch;

				pPatch = pCurNode->DATA;

				vMin [0] = pPatch->m_aabb.x[0];
				vMin [1] = pPatch->m_aabb.y[0];
				vMin [2] = pPatch->m_aabb.z[0];
				vMax [0] = pPatch->m_aabb.x[1];
				vMax [1] = pPatch->m_aabb.y[1];
				vMax [2] = pPatch->m_aabb.z[1]; 
				
				::InputSceneAABB(vMin,vMax,0); 
			
			
			}
		}

		pCurNode = m_PatchLIST.GetNextNode( pCurNode );
	}
}
