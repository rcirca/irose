#include "StdAfx.h"
#include ".\tscrollbar.h"
#include "TButton.h"
#include "TScrollBox.h"
#include "IScrollModel.h"
#include <zmouse.h>
CTScrollBar::CTScrollBar(void)
{
	m_pPrevButton	= NULL;
	m_pNextButton	= NULL;
	m_pScrollBox	= NULL;
	m_pScrollModel	= NULL;
	m_pScrollBarType = NULL;
}

CTScrollBar::~CTScrollBar(void)
{
	if( m_pScrollBarType )
	{
		delete m_pScrollBarType;
		m_pScrollBarType = NULL;
	}

	if( m_pPrevButton )
	{
		delete m_pPrevButton;
		m_pPrevButton = NULL;
	}
	if( m_pNextButton )
	{
		delete m_pNextButton;
		m_pNextButton = NULL;
	}
	if( m_pScrollBox )
	{
		delete m_pScrollBox;
		m_pScrollBox = NULL;
	}
}

bool CTScrollBar::Create( int iScrX, int iScrY, int iWidth, int iHeight ,int iType)
{
	switch( iType )
	{
	case CTScrollBarType::TSBT_VERTICAL:
		m_pScrollBarType = new CTScrollBarTypeVertical;
		break;
	case CTScrollBarType::TSBT_HORIZONTAL:
		m_pScrollBarType = new CTScrollBarTypeHorizontal;
		break;
	default:
		m_pScrollBarType = new CTScrollBarTypeVertical;
		break;
	}
	Init( CTRL_SCROLLBAR, iScrX, iScrY, iWidth, iHeight );
	return true;
}

unsigned int CTScrollBar::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision()) return 0;

	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };

	int iValue  = 0;
	int iExtent = 0;
	int iMax    = 0;

	if( uiMsg == WM_LBUTTONUP )
		iValue = 0;

	if( m_pScrollModel )
	{
		iValue  = m_pScrollModel->GetValue();
		iExtent = m_pScrollModel->GetExtent();
		iMax    = m_pScrollModel->GetMaximum();
	}

	if( iMax <= iExtent )
		return 0;

	if( m_pPrevButton && m_pPrevButton->Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )		
		{
			iValue--;
			SetValue( iValue );
		}
		return m_pPrevButton->GetControlID();
	}

	if( m_pNextButton && m_pNextButton->Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )		
		{
			iValue++;
			SetValue( iValue );
		}
		return m_pNextButton->GetControlID();
	}

	if( m_pScrollBox && m_pScrollBox->Process( uiMsg, wParam, lParam ))
		return m_pScrollBox->GetControlID();

	///영역안에서 
	if( uiMsg == WM_LBUTTONDOWN && m_pScrollBox && IsInside( ptMouse.x, ptMouse.y ) )
	{
		m_pScrollBox->MoveBoxPositionByScreenPoint( ptMouse );
	}
	if( uiMsg == WM_MOUSEWHEEL  )
	{
		RECT rcModel = {0,0,0,0};
		
		if( m_pScrollModel )
		{
			rcModel = m_pScrollModel->GetWindowRect();
			if( PtInRect( &rcModel, ptMouse ) == 0 && !IsInside( ptMouse.x, ptMouse.y) )
				return 0;
		}
		int zDelta = (int)wParam;
		if( zDelta > 0)
			SetValue( --iValue );
		else
			SetValue( ++iValue );
		return m_iControlID;
	}
	return 0;
}

void CTScrollBar::Update( POINT ptMouse )
{
	if( !IsVision()) return;

	if( m_pScrollModel )
	{
		if( m_pScrollBox )
		{
			if( false == m_pScrollModel->IsLastItemDrawn() )
				m_pScrollBox->SetBlink();
			else
				m_pScrollBox->SetUnblink();
		}
		
		int iExtent = m_pScrollModel->GetExtent();
		int iMax    = m_pScrollModel->GetMaximum();

		if( iMax <= iExtent )
			return ;


	}


	if( m_pPrevButton )
		m_pPrevButton->Update( ptMouse );

	if( m_pNextButton )
		m_pNextButton->Update( ptMouse );

	if( m_pScrollBox )
		m_pScrollBox->Update( ptMouse );

}
void CTScrollBar::Draw()
{
	if( !IsVision()) return;

	if( m_pPrevButton )
		m_pPrevButton->Draw();

	if( m_pNextButton )
		m_pNextButton->Draw();

	if( m_pScrollModel )
	{
		int iExtent = m_pScrollModel->GetExtent();
		int iMax    = m_pScrollModel->GetMaximum();

		if( iMax <= iExtent )
			return ;
	}

	if( m_pScrollBox )
		m_pScrollBox->Draw();
}
void CTScrollBar::SetPrevButton( CTButton* pButton )
{
	m_pPrevButton = pButton;
}

void CTScrollBar::SetNextButton( CTButton* pButton )
{
	m_pNextButton = pButton;
}

void CTScrollBar::SetScrollBox( CTScrollBox* pThumb )
{
	m_pScrollBox = pThumb;
	m_pScrollBox->SetScrollBar( this );
}

void CTScrollBar::SetOffset( POINT pt )
{
	CWinCtrl::SetOffset( pt );
	SIZE sizeScrollBar = { m_iWidth, m_iHeight };
	SIZE sizePrevButton, sizeNextButton, sizeScrollBox;

	POINT	ptNewOffset = {0,0};


	if( m_pPrevButton )
	{
		sizePrevButton.cx  = m_pPrevButton->GetWidth();
		sizePrevButton.cy  = m_pPrevButton->GetHeight();
		if( m_pScrollBarType )
			ptNewOffset = m_pScrollBarType->GetPrevButtonOffset( GetOffset(), sizeScrollBar, sizePrevButton );

		m_pPrevButton->SetOffset( ptNewOffset );
	}

	if( m_pNextButton )
	{
		sizeNextButton.cx = m_pNextButton->GetWidth();
		sizeNextButton.cy = m_pNextButton->GetHeight();

		if( m_pScrollBarType )
			ptNewOffset = m_pScrollBarType->GetNextButtonOffset( GetOffset(), sizeScrollBar, sizeNextButton );

		m_pNextButton->SetOffset( ptNewOffset );
	}

	if( m_pScrollBox )
	{
		sizePrevButton.cx = 0;
		sizePrevButton.cy = 0;
		if( m_pPrevButton )
		{
			sizePrevButton.cx = m_pPrevButton->GetWidth();
			sizePrevButton.cy = m_pPrevButton->GetHeight();
		}

		sizeScrollBox.cx  = m_pScrollBox->GetWidth();
		sizeScrollBox.cy  = m_pScrollBox->GetHeight();

		if( m_pScrollBarType )
			ptNewOffset = m_pScrollBarType->GetScrollBoxOffset( GetOffset(), sizeScrollBar, sizePrevButton, sizeScrollBox );

		m_pScrollBox->SetOffset( ptNewOffset );
	}
}

void CTScrollBar::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );

	if( m_pPrevButton )
		m_pPrevButton->MoveWindow( pt );

	if( m_pNextButton )
		m_pNextButton->MoveWindow( pt );

	if( m_pScrollBox )
	{
		m_pScrollBox->MoveWindow( pt );
		if( m_pScrollModel )
			m_pScrollBox->MoveBoxPosition( m_pScrollModel->GetValue(), m_pScrollModel->GetMaximum(), m_pScrollModel->GetExtent() );
	}
}

RECT CTScrollBar::GetThumbMoveRange()
{
	RECT rc = {0,0,0,0};
	if( m_pScrollBox == NULL || m_pScrollBarType == NULL )
		return rc;

	SIZE sizeScrollBar  = { m_iWidth, m_iHeight };
	SIZE sizePrevButton;
	
	if( m_pPrevButton )
	{
		sizePrevButton.cx = m_pPrevButton->GetWidth();
		sizePrevButton.cy = m_pPrevButton->GetHeight();
	}
	else
	{
		sizePrevButton.cy = 0;
		sizePrevButton.cx = 0;
	}

	SIZE sizeScrollBox  = { m_pScrollBox->GetWidth(), m_pScrollBox->GetHeight() };

	POINT pt;
	if( m_pNextButton )
		pt = m_pNextButton->GetPosition();
	else
	{
		pt.x = m_sPosition.x + m_iWidth;
		pt.y = m_sPosition.y + m_iHeight;
	}

	rc = m_pScrollBarType->GetThumbMoveRange( m_sPosition, pt, sizeScrollBar, sizePrevButton, sizeScrollBox );

	return rc;
}

int CTScrollBar::GetExtent()
{
	if( m_pScrollModel == NULL ) 
		return 0;
	return m_pScrollModel->GetExtent();
}

int CTScrollBar::GetMaximum()
{
	if( m_pScrollModel == NULL ) 
		return 0;

	return m_pScrollModel->GetMaximum();
}

void CTScrollBar::SetValue(int iValue)
{
	if( m_pScrollModel )
		m_pScrollModel->SetValue( iValue );
	if( m_pScrollBox )
		m_pScrollBox->MoveBoxPosition( iValue, m_pScrollModel->GetMaximum(), m_pScrollModel->GetExtent() );

}

void CTScrollBar::SetModelValue( int i )
{
	if( m_pScrollModel != NULL ) 
		m_pScrollModel->SetValue( i );
}

void CTScrollBar::SetModel( IScrollModel* pModel )
{
	m_pScrollModel = pModel;
}
void CTScrollBar::Show()
{
	CWinCtrl::Show();
	if( m_pPrevButton )
		m_pPrevButton->Show();

	if( m_pNextButton )
		m_pNextButton->Show();

	if( m_pScrollBox )
		m_pScrollBox->Show();
}

void CTScrollBar::Hide()
{
	CWinCtrl::Hide();
	if( m_pPrevButton )
		m_pPrevButton->Hide();

	if( m_pNextButton )
		m_pNextButton->Hide();

	if( m_pScrollBox )
		m_pScrollBox->Hide();
}

void CTScrollBar::MoveEnd()
{
	if( m_pScrollModel && m_pScrollBox )
		SetValue( m_pScrollModel->GetMaximum());

}

void CTScrollBar::MoveHome()
{
	SetValue( 0 );
}
