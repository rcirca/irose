#include "StdAfx.h"
#include ".\tree.h"
#include "ITreeNode.h"
#include "ITreeModel.h"
#include "ITreeNodeRenderer.h"
#include "tgamectrl/tdefaulttreemodel.h"
#include "tgamectrl/tdefaulttreenode.h"
CTree::CTree()
{
	m_pTreeModel		= NULL;
	m_pTreeNodeRenderer = NULL;
	reset();
}

CTree::~CTree(void)
{
}

CTree::CTree( CTObject*   pCtrl, int iCount )
{
	reset();
}

CTree::CTree( ITreeNode*  pRootNode )
{
	reset();
	m_pTreeModel = new CTDefaultTreeModel( pRootNode );
}

CTree::CTree( ITreeModel* pModel )
{
	reset();
}

void CTree::reset()
{
	m_bRootVisible		= true;	///True if the root node is displayed, false if its children are the highest visible nodes.
	m_iRowHeight		= 15;	///Height to use for each display row.
	m_iIntentWidth		= 15;
	m_iVisibleRowCount	= 10;
	m_iExtent			= 16;
	m_iValue			= 0;
}
/*
void CTree::SetTreeModel( ITreeNode* pRootNode )
{
	reset();
	m_pTreeModel = new CTDefaultTreeModel( pRootNode );
}
*/
void CTree::SetTreeModel( ITreeModel* pTreeModel )
{
	reset();
	m_pTreeModel = pTreeModel;
}

long CTree::DrawTree( ITreeNode* pNode , POINT ptDraw , int& iSelectRow, int& iExtent , int& iStart )
{
	if( pNode == NULL || iExtent <= 0  )
		return ptDraw.y;
	

	POINT ptChildDraw	= ptDraw;
	bool bSelect = false;
	if( iSelectRow == 0 )
		bSelect = true;
	if( pNode == m_pTreeModel->getRoot() )
	{
		if( m_bRootVisible )
		{
			if( iStart <= 0 )
			{
				DrawNode( pNode , ptChildDraw , bSelect );
				ptChildDraw.y += m_iRowHeight;
				iExtent--;
			}
			else
			{
				iStart--;
			}
			ptChildDraw.x += m_iIntentWidth;
			iSelectRow--;
		}
	}
	else
	{
		if( iStart <= 0 )
		{
			DrawNode( pNode, ptChildDraw , bSelect );
			ptChildDraw.y += m_iRowHeight;
			iExtent--;
		}
		else
		{
			iStart--;
		}
		ptChildDraw.x += m_iIntentWidth;
		iSelectRow--;
	}
	if( !pNode->isLeaf() && pNode->isOpen() )
	{
		for( int i = 0; i < pNode->getChildCount(); ++i )
			ptChildDraw.y  = DrawTree( pNode->getChildAt( i ), ptChildDraw ,iSelectRow, iExtent ,iStart );		
	}
	return ptChildDraw.y;
}

ITreeNode*	CTree::GetSelectNode()
{
	CTDefaultTreeModel* pModel = (CTDefaultTreeModel*)m_pTreeModel;
	int iSelectRow = pModel->getSelectRow();
	if( iSelectRow < 0 )
		return NULL;

	if( !m_bRootVisible )
		iSelectRow++;

	return pModel->getChild( pModel->getRoot(), iSelectRow );
}

void CTree::DrawNode( ITreeNode* pNode ,POINT ptDraw , bool bSelect)
{
	if( pNode == NULL )
		return;


	CWinCtrl* pCtrl;
	pCtrl = m_pTreeNodeRenderer->getTreeCellRendererComponent( this, pNode->getUserObject(), bSelect, pNode->isOpen(), pNode->isLeaf(), 0, false );

	pCtrl->Draw( ptDraw );

}

void CTree::SetNodeRenderer( ITreeNodeRenderer* pRenderer )
{
	m_pTreeNodeRenderer = pRenderer;
}
///*----------------------------------------------------------------------------------------
/// CWinCtrl
unsigned int CTree::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if(!IsVision() ) return 0;

	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };
	//영역안에 있는지 체크
	if(!IsInside( ptMouse.x, ptMouse.y )) {
		//m_iButtonState = BS_NORMAL;
		return 0;
	}

	ptMouse.x -= m_sPosition.x;
	ptMouse.y -= m_sPosition.y;

	RECT rc = { 0,0,m_iWidth,m_iRowHeight };
	int iRow = 0;

	switch( uiMsg )
	{
	case WM_LBUTTONDOWN:
		{
			for( ; rc.bottom < m_iHeight; rc.top += m_iRowHeight, rc.bottom += m_iRowHeight, ++iRow )
			{
				if( PtInRect( &rc, ptMouse ) )
				{
					OnLButtonDown( iRow );
					return m_iControlID;
				}
			}
			break;
		}
	case WM_LBUTTONDBLCLK:
		{
			for( ; rc.bottom < m_iHeight; rc.top += m_iRowHeight, rc.bottom += m_iRowHeight, ++iRow )
			{
				if( PtInRect( &rc, ptMouse ) )
				{
					OnLButtonDblClk( iRow );
					return m_iControlID;
				}
			}
			break;
		}
	}
	return m_iControlID;
}

void CTree::OnLButtonDown( int iRow )
{
	ITreeNode* pNode = NULL;
	ITreeNode* pRootNode = m_pTreeModel->getRoot();
	int iSelectRow = iRow + m_iValue;
	
	if( !m_bRootVisible )
		++iSelectRow;

	pNode = m_pTreeModel->getChild( pRootNode, iSelectRow );

	if( pNode )
		((CTDefaultTreeModel*)m_pTreeModel)->setSelectRow( iRow + m_iValue );

}
void CTree::OnLButtonDblClk( int iRow )
{
	ITreeNode* pNode = NULL;
	ITreeNode* pRootNode = m_pTreeModel->getRoot();
	int iSelectRow = iRow + m_iValue;
	
	if( !m_bRootVisible )
		++iSelectRow;

	pNode = m_pTreeModel->getChild( pRootNode, iSelectRow );

	if( pNode )
	{
		if( pNode->isOpen() )
			pNode->close();
		else
			pNode->open();
	}
}
void CTree::Draw()
{
	if( !IsVision() ) return;
	if( m_pTreeModel == NULL ) return;
	if( m_pTreeNodeRenderer == NULL ) return;
	int iSelectRow = ((CTDefaultTreeModel*)m_pTreeModel )->getSelectRow();
	int iExtent    = m_iExtent;
	int iStart     = m_iValue;

	DrawTree( (ITreeNode*)m_pTreeModel->getRoot(), m_sPosition, iSelectRow ,iExtent ,iStart );

}

void CTree::MoveParent()
{
	if( m_pTreeModel == NULL )
		return;
	/*
	CTDefaultTreeModel* pModel = ((CTDefaultTreeModel*)m_pTreeModel);

	int iSelectRow = pModel->getSelectRow();
	int iRow = iSelectRow;
	ITreeNode* pNode = pModel->getChild( pModel->getRoot(), iRow );
	ITreeNode* pParent = NULL;
	if( pNode && (pParent = pNode->getParent()) )
	{
		int iIndex = pParent->getIndex( pNode ) + 1;
		pModel->setSelectRow( iSelectRow - iIndex );
	}
	*/
}
void CTree::MoveFirstChild()
{
	if( m_pTreeModel == NULL )
		return;

	CTDefaultTreeModel* pModel = ((CTDefaultTreeModel*)m_pTreeModel);

	int iSelectRow = pModel->getSelectRow();
	int iRow = iSelectRow;
	ITreeNode* pNode = pModel->getChild( pModel->getRoot(), iRow );
	if( pNode && !pNode->isLeaf() )
	{
		pNode->open();
		pModel->setSelectRow( iSelectRow + 1 );
	}
}

///*----------------------------------------------------------------------------------------
/// IScrollModel
int CTree::GetValue()
{
	return m_iValue;
}

int CTree::GetExtent()
{
	return	m_iExtent;
}

int CTree::GetMaximum()
{
	int iMaximum = 0;
	if( m_pTreeModel )
		m_pTreeModel->getChildCount( m_pTreeModel->getRoot(), iMaximum );

	return iMaximum;
}

int CTree::GetMinimum()
{
	return m_iMinimum;
}

void CTree::SetValue( int i )
{
	m_iValue = i;
	m_iMaximum = GetMaximum();
	if( m_iValue < 0 )
		m_iValue = 0;
	else if( m_iValue + m_iExtent > m_iMaximum )
		m_iValue = m_iMaximum - m_iExtent;

}

void CTree::SetExtent( int i )
{
	m_iExtent = i;
}

void CTree::SetMaximum( int i )
{
	m_iMaximum = i;
}

void CTree::SetMinimum( int i )
{
	m_iMinimum = i;
}

RECT CTree::GetWindowRect()
{
	RECT rc = { m_sPosition.x, m_sPosition.y, m_sPosition.x + m_iWidth, m_sPosition.y + m_iHeight };
	return rc;
}
