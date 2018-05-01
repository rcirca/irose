#include "stdafx.h"
#include ".\changeactionmode.h"
#include "../Game.h"
#include "../CObjCHAR.h"


const int CHANGE_NORMAL_MODE_TIME = 10000;

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief default constructor
//----------------------------------------------------------------------------------------------------

CChangeActionMode::CChangeActionMode(void)
{
	m_iAvatarActionMode		= AVATAR_NORMAL_MODE;
	m_iElapsedAttackEnd		= 0;					// the elapsed time from attack end.

	m_bUpdateMode			= false;
}


//----------------------------------------------------------------------------------------------------
/// @param CObjCHAR* pObjCHAR Owner
/// @brief Constructor
//----------------------------------------------------------------------------------------------------

CChangeActionMode::CChangeActionMode( CObjCHAR* pObjCHAR )
{
	m_iAvatarActionMode		= AVATAR_NORMAL_MODE;
	m_iElapsedAttackEnd		= 0;					// the elapsed time from attack end.

	m_bUpdateMode			= false;

	m_pObjCHAR				= pObjCHAR;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Destructor
//----------------------------------------------------------------------------------------------------

CChangeActionMode::~CChangeActionMode(void)
{
}


//----------------------------------------------------------------------------------------------------
/// @param int iActionMode 목적 액션 타입
/// @brief 액션 모드 전환( 노멀 -> 어택모드, 어택 -> 노멀 )
//----------------------------------------------------------------------------------------------------

void CChangeActionMode::ChangeActionMode( int iActionMode )
{ 
	switch( iActionMode )
	{		
		case AVATAR_NORMAL_MODE:
			{
				/// Attack -> Normal mode 로 진입
				if( m_iAvatarActionMode == AVATAR_ATTACK_MODE )
					AttackEnd();				
				else
					m_iAvatarActionMode = iActionMode;
			}
			break;		
		case AVATAR_ATTACK_MODE:
			{
				/// Normal mode -> Attack Mode  로 진입
				if( m_iAvatarActionMode == AVATAR_NORMAL_MODE )
					AttackStart();

				m_iAvatarActionMode = iActionMode;
				m_pObjCHAR->SetUpdateMotionFlag( true );
			}
			break;
	}	
}


//----------------------------------------------------------------------------------------------------
/// @param int iActionIdx 액션 인덱스
/// @brief Get proper motion by action mode
//----------------------------------------------------------------------------------------------------

int CChangeActionMode::GetAdjustedActionIndex( int iActionIdx )
{
	int iAdjustedActionIdx = iActionIdx;

	switch( iActionIdx )
	{
		case AVT_ANI_STOP1:			
		case AVT_ANI_STOP2:
			{
				switch( m_iAvatarActionMode )
				{
					case AVATAR_NORMAL_MODE:
						iAdjustedActionIdx = AVT_ANI_STOP1;
						break;
					case AVATAR_ATTACK_MODE:
						iAdjustedActionIdx = AVT_ANI_STOP3;
						break;
				}
			}
			break;

		case AVT_ANI_WALK:
		case AVT_ANI_RUN:
		case AVT_ANI_SITTING:
		case AVT_ANI_SIT:
		case AVT_ANI_STANDUP:
		case AVT_ANI_STOP3:
		case AVT_ANI_ATTACK:
		case AVT_ANI_ATTACK2:
		case AVT_ANI_ATTACK3:
		case AVT_ANI_HIT:
		case AVT_ANI_FALL:
		case AVT_ANI_DIE:
		case AVT_ANI_RAISE:
		case AVT_ANI_JUMP1:
		case AVT_ANI_JUMP2:
			break;
	}

	return iAdjustedActionIdx;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Attack start. change attack mode.
//----------------------------------------------------------------------------------------------------

void CChangeActionMode::AttackStart()
{
	m_iElapsedAttackEnd = 0;
	m_bUpdateMode		= false;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief if avt finished attack command, then change action mode to normal mode after 10 sec from attack end time.
//----------------------------------------------------------------------------------------------------

void CChangeActionMode::AttackEnd()
{
	m_iElapsedAttackEnd = 0;
	m_bUpdateMode		= true;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Change action mode by checking time..
//----------------------------------------------------------------------------------------------------

void CChangeActionMode::Proc()
{
	if( m_bUpdateMode == true )
	{
		m_iElapsedAttackEnd += g_GameDATA.GetElapsedFrameTime();

		if( m_iElapsedAttackEnd >= CHANGE_NORMAL_MODE_TIME )
		{
			//ChangeActionMode( AVATAR_NORMAL_MODE );
			m_iAvatarActionMode = AVATAR_NORMAL_MODE; 
			m_pObjCHAR->SetUpdateMotionFlag( true );

			m_bUpdateMode = false;
		}
	}
}