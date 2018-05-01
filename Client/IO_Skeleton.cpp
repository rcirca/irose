/*
	$Header: /Client/IO_Skeleton.cpp 14    03-12-01 11:11a Jeddli $
*/
#include "stdAFX.h"
#include "IO_Skeleton.h"
//-------------------------------------------------------------------------------------------------
CSKELETON::~CSKELETON ()
{
	this->Free ();
}
// "STB\\SYS_SKELETON.STB"
// *.ZMD
bool CSKELETON::Load (char *szSTBFile)
{
	CGameSTB fSTB;

	if ( fSTB.Open (szSTBFile) ) {
		short nColCnt;
		char *szFileName;

		nColCnt		= (short)fSTB.GetColumnCount ();

		Alloc_FILES( fSTB.GetRowCount () );

		m_ppDATA    = new short* [ m_nDataCNT ];

		for (short nY=0; nY<m_nDataCNT; nY++) {
			m_ppDATA[ nY ] = new short[ fSTB.GetColumnCount() ];

			szFileName = fSTB.GetString(0, nY);
			if ( szFileName ) {
				/*
				szName = CStr::Printf (NAME_SKEL, nY);
				m_phNODE[ nY ] = loadSkeleton(szName, szFileName);
				*/
				this->Add_FILE(szFileName, nY);

				for (short nX=1; nX<nColCnt; nX++) {
					m_ppDATA[ nY ][ nX ] = (short) fSTB.GetInteger( nX, nY );
				}
			}
		}

		fSTB.Close ();
		return true;
	}

	return false;
}
void CSKELETON::Free ()
{
	short nI;

	CFileLIST< HNODE >::Free ();

	if ( m_ppDATA ) {
		for (nI=0; nI<m_nDataCNT; nI++)
			SAFE_DELETE_ARRAY( m_ppDATA[ nI ] );

		SAFE_DELETE_ARRAY( m_ppDATA );
	}
}

//-------------------------------------------------------------------------------------------------
t_HASHKEY CSKELETON::Add_SKELETON (char *szFileName)
{
	t_HASHKEY HashKEY = CStr::GetHASH(szFileName);

	LogString (LOG_DEBUG, "SKELETON ::[ %s ] \n", szFileName);

	if ( (CVFSManager::GetSingleton()).IsExistFile( szFileName ) == false )	
	{
		char *szMSG = CStr::Printf ("Skeleton File [%s] not found ...", szFileName);
		g_pCApp->ErrorBOX( szMSG, "ERROR", MB_OK );
		return HashKEY;
	}

	tagFileDATA<HNODE> *pDATA = this->KEY_Find_DATA( HashKEY );
	if ( pDATA == NULL )
		this->Add_FILE(szFileName, -1);

	return HashKEY;
}


//-------------------------------------------------------------------------------------------------
bool CSKELETON::Load_FILE (tagFileDATA<HNODE> *pHNODE)
{
	char *szName = CStr::Printf (NAME_SKEL, pHNODE->m_nTag);
	pHNODE->m_DATA = ::loadSkeleton(szName, pHNODE->m_FileName.Get());
	return ( pHNODE->m_DATA != NULL );
}
void CSKELETON::Free_FILE (tagFileDATA<HNODE> *pHNODE)
{
	if ( pHNODE->m_DATA ) {
		::unloadSkeleton( pHNODE->m_DATA );
		pHNODE->m_DATA = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
