#ifndef _CCommunity_
#define _CCommunity_



/// 현재 (2005/9/15) 사용안하는 Class
/// 삭제 필요
class CCommunity
{
	CCommunity(void);
	~CCommunity(void);
public:
	static CCommunity& GetInstance();
};
#endif