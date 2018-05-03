#ifndef _IListCellRenderer_
#define _IListCellRenderer_

#include "TGameCtrl.h"
#include <crtdbg.h>

class CTObject;
class CWinCtrl;

//---------------------------------------------------------------------------------------------------
/// CJListBox에 items의 Drawing Class
///
/// *관련Class	CJListBox
///
/// @Author		최종진
/// @Date		2005/8/23
//---------------------------------------------------------------------------------------------------

class TGAMECTRL_API IListCellRenderer
{
public:
	virtual CWinCtrl* GetListCellRendererComponent( const CTObject* pObject, bool selected, bool mouseover ) = 0/* { _ASSERT( 0 && "IListCellRenderer::GetListCellRendererComponent"); return NULL;}*/;
};
#endif