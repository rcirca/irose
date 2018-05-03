#include "StdAfx.h"
#include ".\tradiobox.h"
#include "TRadioButton.h"
CTRadioBox::CTRadioBox(void)
{
	Init( CTRL_RADIOBOX,0,0,0,0 );
	m_iPressedButton = 0;
}

CTRadioBox::~CTRadioBox(void)
{
	m_Children.clear();
}

void CTRadioBox::RegisterCtrl( CTRadioButton* pCtrl )
{
	pCtrl->SetRadioBox( this );
	m_Children.insert( RADIOBUTTON_MAP::value_type( pCtrl->GetControlID(), pCtrl ));
	//if( m_Children.size() == 1 )
	//{
	//	pCtrl->SetButtonState( CTRadioButton::BS_CLICKED );
	//	m_iPressedButton = pCtrl->GetControlID();
	//}
	//else
	pCtrl->SetButtonState( CTRadioButton::BS_CLICKED );
}

void CTRadioBox::UnregisterCtrl( CTRadioButton* pCtrl )
{
	RADIOBUTTON_MAP_ITOR iter = m_Children.find( pCtrl->GetControlID() );
	if( iter != m_Children.end() )
	{
		pCtrl->SetRadioBox( NULL );
		m_Children.erase( iter );
	}
	
}

void CTRadioBox::Show()
{
/*	RADIOBUTTON_MAP_ITOR iter;
	CTRadioButton* pButton = NULL;
	iter = m_Childs.begin();
	if( iter != m_Childs.end() )
	{
		iter->second->SetButtonState( CTRadioButton::BS_CLICKED );
		iter++;
	}

	for( ; iter != m_Childs.end(); ++iter )
		iter->second->SetButtonState( CTRadioButton::BS_NORMAL );
*/
}

bool CTRadioBox::SetPressedButton( unsigned int iID )
{
	RADIOBUTTON_MAP_ITOR iter;
	iter = m_Children.find( iID );
	if( iter == m_Children.end() )
		return false;
	
	iter->second->SetButtonState( CTRadioButton::BS_CLICKED );

	EventChangeSelect( iID );
	return true;
}

void CTRadioBox::EventChangeSelect( unsigned int iID )
{
	RADIOBUTTON_MAP_ITOR iter;
	CTRadioButton* pButton = NULL;
	
	m_iPressedButton = iID;

	for( iter = m_Children.begin(); iter != m_Children.end(); ++iter )
	{
		pButton = iter->second;
		if( pButton->GetControlID() != iID )
			pButton->SetButtonState( CTRadioButton::BS_NORMAL );

	}
}
