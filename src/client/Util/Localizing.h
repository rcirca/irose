#ifndef _LOCALIZING_
#define _LOCALIZING_


//-------------------------------------------------------------------------------------
/// Localizing manager
/// class CLocalizing
//-------------------------------------------------------------------------------------
class CLocalizing : public CJSingleton< CLocalizing >
{
private:
	int				m_iCurrentCodePage;
	int				m_iCurrentCharSet;

	WCHAR*			m_pTempWCBuf;
	int				m_TempWCBufSize;

public:
	CLocalizing(void);
	~CLocalizing(void);


	int		GetCurrentCodePageNO(){ return m_iCurrentCodePage; }
	int		GetCurrentCharSet(){ return m_iCurrentCharSet; }

	LANGID	GetCurrentLanguageID();	
	int		GetCharsetFromLang( LANGID langid );
	int		GetCodePageFromCharset( int charset );
	int		GetCodePageFromLang( LANGID langid );
	int		GetCurrentCodePage();


	//-----------------------------------------------------------
	/// UTF-8 converter
	//-----------------------------------------------------------
	int		UTF8ToMBCS( LPSTR inBuf, int iInBufSize );
	int		MBCSToUTF8( LPSTR inBuf, int iInBufSize );

	int		MBCSToUnicode( LPSTR inBuf, int iInBufSize, WCHAR* pwChar, int iOutBufSize );
	int		UnicodeToMBCS( WCHAR* pInBuf, int iInBufSize, LPSTR OutBuf, int iOutBufSize );
};

#endif //_LOCALIZING_