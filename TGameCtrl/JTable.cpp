#include "StdAfx.h"
#include ".\jtable.h"

CJTable::CJTable(void)
{
	m_iValue		= 0;
	m_iExtent		= 0;
	m_iColumnCount	= 1;
	m_iCellWidth	= 0;
	m_iCellHeight	= 0;
	m_iRowMargin	= 0;
	m_iColMargin	= 0;
	m_iSelectedItemID = 0;
	SetControlType( CTRL_TABLE );
}

CJTable::~CJTable(void)
{
	m_ViewItems.clear();

	std::vector< CWinCtrl* >::iterator iter;
	for( iter =	m_Items.begin(); iter != m_Items.end();)
	{
		delete *iter;
		iter = m_Items.erase( iter );
	}
}

/// CWinCtrl
void CJTable::Draw()
{
	if( !IsVision() ) return;
	std::vector< CWinCtrl* >::iterator iter;
	for( iter =	m_ViewItems.begin(); iter != m_ViewItems.end();++iter)
		(*iter)->Draw();
}

void CJTable::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	std::vector< CWinCtrl* >::iterator iter;
	for( iter =	m_ViewItems.begin(); iter != m_ViewItems.end();++iter)
		(*iter)->Update( ptMouse );
}

unsigned CJTable::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	unsigned uiProcID;
	std::vector< CWinCtrl* >::iterator iter;
	for( iter =	m_ViewItems.begin(); iter != m_ViewItems.end();++iter )
	{
		uiProcID = (*iter)->Process(uiMsg, wParam, lParam );
		if( uiProcID )
		{
			m_iSelectedItemID = uiProcID;
			return GetControlID();
		}
	}
	return 0;
}

/// IScrollModel
int CJTable::GetValue()
{
	return m_iValue;
}

int CJTable::GetExtent()
{
	return m_iExtent;
}

int CJTable::GetMaximum()
{
	_ASSERT( m_iColumnCount > 0 );
	if( m_iColumnCount <= 0 )
		return 0;

	int iRet = (int)m_Items.size() / m_iColumnCount;
	if( (int)m_Items.size() % m_iColumnCount )
		iRet++;

	return iRet;
}

int CJTable::GetMinimum()
{
	return 0;
}

void CJTable::SetValue( int iValue )
{
	_ASSERT( m_iColumnCount > 0 );
	if( m_iColumnCount <= 0 ) return;
	int iMaxView = m_iColumnCount * m_iExtent;

	if( iValue >= 0 && iValue <= (int)m_Items.size() / m_iColumnCount )
	{
		
		
		if( (int)m_Items.size() < iMaxView )
			m_iValue = 0;
		else if( (int)m_Items.size() - iValue * m_iColumnCount < iMaxView )
		{
			m_iValue = ( (int)m_Items.size() - iMaxView ) / m_iColumnCount;
			if( ( (int)m_Items.size() - iMaxView ) % m_iColumnCount )
				m_iValue++;
		}
		else
			m_iValue = iValue;
	}

	m_ViewItems.clear();
	int iCount	= 0;
	std::vector< CWinCtrl* >::iterator iter;
	POINT ptOffset = { 0, 0 };
	for( iter = m_Items.begin() + m_iValue * m_iColumnCount; iter != m_Items.end() && iCount < iMaxView ; ++iter, ++iCount )
	{
		ptOffset.x = ( m_iCellWidth + m_iColMargin ) * (iCount % m_iColumnCount);
		ptOffset.y = ( m_iCellHeight + m_iRowMargin ) * (iCount / m_iColumnCount);
		(*iter)->SetOffset( ptOffset );
		(*iter)->SetPosition( ptOffset.x + m_sPosition.x, ptOffset.y + m_sPosition.y );
		m_ViewItems.push_back( *iter );
	}
	
	///새로운 offset으로 실제 스크린좌표를 재계산한다.
	//MoveWindow( m_sPosition );
}

void CJTable::SetExtent( int iExtent )
{
	m_iExtent = iExtent;
}

void CJTable::SetMaximum( int iMaximum )
{

}

void CJTable::SetMinimum( int iMinimum )
{

}

RECT CJTable::GetWindowRect()
{
	RECT rc = { m_sPosition.x, m_sPosition.y, m_sPosition.x + m_iWidth, m_sPosition.y + m_iHeight };
	return rc;
}

void CJTable::SetColumnCount( int iCount )
{
	_ASSERT( iCount > 0 );
	if( iCount > 0 )
	{
		m_iColumnCount = iCount;
		SetValue( m_iValue );
		///=>스크롤바도 바꾸어 주어야 할텐데 : 
	}
}

void CJTable::SetCellWidth( int iWidth )
{
	m_iCellWidth = iWidth;
}

void CJTable::SetCellHeight( int iHeight )
{
	m_iCellHeight = iHeight;
}

void CJTable::SetRowMargin( int iMargin )
{
	m_iRowMargin = iMargin;
}

void CJTable::SetColMargin( int iMargin )
{
	m_iColMargin = iMargin;
}

void CJTable::Add( CWinCtrl* pCtrl )
{
	m_Items.push_back( pCtrl );
	SetValue( (int)m_Items.size() - 1);
}

void CJTable::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );
	std::vector< CWinCtrl* >::iterator iter;
	for( iter =	m_ViewItems.begin(); iter != m_ViewItems.end();++iter)
		(*iter)->MoveWindow( m_sPosition );
}

CWinCtrl* CJTable::GetItem( int iIndex )
{
	if( iIndex < 0 || iIndex >= (int)m_Items.size() )
		return NULL;
	return m_Items[iIndex];
}

int CJTable::GetSelectedItemID()
{
	return m_iSelectedItemID;
}

