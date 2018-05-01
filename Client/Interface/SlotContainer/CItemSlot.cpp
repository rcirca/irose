#include "stdafx.h"
#include ".\citemslot.h"
#include "common/CInventory.h"
#include "../../gamecommon/Item.h"
#include "../../gamecommon/itemcommand.h"
#include "../../gamedata/event/cteventitem.h"
#include "../Command/UICommand.h"
#include "../Icon/CIconItem.h"

CItemSlot::CItemSlot(void)
{
	m_listItems.reserve( INVENTORY_TOTAL_SIZE );
	for( int i = 0; i < INVENTORY_TOTAL_SIZE; ++i )
		m_listItems.push_back( NULL );


	m_pCmdItemEquipInInventory	= new CTCmdItemEquipInInventory;
	m_pCmdItemUseInInventory	= new CTCmdItemUseInInventory;
	m_pCmdItemEtcInInventory	= new CTCmdItemEtcInInventory;
	m_pCmdItemPatInInventory	= new CTCmdAssembleRideItem;

	m_pCmdItemEquiped			= new CTCmdItemEquiped;
	m_pCmdItemBullet			= new CTCmdItemBullet;
	m_pCmdItemPatEquiped		= new CTCmdDisAssembleRideItem;

	m_pEvent = new CTEventItem;
}

CItemSlot::~CItemSlot(void)
{

	SAFE_DELETE( m_pCmdItemEquipInInventory );
	SAFE_DELETE( m_pCmdItemUseInInventory );
	SAFE_DELETE( m_pCmdItemEtcInInventory );
	SAFE_DELETE( m_pCmdItemPatInInventory );
	SAFE_DELETE( m_pCmdItemEquiped );
	SAFE_DELETE( m_pCmdItemBullet );
	SAFE_DELETE( m_pCmdItemPatEquiped );

	SAFE_DELETE( m_pEvent );
}

CItem* CItemSlot::CreateItem( int iInvenIdx, tagITEM& Item )
{
	CItem* pItem = new CItem;
	assert( pItem );
	if( iInvenIdx >= 0 && iInvenIdx < MAX_EQUIP_IDX )
	{
		pItem->SetCommand( m_pCmdItemEquiped );
	}
	else if( iInvenIdx >= MAX_EQUIP_IDX && iInvenIdx < INVENTORY_SHOT_ITEM0 )
	{
		switch( Item.GetTYPE() )
		{
		case ITEM_TYPE_FACE_ITEM:
		case ITEM_TYPE_HELMET:
		case ITEM_TYPE_ARMOR:
		case ITEM_TYPE_GAUNTLET:
		case ITEM_TYPE_BOOTS:
		case ITEM_TYPE_KNAPSACK:
		case ITEM_TYPE_JEWEL:
		case ITEM_TYPE_WEAPON:
		case ITEM_TYPE_SUBWPN:
			pItem->SetCommand( m_pCmdItemEquipInInventory );
			break;
		case ITEM_TYPE_USE:
			pItem->SetCommand( m_pCmdItemUseInInventory );
			break;
		case ITEM_TYPE_ETC:
		case ITEM_TYPE_NATURAL:
			pItem->SetCommand( m_pCmdItemEtcInInventory );
			break;
		case ITEM_TYPE_RIDE_PART:
			pItem->SetCommand( m_pCmdItemPatInInventory );
			break;
		default:
			assert( 0 && CStr::Printf("CItemSlot::AddItem Invalid ItemType(%d)",Item.GetTYPE()));
			break;
		}
	}
	else if( iInvenIdx >= INVENTORY_SHOT_ITEM0 && iInvenIdx < INVENTORY_RIDE_ITEM0 )
	{
		pItem->SetCommand( m_pCmdItemBullet );
	}
	else if( iInvenIdx >= INVENTORY_RIDE_ITEM0 && iInvenIdx < INVENTORY_TOTAL_SIZE )
	{
		pItem->SetCommand( m_pCmdItemPatEquiped );
	}
	else
	{
		SAFE_DELETE( pItem );
		assert( 0 && CStr::Printf("Invalid Inventory Index(%d), CItemSlot::AddItem()", iInvenIdx ) );
	}
	
	return pItem;

}

CItem* CItemSlot::AddItem( int iInvenIdx, tagITEM& Item )
{
	CItem* pItem = NULL;

	if( m_listItems[ iInvenIdx ] == NULL )
	{
		CItem* pItem = CreateItem( iInvenIdx, Item );
		assert( pItem );

		CIcon*	pIcon = new CIcon;

		pItem->SetDefaultIcon( pIcon );
		pItem->SetIndex( iInvenIdx );
		pItem->SetIndexType( CItem::IT_INVENTORY );

		m_pEvent->SetID( CTEventItem::EID_ADD_ITEM );
		m_pEvent->SetIndex( iInvenIdx );
		m_pEvent->SetItem( pItem );

		m_listItems[ iInvenIdx ] = pItem;
		
		SetChanged();
		NotifyObservers( m_pEvent );
	}
	else
	{
		ChangeItemStatus( iInvenIdx );
		pItem = m_listItems[ iInvenIdx ];

	}
	return pItem;
}

///NotifyObserver전에 CInventory에있는 tagITEM을 Empty로 만들어야 한다.
bool CItemSlot::DelItem( int iInvenIdx )
{
	assert( iInvenIdx >= 0 && iInvenIdx < INVENTORY_TOTAL_SIZE );
	CItem* pItem = m_listItems[ iInvenIdx ];
	
	assert( pItem );
	if( pItem )
	{
		pItem->Clear();

		m_pEvent->SetID( CTEventItem::EID_DEL_ITEM );
		m_pEvent->SetIndex( iInvenIdx );
		m_pEvent->SetItem( pItem );

		SetChanged();
		NotifyObservers( m_pEvent );

		delete pItem;
		m_listItems[ iInvenIdx ] = NULL;
	}
	return true;
}

CItem*	CItemSlot::AddItem( tagITEM& Item )
{
	t_InvTYPE type = CInventory::m_InvTYPE[ Item.GetTYPE() ];
	
	int iStartIdx,iEndIdx;

	iStartIdx = MAX_EQUIP_IDX + type * INVENTORY_PAGE_SIZE;
	iEndIdx   = iStartIdx + INVENTORY_PAGE_SIZE - 1;
	int iEmptySlotIdx = GetEmptySlot( iStartIdx, iEndIdx );

	assert( iEmptySlotIdx >= 0 );

	/// TODO::리턴값이 없었는데 일단 임시로 넣었음 원래 리턴값을 넣도록..

	return  AddItem( iEmptySlotIdx, Item );	
}


int	CItemSlot::SetItem( int iInvenIdx, tagITEM& Item )
{
	assert( iInvenIdx >= 0 && iInvenIdx < INVENTORY_TOTAL_SIZE );
	CItem* pItem = GetItem( iInvenIdx );

	if( pItem )
		DelItem( iInvenIdx );

	if( !Item.IsEmpty() )
		AddItem( iInvenIdx, Item );
	return 0;
}

CItem*	CItemSlot::GetItem( int iInvenIdx )
{
	assert( iInvenIdx >= 0 && iInvenIdx < INVENTORY_TOTAL_SIZE );
	return m_listItems[ iInvenIdx ];
}

/// 개수만 감소하는 경우에는 별도로 처리할필요가 없다.
void	CItemSlot::SubItem( int iInvenIdx, tagITEM& Item )
{
	assert( iInvenIdx >= 0 && iInvenIdx < INVENTORY_TOTAL_SIZE );
	CItem* pItem = GetItem( iInvenIdx );
	assert( pItem );
	if( pItem )
	{
		if( !Item.IsEnableDupCNT() )///개수 없는 아이템일경우 삭제한다.
		{
			DelItem( iInvenIdx );
			assert( 0 && "개수 없는 아이템을 빼려고 한다@ CItemSlot::SubItem" );
		}

		if( pItem->IsEmpty() )
		{
			DelItem( iInvenIdx );
			return;
		}
	}
	ChangeItemStatus( iInvenIdx );
}

void	CItemSlot::ChangeItemStatus( int iInvenIdx )
{
	m_pEvent->SetID( CTEventItem::EID_CHANGE_ITEM );
	m_pEvent->SetIndex( iInvenIdx );
	m_pEvent->SetItem( GetItem( iInvenIdx ) );
	SetChanged();
	NotifyObservers( m_pEvent );
}

int		CItemSlot::GetEmptySlot( int iStartIdx, int iEndIdx )
{
	assert( iStartIdx >= 0 && iStartIdx < INVENTORY_TOTAL_SIZE );
	assert( iEndIdx >= 0 && iEndIdx < INVENTORY_TOTAL_SIZE );
	assert( iEndIdx >= iStartIdx );

	for( int idx = iStartIdx; idx <= iEndIdx ; ++idx )
	{
		if( m_listItems[ idx ] == NULL )
			return idx;
	}
	return -1;
}

void	CItemSlot::UpdateItem( int iInvenIdx, tagITEM& Item )
{
	assert( iInvenIdx >= 0 && iInvenIdx < INVENTORY_TOTAL_SIZE );
	m_listItems[ iInvenIdx ]->SetItem( Item );
}

void CItemSlot::Clear()
{
	std::vector< CItem*	>::iterator iter;
	CItem* pItem = NULL;
	for( iter =	m_listItems.begin(); iter != m_listItems.end(); ++iter )
	{
		pItem = *iter;
		if( pItem )
		{
			pItem->Clear();

			m_pEvent->SetID( CTEventItem::EID_DEL_ITEM );
			m_pEvent->SetIndex( pItem->GetIndex() );
			m_pEvent->SetItem( pItem );

			SetChanged();
			NotifyObservers( m_pEvent );

			delete pItem;
			*iter = NULL;
		}
	}
}