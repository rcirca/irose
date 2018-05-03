#ifndef _ITREENODE_
#define _ITREENODE_


#include "TGameCtrl.h"
#include <crtdbg.h>

class CTObject;

//------------------------------------------------------------------------------------------------------------------------
///	CTree에서 Node로 사용하고자 하는 Class가 구현해야할 Interface
///
/// @Author				최종진
/// @Date				2005/08/23 
//------------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API ITreeNode
{
public:
		//children();								///Returns the children of the receiver as an Enumeration. 
	virtual bool		getAllowsChildren()=0; /*{ _ASSERT( 0 && "ITreeNode::getAllowsChildren" );return false; }*/;			///Returns true if the receiver allows children. 
	virtual ITreeNode*	getChildAt(int )=0; /*{ _ASSERT( 0 && "ITreeNode::getChildAt" ); return NULL; }*/;	///Returns the child TreeNode at index childIndex. 
	virtual int			getChildCount()=0; /*{ _ASSERT( 0 && "ITreeNode::getChildCount" ); return 0; }*/;				///Returns the number of children TreeNodes the receiver contains. 
	virtual int			getIndex(ITreeNode* )=0; /*{ _ASSERT( 0 && "ITreeNode::getIndex" ); return 0; }*/;	///Returns the index of node in the receivers children. 
	virtual ITreeNode*	getParent()=0;/*{ _ASSERT( 0 && "ITreeNode::getParent" ); return NULL;}*/;					///Returns the parent TreeNode of the receiver. 
	virtual bool		isLeaf()=0 ;/*{ _ASSERT( 0 && "ITreeNode::isLeaf" ); return true; }*/;
	virtual CTObject*   getUserObject()=0 ;/*{ _ASSERT( 0 && "ITreeNode::getUserObject" ); return NULL; }*/
	virtual bool		isOpen()=0; /*{ _ASSERT( 0 && "ITreeNode::isOpen" ); return false; }*/
	virtual void		open()=0;/* { _ASSERT( 0 && "ITreeNode::open" ); };*/
	virtual void		close()=0;/* { _ASSERT( 0 && "ITreeNode::close" ); };*/
};
#endif