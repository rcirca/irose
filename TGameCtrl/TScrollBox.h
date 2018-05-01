#ifndef	_TSCROLLBOX_
#define _TSCROLLBOX_

#include "winctrl.h"
#include "TScrollBarType.h"
class CTScrollBar;
class CTScrollBoxType;
class CTImage;

/**
* TScrollBar에서의 스크롤박스( 버튼 ) Class
* - 연관Class : CTScrollBoxType, CTScrollBar
*
* @Author		최종진
* @Date			2005/8/30
*/
class TGAMECTRL_API CTScrollBox : public CWinCtrl
{
public:
	CTScrollBox(void);
	virtual ~CTScrollBox(void);

	bool					Create(int iWidth, int iHeight, int iGraphicID,int iModuleID, int iType = CTScrollBarType::TSBT_VERTICAL );
	virtual unsigned int	Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void			Update( POINT ptMouse );
	virtual void			Draw();

	void		SetTickMove( int i );
	void		SetScrollBar( CTScrollBar* pScrollBar ){ m_pScrollBar = pScrollBar; }
	void		MoveBoxPosition( int iValue, int iMax ,int iExtent );
	void		MoveBoxPositionByScreenPoint( POINT pt );
	void		SetBlink();
	void		SetUnblink();
	void		SetBlinkImage( CTImage* pImage );
protected:


protected:
//	BYTE			m_bBlink
	bool			m_bBlink;
	int				m_iModuleID;
	int				m_iGraphicID;
	CTScrollBar*	m_pScrollBar;

	///스크롤박스로 이동시 현재 그래픽과 버튼다운 마우스포인터와의 거리차를 저장해둔다.
	///Update에서 해당 차이만클 빼서 그래픽을 이동시킨다.


	bool				m_bClicked;
	CTScrollBoxType*	m_pScrollBoxType;
	CTImage*			m_pBlinkImage;
};
#endif ///_TSCROLLBOX_