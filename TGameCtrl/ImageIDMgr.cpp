#include "StdAfx.h"
#include ".\imageidmgr.h"

CImageIDMgr* CImageIDMgr::s_pInstance = NULL;

CImageIDMgr::CImageIDMgr(void)
{
}

CImageIDMgr::~CImageIDMgr(void)
{
	m_ResourceTable.clear();
}

void CImageIDMgr::Destroy()
{
	if( s_pInstance )
	{
		delete s_pInstance;
		s_pInstance = NULL;
	}
}

CImageIDMgr* CImageIDMgr::GetInstance()
{
	if( s_pInstance == NULL)
		s_pInstance = new CImageIDMgr;

	return s_pInstance;
}

int CImageIDMgr::GetImageNID( int iModuleID, const char* szSID )
{
	TABLE_MODULE_ITER	iterModule;
	TABLE_IMAGE_ID_ITER iterImageID;



	iterModule = m_ResourceTable.find( iModuleID );
	if( iterModule == m_ResourceTable.end() )
		return -1;

	///대문자로 변경하여 검색한다.
	string SID;
	char Buff[256] = {0};
	int iSize = (int)strlen( szSID );
	memcpy( Buff, szSID, iSize);

	for(int i = 0 ; i < iSize; ++i )
		Buff[i] = toupper( Buff[i] );

	SID = Buff;

	iterImageID = iterModule->second.find( SID );
	if( iterImageID == iterModule->second.end() )
		return -1;
	
	return iterImageID->second;
}

bool CImageIDMgr::AddResource( char* szFileName,int iModuleID )
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

void CImageIDMgr::LoadResourceFile( char* szFileName, int iModuleID )
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
	char*	p;
	char	Buff[256];
	char	sep[] = "\t ";
	string  SID;
	int		NID;
	while( NULL != fgets( Buff, sizeof(Buff), pFile ))
	{
		p = strtok( Buff, sep );///문자열 ImageID;

		int iSize = (int)strlen( p );
		for(int i = 0 ; i < iSize; ++i )
			*(p+i) = toupper( *(p+i) );

		SID = p;

		p = strtok( NULL, sep );
		if( p == NULL )
			continue;

		NID = atoi( p );

		Table.insert( TABLE_IMAGE_ID::value_type( SID,NID ));
	}
	m_ResourceTable.insert( TABLE_MODULE::value_type( iModuleID, Table ));
	fclose( pFile );
}
