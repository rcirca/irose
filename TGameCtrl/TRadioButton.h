#ifndef _RADIOBUTTON_
#define _RADIOBUTTON_

#include "winctrl.h"
#include "SinglelineString.h"

class CTRadioBox;
class ITFont;

/**
* 라디오 버튼 Class
*	- 라디오박스에 등록후 그 라디오박스에 등록된 버튼들과 연계되어 동작된다.
*	- 버튼이 클릭되었을때 라디오 박스의 EventChangeSelect()를 호출하여 다른 버튼들의 상태 변경
*	- 연관Class : CTRadioBox
*
* @Author			최종진
*
* @Data				2005/8/30
*/
class TGAMECTRL_API CTRadioButton :	public CWinCtrl
{
public:
	CTRadioButton(void);
	virtual ~CTRadioButton(void);

	bool	Create( int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID ,int iModuleID);
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void Update( POINT ptMouse );
	virtual void Draw();

	void	SetButtonState( int iState );
	short	GetButtonState(){ return m_iButtonState; }

	void    SetRadioBox( CTRadioBox* pRadioBox ){ m_pRadioBox = pRadioBox; }
	enum{
		BS_NORMAL,
		BS_MOUSEOVER,
		BS_CLICKED
	};

	void				SetText( char* szText );
	CSinglelineString&	GetText();

	void		SetSoundDisableID( int i );
	void		SetGhost();

protected:
	CSinglelineString	m_text;					/// 버튼위에 출력될 스트링

	short			m_iButtonState;

	int				m_iModuleID;
	int				m_iNormalGraphicID;
	int				m_iOverGraphicID;
	int				m_iDownGraphicID;

	int				m_iDisableSoundID;			/// Disable된 상태에서 클릭시 Play할 Sound ID
	CTRadioBox*		m_pRadioBox;				/// 등록된 RadioBox
	ITFont*			m_pFontImpl;
	bool			m_bGhost;					/// 실제로 없지만 자리를 잡기 위한 용도로 사용될 경우 ( CTabbedPane에서 라디오 버튼이 있어야 Tab이 등록되어서 )
};
#endif