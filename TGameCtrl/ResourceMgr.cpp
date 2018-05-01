#include "StdAfx.h"
#include ".\resourcemgr.h"
CResourceMgr* CResourceMgr::s_pInstance = NULL;
CResourceMgr::CResourceMgr(void)
{
}

CResourceMgr::~CResourceMgr(void)
{
	m_ResourceTable.clear();
}
void CResourceMgr::Destroy()
{
	if( s_pInstance )
	{
		delete s_pInstance;
		s_pInstance = NULL;
	}
}

CResourceMgr* CResourceMgr::GetInstance()
{
	if( s_pInstance == NULL)
		s_pInstance = new CResourceMgr;

	return s_pInstance;
}

int CResourceMgr::GetImageNID( int iModuleID, const char* szSID )
{
	TABLE_MODULE_ITER	iterModule;
	TABLE_IMAGE_ID_ITER iterImageID;

	iterModule = m_ResourceTable.find( iModuleID );
	if( iterModule == m_ResourceTable.end() )
		return -1;

//	char* pszSID = NULL;
	char szBuff[256] = {0};
	
	int iSize = (int)strlen( szSID );

	memcpy( szBuff, szSID, iSize);

	strupr( szBuff );

	iterImageID = iterModule->second.find( szBuff );
	if( iterImageID == iterModule->second.end() )
		return -1;
	
	return iterImageID->second;
}

bool CResourceMgr::AddResource( char* szFileName,int iModuleID )
{
	TABLE_MODULE_ITER	iterModule;
	iterModule = m_ResourceTable.find( iModuleID );
	if( iterModule != m_ResourceTable.end() )
	{
		char Buff[256];
		sprintf( Buff,"Duplicated Image Resource Module ID:[%d]",iModuleID );
		MessageBox(NULL,Buff,"Load Error",MB_OK);
		return false;
	}
	LoadResourceFile( szFileName, iModuleID );
	return true;
}
void CResourceMgr::UnLoadResource( int iModuleID )
{
	TABLE_MODULE_ITER	iterModule;
	iterModule = m_ResourceTable.find( iModuleID );
	if( iterModule == m_ResourceTable.end() )
	{
//		char Buff[256];
//		sprintf( Buff,"NotFound Image Resource Module ID:[%d]",iModuleID );
//		MessageBox(NULL,Buff,"Load Error",MB_OK);
		return;
	}
	iterModule->second.clear();
	m_ResourceTable.erase( iterModule );
}

void CResourceMgr::LoadResourceFile( char* szFileName, int iModuleID )
{
	TABLE_IMAGE_ID Table;
	
	FILE* pFile = fopen( szFileName,"r" );
	if( pFile == NULL )
	{
		char Buff[256];
		sprintf( Buff,"Not Found Resource File [%s]",szFileName );
		MessageBox(NULL,Buff,"Load Error",MB_OK);
		return;
	}
	char*	p	= NULL;
//	char*   pszSID = NULL;
	char	Buff[256];
	char	szBuf[512];
	const char	sep[] = "\t ";
	int		NID;
	while( NULL != fgets( Buff, sizeof(Buff), pFile ))
	{
		///대문자로 변경
		strupr( Buff );
		p = strtok( Buff, sep );///문자열 ImageID;

		ZeroMemory( szBuf, sizeof( szBuf ));
		strncpy( szBuf, p, strlen( p ) );

		p = strtok( NULL, sep );
		if( p == NULL )
			continue;

		NID = atoi( p );

		Table.insert( TABLE_IMAGE_ID::value_type( szBuf,NID ));
	}
	m_ResourceTable.insert( TABLE_MODULE::value_type( iModuleID, Table ));
	fclose( pFile );
}
