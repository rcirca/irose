#include "stdafx.h"
#include ".\cavatarstoredlg.h"
#include "../CDragItem.h"
#include "../../GameCommon/Item.h"
#include "../Command/UICommand.h"
#include "../Command/DragCommand.h"
#include "../Command/CTCmdNumberInput.h"
#include "../CDragNDropMgr.h"
#include "../../Object.h"
#include "tgamectrl/tradiobox.h"
#include "../it_mgr.h"
#include "../CTooltipMgr.h"

CAvatarStoreDlg::CAvatarStoreDlg( int iDlgType )
{
	SetDialogType( iDlgType );
	m_pBuyItemAtAvatarStore = new CTCmdBuyItemAtAvatarStore;
	
	CTCmdOpenNumberInputDlg* pNumberInputCmd = new CTCmdOpenNumberInputDlg;
	pNumberInputCmd->SetCommand( m_pBuyItemAtAvatarStore );
	m_pSellDragItem = new CDragItem;
	m_pSellDragItem->AddTarget( DLG_TYPE_ITEM, pNumberInputCmd );

	m_iTab = 0;


	POINT ptOffset;
	for( int iSlot = 0; iSlot < c_iAvatarStoreMaxSlotCount; ++iSlot )
	{
		ptOffset.x = 10 + ( iSlot % 5 ) * ( m_SellSlots[iSlot].GetWidth() + 1 );
		ptOffset.y = 54 + ( iSlot / 5 ) * ( m_SellSlots[iSlot].GetHeight() + 1 );

		m_SellSlots[iSlot].SetParent( iDlgType );
		m_SellSlots[iSlot].SetOffset( ptOffset );
		m_SellSlots[iSlot].SetDragAvailable();
		m_SellSlots[iSlot].SetDragItem( m_pSellDragItem );
		m_SellSlots[iSlot].SetTooltipType( INFO_ADD_PRICE_AVATARSTORE_SELLTAB );

		m_BuySlots[iSlot].SetParent( iDlgType );
		m_BuySlots[iSlot].SetOffset( ptOffset );
		m_BuySlots[iSlot].SetTooltipType( INFO_ADD_PRICE_AVATARSTORE_BUYTAB );
	}


	for( int i = 0 ; i < c_iAvatarStoreMaxSlotCount; ++i )
	{
		m_SellItems[i] = NULL;
		m_BuyItems[i] = NULL;
	}

}

CAvatarStoreDlg::~CAvatarStoreDlg(void)
{
	SAFE_DELETE( m_pBuyItemAtAvatarStore );
	SAFE_DELETE( m_pSellDragItem );
	for( int i = 0 ; i < c_iAvatarStoreMaxSlotCount; ++i )
	{
		SAFE_DELETE( m_SellItems[i] );
		SAFE_DELETE( m_BuyItems[i]);
	}
}

void CAvatarStoreDlg::Show()
{
	AddObserver( &CDragNDropMgr::GetInstance() );

	CTDialog::Show();
	CWinCtrl* pCtrl = Find( IID_RADIOBOX );

	if( pCtrl )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		pRadioBox->SetPressedButton( IID_BTN_SELL );
	}
	m_iTab = 0;

}

void CAvatarStoreDlg::Hide()
{
	CTDialog::Hide();

	if( g_itMGR.IsDlgOpened( DLG_TYPE_N_INPUT ) )
		g_itMGR.CloseDialog( DLG_TYPE_N_INPUT );


	///초기화
	for( int i = 0 ; i < c_iAvatarStoreMaxSlotCount; ++i )
	{
		m_SellSlots[i].DetachIcon();
		m_BuySlots[i].DetachIcon();

		if( m_SellItems[i] )
		{
			SetChanged();
			m_Event.SetIndex( m_SellItems[i]->GetIndex() );
			m_Event.SetID( CTEventAvatarStore::EID_DELETE_ITEM_SELLLIST );
			NotifyObservers( &m_Event );
		}

		SAFE_DELETE( m_SellItems[i] );
		SAFE_DELETE( m_BuyItems[i]);
	}
	DeleteObserver( &CDragNDropMgr::GetInstance() );
}

void CAvatarStoreDlg::SetMasterSvrObjIdx( WORD wSvrObjIdx )
{
	m_wMasterSvrObjIdx = wSvrObjIdx;
}

WORD CAvatarStoreDlg::GetMasterSvrObjIdx()
{
	return m_wMasterSvrObjIdx;
}
void CAvatarStoreDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	if( m_iTab == 0 )
	{
		for( int i = 0 ;i < c_iAvatarStoreMaxSlotCount; ++i )
			m_SellSlots[i].Draw();
	}
	else if( m_iTab == 1 )
	{
		for( int i = 0 ;i < c_iAvatarStoreMaxSlotCount; ++i )
			m_BuySlots[i].Draw();
	}

	if( !m_title.empty() )
	{
		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
		::setTransformSprite( mat );

		m_title.draw();
	}
}

unsigned CAvatarStoreDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( m_iTab == 0 )
		{
			for( int i = 0 ;i < c_iAvatarStoreMaxSlotCount; ++i )
				if( m_SellSlots[i].Process( uiMsg, wParam, lParam ) )
					return uiMsg;
		}
		else if( m_iTab == 1 )
		{
			for( int i = 0 ;i < c_iAvatarStoreMaxSlotCount; ++i )
				if( m_BuySlots[i].Process( uiMsg, wParam, lParam ) )
					return uiMsg;
		}
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( uiProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown( uiProcID, wParam, lParam );
			break;
		default:
			break;
		}

		return uiMsg;
	}
	return 0;

}

void CAvatarStoreDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_CLOSE:
		Hide();
		break;
	default:
		break;
	}
}

void CAvatarStoreDlg::OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_SELL:
		m_iTab = 0;
		break;
	case IID_BTN_BUY:
		m_iTab = 1;
		break;
	default:
		break;
	}
}

void CAvatarStoreDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );

	for( int i = 0 ;i < c_iAvatarStoreMaxSlotCount; ++i )
		m_SellSlots[i].MoveWindow( m_sPosition );

	for( int i = 0 ;i < c_iAvatarStoreMaxSlotCount; ++i )
		m_BuySlots[i].MoveWindow( m_sPosition );

}

void CAvatarStoreDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	CTDialog::Update( ptMouse );


	if( CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog() )
	{
		if( pDlg == (CTDialog*)this )
		{
			POINT pt = ptMouse;
			pt.x = m_sPosition.x;
			pt.y = m_sPosition.y;
			if( m_title.is_ellipsis() && PtInRect( &m_title.get_rect(), pt ) )
				CToolTipMgr::GetInstance().RegToolTip( m_sPosition.x + m_title.get_rect().left - 10, m_sPosition.y - 18, m_title.get_string() );
		}
	}



	if( m_iTab == 0 )
	{
		for( int i = 0 ;i < c_iAvatarStoreMaxSlotCount; ++i )
			m_SellSlots[i].Update( ptMouse );
	}
	else if( m_iTab == 1 )
	{
		for( int i = 0 ;i < c_iAvatarStoreMaxSlotCount; ++i )
			m_BuySlots[i].Update( ptMouse );
	}


	CObjAVT* pAvt = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( m_wMasterSvrObjIdx ) , false );

	if( pAvt == NULL )
		Hide();
	else if( g_pAVATAR && !g_pAVATAR->IsInRANGE( pAvt, AVT_CLICK_EVENT_RANGE ) )
		Hide();

}

void CAvatarStoreDlg::AddItem2SellList( tagPS_SLOT_PRICE& SlotITEM )
{
	assert( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT < c_iAvatarStoreMaxSlotCount );
	if( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT < c_iAvatarStoreMaxSlotCount )
	{
		CItem* pItem = new CItem;
		pItem->SetItem( SlotITEM.m_SlotITEM );
		pItem->SetIndex( SlotITEM.m_btSLOT );
		pItem->SetUnitPrice( SlotITEM.m_dwPRICE );
		m_SellItems[SlotITEM.m_btSLOT] = pItem;
		m_SellSlots[SlotITEM.m_btSLOT].AttachIcon( pItem->CreateItemIcon() );
	}
}

void CAvatarStoreDlg::AddItem2BuyList( tagPS_SLOT_PRICE& SlotITEM )
{
	assert( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT < c_iAvatarStoreMaxSlotCount );
	if( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT < c_iAvatarStoreMaxSlotCount )
	{
		CItem* pItem = new CItem;

		pItem->SetItem( SlotITEM.m_SlotITEM );
		pItem->SetIndex( SlotITEM.m_btSLOT );
		pItem->SetUnitPrice( SlotITEM.m_dwPRICE);
		
		m_BuyItems[SlotITEM.m_btSLOT] = pItem;
		m_BuySlots[SlotITEM.m_btSLOT].AttachIcon( pItem->CreateItemIcon() );
	}
}

/*
* SlotITEM.m_btSLOT == WishList Index ??
*
* @Warning	아닐경우에는 수정필요하다.
*/
void CAvatarStoreDlg::UpdateBuyList( tagPS_SLOT_ITEM& SlotITEM )
{
	assert( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT < c_iAvatarStoreMaxSlotCount );
	if( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT < c_iAvatarStoreMaxSlotCount )
	{
		CItem* pItem;
		for( int iSlot = 0 ; iSlot < c_iAvatarStoreMaxSlotCount; ++iSlot )
		{
			pItem = m_BuyItems[iSlot];
			if( pItem && pItem->GetIndex() == SlotITEM.m_btSLOT )
			{
				if( SlotITEM.m_SlotITEM.IsEmpty() )
				{
					m_BuySlots[iSlot].DetachIcon();
					SAFE_DELETE( m_BuyItems[iSlot] );
				}
				else
				{
					pItem->SetItem( SlotITEM.m_SlotITEM );
				}
				break;
			}
		}
	}
}

void CAvatarStoreDlg::UpdateSellList( tagPS_SLOT_ITEM& SlotITEM )
{
	assert( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT <= c_iAvatarStoreMaxSlotCount );
	if( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT <= c_iAvatarStoreMaxSlotCount )
	{
		CItem* pItem =  m_SellItems[SlotITEM.m_btSLOT];
		assert( pItem );
		if( pItem )
		{
			if( SlotITEM.m_SlotITEM.IsEmpty() )
			{
				m_SellSlots[SlotITEM.m_btSLOT].DetachIcon();

				SetChanged();
				m_Event.SetIndex( m_SellItems[SlotITEM.m_btSLOT]->GetIndex() );
				m_Event.SetID( CTEventAvatarStore::EID_DELETE_ITEM_SELLLIST );
				NotifyObservers( &m_Event );

				SAFE_DELETE( m_SellItems[SlotITEM.m_btSLOT] );
			}
			else
			{
				pItem->SetItem( SlotITEM.m_SlotITEM );
			}
		}
	}
}


/**
* 상점주인이 사기를 원하는 아이템인가?
* @return 사기을 원하는 아이템일경우 개수, 아니라면 0
*/
int CAvatarStoreDlg::IsBuyItem( tagITEM& Item )
{
	enum{
		ERR_NONE,
		ERR_OPTION,
		ERR_UPGRADE,
		ERR_DURABILITY,
		ERR_SOCKET,
		ERR_LIFE,
	};


	bool bFound = false;
	bool bCheckError = false;
	int  iErrCode = ERR_NONE;
	if( Item.IsEquipITEM() )
		bCheckError = true;


	if( bCheckError && Item.GetLife() <= 0 )
	{
		g_itMGR.OpenMsgBox(  STR_AVATARSTORE_CANT_SELL_LIFE );
		return 0;
	}


	CItem*		pItem; m_BuyItems[c_iAvatarStoreMaxSlotCount];	
	for( int iSlot = 0 ; iSlot < c_iAvatarStoreMaxSlotCount; ++iSlot )
	{
		pItem = m_BuyItems[iSlot];
		if( pItem && pItem->GetItem().IsEqual( Item.GetTYPE(), Item.GetItemNO() ) )
		{
			if( bCheckError )
			{
				bFound = true;
				if( pItem->GetItem().GetOption() && pItem->GetItem().GetOption() != Item.GetOption() )
				{
					iErrCode = ERR_OPTION;
					continue;
				}
				if( pItem->GetItem().GetGrade() > Item.GetGrade() )
				{
					iErrCode = ERR_UPGRADE;
					continue;
				}
				if( pItem->GetItem().GetDurability() > Item.GetDurability() )
				{
					iErrCode = ERR_DURABILITY;
					continue;
				}
				if( pItem->GetItem().HasSocket() && !Item.HasSocket() )
				{
					iErrCode = ERR_SOCKET;
					continue;
				}

			}
			return pItem->GetQuantity();
		}
	}
	if( bFound && bCheckError )						/// 상점에서 구입을 원하는 아이템의 옵션( 옵션, 제련등급, 내구도, 소켓유무) 비교
	{
		switch( iErrCode )
		{
		case ERR_OPTION:
			g_itMGR.OpenMsgBox(  STR_AVATARSTORE_CANT_SELL_OPTION );
			break;
		case ERR_UPGRADE:
			g_itMGR.OpenMsgBox( STR_AVATARSTORE_CANT_SELL_UPGRADE );
			break;
		case ERR_DURABILITY:
			g_itMGR.OpenMsgBox( STR_AVATARSTORE_CANT_SELL_DURABILITY );
			break;
		case ERR_SOCKET:
			g_itMGR.OpenMsgBox( STR_AVATARSTORE_CANT_SELL_SOCKET );
			break;
		default:
			break;
		}
	}
	return 0;
}

/**
* 해당 물품을 사고 싶다고 올린 아이템의 WishList Index를 구한다., 없으면 -1
*			중복된 아이템이 있을경우 제일 비싼 가격으로 올린 아이템부터 
* @return WishListIndex or -1
*/
int CAvatarStoreDlg::GetWishIndex( tagITEM& Item , int& iUnitPrice )
{
	int iWishIndex = -1;
	CItem*		pItem; m_BuyItems[c_iAvatarStoreMaxSlotCount];	
	for( int iSlot = 0 ; iSlot < c_iAvatarStoreMaxSlotCount; ++iSlot )
	{
		pItem = m_BuyItems[iSlot];
		if( pItem && pItem->GetItem().IsEqual( Item.GetTYPE(), Item.GetItemNO() ) && iUnitPrice < pItem->GetUnitPrice() )
		{
			iWishIndex = pItem->GetIndex();
			iUnitPrice = pItem->GetUnitPrice();
		}
	}
	return iWishIndex;
}

void CAvatarStoreDlg::SetTitle( const char* pszTitle )
{
	assert( pszTitle );
	if( pszTitle )
	{
		RECT rc = { 35, 6, m_iWidth - 25, 24 };
		m_title.set_string( pszTitle,rc, FONT_NORMAL );
		m_title.set_color( g_dwWHITE );
	}
}