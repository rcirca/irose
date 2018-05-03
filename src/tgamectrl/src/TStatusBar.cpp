#include "StdAfx.h"
#include ".\tstatusbar.h"
#include "TImage.h"
#include "TButton.h"
CTStatusBar::CTStatusBar(void)
{
	m_pImage = NULL;
}

CTStatusBar::~CTStatusBar(void)
{
	if( m_pImage )
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

void CTStatusBar::SetImage( CTImage* pImage )
{
	if( m_pImage )
		delete m_pImage;
	m_pImage = pImage;
}

void CTStatusBar::Draw()
{
	if( m_pImage )
		m_pImage->Draw();
}

void CTStatusBar::Update( POINT pt )
{
	if( m_pImage )
		m_pImage->Update( pt );
}

void CTStatusBar::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );
	if( m_pImage )
		m_pImage->MoveWindow( m_sPosition );
}