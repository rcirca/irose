#ifndef _CJString_
#define _CJString_

#include "JLetter.h"
#include "TGameCtrl.h"
#include <list>

//------------------------------------------------------------------------------------------------------------------------
/// 문자열을  저장및 Draw하는 클래스
///
/// @Author				최종진
///
/// @Date				2005/08/30
//------------------------------------------------------------------------------------------------------------------------

class TGAMECTRL_API CJString
{
public:
	CJString(void);
	~CJString(void);
	void	AddString( const char* pszString, SIZE sizeString, int iFont, DWORD dwColor );
	void    Clear();
	void	Draw( int x, int y , bool bTransform, DWORD force_text_color = 0);
	bool	IsEmpty();
	int		GetStringHeight();

protected:
	int						m_iMaxHeight;													/// 문자열의 최대 높이 , AddString시에 주어진 sizeString을 비교해서 최대값을 저장한다.
	std::list< CJLetter >	m_Letters;

};
#endif