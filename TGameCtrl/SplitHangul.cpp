#include "StdAfx.h"
#include ".\splithangul.h"


CSplitHangul::CSplitHangul()
{
	m_strNull = "";	
}

CSplitHangul::CSplitHangul( char* Str, int iLength ,unsigned uiCodePage)
{		
	DoSplit( Str, iLength ,uiCodePage);
}

CSplitHangul::~CSplitHangul()
{
	m_StringList.clear();
}

bool CSplitHangul::DoSplit( char* pStr, int iLength ,unsigned uiCodePage )
{
	if( pStr == NULL )
		return false;

	m_StringList.clear();

	std::string					strTemp;

	char* pNextChar = pStr;
	char* pTemp		= NULL;;

	do{
		pTemp = CharNextExA( (WORD)uiCodePage, pNextChar, 0 );
		if( pTemp != pNextChar )
		{
			for( ;pNextChar < pTemp; ++pNextChar )
				strTemp.push_back( *pNextChar );

			if( (int)strTemp.size() >= iLength )
			{
				m_StringList.push_back( strTemp );
				strTemp.clear();
			}
		}
		
	}while( pTemp[0] );

	if( !strTemp.empty() )
		m_StringList.push_back( strTemp );

	return true;
}

void CSplitHangul::Clear()
{
	m_StringList.clear();
}

// 한글을 주어진 크기로 자른다..
bool CSplitHangul::SplitHangul( char* pStr, int iLength , unsigned uiCodePage )
{		
	if( pStr == NULL )
		return false;

	DoSplit( pStr, iLength , uiCodePage );
	return true;
}

int	CSplitHangul::GetLineCount( )
{ 
	return (int)m_StringList.size(); 
}

const char* CSplitHangul::GetString( int iIndex )
{
	if( iIndex < 0 ||  iIndex >= (int)m_StringList.size() )
		return m_strNull.c_str();

	return m_StringList[ iIndex ].c_str();	
}