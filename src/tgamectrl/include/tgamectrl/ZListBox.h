#ifndef _ZListBox_
#define _ZListBox_
#include "winctrl.h"
#include "IScrollModel.h"
#include <deque>
//*------------------------------------------------------------------//
///
//*------------------------------------------------------------------//
class TGAMECTRL_API CZListBox : public CWinCtrl, public IScrollModel
{
public:
	CZListBox(void);
	virtual ~CZListBox(void);
	
	virtual void Draw();
	virtual void MoveWindow( POINT pt );
	virtual void Update( POINT ptMouse );
	virtual void Show();
	virtual void Hide();
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );

	void	Add( CWinCtrl* pCtrl );
	void	Clear();
	void	SetSelected( int iIndex );
	void	DeselectAll();
	int		GetSelectedItemIndex();
	CWinCtrl* GetItem( int iIndex );
	void	InsertItem( int iIndex, CWinCtrl* pCtrl );
	bool	DelItem( int iIndex );
	bool	DelItemByControlID( int iID );
	int		GetSize();
	
	
	//*---------------------------------------------------//
	/// implemented from IScrollModel
	virtual int GetValue();
	virtual int GetExtent();
	virtual int GetMaximum();
	virtual int GetMinimum();
	
	virtual void SetValue( int );
	virtual void SetExtent( int );
	virtual void SetMaximum( int );
	virtual void SetMinimum( int );

	virtual RECT GetWindowRect();
	//*---------------------------------------------------//

protected:

	int							m_iValue;
	int							m_iExtent;

	std::deque<CWinCtrl*>		m_Items;
};
#endif