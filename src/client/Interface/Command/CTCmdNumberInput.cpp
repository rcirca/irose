#include "stdafx.h"
#include ".\ctcmdnumberinput.h"

#include "../IO_ImageRes.h"
#include "../IO_Terrain.h"
#include "../InterfaceType.h"

#include "../it_Mgr.h"
#include "../InterfaceType.h"
#include "../Dlgs/SubClass/CSlot.h"
#include "../Dlgs/DealDlg.h"
#include "../Dlgs/MakeDlg.h"
#include "../Dlgs/CBankDlg.h"
#include "../Dlgs/CAvatarStoreDlg.h"

#include "../Icon/CIconItem.h"

#include "../../Network/CNetwork.h"
#include "../../gamedata/cdealdata.h"
#include "../../gamedata/cstore.h"
#include "../../GameData/CExchange.h"
#include "../../GameData/CBank.h"

#include "../../gamecommon/item.h"
#include "../../common/calculation.h"
#include "../../System/CGame.h"
#include "../../misc/gameutil.h"
#include "UICommand.h"

#include "../Country.h"
#include "../System/CGame.h"

const __int64 MAX_TRADE_MONEY = 1000000000;///교환시 최대 거래 돈

/*----------------------------------------------------------------------------------------------------*/
bool CTCmdDropItem::Exec( CTObject* pObj )
{
	//if( m_iNumber <= 0  )
	//	return true;
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdDropItem::Exec" );
		return true;
	}

	int iInvenIndex = 0;
	tagITEM Item;
	if( strcmp( pObj->toString(), "CIcon" ) == 0 )
	{
		iInvenIndex = ((CIconItem*)pObj)->GetIndex();
		Item = ((CIconItem*)pObj)->GetItem();
		if( !Item.IsEnableDROP() )
		{
			g_itMGR.AppendChatMsg(STR_DONT_DROP_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );		
			return true;
		}
	}
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
	{
		iInvenIndex = ((CItem*)pObj)->GetIndex();
		Item = ((CItem*)pObj)->GetItem();
		if( !Item.IsEnableDROP() )
		{
			g_itMGR.AppendChatMsg(STR_DONT_DROP_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );		
			return true;
		}
	}
	else
	{
		assert( 0 && "Invalid TObject Type @CTCmdDropItem::Exec" );
		return true;
	}


	long lPrice = g_pTerrain->m_Economy.Get_ItemSellPRICE( 
			Item,
			g_pAVATAR->GetSellSkillVALUE())  * (int)m_iNumber  ;

	if( lPrice >= 1000 )
	{
		CTCommand* pCmd = new CTCmdSendPacketDropItem( iInvenIndex, (int)m_iNumber );
		g_itMGR.OpenMsgBox( 
			CStr::Printf(F_STR_QUERY_DROP_ITEM,ITEM_NAME( Item.GetTYPE(), Item.GetItemNO() )) ,
			CMsgBox::BT_OK | CMsgBox::BT_CANCEL,true, 0, pCmd, 0, 0 
		);
	}
	else
	{
		CTCmdSendPacketDropItem Cmd(  iInvenIndex, (int)m_iNumber  );
		Cmd.Exec( NULL );
	}
		

	return true;
}

bool CTCmdDropMoney::Exec( CTObject* pObj )
{
	if( m_iNumber >= 1000 )
	{
		CTCommand* pCmd = new CTCmdSendPacketDropItem( 0, (int)m_iNumber );
		std::string strMsg = CStr::Printf("%d",m_iNumber );
		strMsg.append( STR_MONEY );

		g_itMGR.OpenMsgBox( CStr::Printf( F_STR_QUERY_DROP_ITEM, strMsg.c_str() ),
			CMsgBox::BT_OK | CMsgBox::BT_CANCEL,true, 0, pCmd, 0, 0 
		);
	}
	else
	{
		CTCmdSendPacketDropItem Cmd(  0, (int)m_iNumber  );
		Cmd.Exec( NULL );
	}
	return true;
}
/*----------------------------------------------------------------------------------------------------*/


bool CTCmdAddMyItem2Exchange::Exec( CTObject* pObj )
{
	if( CExchange::GetInstance().IsReadyMe() )
		return true;

	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdRemoveMyItemFromExchange" );	
		return true;
	}

	if( strcmp( pObj->toString(), "CItem" ) == 0 )
	{
		tagITEM& Item = ((CItem*)pObj)->GetItem();
		CExchange::GetInstance().AddMyItem( (CItem*)pObj, (int)m_iNumber );

	}
	else
	{
		assert( 0 && "CTObject Type is Invalid @CTCmdAddMyItem2Exchange::Exec" );	
	}
	return true;
}

bool CTCmdAddMyMoney2Exchange::Exec( CTObject* pObj )
{
	if( m_iNumber <= 0 )
		return true;

	if( CExchange::GetInstance().IsReadyMe() )
		return true;

	__int64 i64PrevTradeMoney = CExchange::GetInstance().GetMyTradeMoney();

	if( i64PrevTradeMoney + m_iNumber > MAX_TRADE_MONEY )
		CExchange::GetInstance().SetMyTradeMoney( MAX_TRADE_MONEY );
	else
		CExchange::GetInstance().SetMyTradeMoney( i64PrevTradeMoney + m_iNumber );

	return true;
}

bool CTCmdRemoveMyMoneyFromExchange::Exec( CTObject* pObj )
{
	if( m_iNumber <= 0 )
		return true;

	if( CExchange::GetInstance().IsReadyMe() )
		return true;


	__int64 i64PrevTradeMoney = CExchange::GetInstance().GetMyTradeMoney();

	if( i64PrevTradeMoney <= m_iNumber )
		CExchange::GetInstance().SetMyTradeMoney( 0 );
	else
		CExchange::GetInstance().SetMyTradeMoney( i64PrevTradeMoney - m_iNumber );

	return true;
}

bool CTCmdRemoveMyItemFromExchange::Exec( CTObject *pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdRemoveMyItemFromExchange" );	
		return true;
	}

	if( CExchange::GetInstance().IsReadyMe() )
		return true;

	if( strcmp( pObj->toString(), "CIcon" ) == 0 )
	{
		CExchange::GetInstance().RemoveMyItemByInvenIndex( ((CIconItem*)pObj)->GetIndex() );
	}
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
	{
		CExchange::GetInstance().RemoveMyItemByInvenIndex( ((CItem*)pObj)->GetIndex() );
	}
	else
	{
		assert( 0 && "CTObject Type is Invalid @CTCmdAddMyItem2Exchange::Exec" );	
	}

	return true;
}
/*-----------------------------------------------------------------------------------------------*/
/// 계정창고 관련
bool CTCmdMoveItemInv2Bank::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdMoveItemInv2Bank" );
		return true;
	}

	if( strcmp( pObj->toString(), "CItem" ) )
	{
		assert( 0 && "Invalid TObject Type @CTCmdMoveItemInv2Bank" );	
		return true;
	}

	CItem* pItem = (CItem*)pObj;
	
	tagITEM Item = pItem->GetItem();

	/// 보관 가능한 아이템인가?
	if( !Item.IsEnableKEEPING() )
	{
		g_itMGR.OpenMsgBox(STR_BANK_CANT_STORE_ITEM);
		return true;
	}


	bool bPlatinum = false;
	int  iSlotNum = 0;
	BOOL bFullFirstTab = FALSE;
	
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_BANK ) )
	{
		CBankDlg* p = (CBankDlg*)pDlg;

		iSlotNum= p->GetCurrentTab();

		bFullFirstTab = p->IsFullFirstTab();

		if( iSlotNum == 3 )
		{
			bPlatinum = true;
		}
	}


	if( CCountry::GetSingleton().IsJapan() )
	{
		if( bFullFirstTab )
		{
			if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_EXTRA_STOCK) )
			{
				g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_BANK_EXTRASPACE );
				return true;
			}
		}		
	}
	else if( bPlatinum )///일본이 아닌경우 플래티넘 탭은 플래티넘 사용자만 물건을 넣을수 있다.
	{
		if( CGame::GetInstance().GetPayType() != CGame::PAY_PLATINUM )
		{
			g_itMGR.OpenMsgBox( STR_ONLY_PLATINUM_TAB_BANK );
			return true;
		}	
	}
	
	
	/// 빈슬롯이 있는가?///현재는 모두 True이다
	if( !CBank::GetInstance().HasEmptySlot( bPlatinum ) )
	{
		g_itMGR.OpenMsgBox(STR_BANK_NOT_ENOUGH_SPACE);
		return true;
	}


	int iCount = 1;
	if( Item.IsEnableDupCNT())
	{
		Item.m_uiQuantity = (unsigned)m_iNumber;
		iCount = (int)m_iNumber;
	}

	
	int iStorageFee = CCal::Get_StorageFEE( ITEM_BASE_PRICE( Item.GetTYPE(), Item.GetItemNO() ),
											ITEM_PRICE_RATE( Item.GetTYPE(), Item.GetItemNO() ),
											iCount 
										);
	///보관료가 있는가?
	if( iStorageFee > g_pAVATAR->Get_MONEY() )
	{
		g_itMGR.OpenMsgBox(STR_BANK_NOT_ENOUGH_MONEY);
		return true;
	}

	g_pNet->Send_cli_MOVE_ITEM( MOVE_ITEM_TYPE_INV2BANK, pItem->GetIndex() ,Item, bPlatinum);
	return true;
}


bool CTCmdMoveItemBank2Inv::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdMoveItemBank2Inv" );
		return true;
	}
	if( strcmp( pObj->toString(), "CItem" ) )
	{
		assert( 0 && "Invalid TObject Type @CTCmdMoveItemBank2Inv" );	
		return true;
	}

	
	if(0)
	{	
		bool bPlatinum = false;
		int  iSlotNum = 0;
		BOOL bFullFirstTab = FALSE;

		if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_BANK ) )
		{
			CBankDlg* p = (CBankDlg*)pDlg;
			if( p )
			{
				iSlotNum = p->GetCurrentTab();
				
				bFullFirstTab = p->IsFullFirstTab();			
			}			
		}
		
		if( CCountry::GetSingleton().IsJapan() )
		{
			if( bFullFirstTab )
			{
				if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_EXTRA_STOCK) )
				{
					g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_BANK_EXTRASPACE );
					return true;
				}
			}			
		}		
	}

	CItem* pItem = (CItem*)pObj;
	
	tagITEM& Item = pItem->GetItem();
	
	if( Item.IsEnableDupCNT())
		Item.m_uiQuantity = (unsigned)m_iNumber;

	std::list< tagITEM > appendItems;
	appendItems.push_back( Item );
//	if( g_pAVATAR->IsInventoryFull( appendItems ) )
//		g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_INVENTORY_SPACE );
//	else
	g_pNet->Send_cli_MOVE_ITEM( MOVE_ITEM_TYPE_BANK2INV, pItem->GetIndex() ,Item , true );
	return true;
}
///*-----------------------------------------------------------------------------------------------*/
bool CTCmdAddItem2DealFromInventory::Exec( CTObject* pObj )
{
	///pObj는 Icon혹은 Item이 올수 있다.
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdAddItem2DealFromInventory" );
		return true;
	}

	if( !g_itMGR.IsDlgOpened( DLG_TYPE_DEAL ) )
		return true;


	int iIndex = 0;
	if( strcmp( pObj->toString(), "CIcon" ) == 0 )
	{
		iIndex = ((CIconItem*)pObj)->GetIndex();
		tagITEM& Item = ((CIconItem*)pObj)->GetItem();
		if( Item.IsEnableSELL() )
			CDealData::GetInstance().AddItem2SellList( iIndex, (int)m_iNumber );
		else
			g_itMGR.AppendChatMsg( STR_DONT_SELL_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );
	}
	else if( strcmp( pObj->toString(), "CItem") == 0 )
	{
		iIndex = ((CItem*)pObj)->GetIndex();
		tagITEM& Item = ((CItem*)pObj)->GetItem();
		if( Item.IsEnableSELL() )
			CDealData::GetInstance().AddItem2SellList( iIndex, (int)m_iNumber );
		else
			g_itMGR.AppendChatMsg( STR_DONT_SELL_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );
	}
	else
	{
		assert( 0 && "Invalid CTObject type @CTCmdAddItem2DealFromInventory::Exec" );
	}

	return true;
}


bool CTCmdAddItem2DealFromStore::Exec(CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdAddItem2DealFromStore" );
		return true;
	}

	if( !g_itMGR.IsDlgOpened( DLG_TYPE_DEAL ) )
		return true;

	int iIndex = 0;
	if( strcmp( pObj->toString(), "CIcon" ) == 0 )
	{
		iIndex = ((CIconItem*)pObj)->GetIndex();
		CDealData::GetInstance().AddItem2BuyList( iIndex, (int)m_iNumber );
	}
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
	{
		iIndex = ((CItem*)pObj)->GetIndex();
		CDealData::GetInstance().AddItem2BuyList( iIndex, (int)m_iNumber );
	}
	else
	{
		assert( 0 && "Invalide CTObject Type @CTCmdAddItem2DealFromStore::Exec" );
	}
	return true;
}

///*--------------------------------------------------------------------------------------*/
bool CTCmdBuyItem::Exec(CTObject* pObj )
{
	if( !g_itMGR.IsDlgOpened( DLG_TYPE_STORE ) )
		return true;

	///pObj는 Icon혹은 Item이 올수 있다.
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdSellItem" );
		return true;
	}

	///
	tag_BUY_ITEM BuyItem;
	BuyItem.m_wDupCNT = (WORD)m_iNumber;
	CItem* pItem = NULL;

	if( strcmp( pObj->toString(), "CIcon" ) == 0 )
	{
		pItem = ((CIconItem*)pObj)->GetCItem();
	}
	else if( strcmp( pObj->toString(), "CItem") == 0 )
	{
		pItem = (CItem*)pObj;
	}
	else
	{
		assert( 0 && "Invalid CTObject type @CTCmdBuyItem::Exec" );
		return true;
	}

	tagITEM& Item = pItem->GetItem();
	
	tagITEM TempItem = Item;
	
	if( TempItem.IsEnableDupCNT() )
		TempItem.m_uiQuantity = m_iNumber;

	std::list< tagITEM > appendItems;
	std::list< pair< int, tagITEM> > removeItems;
	appendItems.push_back( TempItem );

	if( g_pAVATAR->IsInventoryFull( appendItems ,removeItems ) )
	{
		g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_INVENTORY_SPACE );
		return true;
	}

	long lPrice = 0;
	if( CStore::GetInstance().IsUnionStore() )
	{
		lPrice = ITEM_TRADE_UNIONPOINT( Item.GetTYPE(), Item.GetItemNO() ) * m_iNumber;
		if( lPrice > g_pAVATAR->Get_AbilityValue( AT_UNION_POINT1 - 1 + g_pAVATAR->Get_UNION() ))
		{
			g_itMGR.OpenMsgBox( STR_WANT_UNIONPOINT );
			return true;
		}
		else
		{
			int iValue = g_pAVATAR->Get_AbilityValue( AT_UNION_POINT1 - 1 + g_pAVATAR->Get_UNION() );
			g_pAVATAR->Set_AbilityValue( AT_UNION_POINT1 - 1 + g_pAVATAR->Get_UNION(), iValue - lPrice );
		}
	}
	else
	{
		lPrice = g_pTerrain->m_Economy.Get_ItemBuyPRICE( Item.GetTYPE(), Item.GetItemNO(), 
			g_pAVATAR->GetBuySkillVALUE() ) * (int)m_iNumber ;

		if( lPrice > g_pAVATAR->Get_MONEY() )
		{
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MONEY );	
			return true;
		}
	}

	BuyItem.m_cColNO  = pItem->GetIndex() % c_iSlotCountPerTab;
	BuyItem.m_cTabNO  = pItem->GetIndex() / c_iSlotCountPerTab;
	g_pNet->Send_cli_STORE_TRADE_REQ( g_pObjMGR->Get_ServerObjectIndex( CStore::GetInstance().GetNpcObjIndex() ), 1, 0, &BuyItem, NULL );
	return true;
}

bool CTCmdSellItem::Exec(CTObject* pObj )
{
	if( !g_itMGR.IsDlgOpened( DLG_TYPE_STORE ) )
		return true;

	///pObj는 Icon혹은 Item이 올수 있다.
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdSellItem" );
		return true;
	}

	tag_SELL_ITEM SellItem;
	
	if( strcmp( pObj->toString(), "CIcon" ) == 0 )
	{
		SellItem.m_btInvIDX = ((CIconItem*)pObj)->GetIndex();
		SellItem.m_wDupCNT  = (WORD)m_iNumber;

		tagITEM& Item = ((CIconItem*)pObj)->GetItem();
		if( Item.IsEnableSELL() )
			g_pNet->Send_cli_STORE_TRADE_REQ( g_pObjMGR->Get_ServerObjectIndex( CStore::GetInstance().GetNpcObjIndex()), 0, 1, NULL, &SellItem );
		else
			g_itMGR.AppendChatMsg( STR_DONT_SELL_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );
		
	}
	else if( strcmp( pObj->toString(), "CItem") == 0 )
	{
		SellItem.m_btInvIDX = ((CItem*)pObj)->GetIndex();
		SellItem.m_wDupCNT  = (WORD)m_iNumber;
		tagITEM& Item = ((CItem*)pObj)->GetItem();
		if( Item.IsEnableSELL() )
			g_pNet->Send_cli_STORE_TRADE_REQ( g_pObjMGR->Get_ServerObjectIndex( CStore::GetInstance().GetNpcObjIndex() ), 0, 1, NULL, &SellItem );
		else
			g_itMGR.AppendChatMsg( STR_DONT_SELL_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );
	}
	else
	{
		assert( 0 && "Invalid CTObject type @CTCmdSellItem::Exec" );
	}
	return true;
}

/*----------------------------------------------------------------------------------------------------*/
bool CTCmdBuyItemAtAvatarStore::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( 0 && "TObject is NULL " );	
		return true;
	}

	if( strcmp(pObj->toString(), "CItem") )
	{
		assert( 0 && "Invalid TObject Type" );	
		return true;
	}

	CItem* pItem = (CItem*)pObj;

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
	if( pDlg )
	{
		CAvatarStoreDlg* pStoreDlg = ( CAvatarStoreDlg*) pDlg;
		CIconItem* pItemIcon = (CIconItem*)pObj;

		tagPS_SLOT_ITEM Item;
		Item.m_btSLOT = pItem->GetIndex();
		Item.m_SlotITEM = pItem->GetItem();


		if( Item.m_SlotITEM.IsEnableDupCNT() )
		{
			assert( Item.m_SlotITEM.m_uiQuantity >= m_iNumber );
			if( Item.m_SlotITEM.m_uiQuantity > m_iNumber )
				Item.m_SlotITEM.m_uiQuantity = m_iNumber;
		}

		std::list< tagITEM > appendItems;
		appendItems.push_back( Item.m_SlotITEM );

		if( g_pAVATAR->IsInventoryFull( appendItems ) )///
		{
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_INVENTORY_SPACE );
			return true;
		}
		///돈 체크 해주자..
		__int64 i64RequireMoney = Item.m_SlotITEM.GetQuantity() * pItem->GetUnitPrice();

		if( g_pAVATAR->Get_MONEY() < i64RequireMoney )
		{
			g_itMGR.AppendChatMsg(STR_NOT_ENOUGH_MONEY, IT_MGR::CHAT_TYPE_SYSTEM );		
			return true;
		}

		CTCommand* pCmd = new CTCmdQueryBuyItemFromPrivateStore( pStoreDlg->GetMasterSvrObjIdx(), 1, Item );

		std::string strMsg = Item.m_SlotITEM.GetName();
		strMsg.append(" ");
		strMsg.append( CStr::Printf(STR_QUERY_BUYITEMFROMPRIVATESTORE, Item.m_SlotITEM.GetQuantity() ) );


		//char buf[128];
		//_i64toa( pItem->GetUnitPrice() * Item.m_SlotITEM.GetQuantity(), buf, 10 );
		
		__int64 total_trade_money = pItem->GetUnitPrice() * Item.m_SlotITEM.GetQuantity();
		const int money_buffer_size = 64;
		char money_buffer[ money_buffer_size ];
		CGameUtil::ConvertMoney2String( total_trade_money, money_buffer, money_buffer_size );
		strMsg.append( CStr::Printf(STR_TOTAL_TRADEMONEY_PRIVATESTORE, money_buffer ) );

		g_itMGR.OpenMsgBox( strMsg.c_str(), CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );

	}
	return true;
}

bool CTCmdSellItem2AvatarStore::Exec( CTObject* pObj )
{
	if( pObj == NULL )
		return true;

	if( strcmp( pObj->toString(), "CItem" ) )
		return true;



	//WORD wStoreObjectIDX, int iItemCount, tagSELL_ITEM*	pSellItems
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
	if( pDlg )
	{
		CAvatarStoreDlg* pStoreDlg = ( CAvatarStoreDlg*) pDlg;
		CItem* pItem = ( CItem*)pObj;
		
		tagSELL_ITEM	SellItem;
		SellItem.m_btInvIDX = pItem->GetIndex();
		SellItem.m_SellITEM = pItem->GetItem();

		if( SellItem.m_SellITEM.IsEnableDupCNT() )
		{
			assert( SellItem.m_SellITEM.m_uiQuantity >= m_iNumber );
			if(  SellItem.m_SellITEM.m_uiQuantity < m_iNumber )
				return true;
			
			SellItem.m_SellITEM.m_uiQuantity = (int)m_iNumber;
		}

		int iUnitPrice = 0;
		int iWishIndex = pStoreDlg->GetWishIndex( SellItem.m_SellITEM, iUnitPrice );
		assert( iWishIndex >= 0 && iWishIndex < MAX_WISH_ITEMS );
		if( iWishIndex >= 0 && iWishIndex < MAX_WISH_ITEMS )
		{
			SellItem.m_btWishSLOT = iWishIndex;
			
			std::string strMsg = SellItem.m_SellITEM.GetName();
			strMsg.append(" ");
			strMsg.append( CStr::Printf(STR_QUERY_SELLITEM2PRIVATESTORE, SellItem.m_SellITEM.GetQuantity() ) );
			char buf[128];
			_i64toa( iUnitPrice * SellItem.m_SellITEM.GetQuantity(), buf, 10 );

			strMsg.append( CStr::Printf(STR_TOTAL_TRADEMONEY_PRIVATESTORE, buf ) );

			CTCommand* pCmd = new CTCmdQuerySellItem2PrivateStore( pStoreDlg->GetMasterSvrObjIdx(), 1, SellItem );
			g_itMGR.OpenMsgBox( strMsg.c_str(), CMsgBox::BT_OK | CMsgBox::BT_CANCEL, true, 0, pCmd );
			
			//g_pNet->Send_cli_P_STORE_SELL_REQ( pStoreDlg->GetMasterSvrObjIdx(), 1, &SellItem );
		}
	}
	return true;
}
/*----------------------------------------------------------------------------------------------------*/
bool CTCmdBringItemFromMall::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "CTCmdBringItemFromMall::Exec" );
		return true;
	}
	
	BYTE slotindex = 0;
	CItem* pItem   = NULL;

	if( strcmp( pObj->toString(),"CIcon" ) == 0 )
	{
		slotindex = (BYTE)((CIconItem*)pObj)->GetIndex();
		pItem     = ((CIconItem*)pObj)->GetCItem();
	}
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
	{
		slotindex = (BYTE)((CItem*)pObj)->GetIndex();
		pItem     = (CItem*)pObj;
	}
	else
	{
		assert( 0 && "Invalid TObject Type @CTCmdBringItemFromMall" );
		return true;
	}

	if( pItem == NULL )
		return true;

	std::list< tagITEM > appendItems;
	appendItems.push_back( pItem->GetItem() );

	if( g_pAVATAR->IsInventoryFull( appendItems ) )///
	{
		g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_INVENTORY_SPACE );
		return true;
	}

	//STR_NOT_ENOUGH_INVENTORY_SPACE
	
	short nQuantity = 1;
	if( pItem->GetItem().IsEnableDupCNT() )
		nQuantity = (short)m_iNumber;
	
	if( slotindex >= 1 )
		g_pNet->Send_cli_MALL_ITEM_BRING( slotindex - 1, nQuantity );
	return true;
}