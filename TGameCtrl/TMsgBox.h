#ifndef _CTMSGBOX_
#define _CTMSGBOX_

#include "winctrl.h"
#include "TDialog.h"
#include "JStringParser.h"
class CTListBox;
class CTButton;
class CTImage;
class CTCaption;

/**
* 게임내에서 메세지박스로 사용될 특정 Dialog 
*	- TDialog를 상속 받아서 모든 TDialog의 기능을 가지고 있으며, Ok,Cancel버튼, 3개로 나누어진 이미지 부분이 추가되어 있다.
*
* @Author	최종진
*
* @Data		2005/8/30	
*/

class TGAMECTRL_API CTMsgBox : public CTDialog
{
public:
	CTMsgBox(void);
	virtual ~CTMsgBox(void);

	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void Update( POINT ptMouse );
	virtual	void Draw();
	virtual void MoveWindow( POINT pt );
	virtual void Show();
	virtual void Hide();
	virtual bool Create( char* szIDD );


	void		SetButtonType( short int iType );
	short int	GetButtonType(){ return m_iButtonType; }



	enum{
		BT_NONE		= 0x0000,
		BT_OK		= 0x0001,
		BT_CANCEL	= 0x0002
	};

	void SetButtons( CTButton* pBtnOk, CTButton* pBtnCancel );
	void SetImages( CTImage* pImageTop, CTImage* pImageMiddle, CTImage* pImageBottom );
	virtual void SetString( const char* szString );
	void SetMsgType( unsigned int iType ){ m_iMsgType = iType; }


	unsigned int GetMsgType(){ return m_iMsgType; }


	void	AddExtraHeight( int iHeight );
	int		GetExtraHeightStartPoint();
protected:
	void DrawImages();

protected:

	CTButton*	m_pButtonOk;									// Ok Button
	CTButton*	m_pButtonCancel;								// Cancel Button
	
	CTImage*	m_pImageTop;									// 배경이미지중 상단
	CTImage*	m_pImageMiddle;									// 배경이미지중 가운데( 메세지 박스에 표시될 스트링의 길이에 따라서 자동으로 반복되어 그려진다 )
	CTImage*	m_pImageBottom;									// 배경이미지중 하단
	
	short int   m_iButtonType;									// BT_NONE, BT_OK, BT_CANCEL

	unsigned int m_iMsgType;
	int			 m_iExtraHeight;								// 추가적인 여유공간
	int			 m_iExtraHeightStartPoint;

	CJStringParser m_message_parser;							// 메세지 박스에 출력될 스트링
};
#endif
