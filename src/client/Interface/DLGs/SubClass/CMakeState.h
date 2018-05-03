#ifndef _CMAKESTATE_
#define _CMAKESTATE_


class CMakeDLG;
/**
* 제조인터페이스 창의 State Base Class : 
*
* @Author		최종진
* @Date			2005/9/15
*/
class CMakeState
{
public:
	CMakeState(void);
	~CMakeState(void);

	virtual void Draw() = 0;
	virtual void Update( POINT ptMouse) = 0;
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void Init() = 0;
	virtual void MoveWindow(POINT ptPos ) = 0;
	virtual bool IsModal() = 0;
	virtual unsigned int Process(UINT uiMsg,WPARAM wParam,LPARAM lParam ) = 0;

protected:
	CMakeDLG*	m_pParent;

};
#endif