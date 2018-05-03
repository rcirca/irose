#include "stdafx.h"
#include ".\localizing.h"


/// for singleton
CLocalizing _localizing;


CLocalizing::CLocalizing(void)
{
	m_pTempWCBuf		= NULL;
	m_TempWCBufSize	= 0;
}

CLocalizing::~CLocalizing(void)
{
}

LANGID CLocalizing::GetCurrentLanguageID()
{
	// Populate s_Locale with the list of keyboard layouts.
    /*UINT cKL = GetKeyboardLayoutList( 0, NULL );    
    HKL *phKL = new HKL[cKL];
    if( phKL )
    {
        GetKeyboardLayoutList( cKL, phKL );
        for( UINT i = 0; i < cKL; ++i )
        {
            return PRIMARYLANGID( LOWORD( phKL[i] ) );
		}
	}*/

	HKL hKL = GetKeyboardLayout( 0 );    
	return LOWORD( hKL );
}

int	CLocalizing::GetCharsetFromLang( LANGID langid )
{
	switch( PRIMARYLANGID(langid) )
	{
		case LANG_JAPANESE:
			return SHIFTJIS_CHARSET;
		case LANG_KOREAN:
			return HANGEUL_CHARSET;
		case LANG_CHINESE:
		{
			switch( SUBLANGID(langid) )
			{
				case SUBLANG_CHINESE_SIMPLIFIED:
					return GB2312_CHARSET;
				case SUBLANG_CHINESE_TRADITIONAL:
					return CHINESEBIG5_CHARSET;
			}
		}
		case LANG_GREEK:
			return GREEK_CHARSET;
		case LANG_TURKISH:
			return TURKISH_CHARSET;
		case LANG_HEBREW:
			return HEBREW_CHARSET;
		case LANG_ARABIC:
			return ARABIC_CHARSET;
		case LANG_ESTONIAN:
		case LANG_LATVIAN:
		case LANG_LITHUANIAN:
			return BALTIC_CHARSET;
		case LANG_THAI:
			return THAI_CHARSET;
		case LANG_CZECH:
		case LANG_HUNGARIAN:
		case LANG_POLISH:
		case LANG_CROATIAN:
		case LANG_MACEDONIAN:
		case LANG_ROMANIAN:
		case LANG_SLOVAK:
		case LANG_SLOVENIAN:
			return EASTEUROPE_CHARSET;
		case LANG_RUSSIAN:
		case LANG_BELARUSIAN:
		case LANG_BULGARIAN:
		case LANG_UKRAINIAN:
			return RUSSIAN_CHARSET;
		case LANG_FRENCH:
			return ANSI_CHARSET; // GARNET ADDED
		default:
			return ANSI_CHARSET;
	}
}

int	CLocalizing::GetCodePageFromCharset( int charset )
{
	switch( charset )
	{
		case SHIFTJIS_CHARSET:
			return 932;
		case HANGUL_CHARSET:
			return 949;
		case GB2312_CHARSET:
			return 936;
		case CHINESEBIG5_CHARSET:
			return 950;
		case GREEK_CHARSET:
			return 1253;
		case TURKISH_CHARSET:
			return 1254;
		case HEBREW_CHARSET:
			return 1255;
		case ARABIC_CHARSET:
			return 1256;
		case BALTIC_CHARSET:
			return 1257;
		case THAI_CHARSET:
			return 874;
		case EASTEUROPE_CHARSET:
			return 1250;
		default:
			return 1252;
	}
}

int	CLocalizing::GetCodePageFromLang( LANGID langid )
{
	m_iCurrentCharSet = GetCharsetFromLang( langid );

	return GetCodePageFromCharset( m_iCurrentCharSet );
}

int	CLocalizing::GetCurrentCodePage()
{
	m_iCurrentCodePage = GetCodePageFromLang( GetCurrentLanguageID() );
	return m_iCurrentCodePage;
}

//--------------------------------------------------------------------------------
/// !! 주의 inBuf 는 변환후의 스트링을 담을 만큼 충분히 커야합니다.
//--------------------------------------------------------------------------------
int CLocalizing::UTF8ToMBCS( LPSTR inBuf, int iInBufSize )
{
	WCHAR outBuf[ 1024 ];

	int iWideCharCnt = MultiByteToWideChar( CP_UTF8, //When this is set, dwFlags must be zero.
											0, 
											inBuf, 
											iInBufSize, 
											outBuf, 
											0 );
	WCHAR* pOutBuf = NULL;
	int    iOutBufSize = 0;

	if( iWideCharCnt >= 1024 )
	{
		if( iWideCharCnt > m_TempWCBufSize )
		{
			if( m_pTempWCBuf )
			{
				delete[] m_pTempWCBuf;
				m_pTempWCBuf = NULL;
			}

			m_pTempWCBuf = new WCHAR[ iWideCharCnt + 10 ];
			m_TempWCBufSize = iWideCharCnt;
		}

		pOutBuf = m_pTempWCBuf;
		iOutBufSize = m_TempWCBufSize;

	}else
	{
		pOutBuf = outBuf;
		iOutBufSize = sizeof( outBuf );
	}

	/// UTF8 -> Unicode
	iWideCharCnt = MultiByteToWideChar( CP_UTF8, //When this is set, dwFlags must be zero.
											0, 
											inBuf, 
											iInBufSize, 
											pOutBuf, 
											iOutBufSize );

	/// Unicode -> MBCS
	return WideCharToMultiByte(	CP_ACP,
							0, 
							pOutBuf, 
							-1, 
							inBuf, 
							iInBufSize, 
							0, 
							0 );
}

int CLocalizing::MBCSToUTF8( LPSTR inBuf, int iInBufSize )
{
	WCHAR outBuf[ 1024 ];

/// MBCS -> Unicode
	int iWideCharCnt = MultiByteToWideChar( CP_ACP, //When this is set, dwFlags must be zero.
											0, 
											inBuf, 
											iInBufSize, 
											outBuf, 
											0 );

	WCHAR* pOutBuf = NULL;
	int    iOutBufSize = 0;

	if( iWideCharCnt >= 1024 )
	{
		if( iWideCharCnt > m_TempWCBufSize )
		{
			if( m_pTempWCBuf )
			{
				delete[] m_pTempWCBuf;
				m_pTempWCBuf = NULL;
			}

			m_pTempWCBuf = new WCHAR[ iWideCharCnt + 10 ];
			m_TempWCBufSize = iWideCharCnt;
		}

		pOutBuf = m_pTempWCBuf;
		iOutBufSize = m_TempWCBufSize;

	}else
	{
		pOutBuf = outBuf;
		iOutBufSize = sizeof( outBuf );
	}

	/// MBCS -> Unicode
	iWideCharCnt = MultiByteToWideChar( CP_ACP, //When this is set, dwFlags must be zero.
											0, 
											inBuf, 
											iInBufSize, 
											pOutBuf, 
											iOutBufSize );

	/// Unicode -> UTF8
	return WideCharToMultiByte(	CP_UTF8,
							0, 
							pOutBuf, 
							-1, 
							inBuf, 
							iInBufSize, 
							0, 
							0 );
}

int	CLocalizing::MBCSToUnicode( LPSTR inBuf, int iInBufSize, WCHAR* pOutBuf, int iOutBufSize )
{	
	/// UTF8 -> Unicode
	return MultiByteToWideChar( CP_ACP, //When this is set, dwFlags must be zero.
								0, 
								inBuf, 
								iInBufSize, 
								pOutBuf, 
								iOutBufSize );
}

/// iInBufSize 사이즈 귀찮으면 -1 로 넣으면 됩니다.
int	CLocalizing::UnicodeToMBCS( WCHAR* pInBuf, int iInBufSize, LPSTR OutBuf, int iOutBufSize )
{		
	/// Unicode -> MBCS
	return WideCharToMultiByte(	CP_ACP,
								0, 
								pInBuf, 
								-1, 
								OutBuf, 
								iOutBufSize, 
								0, 
								0 );
}
