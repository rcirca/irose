#include "StdAfx.h"
#include ".\tanimation.h"
#include "TFrame.h"
#include <Mmsystem.h>
CTAnimation::CTAnimation(void)
{
	m_curr_frame	= 0;
	m_loop			= true;
	m_prev_update_time = 0;
}

CTAnimation::~CTAnimation(void)
{
	std::deque< CTFrame* >::iterator iter;
	for( iter =	m_frames.begin(); iter != m_frames.end(); )
		iter = m_frames.erase( iter );
}

void CTAnimation::Show()
{
	CWinCtrl::Show();
	std::deque< CTFrame* >::iterator iter;
	for( iter =	m_frames.begin(); iter != m_frames.end(); ++iter)
		(*iter)->Show();
}

void CTAnimation::Hide()
{
	CWinCtrl::Hide();
	std::deque< CTFrame* >::iterator iter;
	for( iter =	m_frames.begin(); iter != m_frames.end(); ++iter)
		(*iter)->Hide();
}

void CTAnimation::Draw()
{
	if( !IsVision() || m_curr_frame <= 0 )
		return;

	_ASSERT( m_curr_frame >= 1 && m_curr_frame <= m_frames.size() );
	m_frames[ m_curr_frame - 1]->Draw();

}

void CTAnimation::Update( POINT ptMouse )
{
	if( m_curr_frame <= 0 ) return;

	DWORD curr_time = timeGetTime();
	
	if( curr_time - m_prev_update_time >= m_frames[ m_curr_frame - 1]->get_time() )
	{
		m_prev_update_time = curr_time;

		if( m_curr_frame < m_frames.size() )
			m_curr_frame++;
		else if( m_loop )
			m_curr_frame = 1;
	}
}

void CTAnimation::add_frame( CTFrame* frame )
{
	m_frames.push_back( frame );
}

void CTAnimation::set_loop( bool loop )
{
	m_loop = loop;
}

void CTAnimation::SetPosition( int x, int y )
{
	CWinCtrl::SetPosition( x, y );
	std::deque< CTFrame* >::iterator iter;
	for( iter =	m_frames.begin(); iter != m_frames.end(); ++iter)
		(*iter)->SetPosition( x, y );

}
void CTAnimation::set_curr_frame( unsigned i )
{
	_ASSERT( i >= 0 && i <= m_frames.size() );
	m_curr_frame = i;
	m_prev_update_time = timeGetTime();
}
