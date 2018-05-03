#ifndef	_TCAPTION_
#define _TCAPTION_

#include "winctrl.h"

class CTImage;
class CTButton;

/**
* Windows의 Window Caption같은 기능의 Caption Class
*	- 현재 CTDialog와 CTMsgBox두군데에서 중복된 처리를 해주고 있다. 주의할것
*	: CTCaption에서 처리될수 있도록 수정필요
*
* @Author	최종진
* @Date		2003/12/11
*/
class TGAMECTRL_API CTCaption :	public CWinCtrl
{
public:
	CTCaption(void);
	virtual ~CTCaption(void);

	bool					Create(int iScrX, int iScrY, int iWidth, int iHeight);
	virtual unsigned int	Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void			Update( POINT ptMouse );
	virtual void			Draw();
	virtual void			Hide();
	virtual void			MoveWindow(POINT pt);
	bool		IsClicked();

	void		SetString( const char* pszString );
	void		SetIcon( CTImage* pImage );
	void		SetImage( CTImage* pImage );
	void		SetCloseButton( CTButton* pButton );
	void		SetIconButton( CTButton* pButton );

	enum{
		IID_IMAGE_ICON	= 2,
		IID_IMAGE_BG	= 3,
		IID_BTN_ICON	= 4,
		IID_BTN_CLOSE	= 5
	};

protected:
	bool		m_bClicked;

	/// 이하 현재 구현되어 있지 않다.( 사용하고 있지 않다는 : 2005/8/30 최종진 )
	std::string	m_strTitle;									///타이틀 바에 별도로 그려지는 String
	CTImage*	m_pIcon;									/// 제일 앞에 보여질 아이콘이미지
	CTImage*	m_pImage;									/// 캡션 Image
	CTButton*	m_pCloseButton;								///Dialog Close
	CTButton*	m_pIconButton;								/// Dialog Iconize(??)아이콘화
};
#endif	///_TCAPTION_