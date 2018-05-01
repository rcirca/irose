#include "stdafx.h"
#include ".\cjustmodelavt.h"
#include "Common/IO_STB.h"
#include "TCommand.h"
#include "CAvatarState.h"
CJustModelAVT::CJustModelAVT(void)
{
	m_btRace				= 0;
	m_bIsFemale				= 0;

	m_hNodeMODEL			= NULL;

	for( int i = 0; i < MAX_BODY_PART; i++ )
	{
		m_phPartVIS[ i ]	= NULL;
		m_nPartItemIDX[ i ] = 0;
	}

	m_strName = std::string( "Untitled" );

	m_bIsVisible = true;
	m_dwDeleteRemainSEC = 0;

	m_pState[STATE_NORMAL] = new CAvatarStateNormal;
	m_pState[STATE_SITDOWN] = new CAvatarStateSitdown;
	m_pState[STATE_SIT]		= new CAvatarStateSit;
	m_pState[STATE_STANDUP] = new CAvatarStateStandup;
	
	m_pCurrState = m_pState[STATE_NORMAL];
}

CJustModelAVT::~CJustModelAVT(void)
{

}

void CJustModelAVT::SwitchState( int iID )
{
	assert( iID >= 0 && iID < STATE_MAX );
	if(iID >= 0 && iID < STATE_MAX)
	{
		m_pCurrState->Leave( this );
		m_pCurrState = m_pState[iID];
		m_pCurrState->Enter( this );
	}
}
//-------------------------------------------------------------------------------------------------
bool CJustModelAVT::LoadModelNODE ( const char *szName)
{
	HNODE hSkel  = g_DATA.Get_SKELETON( m_bIsFemale );
	if ( hSkel == NULL ) 
	{
		LogString (LOG_NORMAL, "failed creat char.. SKEL == NULL !! \n");
		return false;
	}

	//if ( !this->Set_CurMOTION( this->Get_MOTION(0) ) ) {		// default stop motion ..
	//	LogString (LOG_NORMAL, "failed creat char.. MOTION == NULL !! \n");
	//	return false;
	//}
	
	m_hNodeMODEL = ::loadModel ( szName, hSkel, 0, 1.0f);	
	if ( m_hNodeMODEL ) 
	{
		::setCollisionLevel( m_hNodeMODEL, 4 );		
		return true;
	}

	return false;
}

void CJustModelAVT::UnloadModelNODE ()
{	
	if ( m_hNodeMODEL ) 
	{
		::unloadModel (m_hNodeMODEL);
		m_hNodeMODEL = NULL;
	}	
}

void CJustModelAVT::Init(	const char* strName,
							int iBODY_PART_FACE,	// 0 link to bone
							int iBODY_PART_HAIR,
							int iBODY_PART_HELMET,
							int iBODY_PART_ARMOR,	// 3 skinning
							int iBODY_PART_GAUNTLET,
							int iBODY_PART_BOOTS,
							int iBODY_PART_FACE_ITEM,// 6 link to dummy
							int iBODY_PART_KNAPSACK,
							// 오른손, 왼손 순서 주의 !!!
							int iBODY_PART_WEAPON_R,		// 8 link to dummy	
							int iBODY_PART_WEAPON_L	)
{
	m_strName = std::string( ( strName == NULL )? "NULL":strName );

	m_nPartItemIDX[ BODY_PART_FACE ]		= iBODY_PART_FACE;
	m_nPartItemIDX[ BODY_PART_HELMET ]		= iBODY_PART_HELMET;

	if( iBODY_PART_HELMET )///모자에 어울리는 머리모양을 선택한다.
		m_nPartItemIDX[ BODY_PART_HAIR ]		= iBODY_PART_HAIR + HELMET_HAIR_TYPE( iBODY_PART_HELMET );
	else
		m_nPartItemIDX[ BODY_PART_HAIR ]		= iBODY_PART_HAIR;
	
	m_nPartItemIDX[ BODY_PART_ARMOR ]		= iBODY_PART_ARMOR;
	m_nPartItemIDX[ BODY_PART_GAUNTLET ]	= iBODY_PART_GAUNTLET;
	m_nPartItemIDX[ BODY_PART_BOOTS ]		= iBODY_PART_BOOTS;
	m_nPartItemIDX[ BODY_PART_FACE_ITEM ]	= iBODY_PART_FACE_ITEM;
	m_nPartItemIDX[ BODY_PART_KNAPSACK ]	= iBODY_PART_KNAPSACK;
	m_nPartItemIDX[ BODY_PART_WEAPON_R ]	= iBODY_PART_WEAPON_R;
	m_nPartItemIDX[ BODY_PART_WEAPON_L ]	= iBODY_PART_WEAPON_L;
}

void CJustModelAVT::Init( int iPartsArray[ MAX_BODY_PART ] )
{
	for( int i = 0; i < MAX_BODY_PART; i++ )
	{
		m_nPartItemIDX[ i ] = iPartsArray[ i ];
	}
}

/// set race and sex
void CJustModelAVT::SetRace( BYTE btRace )
{
	m_btRace		= btRace;
	m_bIsFemale		= btRace & 0x01;	
	m_CharMODEL.Set_SEX( m_bIsFemale );
}

bool CJustModelAVT::SetPart( int iPartType, int iPartIDX )
{
	if ( iPartType < 0 || iPartType >= MAX_BODY_PART )
		return false;


	//----------------------------------------------------------------------------------------------------	
	/// @brief CHILD의removeFromScene제거
	//----------------------------------------------------------------------------------------------------
	//short nI = 0;
	//CMODEL<CCharPART> *pCharPART = m_CharMODEL.GetCharPART( iPartType );
	//if ( pCharPART  ) 
	//{
	//	for ( nI=0 ; nI<pCharPART->m_nPartCNT ; nI++ ) 
	//	{
	//		if ( m_phPartVIS[ iPartType ][ nI ] ) 
	//			::removeFromScene( m_phPartVIS[ iPartType ][ nI ] );	// CObjCHAR::RemoveFromScene
	//	}
	//}

	m_CharMODEL.DeletePART( iPartType, m_phPartVIS[ iPartType ] );
	m_nPartItemIDX[ iPartType ] = iPartIDX;	

	m_CharMODEL.SetPartMODEL( iPartType, iPartIDX );
	m_phPartVIS[ iPartType ] = m_CharMODEL.CreatePART( (char*)( m_strName.c_str() ), m_hNodeMODEL, iPartType );
	
	//----------------------------------------------------------------------------------------------------	
	/// @brief CHILD의InsertToScene제거
	//----------------------------------------------------------------------------------------------------
	//if ( pCharPART  ) 
	//{
	//	for ( nI = 0 ; nI < pCharPART->m_nPartCNT ; nI++ ) 
	//	{
	//		if ( m_phPartVIS[ iPartType ][ nI ] ) 
	//			::insertToScene( m_phPartVIS[ iPartType ][ nI ] );		// CObjCHAR::InsertToScene
	//	}
	//}


	return true;
}

bool CJustModelAVT::LoadModelVisible()
{
	LoadModelNODE( m_strName.c_str() );

	// npc 무기땜에 MAX_BODY_PART까지 ...

	for (short nP=0; nP<MAX_BODY_PART; nP++) 
	{
		m_phPartVIS[ nP ] = m_CharMODEL.CreatePART( (char*)( m_strName.c_str() ), m_hNodeMODEL, nP );
	}	

	SetMotion( 1 );

	return true;
}

void CJustModelAVT::UnloadModelVisible()
{
	this->RemoveFromScene();

	if ( this->m_hNodeMODEL ) 
	{
		// addRenderUnit된것들 삭제.
		::clearRenderUnit( this->m_hNodeMODEL );
	}

	// loadVisible된것들 삭제.
	for ( short nP=0 ; nP<MAX_BODY_PART ; nP++ ) 
	{
		m_CharMODEL.DeletePART( nP, m_phPartVIS[ nP ] );
		m_phPartVIS[ nP ] = NULL;
	}

	UnloadModelNODE();
}



/// update model
bool CJustModelAVT::UpdateModel()
{	
	for ( short nItemIDX, nI=0; nI < MAX_BODY_PART; nI++ ) 
	{
		nItemIDX = m_nPartItemIDX[ nI ];		
		
		this->m_CharMODEL.SetPartMODEL( nI, nItemIDX );		
	}


	return true;
}


/// 
/// scene...
///

void CJustModelAVT::InsertToScene()
{
	if ( !m_bIsVisible || !m_hNodeMODEL )
		return;

	m_bIsVisible = true;

	::insertToScene( m_hNodeMODEL );								// CObjCHAR::InsertToScene

	//----------------------------------------------------------------------------------------------------	
	/// @brief CHILD의InsertToScene제거
	//----------------------------------------------------------------------------------------------------

	//for ( short nP=0 ; nP < MAX_BODY_PART ; nP++ ) 
	//{
	//	CMODEL<CCharPART> *pCharPART = m_CharMODEL.GetCharPART( nP );

	//	if ( pCharPART  ) 
	//	{
	//		short nI;			

	//		for ( nI=0 ; nI < pCharPART->m_nPartCNT ; nI++ ) 
	//		{
	//			if ( m_phPartVIS[ nP ][ nI ] ) 
	//			{
	//				::insertToScene( m_phPartVIS[ nP ][ nI ] );		// CObjCHAR::InsertToScene
	//			}
	//		}
	//	}
	//}

}

void CJustModelAVT::RemoveFromScene()
{
	if ( !m_bIsVisible )
		return;

	m_bIsVisible = false;
	::removeFromScene ( m_hNodeMODEL );								// CObjCHAR::RemoveFromScene

	//----------------------------------------------------------------------------------------------------	
	/// @brief CHILD의removeFromScene제거
	//----------------------------------------------------------------------------------------------------
	//for ( short nP=0 ; nP<MAX_BODY_PART ; nP++ ) 
	//{
	//	CMODEL<CCharPART> *pCharPART = m_CharMODEL.GetCharPART( nP );

	//	if ( pCharPART  ) 
	//	{
	//		short nI;
	//		
	//		for ( nI=0 ; nI<pCharPART->m_nPartCNT ; nI++ ) 
	//		{
	//			if ( m_phPartVIS[ nP ][ nI ] ) 
	//			{
	//				::removeFromScene( m_phPartVIS[ nP ][ nI ] );	// CObjCHAR::RemoveFromScene
	//			}
	//		}
	//	}
	//}
}

void CJustModelAVT::SetPosition( float x, float y, float z )
{
	::setPosition( m_hNodeMODEL, x, y, z );	
}

bool CJustModelAVT::SetMotion( int iMotionIDX ,int iRepeatCount )
{
	tagMOTION *pMOTION = g_MotionFILE.IDX_GetMOTION( iMotionIDX, m_bIsFemale );
	if ( !pMOTION ) 
	{
		return false;	
	}

	::attachMotion		( m_hNodeMODEL, pMOTION->m_hMotion );
	if( iRepeatCount )
		setRepeatCount ( m_hNodeMODEL, iRepeatCount );

	return true;
}

bool CJustModelAVT::SetMotion( const char* strMotion, int iRepeatCount )
{
	if( strMotion == NULL )
		return false;

	unsigned int uiKEY = CStr::GetHASH( strMotion );

	HNODE hMotion = g_MotionFILE.KEY_GetZMOTION( uiKEY );
	if ( !hMotion ) 
	{
		return false;
	}

	::attachMotion		( m_hNodeMODEL, hMotion );
	if( iRepeatCount )
		setRepeatCount ( m_hNodeMODEL, iRepeatCount );

	return true;
}

void CJustModelAVT::Update()
{
	m_pCurrState->Update( this );
}


bool CJustModelAVT::IsPlatinum()
{
	return m_bPlatinum;
}

void CJustModelAVT::SetPlatinum(bool platinum)
{
	m_bPlatinum = platinum;
}

void CJustModelAVT::SetIndex( int index )
{
	m_iIndex = index;
}
int	CJustModelAVT::GetIndex()
{
	return m_iIndex;
}
