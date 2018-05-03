#ifndef _CWINCTRL_
#define _CWINCTRL_


#include "itcontrol.h"
#include "itdraw.h"
#include "ActionListenerList.h"
#include <string>

#define STATUS_VISION			0x0001
#define STATUS_FOCUS			0x0002
#define STATUS_ACTIVE			0x0004
#define STATUS_ENABLE			0x0008
#define STATUS_ALL				STATUS_VISION | STATUS_FOCUS | STATUS_ACTIVE | STATUS_ENABLE

#define CTRL_IMAGE				0x0100
#define CTRL_BUTTON				0x0200
#define CTRL_EDITBOX			0x0300
#define CTRL_LISTBOX			0x0400
#define CTRL_SCROLLBAR			0x0500
#define	CTRL_COMBOBOX			0x0600
#define CTRL_DIALOG				0x0700
#define CTRL_STATIC				0x0800
#define CTRL_SCROLLBOX			0x0900
#define CTRL_CAPTION			0x0A00
#define CTRL_RADIOBOX			0x0B00
#define CTRL_RADIOBUTTON		0x0C00
#define CTRL_MSGBOX				0x0D00
#define CTRL_PUSHBUTTON			0x0E00
#define CTRL_TREE				0x0F00
#define CTRL_GUAGE				0x1000
#define CTRL_JCOMBOBOX			0x1100
#define CTRL_JLISTBOX			0x1200
#define CTRL_TABBEDPANE			0x1300
#define CTRL_STATUSBAR			0x1400
#define CTRL_ZLISTBOX			0x1500
#define CTRL_CHECKBOX			0x1600
#define CTRL_PANE				0x1700
#define CTRL_SLOT				0x1800
#define CTRL_TABLE				0x1900

class  CWinCtrl;
class  CTDialog;

class IActionListener;

typedef list< CWinCtrl* >				WINCTRL_LIST;	
typedef WINCTRL_LIST::iterator			WINCTRL_LIST_ITOR;
typedef WINCTRL_LIST::reverse_iterator  WINCTRL_LIST_RITOR;


typedef map< unsigned int,CWinCtrl* >	WINCTRL_MAP;	
typedef WINCTRL_MAP::iterator			WINCTRL_MAP_ITOR;



/**
*인터페이스에서 사용되는 Control들의 기본이 되는 Class
*
* @Author	최종진
* @Date		2005/8/30
*/

class TGAMECTRL_API CWinCtrl : public CTObject, public ITControl
{

protected:

	CWinCtrl*		m_pParent;			/// 자신을 포함하고 잇는 Parent의 포인터( Parent가 있을지라도 비어 잇을수 있다 )
	DWORD			m_dwStatus;			/// 상태변수 
	DWORD			m_dwCtrlType;		///
	POINT			m_sPosition;		///
	POINT			m_ptOffset;			/// 컨트롤이 Child(Dialog의)일경우 Parent의 Left-Top좌표와의 거리
	int				m_iWidth;
	int				m_iHeight;
	unsigned int	m_iControlID;		/// 컨터롤 아이디
	std::string		m_strControlName;	/// 컨트롤 이름.

	int				m_iParentID;
	ITDraw*			m_pDraw;
	bool			m_bSelected;
	

	static CWinCtrl*		m_pMouseExclusiveCtrl;      /// 마우스를 독점하고 있는 컨트롤
	static CWinCtrl*		m_pProcessMouseOverCtrl;	// 현 프레임에서 마우스가 자신의 바로 위에 있는 WinCtrl의 포인터 : 매프레임 처리전 Clear되어야 한다.
	static bool				m_bProcessMouseOver;

	bool					m_bOwnerDraw;///Client에서 별도의 코드로 그려지게 될경우 true ( Default: false )

	float					m_fScaleWidth;
	float					m_fScaleHeight;
	BYTE					m_btAlphaValue;
	CActionListenerList		m_ActionListenerList;
	bool					m_bSizeFit;

public:
	CWinCtrl(void);
	virtual ~CWinCtrl(void);

	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual	void Update( POINT ptMouse ){};
	virtual	void Draw(){}
	virtual void Draw( POINT ptDraw ){};///임의의 좌표에 그린다.저장되지는 않는다.
	virtual	bool	IsInside( int x, int y );
	/// 초기화 
	virtual void	Init( DWORD dwType,int iScrX, int iScrY, int iWidth, int iHeight );	//초기화함수 

	virtual CWinCtrl*	Find( const char * szName );

	///m_bOwnerDraw == true일때 그리지 않는 Draw 루틴 , 
	///Layer문제로 추가, ListBox나 ComboBox에서 사용
	///Client가 적당한 시기에 Draw하는 루틴
	virtual void	OwnerDraw(){};
	virtual void	SetSelected();
	virtual void	SetDeselected();
	bool			IsSelected();

	///마우스 독점 관련 operation
	static	bool		IsExclusive();
	static	void		SetMouseExclusiveCtrl( CWinCtrl* pCtrl );
	static  CWinCtrl*	GetMouseExclusiveCtrl();
	
	/// 마우스 오버 관련 처리 operation : Client에서 전체 Interface를 Update하기전에 false로 Setting하고
	/// 처리된 Ctrl들이 이를 true로 Set하여 다른 Ctrl이 처리하지 않게끔 한다.
	static  bool		IsProcessMouseOver(){ return m_bProcessMouseOver; }
	static  void		SetProcessMouseOver( bool b ){ m_bProcessMouseOver = b; }
	static  void		SetProcessMouseOverCtrl( CWinCtrl* pCtrl ){ m_pProcessMouseOverCtrl = pCtrl; }
	static  CWinCtrl*	GetProcessMouseOverCtrl(){ return m_pProcessMouseOverCtrl; }

	//////////////////////////////////////////////////////////////////////////
	/// get, set method...
	//////////////////////////////////////////////////////////////////////////


	void			SetControlName(const char * szName)	{	m_strControlName = szName;	}
	const char *	GetControlName()					{	return m_strControlName.c_str();	}

	int				GetControlType(){ return ( m_dwCtrlType ); }
	void			SetControlType( DWORD dwType ){ m_dwCtrlType = dwType; }
	void			SetControlID( unsigned int iID ) { m_iControlID = iID; };
	unsigned int	GetControlID( ){ return m_iControlID; };

	/// 상태변경 함수 
	void			SetCtrlStatus(DWORD	dwStatus)	{ m_dwStatus = dwStatus; }
	virtual	void	Show()							{ m_dwStatus |=STATUS_VISION;  }
	virtual	void	Hide()							{ m_dwStatus &= ~STATUS_VISION; }	
	virtual void	SetFocus(bool bFocus)

	{ 
		if(bFocus)
			m_dwStatus |=STATUS_FOCUS; 
		else
			m_dwStatus &=~STATUS_FOCUS;
	}

	void			SetActive(bool bActive)
	{
		if(bActive)
			m_dwStatus |=STATUS_ACTIVE; 
		else
			m_dwStatus &=~STATUS_ACTIVE; 
	}


	void			SetEnable( bool bEnable )
	{
		if( bEnable )
			m_dwStatus |= STATUS_ENABLE;
		else
			m_dwStatus &= ~STATUS_ENABLE;
	}
	/// 상태정보함수 	
	inline  bool	IsFocus()				{ return (m_dwStatus&STATUS_FOCUS)?true:false;  }
	inline  bool	IsActive()				{ return (m_dwStatus&STATUS_ACTIVE)?true:false; }
	virtual bool	IsVision()				{ return (m_dwStatus&STATUS_VISION)?true:false; }
	inline  bool    IsEnable()				{ return (m_dwStatus&STATUS_ENABLE)?true:false; }

	inline  POINT	GetPosition()			{ return m_sPosition; }
	inline	void	SetPosition( POINT pt )	{ m_sPosition = pt ; }
	inline	void	SetPosition( int x, int y) { m_sPosition.x = x; m_sPosition.y = y; }
	inline	void	SetPositionX( int x ) { m_sPosition.x = x; }
	inline	void	SetPositionY( int y ) { m_sPosition.y = y; }	


	void    SetOwnerDraw( bool b );//{ m_bOwnerDraw = b; }
	bool	IsOwnerDraw();

	///Parent 등록
	void		SetParent( CWinCtrl* pParent );
	CWinCtrl*	GetParent();
	///OffSet
	virtual void    SetOffset( POINT pt);
	virtual void	SetOffset( int x, int y );
	POINT			GetOffset();
	///
	int				GetWidth(){ return m_iWidth; }
	int				GetHeight(){ return m_iHeight; }
	void			SetWidth(int i){ m_iWidth = i; }
	void			SetHeight( int i ){ m_iHeight = i;}
	///
	virtual void    MoveWindow( POINT pt );
	
	///Register  Draw Object
	void			SetDraw( ITDraw* pDraw ){ m_pDraw = pDraw; }
	void			SetScaleWidth( float fScale );
	void			SetScaleHeight( float fScale );
	void			SetAlphaValue( BYTE btValue );


	void			AddActionListener( IActionListener* pListener );
	void			RemoveActionListener( IActionListener* pListener );

	void			SetSizeFit( bool bFit );
	bool			GetSizeFit();

protected:
	void	DrawHeader();
	void	DrawBody();
	void	DrawFooter();
};


#endif /// _CWINCTRL_	