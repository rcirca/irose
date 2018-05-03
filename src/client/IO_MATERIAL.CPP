/*
	$Header: /Client/IO_MATERIAL.CPP 46    04-07-25 3:28p Zho $
*/
#include "stdAFX.h"
#include "Game.h"
#include "IO_MATERIAL.h"
#include <assert.h>

//-------------------------------------------------------------------------------------------------
CMatLIST::CMatLIST (char *szNameTemplate) : CFileLIST<tagMATERIAL*>("MAT ")
{	
	m_NameTemplate.Set(szNameTemplate);	
}

CMatLIST::~CMatLIST ()
{
	this->Free ();
}
// "STB\\SYS_MATERIAL.STB"
bool CMatLIST::Load (char *szSTBFile, char *szNameTemplate)
{
	CGameSTB fSTB;

	m_NameTemplate.Set (szNameTemplate);

	if ( fSTB.Open (szSTBFile) ) {
		char *szFileName;

		Alloc_FILES( fSTB.GetRowCount () );

		for (short nI=0; nI<m_nDataCNT; nI++) {
			szFileName = fSTB.GetString(0, nI);
			if ( szFileName ) {
				tagMATERIAL *pMat;

				pMat = new tagMATERIAL;

				pMat->m_nAlpha    = (short) fSTB.GetInteger(2, nI);	// IsAlpha
				pMat->m_nTwoSide  = (short) fSTB.GetInteger(3, nI);	// Is2Side
				switch( fSTB.GetInteger(1, nI) ) {					// IsLinkBone
					case 1 :	pMat->m_hShader = g_GameDATA.m_hShader_nolit_skin;	break;
					case 2 :	pMat->m_hShader = g_GameDATA.m_hShader_terrain;		break;
					case 0 :	pMat->m_hShader = g_GameDATA.m_hShader_nolit;		break;
				}

				pMat->m_RealFileName.Set( szFileName );
				this->Add_FILE(szFileName, nI, pMat);
			}
		}

		fSTB.Close ();
		return true;
	}

	return false;
}
void CMatLIST::Free ()
{
	#pragma message ("TODO:: CMatLIST::Free() SAFE_DELETE( pData->m_DATA ); --> " __FILE__)

	CFileLIST< tagMATERIAL* >::Free ();
}

bool CMatLIST::Load_FILE (tagFileDATA<tagMATERIAL*> *pData)
{
	char *szName = CStr::Printf (m_NameTemplate.Get(), pData->m_nTag);

	_ASSERT( pData->m_DATA->m_hShader );

	pData->m_DATA->m_hMAT = ::loadColormapMaterial(szName, pData->m_DATA->m_hShader, pData->m_DATA->m_RealFileName.Get() );
	if ( pData->m_DATA->m_hMAT ) 
	{
		if (0 == pData->m_DATA->m_nAlphaTest) 
		{
			LogString ( LOG_DEBUG, "CMatLIST::Load_FILE[ %s / %s] donotuse_alphatest\n",
				pData->m_DATA->m_RealFileName.Get(), pData->m_FileName.Get());
		}

		::setMaterialUseAlpha		(pData->m_DATA->m_hMAT, pData->m_DATA->m_nAlpha);
		::setMaterialUseTwoSide		(pData->m_DATA->m_hMAT, pData->m_DATA->m_nTwoSide);
		::setMaterialUseAlphaTest	(pData->m_DATA->m_hMAT, pData->m_DATA->m_nAlphaTest);
        ::setMaterialAlphaRef		(pData->m_DATA->m_hMAT, pData->m_DATA->m_nAlphaRef);
        ::setMaterialUseSpecular	(pData->m_DATA->m_hMAT, pData->m_DATA->m_nSpecular);
        ::setMaterialAlphaValue	(pData->m_DATA->m_hMAT, pData->m_DATA->m_fAlphaValue);
        ::setMaterialGlow		(pData->m_DATA->m_hMAT, pData->m_DATA->m_nGlowType, pData->m_DATA->m_fGlowColor[0], pData->m_DATA->m_fGlowColor[1], pData->m_DATA->m_fGlowColor[2]);
        ::setMaterialZTest			(pData->m_DATA->m_hMAT, pData->m_DATA->m_nZTest);
        ::setMaterialZWrite			(pData->m_DATA->m_hMAT, pData->m_DATA->m_nZWrite);
        ::setMaterialBlendType		(pData->m_DATA->m_hMAT, pData->m_DATA->m_nBlendType);

		return true;
	} else {
		LogString ( LOG_DEBUG, "ERROR:: LoadMaterial:: %s \n", pData->m_DATA->m_RealFileName.Get());
	}

	return false;
}
void CMatLIST::Free_FILE (tagFileDATA<tagMATERIAL*> *pData)
{
	//LogString (LOG_DEBUG, "CMatLIST::Free_FILE[ %s / %s ] \n", pData->m_DATA->m_RealFileName.Get(), pData->m_FileName.Get() );
	
	if ( pData->m_DATA->m_hMAT ) 
	{
		::unloadMaterial( pData->m_DATA->m_hMAT );
		pData->m_DATA->m_hMAT = NULL;
	}
}
void CMatLIST::Mem_FREE (tagFileDATA<tagMATERIAL*> *pData)
{
	SAFE_DELETE( pData->m_DATA );
}

//-------------------------------------------------------------------------------------------------
/// @bug 파일이름으로 해쉬키를 생성해서.. 이미 테이블내에 존재하는지 서치해서
///      만약 존재한다면 그 해쉬키를 리턴하는데.. 만약 중복된 해쉬키가 생성됬다면
///      결과는 틀린것이다.
///			
///		 2003. 11. 25 아래부분 추가.
///      	이미있는 파일과 입력 파일의 이름이 같다는걸 보증해야한다..
///			if( !strcmp( szKeyName, pFindData->m_FileName.Get() ) )
///				return HashKEY;

t_HASHKEY CMatLIST::Add_MATERIAL (
	char *szFileName,
	short nIsSkin, short nIsAlpha, short nIs2Side,
	bool bTerrainMAT, char *szMatNameUseLightMap,
	short nAlphaTest, short nAlphaRef, short nZTest,
	short nZWrite, short nBlendType, short nSpecular, float fAlphaValue,
	short nGlowType,
	float * pfGlowColor
	)
{
	if ( (CVFSManager::GetSingleton()).IsExistFile( szFileName ) == false )	
	{
		char *szMSG = CStr::Printf ("ERROR Material File [%s] not found ... \n", szFileName);
		// g_pCApp->ErrorBOX(szMSG, "ERROR", MB_OK);
		LogString ( LOG_DEBUG, szMSG);
		return 0;
	}

	HNODE		 hShader;
	t_HASHKEY	 HashKEY;
	char	    *szKeyName;
	
	if ( szMatNameUseLightMap ) {
		szKeyName = szMatNameUseLightMap;

		hShader = g_GameDATA.m_hShader_lightmap;
	} else {
		if ( pfGlowColor && nGlowType ) {
			szKeyName = CStr::Printf ("%s_%d%d%d%d%4.3f%d<%4.3f,%4.3f,%4.3f>", szFileName, nIsSkin, nIsAlpha, nIs2Side, nSpecular, fAlphaValue, nGlowType, pfGlowColor[0], pfGlowColor[1], pfGlowColor[2]);
		}
		else {
			szKeyName = CStr::Printf ("%s_%d%d%d%d%4.3f", szFileName, nIsSkin, nIsAlpha, nIs2Side, nSpecular, fAlphaValue);
		}

		if ( bTerrainMAT ) {
			hShader = g_GameDATA.m_hShader_terrain;
		} else {
			if (nSpecular) {
				hShader = nIsSkin ? g_GameDATA.m_hShader_specular_skin : g_GameDATA.m_hShader_specular;
			}
			else {
				hShader = nIsSkin ? g_GameDATA.m_hShader_lit_skin : g_GameDATA.m_hShader_lit;
			}
		}
	}

	tagFileDATA< tagMATERIAL* > *pFindData;
	HashKEY = CStr::GetHASH(szKeyName);
	pFindData = this->KEY_Find_DATA( HashKEY );
	if ( pFindData ) 
	{
		/// 이미있는 파일과 입력 파일의 이름이 같다는걸 보증해야한다..
		if( !strcmpi( szKeyName, pFindData->m_FileName.Get() ) )
			return HashKEY;
		
		assert( 0 && "같은 키가 생성되서 문제가 생겼다." );		
	}

	tagMATERIAL *pMat = new tagMATERIAL;
	// 같은 파일에 대해서 skin하고 nIsAlpha등이 틀릴경우가 있으므로..
	pMat->m_RealFileName.Set( szFileName );

	HashKEY = this->Add_FILE(szKeyName, -1, pMat, false);
	if ( 0 == HashKEY ) {
		// 서로다른 파일이 같은 키를 갖고 있다...
		_ASSERT( HashKEY );
		delete pMat;
		return 0;
	}

	pMat->m_hShader    = hShader;
	pMat->m_nAlpha     = nIsAlpha;
	pMat->m_nTwoSide   = nIs2Side;
	pMat->m_nAlphaTest = nAlphaTest;
	pMat->m_nAlphaRef  = nAlphaRef;
	pMat->m_nZTest     = nZTest;
	pMat->m_nZWrite    = nZWrite;
	pMat->m_nBlendType = nBlendType;
	pMat->m_nSpecular  = nSpecular;
	pMat->m_fAlphaValue = fAlphaValue;
	pMat->m_nGlowType = nGlowType;

	if (pfGlowColor) {
		pMat->m_fGlowColor[0] = pfGlowColor[0];
		pMat->m_fGlowColor[1] = pfGlowColor[1];
		pMat->m_fGlowColor[2] = pfGlowColor[2];
	}
	else {
		pMat->m_fGlowColor[0] = 1.0f;
		pMat->m_fGlowColor[1] = 1.0f;
		pMat->m_fGlowColor[2] = 1.0f;
	}

	return HashKEY;
}

//-------------------------------------------------------------------------------------------------
HNODE CMatLIST::IDX_HNODE(short nIndex)
{
	tagMATERIAL *pMat;
	pMat = this->Get_DATAUseIDX( nIndex );

	if ( pMat ) return pMat->m_hMAT;
	return NULL;
}
HNODE CMatLIST::KEY_HNODE(t_HASHKEY	HashKEY)
{
	tagMATERIAL *pMat;
	pMat = this->Get_DATAUseKEY( HashKEY );
	if ( pMat ) {
		return pMat->m_hMAT;
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------

