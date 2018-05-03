#include "stdafx.h"
#include ".\deliverystoredlg.h"
#include "../CDragItem.h"
#include "../common/citem.h"
#include "../icon/ciconitemdelivery.h"
#include "../GameCommon/Item.h"
#include "../it_mgr.h"
#include "../command/CTCmdNumberInput.h"
#include "../command/uicommand.h"
#include "../../Network/CNetwork.h"
#include "CDialogDlg.h"


#include "tgamectrl/tbutton.h"

CDeliveryStoreDlg::CDeliveryStoreDlg(void)
{
	m_pDragItem = NULL;
	m_emptyslot = 0;
}

CDeliveryStoreDlg::~CDeliveryStoreDlg(void)
{
	SAFE_DELETE( m_pDragItem );
	ClearItem();
}
bool CDeliveryStoreDlg::Create( const char* IDD )
{
	if( !CTDialog::Create( IDD ) )
		return false;

	
	
	m_pDragItem = new CDragItem;

	CTCmdBringItemFromMall* pCmd = new CTCmdBringItemFromMall;
	CTCmdOpenNumberInputDlg* pOpenCmd = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pCmd );
	
	m_pDragItem->AddTarget( DLG_TYPE_ITEM, pOpenCmd );


	POINT ptOffset;

	for( int slot = 0 ; slot < c_deliverystore_slot_count ; ++slot )
	{
		ptOffset.x = 10 + ( slot % 8 ) * ( m_Slots[slot].GetWidth()  + 1);
		ptOffset.y = 50 + ( slot / 8 ) * ( m_Slots[slot].GetHeight() + 1);

		m_Slots[slot].SetOffset( ptOffset );
		m_Slots[slot].SetDragAvailable();
		m_Slots[slot].SetParent( DLG_TYPE_DELIVERYSTORE );
		m_Slots[slot].SetDragItem( m_pDragItem );
	}

	if( CWinCtrl* pCtrl = Find( IID_BTN_GIFT ) )
	{
		if( pCtrl->GetControlType() == CTRL_BUTTON )
		{
			CTButton* pBtn = (CTButton*)pCtrl;
			pBtn->SetText( STR_GIFT_ITEM );
			pBtn->SetTextColor( g_dwWHITE );
		}
	}
	return true;
}

void CDeliveryStoreDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();

	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y, 0.0f);
	::setTransformSprite( mat );

	drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], true, 35,  7, g_dwBLACK, STR_DELIVERY_STORE );

	for( int slot = 0 ; slot < c_deliverystore_slot_count ; ++slot )
		m_Slots[slot].Draw();
}

void CDeliveryStoreDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	for( int slot = 0 ; slot < c_deliverystore_slot_count ; ++slot )
		m_Slots[slot].Update( ptMouse );

	CGameOBJ* pObj = g_pObjMGR->Get_OBJECT( m_npc_client_object_index );

	///NPC Object가 없거나 있는데 거리가 일정 거리 이상이라면 
	if( pObj == NULL )
		Hide();
	else if( g_pAVATAR->Get_DISTANCE( pObj->Get_CurPOS() ) >= 1000 )
		Hide();
}

void CDeliveryStoreDlg::Show()
{
	CTDialog::Show();
	g_pNet->Send_cli_MALL_ITEM_LIST_REQ	();
	
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DIALOG ) )
	{
		CDialogDlg* p = (CDialogDlg*)pDlg;
		m_npc_client_object_index = p->GetTargetNpcClientObjectIndex();
	}
	else
	{
		m_npc_client_object_index = 0;
	}

}

void CDeliveryStoreDlg::Hide()
{
	CTDialog::Hide();
	ClearItem();
}

void CDeliveryStoreDlg::AddItem( tagBaseITEM* pItem, const char* pszFrom, const char* pszDesc, const char* pszTo )
{
	///해당 슬롯의 아이템및 아이콘 생성
	assert( pItem );
	assert( m_emptyslot < c_deliverystore_slot_count );
	if( pItem && m_emptyslot < c_deliverystore_slot_count )
	{
		CIconItemDelivery* pIcon = new CIconItemDelivery( pItem , pszFrom );
		m_Slots[ m_emptyslot++ ].AttachIcon( pIcon );
		pIcon->GetCItem()->SetIndex( m_emptyslot );
	}
}

void CDeliveryStoreDlg::SetItem( int slotindex, tagBaseITEM& Item )
{
	///해당 슬롯의 아이템아이콘및 아이템 삭제
	assert( slotindex >= 0 && slotindex < c_deliverystore_slot_count  );
	if( slotindex >= 0 && slotindex < c_deliverystore_slot_count  )
	{
		if( CIcon* pIcon = m_Slots[ slotindex ].GetIcon() )
		{
			if( Item.IsEmpty() )
			{
				m_Slots[ slotindex ].DetachIcon();
			}
			else if( Item.IsEnableDupCNT() )
			{
				CIconItem* pItemIcon = (CIconItem*)pIcon;
				tagBaseITEM& item = pItemIcon->GetItem();
				item = Item;
			}
			else
			{
				assert( 0 && "CDeliveryStoreDlg::DeleteItem" );
			}
		}
	}
}

void CDeliveryStoreDlg::ClearItem()
{
	for( int slot = 0 ; slot < c_deliverystore_slot_count; ++slot )
		m_Slots[slot].DetachIcon();

	m_emptyslot = 0;
}
void CDeliveryStoreDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	for( int slot = 0 ; slot < c_deliverystore_slot_count; ++slot )
		m_Slots[slot].MoveWindow( m_sPosition );
}
unsigned CDeliveryStoreDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	if( unsigned uiProcID = CTDialog::Process(uiMsg, wParam,lParam ) )
	{
		for( int slot = 0 ; slot < c_deliverystore_slot_count ; ++slot )
		{
			if( m_Slots[slot].Process( uiMsg,wParam,lParam) )
				return uiMsg;
		}
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			switch( uiProcID )
			{
			case IID_BTN_CLOSE:
				Hide();
				break;
			case IID_BTN_GIFT:
				assert( CIconItemDelivery::s_selected_icon >= 0 && CIconItemDelivery::s_selected_icon <= c_deliverystore_slot_count );
				if( CIconItemDelivery::s_selected_icon > 0 && CIconItemDelivery::s_selected_icon <= c_deliverystore_slot_count )
				{
					CTCmdInputNameGiftReceiver* pCmd = new CTCmdInputNameGiftReceiver;
					g_itMGR.OpenInputNameDlg( pCmd );
				}
				break;
			default:
				break;
			}
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

const char* CDeliveryStoreDlg::GetSelectedItemName()
{
	if( CIconItemDelivery::s_selected_icon > 0 && CIconItemDelivery::s_selected_icon <= c_deliverystore_slot_count )
	{
		if( CIcon* pIcon = m_Slots[ CIconItemDelivery::s_selected_icon - 1].GetIcon() )
			return pIcon->GetName();
	}
	return NULL;
}

void CDeliveryStoreDlg::save_receiver_name( const char* pszName )
{
	assert( pszName );
	if( pszName )
		m_receiver_name = pszName;
}

const char* CDeliveryStoreDlg::get_receiver_name()
{
	return m_receiver_name.c_str();
}

int CDeliveryStoreDlg::get_selecteditem_slotindex()
{
	return CIconItemDelivery::s_selected_icon;
}