#include "StdAfx.h"
#include ".\tdefaulttreemodel.h"
#include "TDefaultTreeNode.h"

CTDefaultTreeModel::CTDefaultTreeModel(void)
{
	m_bAsksAllowsChildren = true;
	m_iSelectRow		  = -1;
	m_pRootNode	= NULL; 

}
CTDefaultTreeModel::CTDefaultTreeModel(ITreeNode* pRoot)
{
	m_iSelectRow		= -1;
	m_pRootNode			= (CTDefaultTreeNode*)pRoot;
	m_pRootNode->open();
}

CTDefaultTreeModel::~CTDefaultTreeModel(void)
{
}

void CTDefaultTreeModel::clear()
{
	if( m_pRootNode )
	{
		m_pRootNode->clear();
		delete m_pRootNode;
		m_pRootNode = NULL;
	}
}

ITreeNode*	CTDefaultTreeModel::getChild(ITreeNode* pParent, int& index)
{
	if( index <= 0 || pParent == NULL )
		return pParent;

	if( !pParent->isOpen() || pParent->isLeaf() )
		return NULL;

	ITreeNode* pNode;
	for( int i = 0; i < pParent->getChildCount(); ++i )
	{
		pNode  = getChild( pParent->getChildAt( i ), --index );
		if( pNode != NULL )
			return pNode;
	}
	return NULL;
}

void			CTDefaultTreeModel::getChildCount(ITreeNode* pParent, int& iCount)
{
	if(  pParent == NULL )
		return ;
	
	iCount++;
	if( !pParent->isOpen() || pParent->isLeaf() )
	{
		return;
	}


	for( int i = 0; i < pParent->getChildCount(); ++i )
		getChildCount( pParent->getChildAt(i), iCount );

	return;
}

int			CTDefaultTreeModel::getIndexOfChild(CTObject* pParent, CTObject* pChild)
{
	return -1;
}

ITreeNode*	CTDefaultTreeModel::getRoot()
{
	return m_pRootNode;
}

bool		CTDefaultTreeModel::isLeaf(CTObject* pNode)
{
	return false;
}

int			CTDefaultTreeModel::getSelectRow()
{
	return m_iSelectRow;
}

void		CTDefaultTreeModel::setSelectRow( int iRow )
{
	m_iSelectRow = iRow;
}
