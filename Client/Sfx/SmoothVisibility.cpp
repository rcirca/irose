#include "stdafx.h"
#include ".\smoothvisibility.h"
#include "../Game.h"


CSmoothVisibility::CSmoothVisibility(void)
{
	m_fCurrentVisibility	= 1.0f;
	m_bToDisappear			= false;		/// 사라지는 중이냐? 나타나는 중이냐?
	m_dwTransitionTime		= 0;			/// 변화에 걸리는 시간

	m_dwStartTime			= g_GameDATA.GetGameTime();	
	m_dwTotalElapsedTime	= 0;				/// Elapsed time until now.

	m_bTransitionActivity	= false;
}

CSmoothVisibility::~CSmoothVisibility(void)
{
}

void CSmoothVisibility::Start( DWORD dwTransitionTime, bool bToDisappear )
{
	m_fCurrentVisibility	= ( bToDisappear )? 1.0f:0.0f;
	m_bToDisappear			= bToDisappear;				/// 사라지는 중이냐? 나타나는 중이냐?
	m_dwTransitionTime		= dwTransitionTime;			/// 변화에 걸리는 시간

	m_dwStartTime			= g_GameDATA.GetGameTime();	
	m_dwTotalElapsedTime	= 0;

	m_bTransitionActivity	= true;
}

void CSmoothVisibility::Proc()
{
	if( !m_bTransitionActivity )
		return;

	DWORD dwCurrentTime = g_GameDATA.GetGameTime();
	m_dwTotalElapsedTime = dwCurrentTime - m_dwStartTime;
    
	/// expire
	if( m_dwTotalElapsedTime < m_dwTransitionTime )
	{
		m_fCurrentVisibility = (float)m_dwTotalElapsedTime / (float)m_dwTransitionTime;
		if( m_bToDisappear )
		{
			m_fCurrentVisibility = 1.0f - m_fCurrentVisibility;
		}
	}else
	{
		m_bTransitionActivity = false;
		m_fCurrentVisibility	= ( m_bToDisappear )? 0.0f:1.0f;
	}
}





//-----------------------------------------------------------------------------------------
///
/// CSmoothVisibilityCombo
/// 
/// Smooth visibility transition Combo
///
//-----------------------------------------------------------------------------------------

CSmoothVisibilityCombo::CSmoothVisibilityCombo()
{
	ResetTime();
}

CSmoothVisibilityCombo::~CSmoothVisibilityCombo()
{
}

void CSmoothVisibilityCombo::ResetTime()
{
	m_fCurrentVisibility		= 1.0f;

	m_dwStartTime				= g_GameDATA.GetGameTime();

	m_dwToFirstTransitionTime	= 0; /// if Disappear combo, use iToDisappear time.
	m_dwDurationTime			= 0;
	m_dwToSecondTransitionTime	= 0;

	/// flash
	m_dwFlashDurationTime		= 0; /// Total duration time
	m_dwFlashTime				= 0; /// One flash time

	m_bTransitionActivated		= false;
	m_iComboType				= 0;	
}

///
/// @param iToDisappear : Transition time to disappear
/// @param iDuration    : Duration time of invisible
/// @param iToAppear    : Transition time to appear
///
void CSmoothVisibilityCombo::StartDisappearCombo( DWORD dwToDisappear, DWORD dwDuration, DWORD dwToAppear )
{
	ResetTime();

	m_iComboType				= VISIBILITY_COMBO_TO_DISAPPEAR;
	m_bTransitionActivated		= true;

	m_dwToFirstTransitionTime	= dwToDisappear;
	m_dwDurationTime			= dwDuration;
	m_dwToSecondTransitionTime	= dwToAppear;
}

///	
/// @param iToAppear    : Transition time to appear
/// @param iDuration    : Duration time of invisible
/// @param iToDisappear : Transition time to disappear	
///
void CSmoothVisibilityCombo::StartAppearCombo( DWORD dwToAppear, DWORD dwDuration, DWORD dwToDisappear )
{
	ResetTime();

	m_iComboType				= VISIBILITY_COMBO_TO_APPEAR;
	m_bTransitionActivated		= true;

	m_dwToFirstTransitionTime	= dwToAppear;
	m_dwDurationTime			= dwDuration;
	m_dwToSecondTransitionTime	= dwToDisappear;
}

/// flash effect
void CSmoothVisibilityCombo::StartFlash( DWORD dwDurationTime, DWORD dwFlashTime )
{
	ResetTime();

	m_iComboType				= VISIBILITY_COMBO_FLASH;
	m_bTransitionActivated		= true;

	m_dwFlashDurationTime		= dwDurationTime; /// Total duration time
	m_dwFlashTime				= dwFlashTime; /// One flash time	
}

/// update visibility
void CSmoothVisibilityCombo::UpdateVisibility()
{
	if( !m_bTransitionActivated )
		return;

	switch( m_iComboType )
	{
		case VISIBILITY_COMBO_TO_DISAPPEAR:
			UpdateDisappearCombo();
			break;
		case VISIBILITY_COMBO_TO_APPEAR:
			UpdateAppearCombo();
			break;
		case VISIBILITY_COMBO_FLASH:
			UpdateFlashCombo();
			break;
	}
}

///
/// 사라졌다가 나타난다.
///
void CSmoothVisibilityCombo::UpdateDisappearCombo()
{
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();
	DWORD dwElapsedTime = dwCurrentTime - m_dwStartTime;

	if( dwElapsedTime > m_dwToFirstTransitionTime )
		m_dwToFirstTransitionTime = 0;

	if( dwElapsedTime > ( m_dwToFirstTransitionTime + m_dwDurationTime ) )
		m_dwDurationTime = 0;

	/// Expire
	if( dwElapsedTime > ( m_dwToFirstTransitionTime + m_dwDurationTime + m_dwToSecondTransitionTime ) )
	{
		m_dwToSecondTransitionTime	= 0;
		m_bTransitionActivated		= false;

		m_fCurrentVisibility		= 1.0f;
		return;
	}

	/// 1. transition to disappear
	if( m_dwToFirstTransitionTime > 0 )
	{
		DWORD dwTransitionTime = m_dwToFirstTransitionTime - dwElapsedTime;		
		m_fCurrentVisibility = (float)dwTransitionTime / (float)m_dwToFirstTransitionTime;

	}else if( m_dwDurationTime > 0 )
	{
		/// 2. duration		
		m_fCurrentVisibility = 0.0f;
		
	}else
	{
		/// 3. transition to appear
		DWORD dwTransitionTime = m_dwToSecondTransitionTime - ( dwElapsedTime - m_dwToFirstTransitionTime + m_dwDurationTime );		
		m_fCurrentVisibility = (float)dwTransitionTime / (float)m_dwToFirstTransitionTime;
	}
}

///
/// 나타났다가 사라짐
///
void CSmoothVisibilityCombo::UpdateAppearCombo()
{
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();
	DWORD dwElapsedTime = dwCurrentTime - m_dwStartTime;

	if( dwElapsedTime > m_dwToFirstTransitionTime )
		m_dwToFirstTransitionTime = 0;

	if( dwElapsedTime > ( m_dwToFirstTransitionTime + m_dwDurationTime ) )
		m_dwDurationTime = 0;

	/// Expire
	if( dwElapsedTime > ( m_dwToFirstTransitionTime + m_dwDurationTime + m_dwToSecondTransitionTime ) )
	{
		m_dwToSecondTransitionTime	= 0;
		m_bTransitionActivated		= false;

		m_fCurrentVisibility		= 1.0f;

		return;
	}

		/// 1. transition to appear		
	if( m_dwToFirstTransitionTime > 0 )
	{
		DWORD dwTransitionTime = m_dwToFirstTransitionTime - dwElapsedTime;		
		m_fCurrentVisibility = 1.0f - (float)dwTransitionTime / (float)m_dwToFirstTransitionTime;

	}else if( m_dwDurationTime > 0 )
	{
		/// 2. duration
		m_fCurrentVisibility = 1.0f;
		
	}else
	{
		/// 3. transition to disappear
		DWORD dwTransitionTime = m_dwToSecondTransitionTime - ( dwElapsedTime - m_dwToFirstTransitionTime + m_dwDurationTime );		
		m_fCurrentVisibility = 1.0f - (float)dwTransitionTime / (float)m_dwToFirstTransitionTime;
	}
}

///
/// 깜빡임
///
void CSmoothVisibilityCombo::UpdateFlashCombo()
{
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();
	DWORD dwElapsedTime = dwCurrentTime - m_dwStartTime;

	/// expire
	if( dwElapsedTime > m_dwFlashDurationTime )
	{
		m_bTransitionActivated = false;
		return;
	}

	if( ( dwElapsedTime / m_dwFlashTime ) & 1 )
		m_fCurrentVisibility = 0.0f;
	else
		m_fCurrentVisibility = 1.0f;
}