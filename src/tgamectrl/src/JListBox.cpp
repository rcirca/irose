/// ---------------------------------------------------------------------------
///  Includes
/// ---------------------------------------------------------------------------
#include "StdAfx.h"
#include ".\jlistbox.h"
#include "tgamectrl/tcontainer.h"
#include "IListCellRenderer.h"
#include "TControlMgr.h"
#include "ITFont.h"
#include <assert.h>

CJListBox::CJListBox(void)
{
	m_pListCellRenderer = NULL;
	m_pContainer		= NULL;
	m_iCellHeight		= 20;
	m_iExtent			= 20;
	m_iValue			= 1;
}

CJListBox::~CJListBox(void)
{
	if( m_pListCellRenderer )
	{
		delete m_pListCellRenderer;
		m_pListCellRenderer = NULL;
	}

	if( m_pContainer )
	{
		delete m_pContainer;
		m_pContainer = NULL;
	}
}
/// ---------------------------------------------------------------------------
///  Overrides of the CWinCtrl method
/// ---------------------------------------------------------------------------
void CJListBox::Draw()
{
	if( !IsVision() ) return ;

	assert( m_pListCellRenderer );
	assert( m_pContainer );
	CWinCtrl*	pDrawCtrl			= NULL;
	const CTObject*	pObject			= NULL;
	POINT		ptDraw				= {0,0};

	bool		bSelectedItemIndex	= false;

	CTControlMgr::GetInstance()->GetFontMgr()->SetTransformSprite( (float)m_sPosition.x, (float)m_sPosition.y );

	
	std::deque< CTObject* >& ItemList = m_pContainer->GetItemList();
	if( (int)ItemList.size() < m_iValue )
	{
		_ASSERT( 0 && "Invalid Value @CJListBox::Draw" );
		return;
	}

	std::deque< CTObject* >::const_iterator citer;

	int iCount = 0;
	for( citer = ItemList.begin() + m_iValue - 1; citer != ItemList.end() && iCount < m_iExtent; ++citer,++iCount )
	{
		pObject	  = *citer;
		if( m_iValue + iCount == (int)m_pContainer->GetSelectedItemIndex() )
			bSelectedItemIndex = true;
		else
			bSelectedItemIndex = false;

		pDrawCtrl	= m_pListCellRenderer->GetListCellRendererComponent( pObject, bSelectedItemIndex, false );
		pDrawCtrl->SetPosition( ptDraw );
		pDrawCtrl->Draw();
		ptDraw.y += m_iCellHeight;
	}
}

unsigned int CJListBox::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() ) return 0;
	assert( m_pContainer );
	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };


	RECT rcHitTest;

	if( (uiMsg == WM_LBUTTONDOWN || uiMsg == WM_LBUTTONDBLCLK ) && IsInside( ptMouse.x, ptMouse.y ) )
	{
		for( int i = 0; i < (int)m_pContainer->GetSize() ; ++i )
		{
			rcHitTest.left		= m_sPosition.x ; 
			rcHitTest.top		= m_sPosition.y  + i * m_iCellHeight;
			rcHitTest.right		= rcHitTest.left + m_iWidth;
			rcHitTest.bottom	= rcHitTest.top  + m_iCellHeight;
			if( PtInRect( &rcHitTest, ptMouse ))
			{
				m_pContainer->SelectItem( m_iValue + i );
				return m_iControlID;
			}
		}
	}
	return 0;
}

/// ---------------------------------------------------------------------------
/// ---------------------------------------------------------------------------
bool CJListBox::Create(int iScrX, int iScrY, int iWidth, int iHeight , int iItemHeight )
{
	Init( CTRL_JLISTBOX, iScrX, iScrY, iWidth, iHeight );
	m_iCellHeight		= iItemHeight;
	return true;
}

void CJListBox::SetCellRenderer( IListCellRenderer* pRenderer )
{
	assert( pRenderer );
	m_pListCellRenderer = pRenderer;
}

void CJListBox::SetContainer( CTContainer* pContainer )
{
	assert( pContainer );
	m_pContainer = pContainer;
}

unsigned CJListBox::GetSize()
{
	assert( m_pContainer );
	if( m_pContainer )
		return m_pContainer->GetSize();

	return 0;
}

CWinCtrl* CJListBox::GetRendererComponent( const CTObject* pObject, bool bSelect, bool bMouseOver )
{
	assert( m_pListCellRenderer );
	return m_pListCellRenderer->GetListCellRendererComponent( pObject, bSelect, bMouseOver );
}

void CJListBox::SelectItem( unsigned subscript )
{
	assert( m_pContainer );
	m_pContainer->SelectItem( subscript );
}

void CJListBox::Clear()
{
	assert( m_pContainer );
	m_pContainer->Clear();
}

void CJListBox::AddItem( CTObject* pObject )
{
	assert( m_pContainer );
	assert( pObject );
	m_pContainer->AddItem( pObject );
}

const CTObject* CJListBox::GetSelectedItem()
{
	assert( m_pContainer );
	if( m_pContainer )
		return m_pContainer->GetSelectedItem();
	return NULL;
}

unsigned CJListBox::GetSelectedItemIndex()
{
	assert( m_pContainer );
	return m_pContainer->GetSelectedItemIndex();
}

void CJListBox::SetCellHeight( int iHeight )
{
	m_iCellHeight = iHeight;
}

int CJListBox::GetValue()
{
	return m_iValue - 1;
}

int CJListBox::GetExtent()
{
	return m_iExtent;
}

int CJListBox::GetMaximum()
{
	_ASSERT( m_pContainer );
	return m_pContainer->GetSize();
}

int CJListBox::GetMinimum()
{
	return 0;
}

void CJListBox::SetValue( int iValue )
{
	m_iValue = iValue + 1;
}

void CJListBox::SetExtent( int iExtent )
{
	m_iExtent = iExtent;
}

void CJListBox::SetMaximum( int iMaximum )
{
}

void CJListBox::SetMinimum( int iMinimum )
{

}

RECT CJListBox::GetWindowRect()
{
	RECT rc = { m_sPosition.x, m_sPosition.y, m_sPosition.x + m_iWidth, m_sPosition.y + m_iHeight };
	return rc;

}
