#ifndef _CITState_
#define _CITState_
/**
* 게임월드에 접속후 IT_MGR(g_itMGR)의 State Class
*
* @Author		최종진
* @Date			2005/9/6
*/
class CITState
{
public:
	CITState(void);
	virtual ~CITState(void);
	virtual void Enter() = 0;
	virtual void Leave() = 0;
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam ) = 0;
	virtual void Update( POINT ptMouse ) = 0;

	/// @return bool default draw를 사용할것인가
	virtual bool Draw(){ return true; }

	int		GetID();
protected:
	int		m_iID;
};
#endif