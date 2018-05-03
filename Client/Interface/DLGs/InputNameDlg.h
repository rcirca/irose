#ifndef _CInputNameDlg_
#define _CInputNameDlg_

#include "tgamectrl/tdialog.h"

class CTCmdInputName;


/**
* 이름및 스트링 입력시 공통으로 사용되는 다이얼로그
* 
* @Warning		항상 이 다이얼로그를 사용하고 있지는 않다.
* @Author		최종진
* @Date			2005/9/14
*/
class CInputNameDlg : public CTDialog
{
public:
	CInputNameDlg(void);
	virtual ~CInputNameDlg(void);

	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	virtual void Hide();
	void	SetCommand( CTCmdInputName* m_command );
private:
	bool	ExecuteCommand();
	enum{
		IID_BTN_CLOSE	= 10,
		IID_BTN_CONFIRM = 11,
		IID_EDITBOX		= 20
	};

	CTCmdInputName* m_command;
};

#endif