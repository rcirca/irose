#ifndef _CSeparateDlg_
#define _CSeparateDlg_
#include "tdialog.h"
#include "../../GameCommon/IObserver.h"
#include "subclass/CSlot.h"
#include <vector>
class CIconItem;
class CDragItem;
/**
* 분리/분해에 사용되는 다이얼로그
*	- Observable : CSeparate
* 
* @Author		최종진
* @Date			2005/9/14
*/
class CSeparateDlg : public CTDialog, public IObserver
{
public:
	CSeparateDlg(void);
	virtual ~CSeparateDlg(void);

	virtual void Update( POINT ptMouse );
	virtual void Show();
	virtual void Draw();
	virtual void MoveWindow( POINT pt );
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	virtual void Update( CObservable* pObservable, CTObject* pObj );

protected:
	bool	HasEnoughInventoryEmptySlot( CIconItem* pItemIcon, std::vector<CSlot>& OutputItems );

protected:
	enum{
		IID_TEXT_COST = 5,
		IID_BTN_START = 10,
		IID_BTN_CLOSE = 11,
	};

	CSlot					m_MaterialItemSlot;/// 분리/분해할 아이템이 들어갈 슬롯
	std::vector< CSlot >	m_OutputItemSlots; /// 분리/분해한 아이템이 들어갈 슬롯 

	CDragItem*				m_pDragItem;
};
#endif