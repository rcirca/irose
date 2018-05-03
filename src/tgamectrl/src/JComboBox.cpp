#include "StdAfx.h"
#include ".\jcombobox.h"
#include "TButton.h"
#include "TImage.h"
#include "tgamectrl/jlistbox.h"
#include "ITFont.h"
#include "TControlMgr.h"
#include <assert.h>

CJComboBox::CJComboBox(void)
{
	m_pDropButton			= NULL;
	m_pDropBoxTopImage		= NULL;
	m_pDropBoxMiddleImage	= NULL;
	m_pDropBoxBottomImage	= NULL;
	m_pListBox				= NULL;
	m_pFontMgr				= NULL;
}

CJComboBox::~CJComboBox(void)
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
unsigned int CJComboBox::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{

	if( m_pListBox && m_pListBox->Process( uiMsg, wParam, lParam ) )
		return m_iControlID;


	if( m_pDropButton && m_pDropButton->Process( uiMsg, wParam, lParam ) )
	{
		if( uiMsg == WM_LBUTTONUP )	
		{
			ToggleShowDropBox();
			return 0;///다른 열려있는 콤보들을 닫기위하여 메세지 전달
		}

		return m_pDropButton->GetControlID();
	}


	///드랍박스 열려 있을때 자동으로 닫기
	if( uiMsg == WM_LBUTTONUP && m_pListBox && m_pListBox->IsVision() )
		ToggleShowDropBox();
	return 0;
}

void CJComboBox::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	if( m_pListBox )
		m_pListBox->Update( ptMouse );

	if( m_pDropButton )
		m_pDropButton->Update( ptMouse );


	return;
}
void CJComboBox::DrawSelectedItem()
{
	assert( m_pListBox );
	if( m_pListBox )
	{
		const CTObject* pItem = m_pListBox->GetSelectedItem();
		
		if( pItem )
		{
			POINT ptDraw = { 2, 0 };
			m_pFontMgr->SetTransformSprite( (float) m_sPosition.x, (float) m_sPosition.y );
			CWinCtrl* pDrawComponent = m_pListBox->GetRendererComponent( pItem, false, false );
			pDrawComponent->SetPosition( ptDraw );
			pDrawComponent->Draw();
		}
	}
}

void CJComboBox::Draw()
{
	if( !IsVision() ) return;

	DrawSelectedItem();

	if( m_pDropButton )
		m_pDropButton->Draw();

	if( !m_bOwnerDraw )
		OwnerDraw();
}

void CJComboBox::OwnerDraw()
{
	DrawDropBoxImage();

	if( m_pListBox )
		m_pListBox->Draw();

}

CJListBox*	CJComboBox::GetListBoxPtr()
{
	return m_pListBox;
}

bool	CJComboBox::Create(int iScrX, int iScrY, int iWidth, int iHeight)
{
	m_pFontMgr = CTControlMgr::GetInstance()->GetFontMgr();
	Init( CTRL_JCOMBOBOX, iScrX, iScrY, iWidth, iHeight );
	return true;
}

void	CJComboBox::SetButton( CTButton* pButton )
{
	m_pDropButton = pButton;
}

void	CJComboBox::SetImage( CTImage* pTop, CTImage* pMiddle, CTImage* pBottom )
{
	m_pDropBoxTopImage	= pTop;
	m_pDropBoxMiddleImage = pMiddle;
	m_pDropBoxBottomImage = pBottom;
}

void	CJComboBox::SetListBox( CJListBox* pListBox )
{
	m_pListBox = pListBox;
}

void	CJComboBox::MoveWindow( POINT pt )
{
	CWinCtrl::MoveWindow( pt );

	if( m_pListBox )
		m_pListBox->MoveWindow( pt );
	if( m_pDropButton )
		m_pDropButton->MoveWindow( pt );

}
void	CJComboBox::Show()
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

void	CJComboBox::Hide()
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

void	CJComboBox::SetOffset( POINT pt )
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

void	CJComboBox::ToggleShowDropBox()
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
		else
		{
			m_pDropBoxTopImage->Show();
			m_pDropBoxMiddleImage->Show();
			m_pDropBoxBottomImage->Show();
			m_pListBox->Show();
			SetMouseExclusiveCtrl( this );
		}
	}
}

void	CJComboBox::AddItem( CTObject* pItem )
{
	assert( m_pListBox );
	assert( pItem );

	if( m_pListBox && pItem )
		m_pListBox->AddItem( pItem );
}

unsigned		CJComboBox::GetSelectedItemIndex()
{
	assert( m_pListBox );
	if( m_pListBox )
		return m_pListBox->GetSelectedItemIndex();

	return 0;
}

const CTObject*	CJComboBox::GetSelectedItem()
{
	assert( m_pListBox );
	if( m_pListBox )
		return m_pListBox->GetSelectedItem();

	return NULL;
}

bool CJComboBox::IsShowDropBox()
{
	if( m_pListBox && m_pListBox->IsVision() )
		return true;

	return false;
}


void CJComboBox::DrawDropBoxImage()
{
	if( m_pListBox && m_pDropBoxTopImage && m_pDropBoxMiddleImage && m_pDropBoxBottomImage )
	{
		int iMaximum = m_pListBox->GetSize();
		
		if( iMaximum < 1 )///아이템이 없을경우는 그릴것이 없다.
			return;

	
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

void CJComboBox::ClearItem()
{
	assert( m_pListBox );
	if( m_pListBox )
		m_pListBox->Clear();
}

void CJComboBox::SelectItem( unsigned iSubscript )
{
	assert( m_pListBox );
	if( m_pListBox )
		m_pListBox->SelectItem( iSubscript );
}