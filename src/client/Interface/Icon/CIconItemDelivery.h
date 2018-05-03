#ifndef _CIconItemDelivery_
#define _CIconItemDelivery_
#include "ciconitem.h"
/**
* 마일리지 아이템 창고에서 사용되는 Item Icon
*
* @Author		최종진
* @Date			2005/9/6
*/
class CIconItemDelivery : public CIconItem
{
public:
	CIconItemDelivery(void);
	CIconItemDelivery( tagITEM* pItem , const char* pszFrom );
	virtual ~CIconItemDelivery(void);


	virtual unsigned	Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void		Draw();
	virtual void		ExecuteCommand(){}
	virtual void		GetToolTip( CInfo& strToolTip,  DWORD dwDialogType, DWORD dwType );


	static int s_selected_icon;			/// 선택된 아이콘에 대한 인덱스를 static으로 가지고 있는다

protected:
	int				m_border_grahicid;
	std::string		m_from;
};
#endif