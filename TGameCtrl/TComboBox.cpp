#include "StdAfx.h"
#include ".\tcombobox.h"
#include "TButton.h"
#include "TImage.h"
#include "TListBox.h"
#include "ITFont.h"
#include "TControlMgr.h"
CTComboBox::CTComboBox(void)
{
	m_pDropButton			= NULL;
	m_pDropBoxTopImage		= NULL;
	m_pDropBoxMiddleImage	= NULL;
	m_pDropBoxBottomImage	= NULL;
	m_pListBox				= NULL;
	m_pFontMgr				= NULL;
}

CTComboBox::~CTComboBox(void)
{
	if( m_pDropButton )
	{
		delete m_pDropButton;
		m_pDropButton = NULL;
	}
	if( m_pDropBoxTopImage )
	{
		delete m_pDropBoxTopImage;
		m_pDropBoxTopImage = NULL;
	}
	if( m_pDropBoxMiddleImage )
	{
		delete m_pDropBoxMiddleImage;
		m_pDropBoxMiddleImage = NULL;
	}
	if( m_pDropBoxBottomImage )
	{
		delete m_pDropBoxBottomImage;
		m_pDropBoxBottomImage = NULL;
	}
	if( m_pListBox )
	{
		delete m_pListBox;
		m_pListBox = NULL;
	}
}
///드랍박스 열려 있을때 자동으로 닫기위해서 WM_LBUTTONDOWN이 처리될때 RETURN 0;
unsigned int CTComboBox::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{

	if( m_pListBox && m_pListBox->Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONDOWN )
			ToggleShowDropBox();


		return m_pListBox->GetControlID();;
	}


	if( m_pDropButton && m_pDropButton->Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )	
		{
			ToggleShowDropBox();
			return m_iControlID;
		}

		//if( uiMsg == WM_LBUTTONDOWN )///다른 열려있는 콤보들을 닫기위하여 Dialog내에서 메세지 전달
		//	return 0;

		return m_pDropButton->GetControlID();
	}

	if( uiMsg == WM_LBUTTONDOWN && IsInside( LOWORD( lParam ), HIWORD( lParam ) ) )
	{
		ToggleShowDropBox();	
		return m_iControlID;
	}

	///드랍박스 열려 있을때 자동으로 닫기
	if( uiMsg == WM_LBUTTONDOWN && m_pListBox && m_pListBox->IsVision() )
	{
		ToggleShowDropBox();
		return m_iControlID;
	}

	return 0;
}

void CTComboBox::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	if( m_pListBox )
		m_pListBox->Update( ptMouse );

	if( m_pDropButton )
		m_pDropButton->Update( ptMouse );


	return;
}
void CTComboBox::DrawSelectedItem()
{
	if( m_pListBox )
	{
		const char* pDrawText = m_pListBox->GetSelectedItem();

		if( pDrawText && m_pFontMgr )
		{
			m_pFontMgr->SetTransformSprite( (float) m_sPosition.x, (float) m_sPosition.y );
			if( strlen(pDrawText) > 0 )
				m_pFontMgr->Draw( 0, true, 2, 2, D3DCOLOR_RGBA(255,255,255,255), pDrawText );		
		}
	}
}
void CTComboBox::Draw()
{
	if( !IsVision() ) return;

	DrawSelectedItem();

	if( m_pDropButton )
		m_pDropButton->Draw();

	if( !m_bOwnerDraw )
		OwnerDraw();

}

void CTComboBox::OwnerDraw()
{
	if( !IsVision() ) return;

	DrawDropBoxImage();

	if( m_pListBox )
		m_pListBox->Draw();

}

bool	CTComboBox::Create(int iScrX, int iScrY, int iWidth, int iHeight)
{
	m_pFontMgr = CTControlMgr::GetInstance()->GetFontMgr();
	Init( CTRL_COMBOBOX, iScrX, iScrY, iWidth, iHeight );
	return true;
}

void	CTComboBox::SetButton( CTButton* pButton )
{
	m_pDropButton = pButton;
}

void	CTComboBox::SetImage( CTImage* pTop, CTImage* pMiddle, CTImage* pBottom )
{
	m_pDropBoxTopImage	= pTop;
	m_pDropBoxMiddleImage = pMiddle;
	m_pDropBoxBottomImage = pBottom;
}

void	CTComboBox::SetListBox( CTListBox* pListBox )
{
	m_pListBox = pListBox;
}
void	CTComboBox::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );

	if( m_pListBox )
		m_pListBox->MoveWindow( pt );
	if( m_pDropButton )
		m_pDropButton->MoveWindow( pt );

}
void	CTComboBox::Show()
{
	CWinCtrl::Show();
	if( m_pDropButton )
		m_pDropButton->Show();

	if( m_pDropBoxTopImage )
		m_pDropBoxTopImage->Hide();

	if( m_pDropBoxMiddleImage )
		m_pDropBoxMiddleImage->Hide();

	if( m_pDropBoxBottomImage )
		m_pDropBoxBottomImage->Hide();

	if( m_pListBox )
		m_pListBox->Hide();
}

void	CTComboBox::Hide()
{
	CWinCtrl::Hide();
	if( m_pDropButton )
		m_pDropButton->Hide();

	if( m_pDropBoxTopImage )
		m_pDropBoxTopImage->Hide();

	if( m_pDropBoxMiddleImage )
		m_pDropBoxMiddleImage->Hide();

	if( m_pDropBoxBottomImage )
		m_pDropBoxBottomImage->Hide();

	if( m_pListBox )
		m_pListBox->Hide();

}

void	CTComboBox::SetOffset( POINT pt )
{
	CWinCtrl::SetOffset( pt );
	POINT ptOffset = {0,0};
	if( m_pDropButton )
	{
		ptOffset = m_pDropButton->GetPosition();	

		ptOffset.x += GetOffset().x;
		ptOffset.y += GetOffset().y;

		m_pDropButton->SetOffset( ptOffset );
	}
}

void	CTComboBox::ToggleShowDropBox()
{
	if( m_pListBox && m_pDropBoxTopImage && m_pDropBoxMiddleImage && m_pDropBoxBottomImage )
	{
		if( m_pListBox->IsVision() )
		{
			m_pDropBoxTopImage->Hide();
			m_pDropBoxMiddleImage->Hide();
			m_pDropBoxBottomImage->Hide();
			m_pListBox->Hide();
		}
		else if( m_pListBox->GetMaximum() > 1)
		{
			m_pDropBoxTopImage->Show();
			m_pDropBoxMiddleImage->Show();
			m_pDropBoxBottomImage->Show();
			m_pListBox->Show();
			SetMouseExclusiveCtrl( this );
		}
	}
}

void	CTComboBox::AddItem( char* szItem )
{
	if( m_pListBox )
	{
		m_pListBox->AppendText( szItem, D3DCOLOR_RGBA(255,255,255,255) );
		m_pListBox->SetSelectedItem( 0 );
		m_pListBox->SetValue(0);
	}

}

short		CTComboBox::GetSelectedItemID()
{
	short iRet = -1;
	if( m_pListBox )
		iRet = m_pListBox->GetSelectedItemID();
	
	return iRet;
}

const char*	CTComboBox::GetSelectedItem()
{
	if( m_pListBox )
		return m_pListBox->GetSelectedItem();
	return NULL;
}
void CTComboBox::SetSelectedItem( short i )
{
	if( m_pListBox )
		m_pListBox->SetSelectedItem( i );
}
bool CTComboBox::IsShowDropBox()
{
	if( m_pListBox && m_pListBox->IsVision() )
		return true;

	return false;
}

const   li_item_vec& CTComboBox::GetList()
{
	if( m_pListBox )
		return m_pListBox->GetList();

	return m_TempList;
}

void CTComboBox::DrawDropBoxImage()
{
	if( m_pListBox && m_pDropBoxTopImage && m_pDropBoxMiddleImage && m_pDropBoxBottomImage )
	{
		int iMaximum = m_pListBox->GetMaximum();
		
		if( iMaximum <= 1 )///아이템이 없을경우는 그릴것이 없다.
			return;

		m_pListBox->SetExtent( iMaximum );///
		
		POINT ptNew = GetPosition();
		ptNew.y += m_iHeight;
		m_pDropBoxTopImage->MoveWindow( ptNew );
		m_pDropBoxTopImage->Draw();
		ptNew = m_pDropBoxTopImage->GetPosition();
		ptNew.y += m_pDropBoxTopImage->GetHeight();

		
		/// 리스트 박스 위치 조정 
		POINT ptListBox = ptNew;
		ptListBox.x += 3;
		m_pListBox->MoveWindow( ptListBox );
		int iListBoxHeight = 0;

		for( int i = 0; i < iMaximum ; ++i )
		{
			m_pDropBoxMiddleImage->MoveWindow( ptNew );
			m_pDropBoxMiddleImage->Draw();
			ptNew.y += m_pDropBoxMiddleImage->GetHeight();
			iListBoxHeight += m_pDropBoxMiddleImage->GetHeight();
		}
		m_pListBox->SetHeight( iListBoxHeight );
		m_pDropBoxBottomImage->MoveWindow( ptNew );
		m_pDropBoxBottomImage->Draw();


	}
}

void CTComboBox::ClearItem()
{
	m_pListBox->ClearText();
}
