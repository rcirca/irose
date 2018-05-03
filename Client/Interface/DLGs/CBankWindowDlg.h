#ifndef _CBankWindowDlg_
#define _CBankWindowDlg_
#include "tgamectrl/tdialog.h"
#include "tgamectrl/singlelinestring.h"


/**
* 은행에서 돈을 보관하거나 찾을때 줄리양을 입력하는 다이얼로그
*
* @Author		최종진
* @Date			2005/9/12
*/
class CBankWindowDlg :	public CTDialog
{
public:
	CBankWindowDlg(void);
	virtual ~CBankWindowDlg(void);
	
	virtual void		Draw();
	virtual unsigned	Process( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void		PostMsg( unsigned msg, unsigned param1, unsigned param2 );
	virtual void		Update( POINT ptMouse );

	enum{
		IID_EDITBOX = 10,
		IID_BTN_OK  = 20,
		IID_BTN_CANCEL = 21
	};

	enum{
		TYPE_NONE		= 0,
		TYPE_SAVE,
		TYPE_WITHDRAW
	};

	enum{
		MSG_SETYPE = 1
	};
private:
	void OnLButtonUp( unsigned proc_id );

private:
	int					m_Type;					/// 보관/찾기 구분
	CSinglelineString   m_Title;				/// 다이얼로그 캡션에 보여질 텍스트
	
};
#endif