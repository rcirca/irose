#ifndef _CTREE_
#define _CTREE_

#include "WinCtrl.h"
#include "IScrollModel.h"
class ITreeNode;
class ITreeModel;
class ITreeNodeRenderer;
class TGAMECTRL_API CTree : public CWinCtrl, public IScrollModel
{

public:
	CTree();
	CTree( CTObject*   pCtrl, int iCount );
	CTree( ITreeNode*  pRootNode );
	CTree( ITreeModel* pModel    );
	~CTree(void);

		
	void SetNodeRenderer( ITreeNodeRenderer* pRenderer );
//	void SetTreeModel( ITreeNode* pRootNode );
	void SetTreeModel( ITreeModel* pTreeModel );
	/// CWinCtrl
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void Draw();

	/// IScrollModel
	virtual int GetValue();
	virtual int GetExtent();
	virtual int GetMaximum();
	virtual int GetMinimum();
	
	virtual void SetValue( int );
	virtual void SetExtent( int );
	virtual void SetMaximum( int );
	virtual void SetMinimum( int );

	virtual RECT GetWindowRect();


	ITreeNode*	GetSelectNode();

	///현재 선택된 Node에서의 이동 처리
	void		MoveParent();
	void		MoveFirstChild();

protected:
	long DrawTree( ITreeNode* pNode ,POINT ptDraw , int& iSelectRow ,int& iExtent, int& iStart );///Recursive
	void DrawNode( ITreeNode* pNode ,POINT ptDraw , bool bSelect );
	void OnLButtonDown( int iRow );
	void OnLButtonDblClk( int iRow );
	void reset();
protected:
	bool				m_bRootVisible;	///True if the root node is displayed, false if its children are the highest visible nodes.
	int					m_iRowHeight;	///Height to use for each display row.
	int					m_iIntentWidth;
	int					m_iVisibleRowCount;
	ITreeModel*			m_pTreeModel;
	ITreeNodeRenderer*	m_pTreeNodeRenderer;


	/// For IScrollModel
	int		m_iValue;
	int		m_iExtent;
	int		m_iMaximum;
	int		m_iMinimum;
};
#endif