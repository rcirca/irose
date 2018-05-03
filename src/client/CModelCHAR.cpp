/*
	$Header: /Client/CModelCHAR.cpp 53    04-09-02 8:36p Jeddli $
*/
#include "stdAFX.h"
#include "IO_Basic.h"
#include "IO_Terrain.h"
#include "CModelCHAR.h"
#include "Util\\VFSManager.h"


CCharModelDATA	g_MOBandNPC;

//-------------------------------------------------------------------------------------------------
CCharMODEL::CCharMODEL ()
{
	m_bIsFemale = false;

	for (short nP=0; nP<MAX_BODY_PART; nP++)
		m_pBodyPART[ nP ] = NULL;

	m_nBodyPartCNT = 0;
	m_HashSkelFILE = 0;
	::ZeroMemory(m_BoneAniFILE, sizeof(t_HASHKEY) * MAX_MOB_ANI );

	m_nBoneEFFECT = 0;
	m_pBoneEFFECT = NULL;

	m_iRenderUnitOrder = 0;
}
CCharMODEL::~CCharMODEL ()
{
	SAFE_DELETE_ARRAY( m_pBoneEFFECT );
}

tagMOTION* CCharMODEL::GetMOTION (short nIndex)
{	
	tagMOTION *pData = g_MotionFILE.KEY_GetMOTION( m_BoneAniFILE[nIndex] );	
	
	if ( NULL == pData ) {
#ifdef	_DEBUG
		LogString (LOG_DEBUG, "(%s) Motion ERROR :: %s [ %d / %d ] \n", m_Name.Get(), g_MotionFILE.Get_FileNAME( m_BoneAniFILE[nIndex] ), m_BoneAniFILE[nIndex], nIndex );
#else
		LogString (LOG_DEBUG, "Motion ERROR :: %s [ %d / %d ] \n", g_MotionFILE.Get_FileNAME( m_BoneAniFILE[nIndex] ), m_BoneAniFILE[nIndex], nIndex );
#endif
	}

	return pData;
}

//-------------------------------------------------------------------------------------------------
bool CCharMODEL::SetPartMODEL (short nPartIdx, CMODEL<CCharPART> *pPartMODEL)
{
	if ( nPartIdx < 0 || nPartIdx >= MAX_BODY_PART )
		return false;

	m_pBodyPART[ nPartIdx ] = pPartMODEL;
	return true;
}

bool CCharMODEL::SetPartMODEL (short nPartIdx, short nModelIdx)
{
	if ( nModelIdx < 0 || nPartIdx < 0 || nPartIdx >= MAX_BODY_PART )
		return false;

	if ( g_DATA.m_pMD_CharPARTS[ m_bIsFemale ][ nPartIdx ] ) {
		m_pBodyPART[ nPartIdx ] = g_DATA.m_pMD_CharPARTS[ m_bIsFemale ][ nPartIdx ]->GetMODEL( nModelIdx );
		return true;
	}

	//_ASSERT( g_DATA.m_pMD_CharPARTS[ m_bIsFemale ][ nPartIdx ] );
	return false;
}

//-------------------------------------------------------------------------------------------------
void CCharMODEL::SetMeshMOTION (HNODE **pphPartVIS, short nAniIdx)
{
	HNODE hMotion;

	short nB, nU;
	for (nB=0; nB<MAX_BODY_PART; nB++) {
		if ( !m_pBodyPART[ nB ] || !pphPartVIS[ nB ] )
			continue;

		for (nU=0; nU<m_pBodyPART[ nB ]->m_nPartCNT; nU++) {
			if ( m_pBodyPART[ nB ]->m_pParts[ nU ].m_pMeshAniFILE ) {
				hMotion = g_MotionFILE.KEY_GetZMOTION( m_pBodyPART[ nB ]->m_pParts[ nU ].m_pMeshAniFILE[ nAniIdx ] );
				if ( hMotion ) {
					::attachMotion( pphPartVIS[ nB ][ nU ], hMotion );
				} else {
					::attachMotion( pphPartVIS[ nB ][ nU ], NULL );
				}
			}
		}
	}
}


///
/// .Txt 단위의 오브젝트 로드
///
HNODE *CCharMODEL::CreatePART (char *szName, HNODE hModelNODE, short nPartIdx, short nLinkDummyIdx)
{
	HNODE *pVIS=NULL;
	CMODEL<CCharPART> *pCharPART = m_pBodyPART[ nPartIdx ];

	if ( pCharPART  ) {
		short nI;
		char *pStr;

		if ( pCharPART->m_nPartCNT < 1 )
			return NULL;

		pVIS = new HNODE [ pCharPART->m_nPartCNT ];
		for (nI=0; nI<pCharPART->m_nPartCNT; nI++) 
		{
			pStr = CStr::Printf ("%s_%d_%d", szName, nPartIdx, nI);
			pVIS[ nI ] = pCharPART->m_pParts[ nI ].Load_ZMODEL( pStr, hModelNODE );
			if ( pVIS[ nI ] ) 
			{
				::setCollisionLevel( pVIS[ nI ], 0 );	// 캐릭터 충돌은 모델만 한다.
			}
			else /// 스키닝 되는 오브젝트일 경우..
			{
				m_RenderUnitPart[ nPartIdx ].push_back( m_iRenderUnitOrder );
				m_iRenderUnitOrder++;
			}
		}
	}

	return pVIS;
}

//-------------------------------------------------------------------------------------------------
void CCharMODEL::DeletePART (short nPartIdx, HNODE *pVIS)
{
	if ( NULL != pVIS ) 
	{
		for (short nI=0; nI<m_pBodyPART[ nPartIdx ]->m_nPartCNT; nI++) 
		{
			if ( pVIS[ nI ] != NULL ) 
				m_pBodyPART[ nPartIdx ]->m_pParts[ nI ].Free_ZMODEL( pVIS[ nI ] );
		}

		SAFE_DELETE_ARRAY( pVIS );
	}
}

void CCharMODEL::ClearRenderUnitParts()
{
	for( int i = 0; i < MAX_BODY_PART; i++ )
	{
		m_RenderUnitPart[ i ].clear();
	}	

	m_iRenderUnitOrder = 0;
}

//-------------------------------------------------------------------------------------------------
bool CCharMODEL::Load_MOBorNPC ( CFileSystem* pFileSystem, t_HASHKEY *pSkelKEY, t_HASHKEY *pAniKEY, t_HASHKEY *pEftKEY)
{
	char cIsValid;

	pFileSystem->ReadChar( &cIsValid );
	if ( cIsValid == 0 )
		return true;

	short nI, nIndex;

	// read skel index
	pFileSystem->ReadInt16( &nIndex );
	m_HashSkelFILE = pSkelKEY[ nIndex ];

	char *pStr = CGameStr::ReadString( pFileSystem );
#ifdef	_DEBUG
	m_Name.Set( pStr );
#endif

	// read data index
	pFileSystem->ReadInt16 ( &m_nBodyPartCNT );
	for (nI=0; nI<m_nBodyPartCNT; nI++) 
	{
		pFileSystem->ReadInt16( &nIndex );
		m_pBodyPART[ nI ] = g_DATA.m_ModelMOBnNPC.GetMODEL( nIndex );
	}

	LogString (LOG_DEBUG, "SKEL:: %s \n", g_DATA.m_SKELETON.Get_FileNAME( m_HashSkelFILE ) );

	// read ani index counter
	short nCnt, nAniIDX;
	pFileSystem->ReadInt16( &nCnt );
	for (nI=0; nI<nCnt; nI++) 
	{
		// nAniIDX == 0 인 정지가 안들어 온다..
		pFileSystem->ReadInt16( &nAniIDX );
		pFileSystem->ReadInt16( &nIndex );

		if ( nAniIDX < 0 ) {
			// 사용하지 않는다...
			continue;
		}

		m_BoneAniFILE[ nAniIDX ] = pAniKEY[ nIndex ];
		LogString (LOG_DEBUG, "		HashMotionFILE::%d: %s %d / %d \n", nIndex,  
			g_MotionFILE.Get_FileNAME( m_BoneAniFILE[nAniIDX] ), m_BoneAniFILE[ nAniIDX ], nAniIDX);
	}

	// read bone effect counter
	short nBoneIDX;
	pFileSystem->ReadInt16( &m_nBoneEFFECT );
	if ( m_nBoneEFFECT > 0 ) 
	{
		m_pBoneEFFECT = new tagBONE_EFFECT[ m_nBoneEFFECT ];
		for (nI=0; nI<m_nBoneEFFECT; nI++) 
		{
			pFileSystem->ReadInt16( &nBoneIDX );
			pFileSystem->ReadInt16( &nIndex );
			m_pBoneEFFECT[ nI ].m_cBoneIDX = (char) nBoneIDX;
			m_pBoneEFFECT[ nI ].m_EffectFILE = pEftKEY[ nIndex ];
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
CEffect** CCharMODEL::CreateBoneEFFECT (HNODE hMODEL)
{
//	return NULL;
	if ( m_nBoneEFFECT < 1 )
		return NULL;

	CEffect **ppEffect;
	ppEffect = new CEffect*[ m_nBoneEFFECT ];
	for (short nE=0; nE<m_nBoneEFFECT; nE++) 
	{
		ppEffect[ nE ] = g_pEffectLIST->Add_EFFECT( m_pBoneEFFECT[ nE ].m_EffectFILE );
		if ( ppEffect[ nE ] ) 
		{
			int iDummyCnt = ::getNumDummies( hMODEL );
			if ( iDummyCnt >= m_pBoneEFFECT[ nE ].m_cBoneIDX )
			{
				::linkDummy( hMODEL, ppEffect[ nE ]->GetZNODE(), m_pBoneEFFECT[ nE ].m_cBoneIDX );
				ppEffect[ nE ]->StartEffect();
				/*D3DVECTOR pos;
				::getPosition( hMODEL, (float*)(&pos) );
				ppEffect[ nE ]->GetPOSITION( &pos );				*/
			}
		}		
	}
	return ppEffect;
}

/// @Bug g_pEffectLIST->Add_EFFECT( .., default false ) 로 추가한 이펙트는 리스트에 추가되지 않기때문에 직접 지워야한다.
/// 
///
void CCharMODEL::DeleteBoneEFFECT (CEffect **ppEffect)
{
	if ( ppEffect ) {
		for (short nE=0; nE<m_nBoneEFFECT; nE++) 
		{
			///g_pEffectLIST->Del_EFFECT( ppEffect[ nE ] );
			ppEffect[ nE ]->StopEffect();
			if( ppEffect[ nE ] )
			{
				delete ppEffect[ nE ];
				ppEffect[ nE ] = NULL;
			}
			ppEffect[ nE ] = NULL;
		}
		SAFE_DELETE_ARRAY( ppEffect );
	}
}
void CCharMODEL::LinkBoneEFFECT (HNODE hMODEL, CEffect **ppEffect)
{
	for (short nE=0; nE<m_nBoneEFFECT; nE++) 
	{
		int iDummyCnt = ::getNumDummies( hMODEL );
		if ( iDummyCnt >= m_pBoneEFFECT[ nE ].m_cBoneIDX )
			::linkDummy( hMODEL, ppEffect[ nE ]->GetZNODE(), m_pBoneEFFECT[ nE ].m_cBoneIDX );		
		//ppEffect[ nE ]->LinkBONE( hMODEL, m_pBoneEFFECT[ nE ].m_cBoneIDX );
	}
}
void CCharMODEL::UnlinkBoneEFFECT (CEffect **ppEffect)
{
	for (short nE=0; nE<m_nBoneEFFECT; nE++) {
		ppEffect[ nE ]->UnlinkNODE ();
	}
}


//-------------------------------------------------------------------------------------------------
//
//
//
CCharModelDATA::CCharModelDATA ()
{
	m_pMODELS = NULL;
	m_nModelCNT = NULL;
}

CCharModelDATA::~CCharModelDATA ()
{
	this->Free ();
}

bool CCharModelDATA::Load_MOBorNPC (char *szFileName)
{
	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if ( pFileSystem->OpenFile( szFileName ) == false )
	{
		return false;
	}

	t_HASHKEY *pSkelKEY, *pAniKEY, *pEftKEY;
	char *pStr;
	short nI, nFileCNT;

	// skel file ...
	pFileSystem->ReadInt16( &nFileCNT );
	pSkelKEY = new t_HASHKEY[ nFileCNT ];
	for (nI=0; nI<nFileCNT; nI++) 
	{
		pStr = CGameStr::ReadString( pFileSystem );
		pSkelKEY[ nI ] = g_DATA.m_SKELETON.Add_SKELETON ( pStr );
	}

	// motion file ...
	pFileSystem->ReadInt16( &nFileCNT );
	pAniKEY = new t_HASHKEY[ nFileCNT ];
	for (nI=0; nI<nFileCNT; nI++) 
	{
		pStr = CGameStr::ReadString (pFileSystem);
		pAniKEY[ nI ] = g_MotionFILE.Add_FILE ( pStr );
	}

	// bone effect file ...
	pFileSystem->ReadInt16( &nFileCNT );
	pEftKEY = new t_HASHKEY[ nFileCNT ];
	for (nI=0; nI<nFileCNT; nI++) 
	{
		pStr = CGameStr::ReadString (pFileSystem);
		pEftKEY[ nI ] = g_pEffectLIST->Add_EffectFILE( pStr );
		LogString (LOG_DEBUG, "Bone EFFECT FILE : %d / %s \n", nI, pStr);
	}

	pFileSystem->ReadInt16( &m_nModelCNT );
	m_pMODELS = new CCharMODEL[ m_nModelCNT ];
	for (nI=0; nI<m_nModelCNT; nI++) 
	{
		m_pMODELS[ nI ].Load_MOBorNPC (pFileSystem, pSkelKEY, pAniKEY, pEftKEY);

		if ( NPC_R_WEAPON( nI ) ) {
			// 오른손 기본 무기
			m_pMODELS[ nI ].SetPartMODEL( BODY_PART_WEAPON_R, NPC_R_WEAPON( nI ) );
		}

		if ( NPC_L_WEAPON( nI ) ) {
			// 왼손 방패or무기
			m_pMODELS[ nI ].SetPartMODEL( BODY_PART_WEAPON_L, NPC_L_WEAPON( nI ) );
		}
	}

	SAFE_DELETE_ARRAY( pEftKEY  );
	SAFE_DELETE_ARRAY( pAniKEY  );
	SAFE_DELETE_ARRAY( pSkelKEY );

	pFileSystem->CloseFile();

	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );
	return true;
}
void CCharModelDATA::Free (void)
{
	SAFE_DELETE_ARRAY( m_pMODELS );
	m_nModelCNT = 0;
}


//-------------------------------------------------------------------------------------------------
