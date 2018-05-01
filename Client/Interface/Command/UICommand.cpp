#include "stdafx.h"
#include "uicommand.h"
#include "CTCmdNumberInput.h"

#include "../it_Mgr.h"
#include "../../Object.h"
#include "../../System/CGame.h"

#include "../Dlgs/CNumberInputDlg.h"
#include "../Dlgs/QuickToolBAR.h"
#include "../Dlgs/CUpgradeDlg.h"
#include "../Dlgs/DeliveryStoreDlg.h"
#include "../Icon/CIconItem.h"
#include "../Icon/CIconDialog.h"

#include "../../Common/CItem.h"
#include "../../Common/IO_Pat.h"
#include "../../GameData/CManufacture.h"
#include "../../GameData/CSeparate.h"
#include "../../gamedata/cprivatestore.h"
#include "../../GameCommon/Item.h"
#include "../../System/CGame.h"

#include "../../Network/CNetwork.h"
#include "../Dlgs/CCommDlg.h"
#include "../ClanMarkTransfer.h"
#include "../../Country.h"


bool CTCmdTakeInItem2MakeDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "CTCmdTakeInItem2MakeDlg::Exec" );
		return true;
	}
	
	if( strcmp( pObj->toString(),"CIcon" ) == 0 )
		CManufacture::GetInstance().SetMaterialItem( ((CIconItem*)pObj)->GetCItem() );
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
		CManufacture::GetInstance().SetMaterialItem( (CItem*)pObj );
	else
		assert( 0 && "Invalid TObject Type @CTCmdTakeInItem2makedlg" );

	return true;
}


bool CTCmdTakeOutItemFromMakeDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "CTCmdTakeOutItemFromMakeDlg::Exec" );
		return true;
	}

	if( strcmp( pObj->toString(),"CIcon" ) == 0 )
		CManufacture::GetInstance().RemoveMaterialItem( ((CIconItem*)pObj)->GetCItem() );
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
		CManufacture::GetInstance().RemoveMaterialItem( (CItem*)pObj );
	else
		assert( 0 && "Invalid TObject Type @CTCmdTakeOutItemFrommakedlg" );

	return true;
}


bool CTCmdTakeOutItemFromSeparateDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj );
		return true;
	}

	if( strcmp( pObj->toString(),"CIcon" ) == 0 )
		CSeparate::GetInstance().RemoveItem();
	else
		assert( 0 && "Invalid TObject Type @CTCmdTakeOutItemFromSeparateDlg" );

	return true;
}


///*-----------------------------------------------------------------------------------------
//CTCmdAssembleRideItem::CTCmdAssembleRideItem( short nPartIdx, short nInvenIdx , short nEquipedBodyPartItemNo )
//{
//	m_nPartIdx	= nPartIdx;
//	m_nInvenIdx = nInvenIdx;
//	m_nEquipedBodyPartItemNo = nEquipedBodyPartItemNo;
//}
bool CTCmdAssembleRideItem::Exec( CTObject* pObj )
{
	if( g_pAVATAR->GetPetMode() >= 0 )
	{
		if( ( g_pAVATAR->GetPetState() != CS_STOP ) )
		{
			g_itMGR.AppendChatMsg( STR_ACTION_COMMAND_STOP_STATE_FAILED, IT_MGR::CHAT_TYPE_SYSTEM );
			return false;
		}
	}


#ifdef _GBC
	// 홍근 : 2인승 카트	
	// 카트 보조석에 게스트가 타고 있을때,
	// 카트 보조석에 타고 있을때
	// 아이템 교환 금지.	
	if( g_pAVATAR->GetRideUserIndex() || g_pAVATAR->IsRideUser() )
	{	
		g_itMGR.AppendChatMsg( STR_BOARDING_CANT_USE, IT_MGR::CHAT_TYPE_SYSTEM );		
		return false;
	}
#endif



	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdAssembleRideItem::Exec" );
		return true;
	}

	short nInvenIdx = 0;
	short nItemNo   = 0;

	if( strcmp(pObj->toString(), "CIcon" ) == 0 )
	{
		CIconItem* pItemIcon = (CIconItem*)pObj;
		nItemNo		= pItemIcon->GetItemNo();
		nInvenIdx	= pItemIcon->GetIndex();
	}
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
	{
		CItem* pItem = (CItem*)pObj;
		nItemNo		= pItem->GetItemNo();
		nInvenIdx	= pItem->GetIndex();
	}
	else
	{
		assert( 0 && "Invalid TObject Type @CTCmdAssembleRideItem::Exec" );
		return true;
	}

	short nPartIdx = PAT_ITEM_PART_IDX( nItemNo );

	CItemSlot* pItemSlot = g_pAVATAR->GetItemSlot();

	CItem* pBodyItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 );///장착되어있는 아이템
	CItem* pItem	 = pItemSlot->GetItem( nInvenIdx );///장착하려는 아이템
	if( pItem == NULL )
	{
		assert( pItem );
		return true;
	}

	if( !g_pAVATAR->Check_PatEquipCondition ( pItem->GetItem() ) )
	{
		g_itMGR.AppendChatMsg( STR_NOTIFY_06, IT_MGR::CHAT_TYPE_SYSTEM );
		return false;
	}


#if defined(_GBC)
	///직업 제한
	if( !g_pAVATAR->Check_JobCollection(PAT_ITEM_EQUIP_REQUIRE_CLASS( nItemNo )) )
	{
		g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_CONDITION, IT_MGR::CHAT_TYPE_SYSTEM );
		return true;
	}
#endif
	
	
	if( nPartIdx != RIDE_PART_BODY )
	{
		if( pBodyItem )
		{
			if( PAT_ITEM_PART_TYPE( pBodyItem->GetItemNo() ) != PAT_ITEM_PART_TYPE( nItemNo ) )
			{
				g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_EQUAL_CLASS, IT_MGR::CHAT_TYPE_SYSTEM );
				return true;
			}

			if( PAT_ITEM_PART_VER( pBodyItem->GetItemNo() ) < PAT_ITEM_PART_VER( nItemNo ) ) 
			{
				g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_VERSION, IT_MGR::CHAT_TYPE_SYSTEM );
				return true;
			}

		}
		else
		{
			g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_EQUIP_BODY, IT_MGR::CHAT_TYPE_SYSTEM );			
			return true;
		}
	}
	else///바디 부품일경우
	{
		if( g_pAVATAR->GetPetMode() >= 0 )///드라이브 스킬 사용중이라면
		{
			///같은 타입만 장착할수 있다.
			if( pBodyItem && PAT_ITEM_PART_TYPE( pBodyItem->GetItemNo() ) != PAT_ITEM_PART_TYPE( pItem->GetItemNo() ) )
			{
				g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_EQUAL_CLASS, IT_MGR::CHAT_TYPE_SYSTEM );
				return true;
			}

			CItem* pPartItem = NULL;
			for( int i = 1; i < MAX_RIDING_PART; ++i )
			{
				pPartItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 + i);
				if( pPartItem )
				{   
					if( PAT_ITEM_PART_TYPE( pItem->GetItemNo() ) != PAT_ITEM_PART_TYPE( pPartItem->GetItemNo() ))
					{
						g_itMGR.AppendChatMsg(STR_PAT_ERROR_NOT_EQUAL_CLASS, IT_MGR::CHAT_TYPE_SYSTEM );
						return true;
					}

					if( PAT_ITEM_PART_VER( pItem->GetItemNo() ) < PAT_ITEM_PART_VER( pPartItem->GetItemNo() ) )
					{
						g_itMGR.AppendChatMsg(STR_ERROR_EQUIP_PAT_BODY_VER, IT_MGR::CHAT_TYPE_SYSTEM );
						return true;
					}
				}
			}
		}
		else///드라이브 스킬 사용중이 아니라면
		{
			/// 새로 장착하는 Body와 다른 Parts가 장착되어 있다면 다 떨군다.
			/// 새로 장착하는 Body의 버젼보다 낮은 부품이 있다면 다 떨군다.
			CItem* pPartItem = NULL;
			for( int i = 1; i < MAX_RIDING_PART; ++i )
			{
				pPartItem = pItemSlot->GetItem( INVENTORY_RIDE_ITEM0 + i);
				if( pPartItem )
				{   
					if( PAT_ITEM_PART_TYPE( pItem->GetItemNo() ) != PAT_ITEM_PART_TYPE( pPartItem->GetItemNo() ) )
						g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( i, 0 );
					else if( PAT_ITEM_PART_VER( pItem->GetItemNo() ) < PAT_ITEM_PART_VER( pPartItem->GetItemNo() ) )
						g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( i, 0 );
				}
			}
		}		
	}



	g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( nPartIdx, nInvenIdx );

	return true;
}

bool CTCmdDisAssembleRideItem::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdDisAssembleRideItem::Exec" );
		return true;
	}

	short nPartIdx;
	if( strcmp(pObj->toString(), "CIcon" ) == 0 )
	{
		CIconItem* pItemIcon = (CIconItem*)pObj;
		nPartIdx = pItemIcon->GetIndex() - INVENTORY_RIDE_ITEM0;
		g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( nPartIdx, 0 );
	}
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
	{
		CItem* pItem = (CItem*)pObj;
		nPartIdx = pItem->GetIndex() - INVENTORY_RIDE_ITEM0;
		g_pNet->Send_cli_ASSEMBLE_RIDE_ITEM( nPartIdx, 0 );
	}
	else
	{
		assert( 0 && "Invalid TObject Type @CTCmdDisAssembleRideItem::Exec" );
	}
	
	return true;
}
///*-----------------------------------------------------------------------------------------


///*-----------------------------------------------------------------------------------------
CTCmdOpenNumberInputDlg::CTCmdOpenNumberInputDlg(void)
{
	m_pCmd = NULL;
	m_i64Maximum = 0;
}

bool CTCmdOpenNumberInputDlg::Exec( CTObject* pObj )
{
	CTDialog* pDlg = NULL;
	pDlg = g_itMGR.FindDlg( DLG_TYPE_N_INPUT );
	if( pDlg )
	{
		CNumberInputDlg* pNInputDlg = (CNumberInputDlg*)pDlg;
		
		__int64 iMaxNumber = 0;
		CTObject* pCmdParam = NULL;
		if( pObj == NULL )///돈일경우
		{
			iMaxNumber = (int)m_i64Maximum;
			LogString( LOG_NORMAL,"maximum money is 0" );
			if( iMaxNumber == 0 )
				return true;
		}
		else if( strcmp( pObj->toString(), "CIcon" ) == 0 )///드래그앤드랍에서 실행되었을 경우
		{
			tagITEM& Item = ((CIconItem*)pObj)->GetItem();
			pCmdParam = ((CIconItem*)pObj)->GetCItem();
			if( Item.IsEnableDupCNT() )
			{
				if( m_i64Maximum )			
					iMaxNumber = m_i64Maximum;
				else
					iMaxNumber = Item.GetQuantity();
			}
			else
				iMaxNumber = 1;
		}
		else if( strcmp( pObj->toString(), "CItem" ) == 0 )
		{
			tagITEM& Item = ((CItem*)pObj)->GetItem();
			pCmdParam = pObj;
			if( Item.IsEnableDupCNT() )
			{
				if( m_i64Maximum )			
					iMaxNumber = m_i64Maximum;
				else
					iMaxNumber = Item.GetQuantity();
			}
			else
				iMaxNumber = 1;
		}
		else
		{
			assert( 0 && "알수 없는 CTobject Type @CTCmdOpenNumberInputDlg::Exec" );
			return true;
		}

		if( iMaxNumber > 1 )
		{
			pNInputDlg->SetMaxNumber( iMaxNumber );
			pNInputDlg->SetCommand( m_pCmd, pCmdParam );
			g_itMGR.OpenDialog( DLG_TYPE_N_INPUT );
		}
		else if( iMaxNumber == 1 )
		{
			m_pCmd->SetNumber( 1 );
			m_pCmd->Exec( pCmdParam );
		}
		else
		{
			assert( 0 && "Maximum 값이 0보다 작거나 같다");
		}
	}

	return true;
}

void CTCmdOpenNumberInputDlg::SetCommand( CTCmdNumberInput* pCmd )
{
	m_pCmd = pCmd;
}

void CTCmdOpenNumberInputDlg::SetMaximum( __int64 i64Maximum )
{
	m_i64Maximum = i64Maximum;
}

//*---------------------------------------------------------------------
#include "../dlgs/CAvatarStoreDlg.h"
bool CTCmdDragItem2AvatarStoreDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL || strcmp( pObj->toString(), "CIcon" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type" );
		return true;
	}

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_AVATARSTORE );
	if( pDlg )
	{
		CAvatarStoreDlg* pStoreDlg = ( CAvatarStoreDlg*) pDlg;
		CIconItem* pItemIcon = (CIconItem*)pObj;
		
		int iCount = pStoreDlg->IsBuyItem( pItemIcon->GetItem() );
		if( iCount > 0 )
		{
			m_i64Maximum = iCount;
			CTCmdOpenNumberInputDlg::Exec(pItemIcon->GetCItem());
		}
	}
	return true;
}

CTCmdAcceptAddFriend::CTCmdAcceptAddFriend( WORD wUserIdx, BYTE btStatus, const char* pszName )
{
	
	m_wUserIdx	= wUserIdx;
	m_btStatus	= btStatus;
	assert( pszName );
	if( pszName )
		m_strName	= pszName;

}

bool CTCmdAcceptAddFriend::Exec(CTObject* pObj )
{
	g_pNet->Send_cli_MCMD_APPEND_REPLY( MSGR_CMD_APPEND_ACCEPT, m_wUserIdx , (char*)m_strName.c_str() );
	return true;
}

CTCmdRejectAddFriend::CTCmdRejectAddFriend( WORD wUserIdx , const char* pszName )
{
	m_wUserIdx = wUserIdx;
	m_strName = pszName;
}

bool CTCmdRejectAddFriend::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_MCMD_APPEND_REPLY( MSGR_CMD_APPEND_REJECT, m_wUserIdx , (char*)m_strName.c_str() );
	return true;
}

CTCmdRemoveFriend::CTCmdRemoveFriend( DWORD dwUserTag )
{
	m_dwUserTag = dwUserTag;
}

bool CTCmdRemoveFriend::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_MCMD_TAG( MSGR_CMD_DELETE, m_dwUserTag );
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_COMMUNITY );
	assert( pDlg );
	if( pDlg )
	{
		CCommDlg* pCommDlg = ( CCommDlg* )pDlg;
		pCommDlg->RemoveFriend( m_dwUserTag );
	}
	return true;
}

CTCmdAbandonQuest::CTCmdAbandonQuest( int iQuestSlotIdx,int iQuestID )
{
	m_iQuestSlotIdx	= iQuestSlotIdx;
	m_iQuestID		= iQuestID;

}

bool CTCmdAbandonQuest::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_QUEST_REQ (TYPE_QUEST_REQ_DEL, m_iQuestSlotIdx, m_iQuestID );
	return true;
}

CTCmdSendPacketDropItem::CTCmdSendPacketDropItem( short nInvenIdx, int iQuantity )
{
	m_nInvenIdx = nInvenIdx;
	m_iQuantity = iQuantity;;

}

bool CTCmdSendPacketDropItem::Exec( CTObject* pObj )
{
	LogString( LOG_NORMAL,CStr::Printf( "Send_cli_Drop_item %d %d" , m_nInvenIdx, m_iQuantity ) );
	g_pNet->Send_cli_DROP_ITEM( m_nInvenIdx, m_iQuantity );
	return true;
}

bool CTCmdMoveDialogIcon2Ground::Exec( CTObject* pObj )
{
	assert( pObj );
	if( pObj == NULL ) return true;
	CIconDialog* pIcon = (CIconDialog*)pObj;

	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	ptMouse.x -= pIcon->GetWidth() / 2;
	ptMouse.y -= pIcon->GetHeight() / 2;

	g_itMGR.SetDialogIconPosition( pIcon->GetDialogType(), ptMouse );

	return true;
}

bool CTCmdMoveDialogIcon2GroundFromMenu::Exec( CTObject* pObj )
{
	assert( pObj );
	if( pObj == NULL ) return true;

	CIconDialog* pIcon = (CIconDialog*)pObj;

	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	ptMouse.x -= pIcon->GetWidth() / 2;
	ptMouse.y -= pIcon->GetHeight() / 2;

	g_itMGR.SetDialogIconPosition( pIcon->GetDialogType(), ptMouse );
	g_itMGR.AddDialogIcon( pIcon->GetDialogType() );

	return true;
}

CTCmdRemoveDialogIcon::CTCmdRemoveDialogIcon( int iDialogType )
{
	m_iDialogType = iDialogType;
}

bool CTCmdRemoveDialogIcon::Exec( CTObject* pObj )
{
	g_itMGR.DelDialogIcon( m_iDialogType );
	return true;
}

bool CTCmdRegistDialogIcon2QuickBar::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( 0 && "Invalid Param @CTCmdDragInven2QuickBar::Exec" );
		return true;
	}
	
	CIconDialog* pIcon = (CIconDialog*)pObj;

	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUICKBAR );
	if( pDlg == NULL )
	{
		assert( 0 && "Not Found QuickBar Dialog @CTCmdDragInven2QuickBar::Exec" );
		return true;
	}
	
	CQuickBAR* pQuickBar = (CQuickBAR*)pDlg;

	short nQuickSlotIdx = pQuickBar->GetMouseClickSlot( ptMouse );

	if( nQuickSlotIdx == -1 )///해당 위치에 슬롯이 없다.
		return true;

	tagHotICON hotICON;

	hotICON.m_cType   = DIALOG_ICON;
	hotICON.m_nSlotNo = pIcon->GetDialogType();

	if( hotICON.m_nSlotNo < 0 || hotICON.m_nSlotNo >= DLG_TYPE_MAX   )
	{
		assert( 0 && "Invalid Dialog Type @CTCmdRegistDialogIcon2QuickBar::Exec" );
		return true;
	}

	g_pNet->Send_cli_SET_HOTICON( (BYTE)nQuickSlotIdx, hotICON );
	return true;
}

CTCmdAddItem2WishList::CTCmdAddItem2WishList( tagITEM& Item )
{
	m_Item = Item;
}

bool CTCmdAddItem2WishList::Exec( CTObject* pObj )
{
	CPrivateStore::GetInstance().AddItemWishList( m_Item, true );
	return true;
}



CTCmdChangeStateUpgradeDlg::CTCmdChangeStateUpgradeDlg( int iState )
{
	m_iState = iState;
}

bool CTCmdChangeStateUpgradeDlg::Exec( CTObject* pObj )
{
	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_UPGRADE ) )
	{
		CUpgradeDlg* pUpgradeDlg = (CUpgradeDlg*)pDlg;
		pUpgradeDlg->ChangeState( m_iState );
	}
	return true;
}

bool CTCmdEndRepair::Exec( CTObject* pObj )
{
	CGame::GetInstance().EndRepair();
	return true;
}


bool CTCmdEndAppraisal::Exec( CTObject* pObj )
{
	CGame::GetInstance().EndAppraisal();
	return true;
}

CTCmdSendAppraisalReq::CTCmdSendAppraisalReq( int iIndex )
{
	m_iIndex = iIndex;
}

bool CTCmdSendAppraisalReq::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_APPRAISAL_REQ( m_iIndex );
	return true;
}

CTCmdQuerySellItem2PrivateStore::CTCmdQuerySellItem2PrivateStore( WORD wSellerSvrIdx, int iItemCount, tagSELL_ITEM& Item )
{
	m_wSellerSvrIdx	= wSellerSvrIdx;
	m_iItemCount	= iItemCount;
	m_SellItem		= Item;


}

bool CTCmdQuerySellItem2PrivateStore::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_P_STORE_SELL_REQ( m_wSellerSvrIdx, m_iItemCount, &m_SellItem );
	return true;
}


CTCmdQueryBuyItemFromPrivateStore::CTCmdQueryBuyItemFromPrivateStore( WORD wSellerSvrIdx, int iItemCount, tagPS_SLOT_ITEM& Item )
{
	m_wSellerSvrIdx	= wSellerSvrIdx;
	m_iItemCount	= iItemCount;;
	m_BuyItem		= Item;
}
bool CTCmdQueryBuyItemFromPrivateStore::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_P_STORE_BUY_REQ( m_wSellerSvrIdx, m_iItemCount, &m_BuyItem );
	return true;
}

CTCmdAcceptReqJoinClan::CTCmdAcceptReqJoinClan( const char* pszMasterName )
{
	assert( pszMasterName );
	if( pszMasterName )
		m_strMasterName = pszMasterName;
}

bool CTCmdAcceptReqJoinClan::Exec( CTObject* pObj )
{
	assert( m_strMasterName.c_str() );
	if( m_strMasterName.c_str() )
		g_pNet->Send_cli_CLAN_COMMAND(   GCMD_INVITE_REPLY_YES ,  (char*)m_strMasterName.c_str()  );
	return true;
}

CTCmdRejectReqJoinClan::CTCmdRejectReqJoinClan( const char* pszMasterName )
{
	assert( pszMasterName );
	if( pszMasterName )
		m_strMasterName = pszMasterName;
}

bool CTCmdRejectReqJoinClan::Exec( CTObject* pObj )
{
	assert( m_strMasterName.c_str() );
	if( m_strMasterName.c_str() )
		g_pNet->Send_cli_CLAN_COMMAND(   GCMD_INVITE_REPLY_NO ,  (char*)m_strMasterName.c_str()  );
	return true;
}

CTCmdClanCommand::CTCmdClanCommand( BYTE btCmd, const char* pszMsg )
{
	m_btCmd = btCmd;
	if( pszMsg )
		m_strMsg = pszMsg;
	else
		m_strMsg.clear();
}

bool CTCmdClanCommand::Exec( CTObject* pObj )
{
	if( m_strMsg.empty() )
		g_pNet->Send_cli_CLAN_COMMAND( m_btCmd, NULL );
	else
		g_pNet->Send_cli_CLAN_COMMAND( m_btCmd, (char*)m_strMsg.c_str() );
	return true;
}

bool CTCmdCancelWaitDisconnect::Exec( CTObject* pObj )
{
	g_pNet->Send_cli_LOGOUT_CANCEL();
	g_itMGR.ChangeState( IT_MGR::STATE_NORMAL );
	return true;
}
/*--------------------------------------------------------------------------------------------------------------*/
void CTCmdInputName::SetName( const char* pszName )
{
	assert( pszName && strlen( pszName ) > 0);
	if( pszName && strlen( pszName ) > 0 )
		m_name = pszName;
}

bool CTCmdInputName::Exec( CTObject* pObj )
{
	return true;
}

/*--------------------------------------------------------------------------------------------------------------*/
bool CTCmdInputNameGiftReceiver::Exec( CTObject* pObj )
{
	if( m_name.empty() ) return true;

	if( CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_DELIVERYSTORE ) )
	{
		CDeliveryStoreDlg* p = (CDeliveryStoreDlg*)pDlg;
		p->save_receiver_name( m_name.c_str() );
	}
	
	g_pNet->Send_cli_MAIL_ITEM_FIND_CHAR( (char*)m_name.c_str() );
	return true;
}

CTCmdGiftMallItem::CTCmdGiftMallItem( BYTE slotindex, const char* name )
{
	assert( name );
	if( name )
	{
		m_slotindex = slotindex;
		m_name      = name;
	}
}

bool CTCmdGiftMallItem::Exec( CTObject* pObj )
{
	if( !m_name.empty() )
		g_pNet->Send_cli_MALL_ITEM_GIVE( m_slotindex, (char*)m_name.c_str() );
	return true;
}

CTCmdRegisterClanMark::CTCmdRegisterClanMark( int clan_id, const char* filename )
{
	m_clan_id = clan_id;
	assert( filename );
	if( filename )
		m_filename = filename;
}

bool CTCmdRegisterClanMark::Exec( CTObject* pObj )
{
	if( m_filename.empty() ) return true;

	CClanMarkTransfer::GetSingleton().RegisterMarkToServer( m_clan_id, m_filename.c_str() );
	return true;
}

CTCmdChangeStateInterface::CTCmdChangeStateInterface( int state )
{
	m_state = state;
}

bool CTCmdChangeStateInterface::Exec( CTObject* pObj )
{
	g_itMGR.ChangeState( m_state );
	return true;
}
