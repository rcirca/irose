/*
	$Header: /Client/Util/CRangeTBL.cpp 5     03-11-27 3:05p Jeddli $
*/
#include "stdAFX.h"
#include "Game.h"
#include "CRangeTBL.h"
#include "Util\\VFSManager.h"

CRangeTBL* CRangeTBL::m_pInstance = NULL;

//-------------------------------------------------------------------------------------------------
CRangeTBL::CRangeTBL ()
{
	m_pTABLE	= NULL;
	m_pPOINTS	= NULL;
	m_pIndexCnt	= NULL;
	m_pStartIdx	= NULL;
}
CRangeTBL::~CRangeTBL ()
{
	Free_TABLE ();
}
CRangeTBL* CRangeTBL::Instance ()
{
	if ( m_pInstance == NULL )
		m_pInstance = new CRangeTBL;

	return m_pInstance;
}
void CRangeTBL::Destroy ()
{
	SAFE_DELETE( m_pInstance );
}

//-------------------------------------------------------------------------------------------------
//
// Sight table
//
bool CRangeTBL::Load_TABLE (char *szFileName )
{	
	short nI;	

	CFileSystem* pFileSystem = (CVFSManager::GetSingleton()).GetFileSystem();
	if( pFileSystem->OpenFile( szFileName ) == false )	
	{		
		char *szStr = CStr::Printf ("File [%s] open error ", szFileName );
		g_pCApp->ErrorBOX(szStr, "ERROR", MB_OK);
		return false;
	}    

	pFileSystem->ReadInt16( &m_nMaxRange );

	m_pStartIdx = new short [ m_nMaxRange ];
	m_pIndexCnt = new short [ m_nMaxRange ];
	for (nI=0; nI<m_nMaxRange; nI++) {
		pFileSystem->ReadInt16( &m_pStartIdx[ nI ] );
		pFileSystem->ReadInt16( &m_pIndexCnt[ nI ] );
	}


	pFileSystem->ReadInt16( &m_nMaxArray);

	m_pPOINTS = new POINTS [ m_nMaxArray ];
	for (nI=0; nI<m_nMaxArray; nI++) 
	{
		pFileSystem->ReadInt16( &m_pPOINTS[ nI ].x );
		pFileSystem->ReadInt16( &m_pPOINTS[ nI ].y );
	}

	pFileSystem->CloseFile();
	(CVFSManager::GetSingleton()).ReturnToManager( pFileSystem );
	

	short nR, nIdx;

	m_pTABLE = new char [ m_nMaxRange  * m_nMaxRange  ];
	for (nI=0; nI<m_nMaxRange * m_nMaxRange; nI++)
		m_pTABLE[ nI ] = m_nMaxRange+1;

	for (nR=0; nR<m_nMaxRange; nR++) {
		nIdx = m_pStartIdx[ nR ];

		for (nI=0; nI<m_pIndexCnt[ nR ]; nI++, nIdx++) {
			if ( m_pPOINTS[ nIdx ].x < 0 || m_pPOINTS[ nIdx ].y < 0 ) 
				continue;

			m_pTABLE[ m_nMaxRange*m_pPOINTS[ nIdx ].y + m_pPOINTS[ nIdx ].x ] = (char)nR;
		}
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
void CRangeTBL::Free_TABLE (void)
{
	SAFE_DELETE_ARRAY( m_pTABLE );
	SAFE_DELETE_ARRAY( m_pPOINTS );
	SAFE_DELETE_ARRAY( m_pIndexCnt );
	SAFE_DELETE_ARRAY( m_pStartIdx );
}


//-------------------------------------------------------------------------------------------------
