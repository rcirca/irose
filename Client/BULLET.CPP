/*
	$Header: /Client/BULLET.CPP 50    05-10-19 6:36p Raning $
*/
#include "stdAFX.h"
#include "BULLET.h"
#include "CGameOBJ.h"
#include "Debug.h"

//-------------------------------------------------------------------------------------------------

CBulletMANAGER* CBulletMANAGER::m_pInstance=NULL;

static const D3DXVECTOR3 s_xAxis(1, 0, 0); // x-축

const float ACCELERATOR_OF_GRAVITY = -( 9.8f / 1000.0f );


///
/// 직선 이동 총알
///

/// 현재 위치를 구하고.. 각 맴버세팅..
void CBulletDIRECTION::Cal_MoveVEC (CObjCHAR *pDest)
{
	// 바라볼 모델의 위치 얻기 -> pos
	::getDummyPosition ( pDest->GetZMODEL(), pDest->GetLastDummyIDX(), (float*)&this->m_PosTO );

	/// 실제 타겟의 50cm 위로 발사..
	m_PosTO.z += 50.0f;

	::rotateByVectorRelative( this->GetZNODE(), s_xAxis, (float*)&this->m_PosTO );

	::getPosition( this->GetZNODE(),	(float*)&this->m_PosCUR );
	this->m_PosFROM  = m_PosCUR;

	D3DXVECTOR3	Dir;
	Dir = this->m_PosTO - this->m_PosCUR;
	// z축거리 제외..
	float fDistance = (float)sqrt( (Dir.x*Dir.x)+(Dir.y*Dir.y) );

	/// 이동벡터
	if( fDistance < 0.0001f )
	{
		this->m_MoveVEC = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}else	
		this->m_MoveVEC = ( Dir * m_fSpeed ) / ( fDistance * 1000 );	// 1/1000초당 이동량.

	m_iDistance = MUL02( (int)fDistance );
}

//-------------------------------------------------------------------------------------------------
bool CBulletDIRECTION::Init ( CObjCHAR *pSour, CObjCHAR *pDest, int iEffectIDX, bool bLinkToPoint, int iPointNO, bool bDummy, int iSkillIDX )
{
	m_iSkillIDX = iSkillIDX;
	m_bDummyBullet = bDummy;

	short nEffectFileIDX = EFFECT_BULLET_NORMAL( iEffectIDX );
	t_HASHKEY HashKEY = g_pEffectLIST->Get_IndexHashKEY( nEffectFileIDX );
	if ( !this->Create( g_pEffectLIST->Get_EffectDATA( HashKEY ) ) ) {
		return false;
	}

	this->m_nSourOBJ	= pSour->m_nIndex;
	this->m_nDestOBJ	= pDest->m_nIndex;
	this->m_nBulletNO	= iEffectIDX;

	/// 무기에 붙일꺼냐? 오른손에 붙일꺼냐?
	if( !bLinkToPoint && ( pSour->Is_AVATAR() ) )
		pSour->LinkEffectToPOINT( this, BODY_PART_WEAPON_R, 0 );
	else
	{		
		pSour->LinkEffectToDUMMY( this, iPointNO );
	}

	this->UnlinkVisibleWorld ();
	this->InsertToScene ();
	this->m_fSpeed = (float)EFFECT_BULLET_SPEED( iEffectIDX );

	Cal_MoveVEC( pDest );

	// 발사 효과음.
	if ( EFFECT_FIRE_SND_IDX( iEffectIDX ) )
		g_pSoundLIST->IDX_PlaySound( EFFECT_FIRE_SND_IDX( iEffectIDX ) );

	return true;
}

#define	__HIT_100_PERCENT
int CBulletDIRECTION::Proc ()
{
	if ( m_dwHitTIME ) {
		if ( g_pBltMGR->GetCurTime()-m_dwHitTIME > 15*1000 )
			return 0;
		CObjCHAR *pDestOBJ = g_pObjMGR->Get_CharOBJ( m_nDestOBJ, true );
		return ( pDestOBJ ) ? 1 : 0;
	}

	/// Elapse 된 타임만큼 현재위치를 갱신해서, 그 위치로 강제로 세팅..
	m_PosCUR += ( m_MoveVEC * (float)g_pBltMGR->GetPassTime() );

	this->SetPOSITION( m_PosCUR );

	if ( m_nDestOBJ ) 
	{
		CObjCHAR *pDestOBJ = g_pObjMGR->Get_CharOBJ( m_nDestOBJ, true );
		if ( NULL == pDestOBJ ) {
			m_nDestOBJ = 0;
			return 1;
		}

		// 100% 타격 되도록 !!
		this->Cal_MoveVEC( pDestOBJ );

		///LogString( LOG_NORMAL, "m_MoveVEC : x:%f, y:%f, z:%f\n", m_MoveVEC.x, m_MoveVEC.y, m_MoveVEC.z );
		

		if ( this->IsIntersect( pDestOBJ->GetZMODEL() ) ) 
		{
			// 충돌 !!! 더미에 꼽는다.
			pDestOBJ->Link2LastDummy ( this->GetZNODE() );			

			m_dwHitTIME = g_pBltMGR->GetCurTime();

			CObjCHAR *pSourOBJ = g_pObjMGR->Get_CharOBJ( m_nSourOBJ, true );
			pDestOBJ->Hitted( pSourOBJ, 
								this->m_nBulletNO, 
								this->m_iSkillIDX, 
								(this->m_iSkillIDX)? true:false, 
								true, 
								m_bDummyBullet );

			/// 맞으면 총알을 없앤다.. 이게.. 부활시 다운되는거랑 관계있다.. 왜냐.. 이놈이 꽂혀 있는데 아바타가 워프하면 대략 낭패..
			return 0;
		}

	} else
	{
		return 0;
	}

	return 1;
}



///
/// 직선 이동 지역 타겟
///
/// 현재 위치를 구하고.. 각 맴버세팅..
void CBulletToPosition::Cal_MoveVEC ( D3DXVECTOR3 TargetPos )
{	
	this->m_PosTO = TargetPos;

	/// 실제 타겟의 50cm 위로 발사..
	m_PosTO.z += 50.0f;
	
	::rotateByVectorRelative( this->GetZNODE(), s_xAxis, (float*)&this->m_PosTO );

	::getPosition( this->GetZNODE(),	(float*)&this->m_PosCUR );
	this->m_PosFROM  = m_PosCUR;

	D3DXVECTOR3	Dir;
	Dir = this->m_PosTO - this->m_PosCUR;
	// z축거리 제외..
	float fDistance = (float)sqrt( (Dir.x*Dir.x)+(Dir.y*Dir.y) );

	/// 이동벡터
	if( fDistance < 0.0001f )
	{
		this->m_MoveVEC = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}else	
		this->m_MoveVEC = ( Dir * m_fSpeed ) / ( fDistance * 1000 );	// 1/1000초당 이동량.

	this->m_MoveVEC = ( Dir * m_fSpeed ) / ( fDistance * 1000 );	// 1/1000초당 이동량.

	m_iDistance = MUL02( (int)fDistance );
}

//-------------------------------------------------------------------------------------------------
bool CBulletToPosition::Init ( CObjCHAR *pSour, D3DXVECTOR3 TargetPos, int iEffectIDX, bool bLinkToPoint, int iPointNO, bool bDummy, int iSkillIDX )
{
	m_iSkillIDX = iSkillIDX;
	m_bDummyBullet = bDummy;

	short nEffectFileIDX = EFFECT_BULLET_NORMAL( iEffectIDX );
	t_HASHKEY HashKEY = g_pEffectLIST->Get_IndexHashKEY( nEffectFileIDX );
	if ( !this->Create( g_pEffectLIST->Get_EffectDATA( HashKEY ) ) ) {
		return false;
	}

	this->m_nSourOBJ	= pSour->m_nIndex;
	this->m_nDestOBJ	= 0;
	this->m_nBulletNO	= iEffectIDX;

	/// 무기에 붙일꺼냐? 오른손에 붙일꺼냐?
	if( !bLinkToPoint && ( pSour->Is_AVATAR() ))
		pSour->LinkEffectToPOINT( this, BODY_PART_WEAPON_R, 0 );
	else
		pSour->LinkEffectToDUMMY( this, iPointNO );

	this->UnlinkVisibleWorld ();
	this->InsertToScene ();
	this->m_fSpeed = (float)( EFFECT_BULLET_SPEED( iEffectIDX ) );

	Cal_MoveVEC( TargetPos );

	// 발사 효과음.
	if ( EFFECT_FIRE_SND_IDX( iEffectIDX ) )
		g_pSoundLIST->IDX_PlaySound( EFFECT_FIRE_SND_IDX( iEffectIDX ) );

	return true;
}

int CBulletToPosition::Proc ()
{
	if ( m_dwHitTIME ) 
	{
		if ( g_pBltMGR->GetCurTime()-m_dwHitTIME > 15*1000 )
			return 0;

		return 1;
	}

	/// Elapse 된 타임만큼 현재위치를 갱신해서, 그 위치로 강제로 세팅..
	m_PosCUR += ( m_MoveVEC * (float)g_pBltMGR->GetPassTime() );

	this->SetPOSITION( m_PosCUR );


	/// 총알의 초기위치와 현재 위치와의 거리.
	int iDistance1 = CD3DUtil::distance ((int)m_PosCUR.x, (int)m_PosCUR.y, (int)m_PosFROM.x, (int)m_PosFROM.y);	

	if ( iDistance1 >= m_iDistance ) {
		// 타겟이 없어 졌거나, 지나쳤다.
		m_dwHitTIME = g_pBltMGR->GetCurTime();		
	}

	return 1;
}



CBulletPARABOLA::CBulletPARABOLA()
{
	m_vZ0				= 0.0f;
	m_vZ				= 0.0f;
	
	m_DisplacementVec	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_fDistance			= 0.0f;	
	m_fArrivedTime		= 0;
	m_dwElapsedTime		= 0;
}

CBulletPARABOLA::~CBulletPARABOLA()
{
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 포물선 이동 총알
//----------------------------------------------------------------------------------------------------
void CBulletPARABOLA::Cal_MoveVEC ( CObjCHAR *pDest )
{
	//----------------------------------------------------------------------------------------------------
	/// 바라볼 모델의 위치 얻기 -> pos
	/// 총알의 시작위치( m_PosFROM )과 총알의 목표위치를 구한다.
	//----------------------------------------------------------------------------------------------------
	::getDummyPosition ( pDest->GetZMODEL(), pDest->GetLastDummyIDX(), (float*)&this->m_PosTO );

	/// 실제 타겟의 50cm 위로 발사..
	m_PosTO.z += 50.0f;

	::rotateByVectorRelative( this->GetZNODE(), s_xAxis, (float*)&this->m_PosTO );
	::getPosition( this->GetZNODE(),	(float*)&this->m_PosCUR );
	this->m_PosFROM = m_PosCUR;
		
	//----------------------------------------------------------------------------------------------------
	/// 구한 총알의 시작위치와 목표위치를 기반으로 총알의 방향및 초기 속도를 구한다.
	//----------------------------------------------------------------------------------------------------	
	m_dwElapsedTime		= 0;
	m_DisplacementVec	= this->m_PosTO - this->m_PosCUR;
	
	// z축거리 제외..
	m_fDistance = (float)sqrt( (m_DisplacementVec.x*m_DisplacementVec.x)+(m_DisplacementVec.y*m_DisplacementVec.y) );	
	

	/// 이동벡터
	if( m_fDistance < 0.0001f )
	{
		this->m_MoveVEC = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}else	
		this->m_MoveVEC = ( m_DisplacementVec * m_fSpeed ) / ( m_fDistance * 1000.0f );	// 1/1000초당 이동량.

	/// 도달시간..
	m_fArrivedTime = abs( m_fDistance / m_fSpeed );	/// t = s / v;	

	/// 도달거리와 목표점도달까지의 시간을 알게됬으므로 z 축의 초기 속도를 구할수 있다.
	m_vZ0 = ( -( ACCELERATOR_OF_GRAVITY * 1000.0f ) * m_fArrivedTime ) /  2.0f;
	m_vZ = m_vZ0;
	this->m_MoveVEC.z = m_vZ0;

	/*if( _isnan( m_MoveVEC.x ) || _isnan( m_MoveVEC.y ) || _isnan( m_MoveVEC.z ) ) 
	{		
		
		sprintf( g_MsgBuf, "[CALC]m_MoveVEC : x:%f, y:%f, z:%f\n", m_MoveVEC.x, m_MoveVEC.y, m_MoveVEC.z );
		g_TempDebugMsg += std::string( g_MsgBuf );		
		memset( 0, 0, 100 );
	}*/


	m_iDistance = MUL02( (int)m_fDistance );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 초기 설정은 기본 총알과 같음
//----------------------------------------------------------------------------------------------------
bool CBulletPARABOLA::Init ( CObjCHAR *pSour, CObjCHAR *pDest, int iEffectIDX, bool bLinkToPoint, int iPointNO, bool bDummy, int iSkillIDX )
{
	m_iSkillIDX = iSkillIDX;
	m_bDummyBullet = bDummy;

	short nEffectFileIDX = EFFECT_BULLET_NORMAL( iEffectIDX );
	t_HASHKEY HashKEY = g_pEffectLIST->Get_IndexHashKEY( nEffectFileIDX );
	if ( !this->Create( g_pEffectLIST->Get_EffectDATA( HashKEY ) ) ) {
		return false;
	}

	this->m_nSourOBJ	= pSour->m_nIndex;
	this->m_nDestOBJ	= pDest->m_nIndex;
	this->m_nBulletNO	= iEffectIDX;

	/// 무기에 붙일꺼냐? 오른손에 붙일꺼냐?
	if( !bLinkToPoint && ( pSour->Is_AVATAR() ) )
		pSour->LinkEffectToPOINT( this, BODY_PART_WEAPON_R, 0 );
	else
	{		
		pSour->LinkEffectToDUMMY( this, iPointNO );
	}

	this->UnlinkVisibleWorld ();
	this->InsertToScene ();
	this->m_fSpeed = ( float )( EFFECT_BULLET_SPEED( iEffectIDX ) );

	Cal_MoveVEC( pDest );

	// 발사 효과음.
	if ( EFFECT_FIRE_SND_IDX( iEffectIDX ) )
		g_pSoundLIST->IDX_PlaySound( EFFECT_FIRE_SND_IDX( iEffectIDX ) );

	return true;
}

bool CBulletPARABOLA::Init ( CObjCHAR *pSour, D3DXVECTOR3 PosTo, int iEffectIDX, bool bLinkToLeftHand, int iPointNO, bool bDummy, int iSkillIDX )
{
	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 설정된 Move vec 에 기반에서 위치 업데이트
//----------------------------------------------------------------------------------------------------
int	 CBulletPARABOLA::Proc ()
{
	if ( m_dwHitTIME ) 
	{
		if ( g_pBltMGR->GetCurTime()-m_dwHitTIME > 15*1000 )
			return 0;
		CObjCHAR *pDestOBJ = g_pObjMGR->Get_CharOBJ( m_nDestOBJ, true );
		return ( pDestOBJ ) ? 1 : 0;
	}

	DWORD dwPassTime = g_pBltMGR->GetPassTime();
	m_dwElapsedTime = dwPassTime;

	//----------------------------------------------------------------------------------------------------
	/// 중력가속도를 적용해서 Z 축에 대한 속도를 계산한다.
	//----------------------------------------------------------------------------------------------------
	float Z1 = ( ACCELERATOR_OF_GRAVITY * (float)dwPassTime );
	m_vZ += Z1 ;
	m_MoveVEC.z = m_vZ;	

	/// Elapse 된 타임만큼 현재위치를 갱신해서, 그 위치로 강제로 세팅..
	m_PosCUR += ( m_MoveVEC * (float)dwPassTime );

	float fAdjustHeight = m_DisplacementVec.z * ( ( (float)m_dwElapsedTime / 1000.0f ) / m_fArrivedTime );

	m_PosCUR.z += fAdjustHeight;


	/*if( _isnan( m_PosCUR.x ) || _isnan( m_PosCUR.y ) || _isnan( m_PosCUR.z ) ) 
	{
		
		
		sprintf( g_MsgBuf, "[PROC]m_MoveVEC : x:%f, y:%f, z:%f\n", m_MoveVEC.x, m_MoveVEC.y, m_MoveVEC.z );
		g_TempDebugMsg += std::string( g_MsgBuf );		

		sprintf( g_MsgBuf, "[PROC]m_PosCUR : x:%f, y:%f, z:%f\n", m_PosCUR.x, m_PosCUR.y, m_PosCUR.z );
		g_TempDebugMsg += std::string( g_MsgBuf );		

		sprintf( g_MsgBuf, "[PROC]PassTime[ %d ], AdjustHeight[ %f ]\n", dwPassTime, fAdjustHeight );
		g_TempDebugMsg += std::string( g_MsgBuf );		

		memset( 0, 0, 100 );
	}*/


	this->Rotation( m_MoveVEC );
	this->SetPOSITION( m_PosCUR );	

	///LogString( LOG_NORMAL, "m_MoveVEC[ %f, %f, %f ]\n", m_MoveVEC.x, m_MoveVEC.y, m_MoveVEC.z );
	///LogString( LOG_NORMAL, "PARA POS[ %f, %f, %f ], m_MoveVEC[ %f, %f, %f ][ Z1:%f ][ PassTime:%d]\n", m_PosCUR.x, m_PosCUR.y, m_PosCUR.z, m_MoveVEC.x, m_MoveVEC.y, m_MoveVEC.z, Z1, dwPassTime );

	if ( m_nDestOBJ ) 
	{
		CObjCHAR *pDestOBJ = g_pObjMGR->Get_CharOBJ( m_nDestOBJ, true );
		if ( NULL == pDestOBJ ) {
			m_nDestOBJ = 0;
			return 1;
		}

		// 100% 타격 되도록 !!
		///this->ReCal_MoveVEC( pDestOBJ );
		///if (  ) 
		/// 일단 첫 타겟위치로..
		if( m_MoveVEC.z <= -m_vZ0 || this->IsIntersect( pDestOBJ->GetZMODEL() ) )
		{
			/// 포물선 운동을 하는 총알을 원래 목표했던 타겟에서 그냥 터짐..

			m_dwHitTIME = g_pBltMGR->GetCurTime();

			CObjCHAR *pSourOBJ = g_pObjMGR->Get_CharOBJ( m_nSourOBJ, true );
			pDestOBJ->Hitted( pSourOBJ, 
								this->m_nBulletNO, 
								this->m_iSkillIDX, 
								(this->m_iSkillIDX)? true:false, 
								true, 
								m_bDummyBullet );

			/// 맞으면 총알을 없앤다.. 이게.. 부활시 다운되는거랑 관계있다.. 왜냐.. 이놈이 꽂혀 있는데 아바타가 워프하면 대략 낭패..
			return 0;
		}

	} else
	{
		return 0;
	}

	return 1;
}











///
/// Bullet Manager class
///
CBulletMANAGER::CBulletMANAGER ()
{
	m_iCreateOrder = 0;
	m_dwCurTime = g_GameDATA.GetGameTime();
	m_dwPassTime = 0;
}
CBulletMANAGER::~CBulletMANAGER ()
{
}
CBulletMANAGER* CBulletMANAGER::Instance ()
{
	if ( !m_pInstance )
		m_pInstance = new CBulletMANAGER;

	return m_pInstance;
}
void CBulletMANAGER::Destroy ()
{
	SAFE_DELETE( m_pInstance );
}

//-------------------------------------------------------------------------------------------------
CBullet *CBulletMANAGER::Add_BULLET(	CObjCHAR *pSour,
										CObjCHAR *pDest,
										int iEffectIDX,
										bool bLinkToPoint,
										int iPointNO,
										D3DXVECTOR3 PosTo,
										bool bUseTargetPos,
										bool bDummy,
										int iSkillIDX )
{
	CBullet *pBullet;

	switch( EFFECT_BULLET_MOVETYPE( iEffectIDX ) ) 
	{
		case BULLET_MOVE_DIRECTION	:
		{
			if( !bUseTargetPos )
			{
				pBullet = new CBulletDIRECTION;
				
				/// if bSkillEffect is true, link to right hand dummy
				if ( !pBullet->Init( pSour, pDest, iEffectIDX, bLinkToPoint, iPointNO, bDummy, iSkillIDX ) ) 
				{
					SAFE_DELETE( pBullet );
					return NULL;
				}				
				
			}else /// 그냥 타겟으로 날린다.
			{
				pBullet = new CBulletToPosition();
				
				/// if bSkillEffect is true, link to right hand dummy
				if ( !pBullet->Init( pSour, PosTo, iEffectIDX, bLinkToPoint, 0, bDummy, iSkillIDX ) ) 
				{
					SAFE_DELETE( pBullet );
					return NULL;
				}				
			}
			break;
		}		

		case BULLET_MOVE_PARABOLA	:
		{			
			pBullet = new CBulletPARABOLA();
			
			/// if bSkillEffect is true, link to right hand dummy
			if ( !pBullet->Init( pSour, pDest, iEffectIDX, bLinkToPoint, iPointNO, bDummy, iSkillIDX ) ) 
			{
				SAFE_DELETE( pBullet );
				return NULL;
			}						
			break;
		}

		case BULLET_MOVE_IMMEDIATE  :
		{
		}

		case BULLET_MOVE_GUIDED	    :
		default :
			return NULL;
	}
	m_LIST.AllocNAppend( pBullet );

	return pBullet;
}

//-------------------------------------------------------------------------------------------------
void CBulletMANAGER::ProcBULLET ()
{
	classDLLNODE< CBullet* > *pNode, *pDelNode;

	DWORD dwTime = g_GameDATA.GetGameTime();
	m_dwPassTime = dwTime - m_dwCurTime;
	m_dwCurTime  = dwTime;

	pNode = m_LIST.GetHeadNode ();
	while( pNode ) {
		/// 뭔가 사라져야할 조건을 체크한다. 맞고난후 지속될 시간이라던지 ( dwHitTime )
		if ( !pNode->DATA->Proc () ) {
			pDelNode = pNode;
			pNode = m_LIST.GetNextNode ( pNode );

			SAFE_DELETE( pDelNode->DATA );
			m_LIST.DeleteNFree( pDelNode );
			continue;
		}

		pNode = m_LIST.GetNextNode ( pNode );
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
