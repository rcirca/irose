#ifndef _CJTable_
#define _CJTable_

#include <vector>
#include "winctrl.h"
#include "IScrollModel.h"

//--------------------------------------------------------------------------------------------------------------------------------------
///	테이블 Class
///	- children을 라인당 컬럼 개수로 나누어 처리한다.
///
/// @Author				최종진
/// @Date				2005/08/30
//--------------------------------------------------------------------------------------------------------------------------------------
class TGAMECTRL_API CJTable :	public CWinCtrl, public IScrollModel
{
public:
	CJTable(void);
	virtual ~CJTable(void);

	/// CWinCtrl
	virtual void Draw();
	virtual void Update( POINT ptMouse );
	virtual unsigned Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam );
	virtual void MoveWindow( POINT pt );

	/// IScrollModel
	virtual int GetValue();
	virtual int GetExtent();
	virtual int GetMaximum();
	virtual int GetMinimum();
	
	virtual void SetValue( int iValue );
	virtual void SetExtent( int iExtent );
	virtual void SetMaximum( int iMaximum );
	virtual void SetMinimum( int iMinimum );

	virtual RECT GetWindowRect();


	void	Add( CWinCtrl* pCtrl );
	void	SetColumnCount( int iCount );										/// 라인당 Cell 개수 Set
	void	SetCellWidth( int iWidth );											/// Cell Width Set
	void	SetCellHeight( int iHeight );										/// Cell Height Set
	void	SetRowMargin( int iMargin );										/// 라인간 Space Set
	void	SetColMargin( int iMargin );										/// Cell간 Space Set
	CWinCtrl*	GetItem( int iIndex );											
	int		GetSelectedItemID();			
protected:
	int		m_iValue;															/// 화면에서 맨위에 그려질 Row Index
	int		m_iExtent;															/// 한 화면에 그려질 Rows
	int		m_iColumnCount;														/// 라인당 Cell 개수
	int		m_iCellWidth;														/// Cell Width
	int		m_iCellHeight;														/// Cell Height
	int		m_iRowMargin;														/// Row간 Space
	int		m_iColMargin;														/// Cell간 Space

	int		m_iSelectedItemID;													/// 선택되어진 Item의 ID

	std::vector< CWinCtrl* > m_Items;
	std::vector< CWinCtrl* > m_ViewItems;
};
#endif