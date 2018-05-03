#include "StdAfx.h"
#include ".\tscrollbox.h"
#include "TScrollBar.h"
#include "TControlMgr.h"
#include "TScrollBoxType.h"
#include "TImage.h"
CTScrollBox::CTScrollBox(void)
{
	m_bClicked   = false;
	m_iGraphicID = 0;
	m_pScrollBar = NULL;
//	m_iSelectDiff = 0;
	m_pDraw = NULL;
	m_pScrollBoxType = NULL;
	m_bBlink = false;
	m_pBlinkImage = NULL;
}

CTScrollBox::~CTScrollBox(void)
{
	if( m_pScrollBoxType )
	{
		delete m_pScrollBoxType;
		m_pScrollBoxType = NULL;
	}
}

bool CTScrollBox::Create(int iWidth, int iHeight, int iGraphicID,int iModuleID,int iType  )
{
	Init( CTRL_SCROLLBOX, 0, 0, iWidth, iHeight );
	m_iGraphicID = iGraphicID;
	m_iModuleID  = iModuleID;
	m_pDraw		 = CTControlMgr::GetInstance()->GetDrawMgr();
	
	switch( iType )
	{
	case CTScrollBarType::TSBT_VERTICAL:
		m_pScrollBoxType = new CTScrollBoxTypeVertical;
		break;
	case CTScrollBarType::TSBT_HORIZONTAL:
		m_pScrollBoxType = new CTScrollBoxTypeHorizontal;
		break;
	default:
		m_pScrollBoxType = new CTScrollBoxTypeVertical;
		break;
	}
	return true;
}
unsigned int CTScrollBox::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() ) return 0;
//	if( !IsInside( m_ptCurrMouse.x, m_ptCurrMouse.y ))
//		return 0;
	
	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };
	
	if( uiMsg == WM_LBUTTONDOWN && IsInside( ptMouse.x, ptMouse.y ))
	{
		POINT ptOffset = { ptMouse.x - m_sPosition.x, ptMouse.y - m_sPosition.y };
		m_pScrollBoxType->SetOffset( ptOffset );
		m_bClicked = true;
		CWinCtrl::SetMouseExclusiveCtrl( this );
		return uiMsg;
	}
	if( uiMsg == WM_LBUTTONUP && m_bClicked )
	{
		m_bClicked = false;
		return uiMsg;
	}

	return 0;
}

void CTScrollBox::Update( POINT ptMouse)
{
	if( m_pBlinkImage )
		m_pBlinkImage->Update( ptMouse );

	if( m_bClicked )
	{
//		POINT ptTemp = ptMouse;	
//		ptTemp.y -= m_iSelectDiff;
		if( IsExclusive() && GetMouseExclusiveCtrl() == (CWinCtrl*)this )
			MoveBoxPositionByScreenPoint( ptMouse );
		else
			m_bClicked = false;
	}
}

void CTScrollBox::Draw()
{
	if( !IsVision() ) return;

	if( m_pDraw )
	{
		if( !m_bBlink )
		{
			m_pDraw->Draw(m_sPosition.x, m_sPosition.y, m_iModuleID, m_iGraphicID );
		}
		else
		{
			if( m_pBlinkImage )
				m_pBlinkImage->Draw( m_sPosition );
			else
				m_pDraw->Draw(m_sPosition.x, m_sPosition.y, m_iModuleID, m_iGraphicID );
		}
	}
}

void CTScrollBox::MoveBoxPosition( int iValue, int iMax ,int iExtent )
{
	if( iMax == iExtent || m_pScrollBoxType == NULL )
		return;
/*

	POINT ptNew;

	ptNew.x = rcMoveRange.left;
	int iRangeHeight = rcMoveRange.bottom - rcMoveRange.top;
	if( iValue <= 0 )
		ptNew.y = rcMoveRange.top;
	else if( iValue > iMax - iExtent )
		ptNew.y = rcMoveRange.bottom;
	else
		ptNew.y = rcMoveRange.top + ( iValue * (iRangeHeight )) / ( iMax - iExtent ) ;

*/
	RECT rcMoveRange = m_pScrollBar->GetThumbMoveRange();
	POINT ptOffset   = GetOffset();
	POINT ptNew = m_pScrollBoxType->GetMoveBoxPos( rcMoveRange, iValue, iMax, iExtent );
	ptNew.x -= ptOffset.x;
	ptNew.y -= ptOffset.y;
	MoveWindow( ptNew );
}
void CTScrollBox::MoveBoxPositionByScreenPoint( POINT pt )
{
	if( m_pScrollBar == NULL || m_pScrollBoxType == NULL ) 
		return;

	RECT rcMoveRange = m_pScrollBar->GetThumbMoveRange();
	POINT ptOffset   = GetOffset();

	int iValue		 = 0;
	int iMax		 = m_pScrollBar->GetMaximum();
	int iExtent		 = m_pScrollBar->GetExtent();

	SIZE sizeScrollBox = { m_iWidth, m_iHeight };
	POINT ptNew = m_pScrollBoxType->GetMoveBoxPosByScreenPoint( sizeScrollBox, rcMoveRange, pt, iMax ,iExtent , iValue );

	ptNew.x -= ptOffset.x;
	ptNew.y -= ptOffset.y;

#ifdef _DEBUG
	static POINT sPt;
	if( sPt.y != ptNew.y )
	{
		sPt = ptNew;
		char strBuf[255];
		sprintf( strBuf, "%d, %d,   pt.x:%d, pt.y:%d \n", pt.x, pt.y,  ptNew.x, ptNew.y );
		OutputDebugString(strBuf);
	}
	
#endif

	MoveWindow( ptNew );
	m_pScrollBar->SetModelValue( iValue );

}
void CTScrollBox::SetTickMove( int i )
{
	_ASSERT( m_pScrollBoxType );
	if( m_pScrollBoxType )
	{
		if( i > 0 )
			m_pScrollBoxType->SetTickMove( true );
		else
			m_pScrollBoxType->SetTickMove( false );
	}
}
void CTScrollBox::SetBlink()
{
	m_bBlink = true;
	if( m_pBlinkImage )
		m_pBlinkImage->SetBlink();
}
void CTScrollBox::SetUnblink()
{
	m_bBlink = false;
	if( m_pBlinkImage )
		m_pBlinkImage->SetUnblink();
}
void CTScrollBox::SetBlinkImage( CTImage* pImage )
{
	m_pBlinkImage = pImage;
}