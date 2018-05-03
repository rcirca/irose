#ifndef _CTSplitString_
#define _CTSplitString_
#include "TGameCtrl.h"
#include <vector>
#include <string>
using namespace std;

/**
*	CSplitHangul과 유사하게 동작하지만 문자개수가 아닌 표시된 너비에 따라 스트링을 자르는 Class
*		- znzin에 실제 표시 길이를 query하기 때문에 느릴수 있다. 프레임마다 실행하지 않도록 유의할것
*
* @Author		최종진
* @Date			2005/8/30
*/

class TGAMECTRL_API CTSplitString
{
	vector< string >	m_StringList;	
	std::string		m_strNull;
	SIZE m_szStrSize;	

public:
	CTSplitString(void);
	CTSplitString( int iFont, char* pszStr, int iDisplayWidth ,unsigned uiCodePage );
	~CTSplitString(void);

	int	GetLineCount();
	const char* GetString( int iIndex );

	bool Split( int iFont, char* pszStr, int iDisplayWidth ,unsigned uiCodePage);
	void Clear();
	
	SIZE & GetSizeText( int iFont, const char * szStr );

};




#endif