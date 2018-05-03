#include "StdAfx.h"
#include ".\tabbedpane.h"
#include "JContainer.h"
#include "TRadioButton.h"
#include "TRadioBox.h"
#include "TButton.h"
#include <algorithm>
//*------------------------------------------------------------------------------*/
///
//*------------------------------------------------------------------------------*/
CTabbedPane::CTabbedPane(void)
{
	SetRect( &m_rcMinimize,0,0,0,0 );
	m_bMinimize = false;
	m_pRadioBox = new CTRadioBox;
	m_pMinimizeButton = NULL;
	SetControlType( CTRL_TABBEDPANE );
}

CTabbedPane::~CTabbedPane(void)
{
	if( m_pMinimizeButton )
	{
		delete m_pMinimizeButton;
		m_pMinimizeButton = NULL;
	}
	if( m_pRadioBox )
	{
		delete m_pRadioBox;
		m_pRadioBox = NULL;
	}
}

unsigned int CTabbedPane::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( m_Tabs.empty() ) return 0;
	if( !IsVision() ) return 0;

	std::list< S_TabbedPane >::reverse_iterator riter;

	unsigned iID, iIndex = 1;
	for( riter = m_Tabs.rbegin(); riter != m_Tabs.rend(); ++riter, ++iIndex )
	{
		iID = riter->m_pRadioButton->Process( uiMsg, wParam, lParam );
		if( iID )
		{
			if( uiMsg == WM_LBUTTONDOWN )
				SetSelectedIndex( riter->m_iIndex );
			return iID ;
		}
	}

	if( m_pMinimizeButton && m_pMinimizeButton->Process( uiMsg, wParam, lParam ) )
		return m_pMinimizeButton->GetControlID();


	if( m_bMinimize )
		return 0;

	///1개이상이 있을경우에만
	_ASSERT( m_Tabs.size() );
	riter = m_Tabs.rbegin();

	return riter->m_pContainer->Process( uiMsg, wParam, lParam );
}

void CTabbedPane::Draw()
{
	if( m_Tabs.empty() ) return;
	if( !IsVision() ) return;

	std::list< S_TabbedPane >::iterator iter;
	for( iter = m_Tabs.begin(); iter != m_Tabs.end(); ++iter )
		iter->m_pRadioButton->Draw();

	if( m_pMinimizeButton )
		m_pMinimizeButton->Draw();

	if( m_bMinimize )
		return;

	std::list< S_TabbedPane >::reverse_iterator riter = m_Tabs.rbegin();
	riter->m_pContainer->Draw();
}


void CTabbedPane::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );
	if( m_Tabs.empty() ) return;

	///버튼들은 다 이동시키고
	std::list< S_TabbedPane >::iterator iter;
	for( iter = m_Tabs.begin(); iter != m_Tabs.end(); ++iter )
		iter->m_pRadioButton->MoveWindow( m_sPosition );

	if( m_pMinimizeButton )
		m_pMinimizeButton->MoveWindow( m_sPosition );

	if( m_bMinimize )
		return;

	///Container는 현재 활성화(선택된)것만
	std::list< S_TabbedPane >::reverse_iterator riter = m_Tabs.rbegin();
	riter->m_pContainer->MoveWindow( m_sPosition );
}

void CTabbedPane::Update( POINT ptMouse )
{
	if( m_Tabs.empty() ) return;
	if( !IsVision() ) return;

	///버튼들은 다 이동시키고
	std::list< S_TabbedPane >::iterator iter;
	for( iter = m_Tabs.begin(); iter != m_Tabs.end(); ++iter )
		iter->m_pRadioButton->Update( ptMouse );

	if( m_pMinimizeButton )
		m_pMinimizeButton->Update( ptMouse );

	if( m_bMinimize )
		return;

	///Container는 현재 활성화(선택된)것만
	std::list< S_TabbedPane >::reverse_iterator riter = m_Tabs.rbegin();
	riter->m_pContainer->Update( ptMouse );
}


unsigned	 CTabbedPane::AddTab(CTRadioButton* pButton, CJContainer* pContainer )
{
	_ASSERT( pButton );
	_ASSERT( pContainer );

	S_TabbedPane Pane;
 
	Pane.m_iIndex		= m_Tabs.size();
	Pane.m_pRadioButton = pButton;
	Pane.m_pContainer	= pContainer;

	///라디오박스에 등록
	m_pRadioBox->RegisterCtrl( pButton );

	m_Tabs.push_back( Pane );
	return (unsigned)m_Tabs.size();
}

CJContainer* CTabbedPane::GetTabContainer( int iIndex )
{
	std::list< S_TabbedPane >::iterator iter;
	for( iter = m_Tabs.begin(); iter != m_Tabs.end(); ++iter )
	{
		if( iter->m_iIndex == iIndex )
			return iter->m_pContainer;
	}
	return NULL;
}
void CTabbedPane::RemoveAll()
{
	CTRadioButton*	pRadioButton	= NULL;
	CJContainer*	pContainer		= NULL;
	std::list< S_TabbedPane >::iterator iter;
	//int iCount = 0;
	for( iter = m_Tabs.begin(); iter != m_Tabs.end();)
	{
		pRadioButton = iter->m_pRadioButton;
		pContainer	 = iter->m_pContainer;

		iter = m_Tabs.erase( iter );

		///라디오박스에서 뺀다.
		m_pRadioBox->UnregisterCtrl( pRadioButton );

		delete pRadioButton;
		delete pContainer;
	}
}

int	 CTabbedPane::GetSelectedIndex()
{
	if( m_Tabs.empty() )
		return -1;

	std::list< S_TabbedPane >::reverse_iterator riter = m_Tabs.rbegin();
	return riter->m_iIndex;
}

bool CTabbedPane::SetSelectedIndex( unsigned index )
{
	if( index >= m_Tabs.size() )
	{
		_ASSERT( 0 && "Invalid Subscript @CTabbedPane::SetSelectedIndex" );	
		return false;
	}

	std::list< S_TabbedPane >::iterator iter;
	for( iter = m_Tabs.begin(); iter != m_Tabs.end(); ++iter)
	{
		S_TabbedPane Pane = *iter;
		if( iter->m_iIndex == (int)index  )
		{
			m_Tabs.erase( iter );
			
			///활성화되어 있지 않은 상태에서 this가 움직였을경우 Position이 Update되지 않는다.
			///이를 선택되었을때마다 보정해준다.
			Pane.m_pRadioButton->MoveWindow( m_sPosition );
			Pane.m_pContainer->MoveWindow( m_sPosition );
			m_pRadioBox->SetPressedButton( Pane.m_pRadioButton->GetControlID() );
			m_Tabs.push_back( Pane );
			return true;
		}
	}
	return false;
}

void CTabbedPane::SetMinimize( bool bMinimize )
{
	m_bMinimize = bMinimize;
	if( !bMinimize && !m_Tabs.empty() )
	{
		std::list< S_TabbedPane >::reverse_iterator riter =  m_Tabs.rbegin();
		riter->m_pRadioButton->MoveWindow( m_sPosition );
		riter->m_pContainer->MoveWindow( m_sPosition );
	}
}

void	CTabbedPane::SetMinimizeRect( const RECT& rc )
{
	m_rcMinimize = rc;
}

bool	CTabbedPane::IsMinimize()
{
	return m_bMinimize;
}

bool CTabbedPane::IsInside( int x, int y )
{
	///최소화 상태일경우에는 해당 영역만큼만 처리할수 있게 한다.
	if( m_bMinimize )
	{
		RECT  rc = { m_sPosition.x + m_rcMinimize.left,
					 m_sPosition.y + m_rcMinimize.top,
					 m_sPosition.x + m_rcMinimize.right,
					 m_sPosition.y + m_rcMinimize.bottom
		};

		POINT pt = { x, y };
		return PtInRect( &rc, pt )?true:false;
	}
	return CWinCtrl::IsInside( x, y );
}

void CTabbedPane::SetMinimizeButton( CTButton* pButton )
{
	_ASSERT( pButton == NULL );
	m_pMinimizeButton = pButton;
}

void CTabbedPane::Show()
{
	CWinCtrl::Show();
	std::list< S_TabbedPane >::iterator iter;
	for( iter = m_Tabs.begin(); iter != m_Tabs.end(); ++iter)
	{
		iter->m_pRadioButton->Show();
		iter->m_pContainer->Show();
	}
}

void CTabbedPane::Hide()
{
	CWinCtrl::Hide();
	std::list< S_TabbedPane >::iterator iter;
	for( iter = m_Tabs.begin(); iter != m_Tabs.end(); ++iter)
	{
		iter->m_pRadioButton->Hide();
		iter->m_pContainer->Hide();
	}
}
