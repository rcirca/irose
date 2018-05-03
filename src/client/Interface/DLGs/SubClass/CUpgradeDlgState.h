#ifndef _CUpgradeDlgState_
#define _CUpgradeDlgState_

/**
* 제련인터페이스 창의 State Base Class : 
*
* @Author		최종진
* @Date			2005/9/15
*/
class CUpgradeDlgState
{
public:
	CUpgradeDlgState(void);
	virtual ~CUpgradeDlgState(void);
	virtual void Enter() = 0;
	virtual void Leave() = 0;
	virtual void Update( POINT ptMouse ) = 0;
	virtual void Draw() = 0;
	virtual void MoveWindow( POINT ptPosition ){}
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam ) = 0;
};
#endif