/*
	$Header: /Client/CCamera.cpp 37    04-10-11 8:21p Navy $
*/
#include "stdAFX.h"
#include "CCamera.h"
#include "Game.h"
#include "IO_Terrain.h"


#define	MIN_DISTANCE	300.f
#ifndef _DEBUG
#define	MAX_DISTANCE	( 1800.f * ((g_GameDATA.m_bFilmingMode)? 2.0f : 1.0f) )
#else
#define	MAX_DISTANCE	( 230000 )
#endif


CCamera* CCamera::m_pInstance = NULL;


CCamera::CCamera ()
{
	m_hNODE		= NULL;
	m_hMotion	= NULL;
}
CCamera::~CCamera ()
{
	;
}

CCamera *CCamera::Instance ()
{
	if ( !m_pInstance )
		m_pInstance = new CCamera;

	return m_pInstance;
}

///*------------------------------------------------------------
/// 2004 / 3/ 31 :수정 nAvy
/// 1. unloadCamera시에 Error발생
/// =>Instance를 지우면 멤버 변수에는 쓰레기 값이 들어가게 된다.
/// =>Instance삭제를 맨 밑으로 이동하거나 Destructor에서 멤버변수를 지우자.
void CCamera::Destroy ()
{
	if( m_hNODE != NULL )
	{
		::unloadCamera( m_hNODE );
		m_hNODE = NULL;
	}

	if( m_hMotion != NULL )
	{
		::unloadMotion( m_hMotion );
		m_hMotion = NULL;
	}

	SAFE_DELETE( m_pInstance );
}

//-------------------------------------------------------------------------------------------------
void CCamera::Init (HNODE hNODE)
{
	m_hNODE = hNODE;

	setCameraDefault( hNODE );

	m_Angle.x = 25.f;
	m_Angle.y = 180.f;
	m_Angle.z = 0.f;

	m_fDistance = 1000.f;

	// -- 카메라 초기값 지정
	m_fYAW   = 0;			// [-180, 180]
	m_fPITCH = 0.5;			// [0, 1]
	m_bFollowMode = false;	// 1 : 뒤에서 따라가는 모드, 0 : 3인칭 모드

	if( m_hMotion != NULL )
	{
		::unloadMotion( m_hMotion );
		m_hMotion = NULL;
	}
}



void CCamera::SetSightInfo( int iInfoIdx )
{

}

void CCamera::ResetToAvatarCamera()
{
	if( m_hNODE != NULL )
	{
		HNODE hMotion = findNode("SelectAvatarCameraMotion");
		if( hMotion )
		{
			attachMotion(m_hNODE, 0 );
			unloadMotion ( hMotion );
		}

		::unloadCamera( m_hNODE );
		m_hNODE = NULL;
	}

	m_hNODE = ::findNode( "avatar_camera" );

	if( m_hNODE == NULL )
	{
		LogString( LOG_DEBUG, "Load camera failed[ cameras/camera01.zca ]"  );
		return;
	}

	setCameraDefault( m_hNODE );

	::setPosition( m_hNODE, m_PosAT.x, m_PosAT.y, m_PosAT.z );
}

/// set motion to camera

void CCamera::SetMotion( const char* strMotion )
{
	if( m_hNODE == NULL )
		return;

	if( m_hMotion != NULL )
	{
		::unloadMotion( m_hMotion );
		m_hMotion = NULL;
	}

	m_hMotion = ::findNode( "CameraMotion" );
	if( m_hMotion != NULL )
	{
		::unloadMotion( m_hMotion );
		m_hMotion = NULL;
	}


	int ZZ_INTERP_SQUAD = 3, ZZ_INTERP_CATMULLROM = 4;
	m_hMotion = ::loadMotion( "CameraMotion", strMotion, 1, ZZ_INTERP_CATMULLROM, ZZ_INTERP_SQUAD, 1, 1 );

	if( m_hMotion == NULL )
	{
		LogString( LOG_DEBUG, "Camera motion loading failed[ %s ]", strMotion );
		return;
	}

	::attachMotion( m_hNODE, m_hMotion );

	::controlAnimatable( m_hNODE, 1 );
}




bool CCamera::Toggle_FloowMODE ()
{
	m_bFollowMode = !m_bFollowMode;

	::setCameraFollowMode ( m_hNODE, m_bFollowMode );

	return m_bFollowMode;
}

void CCamera::Add_PITCH (short nMovement)
{
	m_fPITCH += ((float)nMovement / g_pCApp->GetHEIGHT() );

    if (m_fPITCH < 0.2) m_fPITCH = 0.2f;
	else
    if (m_fPITCH > 1.0) m_fPITCH = 1.0f;

    setCameraFollowPitch ( m_hNODE, m_fPITCH );
}

void CCamera::Add_YAW (short nMovement)
{
	const float fMaxAngle = 180.f;

	// -- 현재 적용된 각 얻어오기(setCameraFollowYaw()에 의해 이전에 적용된 값과 다를 수 있음)
	m_fYAW = getCameraFollowYaw(m_hNODE);

	// -- 마우스의 x축 값으로 회전각 정의(이전각에 누적시킴)
	m_fYAW = m_fYAW - 480.f * nMovement / g_pCApp->GetWIDTH();

	// -- 최대각과 최소각으로 제한
/*
	if ( m_fYAW < -fMaxAngle ) m_fYAW = -fMaxAngle;
	else
	if ( m_fYAW >  fMaxAngle ) m_fYAW = fMaxAngle;
*/
	// -- 카메라 회전각 설정 (값의 범위는 -180 도에서 180 도까지)
	setCameraFollowYaw ( m_hNODE, m_fYAW );
}

void CCamera::Add_Distance (float fDistance)
{	
	m_fDistance += fDistance;
	if ( m_fDistance < MIN_DISTANCE )
		m_fDistance = MIN_DISTANCE;
	else
	if ( m_fDistance > MAX_DISTANCE )
		m_fDistance = MAX_DISTANCE;    

	::setCameraFollowDistance( m_hNODE, m_fDistance );
}


void CCamera::Attach (HNODE hModel)
{	
	::cameraAttachTarget   (m_hNODE, hModel);
	::setCameraTargetHeight( m_hNODE, 170 );
	::setCameraFollowDistance ( m_hNODE, m_fDistance );
	::setCameraFollowYaw      ( m_hNODE, m_fYAW );
	::setCameraFollowPitch    ( m_hNODE, m_fPITCH );
	::setCameraFollowMode     ( m_hNODE, m_bFollowMode );

	::setCameraFollowDistanceRange( m_hNODE, MIN_DISTANCE, MAX_DISTANCE * 2.0f );
}

//#ifdef	_DEBUG
D3DVECTOR CCamera::Get_Position (void)
{		
	::getCameraEye( m_hNODE, (float*)&m_PosAT );

	return m_PosAT;
}
//#endif
//-------------------------------------------------------------------------------------------------
void CCamera::Set_Position (float fX, float fY, float fZ)
{
	m_PosAT.x=fX;
	m_PosAT.y=fY;
	m_PosAT.z=fZ;

	::setPosition( m_hNODE, fX, fY, fZ);
	//::setPosition( m_hNODE, 0.0f, 0.0f, 0.0f );
}


void CCamera::LookAt( float eyeX, float eyeY, float eyeZ,
						float centerX, float centerY, float centerZ,
						float upX, float upY, float upZ )
{
	::lookAt( m_hNODE, eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ );
}

