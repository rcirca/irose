#include "stdafx.h"
#include ".\cprivatestore.h"
#include "../Object.h"
#include "Net_Prototype.h"
#include "Event/CTEventPrivateStore.h"
#include "Event/CTEventItem.h"
#include "../GameCommon/Item.h"
#include "../Network/CNetwork.h"
#include "../interface/it_mgr.h"
#include "../GameCommon/CFilterWord.h"
#include "../IO_Terrain.h"



CPrivateStore::CPrivateStore(void) 
{
	m_bOpened = false;

	for( int i = 0 ; i < MAX_P_STORE_ITEM_SLOT ; ++i )
	{
		m_SellItems.push_back( NULL );
	}

	for( int iIndex = 0 ; iIndex < MAX_WISH_ITEMS; ++iIndex )
		m_WishItems[iIndex] = NULL;

}

CPrivateStore::~CPrivateStore(void)
{
	std::vector< CItemFragment* >::iterator iterSell;
	CItemFragment* pFragmentItem;
	for( iterSell = m_SellItems.begin(); iterSell != m_SellItems.end(); )
	{
		pFragmentItem = *iterSell;
		if( pFragmentItem )
			delete pFragmentItem;

		iterSell = m_SellItems.erase( iterSell );
	}

	std::list< CItemFragment* >::iterator iterBuy;
	for( iterBuy = m_BuyItems.begin(); iterBuy != m_BuyItems.end(); )
	{
		pFragmentItem = *iterBuy;
		iterBuy = m_BuyItems.erase( iterBuy );
		delete pFragmentItem;
	}

	for( int iIndex = 0 ; iIndex < MAX_WISH_ITEMS; ++iIndex )
		SAFE_DELETE( m_WishItems[iIndex] );

}
CPrivateStore& CPrivateStore::GetInstance()
{
	static CPrivateStore s_Instance;
	return s_Instance;
}
//*---------------------------------------------------------------------------//
/// @brief 판매하고자 하는 물품을 등록한다.인벤토리로부터 
//*---------------------------------------------------------------------------//
void CPrivateStore::AddItemSellList( CItem* pItem, int iQuantity, int iPrice )
{
	if( IsOpened() ) return;

	tagITEM& Item = pItem->GetItem();
	if( !Item.IsEnableExchange() )
	{
		g_itMGR.AppendChatMsg(STR_DONT_EXCHANGE_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );	
		return;
	}

	if( !Item.IsEnableSELL() )
	{
		g_itMGR.AppendChatMsg(STR_DONT_SELL_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );	
		return;
	}

	std::vector< CItemFragment* >::iterator iter;
	int iSlot = 0;
	for( iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter, ++iSlot )
	{
		if( *iter == NULL )
		{
			CItemFragment* pFragmentItem = new CItemFragment( pItem );
			pFragmentItem->SetQuantity( iQuantity );
			pFragmentItem->SetUnitPrice( iPrice );
			m_SellItems[iSlot] = pFragmentItem;
			
			CTEventPrivateStore Event;
			Event.SetID( CTEventPrivateStore::EID_ADD_SELLLIST );
			Event.SetIndex( iSlot );
			Event.SetItem( pFragmentItem );
			SetChanged();
			NotifyObservers( &Event );
			return;
		}
	}
}

template<class T>
class null_less
{ 
public: 
	bool operator() (const T &a, const T &b) const 
	{ 
		if( a && b==NULL )
		{
            return true;            
		}	
		return false;
	}; 
}; 

//20050728 홍근 개인 상점 거래 버그
void CPrivateStore::SortItemSellList()
{	

	sort(m_SellItems.begin(), m_SellItems.end(), null_less<CItemFragment*>() );

	CTEventPrivateStore Event;
	Event.SetID( CTEventPrivateStore::EID_SORT_SELLLIST );			
	
	SetChanged();
	NotifyObservers( &Event );			
	
}

//*---------------------------------------------------------------------------//
/// @param iIndex - Inventory Index
/// @brief 판매리스트에서 특정 물품을 뺀다
//*---------------------------------------------------------------------------//
void CPrivateStore::RemoveItemSellList( int iIndex )
{
	if( IsOpened() ) return;

	std::vector< CItemFragment* >::iterator iter;
	int iSlot = 0;
	CItemFragment* pFragmentItem;
	for( iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter, ++iSlot )
	{
		pFragmentItem = *iter;
		if( pFragmentItem && pFragmentItem->GetIndex() == iIndex )
		{
			CTEventPrivateStore Event;

			Event.SetID( CTEventPrivateStore::EID_REMOVE_SELLLIST );
			Event.SetIndex( iSlot );

			SetChanged();
			NotifyObservers( &Event );

			delete pFragmentItem;
			m_SellItems[iSlot] = NULL;

			return;
		}
	}
}
//*---------------------------------------------------------------------------//
/// @param iIndex - Slot Index
/// @brief 판매리스트에서 특정 물품을 리턴한다.
//*---------------------------------------------------------------------------//
CItem* CPrivateStore::GetItemSellList( int iIndex )
{
	//if( iIndex >= 0 && iIndex < m_SellItems.size() )
	//	return m_SellItems[iIndex];
	return NULL;
}
//*---------------------------------------------------------------------------//
/// @param iIndex    - WishList의 Index
/// @param iUnitCost - 단가
/// @brief 찜목록에서 실제 구입리스트에 추가한다
//*---------------------------------------------------------------------------//
void CPrivateStore::AddItemBuyList( int iIndex, int iUnitCost, int iQuantity )
{
	if( IsOpened() ) return;

	assert( iIndex >= 0 && iIndex < MAX_WISH_ITEMS );
	if( iIndex >= 0 && iIndex < MAX_WISH_ITEMS )
	{
		CItem* pItem = m_WishItems[ iIndex ];
		assert( pItem );
		if( pItem )
		{
			CItemFragment* pFragmentItem = new CItemFragment( pItem );
			pFragmentItem->SetUnitPrice( iUnitCost );
			pFragmentItem->SetQuantity( iQuantity );
			m_BuyItems.push_back( pFragmentItem );			
			
			CTEventPrivateStore Event;

			Event.SetID( CTEventPrivateStore::EID_ADD_BUYLIST );
			Event.SetIndex( iIndex );
			Event.SetItem( pFragmentItem );

			SetChanged();
			NotifyObservers( &Event );
		}
	}
}
//*---------------------------------------------------------------------------//
/// @param iIndex    - WishList의 Index
/// @brief 구입리스트에서 해당 Index의 Item을 뺀다.
//*---------------------------------------------------------------------------//
void CPrivateStore::RemoveItemBuyList( int iIndex )
{
	if( IsOpened() ) return;

	assert( iIndex >= 0 && iIndex < MAX_WISH_ITEMS );
	if( iIndex >= 0 && iIndex < MAX_WISH_ITEMS )
	{
		std::list< CItemFragment* >::iterator iter;
		CItemFragment* pFragmentItem;
		for( iter = m_BuyItems.begin(); iter != m_BuyItems.end(); ++iter )
		{
			pFragmentItem = *iter;
			if( pFragmentItem->GetIndex() == iIndex )
			{
				CTEventPrivateStore Event;

				Event.SetID( CTEventPrivateStore::EID_REMOVE_BUYLIST );
				Event.SetIndex( iIndex );
				Event.SetItem( m_WishItems[iIndex] );
				SetChanged();
				NotifyObservers( &Event );

				m_BuyItems.erase( iter );
				delete pFragmentItem;
				break;
			}
		}
	}
}

////*---------------------------------------------------------------------------//
///// @param iIndex - Slot Index
///// @brief 구입리스트에서 특정 물품을 리턴한다.
////*---------------------------------------------------------------------------//
//CItem*	CPrivateStore::GetItemBuyList( int iIndex )
//{
//	if( iIndex >= 0 && iIndex < m_BuyItems.size() )
//		return m_BuyItems[iIndex];
//
//	return NULL;
//}
//*---------------------------------------------------------------------------//
/// @param bSendPacket    - 서버에 추가사항을 알려주는가?
/// @brief 찜리스트에 특정 item을 추가한다.
//*---------------------------------------------------------------------------//
void CPrivateStore::AddItemWishList( tagITEM& Item , bool bSendPacket ,int iSlotIndex )
{
	if( IsOpened() ) return;

	if( Item.IsEmpty() )
		return;

	if( !Item.IsEnableExchange() )
	{
		g_itMGR.AppendChatMsg(STR_DONT_EXCHANGE_ITEM, IT_MGR::CHAT_TYPE_SYSTEM);	
		return;
	}

	if( !Item.IsEnableSELL() )
	{
		g_itMGR.AppendChatMsg(STR_DONT_SELL_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );	
		return;
	}

	if( iSlotIndex >= 0 )
	{
		assert( iSlotIndex < MAX_WISH_ITEMS );
		if( iSlotIndex < MAX_WISH_ITEMS )
		{
			CItem* pItem = new CItem;
			pItem->SetIndex( iSlotIndex );
			pItem->SetUnitPrice(0);

			pItem->SetItem( Item );
			m_WishItems[iSlotIndex] = pItem;

			CTEventPrivateStore Event;

			Event.SetID( CTEventPrivateStore::EID_ADD_WISHLIST );
			Event.SetIndex( iSlotIndex );
			Event.SetItem( pItem );

			SetChanged();
			NotifyObservers( &Event );
		}
	}
	else
	{
		int iIndex = 0;
		for( iIndex = 0; iIndex < MAX_WISH_ITEMS; ++iIndex )
		{
			if( m_WishItems[iIndex] == NULL )
			{	
				CItem* pItem = new CItem;
				pItem->SetIndex( iIndex );

				tagITEM TempItem = Item;
				if( TempItem.IsEnableDupCNT() )
					TempItem.m_uiQuantity = 0;

				pItem->SetItem( TempItem );
				m_WishItems[iIndex] = pItem;

				CTEventPrivateStore Event;

				Event.SetID( CTEventPrivateStore::EID_ADD_WISHLIST );
				Event.SetIndex( iIndex );
				Event.SetItem( pItem );

				SetChanged();
				NotifyObservers( &Event );

				if( bSendPacket )
					g_pNet->Send_cli_SET_WISHITEM( iIndex, TempItem );

				return;
			}
		}
		g_itMGR.AppendChatMsg(STR_CANT_ADDITEM2BUYLIST, IT_MGR::CHAT_TYPE_SYSTEM );
	}
}
//*---------------------------------------------------------------------------//
/// @param iIndex	- WishList Index
/// @brief 찜리스트에서 특정 Item을 뺀다.
//*---------------------------------------------------------------------------//
void CPrivateStore::RemoveItemWishList( int iIndex )
{
	if( IsOpened() ) return;

	if( iIndex < 0 || iIndex >= MAX_WISH_ITEMS )
	{
		assert( iIndex >= 0 && iIndex < MAX_WISH_ITEMS );
		return;
	}

	CItem* pItem = m_WishItems[iIndex];
	assert( pItem );
	if( pItem )
	{
		CTEventPrivateStore Event;
		Event.SetID( CTEventPrivateStore::EID_REMOVE_WISHLIST );
		Event.SetIndex( iIndex );
		SetChanged();
		NotifyObservers( &Event );

		tagITEM Item;
		Item.Clear();

		g_pNet->Send_cli_SET_WISHITEM( iIndex, Item );

		delete pItem;
		m_WishItems[iIndex] = NULL;
	}
}
//*---------------------------------------------------------------------------//
/// @brief 판매리스트를 모두 지운다.
//*---------------------------------------------------------------------------//
void CPrivateStore::ClearSellList()
{
	std::vector< CItemFragment* >::iterator iter;
	CItemFragment* pFragmentItem;
	int iIndex = 0;
	for( iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter, ++iIndex )
	{
		pFragmentItem = *iter;
		if( pFragmentItem )
		{
			delete pFragmentItem;
			m_SellItems[iIndex] = NULL;
		}
	}
	
	
}
//*---------------------------------------------------------------------------//
/// @brief 구입리스트를 모두 지운다.
//*---------------------------------------------------------------------------//
void CPrivateStore::ClearBuyList()
{
	std::list< CItemFragment* >::iterator iter;
	CItemFragment* pFragmentItem;

	for( iter = m_BuyItems.begin(); iter != m_BuyItems.end(); )
	{
		pFragmentItem = *iter;
		iter = m_BuyItems.erase( iter );
		delete pFragmentItem;
	}
}

//*---------------------------------------------------------------------------//
/// @brief 개인상점을 연다
//*---------------------------------------------------------------------------//
bool CPrivateStore::Open()
{
	if( IsOpened() ) return false;

	//---------------------------------------------------------------------------------
	/// PVP존에서는 개인상점 개설 금지.
	//---------------------------------------------------------------------------------
	if( g_pTerrain->IsPVPZone() )
	{
		g_itMGR.AppendChatMsg( STR_CANT_OPEN_PRIVATESTORE, IT_MGR::CHAT_TYPE_SYSTEM );							
		return false;
	}

	//---------------------------------------------------------------------------------
	/// PET 모드 에서는 개인상점 개설 금지.
	//---------------------------------------------------------------------------------
	if( g_pAVATAR->GetPetMode() >= 0 )
	{
		g_itMGR.AppendChatMsg(STR_CANT_OPEN_PRIVATESTORE, IT_MGR::CHAT_TYPE_SYSTEM );
		return false;
	}


	if( g_pAVATAR->Get_STATE() != CS_STOP )
	{
		g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
		return false;
	}

	///CItemSlot에 Observer로 등록
	if( m_strTitle.empty() )
		m_strTitle = STR_PRIVATESTORE;

	std::vector< CItemFragment* >::iterator iter;

	int iSellItemCount = 0;

	CItemFragment* pFragmentItem;
	tagPS_ITEM SellItems[MAX_P_STORE_ITEM_SLOT];
	for( iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter )
	{
		pFragmentItem = *iter;
		if( pFragmentItem )
		{
			SellItems[iSellItemCount].m_btInvIDX		= pFragmentItem->GetIndex();
			SellItems[iSellItemCount].m_dwPricePerEA	= pFragmentItem->GetUnitPrice();
			SellItems[iSellItemCount].m_ITEM			= pFragmentItem->GetItem();

			if( SellItems[iSellItemCount].m_ITEM.IsEnableDupCNT() )
				SellItems[iSellItemCount].m_ITEM.m_uiQuantity = pFragmentItem->GetQuantity();

			++iSellItemCount;
		}
	}

	int iBuyItemCount = 0;
	tagPS_ITEM BuyItems[MAX_P_STORE_ITEM_SLOT];	

	__int64 i64RequireMoney = 0;

	std::list< CItemFragment* >::iterator iterBuyList;
	for( iterBuyList = m_BuyItems.begin(); iterBuyList != m_BuyItems.end(); ++iterBuyList, ++iBuyItemCount )
	{
		pFragmentItem = *iterBuyList;
		BuyItems[iBuyItemCount].m_btWishIDX		= pFragmentItem->GetIndex();
		BuyItems[iBuyItemCount].m_dwPricePerEA	= pFragmentItem->GetUnitPrice();
		BuyItems[iBuyItemCount].m_ITEM			= pFragmentItem->GetItem();

		if( BuyItems[iBuyItemCount].m_ITEM.IsEnableDupCNT() )
			BuyItems[iBuyItemCount].m_ITEM.m_uiQuantity = pFragmentItem->GetQuantity();

		i64RequireMoney += pFragmentItem->GetUnitPrice() * pFragmentItem->GetQuantity();
	}
	
	if( iSellItemCount <= 0 && iBuyItemCount <= 0 )
		return false;
	
	if( g_pAVATAR->Get_MONEY() < i64RequireMoney )
	{
		g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MONEY_PRIVATESTORE );
		return false;
	}
	
	
	g_pNet->Send_cli_P_STORE_OPEN( (BYTE)iSellItemCount,(BYTE)iBuyItemCount, SellItems,BuyItems, (char*)m_strTitle.c_str() );
	m_bOpened = true;
	return true;
}

//*---------------------------------------------------------------------------//
/// @brief 개인상점을 중단한다
//*---------------------------------------------------------------------------//
void CPrivateStore::Close()
{
	g_pNet->Send_cli_P_STORE_CLOSE();
	m_bOpened = false;
}


//*---------------------------------------------------------------------------//
/// @brief 개인상점을 중단한다(UI가 아닌다)
//*---------------------------------------------------------------------------//
void CPrivateStore::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );
	if( pObj == NULL || strcmp( pObj->toString(), "CTEventItem" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type " );
		return;
	}
	
	CTEventItem* pEvent = (CTEventItem*)pObj;
	if( pEvent->GetID() == CTEventItem::EID_DEL_ITEM )///아이템이 삭제되었을경우
	{
		std::vector< CItemFragment* >::iterator iter;
		CItemFragment* pFragmentItem;
		int iSlot = 0;
		for( iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter, ++iSlot )
		{
			pFragmentItem = *iter;
			if( pFragmentItem && pFragmentItem->GetIndex() == pEvent->GetIndex() )
			{
				CTEventPrivateStore Event;

				Event.SetID( CTEventPrivateStore::EID_REMOVE_SELLLIST );
				Event.SetIndex( iSlot );

				SetChanged();
				NotifyObservers( &Event );

				delete pFragmentItem;
				m_SellItems[iSlot] = NULL;

				break;
			}
		}

	}
}


const char* CPrivateStore::GetTitle()
{
	return m_strTitle.c_str();
}
void CPrivateStore::SetTitle( const char* pszTitle )
{
	if( !CFilterWord::GetInstance().CheckString( (char*)pszTitle ) )
	{
		g_itMGR.AppendChatMsg(STR_FILTER_BADWORD,  IT_MGR::CHAT_TYPE_SYSTEM );
	}
	m_strTitle = CFilterWord::GetInstance().GetChangedString();
}

CItem*	CPrivateStore::GetWishItem( int iIndex )
{
	assert( iIndex >= 0 && iIndex < MAX_WISH_ITEMS );
	if( iIndex >= 0 && iIndex < MAX_WISH_ITEMS )
		return m_WishItems[iIndex];

	return NULL;
}

bool CPrivateStore::IsOpened()
{
	return m_bOpened;
}

void CPrivateStore::UpdateBuyList( tagPS_SLOT_ITEM& SlotITEM )
{
	std::list< CItemFragment* >::iterator iter;
	CItemFragment* pFragmentItem;
	for( iter = m_BuyItems.begin(); iter != m_BuyItems.end(); ++iter )
	{
		pFragmentItem = *iter;
		if( pFragmentItem->GetIndex() == SlotITEM.m_btSLOT )
		{
			if( SlotITEM.m_SlotITEM.IsEmpty() )///다 팔렸다.
			{
				CTEventPrivateStore Event;

				Event.SetID( CTEventPrivateStore::EID_REMOVE_BUYLIST );
				Event.SetIndex( SlotITEM.m_btSLOT );
				Event.SetItem( m_WishItems[SlotITEM.m_btSLOT] );

				SetChanged();
				NotifyObservers( &Event );

				m_BuyItems.erase( iter );
				delete pFragmentItem;
			}
			else
			{
				if( SlotITEM.m_SlotITEM.IsEnableDupCNT() )
				{
					pFragmentItem->SetQuantity( SlotITEM.m_SlotITEM.GetQuantity() );
				}
				else
				{
					assert( 0 && "수량이 없는 아이템을 업데이트 하려고한다" );
				}
			
			}
			break;
		}
	}		
}

void CPrivateStore::UpdateSellList( tagPS_SLOT_ITEM& SlotITEM )
{
	assert( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT < m_SellItems.size() );
	if( SlotITEM.m_btSLOT >= 0 && SlotITEM.m_btSLOT < m_SellItems.size()  )
	{
		CItemFragment* pFragmentItem = m_SellItems[ SlotITEM.m_btSLOT ];
		if( pFragmentItem )///서버에서 Recv_gsv_P_STORE_MONEYnINV을 먼저 받았을경우 삭제되었을수 있다
		{
			CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
			int iInvenIndex = pFragmentItem->GetIndex();
			if( SlotITEM.m_SlotITEM.IsEmpty() )///다 팔렸다.
			{
				CTEventPrivateStore Event;

				Event.SetID( CTEventPrivateStore::EID_REMOVE_SELLLIST );
				Event.SetIndex( SlotITEM.m_btSLOT );

				SetChanged();
				NotifyObservers( &Event );

				delete pFragmentItem;
				m_SellItems[SlotITEM.m_btSLOT] = NULL;

			}
			else///몇개만 팔렸다.
			{
				if( SlotITEM.m_SlotITEM.IsEnableDupCNT() )
				{
					//int iSubQuantity = pFragmentItem->GetQuantity() - SlotITEM.m_SlotITEM.GetQuantity();
					pFragmentItem->SetQuantity( SlotITEM.m_SlotITEM.GetQuantity() );
			/*		CItem* pItem = pFragmentItem->GetInnerCItem();
					pItem->SubQuantity( iSubQuantity );*/
					///인벤토리에서 해당개수만큼 삭제
				}
				else
				{
					assert( 0 && "수량이 없는 아이템을 업데이트 하려고한다" );
				}
			}
		}
	}
}

void CPrivateStore::Clear()
{
	m_bOpened = false;

	std::vector< CItemFragment* >::iterator iterSell;
	CItemFragment* pFragmentItem = NULL;
	CTEventPrivateStore Event;
	int iIndex = 0;
	for( iIndex = 0, iterSell = m_SellItems.begin(); iterSell != m_SellItems.end(); ++iIndex ,++iterSell)
	{
		pFragmentItem = *iterSell;
		if( pFragmentItem )
		{
			Event.SetID( CTEventPrivateStore::EID_REMOVE_SELLLIST );
			Event.SetIndex( iIndex );

			SetChanged();
			NotifyObservers( &Event );
			delete pFragmentItem;
			*iterSell = NULL;
		}

	}

	std::list< CItemFragment* >::iterator iterBuy;
	for(iIndex = 0, iterBuy = m_BuyItems.begin(); iterBuy != m_BuyItems.end(); ++iIndex, ++iterBuy )
	{
		pFragmentItem = *iterBuy;

		Event.SetID( CTEventPrivateStore::EID_REMOVE_BUYLIST );
		Event.SetIndex( iIndex );
		Event.SetItem( m_WishItems[iIndex] );
		SetChanged();
		NotifyObservers( &Event );

		delete pFragmentItem;
		*iterBuy = NULL;


	}

	for( iIndex = 0 ; iIndex < MAX_WISH_ITEMS; ++iIndex )
	{
		if( m_WishItems[iIndex] )
		{
			Event.SetID( CTEventPrivateStore::EID_REMOVE_WISHLIST );
			Event.SetIndex( iIndex );
			SetChanged();
			NotifyObservers( &Event );

			SAFE_DELETE( m_WishItems[iIndex] );
		}
	}
}

bool CPrivateStore::IsValidOpen()
{
	std::list< NpcInfoInMap > npcInfoInMapList = g_pObjMGR->GetNpcInfoList();
	std::list< NpcInfoInMap >::iterator begin = npcInfoInMapList.begin();

	D3DVECTOR vecCenter = g_pAVATAR->Get_CurPOS();
	for( ; begin != npcInfoInMapList.end() ; ++begin )
	{
		NpcInfoInMap& npcInfoInMap = *begin;

		int iDistance = CD3DUtil::distance ((int)npcInfoInMap.m_Position.x, (int)npcInfoInMap.m_Position.y, (int)vecCenter.x, (int)vecCenter.y);
		if( abs( iDistance ) < 1500 )
		{
			g_itMGR.AppendChatMsg( STR_TOO_NEAR_NPC, IT_MGR::CHAT_TYPE_SYSTEM );							
			return false;
		}
	}					
	return true;
}