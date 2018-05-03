#include "StdAfx.h"
#include ".\jcontainer.h"
#include "WinCtrl.h"
#include <algorithm> 

CJContainer::CJContainer(void)
{
}

CJContainer::~CJContainer(void)
{

}

void CJContainer::Draw()
{
	std::list< CWinCtrl* >::iterator iter;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
		(*iter)->Draw();
}

void CJContainer::Update( POINT ptMouse )
{
	for_each( m_Children.begin(), m_Children.end(), bind2nd( mem_fun( &CWinCtrl::Update ), ptMouse ) );
}

unsigned CJContainer::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	unsigned uiProcID = 0;
	std::list< CWinCtrl* >::reverse_iterator riter;
	for( riter = m_Children.rbegin(); riter != m_Children.rend(); ++riter)
	{
		uiProcID = (*riter)->Process( uiMsg, wParam, lParam );
		if( uiProcID )
		{
			CWinCtrl* pCtrl = *riter;
			if( uiMsg == WM_LBUTTONDOWN )
				MoveCtrl2ListEnd( pCtrl->GetControlID() );

			return uiProcID;
		}
	}
	return 0;
}

void CJContainer::MoveCtrl2ListEnd( int iID )
{
	std::list< CWinCtrl* >::iterator iter;
	for( iter = m_Children.begin(); iter != m_Children.end(); ++iter )
	{
		if( (int)(*iter)->GetControlID() == iID )
		{
			CWinCtrl* pCtrl = *iter;
			m_Children.erase( iter );
			m_Children.push_back( pCtrl );
			return;
		}
	}
}

void CJContainer::MoveWindow( POINT pt )
{
	std::list< CWinCtrl* >::iterator iter;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
		(*iter)->MoveWindow( pt );
}

void CJContainer::Show()
{
	std::list< CWinCtrl* >::iterator iter;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
		(*iter)->Show();
}
void CJContainer::Hide()
{
	std::list< CWinCtrl* >::iterator iter;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
		(*iter)->Hide();
}

CWinCtrl*	CJContainer::Find( unsigned index )
{
	std::list< CWinCtrl* >::iterator iter;
//	CWinCtrl*	pCtrl = NULL;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
	{
		if( (*iter)->GetControlID() == index )
			return *iter;
	}
	return NULL;
}

CWinCtrl*	CJContainer::Find( const char * szName )
{
	std::list< CWinCtrl* >::iterator iter;
	
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
	{
		CWinCtrl * ctrl = *iter;
		if( strcmp( ctrl->GetControlName(), szName ) == 0 )		
			return *iter;
	}
	return NULL;
}

void CJContainer::SetChildEnable( unsigned iID, bool bEnable )
{
	CWinCtrl* pCtrl = Find( iID );
	if( pCtrl )
		pCtrl->SetEnable( bEnable );
}
void CJContainer::Add( CWinCtrl* pCtrl )
{
	_ASSERT( pCtrl );
	m_Children.push_back( pCtrl );
}

void CJContainer::Remove( unsigned index )
{
	std::list< CWinCtrl* >::iterator iter;
	CWinCtrl*	pCtrl = NULL;
	for( iter =	m_Children.begin(); iter != m_Children.end(); ++iter)
	{
		if( (*iter)->GetControlID() == index )
		{
			pCtrl = *iter;
			iter = m_Children.erase( iter );
			delete pCtrl;
			return;
		}
	}
}

void CJContainer::RemoveAll()
{
	std::list< CWinCtrl* >::iterator iter;
	CWinCtrl*	pCtrl = NULL;
	for( iter =	m_Children.begin(); iter != m_Children.end(); )
	{
		pCtrl = *iter;
		iter = m_Children.erase( iter );
		delete pCtrl;
	}
}