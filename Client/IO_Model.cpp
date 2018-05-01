/*
	$Header: /Client/IO_Model.cpp 50    04-08-19 5:11p Jeddli $
*/

#include "stdAFX.h"
#include "Game.h"
#include "IO_Model.h"
#include "Common/IO_Motion.h"


#define	BIT_ROTAXIS			1

#define	SWITCH_NULL			0
#define	SWITCH_POS			1
#define	SWITCH_ROT			2
#define	SWITCH_SCALE		3
#define	SWITCH_ROTAXIS		4

#define	SWITCH_BONEIDX		5
#define	SWITCH_DUMMYIDX		6

#define SWITCH_PARENT		7

enum {
	MESH_ANI_MOB_STOP = 0,
	MESH_ANI_MOB_MOVE,
	MESH_ANI_MOB_ATTACK,
	MESH_ANI_MOB_HIT,
	MESH_ANI_MOB_DIE,
	MESH_ANI_MOB_ETC,

	MESH_ANI_AVT_STOP1,
	MESH_ANI_AVT_STOP2,
	MESH_ANI_AVT_WALK,
	MESH_ANI_AVT_RUN,
	MESH_ANI_AVT_SITTING,
	MESH_ANI_AVT_SIT,
	MESH_ANI_AVT_STANDUP,
	MESH_ANI_AVT_STOP3,
	MESH_ANI_AVT_ATTACK,
	MESH_ANI_AVT_HIT,
	MESH_ANI_AVT_FALL,
	MESH_ANI_AVT_DIE,
	MESH_ANI_AVT_RAISE,
	MESH_ANI_AVT_JUMP1,
	MESH_ANI_AVT_JUMP2,

	MAX_MESH_ANI_TYPE
} ;

#define	SWITCH_ANI			8
#define SWITCH_COLLISION	( SWITCH_ANI+MAX_MESH_ANI_TYPE )	// 충동 체크 조사 여부.
#define	SWITCH_CNST_ANI		( SWITCH_COLLISION+1 )
#define SWITCH_RANGE_SET	( SWITCH_COLLISION+2 )
#define SWITCH_BUSE_LIGHTMAP	( SWITCH_COLLISION+3 )


bool CPointPART::Load( CFileSystem* pFileSystem )
{
	BYTE btTAG, btLen;

	pFileSystem->ReadByte( &btTAG );
	while( btTAG ) 
	{
		pFileSystem->ReadByte( &btLen );

		switch( btTAG ) 
		{
			case SWITCH_PARENT   :
				// parent->m_nParent = -1 !!
				pFileSystem->ReadInt16( &m_nParent );
				m_nParent --;
				break;

			case SWITCH_POS	:
				pFileSystem->Read( &m_Transform, sizeof(D3DVECTOR) );
				break;

			case SWITCH_ROT	:
				pFileSystem->ReadFloat4( &m_Rotate.w, &m_Rotate.x, &m_Rotate.y, &m_Rotate.z );				
				break;

			case SWITCH_SCALE :
				pFileSystem->Read( &m_Scale, sizeof(D3DVECTOR) );				
				break;

			default :
				pFileSystem->Seek( btLen, FILE_POS_CUR );				
				break;
		}

		pFileSystem->ReadByte( &btTAG );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
void CBasicPART::UnloadVisible (HNODE hVIS)
{
	g_MeshFILE.Sub_DATAUseKEY ( m_uiMeshKEY );
	g_MatFILE.Sub_DATAUseKEY  ( m_uiMatKEY  );

	if ( hVIS ) 
	{		
		LogString (LOG_DEBUG, "unloadVisible :: %d %s \n", hVIS, getName( hVIS ));
		::unloadVisible (hVIS);
	}
}

bool CBasicPART::Load( CFileSystem* pFileSystem, short nLinkBoneNo)
{
	BYTE btTAG, btLen;

	pFileSystem->ReadByte( &btTAG );
	while( btTAG ) {
		pFileSystem->ReadByte( &btLen );
		pFileSystem->Seek( btLen, FILE_POS_CUR);

		pFileSystem->ReadByte( &btTAG );
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CCharPART::Load( CFileSystem* pFileSystem, short nLinkBoneNo, short nLinkDummyNo)
{
	BYTE btTAG, btLen;

	pFileSystem->ReadByte( &btTAG );
	while( btTAG ) 
	{
		pFileSystem->ReadByte( &btLen );

		switch( btTAG ) 
		{
			case SWITCH_BONEIDX  :
				pFileSystem->ReadInt16( &m_nLinkBoneIDX );
				break;

			case SWITCH_DUMMYIDX :
				pFileSystem->ReadInt16( &m_nLinkDummyIDX );
				break;

			default :
				if ( btTAG >= SWITCH_ANI && btTAG < SWITCH_ANI+MAX_MESH_ANI_TYPE ) 
				{
					char *szAniFile = CStr::GetString ();
					if ( btTAG <= SWITCH_ANI+MESH_ANI_MOB_ETC ) 
					{
						// mob mesh ani
						btTAG -= SWITCH_ANI;
						pFileSystem->Read( szAniFile, sizeof(char) * btLen );
					} else {
						// avatar mesh ani
						btTAG -= ( SWITCH_ANI+MESH_ANI_MOB_ETC );
						pFileSystem->Read( szAniFile, sizeof(char) * btLen );
					}
					szAniFile[ btLen ] = 0;

					if ( NULL == m_pMeshAniFILE ) 
					{
						// 아바타 애니가 몹 애니 보다 많으니까... MAX_AVT_ANI사용.
						m_pMeshAniFILE = new t_HASHKEY[ MAX_AVT_ANI ];
						::ZeroMemory( m_pMeshAniFILE, sizeof(t_HASHKEY)*MAX_AVT_ANI);
					}
					m_pMeshAniFILE[ btTAG ] = g_MotionFILE.Add_FILE ( szAniFile );

					LogString (LOG_DEBUG, "MESH ANI:: %d / %d : %s \n", btTAG, m_pMeshAniFILE[ btTAG ], szAniFile);
				} else
					pFileSystem->Seek( btLen, FILE_POS_CUR );					
		}

		pFileSystem->ReadByte( &btTAG );
	}


	// set default mesh ani ..
	if ( m_pMeshAniFILE ) {
		for (short nI=0; nI<MAX_AVT_ANI; nI++) 
		{
			if ( !m_pMeshAniFILE[ nI ] ) {
				// 모션이 없는 부분은 정지로 ..
				m_pMeshAniFILE[ nI ] = m_pMeshAniFILE[ MOB_ANI_STOP ];	// MOB_ANI_STOP == AVT_ANI_STOP1 == 0
			}
		}
	}

	// use default bone idx
	if ( m_nLinkBoneIDX < 0 )
		m_nLinkBoneIDX = nLinkBoneNo;
	if ( m_nLinkDummyIDX < 0 )
		m_nLinkDummyIDX = nLinkDummyNo;
	
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CCharPART::Is_LinkedMODEL (void)
{
	if ( m_pMeshAniFILE || 
		 m_nLinkBoneIDX >= 0 ||
		 m_nLinkDummyIDX >= 0 )
		return true;
	
	return false;
}

//-------------------------------------------------------------------------------------------------
HNODE CCharPART::Load_ZMODEL(char *szName, HNODE hModelNODE)
{
	HNODE hMesh, hMat, hReturn;
	hMesh = g_MeshFILE.KEY_HNODE ( m_uiMeshKEY );
	hMat  = g_MatFILE.KEY_HNODE	 ( m_uiMatKEY  );

	if ( !hMesh || !hMat ) {
		char *szStr=CStr::Printf ("Mesh %s and Texture %s not found \n", g_MeshFILE.Get_FileNAME( m_uiMeshKEY ), g_MatFILE.Get_FileNAME( m_uiMatKEY ) );
		// g_CApp.ErrorBOX ( szStr, "File not found" );
		return NULL;
	}

	if ( this->m_pMeshAniFILE ) {
		_ASSERT( this->m_nLinkBoneIDX >= 0 );

		hReturn = ::loadMorpher( szName, hMesh, NULL, hMat, g_GameDATA.m_hCharacterLight );
		if ( hReturn ) {
			::linkBone ( hModelNODE, hReturn, this->m_nLinkBoneIDX );
		} else {
			LogString (LOG_DEBUG, "ERROR:: loadMorpher(%s) == NULL !!!\n", szName);
			_ASSERT( hReturn );
		}
		return hReturn;
	}
	
	if ( this->m_nLinkBoneIDX >= 0 ) {
		hReturn = ::loadVisible ( szName, hMesh, hMat, g_GameDATA.m_hCharacterLight );
		if ( hReturn ) {
			::linkBone ( hModelNODE, hReturn, this->m_nLinkBoneIDX );
		} else {
			LogString (LOG_DEBUG, "ERROR:: loadVisible(%s) == NULL !!!\n", szName);
			_ASSERT( hReturn );
		}
		HNODE hTemp = ::findNode( szName );
		return hReturn;
	}

	if ( this->m_nLinkDummyIDX >= 0 ) {
		hReturn = ::loadVisible ( szName, hMesh, hMat, g_GameDATA.m_hCharacterLight );
		if ( hReturn ) {
			::linkDummy( hModelNODE, hReturn, this->m_nLinkDummyIDX);
		} else {
			LogString (LOG_DEBUG, "ERROR:: loadVisible(%s) == NULL !!!\n", szName);
			_ASSERT( hReturn );
		}
		HNODE hTemp = ::findNode( szName );
		return hReturn;
	}

	// skinning
	::addRenderUnit( hModelNODE, hMesh, hMat, g_GameDATA.m_hCharacterLight );	
	return NULL;
}

//-------------------------------------------------------------------------------------------------
void CCharPART::Free_ZMODEL( HNODE hNODE )
{
	if ( m_pMeshAniFILE ) {
		for (short nI=0; nI<MAX_AVT_ANI; nI++)
			g_MotionFILE.Sub_DATAUseKEY( m_pMeshAniFILE[ nI ] );
			
		::unloadMorpher( hNODE );
		this->UnloadVisible( NULL );
		return;
	}

	this->UnloadVisible( hNODE );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CFixedPART::CFixedPART ()
{
	m_HashZMOFile = 0;
	m_nCollisionLevel = 0;
	m_nParent=-2;

	::ZeroMemory (&m_Rotate,	sizeof(D3DXQUATERNION));
	::ZeroMemory (&m_Transform,	sizeof(D3DVECTOR));
	::ZeroMemory (&m_RotAXIS,	sizeof(D3DXQUATERNION));
	m_Scale = D3DXVECTOR3( 1,1,1 );

	m_nRangeSet = 0;
}

/// @bug collision level 성정 이상하다..
bool  CFixedPART::Load ( CFileSystem* pFileSystem, short nLinkBoneNo, short nLinkDummyNo)
{
	BYTE btTAG, btLen;

	pFileSystem->ReadByte( &btTAG );
	while( btTAG ) 
	{
		pFileSystem->ReadByte( &btLen );

		switch( btTAG ) {
			case SWITCH_POS	:
				pFileSystem->Read ( &m_Transform, sizeof(D3DVECTOR) );
				break;

			case SWITCH_ROT	:
				pFileSystem->ReadFloat4( &m_Rotate.w, &m_Rotate.x, &m_Rotate.y, &m_Rotate.z );				
				break;

			case SWITCH_SCALE :
				pFileSystem->Read ( &m_Scale, sizeof(D3DVECTOR) );				
				break;

			case SWITCH_ROTAXIS :
				m_wAniTYPE |= BIT_ROTAXIS;
				pFileSystem->ReadFloat4( &m_RotAXIS.w, &m_RotAXIS.x, &m_RotAXIS.y, &m_RotAXIS.z );	// degree			
				break;

			case SWITCH_PARENT   :
				// parent->m_nParent = -1 !!
				pFileSystem->ReadInt16( &m_nParent );
				m_nParent --;
				break;

			case SWITCH_COLLISION :	// 충돌 조사 !!!
			{
				short nColl;
				pFileSystem->ReadInt16( &nColl );
				if ( nColl )
					m_nCollisionLevel = nColl;
				break;
			}
			case SWITCH_CNST_ANI : 
			{
				char *szZmoFile = CStr::GetString ();
				pFileSystem->Read( szZmoFile, sizeof(char) * btLen );
				szZmoFile[ btLen ] = 0;

				m_HashZMOFile = g_MotionFILE.Add_FILE ( szZmoFile );
				LogString (LOG_DEBUG, "Cnst ZMO :: %d : %s \n", m_HashZMOFile, szZmoFile);
				if ( !m_HashZMOFile )
					g_pCApp->ErrorBOX ( szZmoFile, "File not found ..." );
				break;
			}


			/// 2004/3/17 Rangeset, buseLightmap 추가
			case SWITCH_RANGE_SET:
				{
					short nColl;
					pFileSystem->ReadInt16( &nColl );
					if ( nColl )
						m_nRangeSet = nColl;
				}				
				break;
			case SWITCH_BUSE_LIGHTMAP:
				{
					short nColl;
					pFileSystem->ReadInt16( &nColl );
					if ( nColl )
						m_nUseLightMap = nColl;
				}
				break;

			default :
				pFileSystem->Seek( btLen, FILE_POS_CUR );				
				break;
		}

		pFileSystem->ReadByte( &btTAG );
	}

	return true;
}

///
/// 내부에 세팅된 데이터로서 실제 노드를 생성한다.
///
HNODE CFixedPART::LoadVisible (char *szName, D3DVECTOR BasePOS, HNODE hParent)
{
	HNODE hVis, hMesh, hMat;

	hMesh = g_MeshFILE.Get_DATAUseKEY ( m_uiMeshKEY );
	hMat  = g_MatFILE.KEY_HNODE ( m_uiMatKEY  );

	if ( !hMesh ) {
		LogString (LOG_DEBUG, "ERROR:: LoadVisible[ %s ] hMesh: %d, hMat: %d \n", szName, hMesh, hMat);
		_ASSERT ( 0 );
		return NULL;
	}

	// animation mesh !!!
	if ( m_HashZMOFile ) {
		HNODE hMotion = g_MotionFILE.KEY_GetZMOTION( m_HashZMOFile );
		hVis = ::loadAnimatable ( szName, hMesh, hMotion, hMat, g_GameDATA.m_hLight );
		_ASSERT( hVis );
		LogString (LOG_DEBUG, "loadAnimatable :: %d / %d, %s \n", hVis, hMesh, getName( hMesh ));
	} else {
		hVis = ::loadVisible ( szName, hMesh, hMat, g_GameDATA.m_hLight );
		LogString (LOG_DEBUG, "loadVisible :: %d / %d, %s \n", hVis, hMesh, getName( hMesh ));
	}

	if ( hVis ) {
		::setScale	 ( hVis, m_Scale.x,	 m_Scale.y,  m_Scale.z );
		::setQuaternion( hVis, m_Rotate.w, m_Rotate.x, m_Rotate.y,	m_Rotate.z);

		if ( hParent ) {
			::setPosition  ( hVis, m_Transform.x,	m_Transform.y, m_Transform.z );
			::linkNode (hParent, hVis);
		} else
			::setPosition  ( hVis, BasePOS.x+m_Transform.x,	BasePOS.y+m_Transform.y,	BasePOS.z+m_Transform.z	);

		::setCollisionLevel( hVis, m_nCollisionLevel );
	}

	return hVis;
}
void CFixedPART::UnloadVisible (HNODE hNODE)
{
	if ( m_HashZMOFile && hNODE ) {
		g_MotionFILE.Sub_DATAUseKEY( m_HashZMOFile );
		::unloadAnimatable ( hNODE );
		hNODE = NULL;
	}

	CBasicPART::UnloadVisible( hNODE );
}
/*
void  CFixedPART::ProcANI (HNODE hVisible)
{
	// degree x,y,z
	// rotateAxis 2.0  0 1 0   큰 날개.
	// rotateAxis 10.0  1 0 0  작은 팬
	
	if ( m_wAniTYPE & BIT_ROTAXIS ) {
		// degree, x, y, z
		::rotateAxis( hVisible,  m_RotAXIS.w, m_RotAXIS.x, m_RotAXIS.y, m_RotAXIS.z );
	}
}
*/

//-------------------------------------------------------------------------------------------------
