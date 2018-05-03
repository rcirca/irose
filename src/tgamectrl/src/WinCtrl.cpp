#include "StdAfx.h"
#include ".\winctrl.h"
//#include "CToolTip.h"
#include "IActionListener.h"
#include "ActionEvent.h"

CWinCtrl* CWinCtrl::m_pMouseExclusiveCtrl = NULL;   ///마우스를 독점하고 있는 컨트롤
CWinCtrl* CWinCtrl::m_pProcessMouseOverCtrl = NULL;

bool	  CWinCtrl::m_bProcessMouseOver	  = false;
CWinCtrl::CWinCtrl(void)
{
	m_iWidth		= 0;
	m_iHeight		= 0;
	m_iControlID	= 0;		/// 컨터롤 아이디
	m_iParentID		= 0;
	m_sPosition.x = 0;
	m_sPosition.y = 0;
	m_ptOffset.x  = 0;
	m_ptOffset.y  = 0;

	m_pDraw			= NULL;	
	m_pParent		= NULL;

	m_bOwnerDraw	= false;
	m_bSelected		= false;

	m_fScaleWidth   = 0;
	m_fScaleHeight  = 0;
	m_btAlphaValue  = 0;
	m_dwStatus		= 0;
}

CWinCtrl::~CWinCtrl(void)
{
}

void CWinCtrl::Init( DWORD dwType,int iScrX, int iScrY, int iWidth, int iHeight )
{

	m_dwCtrlType	= dwType;
	m_dwStatus		= 0;
	m_sPosition.x	= iScrX;
	m_sPosition.y	= iScrY;
	m_iWidth		= iWidth;
	m_iHeight		= iHeight;


	ZeroMemory( &m_ptOffset, sizeof( POINT ));

	SetEnable( true );
}

void CWinCtrl::MoveWindow( POINT pt )
{
	m_sPosition.x = pt.x + m_ptOffset.x;
	m_sPosition.y = pt.y + m_ptOffset.y;
}

bool CWinCtrl::IsInside( int x, int y )
{
	if( m_sPosition.x <= x && m_sPosition.y <= y && m_sPosition.x + m_iWidth > x && m_sPosition.y + m_iHeight > y )
		return true;
	return false;
}

bool CWinCtrl::IsExclusive()
{ 
	if( m_pMouseExclusiveCtrl)
		return true;
	else
		return false;
}
void CWinCtrl::SetMouseExclusiveCtrl( CWinCtrl* pCtrl )
{
	m_pMouseExclusiveCtrl = pCtrl;
}

CWinCtrl* CWinCtrl::GetMouseExclusiveCtrl()
{
	return m_pMouseExclusiveCtrl;
}

///OffSet
void    CWinCtrl::SetOffset( POINT pt)
{ 
	m_ptOffset = pt; 
}

void	CWinCtrl::SetOffset( int x, int y )
{ 
	m_ptOffset.x = x;
	m_ptOffset.y = y;
}

POINT	CWinCtrl::GetOffset()
{ 
	return m_ptOffset; 
}

void CWinCtrl::SetSelected()
{
	m_bSelected = true;
}
void CWinCtrl::SetDeselected()
{
	m_bSelected = false;
}

bool CWinCtrl::IsSelected()
{
	return m_bSelected;
}
void CWinCtrl::SetParent( CWinCtrl* pParent )
{
	m_pParent = pParent;
}

void CWinCtrl::SetScaleWidth( float fScale )
{
	m_fScaleWidth = fScale;
}

void CWinCtrl::SetScaleHeight( float fScale )
{
	m_fScaleHeight = fScale;
}

void CWinCtrl::SetAlphaValue( BYTE btValue )
{
	m_btAlphaValue = btValue;
}

void CWinCtrl::AddActionListener( IActionListener* pListener )
{
	m_ActionListenerList.Add( pListener );
}

void CWinCtrl::RemoveActionListener( IActionListener* pListener )
{
	m_ActionListenerList.Remove( pListener );
}

CWinCtrl* CWinCtrl::GetParent()
{
	return m_pParent;
}
unsigned int CWinCtrl::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	unsigned uiRet = 0;

	if( !m_ActionListenerList.IsEmpty() )
	{
		std::list< IActionListener* >& list =  m_ActionListenerList.GetListenerList( );
		std::list< IActionListener* >::iterator iter;
		CActionEvent Event( this, uiMsg , wParam, lParam );

		for( iter = list.begin(); iter != list.end(); ++iter )
			uiRet |= (*iter)->ActionPerformed( &Event );
	}
	return uiRet;
}

void CWinCtrl::SetOwnerDraw( bool b )
{ 
	m_bOwnerDraw = b; 
}

bool CWinCtrl::IsOwnerDraw()
{
	return m_bOwnerDraw;
}

CWinCtrl* CWinCtrl::Find( const char * szName )
{
	return NULL;
}

void	CWinCtrl::SetSizeFit( bool bFit )
{
	m_bSizeFit = bFit;
}
bool	CWinCtrl::GetSizeFit()
{
	return m_bSizeFit;
}