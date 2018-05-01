#ifndef _CTFrame_
#define _CTFrame_

#include "winctrl.h"
class CTImage;

/**
* 2D 애니메이션에 사용되는 Frame Class
*	- CTImage와 표시되어질 시간을 가지고 있다
*   - 연관Class : CTAnimation
*
* @Author		최종진
*
* @Date			2005/8/30
*/
class TGAMECTRL_API CTFrame :	public CWinCtrl
{
public:
	CTFrame(void);
	virtual ~CTFrame(void);
	virtual void Show();
	virtual void Hide();
	virtual void Draw();
	void	SetPosition( int x, int y );
	DWORD	get_time();
	void	set_time( DWORD time );
	void	set_image( CTImage* image );

protected:
	CTImage*	m_Image;					 
	DWORD		m_time;						/// 화면에 출력되어질 시간( 밀리세컨드 )
	
};
#endif