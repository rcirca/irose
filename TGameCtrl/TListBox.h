#ifndef	_TLISTBOX_
#define _TLISTBOX_
#include "winctrl.h"
#include "IScrollModel.h"
#include <deque>

using namespace std;

typedef struct {
	char		m_szTxt[MAX_PATH];
	D3DCOLOR	m_dwColor;
	bool		m_bDrawn;
}t_list_item;


typedef deque<t_list_item>						li_item_vec;
typedef deque<t_list_item>::iterator			li_item_vec_itor;
typedef deque<t_list_item>::const_iterator		li_item_vec_const_itor;

class CTScrollBar;
class ITFont;

/**
* ListBox Class : t_list_item을 item으로 가질수 있는 ListBox
*	- ScrollBar를 붙일수 있도록 IScrollModel을 Implement한다.
*
* @Author	최종진
*
* @Date		2003/11/26
*/
class TGAMECTRL_API CTListBox :	public CWinCtrl, public IScrollModel
{
public:
	CTListBox(void);
	virtual ~CTListBox(void);

	bool					Create( int iScrX, int iScrY, int iWidth, int iHeight ,int iExtent,int iCharWidth, int iCharHeight );
	virtual unsigned int	Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void			Update( POINT ptMouse );
	virtual void			Draw();

	/// IScrollModel 구현부 
	virtual int GetValue();
	virtual int GetExtent();
	virtual int GetMaximum();
	virtual int GetMinimum();
	
	virtual void SetValue( int i );
	virtual void SetExtent( int i );
	virtual void SetMaximum( int i );
	virtual void SetMinimum( int i );

	virtual RECT GetWindowRect();
	virtual bool IsLastItemDrawn();
	///텍스트 추가 삭제
	void	AppendText(const char* szTxt,D3DCOLOR dwColor,bool bAutoIncValue = true );
	void	SetText( int index, const char* text, D3DCOLOR dwColor );
	void	DeleteText(int iLineNo);
	const char*	GetText(int iLineNo);
	t_list_item	GetItem( int iLineNo );
	void    ClearText();///모든 Item들을 지운다.
	///
	const   li_item_vec&	GetList(){ return m_ITM; }

	///아이템 선택
	bool	IsSelectable(){ return m_bSelectable; }
	void	SetSelectable( bool b ){ m_bSelectable = b; }
	short   GetSelectedItemID(){ return m_iSelectedItem; }
	const char* GetSelectedItem();
	void    SetSelectedItem( short i){ m_iSelectedItem = i; }
	///라인스페이스 ( 행간 )
	void    SetLineSpace( short i ){ m_iLineSpace = i; }
	short   GetLineSpace(){ return m_iLineSpace; }
	///
	void	SetMaxSize( int i ){ m_iMaxSize = i; }
	int		GetMaxSize(){ return m_iMaxSize; }
	///폰트 크기
	void    SetCharHeight( short i ){ m_iCharHeight = i; }
	short   GetCharHeight(){ return m_iCharHeight; }

	void    SetCharWidth( short i );
	short   GetCharWidth(){ return m_iCharWidth; }

	void	SetFont( int iFont );

protected:
	///리스트가 선택가능한 타입일경우 메세지 처리하는 method
	bool	ProcessSelectable(UINT uiMsg,WPARAM wParam,LPARAM lParam);

protected:
	li_item_vec			m_ITM;			///리스트아이템 
	short				m_nPutLnNum;	///현재 찍을 첫번째 라인번호 
	short				m_nMaxLnCnt;	///최대 찍을수 있는 라인수 
	short				m_nMaxPutChar;	///한라인에찍을수있는 최대문자수 
	short				m_iSelectedItem;///선택된 아이템
	bool				m_bSelectable;  ///아이템이 선택가능한가?
	short				m_iLineSpace;   ///행간
	short				m_iCharHeight;  ///폰트 높이 
	short				m_iCharWidth;   ///폰트 넓이
	int					m_iMaxSize;		///리스트에 추가될수 있는 아이템의 최대 갯수
	ITFont*				m_pFontMgr;
	int					m_iFont;

};
#endif ///_TLISTBOX_