#include "stdafx.h"
#include ".\damageeffect.h"

CDamageEffect::CDamageEffect(void)
{
	int i = 0;
	for( i = 0; i < MAX_DIGIT_EFFECT ; i++ )
	{
		m_DigitNode[ i ].m_hPositionDummy	= NULL;
		m_DigitNode[ i ].m_hAnimatable[ 0 ] = NULL;
		m_DigitNode[ i ].m_hAnimatable[ 1 ] = NULL;
		m_DigitNode[ i ].m_hAnimatable[ 2 ] = NULL;
		m_DigitNode[ i ].m_hAnimatable[ 3 ] = NULL;
		m_DigitNode[ i ].m_bVisible = false;
	}

	m_hMotion[ 0 ]	= NULL;
	m_hMotion[ 1 ]	= NULL;
	m_hMotion[ 2 ]	= NULL;
	m_hMotion[ 3 ]	= NULL;

	m_hMat			= NULL;
	m_hMesh			= NULL;
}

CDamageEffect::~CDamageEffect(void)
{
}

bool CDamageEffect::Init()
{
	//ClearAll();

	//int i = 0;

	//HNODE hDummy = NULL;	
	//HNODE hAnimatable = NULL;	
	//HNODE hMesh = NULL;
	//HNODE hMat = NULL;

	//int ZZ_INTERP_SQUAD = 3, ZZ_INTERP_CATMULLROM = 4;
	//m_hMotion = ::loadMotion( "DigitEffectMotion", "3DData\\Effect\\Special\\hit_figure_01.ZMO", 1, ZZ_INTERP_CATMULLROM, ZZ_INTERP_SQUAD, 1, 1 );
	//if( m_hMotion == NULL )
	//{
	//	ClearAll();
	//	return false;
	//}

	//for( i = 0; i < MAX_DIGIT_EFFECT ; i++ )
	//{		
	//	hMat = ::loadNullColormapMaterial( CStr::Printf ("DigitEffect_%d", i ), g_GameDATA.m_hShader_nolit, DIGIT_WIDTH * DIGIT_COUNT, DIGIT_HEIGHT);
	//	//hMat = ::loadColormapMaterial( CStr::Printf ("DigitEffect_%d", i ), g_GameDATA.m_hShader_nolit, "3DData\\Effect\\Special\\DigitNumber01.tga" );
	//	if( hMat == NULL )
	//	{
	//		ClearAll();
	//		return false;
	//	}

	//	hMesh = ::loadMesh ( CStr::Printf ("DigitMesh_%d", i ), "3DData\\Effect\\Special\\DigitWork01.zms" );
	//	if( hMesh == NULL )
	//	{
	//		ClearAll();
	//		return false;
	//	}		
	//	
	//	hAnimatable = ::loadAnimatable( CStr::Printf ("DigitNode_%d", i ), hMesh, m_hMotion, hMat, g_GameDATA.m_hShader_nolit );
	//	if( hAnimatable == NULL )
	//	{
	//		ClearAll();
	//		return false;
	//	}

	//	hDummy = ::loadVisible( CStr::Printf ("DigitDummy_%d", i ), 0, 0, 0 );
	//	if( hDummy == NULL )
	//	{
	//		ClearAll();
	//		return false;
	//	}

	//	m_DigitNode[ i ].m_hPositionDummy = hDummy;		
	//	m_DigitNode[ i ].m_hAnimatable = hAnimatable;		
	//	m_DigitNode[ i ].m_hMesh = hMesh;
	//	m_DigitNode[ i ].m_hMat = hMat;

	//	/// Material attribute setting

	//	::setMaterialBlendTypeCustom( hMat, D3DBLEND_SRCALPHA,
	//										D3DBLEND_INVSRCALPHA,
	//										D3DBLENDOP_ADD
	//										);


	//	::setMaterialUseAlpha( hMat, 1 );
	//	::setMaterialUseLight( hMat, 0 ); // 라이팅 적용 안함.
	//	
	//	//::setMaterialUseAlphaTest( hMat, 1 );
	//	//::setMaterialZWrite( hMat, 1 );
	//	//::setMaterialZTest( hMat, 1 );


	//	/// 
	//	::setBillboard( m_DigitNode[ i ].m_hAnimatable, 1 );
	//	::controlAnimatable( m_DigitNode[ i ].m_hAnimatable, 0 );

	//	::setRepeatCount( m_DigitNode[ i ].m_hAnimatable, 1 );

	//	::linkNode( hDummy, hAnimatable ); 

	//	
	//	//::insertToScene( hDummy );
	//	//----------------------------------------------------------------------------------------------------	
	//	/// @brief CHILD의InsertToScene제거
	//	//----------------------------------------------------------------------------------------------------
	//	//::insertToScene( hAnimatable );
	//}

	//	
	///// Load digit number
	//m_DigitTex[ DIGIT_NORMAL ] = loadTextureWithPool ( "3DData\\Effect\\Special\\DigitNumber01.dds", 
	//													"3DData\\Effect\\Special\\DigitNumber01.dds", 
	//													1, 
	//													0,
	//													2 );
	//if( m_DigitTex[ DIGIT_NORMAL ] == NULL )
	//{
	//		g_pCApp->ErrorBOX( "Chatbox load failed !!", "File open error" );
	//		//실패한 이유를 적어준다..
	//		ClearAll();
	//		return false;
	//}

	///// Load digit number
	//m_DigitTex[ DIGIT_ATTACKED ] = loadTextureWithPool ( "3DData\\Effect\\Special\\DigitNumber02.dds", 
	//													"3DData\\Effect\\Special\\DigitNumber02.dds", 
	//													1, 
	//													0,
	//													2 );
	//if( m_DigitTex[ DIGIT_ATTACKED ] == NULL )
	//{
	//	g_pCApp->ErrorBOX( "Chatbox load failed !!", "File open error" );
	//	//실패한 이유를 적어준다..
	//	ClearAll();
	//	return false;
	//}




	///// Load digit clear number
	//m_DigitClearTex = loadTextureWithPool ( "3DData\\Effect\\Special\\DigitWorkClear.dds", 
	//									"3DData\\Effect\\Special\\DigitWorkClear.dds", 
	//									1, 
	//									0,
	//									2 );
	//if( m_DigitClearTex == NULL )
	//{
	//		g_pCApp->ErrorBOX( "m_DigitClearTex load failed !!", "File open error" );
	//		//실패한 이유를 적어준다..
	//		ClearAll();
	//		return false;
	//}

	//
	///// Load digit Miss number
	//m_DigitMissTex = loadTextureWithPool ( "3DData\\Effect\\Special\\DigitNumberMiss.dds", 
	//										"3DData\\Effect\\Special\\DigitNumberMiss.dds", 
	//										1, 
	//										0,
	//										2 );
	//if( m_DigitMissTex == NULL )
	//{
	//	g_pCApp->ErrorBOX( "m_DigitMissTex load failed !!", "File open error" );
	//	//실패한 이유를 적어준다..
	//	ClearAll();
	//	return false;
	//}

	return true;	
}

void CDamageEffect::ClearAll()
{
	//HNODE hVisible = NULL;
	//HNODE hMesh = NULL;
	//HNODE hMat = NULL;

	//for( int i = 0; i < MAX_DIGIT_EFFECT ; i++ )
	//{		
	//	if( m_DigitNode[ i ].m_hPositionDummy != NULL )
	//	{
	//		::removeFromScene( m_DigitNode[ i ].m_hPositionDummy );
	//		::unloadVisible( m_DigitNode[ i ].m_hPositionDummy );
	//		m_DigitNode[ i ].m_hPositionDummy = NULL;
	//	}
	//	
	//	if( m_DigitNode[ i ].m_hAnimatable != NULL )
	//	{
	//		//----------------------------------------------------------------------------------------------------	
	//		/// @brief CHILD의RemoveToScene제거
	//		//----------------------------------------------------------------------------------------------------
	//		//::removeFromScene( m_DigitNode[ i ].m_hAnimatable );
	//		::unloadAnimatable( m_DigitNode[ i ].m_hAnimatable );
	//		m_DigitNode[ i ].m_hAnimatable = NULL;
	//	}

	//	if( m_DigitNode[ i ].m_hMesh != NULL )
	//	{
	//		::unloadMesh( m_DigitNode[ i ].m_hMesh );
	//		m_DigitNode[ i ].m_hMesh = NULL;
	//	}

	//	if( m_DigitNode[ i ].m_hMat != NULL )
	//	{
	//		::unloadMaterial( m_DigitNode[ i ].m_hMat );		
	//		m_DigitNode[ i ].m_hMat = NULL;
	//	}

	//}

	//if( m_hMotion != NULL )
	//{
	//	::unloadMotion( m_hMotion );
	//	m_hMotion = NULL;
	//}
	//
	//
	//if( m_DigitTex[ DIGIT_NORMAL ] != NULL )
	//{
	//	::unloadTexture( m_DigitTex[ DIGIT_NORMAL ] );
	//	m_DigitTex[ DIGIT_NORMAL ] = NULL;
	//}
	//if( m_DigitTex[ DIGIT_ATTACKED ] != NULL )
	//{
	//	::unloadTexture( m_DigitTex[ DIGIT_ATTACKED ] );
	//	m_DigitTex[ DIGIT_ATTACKED ] = NULL;
	//}

	//if( m_DigitClearTex != NULL )
	//{
	//	::unloadTexture( m_DigitClearTex );
	//	m_DigitClearTex = NULL;
	//}

	//if( m_DigitMissTex != NULL )
	//{
	//	::unloadTexture( m_DigitMissTex );
	//	m_DigitMissTex = NULL;
	//}
}

void CDamageEffect::Refresh()
{
	//for( int i = 0; i < MAX_DIGIT_EFFECT ; i++ )
	//{
	//	if( ( m_DigitNode[ i ].m_hAnimatable != NULL ) && m_DigitNode[ i ].m_bVisible )
	//	{
	//		short int iState = ::getAnimatableState( m_DigitNode[ i ].m_hAnimatable );

	//		if( iState == 0 )
	//		{
	//			//----------------------------------------------------------------------------------------------------	
	//			/// @brief Visibility 로 조정하는게 아니라 아예 Scene에서 뺀다.
	//			//----------------------------------------------------------------------------------------------------
	//			::removeFromScene( m_DigitNode[ i ].m_hPositionDummy );
	//			m_DigitNode[ i ].m_bVisible = false;
	//			/*::setVisibility( m_DigitNode[ i ].m_hAnimatable, 0 );
	//			::setVisibility( m_DigitNode[ i ].m_hPositionDummy, 0 );*/
	//		}
	//	}
	//}
}

/// 현재 사용되고 있지 않는 노드를 얻는다..( 애니매이션이 끝났거나, 아예 아직 생성되지 않은것 )
int CDamageEffect::GetEmptyNode()
{
	/*for( int i = 0; i < MAX_DIGIT_EFFECT ; i++ )
	{
		if( ( m_DigitNode[ i ].m_bVisible == false ) && ( m_DigitNode[ i ].m_hAnimatable != NULL ) )
		{
			short int iState = ::getAnimatableState( m_DigitNode[ i ].m_hAnimatable );

			if( iState == 0 )
			{
				return i;
			}
		}
	}*/

	return INVALID_EFFECTNODE;
}

void CDamageEffect::CreateDigitEffect( int iPoint, float x, float y, float z, bool bIsUSER )
{
}