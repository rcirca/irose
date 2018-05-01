#include "StdAfx.h"
#include ".\tframe.h"
#include "TImage.h"
CTFrame::CTFrame(void)
{
	m_time	= 0;
	m_Image = NULL;
}

CTFrame::~CTFrame(void)
{
	if( m_Image )
	{
		delete m_Image;
		m_Image = NULL;
	}
}

DWORD CTFrame::get_time()
{
	return m_time;
}

void CTFrame::set_time( DWORD time )
{
	m_time = time;
}

void CTFrame::SetPosition( int x, int y )
{
	CWinCtrl::SetPosition( x, y );
	_ASSERT( m_Image );
	m_Image->SetPosition( x, y );
}

void CTFrame::Draw()
{
	_ASSERT( m_Image );
	m_Image->Draw();
}
void CTFrame::set_image( CTImage* image )
{
	_ASSERT( m_Image == NULL);
	m_Image = image;
}

void CTFrame::Show()
{
	CWinCtrl::Show();
	if( m_Image )
		m_Image->Show();
}
void CTFrame::Hide()
{
	CWinCtrl::Hide();
	if( m_Image )
		m_Image->Hide();
}
