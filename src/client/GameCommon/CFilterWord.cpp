#include "stdafx.h"
#include ".\cfilterword.h"
#include <algorithm>
#include "../util/Localizing.h"
#include "../Misc/GameUtil.h"
CFilterWord::CFilterWord(void)
{
	m_wstrChangeWord = L"*";
}

CFilterWord::~CFilterWord(void)
{

}

CFilterWord& CFilterWord::GetInstance()
{
	static CFilterWord s_Instance;
	return s_Instance;
}
 
void CFilterWord::AddWord( char* pszWord , char* pszChangeWord)
{
	if( pszWord )
	{
		strupr( pszWord );
		std::wstring wstrWord;
		if( MulityByte2WideString( pszWord, wstrWord ) )
		{
			if( pszChangeWord && strlen( pszChangeWord ) >= 1 )
			{
				std::wstring wstrTemp;
				if( MulityByte2WideString( pszChangeWord, wstrTemp ) )
					m_BadWords.insert( std::map< wstring,wstring >::value_type( wstrWord, wstrTemp ) );
				else
					m_BadWords.insert( std::map< wstring,wstring >::value_type( wstrWord, m_wstrChangeWord) );
			}
			else
				m_BadWords.insert( std::map< wstring,wstring >::value_type( wstrWord, m_wstrChangeWord) );
		}
	}
}

void CFilterWord::SetChangeWord( const char* pszWord )
{
	if( pszWord )
		MulityByte2WideString( pszWord, m_wstrChangeWord );
}

bool CFilterWord::CheckString( char* pszString )
{
	if( pszString == NULL) return false;
	if( strlen(pszString) < 1 ) return false;

	m_strChangedString.clear();


	std::map< wstring, wstring >::iterator iter;


	int iStringLen = strlen(pszString);

	std::wstring	wstrChangeWord;
	std::wstring	wstrLastCheckedWord;
	std::wstring	wstrOutputString;

	wchar_t* pwszString = new wchar_t[ iStringLen + 1];
	ZeroMemory( pwszString, sizeof( wchar_t ) * ( iStringLen + 1 ) );

	int iConvertStringLen = MulityByte2WideString( pszString, pwszString , iStringLen );
	if( iConvertStringLen  == 0 )
		return false;


	wchar_t* pwszTemp = new wchar_t[ iConvertStringLen + 1];
	pwszTemp[iConvertStringLen] = 0;
	wcsncpy( pwszTemp, pwszString, iConvertStringLen );
	wcsupr( pwszTemp );
	std::wstring	wstrSourceString( pwszTemp );
	delete []pwszTemp;
	
	
	
	bool bRet			= true;
	bool bFoundBadWord	= false;

	for( int i = 0 ; i < (int)wstrSourceString.length() ; ++i )
	{
		bFoundBadWord = false;
		for( int j = 1; j <= iStringLen - i; ++j )
		{
			iter = m_BadWords.find( wstrSourceString.substr( i, j ) );
			if( iter != m_BadWords.end() )///Found
			{
				bRet			 = false;
				bFoundBadWord	 = true;
				wstrLastCheckedWord = iter->first;
				wstrChangeWord		= iter->second ;
			}
		}
		if( !bFoundBadWord )
		{
			wstrOutputString.push_back( pwszString[i] );
		}
		else
		{
			wstrOutputString.append( wstrChangeWord );
			i += wstrLastCheckedWord.size() - 1;
		}
	}

	delete []pwszString;

	Wide2MultyByteString( wstrLastCheckedWord, m_strCheckedWord );
	Wide2MultyByteString( wstrOutputString, m_strChangedString	);

	return bRet;
}

std::string& CFilterWord::GetChangedString()
{
	return m_strChangedString;
}
//*------------------------------------------------------------------------------/
/// 여러 단어가 걸릴경우 제일 마지막에 걸린 단어가 저장된다.
//*------------------------------------------------------------------------------/
std::string& CFilterWord::GetCheckedWord()
{
	return m_strCheckedWord;
}

int CFilterWord::MulityByte2WideString( const char* pszMultyByte, wstring& wstrWide )
{
	assert( pszMultyByte );
	if( pszMultyByte == NULL ) return 0;

	int iStrLen = strlen( pszMultyByte );
	wchar_t* pWideString = new wchar_t[ iStrLen + 1];
	ZeroMemory( pWideString, sizeof( wchar_t) * (iStrLen + 1) ) ;

	unsigned uiCodePage = CLocalizing::GetSingleton().GetCurrentCodePageNO();

	int iRet = MultiByteToWideChar( CP_ACP, 0, pszMultyByte, iStrLen, (LPWSTR)pWideString, iStrLen + 1);

	wstrWide.assign( pWideString );

	delete []pWideString;
	return iRet;
}

int CFilterWord::MulityByte2WideString( const char* pszMultyByte, wchar_t* pwszWide , int iBufSize )
{
	assert( pszMultyByte );
	if( pszMultyByte == NULL ) return 0;

	ZeroMemory( pwszWide, sizeof( wchar_t) * iBufSize ) ;

	unsigned uiCodePage = CLocalizing::GetSingleton().GetCurrentCodePageNO();

	int iStrLen = strlen( pszMultyByte );

	int iRet = MultiByteToWideChar( CP_ACP, 0, pszMultyByte, iStrLen, (LPWSTR)pwszWide, iBufSize );

	return iRet;
}

int CFilterWord::Wide2MultyByteString( wstring& wstrWide , string& strMultyByte )
{
	int iBufSize = wstrWide.length() * 3 + 1;

	char* pszBuf = new char[ iBufSize ];

	ZeroMemory( pszBuf, sizeof( char) * iBufSize );

	unsigned uiCodePage = CLocalizing::GetSingleton().GetCurrentCodePageNO();

	BOOL bChangedDefaultChar;
	int  iRet = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)wstrWide.c_str(), wstrWide.length(), pszBuf, iBufSize,".", &bChangedDefaultChar );

	strMultyByte = pszBuf;

	delete []pszBuf;
	return iRet;
}

//----------------------------------------------------------------------------------------------------
/// @brief Unicode로부터 해당 언어인가를 판별한다.
//----------------------------------------------------------------------------------------------------
bool CFilterWord::IsEnglish( wchar_t wchar )
{
	if( wchar >= 0x41 && wchar <= 0x5A )
		return true;

	if( wchar >= 0x61 && wchar <= 0x7A )
		return true;

	return false;
}

bool CFilterWord::IsNumber( wchar_t wchar )
{
	if( wchar >= 0x30 && wchar <= 0x39 )
		return true;

	return false;
}

bool CFilterWord::IsKorean( wchar_t wchar )
{
	if( wchar >= 0xAC00 && wchar <= 0xD7A3 )
		return true;

	//완성안된 자모는 false로 취급한다.
	return false;
}

///히라가나, 가타가나
bool CFilterWord::IsJapanese( wchar_t wchar )
{
	if( wchar >= 0x3040 && wchar <= 0x30FF )
		return true;

	return false;
}
///한자
bool CFilterWord::IsHanja( wchar_t wchar )
{
	if( wchar >= 0x4E00 && wchar <= 0x9FFF )
		return true;
	if( wchar >= 0xF900 && wchar <= 0xFAFF )
		return true;
	return false;
}

bool CFilterWord::IsValidName( char* szName )
{
	if( szName == NULL )
		return false;

//	CFilterWord& Filter = CFilterWord::GetInstance();

	if( /*!Filter.*/!CheckString( szName ) )
	{
		std::string strChangedString = GetChangedString();
		std::string strCheckedWord   = GetCheckedWord();
		return false;
	}

	if( !CheckName( szName ) )
		return false;

	//if( strstr( szName,"[GM]" ) || strstr( szName,"[gm]" ) || strstr( szName,"[Gm]") || strstr( szName,"[gM]") )
	//	return false;

	std::wstring wstrName;


	if( /*Filter.*/MulityByte2WideString( szName, wstrName ) )
	{
		WORD wPrimaryLang =  CGameUtil::GetPrimaryLanguage();

		//영어문자권에서는 영어, 숫자로만 쓸수있게
		//키보드 언어 셋팅과는 관계없이 클라빌드 버전으로 검사한다.
#if defined(_USA) || defined(_PHILIPPIN) || defined(_EU)
		return CheckEnglish( wstrName );		
#endif

		switch( wPrimaryLang )
		{
		case LANG_KOREAN:
			return CheckKorean( wstrName );
		case LANG_JAPANESE:
			return CheckJapanese( wstrName );
		case LANG_CHINESE:
			return CheckChinese( wstrName );
		default:
			{
				return CheckEnglish( wstrName );
			}
		}
	}
	return true;
}

bool CFilterWord::CheckKorean( std::wstring& wstrName )
{
	CFilterWord& Filter = CFilterWord::GetInstance();
	std::wstring::iterator iter;

	for( iter = wstrName.begin(); iter != wstrName.end(); ++iter )
	{
		if( !Filter.IsEnglish( *iter ) &&
			!Filter.IsNumber( *iter ) &&
			!Filter.IsKorean( *iter ) )
			return false;
	}
	return true;
}

bool CFilterWord::CheckJapanese( std::wstring& wstrName )
{
	CFilterWord& Filter = CFilterWord::GetInstance();
	std::wstring::iterator iter;

	for( iter = wstrName.begin(); iter != wstrName.end(); ++iter )
	{
		if( !Filter.IsEnglish( *iter ) &&
			!Filter.IsNumber( *iter ) &&
			!Filter.IsJapanese( *iter ) &&
			!Filter.IsHanja( *iter ) )
			return false;
	}
	return true;
}

bool CFilterWord::CheckChinese( std::wstring& wstrName )
{
	CFilterWord& Filter = CFilterWord::GetInstance();
	std::wstring::iterator iter;
	for( iter = wstrName.begin(); iter != wstrName.end(); ++iter )
	{
		if( !Filter.IsEnglish( *iter ) &&
			!Filter.IsNumber( *iter ) &&
			!Filter.IsHanja( *iter ) )
			return false;
	}
	return true;
}

bool CFilterWord::CheckEnglish( std::wstring& wstrName )
{
	CFilterWord& Filter = CFilterWord::GetInstance();
	std::wstring::iterator iter;

	for( iter = wstrName.begin(); iter != wstrName.end(); ++iter )
	{
		if( !Filter.IsEnglish( *iter ) &&
			!Filter.IsNumber( *iter ) )
			return false;
	}
	return true;
}

void CFilterWord::AddName( char* pszName )
{
	assert( pszName );
	if( pszName )
	{
		strupr( pszName );
		std::wstring wstrName;
		if( MulityByte2WideString( pszName, wstrName ) )
			m_BadNames.push_back( wstrName );
	}
}

bool CFilterWord::CheckName( char* pszName )
{
	bool bRet = true;
	assert( pszName );
	if( pszName )
	{
		int iLen = strlen( pszName);
		char* pszTemp = new char[ iLen  + 1];
		strcpy( pszTemp, pszName );
		pszTemp[ iLen ] = 0;
		strupr( pszTemp );

		std::wstring wstrName;
		if( MulityByte2WideString( pszTemp, wstrName ) )
		{
			std::list< wstring >::iterator iter;
			for( iter = m_BadNames.begin(); iter != m_BadNames.end(); ++iter )
			{
				if( wcsstr( wstrName.c_str(), iter->c_str() ) )
				{
					bRet = false;
					break;
				}
			}
		}
		delete []pszTemp;
	}
	return bRet;
}
//----------------------------------------------------------------------------------------------------