#ifndef _CTDefaultTreeModel_
#define _CTDefaultTreeModel_
#include "itreemodel.h"
#include "TObject.h"
class CTObject;
class CTDefaultTreeNode;
class TGAMECTRL_API CTDefaultTreeModel :public CTObject, public ITreeModel
{
	CTDefaultTreeModel(void);
public:
	CTDefaultTreeModel(ITreeNode* pRoot);
	virtual ~CTDefaultTreeModel(void);

	virtual ITreeNode*	getChild(ITreeNode* pParent, int& index);
	virtual void		getChildCount(ITreeNode* pParent, int& iCount);
	virtual int			getIndexOfChild(CTObject* pParent, CTObject* pChild);
	virtual ITreeNode*	getRoot();
	virtual bool		isLeaf(CTObject* pNode);
	int					getSelectRow();
	void				setSelectRow( int iRow );
	void				clear();

protected:
	bool				m_bAsksAllowsChildren ;///Determines how the isLeaf method figures out if a node is a leaf node. 
	CTDefaultTreeNode*  m_pRootNode; 
	int					m_iSelectRow;
};
#endif