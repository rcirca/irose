#include "StdAfx.h"
#include ".\tmsgbox.h"
#include "TListBox.h"
#include "TButton.h"
#include "TImage.h"
#include "TCaption.h"
#include "TControlMgr.h"
#include "ITFont.h"
const short LINESPACE_BUTTONDROW = 2;///버튼을 그리기 위한 여유 라인수 
CTMsgBox::CTMsgBox(void)
{
	m_iExtraHeight		= 0;
	m_iExtraHeightStartPoint = 0;

	m_iButtonType		= BT_OK;
	m_pButtonOk			= NULL;
	m_pButtonCancel		= NULL;
	m_pCaption			= NULL;
	m_pImageTop			= NULL;
	m_pImageMiddle		= NULL;
	m_pImageBottom		= NULL;
//	m_iDialogID			= 0;
	ZeroMemory( &m_ptCaptionClicked, sizeof( POINT ));
}

CTMsgBox::~CTMsgBox(void)
{
	if( m_pButtonOk )
	{
		delete m_pButtonOk;
		m_pButtonOk = NULL;
	}
	if( m_pButtonCancel )
	{
		delete m_pButtonCancel;
		m_pButtonCancel = NULL;
	}
	
	if( m_pImageTop )
	{
		delete m_pImageTop;
		m_pImageTop = NULL;
	}
	if( m_pImageMiddle )
	{
		delete m_pImageMiddle; 
		m_pImageMiddle = NULL;
	}
	if( m_pImageBottom )
	{
		delete m_pImageBottom;
		m_pImageBottom = NULL;
	}
}

unsigned int CTMsgBox::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	if( !IsVision() )
		return 0;

	POINT ptMouse = { LOWORD(lParam), HIWORD(lParam ) };

	if( m_pCaption )
	{
		m_pCaption->Process( uiMsg, wParam, lParam );
		if( m_pCaption->IsClicked() && uiMsg == WM_LBUTTONDOWN )
		{
			m_ptCaptionClicked.x = ptMouse.x - m_sPosition.x;
			m_ptCaptionClicked.y = ptMouse.y - m_sPosition.y;
		}
	}


	if( m_pButtonOk && m_pButtonOk->Process( uiMsg, wParam, lParam))
		return m_pButtonOk->GetControlID();

	if(	m_pButtonCancel  && m_pButtonCancel->Process( uiMsg, wParam, lParam))
		return m_pButtonCancel->GetControlID();


	if( IsInside( ptMouse.x, ptMouse.y ))
		return m_iControlID;
	

	return 0;
}

void CTMsgBox::Update( POINT ptMouse )
{
	if( !IsVision() )
		return;
	if( m_pCaption )
	{
		if(m_pCaption->IsClicked())
		{
			POINT ptNew = {0,0 };
			ptNew.x = ptMouse.x - m_ptCaptionClicked.x;
			ptNew.y = ptMouse.y - m_ptCaptionClicked.y;
			MoveWindow( ptNew );
		}

		m_pCaption->Update( ptMouse );
	}

	if( m_pButtonOk )
		m_pButtonOk->Update( ptMouse );
	if(	m_pButtonCancel )
		m_pButtonCancel->Update( ptMouse );
}
/*
void CTMsgBox::SetCaption( CTCaption* pCaption )
{
	pCaption->SetParent( GetControlID() );
	pCaption->SetOffset( pCaption->GetPosition() );
	pCaption->MoveWindow( GetPosition() );

	m_pCaption = pCaption;
}
*/
void CTMsgBox::Draw()
{
	if( !IsVision() )
		return;

	DrawImages();

	if( int iStringCount = m_message_parser.GetStringCount() )
	{
		CTControlMgr::GetInstance()->GetFontMgr()->SetTransformSprite( (float)m_sPosition.x, (float)m_sPosition.y );

		int y = 30;
		CJString* pJString = NULL;

		for( int i = 0; i < iStringCount ; ++i )
		{
			if( pJString = m_message_parser.GetString( i ) )
				pJString->Draw( 15, y, false );

			y += 22;
		}
	}


	if( m_pButtonOk )
		m_pButtonOk->Draw();
	if(	m_pButtonCancel )
		m_pButtonCancel->Draw();
}

/// 버튼들의 상태와 x좌표결정
void CTMsgBox::SetButtonType( short int iType )
{
	m_iButtonType = iType;
	POINT ptOffset = {0,0};
	switch( m_iButtonType )
	{
	case ( BT_OK | BT_CANCEL ):
		{
			if( m_pButtonOk )
			{
				ptOffset   = m_pButtonOk->GetOffset();
				ptOffset.x = m_iWidth / 4 - m_pButtonOk->GetWidth() / 2;
				m_pButtonOk->SetOffset( ptOffset );

				m_pButtonOk->Show();
			}
			if( m_pButtonCancel )
			{
				ptOffset   = m_pButtonCancel->GetOffset();
				ptOffset.x = (m_iWidth * 3) / 4 - m_pButtonCancel->GetWidth() / 2;
				m_pButtonCancel->SetOffset( ptOffset );
			
				m_pButtonCancel->Show();
			}
			break;
		}
	case BT_OK:
		{
			if( m_pButtonOk )
			{
				m_pButtonOk->Show();

				ptOffset   = m_pButtonOk->GetOffset();
				ptOffset.x = m_iWidth / 2 - m_pButtonOk->GetWidth() / 2;
				m_pButtonOk->SetOffset( ptOffset );
			}

			if( m_pButtonCancel )
				m_pButtonCancel->Hide();
			break;
		}
	case BT_CANCEL:
		{
			if( m_pButtonCancel )
			{
				m_pButtonCancel->Show();

				ptOffset   = m_pButtonCancel->GetOffset();
				ptOffset.x = m_iWidth / 2 - m_pButtonCancel->GetWidth() / 2;
				m_pButtonCancel->SetOffset( ptOffset );
			}

			if( m_pButtonOk )
				m_pButtonOk->Hide();
			break;
		}
	default:
		break;
	}
}

void CTMsgBox::SetString( const char* szString )
{
	m_message_parser.Clear();
	m_message_parser.SetString( szString, m_iWidth - 30 );

	///버튼들의 Y- Offset 및 박스의 높이 결정
	if( m_pImageTop && m_pImageMiddle && m_pImageBottom )
	{
		int iBoxHeight = 0;
		POINT ptOffset = {0,0};

		POINT ptDraw = {0,0};
		ptDraw.y += m_pImageTop->GetHeight();
		
		iBoxHeight += m_pImageTop->GetHeight();

		for( int i = 0; i < m_message_parser.GetStringCount() /*+ LINESPACE_BUTTONDROW*/; ++i )
		{
			ptDraw.y += m_pImageMiddle->GetHeight();
			iBoxHeight += m_pImageMiddle->GetHeight();
		}
		
		if( m_iExtraHeight )
			m_iExtraHeightStartPoint = iBoxHeight;
		
		///버튼들의 Y좌표 재계산
		if( m_pButtonOk && m_pImageBottom )
		{
			ptOffset   = m_pButtonOk->GetOffset();
			ptOffset.y = ptDraw.y + m_pImageBottom->GetHeight() / 2 - m_pButtonOk->GetHeight() / 2 + m_iExtraHeight;
			m_pButtonOk->SetOffset( ptOffset );
		}

		if( m_pButtonCancel && m_pImageBottom )
		{
			ptOffset	= m_pButtonCancel->GetOffset();
			ptOffset.y	= ptDraw.y + m_pImageBottom->GetHeight() / 2 - m_pButtonOk->GetHeight() / 2 + m_iExtraHeight;
			m_pButtonCancel->SetOffset( ptOffset );
		}

		iBoxHeight += m_pImageTop->GetHeight();

		///메세지 박스의 전체 높이 결정
		SetHeight( iBoxHeight + m_iExtraHeight );
		///재계산된 Offset을 이용하여 버튼들의 위치 재조정 
		MoveWindow(GetPosition());
	}
}

void CTMsgBox::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	
	if( m_pCaption )
		m_pCaption->MoveWindow( pt );

	if( m_pButtonOk )
		m_pButtonOk->MoveWindow( pt );

	if( m_pButtonCancel )
		m_pButtonCancel->MoveWindow( pt );
}

void CTMsgBox::SetButtons( CTButton* pBtnOk, CTButton* pBtnCancel )
{
	m_pButtonOk		= pBtnOk;
	m_pButtonCancel = pBtnCancel;
	SetButtonType( m_iButtonType );
}

void CTMsgBox::SetImages( CTImage* pImageTop, CTImage* pImageMiddle, CTImage* pImageBottom )
{
	m_pImageTop		= pImageTop;
	m_pImageMiddle	= pImageMiddle;
	m_pImageBottom	= pImageBottom;
}


/// 2003 /12 / 11 / Navy
/// 세단계로 나누어진 이미지들을 그린다.
/// 버튼의 위치도 생각할것
/// 매프레임마다 처리되는데 해야할 작업이 너무 많다. 정리 요
/// y좌표결정
void CTMsgBox::DrawImages()
{
	if( m_pImageTop && m_pImageMiddle && m_pImageBottom )
	{
		POINT ptDraw = GetPosition();

		///상단이미지 그리기 
		m_pImageTop->MoveWindow( ptDraw );
		m_pImageTop->Draw();


		ptDraw.y += m_pImageTop->GetHeight();

		/// 중단이미지 그리기
		for( int i = 0; i < m_message_parser.GetStringCount() /*+ LINESPACE_BUTTONDROW*/; ++i )
		{
			m_pImageMiddle->MoveWindow( ptDraw );
			m_pImageMiddle->Draw();

			ptDraw.y += m_pImageMiddle->GetHeight();
		}
		///ExtraHeight그리기
		if( m_iExtraHeight > 0)
		{
			for( int i = 0; i < m_iExtraHeight / m_pImageMiddle->GetHeight() + 1; ++i )
			{
				m_pImageMiddle->MoveWindow( ptDraw );
				m_pImageMiddle->Draw();

				ptDraw.y += m_pImageMiddle->GetHeight();
			}
		}
		/// 하단이미지 그리기
		m_pImageBottom->MoveWindow( ptDraw );
		m_pImageBottom->Draw();
	}
}

///버튼 타입에 따라 버튼의 Show처리
void CTMsgBox::Show()
{
	CTDialog::Show();

	switch( m_iButtonType )
	{
	case ( BT_OK | BT_CANCEL ):
		{
			if( m_pButtonOk )
				m_pButtonOk->Show();
			if( m_pButtonCancel )
				m_pButtonCancel->Show();

			break;
		}
	case BT_OK:
		{
			if( m_pButtonOk )
				m_pButtonOk->Show();

			if( m_pButtonCancel )
				m_pButtonCancel->Hide();
			break;
		}
	default:
		break;
	}

	if( m_pImageTop )
		m_pImageTop->Show();
	if( m_pImageMiddle )
		m_pImageMiddle->Show();
	if( m_pImageBottom )
		m_pImageBottom->Show();
	if( m_pCaption )
		m_pCaption->Show();

}

void CTMsgBox::Hide()
{
	CTDialog::Hide();
	if( m_pButtonOk )
		m_pButtonOk->Hide();

	if( m_pButtonCancel )
		m_pButtonCancel->Hide();

	if( m_pImageTop )
		m_pImageTop->Hide();
	if( m_pImageMiddle )
		m_pImageMiddle->Hide();
	if( m_pImageBottom )
		m_pImageBottom->Hide();
	if( m_pCaption )
		m_pCaption->Hide();

}

bool CTMsgBox::Create( char* szIDD )
{
	return CTControlMgr::GetInstance()->MakeMsgBoxByXML( szIDD, this );
}

void CTMsgBox::AddExtraHeight( int iHeight )
{
	_ASSERT( m_iHeight == 0 );
	_ASSERT( m_iExtraHeight == 0 );
	m_iExtraHeight = iHeight;
	SetHeight( GetHeight() + m_iExtraHeight );
	POINT ptOffset;

	if( m_pButtonOk )
	{
		ptOffset	=  m_pButtonOk->GetOffset();
		ptOffset.y	+= m_iExtraHeight;
		m_pButtonOk->SetOffset( ptOffset );
	}

	if( m_pButtonCancel )
	{
		ptOffset	=  m_pButtonCancel->GetOffset();
		ptOffset.y	+= m_iExtraHeight;
		m_pButtonCancel->SetOffset( ptOffset );
	}
}

int CTMsgBox::GetExtraHeightStartPoint()
{
	return m_iExtraHeightStartPoint;
}