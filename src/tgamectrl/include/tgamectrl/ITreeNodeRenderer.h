#ifndef _ITREENODERENDERER_
#define _ITREENODERENDERER_

#include "TGameCtrl.h"
#include <crtdbg.h>

class CTree;
class CTObject;
class CWinCtrl;

//------------------------------------------------------------------------------------------------------------------------
///	CTree에서 Node의 Draw Class가구현해야할 Interface
///
/// @Author			최종진
///
/// @Date			2005/08/23
//------------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API ITreeNodeRenderer
{
public:
	virtual CWinCtrl* getTreeCellRendererComponent(CTree* tree, CTObject* value, bool selected, bool expanded, bool leaf, int row, bool hasFocus) = 0; /*{ _ASSERT( 0 && "ITreeNodeRenderer::getTreeCellRendererComponent"); return NULL; }*/
};
#endif