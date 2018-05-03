/*
	$Header: /Client/CObjGROUND.cpp 57    05-02-28 11:10a Navy $
*/
#include "stdAFX.h"
#include "CObjGROUND.h"
#include "zz_interface.h"	
#include "Game.h"	
#include "common/IO_STB.h"
#include "CViewMSG.h"
#include "../interface/TypeResource.h"


HNODE CObjITEM::m_hMotion = NULL;

//-------------------------------------------------------------------------------------------------
CObjITEM::CObjITEM ()
{
	LogString (LOG_DEBUG, "CObjITEM::CObjITEM () \n");
	m_dwCreatedTIME		= g_GameDATA.GetGameTime();
	
	m_hDummyPosition	= NULL;
	m_hAnimatable		= NULL;
}
CObjITEM::~CObjITEM ()
{
	Delete();
	LogString (LOG_DEBUG, "CObjITEM::~CObjITEM () \n");
}

/// static member
bool CObjITEM::SetMotion( const char* strMotion )
{
	int ZZ_INTERP_SQUAD = 3, ZZ_INTERP_CATMULLROM = 4;
	m_hMotion = ::loadMotion( "ItemMotion", strMotion, 1, ZZ_INTERP_CATMULLROM, ZZ_INTERP_SQUAD, 1, 1 );

	if( m_hMotion == NULL )
		return false;
	
	return true;	
}

/// static member
void CObjITEM::ClearMotion()
{
	if( m_hMotion != NULL )
	{
		::unloadMotion( m_hMotion );
		m_hMotion = NULL; 
	}
}

bool CObjITEM::CreateAnimatable()
{
	if( CObjITEM::m_hMotion == NULL )
		return false;

	m_hDummyPosition = ::findNode( CStr::Printf ("ItemDummy_%d", this->Get_INDEX() ) );
	if( m_hDummyPosition == NULL )
		m_hDummyPosition = ::loadVisible( CStr::Printf ("ItemDummy_%d", this->Get_INDEX() ), 0, 0, 0 );

	if( m_hDummyPosition == NULL )
	{	
		return false;
	}

	m_hAnimatable = ::findNode( CStr::Printf ("ItemAnimatable_%d", this->Get_INDEX() ) );
	if( m_hAnimatable == NULL )
		m_hAnimatable = ::loadAnimatable( CStr::Printf ("ItemAnimatable_%d", this->Get_INDEX() ), NULL, CObjITEM::m_hMotion, NULL, g_GameDATA.m_hShader_nolit );
	if( m_hAnimatable == NULL )
	{		
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief m_hDummyPosition 이 최상의 노드임
//----------------------------------------------------------------------------------------------------

void CObjITEM::InsertToScene ( void )
{
	CreateAnimatable();	
	
	if( m_hDummyPosition && m_hAnimatable )
	{
		::setPosition( this->GetRootZNODE(), 0.0f, 0.0f, 0.0f );		

		::linkNode( m_hAnimatable, this->GetRootZNODE() );
		::linkNode( m_hDummyPosition, m_hAnimatable );		

		//----------------------------------------------------------------------------------------------------	
		/// @brief CHILD의InsertToScene제거
		//----------------------------------------------------------------------------------------------------
		//::insertToScene( m_hDummyPosition );		// CObjGROUND::RegisterToEZIN
		//::insertToScene( m_hAnimatable );		// CObjGROUND::RegisterToEZIN
		::controlAnimatable( m_hAnimatable, 1 );	
		::setRepeatCount( m_hAnimatable, 1 );

		::setPosition( m_hDummyPosition, m_PosCUR.x, m_PosCUR.y, m_PosCUR.z );
	}

	//----------------------------------------------------------------------------------------------------	
	/// @brief CHILD의InsertToScene제거
	//----------------------------------------------------------------------------------------------------
	::insertToScene( m_hDummyPosition );		// CObjGROUND::RegisterToEZIN
	//::insertToScene( m_hAnimatable );		// CObjGROUND::RegisterToEZIN
	///CObjFIXED::InsertToScene();
}

void CObjITEM::RemoveFromScene ( bool bIncludeEFFECT )
{
	//----------------------------------------------------------------------------------------------------	
	/// @brief CHILD의RemoveToScene제거
	//----------------------------------------------------------------------------------------------------
	if( m_hDummyPosition )
		::removeFromScene( m_hDummyPosition );

	/*
	if( m_hAnimatable )
		::removeFromScene( m_hAnimatable );
		*/

	///CObjFIXED::RemoveFromScene( bIncludeEFFECT );
}

void CObjITEM::Delete ()
{
	RemoveFromScene();

	if( m_hDummyPosition )	
		::unloadVisible( m_hDummyPosition );

	m_hDummyPosition = NULL;

	if( m_hAnimatable )	
		::unloadAnimatable( m_hAnimatable );

	m_hAnimatable = NULL;	
}

char* CObjITEM::Get_NAME()
{
	if( ITEM_TYPE_MONEY == m_ITEM.GetTYPE() )
		return CStr::Printf("%d%s",m_ITEM.m_uiQuantity,STR_MONEY );

	if( m_ITEM.GetTYPE() >= 1 && m_ITEM.GetTYPE() < ITEM_TYPE_MONEY )
		return (char*)ITEM_NAME( m_ITEM.GetTYPE(), m_ITEM.GetItemNO() );

	return CStr::Printf("%s","알수 없는 아이템");
}

int CObjITEM::Proc()
{		
	if ( ::inViewfrustum( m_hDummyPosition ) ) // 뷰프러스텀 안에 있으면 참, 없으면 거짓
	{ 
		g_pViewMSG->AddItemToInfoView( this );
	}
	
	return 0;
}








//-------------------------------------------------------------------------------------------------
CObjTREE::CObjTREE ()
{
	this->m_nTreeIdx = 0;
	LogString (LOG_DEBUG, "CObjTREE::CObjTREE () \n");
}
CObjTREE::~CObjTREE ()
{
	_ASSERT( this->m_nTreeIdx >= 0 );
//	LogString (LOG_DEBUG, "CObjTREE::~CObjTREE ( %d ) \n", this->m_nTreeIdx );
}




//-------------------------------------------------------------------------------------------------
CObjCNST::CObjCNST ()
{
	this->m_nCnstIdx = 0;
	LogString (LOG_DEBUG, "CObjCNST::CObjCNST () \n");
}
CObjCNST::~CObjCNST ()
{
	_ASSERT( this->m_nCnstIdx >= 0 );
//	LogString (LOG_DEBUG, "CObjCNST::~CObjCNST ( %d ) %f, %f \n", this->m_nCnstIdx, this->m_PosCUR.x, this->m_PosCUR.y);
}

//-------------------------------------------------------------------------------------------------
/*
int CObjCNST::Proc (void)
{
	if ( m_bIsVisible ) {
		for (short nP=0; nP<m_pMODEL->m_nPartCNT; nP++) {
			m_pMODEL->m_pParts[ nP ].ProcANI( m_pHNODES[nP] );
		}
	}

	return 1;
}
*/
//-------------------------------------------------------------------------------------------------
