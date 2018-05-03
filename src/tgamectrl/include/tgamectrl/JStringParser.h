#ifndef _CJStringParser_
#define _CJStringParser_

#include "JString.h"
#include <vector>

/// Tag 파싱시 필요한 Data 구조체
struct TagInfo{
	DWORD	m_color;
	int		m_font;
	bool	m_linefeed;
	char*   m_pCurrChar;
};

//--------------------------------------------------------------------------------------------------------------------------------------
///	문자열 파싱 Class
///		1. 지원 TAG : 
///			- {BR}	: 줄넘김
///			- {B}	: 볼드		* 주의 : 클라이언트에서 볼드 폰트가 일반폰트 인덱스+3이라는 가정하에 구현되어 있다
///			- {FC=?}: 글자색변경
///	
///		2. 자동줄넘김
///			- 단어별(화이트 스페이스 사이의 문자열로 가정)
///			- 글자별
///
///		- 연관class			CJString
///
/// @Author				최종진
///
/// @Date				2005/08/30
//--------------------------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API CJStringParser
{
public:
	CJStringParser(void);
	~CJStringParser(void);
	int			SetString( const char* pszString ,int iDisplayWidth);
	CJString*	GetString( int index );
	int			GetStringCount();													
	void		Clear();

	void		SetDefaultFont( int iFont );									/// Default 폰트 Set
	void		SetDefaultColor( DWORD dwColor );								/// Default 폰트 색상 Set
	void		SetSplitType( int type );										/// 자동 줄넘김 타입을 Set

	enum{																		/// 자동 줄넘김 타입
		SPLIT_LETTER,															/// 문자별(Default)
		SPLIT_WORD,																/// 단어별
	};
protected:
	std::vector< CJString >	m_Strings;											/// 
	int			m_iDefaultFont;													/// Default 폰트
	int			m_split_type;													/// 줄넘김 타입
	DWORD		m_dwDefaultColor;												/// Default 폰트 색상


	/// 이하 Tag를 파싱하기 위한 Method들
	void		Tag( TagInfo& tag_info );
	void		TagStartProcess( std::string& Tag, TagInfo& tag_info );
	void		TagEndProcess( std::string& Tag, TagInfo& tag_info );
	enum{
		TAG,
		START_TAG,
		END_TAG
	};

	LPSTR		GetNextWord( WORD CodePage,LPSTR pCurrChar, DWORD dwFlags );
};
#endif