#include "stdafx.h"
#include "dragcommand.h"

#include "../it_mgr.h"
#include "../dlgs/QuickToolBar.h"
#include "../dlgs/CPatDlg.h"
#include "../dlgs/CItemDlg.h"
#include "../dlgs/CGoodsDlg.h"
#include "../dlgs/CPrivateStoreDlg.h"
#include "../Icon/CIconItem.h"
#include "../Icon/CIconSkill.h"
#include "../Icon/CIconQuick.h"
#include "../Icon/CIconSkillClan.h"
#include "../../network/Cnetwork.h"
#include "../../System/CGame.h"
#include "../../gamecommon/item.h"
#include "uicommand.h"
#include "../../GameData/CPrivateStore.h"
#include "../../GameData/CSeparate.h"

bool CTCmdDragInven2QuickBar::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( 0 && "Invalid Param @CTCmdDragInven2QuickBar::Exec" );
		return true;
	}
	
	CIconItem* pItemIcon = (CIconItem*)pObj;

	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

#ifdef _NEW_UI
	CTDialog* pDlg = g_itMGR.FindDlg( m_nType );
#else
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUICKBAR );
#endif
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

	hotICON.m_cType = INV_ICON;
	hotICON.m_nSlotNo = pItemIcon->GetIndex();

	if( hotICON.m_nSlotNo < INVENTORY_ITEM_INDEX_0 || hotICON.m_nSlotNo > INVENTORY_ITEM_INDEX_LAST  )
	{
		assert( 0 && "Invalid Inventory Index @CTCmdDragInven2QuickBar::Exec" );
		return true;
	}


	g_pNet->Send_cli_SET_HOTICON( (BYTE)nQuickSlotIdx, hotICON );
	return true;
}

bool	CTCmdDragSkill2QuickBar::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "Invalid Arg @CTCmdDragSkill2QuickBar::Exec" );
		return true;
	}
	
	CIconSkill* pSkillIcon = (CIconSkill*)pObj;

	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	CTDialog* pDlg = g_itMGR.FindDlg( m_nType );
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

	hotICON.m_cType = SKILL_ICON;
	hotICON.m_nSlotNo = pSkillIcon->GetSkillSlotFromIcon();

	if( hotICON.m_nSlotNo < 0 || hotICON.m_nSlotNo >= MAX_LEARNED_SKILL_CNT  )
	{
		assert( 0 && "Invalid Skill Slot Index @CTCmdDragSkill2QuickBar::Exec" );
		return true;
	}

	g_pNet->Send_cli_SET_HOTICON( (BYTE)nQuickSlotIdx, hotICON );
	
	return true;
}


bool	CTCmdDragClanSkill2QuickBar::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "Invalid Arg @CTCmdDragClanSkill2QuickBar::Exec" );
		return true;
	}
	
	CIconSkillClan* pSkillIcon = (CIconSkillClan*)pObj;

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

	hotICON.m_cType = CLANSKILL_ICON;
	hotICON.m_nSlotNo = pSkillIcon->GetSkillSlotFromIcon();

	if( hotICON.m_nSlotNo < 0 || hotICON.m_nSlotNo >= MAX_CLAN_SKILL_SLOT  )
	{
		assert( 0 && "Invalid Skill Slot Index @CTCmdDragClanSkill2QuickBar::Exec" );
		return true;
	}

	g_pNet->Send_cli_SET_HOTICON( (BYTE)nQuickSlotIdx, hotICON );
	
	return true;
}


///*------------------------------------------------------------------------------------------------------------*/
bool	CTCmdDragItemFromQuickBar::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "Invalid Arg @CTCmdItemFromQuickBar::Exec");	
		return true;
	}
	
	CIconQuick* pIcon = (CIconQuick*)pObj;
	
	short nSlotIndex = pIcon->GetQuickBarSlotIndex();

	if( nSlotIndex < 0 || nSlotIndex >= MAX_HOT_ICONS )
	{
		LogString( LOG_NORMAL,"Not Found SlotIndex By Icon Pointer @CTCmdDragItemFromQuickBar::Exec");
		return true;
	}

	tagHotICON hotICON;

	hotICON.m_cType = 0;
	hotICON.m_nSlotNo = 0;
	g_pNet->Send_cli_SET_HOTICON( (BYTE)nSlotIndex, hotICON );
	return true;
}
///*------------------------------------------------------------------------------------------------------------*/
bool	CTCmdMoveIconInQuickBar::Exec( CTObject* pObj )
{

#ifdef _NEW_UI

	if( pObj == NULL )
	{
		assert( pObj && "Invalid Arg @CTCmdItemFromQuickBar::Exec");	
		return true;
	}
	
	CIconQuick* pIcon = (CIconQuick*)pObj;
	
	short nPrevSlotIndex = pIcon->GetQuickBarSlotIndex();

	if( nPrevSlotIndex < 0 || nPrevSlotIndex >= MAX_HOT_ICONS )
	{
		LogString( LOG_NORMAL,"Invalid QuickBar SlotIndex @CTCmdDragItemFromQuickBar::Exec");
		return true;
	}


	CTDialog* pDlg = g_itMGR.FindDlg( m_nType );
	if( pDlg == NULL )
	{
		assert( pDlg && "Not Found QuickBar Dialog @CTCmdMoveIconInQuickBar::Exec" );
		return true;
	}
	CQuickBAR* pQuickBar = ( CQuickBAR*) pDlg;


	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	
	short nNewSlotIndex = pQuickBar->GetMouseClickSlot( ptMouse );
	if( nNewSlotIndex < 0 || nNewSlotIndex >= MAX_HOT_ICONS )
	{
		LogString( LOG_NORMAL,"Invalid QuickBar SlotIndex @CTCmdDragItemFromQuickBar::Exec");
		return true;
	}

	if( nNewSlotIndex == nPrevSlotIndex )
	{
		LogString( LOG_NORMAL,"SlotIndex Is Same @CTCmdDragItemFromQuickBar::Exec");
		return true;
	}
	///이전 슬롯 비우기
	tagHotICON hotICON;
	hotICON.m_cType = 0;
	hotICON.m_nSlotNo = 0;
	g_pNet->Send_cli_SET_HOTICON( (BYTE)nPrevSlotIndex, hotICON );


	///새로운 슬롯으로 이동
	CHotIconSlot* pHotIconSlot	= g_pAVATAR->GetHotIconSlot();
	hotICON						= pHotIconSlot->GetHotItem( nPrevSlotIndex );

	g_pNet->Send_cli_SET_HOTICON( (BYTE)nNewSlotIndex, hotICON );
	
	return true;


#else
	if( pObj == NULL )
	{
		assert( pObj && "Invalid Arg @CTCmdItemFromQuickBar::Exec");	
		return true;
	}
	
	CIconQuick* pIcon = (CIconQuick*)pObj;
	
	short nPrevSlotIndex = pIcon->GetQuickBarSlotIndex();

	if( nPrevSlotIndex < 0 || nPrevSlotIndex >= MAX_HOT_ICONS )
	{
		LogString( LOG_NORMAL,"Invalid QuickBar SlotIndex @CTCmdDragItemFromQuickBar::Exec");
		return true;
	}


	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_QUICKBAR );
	if( pDlg == NULL )
	{
		assert( pDlg && "Not Found QuickBar Dialog @CTCmdMoveIconInQuickBar::Exec" );
		return true;
	}
	CQuickBAR* pQuickBar = ( CQuickBAR*) pDlg;


	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	
	short nNewSlotIndex = pQuickBar->GetMouseClickSlot( ptMouse );
	if( nNewSlotIndex < 0 || nNewSlotIndex >= MAX_HOT_ICONS )
	{
		LogString( LOG_NORMAL,"Invalid QuickBar SlotIndex @CTCmdDragItemFromQuickBar::Exec");
		return true;
	}

	if( nNewSlotIndex == nPrevSlotIndex )
	{
		LogString( LOG_NORMAL,"SlotIndex Is Same @CTCmdDragItemFromQuickBar::Exec");
		return true;
	}
	///이전 슬롯 비우기
	tagHotICON hotICON;
	hotICON.m_cType = 0;
	hotICON.m_nSlotNo = 0;
	g_pNet->Send_cli_SET_HOTICON( (BYTE)nPrevSlotIndex, hotICON );


	///새로운 슬롯으로 이동
	CHotIconSlot* pHotIconSlot	= g_pAVATAR->GetHotIconSlot();
	hotICON						= pHotIconSlot->GetHotItem( nPrevSlotIndex );

	g_pNet->Send_cli_SET_HOTICON( (BYTE)nNewSlotIndex, hotICON );
	
	return true;	

#endif
}

bool CTCmdDragItemEquipFromInven::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdDragItemEquipFromInven::Exec" );
		return true;
	}
	if( strcmp( pObj->toString(), "CIcon" ) == 0 )
	{
		CIconItem* pItemIcon = (CIconItem*)pObj;
		CItem* pItem = pItemIcon->GetCItem();
		assert( pItem );
		if( pItem )
		{
			CTCommand* pCmd = pItem->GetCommand();
			if( pCmd )
				pCmd->Exec( pItem );
		}
	}
	else
	{
		assert( 0 && "Invalid CTObject type @CTCmdDragItemEquipFromInven" );
	}
	return true;
}



bool CTCmdDragItemFromEquipInItemDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL )
		return true;

	if( strcmp( pObj->toString(), "CIcon" ) )
		return true;

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_ITEM );
	if( pDlg == NULL )
		return true;

	CItemDlg* pItemDlg = (CItemDlg*)pDlg;
	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	if( pItemDlg->IsInsideInven( ptMouse ) )
	{
		CIconItem* pItemIcon = (CIconItem*)pObj;
		pItemIcon->ExecuteCommand();
	}
	return true;
}

bool CTCmdDragItemFromInvenInItemDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL )
		return true;

	if( strcmp( pObj->toString(), "CIcon" ) )
		return true;

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_ITEM );
	if( pDlg == NULL )
		return true;

	CIconItem* pItemIcon = (CIconItem*)pObj;
	CItemDlg* pItemDlg = (CItemDlg*)pDlg;
	
	POINT ptMouse;
	CGame::GetInstance().Get_MousePos( ptMouse );

	if( pItemDlg->IsInsideEquip( ptMouse ) )
	{

		int iItemType = pItemIcon->GetItem().GetTYPE();

		/// @brief 보석 아이템이라면..
		if( iItemType == ITEM_TYPE_GEM )
		{
			int iEquipSlot = pItemDlg->GetEquipSlot( ptMouse );
			if( iEquipSlot < 0 )
				return false;

			tagITEM& Item = g_pAVATAR->m_Inventory.m_ItemEQUIP[ iEquipSlot ];			
			/// 소켓이 있다면..
			if( Item.HasSocket() )
			{
				g_pNet->Send_cli_CRAFT_GEMMING_REQ( iEquipSlot, pItemIcon->GetIndex() );
			}

			return true;
		}

		/// 사용아이템은 드래그로 기본 동작( 장착 )을 처리하지 않는다.
		if( pItemIcon->GetItem().GetTYPE() != ITEM_TYPE_USE )
			pItemIcon->ExecuteCommand();		
	}
	else if( CSlot* pTargetSlot = pItemDlg->GetInvenSlot( ptMouse) )
	{
		if( CIcon* pTargetIcon = pTargetSlot->GetIcon() )
		{
			CItem* pTempItem = ((CIconItem*)pTargetIcon)->GetCItem();
			assert( pTempItem );
			pTargetSlot->DetachIcon();

			CSlot* pSourceSlot = pItemIcon->GetSlot();
			assert( pSourceSlot );
			CItem* pSourceItem = pItemIcon->GetCItem();
			assert( pSourceItem );
			pSourceSlot->DetachIcon();

			pTargetSlot->AttachIcon( pSourceItem->CreateItemIcon() );
			pSourceSlot->AttachIcon( pTempItem->CreateItemIcon() );
		}
		else
		{
			CSlot* pSourceSlot = pItemIcon->GetSlot();
			CItem* pSourceItem = pItemIcon->GetCItem();
			pSourceSlot->DetachIcon();
			pTargetSlot->AttachIcon( pSourceItem->CreateItemIcon() );
		}
	}

	return true;
}

bool CTCmdDragItem2PrivateStoreDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL || strcmp( pObj->toString(), "CIcon" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type" );
		return true;
	}

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_PRIVATESTORE );
	if( pDlg )
	{
		CPrivateStoreDlg* pPrivateStoreDlg = (CPrivateStoreDlg*)pDlg;
		CIconItem* pItemIcon = (CIconItem*)pObj;

		switch( pPrivateStoreDlg->GetTabType()  )
		{
		case CPrivateStoreDlg::TAB_SELL:///판매목록에 추가
			{
				pDlg = g_itMGR.FindDlg( DLG_TYPE_GOODS );

				if( pDlg )
				{

					if( pItemIcon->GetItem().IsEnableSELL() )
					{
						CGoodsDlg* pGoodsDlg = (CGoodsDlg*)pDlg;
						pGoodsDlg->SetIcon( pItemIcon );
						pGoodsDlg->SetType( CGoodsDlg::ADD_SELLLIST );
						g_itMGR.OpenDialog( DLG_TYPE_GOODS );
					}
					else
					{
						g_itMGR.AppendChatMsg(STR_DONT_SELL_ITEM, IT_MGR::CHAT_TYPE_SYSTEM );
					}
				}
				break;
			}
		case CPrivateStoreDlg::TAB_BUY :///구입목록에 추가
			{
				CPrivateStore::GetInstance().AddItemWishList( pItemIcon->GetItem(), true );
				break;
			}
		default:
			break;
		}
	}

	return true;
}
bool CTCmdDragItem2WishList::Exec( CTObject* pObj )
{
	if( pObj == NULL || strcmp( pObj->toString(), "CIcon" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type" );
		return true;
	}

	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_PRIVATESTORE );
	if( pDlg )
	{
		CPrivateStoreDlg* pPrivateStoreDlg = (CPrivateStoreDlg*)pDlg;
		CIconItem* pItemIcon = (CIconItem*)pObj;

		CPrivateStore::GetInstance().AddItemWishList( pItemIcon->GetItem(), true );
	}
	return true;
}


bool CTCmdDragSellItemFromPrivateStoreDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL || strcmp( pObj->toString(), "CIcon" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type" );
		return true;
	}
	CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_GOODS );

	if( pDlg )
	{
		CIconItem* pItemIcon = (CIconItem*)pObj;
		CPrivateStore::GetInstance().RemoveItemSellList( pItemIcon->GetIndex() );
	}
	return true;
}

bool CTCmdDragBuyItemFromPrivateStoreDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL || strcmp( pObj->toString(), "CIcon" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type" );
		return true;
	}
	CIconItem* pItemIcon = (CIconItem*)pObj;
	CPrivateStore::GetInstance().RemoveItemWishList( pItemIcon->GetIndex() );
	return true;
}

#include "../Dlgs/CAvatarStoreDlg.h"
bool CTCmdDragSellItemFromAvatarStoreDlg::Exec( CTObject* pObj )
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

		tagPS_SLOT_ITEM Item;
		Item.m_btSLOT = pItemIcon->GetIndex();
		Item.m_SlotITEM = pItemIcon->GetItem();

		g_pNet->Send_cli_P_STORE_BUY_REQ( pStoreDlg->GetMasterSvrObjIdx(), 1, &Item );
	}
	return true;
}


bool CTCmdDragItem2SeparateDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL || strcmp( pObj->toString(), "CIcon" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type" );
		return true;
	}

	CIconItem* pItemIcon = (CIconItem*)pObj;

	CSeparate::GetInstance().SetItem( pItemIcon->GetCItem() );
	return true;
}

#include "../../GameData/CUpgrade.h"
bool CTCmdDragTakeoutTargetItemFromUpgradeDlg::Exec( CTObject* pObj )
{
	assert( pObj );

	if( pObj == NULL || strcmp( pObj->toString(), "CIcon" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type" );
		return true;
	}

	CIconItem* pItemIcon = (CIconItem*)pObj;

	CUpgrade::GetInstance().RemoveTargetItem();
	return true;
}

bool CTCmdDragTakeoutMaterialItemFromUpgradeDlg::Exec( CTObject* pObj )
{
	assert( pObj );
	if( pObj == NULL || strcmp( pObj->toString(), "CIcon" ) )
	{
		assert( 0 && "pObj is NULL or Invalid Type" );
		return true;
	}

	CIconItem* pItemIcon = (CIconItem*)pObj;
	CUpgrade::GetInstance().RemoveMaterialItem( pItemIcon->GetCItem() );
	return true;
}

bool CTCmdDragTakeinItem2UpgradeDlg::Exec( CTObject* pObj )
{
	if( pObj == NULL )
		return true;

	if( strcmp( pObj->toString(), "CIcon" ) )
		return true;

	CIconItem* pItemIcon = (CIconItem*)pObj;

	CUpgrade::GetInstance().SetItem( pItemIcon->GetCItem() );
	return true;
}

