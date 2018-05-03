#ifndef _CClanDlg_
#define _CClanDlg_
#include "tgamectrl/tdialog.h"
#include "../../gamecommon/IObserver.h"
#include "../CClanMarkUserDefined.h"

class CClanMemberItem;
class CTabbedPane;
class CZListBox;

/**
* 클랜정보를 보여주는 다이얼로그
*	- Observable : CClan
*
* @Author		최종진
* @Date			2005/9/12
*/
class CClanDlg : public CTDialog, public IObserver
{
public:
	CClanDlg(void);
	virtual ~CClanDlg(void);
	
	virtual void		Draw();
	virtual void		Show();
	virtual void		Hide();
	virtual bool		Create( const char* IDD );
	virtual unsigned	Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	virtual void		Update( CObservable* pObservable, CTObject* pObj );
			
			void		SetSelectedMember( int iClanPoint );
private:
	enum{
		IID_BTN_CLOSE				= 10,
		IID_BTN_ICONIZE				= 11,
		IID_TABBEDPANE				= 20,
		IID_BTN_TAB_INFO			= 25,
		IID_BTN_PREVIEW				= 28,
		IID_BTN_REGIST_CLANMARK		= 29,
		IID_BTN_TAB_MEMBER			= 35,
		IID_ZLIST_MEMBER			= 36,
		IID_BTN_ENTRUST				= 41,
		IID_BTN_BAN					= 42,
		IID_BTN_CLASS_UP			= 43,
		IID_BTN_CLASS_DOWN			= 44,
		IID_BTN_REQJOIN				= 45,
		IID_BTN_WITHDRAWAL			= 46,
		IID_BTN_TAB_SKILL			= 55,
		IID_ZLIST_SKILL				= 56,
		IID_BTN_TAB_NOTICE			= 70,
		IID_ZLIST_NOTICE			= 75,
		IID_ZLIST_NOTICE_CONTENT	= 77,
		IID_BTN_REGIST_NOTICE		= 80,
		IID_BTN_DELETE_NOTICE		= 81,
	};

	enum{
		TAB_MEMBER	= 0,
		TAB_SKILL	= 1,
		TAB_NOTICE	= 2,
		TAB_INFO	= 3
	};

	void		DrawInfomation();					/// 기본정보 탭 Draw
	void		DrawMembers();						/// 멤버정보 탭 Draw
	void		DrawSkills();						/// 클랜스킬 탭 Draw
	void		DrawNotice();						/// 공지사항 탭 Draw

	void		OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam );
	void		OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam );

	void		SortMemberlistByClass();			/// 등급별로 소트

	CWinCtrl*	FindCtrlInTabbedPane( CTabbedPane* pPane, int iTabID, int iCtrlID );

	void		RemoveClanSkillListItem( CZListBox* pListBox, int iSkillSlotIndex );

	CClanMemberItem* GetSelectedMember();



	int		m_iTab;									/// 탭 구분
	int		m_iClassMarkImageNo[ 7 ];				/// 클랜 멤버의 등급에 따른 이미지 GraphicID 저장
	

	std::string m_strSelectedMemberView;			/// 멤버정보탭에서 선택된 멤버표시시에 쓰이는 정보 저장

	CClanMarkUserDefined	m_ClanMarkPreView;		/// 클랜마크 View Class
};
#endif