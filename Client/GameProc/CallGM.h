#ifndef _CALLGM_
#define _CALLGM_

#include <string>

///
/// 유져의 GM 호출 요청을 웹으로 전송( 각 국가별 게시판 )
///

class CCallGM
{
public:
	CCallGM(void);
	~CCallGM(void);

	void		CallGM( std::string& strServerName, std::string& strChannelName, std::string& strMsg  );
};

#endif //_CALLGM_
