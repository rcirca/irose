#include "stdafx.h"
#include ".\notifybuttondlg.h"

#include "../../Tutorial/TutorialEventUtility.h"
#include "../it_mgr.h"
#include "../../system/system_func.h"

#include "TButton.h"
#include <algorithm>
CNotifyButtonDlg::CNotifyButtonDlg(void)
{
	m_sPosition.x = 10;
	m_iWidth      = 390;
	assert( g_pCApp );
	if( g_pCApp )
		m_sPosition.y = g_pCApp->GetHEIGHT() - 290;
}

CNotifyButtonDlg::~CNotifyButtonDlg(void)
{
	Clear();
}
unsigned CNotifyButtonDlg::Process( unsigned uiMsg, WPARAM wParam ,LPARAM lParam )
{
	std::list< CTButton* >::iterator iter;
	for( iter = m_Children.begin(); iter != m_Children.end();++iter )
	{
		if( (*iter)->Process( uiMsg, wParam, lParam ) )
		{
			if( uiMsg == WM_LBUTTONUP )
			{
				//if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_NOTIFY ) )
				//{
				//	POINT pt = { (*iter)->GetPosition().x, (*iter)->GetPosition().y - pDlg->GetHeight() - 2 };			
				//	pDlg->MoveWindow( pt );
				//}

				CTutorialEventUtility::GetSingleton().ExecNotifyEvent( (*iter)->GetControlID() );
				delete *iter;
				m_Children.erase( iter );
				UpdatePosition();
			}
			return uiMsg;
		}
	}
	return 0;
}

void CNotifyButtonDlg::UpdatePosition()
{
	std::list< CTButton* >::iterator iter;
	int iCount = 0;

	m_sPosition.y = g_pCApp->GetHEIGHT() - 290;

	int x   = m_sPosition.x;
	int y   = m_sPosition.y;
	for( iter = m_Children.begin(); iter != m_Children.end();++iter , ++iCount )
	{
		(*iter)->SetPosition( x, y );
		x += (*iter)->GetWidth() + 2;
		if( x + (*iter)->GetWidth() + 2 > m_iWidth )
		{
			y -= (*iter)->GetHeight() + 2;
			x  = m_sPosition.x;
		}
	}
}

void CNotifyButtonDlg::AddButton( CTButton* pBtn )
{
	assert( pBtn );
	if( pBtn )
	{
		pBtn->Show();
		m_Children.push_back( pBtn );
		UpdatePosition();
	}
}

void CNotifyButtonDlg::Draw()
{
	for_each( m_Children.begin(), m_Children.end(), mem_fun( &CTButton::Draw ));
}

void CNotifyButtonDlg::Update( POINT ptMouse )
{
	for_each( m_Children.rbegin(), m_Children.rend(), bind2nd( mem_fun( &CTButton::Update), ptMouse ));
}

void CNotifyButtonDlg::Clear()
{
	std::list< CTButton* >::iterator iter;
	for( iter = m_Children.begin(); iter != m_Children.end();)
	{
		delete *iter;
		iter = m_Children.erase( iter );
	}
}