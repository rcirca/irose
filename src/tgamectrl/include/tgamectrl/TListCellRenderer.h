#ifndef _CTListCellRenderer_
#define _CTListCellRenderer_
#include "ilistcellrenderer.h"
class CTStatic;

/**
* CTContainerItem을 Item으로 가지는 Container를 소유한 JListBox를 위한 Renderer
*
* @Author		최종진
*
* @Data			2005/8/30	
*/
class TGAMECTRL_API CTListCellRenderer : public IListCellRenderer
{
	CTStatic*		m_pComponent;
public:
	CTListCellRenderer(void);
	~CTListCellRenderer(void);

	virtual CWinCtrl* GetListCellRendererComponent( const CTObject* pObject, bool selected, bool mouseover );
};
#endif