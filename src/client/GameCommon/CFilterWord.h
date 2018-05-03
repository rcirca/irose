#ifndef _CFilterWord_
#define _CFilterWord_
#include <map>
#include <string>

using namespace std;

class CFilterWord
{
	CFilterWord(void);
	~CFilterWord(void);
public:
	static CFilterWord& GetInstance();

	void AddWord( char* pszWord , char* pszChangeWord);
	void AddName( char* pszName );

	void SetChangeWord( const char* pszWord );
	bool CheckString( char* pszString );
	bool CheckName( char* pszName );

	string&	GetChangedString();
	string&	GetCheckedWord();
	
	

	int MulityByte2WideString( const char* pszMultyByte, wstring& wstrWide );
	int MulityByte2WideString( const char* pszMultyByte, wchar_t* pwszWide , int iBufSize );

	int Wide2MultyByteString( wstring& wstrWide , string& strMultyByte );

	bool IsEnglish( wchar_t wchar );
	bool IsNumber( wchar_t wchar );
	bool IsKorean( wchar_t wchar );
	bool IsJapanese( wchar_t wchar );
	bool IsHanja( wchar_t wchar );

	bool IsValidName( char* szName );
	bool CheckKorean( std::wstring& wstrName );
	bool CheckJapanese( std::wstring& wstrName );
	bool CheckEnglish( std::wstring& wstrName );
	bool CheckChinese( std::wstring& wstrName );

private:

	string		m_strCheckedWord;
	string		m_strChangedString;
	
	wstring		m_wstrChangeWord;

	std::map< wstring,wstring > m_BadWords;
	std::list< wstring > m_BadNames;
};
#endif