#ifndef _CCursorStateDx_
#define _CCursorStateDx_
#include "ccursorstate.h"
#include "CCursor.h"

/**
* 윈도우 커서를 게임상황에 따라 변경하여 사용한다( 디폴트 )
*  - 만들당시 Znzin에서 처리하게끔 구현되었다가 문제점이 해결안되어 방식을 바꿈 -> ReloadCursor()로 해결
*
* @Author		최종진
*
* @Date			2005/9/6
*/


class CCursorStateDx : public CCursorState
{
public:
	CCursorStateDx(void);
	virtual ~CCursorStateDx(void);
	virtual void Draw( POINT ptMouse ){}
	virtual int SetCursorType( int iType );
	virtual void ReloadCursor();
private:
	void LoadCursor();
	void UnloadCursor();
	HCURSOR			m_hCursor[ CCursor::CURSOR_MAX ];
	HNODE			m_hDxCursor[ CCursor::CURSOR_MAX ];

};
#endif