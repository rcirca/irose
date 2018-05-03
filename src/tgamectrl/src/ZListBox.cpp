#include "StdAfx.h"
#include ".\zlistbox.h"

CZListBox::CZListBox(void)
{
	SetControlType( CTRL_ZLISTBOX );
	m_iValue		= 0;
	m_iExtent		= 1;
}

CZListBox::~CZListBox(void)
{
}
int CZListBox::GetValue()
{
	return m_iValue;
}
int CZListBox::GetExtent()
{
	return m_iExtent;
}

int CZListBox::GetMaximum()
{
	return m_Items.size();
}

int CZListBox::GetMinimum()
{
	return 0;
}

void CZListBox::SetValue( int iValue )
{
	if( (int)m_Items.size() >= iValue )
	{
		m_iValue	= iValue;

		if( (int)m_Items.size() < m_iExtent )
			m_iValue = 0;
		else if( m_iValue > (int)m_Items.size() - m_iExtent )
			m_iValue = m_Items.size() - m_iExtent;

		if( m_iValue < 0 )
			m_iValue = 0;

		int iCount	 = 0;
		int iOffsetY = 0;
		std::deque<CWinCtrl*>::iterator iter;
		for( iter = m_Items.begin() + m_iValue; iter != m_Items.end() && iCount < m_iExtent; ++iCount, ++iter )
		{
			(*iter)->SetOffset( 0, iOffsetY );
			(*iter)->MoveWindow( m_sPosition );
			iOffsetY += (*iter)->GetHeight();
		}
	}
}

void CZListBox::SetExtent( int iExtent )
{
	m_iExtent = iExtent;
}

void CZListBox::SetMaximum( int iMaximum )
{
}
void CZListBox::SetMinimum( int iMinimium )
{
}

RECT CZListBox::GetWindowRect()
{
	RECT rc = { m_sPosition.x , m_sPosition.y, m_sPosition.x + m_iWidth, m_sPosition.y + m_iHeight };
	return rc;
}

void CZListBox::Draw()
{
	if( !IsVision() ) return;

	_ASSERT( (int)m_Items.size() >= m_iValue );
	if( (int)m_Items.size() >= m_iValue )
	{
		std::deque<CWinCtrl*>::iterator iter;
		int iCount = 0;
		for( iter = m_Items.begin() + m_iValue; iter != m_Items.end() && iCount < m_iExtent; ++iCount, ++iter )
			(*iter)->Draw();
	}
}

void CZListBox::MoveWindow( POINT pt )
{
	_ASSERT( (int)m_Items.size() >= m_iValue );
	if( (int)m_Items.size() >= m_iValue )
	{
		CWinCtrl::MoveWindow( pt );
		std::deque<CWinCtrl*>::iterator iter;
		int iCount   = 0;
		int iOffsetY = 0;
		for( iter = m_Items.begin() + m_iValue; iter != m_Items.end() && iCount < m_iExtent; ++iCount, ++iter )
		{
			(*iter)->SetOffset( 0, iOffsetY );
			(*iter)->MoveWindow( m_sPosition );
			iOffsetY += (*iter)->GetHeight();
		}
	}
}
void CZListBox::Add( CWinCtrl* pCtrl )
{
	_ASSERT( pCtrl );
	if( pCtrl )
	{
		pCtrl->SetParent( this );
		m_Items.push_back( pCtrl );
		SetValue( m_Items.size() - 1 );///맨밑으로 이동, 그렇지 않을시 경우에 따라 위치 재조정 필요
	}
}
void CZListBox::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	_ASSERT( (int)m_Items.size() >= m_iValue );
	if( (int)m_Items.size() >= m_iValue )
	{
		CWinCtrl::Update( ptMouse );
		std::deque<CWinCtrl*>::iterator iter;
		int iCount = 0;
		for( iter = m_Items.begin() + m_iValue; iter != m_Items.end() && iCount < m_iExtent; ++iCount, ++iter )
			(*iter)->Update( ptMouse );
	}
}
void CZListBox::Show()
{
	CWinCtrl::Show();
	std::deque<CWinCtrl*>::iterator iter;
	for( iter = m_Items.begin() ; iter != m_Items.end() ; ++iter )
		(*iter)->Show();
}
void CZListBox::Hide()
{
	CWinCtrl::Hide();
	std::deque<CWinCtrl*>::iterator iter;
	for( iter = m_Items.begin() ; iter != m_Items.end() ; ++iter )
		(*iter)->Hide();
}

unsigned CZListBox::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	_ASSERT( (int)m_Items.size() >= m_iValue );
	if( !IsInside( LOWORD(lParam), HIWORD( lParam) )) return 0;

	if( (int)m_Items.size() >= m_iValue )
	{
		std::deque<CWinCtrl*>::iterator iter;
		int iCount = 0;
		unsigned uiProcID = 0;
		for( iter = m_Items.begin() + m_iValue; iter != m_Items.end() && iCount < m_iExtent; ++iCount, ++iter )
			if( uiProcID = (*iter)->Process( uiMsg, wParam, lParam ) )
				return uiProcID;
	}
	return 0;
}

void CZListBox::Clear()
{
	CWinCtrl* pCtrl;
	std::deque<CWinCtrl*>::iterator iter;
	for( iter = m_Items.begin(); iter != m_Items.end();)
	{
		pCtrl = *iter;
		iter = m_Items.erase( iter );
		delete pCtrl;
	}
}

void CZListBox::SetSelected( int iIndex )
{
	_ASSERT( iIndex >= 0 && iIndex < (int)m_Items.size() );

	DeselectAll();
	if( iIndex >= 0 && iIndex < (int)m_Items.size() )
		m_Items[iIndex]->SetSelected();
}

int CZListBox::GetSelectedItemIndex()
{
	std::deque<CWinCtrl*>::iterator iter;
	int iIndex = 0;
	for( iter = m_Items.begin(); iter != m_Items.end();++iter, ++iIndex )
	{
		if( (*iter)->IsSelected() )
			return iIndex;
	}
	return -1;
}

///언젠가는 EventHandler를 구현해야 할텐테 언제 까지 이렇게 작성해야 할지..
void CZListBox::DeselectAll()
{
	std::deque<CWinCtrl*>::iterator iter;
	for( iter = m_Items.begin(); iter != m_Items.end(); ++iter)
		(*iter)->SetDeselected();
}
int CZListBox::GetSize()
{
	return m_Items.size();
}
CWinCtrl* CZListBox::GetItem( int iIndex )
{
	_ASSERT( iIndex >= 0 && iIndex < (int)m_Items.size() );
	if( iIndex >= 0 && iIndex < (int)m_Items.size() )
	{
		return m_Items[iIndex];
	}
	return NULL;
}

bool CZListBox::DelItem( int iIndex )
{
	_ASSERT( iIndex >= 0 && iIndex < (int)m_Items.size() );
	std::deque<CWinCtrl*>::iterator iter;
	std::deque<CWinCtrl*> tempContainer;
	int iCount = 0;
	for( iter = m_Items.begin(); iter != m_Items.end(); ++iter, ++iCount )
	{
		if( iCount == iIndex )
			delete *iter;
		else
			tempContainer.push_back( *iter );
	}

	swap( m_Items, tempContainer );
	SetValue( GetValue() );
	return true;
}
bool CZListBox::DelItemByControlID( int iID )
{
	std::deque<CWinCtrl*>::iterator iter;
	std::deque<CWinCtrl*> tempContainer;
	int iCount = 0;
	for( iter = m_Items.begin(); iter != m_Items.end(); ++iter, ++iCount )
	{
		if( iID == (*iter)->GetControlID() )
			delete *iter;
		else
			tempContainer.push_back( *iter );
	}

	swap( m_Items, tempContainer );
	SetValue( GetValue() );
	return true;
}
void CZListBox::InsertItem( int iIndex, CWinCtrl* pCtrl )
{
	_ASSERT( pCtrl && iIndex >= 0 && iIndex < (int)m_Items.size() );
	if( pCtrl == NULL || iIndex < 0 || iIndex >= (int)m_Items.size() ) return;

	std::deque<CWinCtrl*>::iterator iter;
	std::deque<CWinCtrl*> tempContainer;
	int iCount = 0;
	for( iter = m_Items.begin(); iter != m_Items.end(); ++iter, ++iCount )
	{
		if( iCount == iIndex )
		{
			pCtrl->SetParent( this );
			tempContainer.push_back( pCtrl );
		}
		tempContainer.push_back( *iter );
	}

	swap( m_Items, tempContainer );	

}