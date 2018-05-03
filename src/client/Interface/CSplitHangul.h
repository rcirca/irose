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

#include <string>
#include <vector>

using namespace std;

// 현재의 스트링이 한글이 짤린 문장인가 판별..( IsDBCSLeadByte 동작이 불분명함.. )
inline BOOL IsDBCSTrailByte(char *base, char *p)
 {
        int lbc = 0; // lead byte count

        while (p > base)
        {
                if (!IsDBCSLeadByte(*(--p)))
                        break;
                lbc++;  
        }

        return (lbc & 1);
}


#define MAX_BUF_SIZE	1024
#define MAX_STACK_DEPTH	50

class CSplitHangul
{
	vector< string >	m_StringList;	
	char				m_TempBuf[ MAX_BUF_SIZE ];

	int					m_iStackDepth;

public:
	CSplitHangul();
	CSplitHangul( char* Str, int iLength );

	~CSplitHangul();
	

	int	GetLineCount( ){ return (int)m_StringList.size(); };
	const char* GetString( int iIndex )
	{
		if( iIndex >= m_StringList.size() )
			return NULL;

		return m_StringList[ iIndex ].c_str();	
	}


	bool DoSplit( char* Str, int iLength );

	// 한글을 주어진 크기로 자른다..
	bool SplitHangul( char* Str, int iLength );
	void Clear();
};




#endif //_SPLITHANGUL_