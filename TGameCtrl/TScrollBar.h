#ifndef	_TSCROLLBAR_
#define _TSCROLLBAR_

#include "winctrl.h"
#include "TScrollBarType.h"


class CTButton;
class CTScrollBox;
class IScrollModel;
class CTScrollBarType;

/**
* 스크롤 바 Class
*	- 연관Class		CTScrollBarType, CTScrollBox
*
* @Author	최종진
*
* @Data		2005/8/30
*/
class TGAMECTRL_API CTScrollBar : public CWinCtrl
{
public:
	CTScrollBar(void);
	virtual ~CTScrollBar(void);
	bool					Create(int iScrX, int iScrY, int iWidth, int iHeight,int iType = CTScrollBarType::TSBT_VERTICAL );
	virtual unsigned int	Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void			Update( POINT ptMouse );
	virtual void			Draw();

	void SetPrevButton( CTButton* pButton );
	void SetNextButton( CTButton* pButton );
	void SetScrollBox( CTScrollBox* pThumb );

	virtual void	MoveWindow( POINT pt );
	virtual void	SetOffset( POINT pt );
	void			SetTickMove( int i );
	RECT			GetThumbMoveRange();
	void			MoveEnd();
	void			MoveHome();

	///	Scroll을 붙이기 위한 Class(IScrollModel을 상속받은)의 포인터
	void			SetModel( IScrollModel* pModel );

	/// ScrollBox가 ScrollModel의 값을 알기 위하여 중계해주는 operation
	int				GetExtent();
	int				GetMaximum();
	void			SetModelValue( int i );
	virtual	void	Show();
	virtual void	Hide();


	/// IScrollModel과 ScrollBox(Thumb)을 조정( Prev,Next버튼,wheel 처리시 )
	void			SetValue(int iValue);

protected:
	CTButton*		m_pPrevButton;				// "다음" 버튼						
	CTButton*		m_pNextButton;				// "이전" 버튼
	CTScrollBox*	m_pScrollBox;				// 수평,수직에 따라 스테이트 패턴사용

	IScrollModel*		m_pScrollModel;			// ScrollBar와 같이 동작될 Model의 포인터
	CTScrollBarType*	m_pScrollBarType;		// 수평,수직에 따라 스트레티지 패턴 사용
};
#endif ///_TSCROLLBAR_