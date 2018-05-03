/*
	$Header: /Client/CGameOBJ.cpp 7     05-06-08 4:32p Gioend $
*/
#include "stdAFX.h"
#include "OBJECT.h"
#include "CGameOBJ.h"
#include "System/CGame.h"
//-------------------------------------------------------------------------------------------------

float		CGameOBJ::m_fPickDistance;
D3DXVECTOR3 CGameOBJ::m_PickPosition;
bool		CGameOBJ::m_bStepOnObject = false;

//-------------------------------------------------------------------------------------------------
CGameOBJ::CGameOBJ ()
{
	m_bIsVisible = false;
	m_nIndex = 0;

	m_iSpecialUserData = 0;
}
CGameOBJ::~CGameOBJ ()
{
}

//-------------------------------------------------------------------------------------------------
bool CGameOBJ::IsIntersect (HNODE hNODE, float &fCurDistance)
{

	D3DXVECTOR3 RayOrig;
	D3DXVECTOR3 RayDir;
	CGame::GetInstance().GetRayOrig( RayOrig );
	CGame::GetInstance().GetRayDir( RayDir );

	if ( ::intersectRay( hNODE,
			RayOrig.x,	RayOrig.y,	RayOrig.z,
			RayDir.x,	RayDir.y,	RayDir.z,
			&m_PickPosition.x,		&m_PickPosition.y,		&m_PickPosition.z,		&m_fPickDistance) ) {
		if ( m_fPickDistance < fCurDistance ) {
			fCurDistance = m_fPickDistance;
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
void CGameOBJ::AdjustPickPOSITION (float fMaxDistanceFromAvatar)
{
	if ( !g_pAVATAR ) return;

	D3DXVECTOR3 vAvatarPos = g_pAVATAR->Get_CurPOS();
	D3DXVECTOR3 vAdjustedPickPos;
	D3DXVECTOR3 vDirection = m_PickPosition - vAvatarPos;

	float fDistance = D3DXVec3Length(&vDirection);

	if (fDistance < fMaxDistanceFromAvatar) return;
	D3DXVECTOR3 vDirectionNormalized;

	D3DXVec3Normalize( &vDirectionNormalized, &vDirection );

	m_PickPosition = vAvatarPos + fMaxDistanceFromAvatar * vDirectionNormalized;
}

