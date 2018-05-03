#ifndef _CNotifyDlg_
#include "tgamectrl/tdialog.h"
#include "tgamectrl/JStringParser.h"

/**
* 알림버튼클릭시 해당 도움말을 표시하기 위한 다이얼로그 
*	- 2005/9/15일 현재 채팅창 오른쪽에 표시되고 있다
*
* @Author		최종진
* @Date			2005/9/15
*/
class CNotifyDlg :	public CTDialog
{
public:
	CNotifyDlg(void);
	virtual ~CNotifyDlg(void);
	virtual void Draw();
	virtual void Hide();
	virtual bool Create( const char* IDD );
	virtual void Show();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam ,LPARAM lParam );

	
	void	SetAdjPos();

	void	SetScript( const char* pszScript );
	void	Clear();

private:

	enum{
		IID_BTN_CLOSE = 10,
		IID_BTN_PREV  = 11,
		IID_BTN_NEXT  = 12
	};
	CJStringParser	 m_Script;
	int				 m_iExtentLine;
	int				 m_iDrawLine;

	
	

};
#endif