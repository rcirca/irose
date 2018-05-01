#include "StdAfx.h"
#include ".\treepath.h"

CTreePath::CTreePath(void)
{
}

CTreePath::~CTreePath(void)
{
}
bool		CTreePath::equals(CTObject* pCtrl)/// Tests two TreePaths for equality by checking each element of the paths for equality. 
{
	return false;
}
CTObject*	CTreePath::getLastPathComponent()///Returns the last component of this path. 
{
	return false;
}
CTObject*	CTreePath::getParentPath()///Returns a path containing all the elements of this object, except the last path component. 
{
	return NULL;
}
CTObject*	CTreePath::getPath()		///Returns an ordered array of Objects containing the components of this TreePath. 
{
	return NULL;
}
CTObject*	CTreePath::getPathComponent(int element)///Returns the path component at the specified index. 
{
	return NULL;
}
int			CTreePath::getPathCount()/// Returns the number of elements in the path. 
{
	return 0;
}

int			CTreePath::hashCode()	/// Returns the hashCode for the object. 
{
	return 0;
}
bool		CTreePath::isDescendant(CTreePath aTreePath)	///Returns true if aTreePath is a descendant of this TreePath. 
{
	return 0;
}
CTreePath*   CTreePath::pathByAddingChild(CTObject child)	///Returns a new path containing all the elements of this object plus child. 
{
	return NULL;
}
