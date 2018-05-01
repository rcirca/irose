#include "StdAfx.h"
#include ".\tsplitstring.h"
#include "ITFont.h"
#include "TControlMgr.h"


CTSplitString::CTSplitString(void)
{
	m_strNull = "";
}

CTSplitString::~CTSplitString(void)
{
}

CTSplitString::CTSplitString( int iFont, char* pszStr, int iDisplayWidth ,unsigned uiCodePage )
{
	Split( iFont, pszStr, iDisplayWidth, uiCodePage );
}


int	CTSplitString::GetLineCount()
{
	return m_StringList.size();
}
const char* CTSplitString::GetString( int iIndex )
{
	if( iIndex < 0 ||  iIndex >= (int)m_StringList.size() )
		return m_strNull.c_str();

	return m_StringList[ iIndex ].c_str();	
}


bool CTSplitString::Split( int iFont, char* pszStr, int iDisplayWidth ,unsigned uiCodePage )
{
	if( pszStr == NULL )
		return false;

	m_StringList.clear();

	std::string					strTemp;

	char* pCurrPos	= pszStr;
	char* pNextPos	= NULL;
	char* pPrevPos	= NULL;
	char* pStartPos = pCurrPos;
	char* pCurrPosBuf = NULL;
	SIZE sizeString;


	ITFont* pFont = CTControlMgr::GetInstance()->GetFontMgr();

	pNextPos = CharNextExA( (WORD)uiCodePage, pCurrPos, 0 );
	while( pCurrPos != pNextPos )
	{
		for( ; pCurrPos < pNextPos; ++pCurrPos )
			strTemp.push_back( *pCurrPos );

		sizeString = pFont->GetFontTextExtent( iFont, strTemp.c_str() );

		if( sizeString.cx > iDisplayWidth )
		{
			//hong geun.
			pCurrPosBuf = pCurrPos;			

			if( uiCodePage == 1252 )
			{
				while(true)
				{
					pPrevPos = CharPrevExA( uiCodePage, pStartPos, pCurrPosBuf, 0 );				
					pCurrPosBuf = pPrevPos;				
					
					if(	pPrevPos[0] == ' ' )
					{
						break;
					}

					if( strTemp.size() < (pCurrPos-pPrevPos) * 3 )
					{
						pPrevPos = CharPrevExA( uiCodePage, pStartPos, pCurrPos, 0 );
						break;
					}					
					//a-z
					//A-Z
					//if( !((pPrevPos[0] >= 97 && pPrevPos[0] <= 122) || (pPrevPos[0] >= 65 && pPrevPos[0] <= 90)) )
					//{
					//break;
					//}				
				}				
			}
			else
			{
				pPrevPos = CharPrevExA( uiCodePage, pStartPos, pCurrPos, 0 );				
			}			

			for( ;pCurrPos > pPrevPos; --pCurrPos )
			{
				strTemp = strTemp.erase( strTemp.size() - 1, 1 );
			}

			m_StringList.push_back( strTemp );

			strTemp.clear();
			pStartPos = pCurrPos;
		}

		pNextPos = CharNextExA( uiCodePage, pCurrPos, 0 );
	}

	if( !strTemp.empty() )
		m_StringList.push_back( strTemp );

	

	return true;
}

void CTSplitString::Clear()
{
	m_StringList.clear();
}

SIZE & CTSplitString::GetSizeText( int iFont, const char * szStr )
{		
	ITFont* pFont = CTControlMgr::GetInstance()->GetFontMgr();
	if( pFont )
	{
		m_szStrSize = pFont->GetFontTextExtent( iFont, szStr );		
	}	
	return m_szStrSize;
}