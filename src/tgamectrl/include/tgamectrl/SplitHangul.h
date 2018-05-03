/////////////////////////////////////////////////////////////////////////////////////////////
//
//	주어진 스트링을 주어진 길이의 스트링으로 분할하는 클래스
//	한글의 경우 정확한 분할이 필요하다..
//
//	사용법 : CSplitHangul( char* Str, int iLength )로 생성해서 사용하거나..
//			 변수 선언후.. DoSplit( char* Str, int iLength ); 사용..
//			  분리후에는 GetLineCount 로 라인 개수를 얻고..
//			  각 라인에 대한 스트링은 GetString( Index ) 로 얻는다.
//
//	수정을 하셨거나.. 버그를 발견하시면.. jeddli 한테 애기해주세요...
//
//		2002. 9. 13		jeddli
//
/////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _SPLITHANGUL_
#define _SPLITHANGUL_
#include "TGameCtrl.h"
#include <string>
#include <vector>

using namespace std;

class TGAMECTRL_API CSplitHangul
{
	vector< string >	m_StringList;	
	std::string			m_strNull;
public:
	CSplitHangul();
	CSplitHangul( char* Str, int iLength ,unsigned uiCodePage );

	~CSplitHangul();
	
	int	GetLineCount();
	const char* GetString( int iIndex );

	bool DoSplit( char* pStr, int iLength ,unsigned uiCodePage);

	// 한글을 주어진 크기로 자른다..
	bool SplitHangul( char* pStr, int iLength ,unsigned uiCodePage);
	void Clear();
};

#endif //_SPLITHANGUL_