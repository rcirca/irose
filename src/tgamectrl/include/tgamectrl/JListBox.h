#ifndef _CJListBox_
#define _CJListBox_

#include "winctrl.h"
#include "IScrollModel.h"

class IListCellRenderer;
class CTContainer;

//------------------------------------------------------------------------------------------------------------------------
///	리스트 박스 class
///
/// @Author	최종진
///
/// @Date	2005/08/30
//------------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API CJListBox : public CWinCtrl, public IScrollModel
{

public:
	CJListBox(void);
	~CJListBox(void);

	bool	Create( int iScrX, int iScrY, int iWidth, int iHeight , int iItemHeight );

	virtual void Draw();
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	///Renderer
	void	SetCellRenderer( IListCellRenderer* pRenderer );									/// Item의 Draw가 정의된 Class를 Set
	void	SetContainer( CTContainer* pContainer );											/// Item을 저장할 Container를 Set
	
	///Container
	void		Clear();
	void		AddItem( CTObject* pObject );

	/// IScrollModel
	virtual int GetValue();
	virtual int GetExtent();
	virtual int GetMaximum();
	virtual int GetMinimum();
	
	virtual void SetValue( int );
	virtual void SetExtent( int );
	virtual void SetMaximum( int );
	virtual void SetMinimum( int );

	virtual RECT GetWindowRect();


	///operation
	const CTObject*	GetSelectedItem();																/// 선택된 Item을 리턴한다.
	unsigned		GetSelectedItemIndex();															/// 선택된 Item의 인덱스를 리턴한다.
	unsigned		GetSize();																		

	CWinCtrl*		GetRendererComponent( const CTObject* pObject, bool bSelect, bool bMouseOver ); /// Item의 Draw가 정의된 Class를 리턴
	void			SelectItem( unsigned subscript );												/// 해당 Item을 선택한다.


	void			SetCellHeight( int iHeight );													/// Draw시 Item간 Space를 Set
private:
	IListCellRenderer*		m_pListCellRenderer;													/// Item의 Draw가 정의된 Class
	CTContainer*			m_pContainer;															/// Item을 저장할 Container


	int						m_iCellHeight;															/// Draw시 Item간 Space
	int						m_iExtent;																/// 한번에 보여줄 Item의 수
	int						m_iValue;																/// 현재 화면에서 제일 위에 보여질 Item의 Index

};
#endif