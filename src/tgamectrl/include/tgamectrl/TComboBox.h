#ifndef _CTComboBox_
#define _CTComboBox_

#include "winctrl.h"
#include "TListBox.h"

class CTButton;
class CTImage;
class CTListBox;
class ITFont;
/**
* 콤보 박스 Class - 업그레이드 버젼으로 CTComboBox이 있다
*
* @Author	최종진
*
* @Date		2005/08/30
*/
class TGAMECTRL_API CTComboBox : public CWinCtrl
{
public:
	CTComboBox(void);
	virtual ~CTComboBox(void);

	virtual		unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual		void Update( POINT ptMouse );
	virtual		void Draw();

	virtual		void Show();
	virtual		void Hide();
	virtual		void SetOffset( POINT pt );
	virtual		void MoveWindow( POINT pt );
	virtual		void OwnerDraw();

	bool		Create(int iScrX, int iScrY, int iWidth, int iHeight);
	
	void		SetButton( CTButton* pButton );
	void		SetImage( CTImage* pTop, CTImage* pMiddle, CTImage* pBottom );
	void        SetListBox( CTListBox* pListBox );


	void		AddItem( char* szItem );
	short		GetSelectedItemID();

	bool		IsShowDropBox();

	const		li_item_vec&	GetList();
	const		char*			GetSelectedItem();
	void		SetSelectedItem( short i );

	CTListBox*	GetListBox(){ return m_pListBox; }
	void        DrawDropBoxImage();

	
	void		ClearItem();///DropBox에 있는 모든 Item들을 지운다.
protected:
	void		ToggleShowDropBox();
	void		DrawSelectedItem();


protected:
	ITFont*		m_pFontMgr;
	CTButton*	m_pDropButton;
	CTImage*	m_pDropBoxTopImage;
	CTImage*	m_pDropBoxMiddleImage;
	CTImage*	m_pDropBoxBottomImage;
	CTListBox*	m_pListBox;

	li_item_vec	m_TempList;///리스트 박스가 없을경우 대신 넘겨줄 리스트의 레퍼런스를 위한 임시 변수--;


};
#endif