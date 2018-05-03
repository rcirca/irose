#ifndef _CGoodsDlg_
#define _CGoodsDlg_
#include "tgamectrl/tdialog.h"
#include "subclass/CSlot.h"
/**
* 개인상점에 물품 등록시 사용되는 UI Dialog
*
* @Author		최종진
* @Date			2005/9/12
*/
class CGoodsDlg : public CTDialog
{
public:
	CGoodsDlg(int iDlgType);
	virtual ~CGoodsDlg(void);
	virtual void Draw();
	virtual void Show();
	virtual void MoveWindow( POINT pt );
	virtual void Update( POINT ptMouse );
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	
	void	SetIcon( CIcon* pIcon );
	void	SetType( int iType );

	enum{
		ADD_SELLLIST,
		ADD_BUYLIST
	};
private:

	enum{
		IID_TEXT_BUY		= 6,
		IID_TEXT_SELL		= 7,
		IID_BTN_CLOSE		= 10,
		IID_BTN_CONFIRM		= 11,
		IID_EDIT_PRICE		= 20,
		IID_EDIT_QUANTITY	= 21,
	};
	int		m_iType;				/// 구입희망 or 판매희망 구분
	CSlot	m_Slot;
};
#endif