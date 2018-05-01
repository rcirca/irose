#ifndef _CCursorStateWindow_
#define _CCursorStateWindow_
#include "ccursorstate.h"


/**
* 윈도우 기본 커서를 변경없이 사용하는  State Class
* 
*
* @Author		최종진
*
* @Date			2005/9/6
*/
class CCursorStateWindow : public CCursorState
{
public:
	CCursorStateWindow(void);
	virtual ~CCursorStateWindow(void);
	virtual void Draw( POINT ptMouse ){}
	virtual int SetCursorType( int iType );

};
#endif