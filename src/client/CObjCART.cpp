#include "stdafx.h"
#include ".\cobjcart.h"
#include "Common/IO_Pat.h"
#include "Interface/it_mgr.h"
#include "CObjCHAR_Collision.h"


CObjCART::CObjCART(void)
{
	m_pObjParent	= NULL;
	m_iCartType		= 0;

	m_iCurrentCartState	= CART_STATE_STOP;
	m_iOldCartState		= CART_STATE_STOP;

//----------------------------------------------------------------
//박지호
	m_pRideUser = NULL;

//----------------------------------------------------------------

}

CObjCART::~CObjCART(void)
{
	if( m_pObjParent != NULL )
	{
		D3DXVECTOR3 pos = Get_CurPOS();
		::unlinkNode( m_pObjParent->GetZMODEL() );
		m_pObjParent->ResetCUR_POS( pos );
		///::savePrevPosition( m_pObjParent->GetZMODEL() );

		CopyCollisionInformation( false );
		
		((CObjAVT*)m_pObjParent)->m_pObjCART = NULL;
		m_pObjParent = NULL;
	}

	this->StopSound( m_iOldCartState );

//-----------------------------------------------------------------
//박지호
	m_pRideUser = NULL;
//-----------------------------------------------------------------

}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 나에게 링크된 애들을 모두 분리
//----------------------------------------------------------------------------------------------------
void CObjCART::UnLinkChild(int iStart)
{

#if defined(_GBC)
//----------------------------------------------------------------
//박지호::카트에서 탑승한 유저를 링크해제 한다. (수정)
	D3DXVECTOR3 pos = Get_CurPOS();		

	CObjCHAR* pClass[] = { m_pObjParent, m_pRideUser };

	int si=0;
	
	if(iStart)
	{	si = 1; } 

	for(register int i=si;i<2;i++)
	{
		if(pClass[i] == NULL)
			continue;
		
		::unlinkVisibleWorld( pClass[i]->GetZMODEL() );
						
		if(i == 0)
		{
			pClass[i]->ReleaseRideUser();
			pClass[i]->ResetCUR_POS( pos );
			pClass[i]->SetResetPosZ( true, pos.z );
		}
		//뒷에 탑승한 유저는 조금 뒤에 위치를 탑승한다. 
		else
		{
			D3DXVECTOR3 ruDir;
			D3DXVECTOR3 ruPos = pos;

			::getModelDirectionVector(this->GetZMODEL(),&ruDir.x);
			
			ruPos+=((ruDir * -1.0f) * 200.0f);
			
			pClass[i]->ResetCUR_POS( ruPos );
			//pClass[i]->DropFromSky(ruPos.x,ruPos.y);
			pClass[i]->SetResetPosZ( true, pos.z + 200);
			
			m_pRideUser = NULL;
		}

		pClass[i]->Set_STATE(CS_STOP);
		pClass[i]->SetCMD_STOP();


		CopyCollisionInformation( false );

		///((CObjAVT*)m_pObjParent)->m_pObjCART = NULL;
		pClass[i] = NULL;

		this->StopSound( m_iCurrentCartState );
	}
#else	
	if( m_pObjParent != NULL )
	{
		D3DXVECTOR3 pos = Get_CurPOS();		
	
		::unlinkVisibleWorld( m_pObjParent->GetZMODEL() );
		m_pObjParent->ResetCUR_POS( pos );
		m_pObjParent->SetResetPosZ( true, pos.z );
		///::savePrevPosition( m_pObjParent->GetZMODEL() );
		CopyCollisionInformation( false );

		///((CObjAVT*)m_pObjParent)->m_pObjCART = NULL;
		m_pObjParent = NULL;

		this->StopSound( m_iCurrentCartState );
	}
#endif 

}


void CObjCART::SetPetParts( unsigned int iPartIDX, unsigned int iItemIDX )
{
	if( iPartIDX >= MAX_RIDING_PART )
		return;

	m_nPartItemIDX[ iPartIDX ] = iItemIDX;
}

bool CObjCART::Create ( CObjCHAR* pParent, int iCartType, D3DVECTOR &Position )
{
	if( pParent == NULL )
		return false;

	float fDir = ::getModelDirection( pParent->GetZMODEL() );

	/// CObjCHAR의 메쏘드 사용을 위해서( InserToScene 따위 )
	m_pCharMODEL = &m_CharMODEL;

	m_pObjParent = pParent;
	m_iCartType  = iCartType;


	char *szName  = CStr::Printf ( "%s_Cart", pParent->Get_NAME() );

	CMODEL<CCharPART> *pPartMODEL = NULL;
	for ( short nI = 0; nI < MAX_RIDING_PART; nI++ ) 
	{		
		pPartMODEL = g_DATA.m_pMD_CartPARTS->GetMODEL( m_nPartItemIDX[ nI ] );
		m_CharMODEL.SetPartMODEL( nI, pPartMODEL );	
	}	

	if ( CObjCHAR::CreateCHAR ( szName, &m_CharMODEL, MAX_RIDING_PART, Position) ) 
	{		
		this->m_iHP = pParent->Get_HP();		
		HNODE hChild = m_pObjParent->GetZMODEL();
		
		this->LinkDummy( hChild, 0 );					
		this->Set_ModelDIR( fDir, 1 );  

		/// CObjCHAR::CreateCHAR 내부에서 DropFromSky 함수를 호출하기 때문에 다시 리셋을 해줘야된다.
		/// 이것때문에 타고 내릴때 다리아래에서 타고 내리면 문제가 있었다. 아래에서 탔는데 다리 위로 올라가 버리는..
		this->ResetCUR_POS( D3DXVECTOR3( Position.x, Position.y, Position.z ) );

		::setUserData( this->GetZMODEL(), reinterpret_cast< HNODE >( this ) );
		///::setScale( this->GetZMODEL(), 2.0f, 2.0f, 2.0f );

		SetEffect();

		CopyCollisionInformation( true );


		return true;
	}	

	LogString (LOG_DEBUG, "Cart create failed .. %d: %s \n", szName );

	/// 카트 및 캐슬기어 크기 1.2 배 확대..
	::setScale( this->GetZMODEL(), 1.2f, 1.2f, 1.2f );

	return false;
}



//----------------------------------------------------------------------------------------------------
/// @박지호
/// @박지호::선택된 유저를 카트에 태운다.
//----------------------------------------------------------------------------------------------------
bool CObjCART::Create( CObjCHAR* pTarget)
{
	
	if(m_pObjParent == NULL)
		return false;

	m_pRideUser = pTarget;

	//위치 초기화 
	pTarget->Set_CurPOS(D3DXVECTOR3(0,0,0));
	::setPosition(pTarget->GetZMODEL(),0,0,0);

	HNODE hChild = pTarget->GetZMODEL();
		
	this->LinkDummy(hChild,10);					
	
	//회전 및 모션 설정 
	m_pRideUser->Set_ModelDIR(0.0f,1);  
	m_pRideUser->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_STOP1 );
	
	return true;

}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Cart 의 경우는 무기에 따른 모션이 아니라 ARMS 테이블의 모션 타입에 의존한다.
//----------------------------------------------------------------------------------------------------
tagMOTION *CObjCART::Get_MOTION (short nActionIdx)
{	

//박지호::
#if defined(_GBC)
	short nFileIDX = FILE_MOTION( PAT_RELATIVE_MOTION_POS( this->m_nWeaponIDX ), nActionIdx );
#else
	short nFileIDX = FILE_MOTION( PAT_RELATIVE_MOTION_POS( this->m_nAbilIDX ), nActionIdx );
#endif 

	if ( 0 == nFileIDX ) 
	{
		assert( 0 && "PET Motion is Invalid" );
		g_itMGR.OpenMsgBox( "INVALID PET MOTION" );
		
#if defined(_GBC)
		nFileIDX = FILE_MOTION( PAT_RELATIVE_MOTION_POS( this->m_nWeaponIDX ), 0 );
#else 
		nFileIDX = FILE_MOTION( PAT_RELATIVE_MOTION_POS( this->m_nAbilIDX ), 0 );
#endif 


	}

	tagMOTION *pMOTION = g_MotionFILE.IDX_GetMOTION( nFileIDX, false );
	if ( pMOTION ) 
	{
		pMOTION->m_nActionIdx = nActionIdx;
	}

	return pMOTION;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 카트에 이펙트 설정
///		   일반 부품의 효과는 항상 나오고, 랙의 부품효과는 이동시에만 나온다.
//----------------------------------------------------------------------------------------------------

void CObjCART::SetEffect()
{
	for ( short nI = 0; nI < MAX_RIDING_PART; nI++ ) 
	{
		if( nI != RIDE_PART_LEG )
			SetPartEffect( nI );		
	}
	//SetPartEffect( RIDE_PART_ENGINE );
}

#define MAX_CART_EFFECT_DUMMIES		8
void CObjCART::SetPartEffect( int iPart )
{
	int iEffectNO = 0;
	for( int i = 0; i < MAX_CART_EFFECT_DUMMIES; i++ )
	{
		iEffectNO = PAT_ITEM_ATTACHED_EFFECT( m_nPartItemIDX[ iPart ], i );

		/// 설정할 이펙트가 있을경우..
		if( iEffectNO )
		{
			CEffect *pEffect = g_pEffectLIST->Add_EffectWithIDX( iEffectNO, true );
			if ( pEffect ) 
			{
				this->LinkDummy( pEffect->GetZNODE(), i );
				this->AddExternalEffect( pEffect );
				pEffect->SetParentCHAR( this );

				pEffect->InsertToScene();
			}		
		}
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 이동중에만 나오는 이펙트. 혹은 그 반대.
//----------------------------------------------------------------------------------------------------
void CObjCART::SetEffectByMoveState( bool bShow )
{
}

///
/// 카트에 사운드 설정
///
void CObjCART::PlaySound( int iCurrentState )
{
	for ( short nI = 0; nI < MAX_RIDING_PART; nI++ ) 
	{			
		PlayPartSound( nI, iCurrentState );		
	}
}

void CObjCART::PlayPartSound( int iPart, int iCurrentState )
{
	switch( iCurrentState )
	{
		case CART_STATE_STOP:
			{
				int iSoundIDX = PAT_STOP_SOUND( m_nPartItemIDX[ iPart ] );
				if( iSoundIDX )
				{
					D3DXVECTOR3 vecWorld = this->Get_CurPOS();
					g_pSoundLIST->IDX_PlaySound3DLoop( iSoundIDX, vecWorld, NULL ); 
				}
			}
			break;

		case CART_STATE_MOVE:
			{
				int iSoundIDX = PAT_MOVE_SOUND( m_nPartItemIDX[ iPart ] );
				if( iSoundIDX )
				{
					D3DXVECTOR3 TargetPos;
					TargetPos.x = this->m_PosGOTO.x;
					TargetPos.y = this->m_PosGOTO.y;
					TargetPos.z = 100.0f;

					D3DXVECTOR3 Velocity = TargetPos - Get_CurPOS(); // 1초 동안에 이동하는 벡터

					D3DXVECTOR3 vecWorld = this->Get_CurPOS();
					g_pSoundLIST->IDX_PlaySound3DLoop( iSoundIDX, vecWorld, &Velocity ); 
				}
			}
			break;	
	}
}

void CObjCART::StopSound( int iCurrentState )
{
	for ( short nI = 0; nI < MAX_RIDING_PART; nI++ ) 
	{			
		StopPartSound( nI, iCurrentState );		
	}
}

void CObjCART::StopPartSound( int iPart, int iCurrentState )
{
	switch( iCurrentState )
	{
		case CART_STATE_STOP:
			{
				int iSoundIDX = PAT_STOP_SOUND( m_nPartItemIDX[ iPart ] );
				if( iSoundIDX )
				{					
					g_pSoundLIST->IDX_StopSound3DLoop( iSoundIDX ); 
				}
			}
			break;

		case CART_STATE_MOVE:
			{				
				int iSoundIDX = PAT_MOVE_SOUND( m_nPartItemIDX[ iPart ] );
				if( iSoundIDX )
				{					
					g_pSoundLIST->IDX_StopSound3DLoop( iSoundIDX ); 
				}
			}
			break;
		case CART_STATE_ATTACK:
			{
				int iSoundIDX = PAT_STOP_SOUND( m_nPartItemIDX[ iPart ] );
				if( iSoundIDX )
				{					
					g_pSoundLIST->IDX_StopSound3DLoop( iSoundIDX ); 
				}
			}
			break;
	}
}


int	CObjCART::GetRideAniPos()	{ return PAT_RELATIVE_MOTION_POS_AVATAR( m_nPartItemIDX[ RIDE_PART_BODY ] ); }


int	CObjCART::GetANI_Stop()		{	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  CART_ANI_STOP1;	}
int	CObjCART::GetANI_Move()		
{	
	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  CART_ANI_MOVE;	
}
int	CObjCART::GetANI_Attack()	{	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  CART_ANI_ATTACK01; /*+ RANDOM(3);*/	}
int	CObjCART::GetANI_Die ()		{	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  CART_ANI_DIE;	}
int	CObjCART::GetANI_Hit()		{	return PAT_RELATIVE_MOTION_POS( m_nPartItemIDX[ RIDE_PART_BODY ] ) +  CART_ANI_STOP1;	}	

//박지호::스킬 애니메이션 
int	CObjCART::GetANI_Casting()	{	return SKILL_ANI_CASTING(m_nActiveSkillIDX);							}	
int	CObjCART::GetANI_CastingRepeat()	{	return  SKILL_ANI_CASTING_REPEAT(m_nActiveSkillIDX);			}	
int	CObjCART::GetANI_Skill()	{	return 	SKILL_ANI_ACTION_TYPE(m_nActiveSkillIDX);						}	



//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

bool CObjCART::SetCMD_MOVE( const D3DVECTOR& PosTO, BYTE btRunMODE )
{
	CObjAI::SetCMD_MOVE( PosTO, btRunMODE );

	//----------------------------------------------------------------------------------------------------
	/// 펫 탑승을 한 캐릭터의 모션을 교체한다.
	//----------------------------------------------------------------------------------------------------		
	m_pObjParent->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_MOVE );
	
	//----------------------------------------------------------------------------------------------------
#if defined(_GBC)
	//박지호::
	if(m_pRideUser)
		m_pRideUser->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_MOVE );
#endif
	//----------------------------------------------------------------------------------------------------


	m_iOldCartState = m_iCurrentCartState;
	m_iCurrentCartState = CART_STATE_MOVE;

	StopSound( m_iOldCartState );
	PlaySound( m_iCurrentCartState );
	return true;
}

void CObjCART::SetCMD_MOVE( WORD wSrvDIST, const D3DVECTOR& PosTO, int iServerTarget)
{
	CObjAI::SetCMD_MOVE( wSrvDIST, PosTO, iServerTarget );

	//----------------------------------------------------------------------------------------------------
	/// 펫 탑승을 한 캐릭터의 모션을 교체한다.
	//----------------------------------------------------------------------------------------------------		
	m_pObjParent->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_MOVE );
	
	//----------------------------------------------------------------------------------------------------
	//박지호::
#if defined(_GBC)
	if(m_pRideUser)
		m_pRideUser->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_MOVE );
#endif 
	//----------------------------------------------------------------------------------------------------
		


	m_iOldCartState = m_iCurrentCartState;
	m_iCurrentCartState = CART_STATE_MOVE;

	StopSound( m_iOldCartState );
	PlaySound( m_iCurrentCartState );
}

bool CObjCART::SetCMD_STOP( void )
{
	CObjAI::SetCMD_STOP();

	//----------------------------------------------------------------------------------------------------
	/// 펫 탑승을 한 캐릭터의 모션을 교체한다.
	//----------------------------------------------------------------------------------------------------		
	m_pObjParent->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_STOP1 );
	
	//----------------------------------------------------------------------------------------------------
#if defined(_GBC)
	//박지호::
	if(m_pRideUser)
		m_pRideUser->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_STOP1 );
#endif 
	//----------------------------------------------------------------------------------------------------


	m_iOldCartState = m_iCurrentCartState;
	m_iCurrentCartState = CART_STATE_STOP;

	StopSound( m_iOldCartState );
	PlaySound( m_iCurrentCartState );	
	return true;
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief /*override*/virtual bool	SetCMD_ATTACK ( int iServerTarget ); 는 정의할 필요없다.
///			CObjAI::SetCMD_ATTACK 가 내부적으로 호출하는 함수이다.
//----------------------------------------------------------------------------------------------------

void CObjCART::SetCMD_ATTACK (int iServerTarget, WORD wSrvDIST, const D3DVECTOR& PosGOTO)
{
	CObjCHAR::SetCMD_ATTACK( iServerTarget, wSrvDIST, PosGOTO );

	//----------------------------------------------------------------------------------------------------
	/// 펫 탑승을 한 캐릭터의 모션을 교체한다.
	//----------------------------------------------------------------------------------------------------	
	
	//----------------------------------------------------------------------------------------------------
#if defined(_GBC)
	m_pObjParent->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_STOP1 );
	//박지호::
	if(m_pRideUser)
		m_pRideUser->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_STOP1 );
#else 
	m_pObjParent->Set_MOTION( this->GetRideAniPos() + PETMODE_AVATAR_ANI_ATTACK01 + RANDOM(3) );
#endif 
	//----------------------------------------------------------------------------------------------------


	m_iOldCartState = m_iCurrentCartState;
	m_iCurrentCartState = CART_STATE_ATTACK;

	StopSound( m_iOldCartState );
	PlaySound( m_iCurrentCartState );
}




//----------------------------------------------------------------------------------------------------	
/// @brief 카트계열은 LIST_PAT 에서 공격관련 데이터는 얻어온다.
///        Move Speed 등은 펫을 탓다는 가정하에 계산이 부모에게 계산이 된다. 결국 부모의 스피드를 이용하면된다.
///			이는 부모의 SpeedUpdate 를 할때 펫 탑승여부가 고려되어서 계산되기 때문이다.
///			그러나 Attack Speed 따위는 틀리다. 이는 테이블에서 얻어와야한다.( 아바타는 무기에 의존하고 기타등등..
//----------------------------------------------------------------------------------------------------

float CObjCART::Get_MoveSPEED ()
{	
	return m_fAdjustSPEED;		
}

float CObjCART::Get_DefaultSPEED ()		
{	
	return ( ( m_pObjParent )? m_pObjParent->GetOri_RunSPEED(): 10 );
}

short CObjCART::Get_nAttackSPEED ()	
{ 
	return ( m_pObjParent )? m_pObjParent->GetOri_ATKSPEED() : 10; 
}

float CObjCART::Get_fAttackSPEED ()	
{ 
	return (float)Get_nAttackSPEED() / 100.0f; 
}
int	 CObjCART::Get_AttackRange ()	
{

#if defined(_GBC)
	return (int) ( PAT_ITEM_ATK_RANGE( m_nWeaponIDX ) + ( Get_SCALE() * 120 ) );	
#else
	return (int) ( PAT_ITEM_ATK_RANGE( m_nAbilIDX ) + ( Get_SCALE() * 120 ) );	
#endif 

}


/// 충돌에 필요한 위치정보를 복사한다.
void CObjCART::CopyCollisionInformation( bool bRiding )
{
	/// Parent 는 개념상의 부모 실제로는 아바타고 카트에 링크 되어 있다.
	/// 여기서 Parent 는 아바타
	CObjCHAR_Collision* pCollision = this->GetCollisionUtility();
	CObjCHAR_Collision* pParentCollision = m_pObjParent->GetCollisionUtility();
 
	/// 탈때는 아바타의 정보를 카트에 복사  
	if( bRiding )
	{
		if( m_pObjParent )
		{
			if( pCollision && pParentCollision )
			{
				pCollision->m_vPrevious = pParentCollision->m_vPrevious;
				pCollision->m_vCurrent	= pParentCollision->m_vCurrent;
				pCollision->m_vNext		= pParentCollision->m_vNext;
			}
		}
	}else
	{
		/// 내릴때는 카트의 정보를 아바타에 복사
		if( m_pObjParent )
		{
			if( pCollision && pParentCollision )
			{
				pParentCollision->m_vPrevious	= pCollision->m_vPrevious;
				pParentCollision->m_vPrevious.z	= pCollision->m_vPrevious.z + 500.0f;
				pParentCollision->m_vCurrent	= pCollision->m_vCurrent;				
				pParentCollision->m_vNext		= pCollision->m_vNext;
				
			}
		}
	}
}


///박지호::카트의 각 파트를 불투명도를 설정한다. 
void CObjCART::SetCartPartVisible(float fv)
{

	CMODEL<CCharPART> *pCharPART = NULL;
	short iPart = 0;

	for(register int i=0;i<MAX_RIDING_PART;i++)
	{
		iPart = m_nPartItemIDX[i];

		if(!iPart)
			continue;

		pCharPART = m_pCharMODEL->GetCharPART(iPart);
		
		if(pCharPART == NULL)
			continue;
		 
		for(register int j=0;j<pCharPART->m_nPartCNT;j++)
		{ ::setVisibilityRecursive(m_phPartVIS[iPart][j],fv); }
		
	}

}




