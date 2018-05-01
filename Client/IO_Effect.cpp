/*
	$Header: /Client/IO_Effect.cpp 73    05-01-10 6:14p Jeddli $
*/
#include "stdAFX.h"
#include "Game.h"
#include "Common\IO_Motion.h"
#include "IO_Effect.h"
#include "IO_Mesh.h"
#include "Util\\VFSManager.h"

#include "CObjCHAR.h"

#include <assert.h>



/////////////////////////////////////////////////////////////////////////////////////
/// CParticle
/////////////////////////////////////////////////////////////////////////////////////
CParticle::~CParticle ()
{
	this->UnregisterFromNZINE ();
}

bool CParticle::RegisterToNZIN (char *szName, char *szParticleFile)
{
	if ( (CVFSManager::GetSingleton()).IsExistFile( szParticleFile ) == false )	
	{
		LogString ( LOG_DEBUG, "ERROR:: Particle File [%s] not found...\n", szParticleFile);
		return false;
	}

	m_hVisible = ::loadVisible ( NULL, NULL, NULL, NULL );
	if ( !m_hVisible )
		return false;
	
	m_hParticle = ::loadParticle ( szName, szParticleFile );
	if ( !m_hParticle ) {
		::unloadVisible( m_hVisible );
		m_hVisible = NULL;
		return false;
	}

	::controlParticle( m_hParticle, 0 );	// stop !
	::linkNode( m_hVisible, m_hParticle );

	return true;
}
void CParticle::UnregisterFromNZINE (void)
{
	if ( m_hParticle ) 
	{		
		::unloadParticle( m_hParticle );
		m_hParticle = NULL;
	}
	if ( m_hVisible ) {
		::unloadVisible( m_hVisible );
		m_hVisible = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/// CMeshEffect
/////////////////////////////////////////////////////////////////////////////////////
CMeshEffect::~CMeshEffect ()
{
	this->UnregisterFromNZINE ();
}

bool CMeshEffect::RegisterToNZIN (HNODE hMesh, HNODE hMotion, HNODE hMaterial)
{
	m_hVisible = ::loadVisible ( NULL, NULL, NULL, NULL );
	if ( !m_hVisible )
		return false;

	m_hAnimatable = ::loadAnimatable(NULL, 0, 0, 0, 0);
	if ( !m_hAnimatable ) {
		::unloadVisible( m_hVisible );
		m_hVisible = NULL;
		return false;
	}

	m_hMorpher = ::loadMorpher(NULL, hMesh, hMotion, hMaterial, NULL); // At this moment, MORPHER does not need to use g_GameDATA.m_hLight.
	if ( !m_hMorpher ) {
		::unloadVisible( m_hVisible );
		m_hVisible = NULL;
		::unloadAnimatable( m_hAnimatable );
		m_hAnimatable = NULL;
		return false;
	}

	::controlAnimatable( m_hMorpher,	0 );
	::controlAnimatable( m_hAnimatable, 0 );
	::linkNode ( m_hAnimatable, m_hMorpher );
	::linkNode ( m_hVisible, m_hAnimatable );
	return true;
}

void CMeshEffect::UnregisterFromNZINE (void)
{
	if ( m_hMorpher ) 
	{		
		::unloadMorpher( m_hMorpher );
		m_hMorpher = NULL;
	}

	if ( m_hAnimatable ) 
	{
		::unloadAnimatable ( m_hAnimatable );
		m_hAnimatable = NULL;
	}

	if ( m_hVisible ) 
	{
		::unloadVisible( m_hVisible );
		m_hVisible = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////////////
/// CEffectDATA
/////////////////////////////////////////////////////////////////////////////////////
CEffectDATA::CEffectDATA ()
{
	m_pParticle = NULL;
	m_pMeshAni  = NULL;
}

CEffectDATA::~CEffectDATA ()
{
	this->Free ();
}
void CEffectDATA::Free (void)
{
	SAFE_DELETE_ARRAY( m_pParticle );
	SAFE_DELETE_ARRAY( m_pMeshAni );
}

bool CEffectDATA::Load (char *szFileName)
{
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szFileName ) == false )	
	{
		return false;
	}

	int iStrLen;
	char *szName=CStr::GetString ();

	// Effect Name...
	pFileSystem->ReadInt32( &iStrLen );
	if ( iStrLen > 0 ) 
	{
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		szName[ iStrLen ] = 0;
		m_Name.Set( szName );
	}else
	{
		m_Name.Set( szFileName );
	}
	
	// Sound file name...
	pFileSystem->ReadInt32( &iStrLen );
	m_bUseSoundFile = ( iStrLen > 0 );

	pFileSystem->ReadInt32( &iStrLen );
	if ( iStrLen > 0 ) 
	{
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		szName[ iStrLen ] = 0;

		if ( (CVFSManager::GetSingleton()).IsExistFile( szName ) == false )			
		{
			m_bUseSoundFile = false;
		}
	}

	if ( m_bUseSoundFile && iStrLen ) 
	{

		m_SoundHash = g_pSoundLIST->AddSoundFile( szName, 1 );
		if ( 0 == m_SoundHash ) 
		{			
			char *szStr = CStr::Printf ("Sound file [%s] open error ", szName);
			// g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
			LogString ( LOG_DEBUG, "%s \n", szStr);
			m_bUseSoundFile = false;
		}
	}

	pFileSystem->ReadInt32( &m_iSoundLoopCNT );

	pFileSystem->ReadInt32( &m_iParticleCNT );
	if ( m_iParticleCNT > 0 )
		m_pParticle = new tagEFFECT_PARTICLE[ m_iParticleCNT ];

	for (int iL=0; iL < m_iParticleCNT ; iL++ ) 
	{
		// Particle effect name
		pFileSystem->ReadInt32(&iStrLen );
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		
		// Particle unique name
		pFileSystem->ReadInt32( &iStrLen );
		pFileSystem->Read( szName,	sizeof(char) * iStrLen );

		// Particle index in stb table...
		pFileSystem->ReadInt32( &iStrLen );


		// Particle file path & name
		pFileSystem->ReadInt32( &iStrLen );
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		szName[ iStrLen ] = 0;		
		m_pParticle[ iL ].m_FileName.Set( szName );

		// use flag animation file ..
		pFileSystem->ReadInt32( &iStrLen );
		m_pParticle[ iL ].m_bUseAniFile = iStrLen;

		// Particle animation name..
		pFileSystem->ReadInt32( &iStrLen );
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		szName[ iStrLen ] = 0;
		if ( m_pParticle[ iL ].m_bUseAniFile ) 
		{
			m_pParticle[ iL ].m_HashBoneANI = g_MotionFILE.Add_FILE ( szName );			
		}
		
		// Particle animation loop count...
		pFileSystem->ReadInt32( &m_pParticle[ iL ].m_iAniLoopCnt );

		// Particle animation index index in stb table...
		pFileSystem->ReadInt32( &iStrLen );

		// Position.. x,y,z
		pFileSystem->ReadFloat3( &m_pParticle[ iL ].m_Transform.x,
									&m_pParticle[ iL ].m_Transform.y,
									&m_pParticle[ iL ].m_Transform.z );		

		// rotation.. x,y,z,w
		// 쿼터니언이 아닌 Yaw, Pitch, Roll 로 들어온다.. 변환 필요
		float x, y, z, w;
		
		pFileSystem->ReadFloat4( &x, &y, &z, &w );
		
		
		D3DXQUATERNION  quat;
		D3DXQuaternionRotationYawPitchRoll( &quat, D3DXToRadian( y ), D3DXToRadian( x ), D3DXToRadian( z ) );

		m_pParticle[ iL ].m_Rotate.x = quat.x;
		m_pParticle[ iL ].m_Rotate.y = quat.z;
		m_pParticle[ iL ].m_Rotate.z = quat.y;
		m_pParticle[ iL ].m_Rotate.w = -quat.w;

		pFileSystem->ReadInt32( &m_pParticle[ iL ].m_iDelay );
		pFileSystem->ReadInt32( &m_pParticle[ iL ].m_iIsLink );
	}



	pFileSystem->ReadInt32( &m_iMeshAniCNT );
	if ( m_iMeshAniCNT > 0 )
		m_pMeshAni = new tagEFFECT_MESHANI[ m_iMeshAniCNT ];

	for( int iL=0; iL < m_iMeshAniCNT ; iL++ ) 
	{
		// Particle effect name
		pFileSystem->ReadInt32( &iStrLen );
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		// mesh ani unique name
		pFileSystem->ReadInt32( &iStrLen );
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		// particle index in stb table..
		pFileSystem->ReadInt32( &iStrLen );


		// mesh path
		pFileSystem->ReadInt32( &iStrLen );
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		szName[ iStrLen ] = 0;		
		m_pMeshAni[ iL ].m_HashMESH = g_MeshFILE.Add_FILE( szName );

		// mesh ani path		
		pFileSystem->ReadInt32( &iStrLen );
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		szName[ iStrLen ] = 0;		
		m_pMeshAni[ iL ].m_HashMeshANI = g_MotionFILE.Add_FILE( szName ) ;

		// mesh texture path
		pFileSystem->ReadInt32( &iStrLen );
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		szName[ iStrLen ] = 0;


		int	iAlpha;
		int	iTwoSide;

		pFileSystem->ReadInt32( &iAlpha );	// alpha
		pFileSystem->ReadInt32( &iTwoSide );	// two side
		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iAlphaTest );	// alpha tet
		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iZTest );	// z test
		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iZWrite );	// z write		
		m_pMeshAni[ iL ].m_HashMAT = g_MatFILE.Add_MATERIAL (szName, 0, iAlpha, iTwoSide);



		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iBlendSour );
		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iBlendDest );
		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iBlendOP );
		

		// use flag animation file ..
		pFileSystem->ReadInt32( &iStrLen );
		m_pMeshAni[ iL ].m_bUseAniFile = ( iStrLen != 0 );
		
		// animation name
		pFileSystem->ReadInt32( &iStrLen );
		pFileSystem->Read( szName, sizeof(char) * iStrLen );
		szName[ iStrLen ] = 0;		

		if ( (CVFSManager::GetSingleton()).IsExistFile( szName ) == false )			
		{
			m_pMeshAni[ iL ].m_bUseAniFile = false;
		}

		if ( m_pMeshAni[ iL ].m_bUseAniFile ) 
		{			
			if( strcmpi( szName, "NULL" ) == 0 )
			{
				return NULL;
			}

			m_pMeshAni[ iL ].m_HashBoneANI = g_MotionFILE.Add_FILE ( szName );
		}

		// meshani animation loop count...
		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iAniLoopCnt );

		// meshani index in stb table.
		pFileSystem->ReadInt32( &iStrLen );


		// Position.. x,y,z
		pFileSystem->ReadFloat3(    &(m_pMeshAni[ iL ].m_Transform.x),
									&(m_pMeshAni[ iL ].m_Transform.y),
									&(m_pMeshAni[ iL ].m_Transform.z) );

		// rotation.. x,y,z,w
		// 쿼터니언이 아닌 Yaw, Pitch, Roll 로 들어온다.. 변환 필요
		float x, y, z, w;
		
		pFileSystem->ReadFloat4( &x, &y, &z, &w );   		
		
		D3DXQUATERNION quat;
		D3DXQuaternionRotationYawPitchRoll( &quat, D3DXToRadian( y ), D3DXToRadian( x ), D3DXToRadian( z ) );

		m_pMeshAni[ iL ].m_Rotate.x = quat.x;
		m_pMeshAni[ iL ].m_Rotate.y = quat.z;
		m_pMeshAni[ iL ].m_Rotate.z = quat.y;
		m_pMeshAni[ iL ].m_Rotate.w = -quat.w;

		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iDelay );
		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iRepeatCNT );
		pFileSystem->ReadInt32( &m_pMeshAni[ iL ].m_iIsLink );
	}

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );

	return true;
}


int CEffect::m_iEffectCnt = 0;
/////////////////////////////////////////////////////////////////////////////////////
/// CEffect
/////////////////////////////////////////////////////////////////////////////////////
CEffect::CEffect ()
{
	m_pParticle = NULL;
	m_pMeshEffect = NULL;
	m_hRootNODEs[ 0 ] = NULL;
	m_hRootNODEs[ 1 ] = NULL;

	m_bIsVisible = false;
	m_pListNODE = NULL;

	m_pEffectDATA = NULL;

	m_hParent = NULL;

	m_iEffectCnt++;
}

//-------------------------------------------------------------------------------------------------
CEffect::CEffect( CEffectDATA *pEffectDATA )
{
	m_pParticle   = NULL;
	m_pMeshEffect = NULL;
	m_hRootNODEs[ 0 ] = NULL;
	m_hRootNODEs[ 1 ] = NULL;

	m_bIsVisible  = false;
	m_pListNODE = NULL;

	this->Create( pEffectDATA );

	m_pParent = NULL;	

	m_bNightEffect = false;	/// 밤에만 나오는 이펙트 인가?
}

CEffect::~CEffect ()
{
	if ( m_bIsVisible )
		this->RemoveFromScene ();

	int iC;
	if ( m_pMeshEffect ) {
		for (iC=0; iC<m_pEffectDATA->m_iMeshAniCNT; iC++) {
			m_pMeshEffect[ iC ].UnregisterFromNZINE ();
		}
		SAFE_DELETE_ARRAY( m_pMeshEffect );
	}

	if ( m_pParticle ) {
		for (iC=0; iC<m_pEffectDATA->m_iParticleCNT; iC++) {
			m_pParticle[ iC ].UnregisterFromNZINE ();
		}
		SAFE_DELETE_ARRAY( m_pParticle );
	}

	if ( m_hRootNODEs[ 1 ] ) 
	{
//		if( ::getParent( m_hRootNODEs[ 1 ] ) != NULL )		
//			::unlinkNode( m_hRootNODEs[ 1 ] );

		::unloadVisible( m_hRootNODEs[ 1 ] );
		m_hRootNODEs[ 1 ] = NULL;
	}
	if ( m_hRootNODEs[ 0 ] ) 
	{
//		if( ::getParent( m_hRootNODEs[ 0 ] ) != NULL )		
//			::unlinkNode( m_hRootNODEs[ 0 ] );

		::unloadVisible( m_hRootNODEs[ 0 ] );
		m_hRootNODEs[ 0 ] = NULL;
	}	
}

bool CEffect::Create (CEffectDATA *pEffectDATA)
{
	if ( NULL == pEffectDATA )
		return false;

	m_pEffectDATA = pEffectDATA;

	if ( m_pEffectDATA->m_bUseSoundFile ) 
	{
		g_pSoundLIST->KEY_PlaySound( m_pEffectDATA->m_SoundHash );
	}
	
	char *szName  = CStr::Printf ( "EFT_%s_%d", ( pEffectDATA->m_Name.Get() == NULL )? "NULL":pEffectDATA->m_Name.Get(), m_iEffectCnt );
	char *szName2  = CStr::Printf ( "EFT_%s_%d_R2", ( pEffectDATA->m_Name.Get() == NULL )? "NULL":pEffectDATA->m_Name.Get(), m_iEffectCnt );

	m_iEffectCnt++;

	m_hRootNODEs[ 0 ] = ::loadVisible ( szName, NULL, NULL, NULL );
	m_hRootNODEs[ 1 ] = ::loadVisible ( szName2, NULL, NULL, NULL );
	_ASSERT( m_hRootNODEs[0] && m_hRootNODEs[1] );	

	int iC;
	HNODE hMesh, hMat, hMotion;
	m_pParticle = new CParticle[ pEffectDATA->m_iParticleCNT ];
	for (iC=0; iC<m_pEffectDATA->m_iParticleCNT; iC++) 
	{
		if ( !m_pParticle[ iC ].RegisterToNZIN( NULL, m_pEffectDATA->m_pParticle[ iC ].m_FileName.Get() ) )
			continue;

		::setStartDelay ( m_pParticle[ iC ].GetPARTICLE(), m_pEffectDATA->m_pParticle[ iC ].m_iDelay );
		::setPosition( m_pParticle[ iC ].GetVISIBLE(), 
			m_pEffectDATA->m_pParticle[ iC ].m_Transform.x, 
			m_pEffectDATA->m_pParticle[ iC ].m_Transform.y,
			m_pEffectDATA->m_pParticle[ iC ].m_Transform.z );

		::setQuaternion( m_pParticle[ iC ].GetVISIBLE(), 
				m_pEffectDATA->m_pParticle[ iC ].m_Rotate.w,
				m_pEffectDATA->m_pParticle[ iC ].m_Rotate.x,
				m_pEffectDATA->m_pParticle[ iC ].m_Rotate.y,
				m_pEffectDATA->m_pParticle[ iC ].m_Rotate.z );

		/// particle 자체가 ( hVisible <- hparticle )구조이다.
		if ( m_pEffectDATA->m_pParticle[ iC ].m_iIsLink )
			::linkNode( m_hRootNODEs[ 0 ], m_pParticle[ iC ].GetVISIBLE() );
		else
			::linkNode( m_hRootNODEs[ 1 ], m_pParticle[ iC ].GetVISIBLE() );

		if ( m_pEffectDATA->m_pParticle[ iC ].m_bUseAniFile ) {
			hMotion = g_MotionFILE.KEY_GetZMOTION( m_pEffectDATA->m_pParticle[ iC ].m_HashBoneANI );
			if ( hMotion ) 
			{
				::attachMotion( m_pParticle[ iC ].GetPARTICLE(), hMotion );
				::setRepeatCount( m_pParticle[ iC ].GetPARTICLE(), m_pEffectDATA->m_pParticle[ iC ].m_iAniLoopCnt );
			}
		}
	}

	if ( pEffectDATA->m_iMeshAniCNT ) {
		m_pMeshEffect = new CMeshEffect[ pEffectDATA->m_iMeshAniCNT ];
		for (iC=0; iC<m_pEffectDATA->m_iMeshAniCNT; iC++) {
			hMesh   = g_MeshFILE.KEY_HNODE ( m_pEffectDATA->m_pMeshAni[ iC ].m_HashMESH );
			assert( hMesh );
			
			hMat    = g_MatFILE.KEY_HNODE  ( m_pEffectDATA->m_pMeshAni[ iC ].m_HashMAT );
			assert( hMat );
			
			::setMaterialBlendTypeCustom( hMat, m_pEffectDATA->m_pMeshAni[ iC ].m_iBlendSour,
												m_pEffectDATA->m_pMeshAni[ iC ].m_iBlendDest,
												m_pEffectDATA->m_pMeshAni[ iC ].m_iBlendOP);

			::setMaterialUseAlphaTest( hMat, m_pEffectDATA->m_pMeshAni[ iC ].m_iAlphaTest );
			::setMaterialZWrite( hMat, m_pEffectDATA->m_pMeshAni[ iC ].m_iZWrite );
			::setMaterialZTest( hMat, m_pEffectDATA->m_pMeshAni[ iC ].m_iZTest );


			hMotion = g_MotionFILE.KEY_GetZMOTION( m_pEffectDATA->m_pMeshAni[ iC ].m_HashMeshANI );			

			if ( !m_pMeshEffect[ iC ].RegisterToNZIN( hMesh, hMotion, hMat ) ) {
				g_MeshFILE.Sub_DATAUseKEY ( m_pEffectDATA->m_pMeshAni[ iC ].m_HashMESH );
				g_MatFILE.Sub_DATAUseKEY  ( m_pEffectDATA->m_pMeshAni[ iC ].m_HashMAT );
				g_MotionFILE.Sub_DATAUseKEY( m_pEffectDATA->m_pMeshAni[ iC ].m_HashMeshANI );
				continue;
			}
			
			::setPosition( m_pMeshEffect[ iC ].GetVISIBLE(), 
				m_pEffectDATA->m_pMeshAni[ iC ].m_Transform.x,
				m_pEffectDATA->m_pMeshAni[ iC ].m_Transform.y,
				m_pEffectDATA->m_pMeshAni[ iC ].m_Transform.z );

			::setQuaternion( m_pMeshEffect[ iC ].GetVISIBLE(), 
				m_pEffectDATA->m_pMeshAni[ iC ].m_Rotate.w,
				m_pEffectDATA->m_pMeshAni[ iC ].m_Rotate.x,
				m_pEffectDATA->m_pMeshAni[ iC ].m_Rotate.y,
				m_pEffectDATA->m_pMeshAni[ iC ].m_Rotate.z );
		

			/// link 구조( hVisible <- hAnimatable <- hMorpher )
			if ( m_pEffectDATA->m_pMeshAni[ iC ].m_iIsLink )
				::linkNode( m_hRootNODEs[ 0 ], m_pMeshEffect[ iC ].GetVISIBLE() );
			else
				::linkNode( m_hRootNODEs[ 1 ], m_pMeshEffect[ iC ].GetVISIBLE() );

			if ( m_pEffectDATA->m_pMeshAni[ iC ].m_bUseAniFile ) {
				hMotion = g_MotionFILE.KEY_GetZMOTION( m_pEffectDATA->m_pMeshAni[ iC ].m_HashBoneANI );
				if ( hMotion )
				{
					::attachMotion( m_pMeshEffect[ iC ].GetANIMATABLE(), hMotion );
					::setRepeatCount( m_pMeshEffect[ iC ].GetANIMATABLE(), m_pEffectDATA->m_pMeshAni[ iC ].m_iAniLoopCnt );
				}
			}

			::setStartDelay ( m_pMeshEffect[ iC ].GetMORPHER(), m_pEffectDATA->m_pMeshAni[ iC ].m_iDelay );
			::setRepeatCount( m_pMeshEffect[ iC ].GetMORPHER(), m_pEffectDATA->m_pMeshAni[ iC ].m_iRepeatCNT );
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
void CEffect::InsertToScene ()
{
	if ( m_bIsVisible )
		return;	

	::insertToScene( m_hRootNODEs[ 0 ] );
	::insertToScene( m_hRootNODEs[ 1 ] );

	StartEffect();
	
	m_bIsVisible = true;	
}

void CEffect::RemoveFromScene ()
{
	if ( !m_bIsVisible )
		return;

	if( m_pEffectDATA == NULL )
	{
		assert( 0 && " CEffect::RemoveFromScene[ Effect data is invalid ]" );
	}

	//----------------------------------------------------------------------------------------------------	
	/// @brief CHILD의removeFromScene제거
	//----------------------------------------------------------------------------------------------------
	::removeFromScene( m_hRootNODEs[ 0 ] );
	::removeFromScene( m_hRootNODEs[ 1 ] );	

	StopEffect();

	m_bIsVisible = false;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 이펙트 애니매이션 시작
//----------------------------------------------------------------------------------------------------
void CEffect::StartEffect()
{
	int iC;
	if ( m_pEffectDATA->m_iMeshAniCNT ) 
	{
		for (iC=0; iC<m_pEffectDATA->m_iMeshAniCNT; iC++) 
		{
			::controlAnimatable( m_pMeshEffect[ iC ].GetANIMATABLE (),	1 );
			::controlAnimatable( m_pMeshEffect[ iC ].GetMORPHER (),		1 );
			//----------------------------------------------------------------------------------------------------	
			/// @brief CHILD의InsertToScene제거
			//----------------------------------------------------------------------------------------------------
			//::insertToScene    ( m_pMeshEffect[ iC ].GetMORPHER () );
			//::insertToScene    ( m_pMeshEffect[ iC ].GetANIMATABLE () );
			//::insertToScene    ( m_pMeshEffect[ iC ].GetVISIBLE () );
		}
	}

	for (iC=0; iC<m_pEffectDATA->m_iParticleCNT; iC++) {
		::controlParticle( m_pParticle[ iC ].GetPARTICLE (), 1 );
		//::insertToScene  ( m_pParticle[ iC ].GetPARTICLE () );
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 이펙트 애니매이션 스탑
//----------------------------------------------------------------------------------------------------
void CEffect::StopEffect()
{
	//----------------------------------------------------------------------------------------------------	
	/// @brief 모든 동작 정지..
	//----------------------------------------------------------------------------------------------------
	int iC;
	if ( m_pEffectDATA->m_iMeshAniCNT ) 
	{
		for (iC=0; iC<m_pEffectDATA->m_iMeshAniCNT; iC++) 
		{
			::controlAnimatable( m_pMeshEffect[ iC ].GetANIMATABLE (),	0 );
			::controlAnimatable( m_pMeshEffect[ iC ].GetMORPHER (),		0 );			
		}
	}

	for (iC=0; iC<m_pEffectDATA->m_iParticleCNT; iC++) 
	{
		::controlParticle( m_pParticle[ iC ].GetPARTICLE (), 0 );		
	}
}

//-------------------------------------------------------------------------------------------------
/// 충돌 조사는 반드시 메시가 있어야 한다.
bool CEffect::IsIntersect (HNODE hMODEL)
{
	for (int iC=0; iC<m_pEffectDATA->m_iMeshAniCNT; iC++) 
	{
		if ( !m_pMeshEffect[ iC ].GetMORPHER() )
			continue;

		if ( ::intersectNode( m_pMeshEffect[ iC ].GetMORPHER(), hMODEL) )
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
/// 모든 애니매이션이 종료 되었는가?( It's time to end. )
bool CEffect::IsFinish (void)
{
	if( m_pEffectDATA == NULL )
		return true;

	for (int iC=0; iC<m_pEffectDATA->m_iParticleCNT; iC++) 
	{
		if ( ::getParticleState( m_pParticle[ iC ].GetPARTICLE() ) )
			return false;
	}

	for (int iC=0; iC<m_pEffectDATA->m_iMeshAniCNT; iC++) 
	{
		if ( ::getAnimatableState( m_pMeshEffect[ iC ].GetMORPHER() ) )
			return false;
	}

	return true;
}


static D3DXVECTOR3 s_xAxis(1, 0, 0);
#define M_PI       3.14159265358979323846
void CEffect::Rotation( D3DXVECTOR3 &vec )
{		
	D3DXVECTOR3 vPos;
	this->GetPOSITION( &vPos );
	::rotateByVectorRelative( m_hRootNODEs[ 0 ], s_xAxis, (float*)&( vPos + vec ) );
	::rotateByVectorRelative( m_hRootNODEs[ 1 ], s_xAxis, (float*)&( vPos + vec ) );
	/// d3d coord
	/*float fRY	= atan2f( vec.x, vec.y ); 		
	float fRZ   = atan2f( vec.x*sinf(fRY) + vec.z*cosf(fRY), vec.y ) - (M_PI/2.0f);	
	float fRX	= 0.0f;*/
 //   
	//D3DXQUATERNION quat;
	//D3DXQuaternionRotationYawPitchRoll( &quat, fRY, fRX, fRZ );

	//LogString( LOG_NORMAL, "X[%.4f],Y[%.4f],Z[%.4f],rRY[%.4f]\n", vec.x, vec.y, vec.z, fRY );
	//LogString( LOG_NORMAL, "Yaw[%.4f],Pitch[%.4f],Roll[%.4f]\n", fRY, fRX, fRZ );
	/////LogString( LOG_NORMAL, "PITCH[%.4f] ROLL[%.4f]\n", fRX, fRZ );
	//
	///*float fTemp = quat.z;
	//quat.z = quat.y;
	//quat.y = fTemp;
	//quat.w = -quat.w;*/
	
	//Rotation( quat );
}




/////////////////////////////////////////////////////////////////////////////////////
/// CEffectLIST
/////////////////////////////////////////////////////////////////////////////////////
CEffectLIST::CEffectLIST (char *szFileEFFECT)
{
	m_pHashKEY = NULL;
	m_nFileEffectCNT = 0;

	CGameSTB fSTB;

	if ( fSTB.Open (szFileEFFECT) ) {
		char *szFileName;

		m_nFileEffectCNT = fSTB.GetRowCount();

		m_pHashKEY = new t_HASHKEY [ m_nFileEffectCNT ];
		for (short nI=0; nI<m_nFileEffectCNT; nI++) {
			m_pHashKEY[ nI ] = 0;

			szFileName = fSTB.GetString(1, nI);			
			if ( szFileName ) 
			{
				if ( (CVFSManager::GetSingleton()).IsExistFile( szFileName ) == true )					
				{					
					m_pHashKEY[ nI ] = this->Add_EffectFILE( szFileName );
				}else
				{
					LogString ( LOG_DEBUG, "EffectFile:: %s 없다.\n", szFileName );
				}
			}
		}
		fSTB.Close ();
	}
}

CEffectLIST::~CEffectLIST ()
{
	SAFE_DELETE_ARRAY( m_pHashKEY );

	classDLLNODE< CEffect* > *pNode;
	pNode = m_LIST.GetHeadNode ();
	while( pNode ) {
		SAFE_DELETE( pNode->DATA );
		m_LIST.DeleteNFree( pNode );

		pNode = m_LIST.GetHeadNode ();
	}
}

//-------------------------------------------------------------------------------------------------
/// @ bug \\   /  \ 등 패스를 지정하는 방법에따라 Hash값이 틀리게 나온다 ㅡ,.ㅡ
///				char *aa[] = { "3ddata\\effect\\smoke_01.eft", "3ddata\Effect\smoke_01.eft" };
///			
t_HASHKEY CEffectLIST::Add_EffectFILE(char *szEffectFile)
{
	if ( (CVFSManager::GetSingleton()).IsExistFile( szEffectFile ) == false )		
	{
		char *szMSG = CStr::Printf ("ERROR Effect File [%s] not found ... \n", szEffectFile);
		g_pCApp->ErrorBOX( szMSG, "ERROR" );
		LogString ( LOG_DEBUG, szMSG);
		return 0;
	}

	t_HASHKEY uiKEY = CStr::GetHASH( szEffectFile );
	if ( m_HASH.Search( uiKEY ) ) {
		return uiKEY;
	}

	CEffectDATA *pEffectDATA;

	pEffectDATA = new CEffectDATA;
	if ( !pEffectDATA->Load (szEffectFile) ) {
		delete pEffectDATA;
		pEffectDATA = NULL;
		return 0;
	}

	m_HASH.Insert( uiKEY, pEffectDATA );
	return uiKEY;
}

//-------------------------------------------------------------------------------------------------
t_HASHKEY CEffectLIST::Get_IndexHashKEY (short nIndex)
{
	if ( nIndex < 0 || nIndex >= m_nFileEffectCNT ) {
		_ASSERT( 0 );
		return 0;
	}

	return m_pHashKEY[ nIndex ];
}

CEffect *CEffectLIST::Add_EffectWithIDX (short nIndex, bool bAdd2LIST)
{
	if ( nIndex < 0 || nIndex >= m_nFileEffectCNT ) {
		_ASSERT( 0 );
		return NULL;
	}

	return Add_EFFECT( m_pHashKEY[ nIndex ], bAdd2LIST );
}

//-------------------------------------------------------------------------------------------------
CEffectDATA *CEffectLIST::Get_EffectDATA( t_HASHKEY uiHashKEY )
{
	if ( 0 == uiHashKEY )
		return NULL;

	tagHASH< CEffectDATA* > *pHashNode = m_HASH.Search( uiHashKEY );
	return pHashNode ? pHashNode->m_DATA : NULL;
}
CEffect *CEffectLIST::Add_EFFECT (t_HASHKEY uiHashKEY, bool bAdd2LIST)
{
	if ( 0 == uiHashKEY )
		return NULL;

	tagHASH< CEffectDATA* > *pHashNode = m_HASH.Search( uiHashKEY );
	
	CEffectDATA *pEffectDATA = pHashNode ? pHashNode->m_DATA : NULL;
	if ( NULL == pEffectDATA )
		return NULL;
	
	CEffect *pEffect = new CEffect( pEffectDATA );
	if ( pEffect && bAdd2LIST ) {
		pEffect->m_pListNODE = m_LIST.AllocNAppend( pEffect );
	}

	return pEffect;
}

/// Hash table 에서는 빼지 않는다.. 이것이 기본 시스템이다.
void CEffectLIST::Del_EFFECT(CEffect *pCEffect, bool bUpdateParent )
{
	if ( pCEffect ) 
	{
		if ( pCEffect->m_pListNODE )
		{
			m_LIST.DeleteNFree( pCEffect->m_pListNODE );

			if( bUpdateParent )
			{
				CObjCHAR* pChar = pCEffect->GetParentCHAR();
				/// 내게 부모가 있다면.. 부모의 이펙트 리스트를 갱신하라..
				if( pChar )
				{
					pChar->DeleteExternalEffect( pCEffect );
				}
			}

			SAFE_DELETE( pCEffect );
		}
	}
}


CEffect* CEffectLIST::Get_EffectClassWithHashKEY( t_HASHKEY uiHashKEY )
{
	return NULL;
}


//-------------------------------------------------------------------------------------------------
void CEffectLIST::Proc ()
{
	classDLLNODE< CEffect* > *pNode;
	pNode = m_LIST.GetHeadNode ();
	while( pNode ) 
	{		
		if ( pNode->DATA->IsFinish () ) 
		{
			classDLLNODE< CEffect* > *pDelNode;

			pDelNode = pNode;
			pNode = m_LIST.GetNextNode( pNode );

			/// 부모가 있다면 부모의 레퍼런스 갱신
			if( pDelNode->DATA->GetParentCHAR() != NULL )
				Del_EFFECT( pDelNode->DATA, true );
			else
				Del_EFFECT( pDelNode->DATA, false );

			continue;			
		}

		pNode = m_LIST.GetNextNode( pNode );
	}
}

//-------------------------------------------------------------------------------------------------
