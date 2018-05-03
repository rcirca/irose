#include "stdafx.h"
#include ".\objvibration.h"
#include "../Game.h"
#include "../CObjCHAR.h"


const int DEFAULT_VIBRATION_INTERVAL	= 20;
const int DEFAULT_VIBRATION_SCALE		= 30;
const int DEFAULT_DURATION_TIME			= 300;

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjvibration::CObjvibration(void)
{
	 m_iDurationTime		= DEFAULT_DURATION_TIME;			///< Vibration duration time
	 m_iVibrationInterval	= DEFAULT_VIBRATION_INTERVAL;
	 m_iStartTime			= g_GameDATA.GetGameTime();
	 m_iElapsedTime			= 0;
	 m_iElapsedVibrationTime= 0;

	 m_pObjCHAR				= NULL;
	 m_bActivated			= false;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjvibration::~CObjvibration(void)
{
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

CObjvibration::CObjvibration( CObjCHAR* pObjCHAR, int iDurationTime = DEFAULT_DURATION_TIME )
{
	 m_iDurationTime		= iDurationTime;			///< Vibration duration time
	 m_iVibrationInterval	= DEFAULT_VIBRATION_INTERVAL;
	 m_iStartTime			= g_GameDATA.GetGameTime();
	 m_iElapsedTime			= 0;
	 m_iElapsedVibrationTime= 0;

	 m_pObjCHAR				= pObjCHAR;

	 m_bActivated			= false;

	 m_vOldObjPos			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief start vibration of obj..
//----------------------------------------------------------------------------------------------------

void CObjvibration::StartVibration()
{
	if( m_pObjCHAR == NULL )
		return;

	/// 이미 떨고 있는데 또맞는다면..
	if( m_bActivated )
	{
		return;
		//m_pObjCHAR->ResetCUR_POS( m_vOldObjPos );	
	}

	m_iStartTime			= g_GameDATA.GetGameTime();
	m_iElapsedTime			= 0;
	m_iElapsedVibrationTime = 0;
	
	m_vOldObjPos			= m_pObjCHAR->Get_CurPOS();

	m_bActivated			= true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief end vibration
//----------------------------------------------------------------------------------------------------

void CObjvibration::EndVibration()
{
	if( m_bActivated )
	{
		m_bActivated = false;
		m_pObjCHAR->ResetCUR_POS( m_vOldObjPos );		
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief update procedure..
//----------------------------------------------------------------------------------------------------

void CObjvibration::Proc()
{

	if( !m_bActivated )
		return;

	if( m_pObjCHAR == NULL )
		return;


	m_iElapsedTime = g_GameDATA.GetGameTime() - m_iStartTime;

	/// Time out
	if( m_iElapsedTime > m_iDurationTime )
	{
		EndVibration();
		return;
	}
	
	/// Vibration
	if( m_iElapsedTime - m_iElapsedVibrationTime > m_iVibrationInterval )
	{
		m_iElapsedVibrationTime = ( m_iElapsedTime / m_iVibrationInterval ) * m_iVibrationInterval;

		Vibration();		
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Vibration algorithm..
//----------------------------------------------------------------------------------------------------

void CObjvibration::Vibration()
{
	if( m_pObjCHAR == NULL )
		return;

	int iX = g_GameDATA.Random( DEFAULT_VIBRATION_SCALE );
	int iY = g_GameDATA.Random( DEFAULT_VIBRATION_SCALE );
	int iZ = g_GameDATA.Random( DEFAULT_VIBRATION_SCALE );

	D3DXVECTOR3 vNewPos( m_vOldObjPos.x + iX, m_vOldObjPos.y + iY, m_vOldObjPos.z + iZ );

	m_pObjCHAR->ResetCUR_POS( vNewPos );	
}
