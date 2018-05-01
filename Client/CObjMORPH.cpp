
#include "stdAFX.h"
#include "Game.h"
#include "CObjMORPH.h"
#include "IO_Morpher.h"
#include "IO_Basic.h"


//-------------------------------------------------------------------------------------------------
CObjMORPH::CObjMORPH ()
{
	m_hMorpher = NULL;
}
CObjMORPH::~CObjMORPH ()
{
	if ( m_hMorpher )
		::unloadMorpher( m_hMorpher );
}

//-------------------------------------------------------------------------------------------------
bool	CObjMORPH::Create (int iMorphID, D3DVECTOR &Position)
{
	tagMORPHER *pMorpher = g_DATA.m_MORPHER.Get( iMorphID );
	if ( !pMorpher )
		return false;

	m_PosCUR = Position;

	HNODE hMesh		= g_MeshFILE.KEY_HNODE ( pMorpher->m_HashMESH );
	HNODE hMotion	= g_MotionFILE.KEY_GetZMOTION ( pMorpher->m_HashANI );
	HNODE hMaterial	= g_MatFILE.KEY_HNODE ( pMorpher->m_HashMAT );

	m_hMorpher = ::loadMorpher(NULL, hMesh, hMotion, hMaterial, g_GameDATA.m_hLight);
	if ( !m_hMorpher ) {
		return false;
	}

	::setPosition( m_hMorpher, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z );

	
	::setMaterialBlendTypeCustom( hMaterial, pMorpher->m_nSrcBlend,
											pMorpher->m_nDestBlend,
											pMorpher->m_nBlendOP );

	::setMaterialUseAlphaTest( hMaterial, pMorpher->m_nAlphaTest );
	::setMaterialZWrite( hMaterial, pMorpher->m_nZWrite );
	::setMaterialZTest( hMaterial, pMorpher->m_nZTest );	
	
	return true;
}

//-------------------------------------------------------------------------------------------------
void	CObjMORPH::Scale (D3DVECTOR &Scale)
{
	if ( m_hMorpher )
		::setScale( m_hMorpher , Scale.x, Scale.y, Scale.z );
}

void	CObjMORPH::Rotate (D3DXQUATERNION &Rotate)
{
	if ( m_hMorpher )
		::setQuaternion( m_hMorpher, Rotate.w, Rotate.x, Rotate.y, Rotate.z );
}

//-------------------------------------------------------------------------------------------------
bool	CObjMORPH::IsIntersect (float &fCurDistance)
{
	return false;
}

void	CObjMORPH::InsertToScene (void)
{
	if ( m_bIsVisible || !m_hMorpher )
		return;

	m_bIsVisible = true;
	::insertToScene( m_hMorpher );		// CObjGROUND::RegisterToEZIN
}

void	CObjMORPH::RemoveFromScene (bool bIncludeEFFECT)
{
	if ( !m_bIsVisible || !m_hMorpher )
		return;

	m_bIsVisible = false;
	::removeFromScene( m_hMorpher );	// CObjGROUND::UnregisterFromNZIN
}

//-------------------------------------------------------------------------------------------------
