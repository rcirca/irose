#ifndef _CJComboBox_
#define _CJComboBox_

#include "winctrl.h"

class CTButton;
class CTImage;
class CJListBox;
class ITFont;
class CTObject;

//------------------------------------------------------------------------------------------------------------------------
///	콤보 박스 Class - CTComboBox의 개량판( 기능적으로 크게 다른점은 없다 );
///
/// @Author				최종진
///
/// @Date				2005/08/23
//------------------------------------------------------------------------------------------------------------------------

class TGAMECTRL_API CJComboBox : public CWinCtrl
{
public:
	CJComboBox(void);
	~CJComboBox(void);

	virtual		unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual		void Update( POINT ptMouse );
	virtual		void Draw();

	virtual		void Show();
	virtual		void Hide();
	virtual		void SetOffset( POINT pt );
	virtual		void MoveWindow( POINT pt );
	virtual		void OwnerDraw();												/// 특정한 상황에 제일 마지막에 드랍박스를 그리기 위하여 Draw부분이 구분되어있다
																				/// 다른 WinCtrl이 뒤에 그려진다면 드랍박스가 뒤에 감춰지는 상황


	bool		Create(int iScrX, int iScrY, int iWidth, int iHeight);			
	
	void		SetButton( CTButton* pButton );									/// 드랍박스 버튼 Set
	void		SetImage( CTImage* pTop, CTImage* pMiddle, CTImage* pBottom );	/// 드랍박스를 그릴 이미지(3부분)를 Set
	void        SetListBox( CJListBox* pListBox );								/// 아이템들을 보관할 JListBox를 Set 

	void		AddItem( CTObject* pItem );										/// 드랍박스에 아이템을 추가
	void		SelectItem( unsigned iSubscript );								/// 해당 인덱스의 아이템을 선택
	unsigned	GetSelectedItemIndex();											/// 현재 선택된 아이템의 인덱스 리턴

	CJListBox*	GetListBoxPtr();												/// 드랍박스에서 아이템을 보관하는 리스트박스의 포인터 리턴
	const CTObject*	GetSelectedItem();											/// 현재 선택되어진 아이템을 구한다.

	bool			IsShowDropBox();											/// 현재 드랍박스를 그려야 하는가?
	void			DrawDropBoxImage();											/// 3부분으로 나누어진 Images를 그린다.
	void			ClearItem();												/// DropBox에 있는 모든 Item들을 지운다.

protected:
	void		ToggleShowDropBox();											/// 드랍박스를 Show/Hide한다
	void		DrawSelectedItem();												/// 선택된 아이템을 드랍박스 위에 그린다.

protected:
	ITFont*		m_pFontMgr;														/// TControlMgr에서 얻어온 Client가 구현한 ITFont의 Implemented-Class의 포인터 임시보관
	CTButton*	m_pDropButton;
	CTImage*	m_pDropBoxTopImage;
	CTImage*	m_pDropBoxMiddleImage;
	CTImage*	m_pDropBoxBottomImage;
	CJListBox*	m_pListBox;

};
#endif