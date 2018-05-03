#ifndef _CMenuDlg_
#define _CMenuDlg_
#include "tgamectrl/tdialog.h"


/// 튜토리얼 스크립트에서 해당 버튼의 인덱스를 참조하기 위하여 전역으로 열거함
enum{	
	MENU_BTN_CHAR = 10,
	MENU_BTN_ITEM,
	MENU_BTN_SKILL,
	MENU_BTN_QUEST,
	MENU_BTN_COMMUNITY,
	MENU_BTN_CLAN, 
	MENU_BTN_HELP,
	MENU_BTN_INFO,
	MENU_BTN_OPTION,
	MENU_BTN_EXIT,
};


/**
* 윈도우즈 스타일의 메뉴를 위한 다이얼로그
*
* @Author		최종진
* @Date			2005/9/12
*/
class CMenuDlg : public CTDialog
{
public:
	CMenuDlg(void);
	virtual ~CMenuDlg(void);
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void Update( POINT ptMouse );
	virtual void Show();

	void	SetInterfacePos_After();
	void	RefreshDlg();

	void	SetOpen( bool b = true );
	void	SetClose();
	bool	GetIsOpen();

	void	SetOpenPos( POINT & pt );
	POINT	&GetOpenPos();	

	void	ProcOpen();
	void	SetOpenType(int iType );
	int		GetOpenType();

private:
	void OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam );

private:

	enum{
		IID_BTN_CHARACTER = MENU_BTN_CHAR,
		IID_BTN_ITEM,
		IID_BTN_SKILL,
		IID_BTN_QUEST,
		IID_BTN_CUMMUNITY,
		IID_BTN_GUILD,
		IID_BTN_HELP,
		IID_BTN_GAMEINFO,
		IID_BTN_OPTION,
		IID_BTN_EXIT		
	};

	enum
	{
		_STOP = 0,
		_OPEN,
		_CLOSE,
	};

	int		m_uiClickedButtonID;		/// 눌려진 버튼의 ID
	POINT	m_ptClickedButton;			/// 버튼 클릭시 마우스 포인터( 메뉴에서 바로 인터페이스창 아이콘을 드래그 하여 바탕화면에 빼내기 위하여 사용)

	bool	m_bIsOpen;
	POINT	m_ptOpenPos;
	int		m_iOpenType;				//0 : Stop, 1 : Open, 2 : Close
};
#endif