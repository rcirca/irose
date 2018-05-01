#ifndef _CTAnimation_
#define _CTAnimation_

#include "winctrl.h"
#include <deque>

class CTFrame;

//---------------------------------------------------------------------------------------------------------------------
/// 2D 애니메이션 Class
///		- 클라이언트에서 미니맵의 Indicator에 사용되고 있다
///     - 연관Class						CTFrame
///
/// @Author				최종진
///
/// @Date				2005/8/30
//---------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API CTAnimation :	public CWinCtrl
{
public:
	CTAnimation(void);
	virtual ~CTAnimation(void);
	virtual void Show();
	virtual void Hide();
	virtual void Draw();
	virtual void Update( POINT ptMouse );

	void	SetPosition( int x, int y );									/// Draw 할 화면 좌표 Set
	void	add_frame( CTFrame* frame );									/// Frame추가
	void	set_loop( bool loop );											/// 애니메이션의 Loop 옵션 Set 
	void	set_curr_frame( unsigned i );									/// Frame의 강제 이동

		

protected:
	 
	std::deque< CTFrame* >::size_type						m_curr_frame;	/// 현재 Draw할 Frame의 인덱스
	bool					m_loop;											/// 애니메이션의 Loop On/Off
	std::deque< CTFrame* >	m_frames;										/// Frame Container
	DWORD					m_prev_update_time;								/// Update된 바로 이전의 시간
};
#endif