#ifndef	_TDIALOG_
#define	_TDIALOG_

#include "winctrl.h"
#include <string>
#include "TCommandQ.h"

class CTCaption;
class CTCommand;
class CTStatusBar;
class CTImage;
/**
* WinCtrl들을 child 가지고 처리하는 Class - 인터페이스처리에서 가장 기본이 되는 Class이다
* 
* @Author		최종진
*
* @Date			2005/8/30 
*/
class TGAMECTRL_API CTDialog : public CWinCtrl
{
public:
	CTDialog(void);
	virtual ~CTDialog(void);
	bool     Create( int iScrX, int iScrY, int iWidth, int iHeight );
	virtual bool	Create( const char* IDD );
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void	Update( POINT ptMouse );
	virtual void	Draw();
	virtual	void	Show();
	virtual	void	Hide();
	virtual void    MoveWindow( POINT pt );
	virtual void    MoveWindow( int x, int y );
	virtual void	SetInterfacePos_After();
	
	/// 모달 다이얼로그 인가?
	virtual bool    IsModal();													
	
	/// 다이얼로그의 일부 속성및 간단한 행위를 하기위한 virtual method : client에서 재정의해서 사용한다.
	virtual void	PostMsg( unsigned msg, unsigned param1, unsigned param2 ){}	

	/// 다이얼로그의 사용가능을 체크하기위한 virtual method : Client에서 재정의하여 사용 - 	if( return value == 1 ) Valid 
	virtual int		IsInValidShow(){ return 0; }

	///WinCtrl 추가/삭제/찾기 Method
	void		Add( CWinCtrl* pChild );
	bool		Remove( int iID );
	CWinCtrl*	Find( int iID );
	CWinCtrl*	Find( const char * strName );

	/// 사용안하고 있다.
	void		SetStatusBar( CTStatusBar* pStatusBar );				

	///캡션 관련
	void		SetCaption( CTCaption* pCaption );
	long		GetChildCount();
	CTCaption*	GetCaption(){ return m_pCaption; }
	void		ChangeCaptionRect( RECT rc );


	///특정 child만을 Show/Hide/Enable
	bool		ShowChild( unsigned int iID );
	bool		HideChild( unsigned int iID );
	bool		ShowChild( const char * strName  );
	bool		HideChild( const char * strName  );
	bool		SetEnableChild( unsigned int iID, bool bEnable );

	virtual void RefreshDlg();


	/// Dialog생성후 최초 표시 위치의 상대적 위치 타입: X,Y
	enum{
		TDXP_LEFT,
		TDXP_CENTER,
		TDXP_RIGHT
	};
	
	enum{
		TDYP_TOP,
		TDYP_CENTER,
		TDYP_BOTTOM
	};

	DWORD		GetDialogType(){ return m_dwDialogType; }
	void		SetDialogType( DWORD iDialogType ){ m_dwDialogType = iDialogType;}

	///	Command Pattern으로 사용되는 Command의 추가/삭제/관리
	void		Push_Back_Cmd( CTCommand* pCmd );
	void		Push_Front_Cmd( CTCommand* pCmd );
	void		ClearCommandQ();
	CTCommand*	GetTCommand();

	/// 한프레임내에서 마우스가 자신의 바로 위에 있는( 다른 control이 없는경우 ) TDialog의 포인터 관리
	static  void		SetProcessMouseOverDialog( CTDialog* pTDialog ){ m_pProcessMouseOverDialog = pTDialog; }
	static  CTDialog*	GetProcessMouseOverDialog(){ return m_pProcessMouseOverDialog; }

	///사운드
	void		SetSoundShowID( int i ){ m_iShowSoundID = i; }	/// Dialog가 Show시 Play될 Sound ID Set
	void		SetSoundHideID( int i ){ m_iHideSoundID = i; }	/// Dialog가 Hide시 Play될 Sound ID Set

	/// Default Position 관련 
	void		SetDefaultPosX( int iPosX ){ m_iXPos = iPosX; }
	void		SetDefaultPosY( int iPosY ){ m_iYPos = iPosY; }
	void		SetDefaultAdjustPosX( int iAdjustPosX ){ m_iAdjustXPos = iAdjustPosX; }
	void		SetDefaultAdjustPosY( int iAdjustPosY ){ m_iAdjustYPos = iAdjustPosY; }

	int			GetDefaultPosX(){ return m_iXPos; }
	int			GetDefaultPosY(){ return m_iYPos; }
	int			GetDefaultAdjustPosX(){ return m_iAdjustXPos; }
	int			GetDefaultAdjustPosY(){ return m_iAdjustYPos; }

	void		SetDefaultVisible(bool b) { m_bDefaultVisible = b; }
	bool		IsDefaultVisible(){ return m_bDefaultVisible; }


	void		SetModal();																/// Dialog를 모달로 
	void		SetModeless();															/// Dialog를 모달리스로


	void		SetMinimizeRect( const RECT& rc );

	void		SetExtent( int i );
	void		SetImage( CTImage* pImage );


protected:
	void		MoveCtrl2ListEnd( int iID );											/// 해당 ID의 child를 리스트의 맨뒤로 이동
	void		ProcessLButtonDown();													/// Dialog위에서 LButtonDown이 일어났을때 처리할 Method


protected:
	WINCTRL_LIST	m_listChild;														/// WinCtrl을 보관할 Container;
	WINCTRL_LIST	m_OwnerDrawChildren;												/// 사용안함

	CTCaption*		m_pCaption;															/// TCaption Object
	CTStatusBar*	m_pStatusBar;														/// 사용안함
	CTImage*		m_pImage;															/// 사용안함	


	POINT			m_ptCaptionClicked;													/// 캡션이 눌려졌을때의 Mouse 와 Dialog Left-Top과의 거리 

	DWORD			m_dwDialogType;

	bool			m_bModal;															/// Modal Or Modaless : Default - Modaless
	int				m_iShowSoundID;														/// Dialog::Show() 시 Play될 Sound ID	
	int				m_iHideSoundID;														/// Dialog::Hide() 시 Play될 Sound ID	

	CTCommandQ		m_CommandQ;															/// TCommand를 저장 - Hide및 소멸시에 자동으로 Clear되게 한다.

	///Default Position
	int				m_iXPos;															/// LEFT-CENTER-RIGHT( 상대적 X )
	int				m_iYPos;															/// TOP-CENTER-BOTTOM( 상대적 Y )
	int				m_iAdjustXPos;														/// 보정 X위치 - 픽셀
	int				m_iAdjustYPos;														/// 보정 Y위치 - 픽셀
	bool			m_bDefaultVisible;													/// 클라이언트에서 최초 게임실행시 보여줄것인가?
	

	int				m_iExtent;															///	ClientRect에 그려질 Image의 개수(Row) - 중복된 Image의 여러번 그리기에 사용하려고 했으나 현재 1로 고정

	static CTDialog* m_pProcessMouseOverDialog;											/// 한 프레임안에서 마우스가 바로 자신 위에 있는 Dialog의 포인터( Update시 맨처음 clear시켜주어야 한다 )

};
#endif	///_TDIALOG_
