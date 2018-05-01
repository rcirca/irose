/*
	$Header: /Client/CObjCHAR_Collision.cpp 105   05-08-30 5:29p Gioend $
*/
#include "stdAFX.h"
#include "Game.h"
#include "OBJECT.h"
#include "IO_Terrain.h"
#include "Network\CNetwork.h"
#include "common\Calculation.h"
#include "Game_FUNC.h"
#include "ObjectActionProcessor.h"
#include "CObjCART.h"
#include "ObjCastleGear.h"
#include "CObjCHAR_Collision.h"
#include "../Country.h"

// get the difference between first-frame COM position z and m_vCurrent-frame COM position z
#define GET_MOTION_DIFF_Z (m_vCOM.z - m_vCurrent.z - m_vInitCOM.z)

#define DISTANCE_INFINITE (999999999.f)

const float AVATAR_BODY_FROM_COM = 45.0f;
const float AVATAR_FOOT_FROM_COM = -70.0f;
const float AVATAR_BODY_FRONT_RADIUS = 30.0f;   //70.0f;     //test 12_10
const float AVATAR_FOOT_FRONT_RADIUS = 20.0f;
const float AVATAR_FOOT_RADIUS = 15.0f;     //test 12_10
const float AVATAR_FRONT_LENGTH = 40.0f;

const float CART_HEIGHT = 70.0f; // from com to ground
const float CART_WIDTH = 160.0f; // from left door to right door
const float CART_LENGTH = 180.0f; // from head to tail

const float CGEAR_HEIGHT = 2.0f * CART_HEIGHT; // from com to grond
const float CGEAR_WIDTH = 2.0f * CART_WIDTH; // from left door to right door
const float CGEAR_LENGTH = 2.0f * CART_LENGTH;

const float CART_BODY_FROM_COM = 50.0f;
const float CART_FOOT_FROM_COM = -CART_HEIGHT;
const float CART_BODY_FRONT_RADIUS = 30.0f;
const float CART_FOOT_FRONT_RADIUS = 30.0f;
const float CART_FOOT_RADIUS = 40.0f;
const float CART_FRONT_LENGTH = 60.0f;

const float CGEAR_BODY_FROM_COM = 2.0f * CART_BODY_FROM_COM;
const float CGEAR_FOOT_FROM_COM = -CGEAR_HEIGHT;
const float CGEAR_BODY_FRONT_RADIUS = 2.0f * CART_BODY_FRONT_RADIUS;
const float CGEAR_FOOT_FRONT_RADIUS = 2.0f * CART_FOOT_FRONT_RADIUS;
const float CGEAR_FOOT_RADIUS = 2.0f * CART_FOOT_RADIUS;
const float CGEAR_FRONT_LENGTH = 2.0f * CART_FRONT_LENGTH;

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::UpdateHeight ( CObjCHAR * pObj )
//--------------------------------------------------------------------------------
{
	m_pObj = pObj;
	assert(m_pObj);

	if (pObj->GetPetMode() >= 0) { // if is on the PET, then ignore UpdateHeight
		return;
	}
			
	m_hNodeModel = m_pObj->GetZMODEL();
	int iObjTYPE = m_pObj->Get_TYPE();

	m_footCollisionPass = false;
	
	
	switch( iObjTYPE )
	{
		case OBJ_MORPH:
		case OBJ_ITEM:
		case OBJ_GROUND:
		case OBJ_CNST:
		case OBJ_NPC:		// OBJ_NPC >= character object ...~
		case OBJ_MOB:
			AdjustHeight_Monster();
			break;
		case OBJ_AVATAR:
			AdjustHeight_Other();			
			break;
		case OBJ_USER:
			if( g_GameDATA.m_bFilmingMode )
				AdjustHeight_Monster();
			else
				AdjustHeight_Avatar();
			break;
		case OBJ_CART:
			{
				CObjCART* pCart = (CObjCART*)pObj;
				CObjCHAR* pParent = pCart->GetParent();
				if( pParent )
				{
					/// 내 아바타일경우
					if( pParent->IsA( OBJ_USER ) )
						AdjustHeight_AvatarCart();
					else
						AdjustHeight_OtherCart();	

					pParent->Set_CurPOS( m_pObj->Get_CurPOS() );
				}
			}
			break;
		case OBJ_CGEAR:
			{
				CObjCastleGear* pCgear = (CObjCastleGear*)pObj;
				CObjCHAR* pParent = pCgear->GetParent();
				if( pParent )
				{
					/// 내 아바타일경우
					if( pParent->IsA( OBJ_USER ) )
						AdjustHeight_AvatarCgear();
					else
						AdjustHeight_OtherCgear();	

					pParent->Set_CurPOS( m_pObj->Get_CurPOS() );
				}
			}
			break;
	}	
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::AdjustHeight_AvatarCart ( void )
//--------------------------------------------------------------------------------
{
	if (!InitializePC()) return;

	m_fTerrainHeight = g_pTerrain->GetHeight( m_vCurrent.x, m_vCurrent.y );

	SetSpheres_AvatarCart();
	CollectContact( CollisionCallback_Avatar );
	GetCollisionStateCart();

	D3DXVECTOR3 vTerrainNormal;

	switch ( m_State )
	{
	case CollisionState::CS_BODY_FRONT:
	case CollisionState::CS_FOOT_FRONT:
		CollisionResponseBody_Cart();
		setModelNormal( m_hNodeModel, D3DXVECTOR3(0, 0, 1) );
		break;
	case CollisionState::CS_FOOT:
		CollisionResponseFoot_Cart();
		setModelNormal( m_hNodeModel, D3DXVECTOR3(0, 0, 1) );		
		break;
	case CollisionState::CS_NONE:
		CollisionResponseNone_Cart();
		ApplyTerrainNormal( m_vNext.x, m_vNext.y );
		break;
	}


//--------------------------------------------------------------------------
#if defined(_GBC)
	//박지호::펫모드 일때 아바타를 정지 시킨다.
	if((m_State == CS_BODY_FRONT) || (m_State == CS_FOOT_FRONT))
	{	
		if(m_pObj->IsPET())
		{
			CObjCART* pCart =(CObjCART*)m_pObj;
			pCart->GetParent()->Set_COMMAND(CMD_STOP);	
		}
	}
#endif 
//--------------------------------------------------------------------------
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::AdjustHeight_OtherCart (void)
//--------------------------------------------------------------------------------
{
	if (!InitializePC()) return;

	m_fTerrainHeight = g_pTerrain->GetHeight( m_vCurrent.x, m_vCurrent.y );

	SetSpheres_OtherCart();
	CollectContact( CollisionCallback_Other );
	GetCollisionStateCart();

	D3DXVECTOR3 vTerrainNormal;

	switch ( m_State )
	{
	case CollisionState::CS_BODY_FRONT:
	case CollisionState::CS_FOOT_FRONT:
		CollisionResponseBody_Cart();
		setModelNormal( m_hNodeModel, D3DXVECTOR3(0, 0, 1) );
		break;
	case CollisionState::CS_FOOT:
		CollisionResponseFoot_Cart();
		setModelNormal( m_hNodeModel, D3DXVECTOR3(0, 0, 1) );		
		break;
	case CollisionState::CS_NONE:
		CollisionResponseNone_Cart();
		ApplyTerrainNormal( m_vNext.x, m_vNext.y );
		break;
	}
}



//--------------------------------------------------------------------------------
void CObjCHAR_Collision::AdjustHeight_AvatarCgear ( void )
//--------------------------------------------------------------------------------
{
	if (!InitializePC()) return;

	m_fTerrainHeight = g_pTerrain->GetHeight( m_vCurrent.x, m_vCurrent.y );

	SetSpheres_AvatarCgear();
	CollectContact( CollisionCallback_Avatar );
	GetCollisionState();

	D3DXVECTOR3 vTerrainNormal;

	switch ( m_State )
	{
	case CollisionState::CS_BODY_FRONT:
	case CollisionState::CS_FOOT_FRONT:
		CollisionResponseBody_Cgear();
		//setModelNormal( m_hNodeModel, D3DXVECTOR3(0, 0, 1) );
		break;
	case CollisionState::CS_FOOT:
		CollisionResponseFoot_Cgear();
		//setModelNormal( m_hNodeModel, D3DXVECTOR3(0, 0, 1) );		
		break;
	case CollisionState::CS_NONE:
		CollisionResponseNone_Cgear();
		//ApplyTerrainNormal( m_vNext.x, m_vNext.y );
		break;
	}
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::AdjustHeight_OtherCgear (void)
//--------------------------------------------------------------------------------
{
	if (!InitializePC()) return;

	m_fTerrainHeight = g_pTerrain->GetHeight( m_vCurrent.x, m_vCurrent.y );

	SetSpheres_OtherCgear();
	CollectContact( CollisionCallback_Other );
	GetCollisionState();

	D3DXVECTOR3 vTerrainNormal;

	switch ( m_State )
	{
	case CollisionState::CS_BODY_FRONT:
	case CollisionState::CS_FOOT_FRONT:
		CollisionResponseBody_Cgear();
		//setModelNormal( m_hNodeModel, D3DXVECTOR3(0, 0, 1) );
		break;
	case CollisionState::CS_FOOT:
		CollisionResponseFoot_Cgear();
		//setModelNormal( m_hNodeModel, D3DXVECTOR3(0, 0, 1) );		
		break;
	case CollisionState::CS_NONE:
		CollisionResponseNone_Cgear();
		//ApplyTerrainNormal( m_vNext.x, m_vNext.y );
		break;
	}
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::AdjustHeight_Avatar ()
//--------------------------------------------------------------------------------
{  
    if (!InitializePC( true )) return;
    
	m_fTerrainHeight = g_pTerrain->GetHeight( m_vCOM.x, m_vCOM.y );

	SetSpheres_Avatar();
	CollectContact( CollisionCallback_Avatar );
	GetCollisionStateAvatar();
	

	switch ( m_State )
	{
	case CollisionState::CS_BODY_FRONT:
	case CollisionState::CS_FOOT_FRONT:
		CollisionResponseBody_Avatar();
		break;
	case CollisionState::CS_FOOT:
		CollisionResponseFoot_Avatar();
		break;
	case CollisionState::CS_NONE:
		CollisionResponseNone_Avatar();
		break;
	}
  
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::AdjustHeight_Other ()
//--------------------------------------------------------------------------------
{
	if (!InitializePC()) return;

	m_fTerrainHeight = g_pTerrain->GetHeight( m_vCOM.x, m_vCOM.y );

	SetSpheres_Other ();
	CollectContact( CollisionCallback_Other );
	GetCollisionStateAvatar();
	
	switch ( m_State )
	{
	case CollisionState::CS_FOOT:
		CollisionResponseFoot_Other();
		break;
	case CollisionState::CS_NONE:
		CollisionResponseNone_Other();
		break;
	}
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::AdjustHeight_Monster (void)
//--------------------------------------------------------------------------------
{
		
	::getPosition( m_hNodeModel, m_vCurrent );
	::getModelCOMPositionWorld( m_hNodeModel, m_vCOM );
		
	int nZonePatchX, nZonePatchY;
	float fZonePatchSize;

	fZonePatchSize = (GRID_COUNT_PER_PATCH_AXIS * fGRID_SIZE );
	
    nZonePatchX = (short) ( m_vCurrent.x / fZonePatchSize );
	nZonePatchY = (short) ( m_vCurrent.y / fZonePatchSize );
	
	CMAP_PATCH *pPatch;
	pPatch = g_pTerrain->GetPATCH(nZonePatchX , nZonePatchY);
	
	if(pPatch)
	{


		if(CollisionResponseMonster(pPatch))
		{

			m_vCurrent.z = g_pTerrain->GetHeightTop( m_vCurrent.x, m_vCurrent.y ) + m_pObj->m_fAltitude;
			m_pObj->ResetCUR_POS( m_vCurrent);	
			CObjCHAR *pTarget = (CObjCHAR*)m_pObj->Get_TARGET(); 
    		if(pTarget != NULL)
			m_pObj->Restart_MOVE_AL( pTarget->Get_CurPOS() );
		}
		else
		{
			m_vCurrent.z = g_pTerrain->GetHeightTop( m_vCOM.x, m_vCOM.y ) + m_pObj->m_fAltitude;
			m_pObj->ResetCUR_POS( m_vCurrent );	
		}
	
	}
	else
	{
		m_vCurrent.z = g_pTerrain->GetHeightTop( m_vCOM.x, m_vCOM.y ) + m_pObj->m_fAltitude;
		m_pObj->ResetCUR_POS( m_vCurrent );	
	}
	
	
}


bool CObjCHAR_Collision::CollisionResponseMonster (CMAP_PATCH *pPatch)
{
		

	classDLLNODE< tagCYLINDERINFO > *pCurPatchNode = pPatch->m_CylinderLIST.GetHeadNode();
	while( pCurPatchNode )
	{
		if( ReviseMonsterPosition( &(pCurPatchNode->DATA)) )
        return true;
	
	
		pCurPatchNode = pPatch->m_CylinderLIST.GetNextNode( pCurPatchNode );
		
	}
	
	
	
	return false;


}

bool CObjCHAR_Collision::ReviseMonsterPosition(tagCYLINDERINFO *Cylinder)
{

	float distance;
	
	distance = sqrtf( (m_vCurrent.x - Cylinder->m_Position.x)*(m_vCurrent.x - Cylinder->m_Position.x) + (m_vCurrent.y - Cylinder->m_Position.y)*(m_vCurrent.y - Cylinder->m_Position.y) );  
	if(distance < Cylinder->m_fRadius)
	{
        float dir_[3];
		D3DXVECTOR2 v1,v2;
		D3DXVECTOR2 direction, p;
		float a,b,d,t;
		
		::getModelDirectionVector(m_hNodeModel, dir_);
				
		v1 = (D3DXVECTOR2)m_vCurrent - (D3DXVECTOR2)Cylinder->m_Position;
		v2.x = dir_[0]; v2.y = dir_[1]; 
		v2 /= D3DXVec2Length(&v2);
				direction = v1 - D3DXVec2Dot(&v1,&v2)*v2;
		direction/=D3DXVec2Length(&direction);
		p = (D3DXVECTOR2)m_vCurrent - (D3DXVECTOR2)Cylinder->m_Position;

		a= D3DXVec2Dot(&direction,&p);
		b= D3DXVec2Dot(&p,&p);
		d= Cylinder->m_fRadius * Cylinder->m_fRadius;
		t = -a + sqrtf(a*a+d-b); 

		if(t<0.0f)
			t=0.0f; 
		else
		{
		    m_vCurrent.x += (t+5)*direction.x;
			m_vCurrent.y += (t+5)*direction.y;
		}
	    
		return true;
       
	}

	return false;

}


//--------------------------------------------------------------------------------
bool CObjCHAR_Collision::InitializePC ( bool bIsAvatar )
//--------------------------------------------------------------------------------
{
	if( !bIsAvatar )
	{
		assert(m_pObj);

		float fResetPosZ;

		if ( m_pObj->GetResetPosZ( fResetPosZ ) ) {	// 높이값이 갱신되어야 할 타이밍인가?
			D3DXVECTOR3 vCurPos = m_pObj->Get_CurPOS();

			vCurPos.z = fResetPosZ; // 높이 값만 변경

			m_pObj->SetResetPosZ(false, 0); // 초기화시킴

			m_pObj->ResetCUR_POS( vCurPos ); // 현재 위치 갱신
	//		return false; // 충돌체크 후 높이 재조정 작업 필요 없음.   //test
		}
	}
	
	// 일반 충돌 모드 초기화
	Initialize(); 
	return true;
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::Initialize ( void )
//--------------------------------------------------------------------------------
{
	assert(m_pObj);

	m_pObj->SetCollisionGroundNode( NULL );
	m_State = CollisionState::CS_NONE;

	::getPosition( m_hNodeModel, m_vCurrent );
	
	m_vNext = m_vCurrent;
	::getPrevPosition( m_hNodeModel, m_vPrevious );
    ::getModelCOMPositionWorld( m_hNodeModel, m_vCOM );
	::getVisibleVelocity( m_hNodeModel, m_vVelocity );

	::getModelDirectionVector( m_hNodeModel, m_vDirection );
	::getModelInitialCOMPosition( m_hNodeModel, m_vInitCOM );
	m_fStature = ::getModelHeight( m_hNodeModel );
	m_fRadius = ::getVisibleRadius( m_hNodeModel, 0 /* bSelectSmaller */ );

	m_hCollisionBlockNode = ::OutPutModelCollisionBlock(m_hNodeModel);
	HNODE m_test;
	m_test = ::OutPutModelCollisionBlock(m_hNodeModel);
	
	if(m_hCollisionBlockNode != NULL)
	{
		D3DXVECTOR3 vPrevious;
        D3DXVECTOR3 vCurrent;
		D3DXVECTOR3 vel;

		::getPrevPosition( m_hCollisionBlockNode, vPrevious );
		::getPosition( m_hCollisionBlockNode, vCurrent );
	    vel = vCurrent - vPrevious;

		if(vel.z > 0.0f)
		{
			m_vCOM+=vel;
			m_vNext+=vel;
			m_vCurrent+=vel;
		
		}

	}

	m_Spheres.clear();
	m_ContactNodes.clear();
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::SetSpheres_Avatar ( void )
//--------------------------------------------------------------------------------
{
	m_vFront.x = m_vDirection.x * AVATAR_FRONT_LENGTH;
	m_vFront.y = m_vDirection.y * AVATAR_FRONT_LENGTH;
	m_vFront.z = m_vDirection.z * AVATAR_FRONT_LENGTH;

	m_vBodyFront.x = m_vCOM.x + m_vFront.x;
	m_vBodyFront.y = m_vCOM.y + m_vFront.y;
	m_vBodyFront.z = m_vCOM.z + AVATAR_BODY_FROM_COM + m_vFront.z;

	m_vFootFront.x = m_vCurrent.x+m_vDirection.x * 15.0f;
	m_vFootFront.y = m_vCurrent.y+m_vDirection.y * 15.0f;
	m_vFootFront.z = m_vCOM.z + 75.0f ;

	m_vFoot.x = m_vCurrent.x;
	m_vFoot.y = m_vCurrent.y;
	m_vFoot.z = m_vCurrent.z;

/*	::InputSceneSphere(m_vFoot.x ,m_vFoot.y, m_vFoot.z, AVATAR_FOOT_RADIUS);
	::InputSceneSphere(m_vBodyFront.x ,m_vBodyFront.y, m_vBodyFront.z, AVATAR_BODY_FRONT_RADIUS);
	::InputSceneSphere(m_vFootFront.x ,m_vFootFront.y, m_vFootFront.z, AVATAR_FOOT_FRONT_RADIUS); */

	m_Spheres.push_back( CollisionSphere( CollisionState::CS_FOOT, m_vFoot, AVATAR_FOOT_RADIUS ) );
	m_Spheres.push_back( CollisionSphere( CollisionState::CS_BODY_FRONT, m_vBodyFront, AVATAR_BODY_FRONT_RADIUS ) );
	m_Spheres.push_back( CollisionSphere( CollisionState::CS_FOOT_FRONT, m_vFootFront, AVATAR_FOOT_FRONT_RADIUS ) );
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::SetSpheres_Other ( void )
//--------------------------------------------------------------------------------
{
	m_vFoot.x = m_vCurrent.x;
	m_vFoot.y = m_vCurrent.y;
	m_vFoot.z = m_vCurrent.z;

	m_Spheres.push_back( CollisionSphere( CollisionState::CS_FOOT, m_vFoot, AVATAR_FOOT_RADIUS ) );
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::SetSpheres_AvatarCart ( void )
//--------------------------------------------------------------------------------
{
	m_vFront.x = m_vDirection.x * CART_FRONT_LENGTH;
	m_vFront.y = m_vDirection.y * CART_FRONT_LENGTH;
	m_vFront.z = m_vDirection.z * CART_FRONT_LENGTH;

	m_vBodyFront.x = m_vCOM.x + m_vFront.x;
	m_vBodyFront.y = m_vCOM.y + m_vFront.y;
	m_vBodyFront.z = m_vCOM.z + CART_BODY_FROM_COM + m_vFront.z;

	m_vFootFront.x = m_vCurrent.x + m_vDirection.x * 10.0f;
	m_vFootFront.y = m_vCurrent.y + m_vDirection.y * 10.0f;
	m_vFootFront.z = m_vCOM.z + 30.0f; 
	
	
	m_vFoot.x = m_vCurrent.x;
	m_vFoot.y = m_vCurrent.y;
	m_vFoot.z = m_vCurrent.z;
		
    
    m_Spheres.push_back( CollisionSphere( CollisionState::CS_FOOT, m_vFoot, CART_FOOT_RADIUS ) );
	m_Spheres.push_back( CollisionSphere( CollisionState::CS_BODY_FRONT, m_vBodyFront, CART_BODY_FRONT_RADIUS ) );
	m_Spheres.push_back( CollisionSphere( CollisionState::CS_FOOT_FRONT, m_vFootFront, CART_FOOT_FRONT_RADIUS ) );
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::SetSpheres_AvatarCgear ( void )
//--------------------------------------------------------------------------------
{
	m_vFront.x = m_vDirection.x * CGEAR_FRONT_LENGTH;
	m_vFront.y = m_vDirection.y * CGEAR_FRONT_LENGTH;
	m_vFront.z = m_vDirection.z * CGEAR_FRONT_LENGTH;

	m_vBodyFront.x = m_vCOM.x + m_vFront.x;
	m_vBodyFront.y = m_vCOM.y + m_vFront.y;
	m_vBodyFront.z = m_vCOM.z + CGEAR_BODY_FROM_COM + m_vFront.z;

	m_vFootFront.x = m_vCOM.x + m_vFront.x;
	m_vFootFront.y = m_vCOM.y + m_vFront.y;
	m_vFootFront.z = m_vCOM.z + CGEAR_FOOT_FROM_COM + m_vFront.z;

	m_vFoot.x = m_vCOM.x;	//m_vCurrent.x;   //test 12_10 
	m_vFoot.y = m_vCOM.y;	//m_vCurrent.y;
	m_vFoot.z = m_vCurrent.z;

	m_Spheres.push_back( CollisionSphere( CollisionState::CS_FOOT, m_vFoot, CGEAR_FOOT_RADIUS ) );
	m_Spheres.push_back( CollisionSphere( CollisionState::CS_BODY_FRONT, m_vBodyFront, CGEAR_BODY_FRONT_RADIUS ) );
	m_Spheres.push_back( CollisionSphere( CollisionState::CS_FOOT_FRONT, m_vFootFront, CGEAR_FOOT_FRONT_RADIUS ) );
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::SetSpheres_OtherCart ( void )
//--------------------------------------------------------------------------------
{
	m_vFoot.x = m_vCurrent.x;
	m_vFoot.y = m_vCurrent.y;
	m_vFoot.z = m_vCurrent.z;

	m_Spheres.push_back( CollisionSphere( CollisionState::CS_FOOT, m_vFoot, CART_FOOT_RADIUS ) );
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::SetSpheres_OtherCgear ( void )
//--------------------------------------------------------------------------------
{
	m_vFoot.x = m_vCurrent.x;
	m_vFoot.y = m_vCurrent.y;
	m_vFoot.z = m_vCurrent.z;

	m_Spheres.push_back( CollisionSphere( CollisionState::CS_FOOT, m_vFoot, CGEAR_FOOT_RADIUS ) );
}

//--------------------------------------------------------------------------------
bool CObjCHAR_Collision::CollisionCallback_Avatar ( HNODE hNodeIn )
//--------------------------------------------------------------------------------
{
	CObjectActionProcessor::GetSingleton().ProcessChain( hNodeIn );

	return true;
}

//--------------------------------------------------------------------------------
bool CObjCHAR_Collision::CollisionCallback_Other ( HNODE hNodeIn )
//--------------------------------------------------------------------------------
{
	int collisionLevel = ::getCollisionLevel( hNodeIn );
	if ( ZZ_IS_NOTMOVEABLE( collisionLevel ) ) {
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollectContact ( CollisionCallback callbackFunc )
// collects collidable objects into m_ContactNodes
//--------------------------------------------------------------------------------
{
	D3DXVECTOR3 vMin, vMax;
	D3DXVECTOR3 vTemp;
	
	const float MAX_GRAVITY_HEIGHT = 1000.0f;

	// get vMin/vMax
	
	
	vMin = (m_vPrevious + m_vCurrent)*0.5f;
	vMax = vMin;

	vMin.x = (m_vPrevious.x < m_vCurrent.x)?  (m_vPrevious.x - m_fRadius) : (m_vCurrent.x - m_fRadius);
    vMin.y = (m_vPrevious.y < m_vCurrent.y)?  (m_vPrevious.y - m_fRadius) : (m_vCurrent.y - m_fRadius);
    vMin.z = (m_vPrevious.z < m_vCurrent.z)?  (m_vPrevious.z - MAX_GRAVITY_HEIGHT) : (m_vCOM.z - MAX_GRAVITY_HEIGHT);
 
	vMax.x = (m_vPrevious.x > m_vCurrent.x)?  (m_vPrevious.x + m_fRadius) : (m_vCurrent.x + m_fRadius);
    vMax.y = (m_vPrevious.y > m_vCurrent.y)?  (m_vPrevious.y + m_fRadius) : (m_vCurrent.y + m_fRadius);
    vMax.z = (m_vPrevious.z > m_vCurrent.z)?  (m_vPrevious.z + m_fRadius) : (m_vCOM.z + m_fRadius);
  

	int iNumCollidableObjects = ::collectByMinMaxVec3( vMin, vMax );
	

	HNODE hNode;
	HNODE hUserData = 0;  
	

	m_ContactNodes.clear();
	for (int iC = 0; iC < iNumCollidableObjects; ++iC) 
	{
		hNode = ::getCollectNode ( iC );
		if ( ::isA( hNode, ZZ_TYPE_TERRAIN ) || ::isA( hNode, ZZ_TYPE_OCEAN ) || ::isDescendantOfType( hNode, ZZ_TYPE_MODEL ) ) {
			continue;
		}

		hUserData = ::getUserData( hNode );
		if ( hUserData ) 
		{
			CGameOBJ* pObj = reinterpret_cast< CGameOBJ* >( hUserData );
			if( pObj->IsA( OBJ_ITEM ) )
			{
				continue; // skip item
			}
			if ( !callbackFunc( hNode ) ) 
			{
				continue;
			}
		}
		m_ContactNodes.push_back( hNode );
//		::InputSceneOBBNode(hNode);
	}
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::GetCollisionState ( void )
//--------------------------------------------------------------------------------
{
	CollisionSphere * sp;
	HNODE hNode;
	const float MAX_DISTANCE_SQUARE = 2000.0f*2000.0f;
	D3DXVECTOR3 prevCenter, closestCenter;
	int collisionLevel;

	bool isFalling = (m_vVelocity.x == 0) && (m_vVelocity.y == 0); // not moving forward

	m_State = CollisionState::CS_NONE;

	for (std::vector<CollisionSphere>::iterator it = m_Spheres.begin(); it != m_Spheres.end(); ++it) {
		sp = &(*it);
		prevCenter = sp->center - m_vVelocity;

		for (std::vector<HNODE>::iterator itnode = m_ContactNodes.begin(); itnode != m_ContactNodes.end(); ++itnode) {
			hNode = (*itnode);

			if (sp->state == CollisionState::CS_BODY_FRONT) {
				if (isFalling) continue;
				if (getHeightCollisionLevelOnOff(hNode)) continue;
				if (::intersectNodeTriSphereMoving( hNode, sp->center, sp->radius, prevCenter, closestCenter, MAX_DISTANCE_SQUARE)) {
					sp->collided.push_back( hNode );
					m_State = CollisionState::CS_BODY_FRONT;
					//LogString(LOG_DEBUG, "CS_BODY_FRONT\n");
					return;
				}
			}
			else if (sp->state == CollisionState::CS_FOOT_FRONT) {
				if (isFalling) continue;
				collisionLevel = ::getCollisionLevel( hNode );
				/*
				if ( !ZZ_IS_MOVEABLE(collisionLevel) && ::intersectNodeTriSphere( hNode, sp->center, sp->m_fRadius )) { // collided not moveable object
					sp->collided.push_back( hNode );
					m_State = CollisionState::CS_FOOT_FRONT;
					//LogString(LOG_DEBUG, "CS_FOOT_FRONT\n");
					return;
				}
				*/
			}
			else if (sp->state == CollisionState::CS_FOOT) {
				sp->collided.push_back( hNode );
				if (isFalling && ::intersectNodeTriSphereMoving( hNode, sp->center, sp->radius, prevCenter, closestCenter, MAX_DISTANCE_SQUARE)) {
					m_State = CollisionState::CS_FOOT;
				}
				else if (::intersectNodeTriSphere( hNode, sp->center, sp->radius )) {
					m_State = CollisionState::CS_FOOT;
				}
				continue;
			}
			else {
				LogString( LOG_DEBUG, "error: Cannot reach here. CObjCHAR_Collision.cpp" );
				// cannot reach here
			}
		}
	}
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::GetCollisionStateAvatar ( void )
//--------------------------------------------------------------------------------
{
	CollisionSphere * sp;
	HNODE hNode;
	const float MAX_DISTANCE_SQUARE = 2000.0f*2000.0f;
	D3DXVECTOR3 prevCenter, closestCenter;
	float distance_ , distanceLimit, distanceLimitBody;
	D3DXVECTOR3 DIRECTION_DOWN(0, 0, -1) ,DIRECTION_MOVE;
	D3DXVECTOR3 rayOrigin(m_vFoot.x,m_vFoot.y, 1.8f*(75.0f)+m_vFoot.z) ;
    D3DXVECTOR3 rayOriginBody(m_vPrevious.x, m_vPrevious.y,m_vPrevious.z+1.5f*(75.0f));
    bool BodyRay = false;
	bool SphereResetOnOff = false;
    
	
	distanceLimit =1.8f*(75.0f);
	DIRECTION_MOVE = m_vFoot - m_vPrevious;
    distanceLimitBody = D3DXVec3Length(&DIRECTION_MOVE);  
    
	if(distanceLimitBody >0.001f)   
	{
		DIRECTION_MOVE /= distanceLimitBody;
	    BodyRay = true;
	}
	int collisionLevel;

	bool isFalling = (m_vVelocity.x == 0) && (m_vVelocity.y == 0); // not moving forward

	if(isFalling == true)
    isFalling = true;
	
	m_State = CollisionState::CS_NONE;

	for (std::vector<CollisionSphere>::iterator it = m_Spheres.begin(); it != m_Spheres.end(); ++it) {
		sp = &(*it);
		prevCenter = sp->center - m_vVelocity;

		if(sp->state == CollisionState::CS_FOOT_FRONT && SphereResetOnOff)
		sp->center.z = m_vFoot.z;
		
		for (std::vector<HNODE>::iterator itnode = m_ContactNodes.begin(); itnode != m_ContactNodes.end(); ++itnode) {
			hNode = (*itnode);

					
			if (sp->state == CollisionState::CS_BODY_FRONT) {
				if (isFalling) continue;
				if (::intersectNodeTriSphere( hNode, sp->center,sp->radius)) {
					
					if (getHeightCollisionLevelOnOff(hNode))
					{
						SphereResetOnOff = CollisionResponseBodyHeight(hNode);
						continue;
					} 
					if(m_hCollisionBlockNode != NULL)
					{
						CollisionResponseBody_Avatar();
                        break;
					}
					else
					{
						sp->collided.push_back( hNode );
						m_State = CollisionState::CS_BODY_FRONT;
						LogString(LOG_DEBUG, "CS_BODY_FRONT\n");
						return;
					}

				}   
			
			    if(BodyRay)
			    {
					distance_ = ::shootRayVec3( hNode, 1 /* nearest */, rayOriginBody, DIRECTION_MOVE );
					if(distance_ < distanceLimitBody)
					{
						
						if (getHeightCollisionLevelOnOff(hNode))
						{
							SphereResetOnOff = CollisionResponseBodyHeight(hNode);
							continue;
						} 
						if(m_hCollisionBlockNode != NULL)
						{
							CollisionResponseBody_Avatar();
							break;
						}
						else
						{
							sp->collided.push_back( hNode );
							m_State = CollisionState::CS_BODY_FRONT;
							LogString(LOG_DEBUG, "CS_BODY_FRONT\n");
							return;
						}

					}
			    }
			
			}
			else if (sp->state == CollisionState::CS_FOOT_FRONT) {
				
				if (::intersectNodeTriSphere( hNode, sp->center,sp->radius)) {
					
					if (getHeightCollisionLevelOnOff(hNode))
					{
						SphereResetOnOff = CollisionResponseBodyHeight(hNode);
						continue;
					} 
					if(m_hCollisionBlockNode != NULL)
					{
						CollisionResponseBody_Avatar();
						break;
					}
					else
					{
						sp->collided.push_back( hNode );
						m_State = CollisionState::CS_BODY_FRONT;
						LogString(LOG_DEBUG, "CS_BODY_FRONT\n");
						return;
					}
				}            
				
			}
			else if (sp->state == CollisionState::CS_FOOT) {
				sp->collided.push_back( hNode );
				if (isFalling && ::intersectNodeTriSphere( hNode, sp->center,sp->radius)) {
					m_State = CollisionState::CS_FOOT;
				    if (getHeightCollisionLevelOnOff(hNode))
						m_footCollisionPass = true;
				
				}
				else if (::intersectNodeTriSphere( hNode, sp->center, sp->radius )) {
					m_State = CollisionState::CS_FOOT;
				    if (getHeightCollisionLevelOnOff(hNode))
						m_footCollisionPass = true;
				}
				else{
                    distance_ = ::shootRayVec3( hNode, 1 /* nearest */, rayOrigin, DIRECTION_DOWN );
                    if(distance_ < distanceLimit)
					{
						m_State = CollisionState::CS_FOOT; 
					    if (getHeightCollisionLevelOnOff(hNode))
							m_footCollisionPass = true;
					}
                }
				continue;
			}
			else {
				LogString( LOG_DEBUG, "error: Cannot reach here. CObjCHAR_Collision.cpp" );
				// cannot reach here
			}
		}
	}
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::GetCollisionStateCart ( void )
//--------------------------------------------------------------------------------
{
	CollisionSphere * sp;
	HNODE hNode;
	const float MAX_DISTANCE_SQUARE = 2000.0f*2000.0f;
	D3DXVECTOR3 prevCenter, closestCenter;
	float distance_ , distanceLimit, distanceLimitBody;

	D3DXVECTOR3 DIRECTION_DOWN(0, 0, -1) ,DIRECTION_MOVE;
	D3DXVECTOR3 rayOrigin(m_vFoot.x,m_vFoot.y, 1.8f*(75.0f)+m_vFoot.z) ;
    D3DXVECTOR3 rayOriginBody(m_vPrevious.x, m_vPrevious.y,m_vBodyFront.z);
    bool BodyRay = false;
	bool SphereResetOnOff = false;

    distanceLimit =1.8f*(75.0f);
	DIRECTION_MOVE = m_vBodyFront - rayOriginBody;
    distanceLimitBody = D3DXVec3Length(&DIRECTION_MOVE);  
    
	if(distanceLimitBody >0.001f)   
	{
		DIRECTION_MOVE /= distanceLimitBody;
	    BodyRay = true;
	}
	int collisionLevel;

	bool isFalling = (m_vVelocity.x == 0) && (m_vVelocity.y == 0); // not moving forward

	if(isFalling == true)
    isFalling = true;
	
	m_State = CollisionState::CS_NONE;

	for (std::vector<CollisionSphere>::iterator it = m_Spheres.begin(); it != m_Spheres.end(); ++it) {
		sp = &(*it);
		prevCenter = sp->center - m_vVelocity;

		if(sp->state == CollisionState::CS_FOOT_FRONT && SphereResetOnOff)
		sp->center.z = m_vFoot.z;
		
		
		for (std::vector<HNODE>::iterator itnode = m_ContactNodes.begin(); itnode != m_ContactNodes.end(); ++itnode) {
			hNode = (*itnode);

					
			if (sp->state == CollisionState::CS_BODY_FRONT) {
								
				if (isFalling) continue;
				if (::intersectNodeTriSphere( hNode, sp->center,sp->radius)) {
					
					if (getHeightCollisionLevelOnOff(hNode)) 
				    {
						SphereResetOnOff = CollisionResponseBodyHeight(hNode);
						continue;
			        } 
					
					if(m_hCollisionBlockNode != NULL)
					{
						CollisionResponseBody_Cart();
                        break;
					}
					else
					{
						sp->collided.push_back( hNode );
						m_State = CollisionState::CS_BODY_FRONT;
						LogString(LOG_DEBUG, "CS_BODY_FRONT\n");
						return;
					}

				}   
			
			    if(BodyRay)
			    {
					distance_ = ::shootRayVec3( hNode, 1 /* nearest */, rayOriginBody, DIRECTION_MOVE );
					if(distance_ < distanceLimitBody)
					{
						if (getHeightCollisionLevelOnOff(hNode))
						{
							SphereResetOnOff = CollisionResponseBodyHeight(hNode);
							continue;
						} 
						
						if(m_hCollisionBlockNode != NULL)
						{
							CollisionResponseBody_Cart();
							break;
						}
						else
						{
							sp->collided.push_back( hNode );
							m_State = CollisionState::CS_BODY_FRONT;
							LogString(LOG_DEBUG, "CS_BODY_FRONT\n");
							return;
						}

					}
			    }
			
			}
			else if (sp->state == CollisionState::CS_FOOT_FRONT) {
				
				if (::intersectNodeTriSphere( hNode, sp->center, sp->radius)) {
				
					if (getHeightCollisionLevelOnOff(hNode))
					{
						SphereResetOnOff = CollisionResponseBodyHeight(hNode);
						continue;
					} 
						
					if(m_hCollisionBlockNode != NULL)
					{
						CollisionResponseBody_Cart();
						break;
					}
					else
					{
						sp->collided.push_back( hNode );
						m_State = CollisionState::CS_BODY_FRONT;
						LogString(LOG_DEBUG, "CS_BODY_FRONT\n");
						return;
					}
					
				}       
				
			}
			else if (sp->state == CollisionState::CS_FOOT) {
				sp->collided.push_back( hNode );
				if (isFalling && ::intersectNodeTriSphere( hNode, sp->center, sp->radius)) {
					m_State = CollisionState::CS_FOOT;
				    if (getHeightCollisionLevelOnOff(hNode))
							m_footCollisionPass = true;
				
				}
				else if (::intersectNodeTriSphere( hNode, sp->center, sp->radius )) {
					m_State = CollisionState::CS_FOOT;
				    if (getHeightCollisionLevelOnOff(hNode))
							m_footCollisionPass = true;
				}
				else{
                    distance_ = ::shootRayVec3( hNode, 1 /* nearest */, rayOrigin, DIRECTION_DOWN );
                    if(distance_ < distanceLimit)
					{
						m_State = CollisionState::CS_FOOT; 
					    if (getHeightCollisionLevelOnOff(hNode))
							m_footCollisionPass = true;
					}				
                }
				continue;
			}
			else {
				LogString( LOG_DEBUG, "error: Cannot reach here. CObjCHAR_Collision.cpp" );
				// cannot reach here
			}
		}
	}
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseBody_Cart ( void )
//--------------------------------------------------------------------------------
{
	float vRotation[4];
	
	::getPrevRotation(m_hNodeModel , vRotation);
    StopMovingForCollision();	
	::setRotationQuat(m_hNodeModel, vRotation); 
    ::stopModelRotation(m_hNodeModel);
}


//--------------------------------------------------------------------------------
bool CObjCHAR_Collision::CollisionResponseBodyHeight (HNODE hNode)
//--------------------------------------------------------------------------------
{
	D3DXVECTOR3 DIRECTION_UP(0, 0, 1);
	float distance;
	distance = ::shootRayVec3(hNode,0,m_vCurrent,DIRECTION_UP);
 
	if( distance == DISTANCE_INFINITE)
		return false;

    m_vCurrent.z += distance;
    m_vCOM.z += distance;
    m_vFoot.z = m_vCurrent.z;

	return true;
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseBody_Cgear ( void )
//--------------------------------------------------------------------------------
{
	StopMovingForCollision();	
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseBody_Avatar ( void )
//--------------------------------------------------------------------------------
{
	float vRotation[4];

	::getPrevRotation(m_hNodeModel , vRotation);
    StopMovingForCollision();	
	::setRotationQuat(m_hNodeModel, vRotation); 
	::stopModelRotation(m_hNodeModel);
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseBody_Other ( void )
//--------------------------------------------------------------------------------
{
	m_pObj->ResetCUR_POS( m_vPrevious );	
}

//--------------------------------------------------------------------------------
bool CObjCHAR_Collision::ApplySliding ( void )
//--------------------------------------------------------------------------------
{
	if ( (m_vVelocity.x == 0) && (m_vVelocity.y == 0) ) {
		return true; // if not moved, do nothing
	}
	
	if ( m_vFoot.z > m_fTerrainHeight + AVATAR_FOOT_RADIUS ) {
		return true; // if not on the terrain, do nothing
	}

	// apply sliding

	D3DXVECTOR3 vTerrainNormal;
	float Direction2DLength;
	D3DXVECTOR3 MovingDirection;
	D3DXVECTOR3 TangentialDirection;
	bool SlideOnOff; 
	float angle;

	const float fMinDampHeight = 1.0f; // 최소 1 cm 이하 아래로 내려갈 시에는 움직이는 거리 보정 안 함. 

	if ( g_pTerrain->GetNormal( m_vCOM.x, m_vCOM.y, vTerrainNormal ) ) {
		
		Direction2DLength = sqrtf( vTerrainNormal.x*vTerrainNormal.x + vTerrainNormal.y*vTerrainNormal.y);
		MovingDirection = m_vNext - m_vPrevious;
		
		if(vTerrainNormal.z>0.0f){

			TangentialDirection = -vTerrainNormal;
			TangentialDirection.z =  Direction2DLength*Direction2DLength/vTerrainNormal.z;
			
			if(Direction2DLength<0.001f)
			{
				angle = 0.0f;
				SlideOnOff = true;
			}
			else
			{
				angle = atanf(TangentialDirection.z/Direction2DLength);
				SlideOnOff = (D3DXVec3Dot(&TangentialDirection,&MovingDirection) <0.0f);
			}
		}
		else
		{  
			TangentialDirection = vTerrainNormal;
			TangentialDirection.z =  -Direction2DLength*Direction2DLength/vTerrainNormal.z;
			if(Direction2DLength<0.001f)
			{
				angle = 0.0f;
				SlideOnOff = true;
			}
			else
			{
				angle = atanf(TangentialDirection.z/Direction2DLength);
				SlideOnOff = (D3DXVec3Dot(&TangentialDirection,&MovingDirection) <0.0f);
			}
		}
	
		if(!((angle<3.141592f*0.3f) || SlideOnOff) )
		{
			return false;
		}
	
	
/*		if (0) { // 경사도에 따른 속도 변화 코드 무시. 서버와 동기가 안 맞아서 여러 문제 생김.
		  fSliding = 1.f + fSlopeDot; // 평지에서는 1. 경사가 위로 급할수록 0, 아래로 급할수록 2가 된다.

			// slope check, not to do too-fast climbing.
			if ( m_vNext.z > m_vPrevious.z - fMinDampHeight ) {
				D3DXVECTOR3 vDirection = m_vNext - m_vPrevious;
				float fMoveLength = sqrtf( vDirection.x * vDirection.x + vDirection.y * vDirection.y );

				if ( fMoveLength > 0 ) {
					D3DXVECTOR3 vDirectionNormalized;
					D3DXVec3Normalize( &vDirectionNormalized, &vDirection );
					m_vNext = m_vPrevious + fSliding * fMoveLength * vDirectionNormalized; 
				}
			}
		}   */
	}
	return true;
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseFoot_Avatar ( void )
//--------------------------------------------------------------------------------
{
	m_vNext.z = m_fTerrainHeight;  //test  12_20
	UpdateFootHeight_Avatar();
	if(m_ResetFootHeight)
	{
		StopMovingForCollision();
	    return;
	}

	if(fabsf(m_vNext.z-m_fTerrainHeight)<0.001f)
    m_vNext.z = m_vCurrent.z;

	if (0) {
		// not to over move-up in jumping
		const float MIN_DIFF_MOTION = 50.0f;
		float diff = GET_MOTION_DIFF_Z;
		if ( diff > MIN_DIFF_MOTION ) {
			m_vNext.z -= diff;
		}
	}
	UpdatePositionAfterCollision();

}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseFoot_Cart ( void )
//--------------------------------------------------------------------------------
{
    m_vNext.z = m_fTerrainHeight;
	UpdateFootHeight_Avatar();
	
	if(m_ResetFootHeight)
	{
		StopMovingForCollision();
	    return;
	}
	
	if(fabsf(m_vNext.z-m_fTerrainHeight)<0.001f)
    m_vNext.z = m_vCurrent.z;
	
	UpdatePositionAfterCollision();
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseFoot_Cgear ( void )
//--------------------------------------------------------------------------------
{
	m_vNext.z = m_fTerrainHeight;
	UpdateFootHeight_Avatar();
	UpdatePositionAfterCollision();
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseFoot_Other ( void )
//--------------------------------------------------------------------------------
{
	m_vNext.z = m_fTerrainHeight;
	UpdateFootHeight_Avatar();
	UpdatePositionAfterCollision();
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseNone_Other ( void )
//--------------------------------------------------------------------------------
{
	m_vNext.z = m_fTerrainHeight;
	m_pObj->SetCollisionGroundNode( NULL );
	UpdateFootHeight_Avatar();
	UpdatePositionAfterNonCollision();
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseNone_Cart ( void )
//--------------------------------------------------------------------------------
{
	m_vNext.z = m_fTerrainHeight;
	m_pObj->SetCollisionGroundNode( NULL );
	UpdateFootHeight_Avatar();
	if (!ApplySliding()) {
		StopMovingForCollision();
		return;
	}
	UpdatePositionAfterNonCollision();
}


//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseNone_Cgear ( void )
//--------------------------------------------------------------------------------
{
	m_vNext.z = m_fTerrainHeight;
	m_pObj->SetCollisionGroundNode( NULL );
	UpdateFootHeight_Avatar();
	if (!ApplySliding()) {
		StopMovingForCollision();
		return;
	}
	UpdatePositionAfterNonCollision();
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::CollisionResponseNone_Avatar ( void )
//--------------------------------------------------------------------------------
{
	m_vNext.z = m_fTerrainHeight;
	m_pObj->SetCollisionGroundNode( NULL );
	UpdateFootHeight_Avatar();
	if (!ApplySliding()) {
		StopMovingForCollision();
		return;
	}
	UpdatePositionAfterNonCollision();
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::UpdateFootHeight_Avatar ( void )
//--------------------------------------------------------------------------------
{
	CollisionSphere * sp;
	float distance;
	const float TOO_BIG_DISTANCE = 1000000.0f;
	D3DXVECTOR3 DIRECTION_DOWN(0, 0, -1), DIRECTION_UP(0, 0, 1);
	float currentHeight;
	float angle ,Direction2DLength;
    D3DXVECTOR3 ContactNormal,ContactPoint;
    float ContactMeshHeight;
	D3DXVECTOR3 MainNormal;
	D3DXVECTOR3 MovingDirection, TangentialDirection; 
	D3DXVECTOR3 MeshPoint[3];
	bool SlideOnOff; 
    bool GradientOnOff;

	D3DXVECTOR3 rayOriginUp = m_vFoot;               
	D3DXVECTOR3 rayOriginDown(m_vFoot.x,m_vFoot.y,1.8f*(75.0f) + m_vFoot.z);
    
  	GradientOnOff = false;
	m_ResetFootHeight = false;

	for (std::vector<CollisionSphere>::iterator it = m_Spheres.begin(); it != m_Spheres.end(); ++it) {
		sp = &(*it);
		if (sp->state != CollisionState::CS_FOOT) continue;

		for (std::vector<HNODE>::iterator itnode = sp->collided.begin(); itnode != sp->collided.end(); ++itnode) {
			// down 
            distance = ::shootRayVec3Ex( (*itnode), 1 /* nearest */, rayOriginDown, DIRECTION_DOWN, ContactPoint, ContactNormal, MeshPoint[0], MeshPoint[1], MeshPoint[2] );
			if ( distance < TOO_BIG_DISTANCE ) {
				currentHeight = rayOriginDown.z - distance;
				if ( currentHeight > m_vNext.z ) {
					 if(::IsAnimatable((*itnode)))
					 {
						m_vNext.z = currentHeight;
						m_pObj->SetCollisionGroundNode( (*itnode) );
                        ::InputModelCollisionBlock( m_hNodeModel, (*itnode));
						::savePrevPosition((*itnode));
                        MainNormal = ContactNormal;
					 }
					 else
					 {
                       m_vNext.z = currentHeight;
					   m_pObj->SetCollisionGroundNode( (*itnode) );
					   MainNormal = ContactNormal;
					   ContactMeshHeight = MeshPoint[0].z;
					   GradientOnOff = true;

					   for(int i = 1; i<3; i+=1)
					   {
						   if(ContactMeshHeight < MeshPoint[i].z)
						   ContactMeshHeight = MeshPoint[i].z;
					   }
					 }
				}
			}    
		
			// up
//			distance = ::shootRayVec3( (*itnode), 0 /* furthest */, rayOriginUp, DIRECTION_UP );
/*			if ( distance < m_fStature ) {
				currentHeight = rayOriginUp.z + distance;
				if ( currentHeight > m_vNext.z ) {
					m_vNext.z = currentHeight;
					m_pObj->SetCollisionGroundNode( *itnode );
				}
			}     */            
		}         
	}


		
    
	if(GradientOnOff)
	{
		
		MainNormal/=D3DXVec3Length(&MainNormal);
		Direction2DLength = sqrtf( MainNormal.x*MainNormal.x + MainNormal.y*MainNormal.y);
		MovingDirection = m_vNext - m_vPrevious;
				
		if(MainNormal.z>0.0f)
		{
			TangentialDirection = -MainNormal;
			TangentialDirection.z =  Direction2DLength*Direction2DLength/MainNormal.z;
			if(Direction2DLength<0.001f)
			{
				angle = 0.0f;
				SlideOnOff = true;
			}
			else
			{
				angle = atanf(TangentialDirection.z/Direction2DLength);
				SlideOnOff = (D3DXVec3Dot(&TangentialDirection,&MovingDirection) <0.0f);
			}
		}
		else
		{  
			TangentialDirection = MainNormal;
			TangentialDirection.z =  -Direction2DLength*Direction2DLength/MainNormal.z;
			if(Direction2DLength<0.001f)
			{
				angle = 0.0f;
				SlideOnOff = true;
			}
			else
			{
				angle = atanf(TangentialDirection.z/Direction2DLength);
				SlideOnOff = (D3DXVec3Dot(&TangentialDirection,&MovingDirection) <0.0f);
			}
		}
						
		
		if(!((angle<3.141592f*0.3f) || SlideOnOff) )
		{	
			
		    if( (ContactMeshHeight > ((m_vCOM.z-m_vFoot.z)*0.7f+m_vFoot.z)) && !m_footCollisionPass ) 
			m_ResetFootHeight = true;
		}
	

	}
	
	

}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::UpdateFootHeight_Other ( void )
//--------------------------------------------------------------------------------
// get the top height of contacted objects
{
	CollisionSphere * sp;
	float distance;
	const float TOO_BIG_DISTANCE = 1000000.0f;
	D3DXVECTOR3 DIRECTION_DOWN(0, 0, -1), DIRECTION_UP(0, 0, 1);
	float currentHeight;
	
	D3DXVECTOR3 rayOriginUp = m_vFoot;
	D3DXVECTOR3 rayOriginDown(m_vFoot.x, m_vFoot.y, m_vCOM.z);

	for (std::vector<CollisionSphere>::iterator it = m_Spheres.begin(); it != m_Spheres.end(); ++it) {
		sp = &(*it);
		if (sp->state != CollisionState::CS_FOOT) continue;

		for (std::vector<HNODE>::iterator itnode = sp->collided.begin(); itnode != sp->collided.end(); ++itnode) {
			// down 
            distance = ::shootRayVec3( (*itnode), 1 /* nearest */, rayOriginDown, DIRECTION_DOWN );
			if ( distance < TOO_BIG_DISTANCE ) {
				currentHeight = rayOriginDown.z - distance;
				if ( currentHeight > m_vNext.z ) {
					m_vNext.z = currentHeight;
					m_pObj->SetCollisionGroundNode( (*itnode) );
				}
			}
			
			// up
			distance = ::shootRayVec3( (*itnode), 0 /* furthest */, rayOriginUp, DIRECTION_UP );
			if ( distance < TOO_BIG_DISTANCE ) {
				currentHeight = rayOriginUp.z + distance;
				if ( currentHeight > m_vNext.z ) {
					m_vNext.z = currentHeight;
					m_pObj->SetCollisionGroundNode( *itnode );
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::UpdatePositionAfterCollision ( void )
//--------------------------------------------------------------------------------
{
//	ApplyGravity();

	if (m_vNext.z < m_fTerrainHeight) {
		m_vNext.z = m_fTerrainHeight;
	}
	m_pObj->ResetCUR_POS( m_vNext );	
	
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::UpdatePositionAfterNonCollision ( void )
//--------------------------------------------------------------------------------
{
	ApplyGravity();

	if (m_vNext.z < m_fTerrainHeight) {
		m_vNext.z = m_fTerrainHeight;
	}
	m_pObj->ResetCUR_POS( m_vNext );	
}	

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::StopMovingForCollision ( void )
//--------------------------------------------------------------------------------
{
	if (m_pObj->m_fCurMoveSpeed == 0) return;

	m_vNext = m_vPrevious;

	m_pObj->Set_CurPOS( m_vNext );
	g_pNet->Send_cli_CANTMOVE();
	m_pObj->SetCMD_STOP();
	
	setPositionVec3( m_hNodeModel, m_vNext );
}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::ApplyGravity ( void )
//--------------------------------------------------------------------------------
{
	// apply gravity
	if (m_vNext.z < m_vCurrent.z) {
		float fGravityAppliedZ = m_vCurrent.z + ::getGravityVelocityZ( m_hNodeModel );
		if (m_vNext.z < fGravityAppliedZ) {
			m_vNext.z = m_vCurrent.z;
			::InputModelGravity(m_hNodeModel);
		}
	}      //test 12_15

}

//--------------------------------------------------------------------------------
void CObjCHAR_Collision::ApplyTerrainNormal ( float fTerrainX, float fTerrainY )
//--------------------------------------------------------------------------------
{
	D3DXVECTOR3 vTerrainNormal;
	if (g_pTerrain->GetNormal( fTerrainX, fTerrainY, vTerrainNormal )) {
		setModelNormal( m_hNodeModel, vTerrainNormal );
	}
}
