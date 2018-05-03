#ifndef _CTDefaultListCellRenderer_
#define _CTDefaultListCellRenderer_
#include "ilistcellrenderer.h"

class CTStatic;
class TGAMECTRL_API CTDefaultListCellRenderer : public IListCellRenderer
{
	CTStatic*		m_pComponent;
public:
	CTDefaultListCellRenderer(void);
	~CTDefaultListCellRenderer(void);

	virtual CWinCtrl* GetListCellRendererComponent( const CTObject* pObject, bool selected, bool mouseover );
};
#endif