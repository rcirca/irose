#ifndef _CTDefaultTreeNode_
#define _CTDefaultTreeNode_
#include "itreenode.h"
#include "TObject.h"
#include	<vector>
class CTObject;
class TGAMECTRL_API CTDefaultTreeNode :public CTObject, public ITreeNode
{
public:
	CTDefaultTreeNode(void);
	CTDefaultTreeNode( CTObject* pObject );
	CTDefaultTreeNode( CTObject* pObject, bool bAllowChildren );

	virtual ~CTDefaultTreeNode(void);

	virtual bool		getAllowsChildren();	
	virtual ITreeNode*	getChildAt(int childIndex);
	virtual int			getChildCount();
	virtual int			getIndex(ITreeNode* pNode);
	virtual ITreeNode*	getParent();
	virtual bool		isLeaf();
	virtual CTObject*	getUserObject();
	virtual ITreeNode*	getNextNode();
	virtual ITreeNode*	getPrevNode();
	virtual bool		isOpen(){ return m_bOpen; }
	virtual void		open(){ m_bOpen = true; }
	virtual void		close(){ m_bOpen = false; }


	/// CTObject


	void		clear();
	void		setAllowsChildren( bool bAllowChildren );
	int			getLevel();///root Node로 부터의 Level, if( this == root ) return 0;
	void		add( ITreeNode* pNode );
	void		setParent( CTDefaultTreeNode* pNode );
protected:
	void reset();


protected:
	bool								m_bOpen;
	bool								m_bAllowChildren;	///true if the node is able to have children 
	std::vector<CTDefaultTreeNode*>		m_children;			///array of children, may be null if this node has no children 
	CTDefaultTreeNode*					m_pParentNode;		///this node's parent, or null if this node has no parent 
	CTObject*							m_pUserObject;      ///optional user object 

};
#endif