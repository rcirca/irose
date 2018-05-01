#include "stdafx.h"
#include ".\cbankdlg.h"
#include "CBankWindowDlg.h"
#include "../../Network/CNetwork.h"

#include "../../Object.h"
#include "../../System/CGame.h"
#include "../../GameCommon/Item.h"
#include "../../Misc/GameUtil.h"

#include "../IO_ImageRes.h"
#include "../it_mgr.h"
#include "../icon/ciconitem.h"


#include "../CDragItem.h"
#include "../command/ctcmdnumberinput.h"
#include "../command/uicommand.h"

#include "../Country.h"

#include "TRadioBox.h"
#include "TRadioButton.h"
#include "TButton.h"



CBankDlg::CBankDlg( int iType )
{
	SetDialogType( iType );

	POINT ptOffset;

	m_pDragItem = new CDragItem;
	CTCmdNumberInput* pNumberCmd = new CTCmdMoveItemBank2Inv;
	CTCmdOpenNumberInputDlg* pOpenCmd = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	
	m_pDragItem->AddTarget( DLG_TYPE_ITEM, pOpenCmd );


	for( int iPage = 0; iPage < g_iPageCount; ++iPage )
	{
		for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
		{
			ptOffset.x = 10 + ( iSlot % 8 ) * ( m_Slots[iPage][iSlot].GetWidth()  + 1);
			ptOffset.y = 50 + ( iSlot / 8 ) * ( m_Slots[iPage][iSlot].GetHeight() );

			m_Slots[iPage][iSlot].SetOffset( ptOffset );
			m_Slots[iPage][iSlot].SetParent( iType );
			m_Slots[iPage][iSlot].SetDragAvailable();
			m_Slots[iPage][iSlot].SetDragItem( m_pDragItem );
		}
	}
	
	m_iCurrBankPage = 0;
}

CBankDlg::~CBankDlg(void)
{
	SAFE_DELETE( m_pDragItem );
}
bool CBankDlg::Create( const char* szIDD )
{
	CTDialog::Create( szIDD );
	char szTemp[32];
	sprintf(szTemp,"%s%d",STR_BANK,1);
	SetTabButtonText(IID_BTN_TAB1,szTemp);
	sprintf(szTemp,"%s%d",STR_BANK,2);
	SetTabButtonText(IID_BTN_TAB2,szTemp);
	sprintf(szTemp,"%s%d",STR_BANK,3);
	SetTabButtonText(IID_BTN_TAB3,szTemp);

	if( CCountry::GetSingleton().IsJapan() )
	{
		sprintf(szTemp,"%s%d",STR_BANK,4);
		SetTabButtonText(IID_BTN_TAB4,szTemp);
	}
	else
	{
		SetTabButtonText(IID_BTN_TAB4,STR_PLATINUM );
	}

	CTButton* pBtn	= NULL;
	CWinCtrl* pCtrl = Find( IID_BTN_SAVE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON )
	{
		pBtn = (CTButton*)pCtrl;
		pBtn->SetText( STR_SAVE );
	}

	pCtrl = Find( IID_BTN_WITHDRAW );
	if( pCtrl && pCtrl->GetControlType() == CTRL_BUTTON )
	{
		pBtn = (CTButton*)pCtrl;
		pBtn->SetText( STR_WITHDRAW );
	}

	return true;
}
void CBankDlg::Show()
{
	CTDialog::Show();



	m_iCurrBankPage = 0;

	CWinCtrl* pCtrl = Find( IID_RADIOBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		pRadioBox->SetPressedButton( IID_BTN_TAB1 );
	}

/*	
	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_EXTRA_STOCK) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_BANK_EXTRASPACE );

			CWinCtrl* pCtrl = Find( IID_BTN_TAB4 );
			if( pCtrl )
			{
				pCtrl->SetEnable(true);
			}
			return;
		}        
	}
*/

}
void CBankDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	for( int iPage = 0; iPage < g_iPageCount; ++iPage )
		for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
			m_Slots[iPage][iSlot].MoveWindow( m_sPosition );

}
void CBankDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
		m_Slots[m_iCurrBankPage][iSlot].Draw();


	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 35, 5, g_dwBLACK, F_STR_BANK_TITLE,g_GameDATA.m_Account.Get() );


	const int money_buffer_size = 64;
	char money_buffer[ money_buffer_size ];
	CGameUtil::ConvertMoney2String( CBank::GetInstance().GetMoney(), money_buffer, money_buffer_size );

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 40, 270, g_dwWHITE, money_buffer );
}

/**
* 은행원( 은행을 열수 있는 NPC )와 일정거리 이상이 되엇을경우 창을 닫는다.
*/
void CBankDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
		m_Slots[m_iCurrBankPage][iSlot].Update( ptMouse );

	

	CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( CBank::GetInstance().GetNpcClientIndex(), false );
	///Object가 없거나 거리가 일정이상이면 닫아버려라.
	if( pObjChar == NULL )
		Hide();
	else if( g_pAVATAR->Get_DISTANCE( pObjChar ) >= 1000 )
		Hide();

}

unsigned int CBankDlg::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	if( !IsVision() ) return 0;
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		for( int iSlot = 0 ; iSlot < g_iSlotCountPerPage ; ++iSlot )
		{
			if( m_Slots[m_iCurrBankPage][iSlot].Process( uiMsg,wParam,lParam) )
				return uiMsg;
		}
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( iProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown( iProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CBankDlg::OnLButtonDown( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case IID_BTN_TAB1:
		m_iCurrBankPage = 0;
		break;
	case IID_BTN_TAB2:
		m_iCurrBankPage = 1;
		break;
	case IID_BTN_TAB3:
		m_iCurrBankPage = 2;
		break;
	case IID_BTN_TAB4:
		m_iCurrBankPage = 3;
		break;
	default:
		break;
	}
}

void CBankDlg::OnLButtonUp( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case IID_BTN_SAVE:
		g_itMGR.PostMsg2Dlg( DLG_TYPE_BANKWINDOW, CBankWindowDlg::MSG_SETYPE, CBankWindowDlg::TYPE_SAVE , 0);
		g_itMGR.OpenDialog( DLG_TYPE_BANKWINDOW );
		break;
	case IID_BTN_WITHDRAW:
		g_itMGR.PostMsg2Dlg( DLG_TYPE_BANKWINDOW, CBankWindowDlg::MSG_SETYPE, CBankWindowDlg::TYPE_WITHDRAW, 0 );
		g_itMGR.OpenDialog( DLG_TYPE_BANKWINDOW );
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	default:
		break;
	}
}

void CBankDlg::SetTabButtonText( int iID, char* szText )
{
	CWinCtrl* pCtrl = Find( iID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBUTTON )
	{
		CTRadioButton* pBtn = (CTRadioButton*)pCtrl;
		pBtn->SetText( szText );
	}
}

void CBankDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CBankDlg::update" );
		return;
	}
	
	CItem* pItem = (CItem*)pObj;
	tagITEM& Item = pItem->GetItem();
	CIcon* pIcon = NULL;

	int iPage = pItem->GetIndex() / g_iSlotCountPerPage;
	int iSlot = pItem->GetIndex() % g_iSlotCountPerPage;

	if( Item.IsEmpty() )///삭제
	{
		if( m_Slots[iPage][iSlot].GetIcon() == NULL )
		{
			assert( 0 && "빈 슬롯을 다시 비우려고 합니다 @CBankDlg::Update" );
		}
		else
		{
			m_Slots[iPage][iSlot].DetachIcon();
		}
	}
	else///추가
	{
		if( m_Slots[iPage][iSlot].GetIcon() )
		{
			assert( 0 && "비워있지 않은 슬롯에 Item을 넣을려고 합니다 @CBankDlg::Update" );
		}
		else
		{
			pIcon = pItem->CreateItemIcon();
			m_Slots[iPage][iSlot].AttachIcon( pIcon );
		}
	}
}
int CBankDlg::GetCurrentTab()
{
	return m_iCurrBankPage;/// 0 ~ 3
}


bool CBankDlg::IsFullFirstTab()
{
	for( int i = 0 ; i < g_iSlotCountPerPage; ++i )
	{
		if( NULL == m_Slots[0][i].GetIcon() )
			return false;
	}
	return true;
}