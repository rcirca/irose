/*
	$Header: /Client/CObjCHAR_Collision.h 16    05-03-25 11:17a Choo0219 $
*/
#ifndef	__COBJCOLLISION_H
#define	__COBJCOLLISION_H

#include <vector>
#include "IO_Terrain.h"

class CObjCHAR_Collision
{
public:
	// collision type: currently not used
	enum CollisionType {
		CT_AVATAR,
		CT_OTHER,
		CT_MONSTER,
		CT_AVATAR_CART,
		CT_AVATAR_CGEAR, // castle_gear for main avatar
		CT_OTHER_CART,
		CT_OTHER_CGEAR, // castle_gear for other character
	};

public:
	enum CollisionState {
		CS_NONE = 0, // not collided except the terrain
		CS_BODY_FRONT = 1, // body front collided
		CS_FOOT_FRONT = 1 << 1, // foot front collided
		CS_FOOT = 1 << 2, // foot collided
	};
	
	struct CollisionSphere {
		D3DXVECTOR3 center;
		float radius;
		CollisionState state;
		std::vector<HNODE> collided;

		CollisionSphere( CollisionState state_in, D3DXVECTOR3 center_in, float radius_in )
			: state(state_in), center(center_in), radius(radius_in)
		{}
	};

	//--------------------------------------------------------------------------------
	// member variables
	CollisionState m_State;

	std::vector<CollisionSphere> m_Spheres; // all collided collision spheres
	std::vector<HNODE> m_ContactNodes; // adjacent nodes

	HNODE m_hNodeModel; // model engine node
	HNODE m_hCollisionBlockNode; // test  12_14
	CObjCHAR * m_pObj; // real object pointer
	D3DXVECTOR3 m_vPrevious; // previous model origin
	D3DXVECTOR3 m_vCurrent; // current model origin
	D3DXVECTOR3 m_vNext; // next origin position = m_CurPOS
	D3DXVECTOR3 m_vCOM; // model root bone position
	D3DXVECTOR3 m_vInitCOM; // initial model com position in model space
	D3DXVECTOR3 m_vDirection; // model front direction (normalized)
	D3DXVECTOR3 m_vVelocity; // model moving velocity
	D3DXVECTOR3 m_vFront; // model front vector (direction x front_length)

	D3DXVECTOR3 m_vBodyFront; // front body collision censor to check if we meet a wall
	D3DXVECTOR3 m_vFootFront; // front foot collision censor to check if we are to be on the not-moveable object
	D3DXVECTOR3 m_vFoot; // foot collision censor to adjust height
    
	HNODE m_hNodeCollision;
	
	float m_fStature; // model height
	float m_fRadius; // model radius
	float m_fTerrainHeight; // terrain height for current com position
	bool m_ResetFootHeight;
    bool m_footCollisionPass;
	//--------------------------------------------------------------------------------

	void Initialize ( void );
	bool InitializePC (  bool bIsAvatar = false ); // 높이를 보정할 필요가 없으면 false 를 리턴함.

	static bool CollisionCallback_Avatar ( HNODE hNodeIn );
	static bool CollisionCallback_Other ( HNODE hNodeIn );
	static bool CollisionCallback_Monster ( HNODE hNodeIn );

	typedef bool (*CollisionCallback)( HNODE );

	void CollectContact ( CollisionCallback callbackFunc );

	void AdjustHeight_AvatarCart ( void );
	void AdjustHeight_OtherCart ( void );
	void AdjustHeight_AvatarCgear ( void );
	void AdjustHeight_OtherCgear ( void );
	void AdjustHeight_Monster ( void );
	void AdjustHeight_Other ( void );
	void AdjustHeight_Avatar ( void );
   	
	void SetSpheres_AvatarCart ( void );
	void SetSpheres_OtherCart ( void );
	void SetSpheres_AvatarCgear ( void );
	void SetSpheres_OtherCgear ( void );
	void SetSpheres_Avatar ( void );
	void SetSpheres_Other ( void );

	void GetCollisionState ( void );
	void GetCollisionStateAvatar(void);
	void GetCollisionStateCart ( void );
	
	void UpdateFootHeight_Avatar ( void );
	void UpdateFootHeight_Other ( void );

	void UpdatePositionAfterCollision ( void );
	void UpdatePositionAfterNonCollision ( void );

	void CollisionResponseBody_Avatar ( void );
	void CollisionResponseFoot_Avatar ( void );
	void CollisionResponseNone_Avatar ( void );
	
	void CollisionResponseBody_Other ( void );
	void CollisionResponseFoot_Other ( void );
	void CollisionResponseNone_Other ( void );

	void CollisionResponseBody_Cart ( void );
	void CollisionResponseNone_Cart ( void );
	void CollisionResponseFoot_Cart ( void );

	void CollisionResponseBody_Cgear ( void );
	void CollisionResponseNone_Cgear ( void );
	void CollisionResponseFoot_Cgear ( void );
    
	bool CollisionResponseBodyHeight (HNODE hNode);
	
	bool CollisionResponseMonster (CMAP_PATCH *pPatch);
	bool ReviseMonsterPosition(tagCYLINDERINFO *Cylinder);
	
	void ApplyTerrainNormal ( float fTerrainX, float fTerrainY );
	bool ApplySliding ( void ); // return true if sliding was succeeded.
	void ApplyGravity ( void );
	void StopMovingForCollision ( void );

public:
	//--------------------------------------------------------------------------------
	// public methods
	void UpdateHeight ( CObjCHAR * pObjIn );
	//--------------------------------------------------------------------------------
};

#endif // __COBJCOLLISION_H