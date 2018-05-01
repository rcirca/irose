#ifndef _CCursorState_
#define _CCursorState_

class CCursorState
{
public:
	CCursorState(void);
	virtual ~CCursorState(void);
	virtual void Enter();
	virtual void Leave();
	virtual void Draw( POINT ptMouse ) = 0;
	virtual int  SetCursorType( int iType ) = 0;
	virtual void ReloadCursor(){};
	int		GetState();
protected:
	int		m_iState;
};
#endif