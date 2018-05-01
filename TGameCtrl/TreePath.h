#ifndef _CTREEPATH_
#define _CTREEPATH_


#include "TObject.h"
class TGAMECTRL_API CTreePath : public CTObject
{
protected:
	CTreePath(void);

public:

	~CTreePath(void);
//	CTreePath(CTObject&  pSinglePath) ; ///Constructs a TreePath containing only a single element. 
//	CTreePath(CTObject&  Path[]);		///Constructs a path from an array of Objects, uniquely identifying the path from the root of the tree to a specific node, as returned by the tree's data model. 


	bool		equals(CTObject* pCtrl) ;/// Tests two TreePaths for equality by checking each element of the paths for equality. 
	CTObject*	getLastPathComponent();///Returns the last component of this path. 
	CTObject*	getParentPath() ;///Returns a path containing all the elements of this object, except the last path component. 
	CTObject*	getPath() ;		///Returns an ordered array of Objects containing the components of this TreePath. 
	CTObject*	getPathComponent(int element) ;///Returns the path component at the specified index. 
	int			getPathCount() ;/// Returns the number of elements in the path. 
	int			hashCode() ;	/// Returns the hashCode for the object. 
	bool		isDescendant(CTreePath aTreePath) ;	///Returns true if aTreePath is a descendant of this TreePath. 
	CTreePath*   pathByAddingChild(CTObject child) ;	///Returns a new path containing all the elements of this object plus child. 

};
#endif