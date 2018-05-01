#include "stdafx.h"
#include ".\cdealdata.h"
#include "event/cteventdeal.h"
#include "../Object.h"
#include "../gamecommon/item.h"
#include "../gamedata/cstore.h"
#include "../Network/CNetwork.h"
#include "../IO_Terrain.h"
#include "../interface/it_mgr.h"
#include "Event/CTEventItem.h"

CDealData::CDealData(void)
{
	m_pEvent = new CTEventDeal;
}

CDealData::~CDealData(void)
{
	SAFE_DELETE( m_pEvent );
}

CDealData& CDealData::GetInstance()
{
	static CDealData s_Instance;
	return s_Instance;
}

void CDealData::Update( CObservable* pObservable, CTObject* pObj )
{
	//만약 CItemSlot에 있는 Item중 지워진경우에 이 Method가 호출된다.
	assert( pObservable );

	if( pObj && strcmp( pObj->toString(), "CTEventItem" ) == 0 )
	{
		
		CItemFragment* pFragmentItem = NULL;
		CTEventItem* pEvent = ( CTEventItem* )pObj;
		int iIndex = pEvent->GetIndex();
		
		switch( pEvent->GetID() )
		{
		case CTEventItem::EID_DEL_ITEM:
			{
				std::vector< CItemFragment* >::iterator iter;
				for( iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter )
				{
					pFragmentItem = *iter;
					if( pFragmentItem->GetIndex() == iIndex )
					{
						m_pEvent->SetID( CTEventDeal::EID_REMOVE_SELLITEM );
						m_pEvent->SetItem( pFragmentItem );

						SetChanged();
						NotifyObservers( m_pEvent );

						delete pFragmentItem;
						iter = m_SellItems.erase( iter );
						return;
					}
				}
				break;
			}
		case CTEventItem::EID_CHANGE_ITEM:///수량이 변경되었을경우에 비교해서 지운다.
			{
				CItem* pItem = pEvent->GetItem();
				std::vector< CItemFragment* >::iterator iter;
				for( iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter )
				{
					pFragmentItem = *iter;
					if( pFragmentItem->GetIndex() == iIndex )
					{
						if( pFragmentItem->GetQuantity() > pItem->GetQuantity() )
						{
							m_pEvent->SetID( CTEventDeal::EID_REMOVE_SELLITEM );
							m_pEvent->SetItem( pFragmentItem );

							SetChanged();
							NotifyObservers( m_pEvent );

							delete pFragmentItem;
							iter = m_SellItems.erase( iter );
						}
						return;
					}
				}
				break;
			}
		default:
			break;
		}
	}
	else
	{
		assert( 0 && "pObj is NULL or Invalid Type @CDealData::Update() " );
	}

}

void CDealData::AddItem2BuyList( int iIndex, int iQuantity )
{
	if( m_BuyItems.size() >= TOTAL_DEAL_INVENTORY )
		return;

	CItem* pItem = 	CStore::GetInstance().GetItem( iIndex );

	if( pItem )
	{
		CItemFragment* pFragmentItem = new CItemFragment( pItem );
		pFragmentItem->SetQuantity( iQuantity );
		m_BuyItems.push_back( pFragmentItem );
		
		m_pEvent->SetID( CTEventDeal::EID_ADD_BUYITEM );
		m_pEvent->SetItem( pFragmentItem );

		SetChanged();
		NotifyObservers( m_pEvent );
	}
	else
	{
		assert( 0 && "Not Found Item by Index @CDealData::AddItem2BuyList " );
	}
}

void CDealData::AddItem2SellList( int iIndex, int iQuantity )
{
	if( m_SellItems.size() >= TOTAL_DEAL_INVENTORY )
		return;

	assert( g_pAVATAR );
	CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();
	assert( pItemSlot );

	CItem* pItem = pItemSlot->GetItem( iIndex );
	if( pItem )
	{
		CItemFragment* pFragmentItem = new CItemFragment( pItem );
		pFragmentItem->SetQuantity( iQuantity );
		m_SellItems.push_back( pFragmentItem );

		m_pEvent->SetID( CTEventDeal::EID_ADD_SELLITEM );
		m_pEvent->SetItem( pFragmentItem );

		SetChanged();
		NotifyObservers( m_pEvent );
	}
	else
	{
		assert( 0 && "Not Found CItem By Index @CDealData::AddItem2SellList" );
	}
}

void CDealData::ClearTradeList()
{
	std::vector< CItemFragment* >::iterator iter;
	CItemFragment* pFragmentItem = NULL;
	for( iter = m_BuyItems.begin(); iter != m_BuyItems.end(); )
	{
		pFragmentItem = *iter;
		iter = m_BuyItems.erase( iter );
		delete pFragmentItem;
	}

	for( iter = m_SellItems.begin(); iter != m_SellItems.end(); )
	{
		pFragmentItem = *iter;
		iter = m_SellItems.erase( iter );
		delete pFragmentItem;
	}

	m_pEvent->SetID( CTEventDeal::EID_CLEAR );
	SetChanged();
	NotifyObservers( m_pEvent );
}

void CDealData::RemoveItemFromBuyList( CItem* pItem )
{
	if( pItem )
	{
		std::vector< CItemFragment* >::iterator iter;
		CItemFragment* pFragmentItem = NULL;
		for( iter = m_BuyItems.begin(); iter != m_BuyItems.end(); ++iter )
		{
			pFragmentItem = *iter;
			if( ((CItem*)pFragmentItem) == pItem )
			{
				m_pEvent->SetID( CTEventDeal::EID_REMOVE_BUYITEM );
				m_pEvent->SetItem( pFragmentItem );
			
				SetChanged();
				NotifyObservers( m_pEvent );
				
				delete pFragmentItem;
				iter = m_BuyItems.erase( iter );

				return;
			}
		}
		assert( 0 && "Buy List에 없는 아이템을 빼려고 시도합니다 @CDealData::RemoveItemFromBuyList" );
	}
	else
	{
		assert( 0 && "Not Found CItem By index @CDealData::RemoveItemFromBuyList ");
	}
}

void CDealData::RemoveItemFromSellList( CItem* pItem )
{
	if( pItem )
	{
		std::vector< CItemFragment* >::iterator iter;
		CItemFragment* pFragmentItem = NULL;
		for( iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter )
		{
			pFragmentItem = *iter;
			if( ((CItem*)pFragmentItem) == pItem )
			{
				m_pEvent->SetID( CTEventDeal::EID_REMOVE_SELLITEM );
				m_pEvent->SetItem( pFragmentItem );

				SetChanged();
				NotifyObservers( m_pEvent );

				delete pFragmentItem;
				iter = m_SellItems.erase( iter );
				return;
			}
		}
		assert( 0 && "Sell List에 없는 아이템을 빼려고 시도합니다" );
	}
	else
	{
		assert( 0 && "Not Found CItem by index @CDealData::RemoveItemFromSellList" );
	}
}

unsigned CDealData::GetTradeItemCnt()
{
	return GetBuyItemCnt() + GetSellItemCnt();
}

unsigned CDealData::GetBuyItemCnt()
{
	return m_BuyItems.size();
}

unsigned CDealData::GetSellItemCnt()
{
	return m_SellItems.size();
}

void CDealData::SendTradeReq()
{
	long  lTotalSELL  = GetTotal_SELL();
	long  lTotalBUY   = GetTotal_BUY();
	long  lCurMoney   = g_pAVATAR->Get_MONEY() + lTotalSELL;

	
	if( CStore::GetInstance().IsUnionStore() )
	{
		if( lTotalBUY > g_pAVATAR->Get_AbilityValue( AT_UNION_POINT1 - 1 + g_pAVATAR->Get_UNION() ) )
		{
			g_itMGR.OpenMsgBox( STR_WANT_UNIONPOINT );
			return;
		}
	}
	else
	{
		/// 팔꺼 살꺼 다 계산해서..
		if( lCurMoney - lTotalBUY < 0 ) 
		{
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MONEY );	
			return; //돈이 모자란다 .....
		}
	}

	
	tag_BUY_ITEM  BuyITEMs [ TOTAL_DEAL_INVENTORY ];
	tag_SELL_ITEM SellITEMs[ TOTAL_DEAL_INVENTORY ];

	ZeroMemory( BuyITEMs, sizeof( BuyITEMs ) );
	ZeroMemory( SellITEMs, sizeof( SellITEMs ) );

	tagITEM TempItem;
	std::list< tagITEM > appendItems;
	std::list<  pair<int , tagITEM > > removeItems;
	std::vector< CItemFragment* >::iterator iter;
	int iCount = 0;

	for( iCount = 0, iter = m_BuyItems.begin(); iter != m_BuyItems.end(); ++iter,++iCount )
	{
		BuyITEMs[iCount].m_cTabNO	= (*iter)->GetIndex() / c_iSlotCountPerTab;
		BuyITEMs[iCount].m_cColNO	= (*iter)->GetIndex() % c_iSlotCountPerTab;
		BuyITEMs[iCount].m_wDupCNT	= (*iter)->GetQuantity();
		TempItem = (*iter)->GetItem();
		if( TempItem.IsEnableDupCNT() )
			TempItem.m_uiQuantity = (*iter)->GetQuantity();

		appendItems.push_back( TempItem );
	}

	for( iCount = 0, iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter, ++iCount )
	{
		SellITEMs[iCount].m_btInvIDX	= (*iter)->GetIndex();
		SellITEMs[iCount].m_wDupCNT		= (*iter)->GetQuantity();

		TempItem = (*iter)->GetItem();
		if( TempItem.IsEnableDupCNT() )
			TempItem.m_uiQuantity = (*iter)->GetQuantity();

		removeItems.push_back( make_pair( (*iter)->GetIndex(), TempItem ) ) ;
	}


	if( g_pAVATAR->IsInventoryFull( appendItems, removeItems ) )
	{
		g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_INVENTORY_SPACE );
		return;
	}

	g_pNet->Send_cli_STORE_TRADE_REQ( g_pObjMGR->Get_ServerObjectIndex( CStore::GetInstance().GetNpcObjIndex() ), 
		(char)GetBuyItemCnt(), (char)GetSellItemCnt(), 
		BuyITEMs, SellITEMs );

	if( CStore::GetInstance().IsUnionStore() )
	{
		int iValue = g_pAVATAR->Get_AbilityValue( AT_UNION_POINT1 - 1 + g_pAVATAR->Get_UNION() );
		g_pAVATAR->Set_AbilityValue( AT_UNION_POINT1 - 1 + g_pAVATAR->Get_UNION(), iValue - lTotalBUY );
	}

	ClearTradeList();
}

long CDealData::GetTotal_BUY()
{
	long lPrice = 0;
	std::vector< CItemFragment* >::iterator iter;
	if( CStore::GetInstance().IsUnionStore() )
	{
		for( iter = m_BuyItems.begin(); iter != m_BuyItems.end(); ++iter )
			lPrice += ITEM_TRADE_UNIONPOINT( (*iter)->GetItem().GetTYPE(), (*iter)->GetItem().GetItemNO() )  * (*iter)->GetQuantity() ;
	}
	else
	{
		for( iter = m_BuyItems.begin(); iter != m_BuyItems.end(); ++iter )
		{
			lPrice += g_pTerrain->m_Economy.Get_ItemBuyPRICE( 
				(*iter)->GetItem().GetTYPE(), 
				(*iter)->GetItem().GetItemNO(),
				g_pAVATAR->GetBuySkillVALUE() ) * (*iter)->GetQuantity() ;
		}
	}
	return lPrice;
}

long CDealData::GetTotal_SELL()
{
	long lPrice = 0;
	std::vector< CItemFragment* >::iterator iter;

	for( iter = m_SellItems.begin(); iter != m_SellItems.end(); ++iter )
	{
		lPrice += g_pTerrain->m_Economy.Get_ItemSellPRICE( 
			(*iter)->GetItem(),
			g_pAVATAR->GetSellSkillVALUE() ) * (*iter)->GetQuantity() ;
	}

	return abs(lPrice);
}

