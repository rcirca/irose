#include "StdAfx.h"
#include ".\tdefaulttreenode.h"

CTDefaultTreeNode::CTDefaultTreeNode(void)
{
	reset();
}
CTDefaultTreeNode::~CTDefaultTreeNode(void)
{

}
void CTDefaultTreeNode::clear()
{
	std::vector<CTDefaultTreeNode*>::iterator iter;
	CTDefaultTreeNode* pNode = NULL;
	for( iter = m_children.begin(); iter != m_children.end(); )
	{
		pNode = (*iter);
		iter = m_children.erase( iter );
		pNode->clear();
		delete pNode;
	}
}

CTDefaultTreeNode::CTDefaultTreeNode( CTObject* pObject )
{
	reset();
	m_pUserObject = pObject;
}

CTDefaultTreeNode::CTDefaultTreeNode( CTObject* pObject, bool bAllowChildren )
{
	reset();
	m_pUserObject = pObject;
	m_bAllowChildren = bAllowChildren;
}

void CTDefaultTreeNode::reset()
{
	m_bAllowChildren  = true;
	m_pUserObject	  = NULL;
	m_pParentNode	  = NULL;
	m_strString		  = "TreeNode";
	m_bOpen			  = false;
}

bool		CTDefaultTreeNode::getAllowsChildren()
{
	return m_bAllowChildren;
}

ITreeNode*	CTDefaultTreeNode::getChildAt(int childIndex)
{
	if( childIndex < 0 || childIndex >= getChildCount() )
		return NULL;

	return m_children[ childIndex ];
}

int			CTDefaultTreeNode::getChildCount()
{
	return (int)m_children.size();
}

int			CTDefaultTreeNode::getIndex(ITreeNode* pNode)
{
	std::vector<CTDefaultTreeNode*>::iterator iter;
	int iCount = 0;
	for( iter = m_children.begin(); iter != m_children.end(); ++iter ,++iCount)
	{
		if( pNode == *iter )
			return iCount;
	}
	return -2;
}

ITreeNode*	CTDefaultTreeNode::getParent()
{
	return m_pParentNode;
}

int			CTDefaultTreeNode::getLevel()
{
	if( m_pParentNode )/// == ( !isRoot() )
		return m_pParentNode->getLevel() + 1;

	return 0;
}

bool		CTDefaultTreeNode::isLeaf()
{
	if( m_children.empty() ) 
		return true;

	return (!m_bAllowChildren);
}

void		CTDefaultTreeNode::setParent( CTDefaultTreeNode* pNode )
{
	m_pParentNode = pNode;
}

void		CTDefaultTreeNode::add( ITreeNode* pNode )
{
	CTDefaultTreeNode* pTreeNode = (CTDefaultTreeNode*)pNode;
	pTreeNode->setParent( this );
	m_children.push_back(pTreeNode );
}

CTObject*	CTDefaultTreeNode::getUserObject()
{
	return m_pUserObject;
}

ITreeNode*	CTDefaultTreeNode::getNextNode()
{
	if( m_pParentNode )
	{
		int iIndex = m_pParentNode->getIndex( this );
		return m_pParentNode->getChildAt( iIndex + 1 );
	}
	return NULL;	
}

ITreeNode*	CTDefaultTreeNode::getPrevNode()
{
	if( m_pParentNode )
	{
		int iIndex = m_pParentNode->getIndex( this );
		return m_pParentNode->getChildAt( iIndex - 1 );
	}
	return NULL;
}

void		CTDefaultTreeNode::setAllowsChildren( bool bAllowChildren )
{
	m_bAllowChildren = bAllowChildren;
}