#include "stdafx.h"
#include ".\cprivatestoredlg.h"
#include "tgamectrl/tradiobox.h"
#include "tgamectrl/teditbox.h"
#include "CGoodsDlg.h"
#include "../it_mgr.h"
#include "../../Object.h"
#include "../CDragItem.h"
#include "../Icon/CIconItem.h"
#include "../Command/DragCommand.h"
#include "../../GameCommon/item.h"
#include "../../GameData/Event/CTEventPrivateStore.h"
CPrivateStoreDlg::CPrivateStoreDlg( int iDlgType )
{
	SetDialogType( iDlgType );
	POINT ptOffset;
	CTCommand* pCmd;
	m_pSellDragItem = new CDragItem;
	m_pSellDragItem->AddTarget( iDlgType, NULL );
	pCmd = new CTCmdDragSellItemFromPrivateStoreDlg;
	m_pSellDragItem->AddTarget( CDragItem::TARGET_ALL, pCmd );


	m_pBuyDragItem	= new CDragItem;
	m_pBuyDragItem->AddTarget( iDlgType, NULL );
	pCmd = new CTCmdDragBuyItemFromPrivateStoreDlg;
	m_pBuyDragItem->AddTarget( CDragItem::TARGET_ALL, pCmd );


	for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
	{
		m_SellSlots[iSlot].SetWidth( 40 );
		m_SellSlots[iSlot].SetHeight( 40 );
		m_BuySlots[iSlot].SetWidth( 40 );
		m_BuySlots[iSlot].SetHeight( 40 );

		ptOffset.x = 10 + ( iSlot % 5 ) * ( m_SellSlots[iSlot].GetWidth() + 1 );
		ptOffset.y = 54 + ( iSlot / 5 ) * ( m_SellSlots[iSlot].GetHeight() + 1 );

		m_SellSlots[iSlot].SetParent( iDlgType );
		m_SellSlots[iSlot].SetOffset( ptOffset );
		m_SellSlots[iSlot].SetDragAvailable();
		m_SellSlots[iSlot].SetDragItem( m_pSellDragItem );

		m_BuySlots[iSlot].SetParent( iDlgType );
		m_BuySlots[iSlot].SetOffset( ptOffset );
		m_BuySlots[iSlot].SetDragAvailable();
		m_BuySlots[iSlot].SetDragItem( m_pBuyDragItem );

	}
}

CPrivateStoreDlg::~CPrivateStoreDlg(void)
{
	SAFE_DELETE( m_pSellDragItem );
	SAFE_DELETE( m_pBuyDragItem );
}

void CPrivateStoreDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
	if( m_iTab == 0 )
	{
		for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
			m_SellSlots[iSlot].Draw();
	}
	else
	{
		for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
			m_BuySlots[iSlot].Draw();
	}
}
void CPrivateStoreDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
		m_SellSlots[iSlot].MoveWindow( m_sPosition );

	for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
		m_BuySlots[iSlot].MoveWindow( m_sPosition );

}
void CPrivateStoreDlg::Show()
{
	CTDialog::Show();
	HideChild( IID_TEXT_OPENED );
	HideChild( IID_BTN_STOP );
	HideChild( IID_BTN_ADD_BUYLIST);
	SetEnableChild( IID_EDITBOX, true );
	
	CPrivateStore& refPrivateStore = CPrivateStore::GetInstance();
	m_iTab = 0;
	
	CWinCtrl* pCtrl = Find( IID_RADIOBOX );
	if( pCtrl )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		pRadioBox->SetPressedButton( IID_BTN_SELL );
	}

	pCtrl = Find( IID_EDITBOX );
	if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
	{
		CTEditBox* pEdit = (CTEditBox*)pCtrl;
		pEdit->clear_text();
		pEdit->AppendText( CStr::Printf( STR_DEFAULT_PRIVATESTORE_TITLE , g_pAVATAR->Get_NAME()) ) ;
	}

	for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
		m_SellSlots[iSlot].DetachIcon();

	for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
		m_BuySlots[iSlot].DetachIcon();


	refPrivateStore.ClearBuyList();
	refPrivateStore.ClearSellList();
	refPrivateStore.AddObserver( this );

	CItem* pItem;
	for( int iIndex = 0 ; iIndex < MAX_WISH_ITEMS; ++iIndex )
	{
		pItem = refPrivateStore.GetWishItem( iIndex );
		if( pItem )
			m_BuySlots[iIndex].AttachIcon( pItem->CreateItemIcon() );
	}
}

void CPrivateStoreDlg::Hide()
{

	CTDialog::Hide();
	CPrivateStore& refPrivateStore = CPrivateStore::GetInstance();

	refPrivateStore.DeleteObserver( this );
	refPrivateStore.Close();

	for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
		m_SellSlots[iSlot].DetachIcon();

	for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
		m_BuySlots[iSlot].DetachIcon();


	refPrivateStore.ClearBuyList();
	refPrivateStore.ClearSellList();

}

void CPrivateStoreDlg::OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_SELL:
		m_iTab = 0;
		HideChild( IID_BTN_ADD_BUYLIST);
		break;
	case IID_BTN_BUY:
		m_iTab = 1;
		ShowChild( IID_BTN_ADD_BUYLIST);
		break;
	default:
		break;
	}
}

void CPrivateStoreDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_ADD_BUYLIST:
		{
			for( int i = 0 ; i < c_iMaxSlotCount; ++i )
			{
				CIcon* pIcon = m_BuySlots[i].GetIcon();
				if( pIcon && m_BuySlots[i].IsSelected() )
				{
					CIconItem* pItemIcon = (CIconItem*)pIcon;
					if( m_BuySlots[i].IsExhibition() )
					{
						CPrivateStore::GetInstance().RemoveItemBuyList( pItemIcon->GetIndex() );
					}
					else
					{
						CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_GOODS );
						if( pDlg )
						{
							CGoodsDlg* pGoodsDlg = (CGoodsDlg*)pDlg;
							pGoodsDlg->SetIcon( pItemIcon );
							pGoodsDlg->SetType( CGoodsDlg::ADD_BUYLIST );
							g_itMGR.OpenDialog( DLG_TYPE_GOODS );
						}
					}
					break;
				}
			}
			break;
		}
	case IID_BTN_START:
		{
			if( CPrivateStore::GetInstance().IsValidOpen() )
			{
				CWinCtrl* pCtrl = Find( IID_EDITBOX );
				if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
				{
					CTEditBox* pEdit = (CTEditBox*)pCtrl;
					const char* pszTitle = pEdit->get_text();
					
					//20050727 홍근 개인 상점의 버그
					if( strlen(pszTitle) == 0 )
					{
						std::string strTitle = CStr::Printf( STR_DEFAULT_PRIVATESTORE_TITLE,g_pAVATAR->Get_NAME() );
						CPrivateStore::GetInstance().SetTitle( strTitle.c_str() );
					}
					else
					{
						CPrivateStore::GetInstance().SetTitle( pszTitle );
					}
				}

				
				CPrivateStore::GetInstance().SortItemSellList();
					

				if( CPrivateStore::GetInstance().Open() )
				{
					ShowChild( IID_BTN_STOP );
					ShowChild( IID_TEXT_OPENED );
					HideChild( IID_BTN_START );
					HideChild( IID_TEXT_CLOSED );
					SetEnableChild( IID_EDITBOX, false );
				}
			}
			break;
		}
	case IID_BTN_STOP:
		{
			HideChild( IID_BTN_STOP );
			HideChild( IID_TEXT_OPENED );
			ShowChild( IID_BTN_START );
			ShowChild( IID_TEXT_CLOSED );
			SetEnableChild( IID_EDITBOX, true );
			CPrivateStore::GetInstance().Close();
			break;
		}
	case IID_BTN_CLOSE:
		{
			Hide();
			break;
		}
	default:
		break;
	}
}

void CPrivateStoreDlg::SortItemSellList()
{	
	CIcon * pIcon[c_iMaxSlotCount];	

	int i = 0;
	int iIndex = 0;
	
	for(i=0; i< c_iMaxSlotCount; i++)
	{	
		if( m_SellSlots[i].GetIcon() )
		{
			pIcon[iIndex] = m_SellSlots[i].MoveIcon();
			iIndex++;
		}
	}

    for(i=0; i< iIndex; i++)
	{	
		if( pIcon[i] )
		{
			m_SellSlots[i].AttachIcon( pIcon[i] );			
		}
	}
}


unsigned CPrivateStoreDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( m_iTab == 0 )
		{
			for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
			{
				if( m_SellSlots[iSlot].Process( uiMsg,wParam,lParam ) )
					return uiMsg;
			}
		}
		else
		{
			for( int iSlot = 0 ; iSlot < c_iMaxSlotCount; ++iSlot )
				if( m_BuySlots[iSlot].Process( uiMsg, wParam,lParam ))
				{
					if( uiMsg == WM_LBUTTONDOWN && m_BuySlots[iSlot].GetIcon() )
						m_BuySlots[iSlot].SetSelected();

					return uiMsg;
				}
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
void CPrivateStoreDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj == NULL || strcmp( pObj->toString(), "PrivateStore" ) )
	{
		assert( 0 && "CTEventPrivateStore is NULL or Invalid Type" );
		return;
	}


	CTEventPrivateStore* pEvent = ( CTEventPrivateStore*)pObj;
	int iIndex	= pEvent->GetIndex();

	switch( pEvent->GetID() )
	{
	case CTEventPrivateStore::EID_SORT_SELLLIST:
		{
            SortItemSellList();
		}
		break;

	case CTEventPrivateStore::EID_ADD_SELLLIST:
		{
			CItem* pItem = pEvent->GetItem();
			assert( pItem );
			if( pItem )
				m_SellSlots[iIndex].AttachIcon( pItem->CreateItemIcon() );
			break;
		}
	case CTEventPrivateStore::EID_REMOVE_SELLLIST:
		{
			assert( m_SellSlots[iIndex].GetIcon() );
			m_SellSlots[iIndex].DetachIcon();
			break;
		}
	case CTEventPrivateStore::EID_ADD_WISHLIST :
		{
			CItem* pItem = pEvent->GetItem();
			assert( pItem );
			if( pItem )
				m_BuySlots[iIndex].AttachIcon( pItem->CreateItemIcon() );
			break;
		}
	case CTEventPrivateStore::EID_REMOVE_WISHLIST :
		{
			assert( m_BuySlots[iIndex].GetIcon() );
			m_BuySlots[iIndex].DetachIcon();
			break;
		}
	case CTEventPrivateStore::EID_ADD_BUYLIST:
		{
			CIcon* pIcon = NULL;
			CIconItem* pItemIcon = NULL;
			for( int i = 0 ; i < c_iMaxSlotCount; ++i )
			{
				pIcon = m_BuySlots[i].GetIcon();
				if( pIcon )
				{
					pItemIcon = ( CIconItem* )pIcon;
					if( pItemIcon->GetIndex() == iIndex )
					{
						m_BuySlots[i].DetachIcon();
						CItem* pItem = pEvent->GetItem();
						assert( pItem );
						if( pItem )
						{
							m_BuySlots[i].AttachIcon( pItem->CreateItemIcon() );
							m_BuySlots[i].SetExhibition( true );
						}
						break;
					}
				}
			}
			break;
		}
	case CTEventPrivateStore::EID_REMOVE_BUYLIST:
		{
			CIcon* pIcon = NULL;
			CIconItem* pItemIcon = NULL;
			for( int i = 0 ; i < c_iMaxSlotCount; ++i )
			{
				pIcon = m_BuySlots[i].GetIcon();
				if( pIcon )
				{
					pItemIcon = ( CIconItem* )pIcon;
					if( pItemIcon->GetIndex() == iIndex )
					{
						m_BuySlots[i].DetachIcon();
						CItem* pItem = pEvent->GetItem();
						assert( pItem );
						if( pItem )
						{
							m_BuySlots[i].AttachIcon( pItem->CreateItemIcon() );
							m_BuySlots[i].SetExhibition( false );
						}
						break;
					}
				}
			}
			break;
		}
	default:
		break;
	}
}

void CPrivateStoreDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );
	if( m_iTab == 0 )
	{
		for( int iSlot = 0 ; iSlot < 30; ++iSlot )
			m_SellSlots[iSlot].Update( ptMouse );
	}
	else
	{
		for( int iSlot = 0 ; iSlot < 30; ++iSlot )
			m_BuySlots[iSlot].Update( ptMouse );
	}

}

//*------------------------------------------------------------------
/// @brief 현재 선택된 탭의 종류 
//*------------------------------------------------------------------
int CPrivateStoreDlg::GetTabType()
{
	return m_iTab;
}