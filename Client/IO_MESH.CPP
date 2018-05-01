/*
	$Header: /Client/IO_MESH.CPP 10    04-04-15 1:35p Jeddli $
*/
#include "stdAFX.h"
#include "IO_MESH.h"

//-------------------------------------------------------------------------------------------------
bool CMeshLIST::Load_FILE (tagFileDATA<HNODE> *pHNODE)
{
	char *szName = CStr::Printf (NAME_MESH, pHNODE->m_nTag);

	pHNODE->m_DATA = ::loadMesh(szName, pHNODE->m_FileName.Get() );

	if ( pHNODE->m_DATA == NULL ) {
		int kkkk = 0;
	}

	return ( pHNODE->m_DATA != NULL );
}
void CMeshLIST::Free_FILE (tagFileDATA<HNODE> *pHNODE)
{
	if ( pHNODE->m_DATA ) 
	{
		//LogString ( LOG_DEBUG, "Free Mesh :: %s \n", pHNODE->m_FileName.Get() );
		::unloadMesh( pHNODE->m_DATA );
		pHNODE->m_DATA = NULL;
	}
}

HNODE CMeshLIST::KEY_HNODE(t_HASHKEY HashKEY)
{	
	HNODE hResult;

	hResult = this->Get_DATAUseKEY(HashKEY);
	if ( hResult == NULL ) {
		tagFileDATA< HNODE > *pData;
		
		pData = this->KEY_Find_DATA( HashKEY );
		if ( pData ) {
			LogString ( LOG_DEBUG, "Mesh ERROR:: %s ", pData->m_FileName.Get() );

			if ( (CVFSManager::GetSingleton()).IsExistFile( pData->m_FileName.Get() ) == true )			
				LogString (LOG_DEBUG, " in exist file\n" );
			else
				LogString (LOG_DEBUG, " file not found \n" );
		}
	}

	return hResult;
}

//-------------------------------------------------------------------------------------------------
