#include "stdafx.h"
#include ".\changevisibility.h"
#include "..\CObjCHAR.h"


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief default constructor
//----------------------------------------------------------------------------------------------------

CChangeVisibility::CChangeVisibility(void)
{
	m_iProgressTime			= 0;					/// It's time to finishing the change
	m_iTotalElaspedTime		= 0;					/// Elapsed time until now.

	m_pObjCHAR				= NULL;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Constructor
//----------------------------------------------------------------------------------------------------

CChangeVisibility::CChangeVisibility( CObjCHAR* pObjCHAR, int iProgressTime, bool bToDisappear ) : m_pObjCHAR( pObjCHAR )
{
	m_iProgressTime			= iProgressTime;					/// It's time to finishing the change
	m_iTotalElaspedTime		= 0;								/// Elapsed time until now.

	m_bToDisappear			= bToDisappear;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Destructor
//----------------------------------------------------------------------------------------------------

CChangeVisibility::~CChangeVisibility(void)
{
}


//----------------------------------------------------------------------------------------------------
/// @param int iElapsedFrameTime 이전 프레임으로 부터의 진행시간.
/// @brief Process visibility
//----------------------------------------------------------------------------------------------------

bool CChangeVisibility::ProcessVisibility( int iElapsedFrameTime )
{
	if( m_pObjCHAR == NULL )
	{
		assert( 0 && "CChangeVisibility::m_pObjCHAR is NULL" );
		return false;
	}

	m_iTotalElaspedTime += iElapsedFrameTime;

	/// prograss ratio
	float fElapseRatio = (float)m_iTotalElaspedTime / (float)m_iProgressTime;

	HNODE hNode = m_pObjCHAR->GetZMODEL();
	/// when distroyed
	if( m_bToDisappear == true )
	{
		::setVisibility( hNode, 1.0f - fElapseRatio );
	}else
	{
		::setVisibility( hNode, fElapseRatio );
	}

	/// expired..
	if( m_iTotalElaspedTime >= m_iProgressTime )
		return false;

	return true;
}
