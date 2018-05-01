#ifndef _CTabbedPanel_
#define _CTabbedPanel_

#include "winctrl.h"

class CJContainer;
class CTRadioButton;
class CTRadioBox;
class CTButton;

struct S_TabbedPane{
	int				m_iIndex;
	CTRadioButton*	m_pRadioButton;
	CJContainer*	m_pContainer;
};

//--------------------------------------------------------------------------------------*/
/// Tab Container Control
///		- S_TabbedPane을 child로 가지며 한개의 S_TabbedPane 오브젝트가 한개의 Tab이된다
/// 
/// @Author				최종진
/// @Date				2005/8/30
//--------------------------------------------------------------------------------------*/
class TGAMECTRL_API CTabbedPane :	public CWinCtrl
{
public:
	CTabbedPane(void);
	virtual ~CTabbedPane(void);

	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void Update( POINT ptMouse );
	virtual	void Draw();
	virtual void Show();
	virtual void Hide();
	virtual void MoveWindow( POINT pt );
	virtual bool IsInside( int x, int y );

	unsigned AddTab( CTRadioButton* pButton, CJContainer* pContainer );		/// Tab 추가 - 인덱스가 부여된다( 0-Base )
	CJContainer*	GetTabContainer( int iIndex );							/// 해당 인덱스의 S_TabbedPane을 리턴한다.

	void	 RemoveAll();													
	int		 GetSelectedIndex();											/// 현재 선택된 S_TabbedPane을 리턴
	bool	 SetSelectedIndex( unsigned index );							/// 해당인덱스의 S_TabbedPane을 선택한다

	/// 창의 최소화 기능을 위한 추가적인 Method
	void	 SetMinimizeRect( const RECT& rc );								/// 최소화시 영역 Set
	bool	 IsMinimize();													/// 최소화 상태인가?
	void	 SetMinimize( bool bMinimize );									/// 최소/최대화
	void	 SetMinimizeButton( CTButton* pButton );						/// 최소화 버튼 Set


protected:
	std::list< S_TabbedPane >	m_Tabs;										/// child를 보관할 Container
	CTRadioBox*					m_pRadioBox;								/// 탭버튼은 라디오 버튼으로 라디오 박스와 연결되어 있다
	CTButton*					m_pMinimizeButton;							/// 최소화 버튼

	bool						m_bMinimize;								/// 최소화중
	RECT						m_rcMinimize;								/// 최소화시 영역
};
#endif