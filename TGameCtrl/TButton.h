#ifndef	_TBUTTON_
#define	_TBUTTON_

#include "winctrl.h"
#include "SinglelineString.h"

//---------------------------------------------------------------------------------------------------------------------
/// Button Class
///
/// @Author	최종진
/// @Date	2005/8/30
//---------------------------------------------------------------------------------------------------------------------
class  TGAMECTRL_API CTButton : public CWinCtrl
{
public:
	CTButton(void);
	virtual ~CTButton(void);
	bool	Create( int iScrX, int iScrY, int iWidth, int iHeight, int iNormalGraphicID, int iOverGraphicID, int iDownGraphicID , int iModuleID );
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void Update( POINT ptMouse );
	virtual void Draw();
	
	enum{
		BS_NORMAL,
		BS_MOUSEOVER,
		BS_CLICKED,
	};

	void		SetButtonState( int iState ){ m_iButtonState = iState; }
	int			GetButtonState(){ return m_iButtonState; }

	void				SetText( char* szText );						
	CSinglelineString&	GetText();
	void				SetTextColor( DWORD color );

	void		SetSoundOverID( int i ){ m_iOverSoundID = i; }
	void		SetSoundClickID( int i ){ m_iClickSoundID = i; }
	void		SetDisableGrahicID( int i ){ m_iDisableGrahicID = i; }

	void		SetNoImage();
	void		SetBlink( int iBlink , int iBlinkTime );

	void		SetBlinkGid( int iGid );
	
protected:
	CSinglelineString	m_text;							/// 버튼위에 출력될 Text

	int				m_iButtonState;						/// BS_NORMAL, BS_MOUSEOVER, BS_CLICKED

	int				m_iModuleID;						/// Images의 모듈 ID - 최대 5개 상태의 이미지를 설정할수 있지만 모듈은 한개이다(공통된 모듈에 있어야 한다)
	
	int				m_iNormalGraphicID;					/// 보통상태에서의 이미지 ID
	int				m_iOverGraphicID;					/// 마우스가 버튼영역위에 있을때의 이미지 ID
	int				m_iDownGraphicID;					///	유저의 입력으로 버튼이 눌렸을대의 이미지 ID
	int				m_iDisableGrahicID;					/// Disable상태일때의 이미지 ID	
	int				m_iBlinkGraphicID;					/// 깜박임상태일때 노말상태와 번갈아 표시될 이미지 ID
	int				m_iSourceNormalGraphicID;			/// 깜박임상태일때 임시로 실제 노말상태의 이미지 ID를 저장하고 있을 변수 


	

	
	int				m_iOverSoundID;						/// 버튼이 
	int				m_iClickSoundID;					/// 버튼 클릭시 Play할 사운드 ID
	int				m_iBlink;							/// 깜박임 상태인가?
	int				m_iBlinkTime;						/// 깜박임상태일경우 시간 간격
	DWORD			m_dwPrevUpdateTime;
	bool			m_bNoImage;							/// 이미지 없는 버튼인가?
	DWORD			m_textcolor;						/// 텍스트 칼라

};
#endif/// _TBUTTON_
