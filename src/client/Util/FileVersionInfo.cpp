#include "stdafx.h"
#include ".\fileversioninfo.h"

CFileVersionInfo::CFileVersionInfo(void)
{
	m_iMajorVersion			= 0;
	m_MinorVersion			= 0;
	m_BuildNumber			= 0;
	m_RevisionNumber		= 0;
}

CFileVersionInfo::~CFileVersionInfo(void)
{
}

bool CFileVersionInfo::GetVersionInfo( const char* pFileName )
{
	DWORD dwHandle = 0, dwLen = 0;
	UINT BufLen = 0;
	LPTSTR lpData = 0, lpBuffer = 0;
	VS_FIXEDFILEINFO *pFileInfo = 0;	

	dwLen = GetFileVersionInfoSize ( pFileName, &dwHandle);		
	if (!dwLen)	
	{
		assert ( 0 && "VersionInfo           not found\n");
		return false;
	}
	lpData = (LPTSTR) new LPTSTR[ dwLen ];
	if (!lpData) 
	{		
		return false;
	}
	if (!GetFileVersionInfo ( pFileName, dwHandle, dwLen, lpData ) )
	{
		assert ( 0 && "GetFileVersionInfo failed\n");
		delete[] lpData;
		return false;
	}

	if (!VerQueryValue ( lpData, "\\", (LPVOID*) &pFileInfo, (PUINT) &BufLen ) ) 
	{
		assert ( 0 && "VersionInfo           not found\n");
	}
	else 
	{
		m_iMajorVersion		= HIWORD(pFileInfo->dwFileVersionMS);
		m_MinorVersion		= LOWORD(pFileInfo->dwFileVersionMS);
		m_BuildNumber		= HIWORD(pFileInfo->dwFileVersionLS);
		m_RevisionNumber	= LOWORD(pFileInfo->dwFileVersionLS);		
	}
	
	if (!VerQueryValue (lpData, TEXT("\\StringFileInfo\\040904E4\\FileVersion"),
			(LPVOID*) &lpBuffer, (PUINT) &BufLen)) 
	{
	    /* language ID 040904E4: U.S. English, char set = Windows, Multilingual */
		//assert ( 0 && "FileVersion           not found\n");		
	}
	else
	{
		m_strFileVersion = lpBuffer;		
	}

	if (!VerQueryValue (lpData, TEXT("\\StringFileInfo\\040904E4\\LibToolFileVersion"),
			(LPVOID*) &lpBuffer, (PUINT) &BufLen)) 
	{
	    /* language ID 040904E4: U.S. English, char set = Windows, Multilingual */
		//assert ( 0 && "LibToolFileVersion           not found\n");				
	}
	else
	{
		m_strLibToolFileVersion = lpBuffer;				
	}
	
	delete[] lpData;

	return true;
}