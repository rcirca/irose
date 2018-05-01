#ifndef _ChatRoomDlg_
#define _ChatRoomDlg_
#include "TDialog.h"
#include "../../GameData/CChatRoom.h"
#include "../../GameCommon/IObserver.h"
#include "IActionListener.h"
/**
* 커뮤니티 창에서 채팅룸을  위한 UI Dialog
*	- Observable : CChatRoom
* 
* @Author		최종진
* @Date			2005/9/12
*/
class CChatRoomDlg : public CTDialog, public IObserver, public IActionListener
{
public:
	CChatRoomDlg( int iDlgType );
	virtual ~CChatRoomDlg(void);
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual bool		Create( const char* IDD );
	virtual void		Draw();
	virtual void		Show();
	virtual void		Hide();

	virtual void		Update( CObservable* pObservable, CTObject* pObj );

	virtual unsigned	ActionPerformed( CActionEvent* e );							

	void				RecvChatMsg( WORD wUserID, const char* pszMsg );			/// 서버로 부터 받은 채팅룸의 채팅메세지 처리


private:
	void		AddChatMsg( const char* pszMsg, DWORD dwColor );

	void		OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam );
	void		OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam );
	void		SendChatMsg();

	void		Minimize();															/// 최소화시 처리
	void		Maximize();															/// 최대화시 처리

	CWinCtrl*	FindChildInPane( unsigned uiPaneID, unsigned uiChildID );
private:
	enum{
		IID_BTN_CLOSE		 = 10,
		IID_CHAT_EDIT		 = 11,
		IID_CHAT_LIST		 = 12,
		IID_CHAT_SCROLLBAR	 = 13,
		IID_TABBEDPANE		 = 20,
		IID_TABBUTTON_MEMBER = 25,
		IID_EDITBOX_TITLE	 = 39,
		IID_EDITBOX_PWD		 = 34,
		IID_TABBUTTON_CONFIG = 35,
		IID_COMBOBOX_MAXUSER = 36,
		IID_BTN_MAKE		 = 32,
		IID_COMBOBOX_TYPE	 = 136,
		IID_PANE_TOP		 = 100,
		IID_PANE_BOTTOM		 = 200,
		IID_BTN_MINIMIZE	 = 113,
		IID_BTN_MAXIMIZE	 = 114,

	};
	enum{
		TAB_CONFIG	= 0,
		TAB_MEMBER	= 1,
	};

	bool	m_bMinimize;															/// 최소화/최대화 구분
	int		m_iTab;																	/// 탭 구분
	std::list< CChatMember >	m_Members;											/// 채팅룸 멤버 저장 컨테이너
};
#endif