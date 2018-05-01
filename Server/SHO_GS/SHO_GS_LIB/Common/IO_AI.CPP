
#include "stdAFX.h"
#ifndef	__SERVER
	#include "..\util\CGameSTB.h"
#else
	#include "..\util\classSTB.h"
#endif
#include "CObjCHAR.h"
#include "IO_AI.h"

//-------------------------------------------------------------------------------------------------
CAI_LIST::CAI_LIST ()
{
	m_ppAI = NULL;
	m_nAICNT = 0;
}
CAI_LIST::~CAI_LIST ()
{
	Free ();
}

//-------------------------------------------------------------------------------------------------
bool CAI_LIST::Load (char *szBaseDIR, char *szSTBFile, char *szLangSTB, int iLangCol)
{
	char *pFullPath;

	CGameSTB fSTB;

	if ( szBaseDIR ) {
		pFullPath = CStr::Printf ("%s%s", szBaseDIR, szSTBFile);
	} else
		pFullPath = szSTBFile;

	if ( fSTB.Open (pFullPath) ) {
		STBDATA	AILang;
		if ( szLangSTB ) {
			if ( szBaseDIR ) {
				pFullPath = CStr::Printf ("%s%s", szBaseDIR, szLangSTB);
			} else
				pFullPath = szLangSTB;

			AILang.LoadWSTB( false, pFullPath, -1, iLangCol, -1 );
		}

		char *szFileName;

		m_nAICNT = fSTB.GetRowCount();

		m_ppAI = new CAI_FILE* [ m_nAICNT ];
		for (short nI=0; nI<m_nAICNT; nI++) {
			m_ppAI[ nI ] = NULL;

			szFileName = fSTB.GetString(0, nI);
			if ( szFileName ) {
				if ( szBaseDIR )
					pFullPath = CStr::Printf ("%s%s", szBaseDIR, szFileName);
				else
					pFullPath = szFileName;

#ifndef	__SERVER
				if ( (CVFSManager::GetSingleton()).IsExistFile( pFullPath ) == true )				
#else
				if ( CUtil::Is_FileExist(pFullPath) )
#endif
				{
					m_ppAI[ nI ] = new CAI_FILE;
					if ( !m_ppAI[ nI ]->Load( pFullPath, &AILang, iLangCol ) ) {
						fSTB.Close();
						return false;
					}
				} 
#ifndef	__SERVER
				else {
					g_pCApp->ErrorBOX( pFullPath, "file not found", MB_OK);
				}
#endif
			}
		}
		AILang.Free ();
		fSTB.Close ();
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
void CAI_LIST::Free ()
{
	for (short nI=0; nI<m_nAICNT; nI++) {
		SAFE_DELETE( m_ppAI[ nI ] );
	}
	m_nAICNT = 0;
	SAFE_DELETE_ARRAY( m_ppAI );
}

//-------------------------------------------------------------------------------------------------
void CAI_LIST::AI_Created (int iAI_IDX, CObjCHAR *pSourCHAR)
{
	if ( iAI_IDX <= 0 ) return;
	if ( iAI_IDX >= m_nAICNT ) return;

	_ASSERT( m_ppAI[ iAI_IDX ] );

	// 처음 생성시
	m_ppAI[ iAI_IDX ]->AI_WhenCREATED( pSourCHAR );
}
void CAI_LIST::AI_Stop (int iAI_IDX, CObjCHAR *pSourCHAR)
{
	if ( iAI_IDX <= 0 ) 
		return;
	if ( iAI_IDX >= m_nAICNT ) 
		return;
	
	if ( !m_ppAI[ iAI_IDX ] ) return;

	// 정지 상태일때
	m_ppAI[ iAI_IDX ]->AI_WhenSTOP( pSourCHAR );
}

void CAI_LIST::AI_AttackMove (int iAI_IDX, CObjCHAR *pSourCHAR, CObjCHAR *pDestCHAR)
{
	if ( iAI_IDX <= 0 ) return;
	if ( iAI_IDX >= m_nAICNT ) return;

	if ( !m_ppAI[ iAI_IDX ] ) return;
	//	_ASSERT( m_ppAI[ iAI_IDX ] );

	// 공격 이동시
	m_ppAI[ iAI_IDX ]->AI_WhenAttackMOVE ( pSourCHAR, pDestCHAR );
}

void CAI_LIST::AI_Damaged (int iAI_IDX, CObjCHAR *pSourCHAR, CObjCHAR *pDestCHAR, int iDamage)
{
	if ( iAI_IDX <= 0 ) return;
	if ( iAI_IDX >= m_nAICNT ) return;

	if ( !m_ppAI[ iAI_IDX ] ) return;
	//	_ASSERT( m_ppAI[ iAI_IDX ] );

	// 타격 당했을때
	m_ppAI[ iAI_IDX ]->AI_WhenDAMAGED( pSourCHAR, pDestCHAR, iDamage );
}
void CAI_LIST::AI_Kill (int iAI_IDX, CObjCHAR *pSourCHAR, CObjCHAR *pDestCHAR, int iDamage)
{
	if ( iAI_IDX <= 0 ) return;
	if ( iAI_IDX >= m_nAICNT ) return;

	if ( !m_ppAI[ iAI_IDX ] ) return;
	//	_ASSERT( m_ppAI[ iAI_IDX ] );

	// 상대방을 죽였을때
	m_ppAI[ iAI_IDX ]->AI_WhenKILL( pSourCHAR, pDestCHAR, iDamage );
}
void CAI_LIST::AI_Dead (int iAI_IDX, CObjCHAR *pSourCHAR, CObjCHAR *pDestCHAR, int iDamage)
{
	if ( iAI_IDX <= 0 ) return;
	if ( iAI_IDX >= m_nAICNT ) return;

	if ( !m_ppAI[ iAI_IDX ] ) return;
	//	_ASSERT( m_ppAI[ iAI_IDX ] );

	// 자신이 죽을때
	m_ppAI[ iAI_IDX ]->AI_WhenDEAD( pSourCHAR, pDestCHAR, iDamage);
}

//-------------------------------------------------------------------------------------------------
