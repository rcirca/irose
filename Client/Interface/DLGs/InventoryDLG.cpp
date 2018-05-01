
#include "StdAfx.h"
#include "InventoryDLG.h"
#include "DealDlg.h"
#include "MakeDlg.h"
#include "ChattingDlg.h"
#include "JCommandState.h"

#include "../it_mgr.h"
#include "../CDragItem.h"
#include "../TypeResource.h"
//#include "../LookUpImageIndex.h"
#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"
#include "../InterfaceType.h"
#include "../IO_ImageRes.h"

#include "../Command/CTCmdNumberInput.h"
#include "../Command/CTCmdOpenDlg.h"
#include "../Command/CTCmdHotExec.h"
#include "../Command/UICommand.h"
#include "../Command/DragCommand.h"


#include "../../Common/CInventory.h"
#include "../../Common/IO_STB.h"
#include "../../Network/CNetwork.h"
#include "../../System/CGame.h"
#include "../../CClientStorage.h"




#include "../../../TGameCtrl/ResourceMgr.h"
#include "../../GameCommon/Skill.h"
#include "../../GameData/CExchange.h"
#include "../../GameData/Event/CTEventItem.h"


#define ITEM_START_POS_X		8
#define ITEM_START_POS_Y		66	
#define ITEM_WIDTH				33
#define ITEM_HEIGHT				33

#define WEIGHT_INFO_X			80
#define WEIGHT_INFO_Y			172
#define WEIGHT_INFO_WIDTH		82
#define WEIGHT_INFO_HEIGHT		16
#define GOLD_INFO_X				222
#define GOLD_INFO_Y				172
#define GOLD_INFO_WIDTH			82
#define GOLD_INFO_HEIGHT		16

#define INDEX_MONEY				0

const	int	MAX_DROP_MONEY	= 100000;///돈을 바닥에 버릴경우 최대 10만까지
///const	int	MAX_DROP_MONEY	= 100000000;///돈을 바닥에 버릴경우 최대 10만까지
CInventoryDLG::CInventoryDLG( int iType )
{
	m_nInvType   = 0;
	POINT ptOffset;

	CTCommand* pCmd = NULL;
	m_pDragItem = new CDragItem;

	///퀵바로의 이동
	pCmd = new CTCmdDragInven2QuickBar;
	m_pDragItem->AddTarget( DLG_TYPE_QUICKBAR, pCmd );

	///매매창으로의 이동
	CTCmdNumberInput* pNumberCmd = new CTCmdAddItem2DealFromInventory;
	CTCmdOpenNumberInputDlg* pOpenCmd = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pDragItem->AddTarget( DLG_TYPE_DEAL, pOpenCmd );

	///교환창으로 이동
	pNumberCmd  = new CTCmdAddMyItem2Exchange;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pDragItem->AddTarget( DLG_TYPE_EXCHANGE, pOpenCmd );


	///상점창으로 이동
	pNumberCmd = new CTCmdSellItem;
	pOpenCmd   = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pDragItem->AddTarget( DLG_TYPE_STORE, pOpenCmd );

	///땅으로 드랍
	pNumberCmd	= new CTCmdDropItem;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pDragItem->AddTarget( CDragItem::TARGET_GROUND, pOpenCmd );

	///Bank로 이동
	pNumberCmd	= new CTCmdMoveItemInv2Bank;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pDragItem->AddTarget( DLG_TYPE_BANK, pOpenCmd );

	///제조창으로 이동
	pCmd = new CTCmdTakeInItem2MakeDlg;
	m_pDragItem->AddTarget( DLG_TYPE_MAKE, pCmd );

	///캐릭터 창으로의 이동
	pCmd = new CTCmdDragItemEquipFromInven;
	m_pDragItem->AddTarget( DLG_TYPE_CHAR, pCmd );

	for( int i = 0; i < MAX_INV_TYPE; ++i )
	{
		for( int j = 0; j < INVENTORY_PAGE_SIZE; ++j )
		{
			ptOffset.x = ITEM_START_POS_X + ( (j%10) * ITEM_WIDTH );
			ptOffset.y = ITEM_START_POS_Y + ( (j/10) * ITEM_HEIGHT );
			m_ItemSlots[i][j].SetOffset( ptOffset );
			m_ItemSlots[i][j].SetParent( iType );
			m_ItemSlots[i][j].SetDragAvailable();
			m_ItemSlots[i][j].SetDragItem( m_pDragItem );
		}
	}
	SetDialogType( iType );

	///돈Drop관련 command
	m_pCmdDropMoney				= new CTCmdDropMoney;
	m_pCmdAddMyMoney2Exchange	= new CTCmdAddMyMoney2Exchange;
}

CInventoryDLG::~CInventoryDLG()
{
	if( m_pDragItem )
	{
		delete m_pDragItem;
		m_pDragItem = NULL;
	}

}

void CInventoryDLG::Draw()
{	
	if(!IsVision()) return ;
	CTDialog::Draw();

	DrawWeightANDGold();

	for( int i = 0; i < INVENTORY_PAGE_SIZE; ++i )
		m_ItemSlots[m_nInvType][i].Draw();
}

/// 무게와, 금 정보를 출력한다.
void CInventoryDLG::DrawWeightANDGold()
{
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	/// 무게 표시..
	RECT rt = { WEIGHT_INFO_X , 
				WEIGHT_INFO_Y , 
				WEIGHT_INFO_X + WEIGHT_INFO_WIDTH, 
				WEIGHT_INFO_Y + WEIGHT_INFO_HEIGHT }; 

	::drawFontf(g_GameDATA.m_hFONT[ FONT_NORMAL ],true, &rt,D3DCOLOR_XRGB( 0, 210, 255 ), DT_CENTER|DT_VCENTER, "%d/%d", g_pAVATAR->GetCur_WEIGHT(), g_pAVATAR->GetCur_MaxWEIGHT() );
	
	//돈표시 
	SetRect( &rt, GOLD_INFO_X , 
				  GOLD_INFO_Y , 
				  GOLD_INFO_X + GOLD_INFO_WIDTH , 
				  GOLD_INFO_Y + GOLD_INFO_HEIGHT );

	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt,D3DCOLOR_XRGB( 255, 240, 0 ), DT_RIGHT|DT_VCENTER, "%d", g_pAVATAR->Get_MONEY() );
}

unsigned int CInventoryDLG::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		///Slots처리
		for( int i = 0; i < INVENTORY_PAGE_SIZE; ++i )
		{
			if( m_ItemSlots[m_nInvType][i].Process( uiMsg, wParam, lParam ) )
				return uiMsg;
		}

		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			On_LButtonUP( iProcID, wParam, lParam); 
			break;
		default:
			break;
		}

		return uiMsg;
	}
	return 0;
}


// *------------------------------------------------------------------------* //
bool CInventoryDLG::On_LButtonUP( unsigned iProcID,WPARAM wParam, LPARAM lParam )
{
	switch(iProcID)
	{
	case INV_BTN_CLOSE:		
		Hide();
		return true;
	case INV_BTN_TAB1:
		m_nInvType = 0;
		UpdateSlotPosition( m_nInvType );
		return true;
	case INV_BTN_TAB2:
		m_nInvType = 1;
		UpdateSlotPosition( m_nInvType );
		return true;
	case INV_BTN_TAB3:
		m_nInvType = 2;
		UpdateSlotPosition( m_nInvType );
		return true;
	/// 메세지창 띄우고 확인에 숫자입력창 띄운다.
	case INV_BTN_MONEY:
		{
			int iMaxDropMoney = 0;		

			if( g_itMGR.IsDlgOpened( DLG_TYPE_EXCHANGE ) )
			{
				CTCmdOpenNumberInputDlg OpenCmd;
				OpenCmd.SetCommand( m_pCmdAddMyMoney2Exchange );
				OpenCmd.SetMaximum( g_pAVATAR->Get_MONEY() - CExchange::GetInstance().GetMyTradeMoney() );
				OpenCmd.Exec( NULL );

			}
			else
			{
				CTCmdOpenNumberInputDlg OpenCmd;
				OpenCmd.SetCommand( m_pCmdDropMoney );

				__int64 i64MaxDropMoney;
				if( MAX_DROP_MONEY >= g_pAVATAR->Get_MONEY() )
					i64MaxDropMoney = g_pAVATAR->Get_MONEY();
				else
					i64MaxDropMoney = MAX_DROP_MONEY;

				OpenCmd.SetMaximum( i64MaxDropMoney );
				OpenCmd.Exec( NULL );
			}
			return true;
		}
	}

	return false;
}



// *------------------------------------------------------------------------* //
// Check function

/// 아이템 사용요구를 만족시키는가?
/*
bool CInventoryDLG::CheckBUseItem( short nSelectNo )
{
	return true;
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// 2003 / 12 /24 / Navy
/// 판매목록에 등록된 아이템은 사용및 장착을 막는다.
/// 아이템 사용..
/*
bool CInventoryDLG::UseItem( short nRealIndex )
{
	tagITEM sITEM;
	sITEM = m_pInventory->m_ItemLIST[ nRealIndex ];

	if( sITEM.GetTYPE() == 0 )
		return false;

	short nItemType = ( nRealIndex - MAX_EQUIP_IDX ) / INVENTORY_PAGE_SIZE; 

	CTDialog* pDlg  = g_itMGR.FindDlg( DLG_TYPE_DEAL );
	CTDialog* pDlg2 = g_itMGR.FindDlg( DLG_TYPE_EXCHANGE );

	if( g_pAVATAR->Get_HP() <= 0 )
	{
		g_itMGR.AppendChatMsg(STR_CANT_USEITEM_REASON_SYNCOPE,g_dwYELLOW, CChatDLG::CHAT_TYPE_SYSTEM);

		return false;
	}
	switch( nItemType )
	{
		case 0://장비탭
			{
				if( g_itMGR.IsQualityItemInTradeList( nRealIndex ) )
					break;
				//아이템을 캐릭터창으로 ...
				//현재 선택아이템을 등록하고 등록된 아이템을 가져온다 ....
				/// drag entity setting
				tagDragItem dragEntity;
				dragEntity.m_DragItem = sITEM;
				dragEntity.m_iDragItemType = DRAGITEM_ITEM;

				g_DragNDrop.SetMember( dragEntity, GetDialogType(), nRealIndex/* + MAX_EQUIP_IDX*/ ///);
/*				g_DragNDrop.CharDLG_DragProc( false );
				break;
			}
			break;
		case 1://소모탭
			{				
				/// 바로 소모한다..물론 테스트후..
				if( CheckBUseItem( nRealIndex ) )
				{
					if( g_itMGR.IsQuantityItemInTradeList( nRealIndex ) )
						break;
					
					switch( ITEM_TYPE( sITEM.GetTYPE(), sITEM.GetItemNO() ) )
					{
						/// 마법책
						case 314:
							{
								/// 2004 / 2 / 11 : nAvy - 사용아이템이라는 조건아래
								///BYTE btT = g_pAVATAR->Skill_LearnCondition( USEITEM_NEED_DATA_TYPE( sITEM.GetItemNO()) );						
								
								///
								/// @todo 스킬을 배울수 없습니다. 관련 처리...
								///
							}
							break;

						/// 스크롤 마법
						case 313:
							{	
								int iSkillIDX		= USEITEM_SCROLL_USE_SKILL( sITEM.GetItemNO() );
								int iCurrentTarget	= g_CommandState.GetCurrentTarget();				
						
								if( CSkillManager::CheckConditionForFireSkill( iSkillIDX, iCurrentTarget ) == false )
								{
									g_itMGR.AppendChatMsg( STR_NOT_ENOUGH_CONDITION, g_dwRED );
									return false;
								}
							}
							break;							
					}

					//사용?
					if ( g_pNet->Send_cli_USE_ITEM( nRealIndex ) ) 
						g_pSoundLIST->IDX_PlaySound( USEITEM_USE_SOUND( m_pInventory->m_ItemLIST[ nRealIndex ].m_nItemNo_1 )  );
				}
				break;
			}
		case 2://기타탭 
			{
				///거래나 교환시 다른곳에서 사용되는 아이템일경우 동작되면 안된다.
				if( g_itMGR.IsQuantityItemInTradeList( nRealIndex ) )
					break;


				if( sITEM.GetTYPE() == ITEM_TYPE_NATURAL  )
				{
					CMakeDLG* pDlg = (CMakeDLG*)g_itMGR.FindDlg( DLG_TYPE_MAKE );
					if( pDlg && pDlg->IsVision() )///제조창이 열려 있는동안에는 소모탄 장착불가
					{
						CTCmdTakeInItem2MakeDlg* pCmd = new CTCmdTakeInItem2MakeDlg( nRealIndex );
						/*
						CTCmdAddItemToMakeDlg* pCmd = new CTCmdAddItemToMakeDlg( sITEM, nRealIndex );
						int iCount = pDlg->GetItemQuantityInMaterialList( nRealIndex );
						int iMaxCount = sITEM.GetQuantity() - iCount;
						if( iMaxCount > 1 )
						{
							g_itMGR.ShowNumberInputDlg( DLG_TYPE_MAKE, pCmd, DLG_TYPE_MAKE , iMaxCount );						
						}
						else if( iMaxCount == 1 )
						{
							pCmd->SetNumber(1);
							
						}
						*/
/*						g_itMGR.AddTCommand( DLG_TYPE_MAKE, pCmd );
					}
					else if( NATURAL_BULLET_NO(sITEM.GetItemNO() ) )
					{
						CTCmdEquipBullets* pCmd = new CTCmdEquipBullets( sITEM, nRealIndex );
						g_itMGR.AddTCommand( 0, pCmd );
					}
				}
			}
			break;
	}

	return true;
}

///Profile에 저장된 가상인벤토리가 있다면 그것을 가져와서 Setting한다.
void CInventoryDLG::CreateVirtualInventory()
{
	POINT			ptDraw;
	///일단 실제 인벤토리로부터 가상인벤토리를 만든다.
	for( int iType = 0; iType < MAX_INV_TYPE; ++iType )
	{
		for( int iSlotIdx = 0; iSlotIdx < INVENTORY_PAGE_SIZE; ++iSlotIdx )
		{
			tagITEM& refItem = m_pInventory->m_ItemPAGE[iType][iSlotIdx];
			CIconItem& refIcon = m_VirtualInventory[iType][iSlotIdx];
			if( !refItem.IsEmpty() )
			{
				refIcon.SetInvenIdx( MAX_EQUIP_IDX  + ( iType * INVENTORY_PAGE_SIZE ) + iSlotIdx );
				refIcon.SetGraphicID( ITEM_ICON_NO( refItem.GetTYPE(), refItem.GetItemNO() ));
			}
			refIcon.SetGraphicModuleID( IMAGE_RES_ITEM );
			ptDraw.x = m_sPosition.x + ITEM_START_POS_X + ( (iSlotIdx%10) * ITEM_WIDTH );
			ptDraw.y = m_sPosition.y + ITEM_START_POS_Y + ( (iSlotIdx/10) * ITEM_HEIGHT );
			refIcon.SetPosition( ptDraw );
			refIcon.SetParent( this );
		}
	}
	
	///저장되어 있던 가상 인벤토리테이블과 비교해서 재구성한다.
	short nRealInvenIdx = 0;
	short nSlotIdx      = 0;
	for( int iType = 0; iType < MAX_INV_TYPE; ++iType )
	{
		for( int iSlotIdx = 0; iSlotIdx < INVENTORY_PAGE_SIZE; ++iSlotIdx )
		{
			nRealInvenIdx   = g_ClientStorage.GetInvenIdxFromSavedVirtualInven( iType,iSlotIdx );
			CIconItem& refIcon = m_VirtualInventory[iType][iSlotIdx];
			if( g_nDefaultInventoryIdx != nRealInvenIdx && refIcon.GetInvenIdx() != nRealInvenIdx )
			{
				nSlotIdx = GetVirtualSlotIdxByRealInvenIdxNType( iType, nRealInvenIdx );
				if( nSlotIdx >= 0 && nSlotIdx != iSlotIdx )
					MoveIcon( iSlotIdx, nSlotIdx ,iType );
			}
		}
	}
}
*/
void CInventoryDLG::MoveIcon( int iVirtualInvenIdx1 , int iVirtualInvenIdx2 , int iInvenType )
{
	/*	CIconItem tempItem = m_VirtualInventory[ iInvenType ][ iVirtualInvenIdx1 ];
	m_VirtualInventory[ iInvenType ][ iVirtualInvenIdx1 ] = m_VirtualInventory[ iInvenType ][ iVirtualInvenIdx2 ];
	m_VirtualInventory[ iInvenType ][ iVirtualInvenIdx2 ] = tempItem;

	POINT ptDraw;

	ptDraw.x = m_sPosition.x + ITEM_START_POS_X + ( (iVirtualInvenIdx1%10) * ITEM_WIDTH );
	ptDraw.y = m_sPosition.y + ITEM_START_POS_Y + ( (iVirtualInvenIdx1/10) * ITEM_HEIGHT );
	m_VirtualInventory[ iInvenType ][ iVirtualInvenIdx1 ].SetPosition( ptDraw );

	ptDraw.x = m_sPosition.x + ITEM_START_POS_X + ( (iVirtualInvenIdx2%10) * ITEM_WIDTH );
	ptDraw.y = m_sPosition.y + ITEM_START_POS_Y + ( (iVirtualInvenIdx2/10) * ITEM_HEIGHT );
	m_VirtualInventory[ iInvenType ][ iVirtualInvenIdx2 ].SetPosition( ptDraw );
*/

}

void CInventoryDLG::MoveIcon( int iInvenIdx )
{
	/*
	POINT ptSlotIdx = GetVirtualSlotIdxByRealInvenIdx( iInvenIdx );

	int iSrcSlotIdx = (int)ptSlotIdx.y;
	if( iSrcSlotIdx >= 0 )
	{
		POINT pt;
		CGame::GetInstance().Get_MousePos( pt );
		int iDestSlotIdx = GetVirtualSlotIdxByPoint( pt );
		if( iDestSlotIdx >= 0 )
		{
			POINT ptDraw;
			if( m_VirtualInventory[m_nInvType][iDestSlotIdx].IsEmpty() )
			{
				m_VirtualInventory[m_nInvType][iDestSlotIdx] = m_VirtualInventory[m_nInvType][iSrcSlotIdx];

				ptDraw.x = m_sPosition.x + ITEM_START_POS_X + ( (iDestSlotIdx%10) * ITEM_WIDTH );
				ptDraw.y = m_sPosition.y + ITEM_START_POS_Y + ( (iDestSlotIdx/10) * ITEM_HEIGHT );

				m_VirtualInventory[m_nInvType][iDestSlotIdx].SetPosition( ptDraw );
				m_VirtualInventory[m_nInvType][iSrcSlotIdx].Clear();
			}
			else
			{
				CIconInvenItem Icon = m_VirtualInventory[m_nInvType][iDestSlotIdx];
				
				m_VirtualInventory[m_nInvType][iDestSlotIdx] = m_VirtualInventory[m_nInvType][iSrcSlotIdx];
				ptDraw.x = m_sPosition.x + ITEM_START_POS_X + ( (iDestSlotIdx%10) * ITEM_WIDTH );
				ptDraw.y = m_sPosition.y + ITEM_START_POS_Y + ( (iDestSlotIdx/10) * ITEM_HEIGHT );
				m_VirtualInventory[m_nInvType][iDestSlotIdx].SetPosition( ptDraw );


				m_VirtualInventory[m_nInvType][iSrcSlotIdx] = Icon;
				ptDraw.x = m_sPosition.x + ITEM_START_POS_X + ( (iSrcSlotIdx%10) * ITEM_WIDTH );
				ptDraw.y = m_sPosition.y + ITEM_START_POS_Y + ( (iSrcSlotIdx/10) * ITEM_HEIGHT );
				m_VirtualInventory[m_nInvType][iSrcSlotIdx].SetPosition( ptDraw );
			}
		}
	}*/
}

/*
short   CInventoryDLG::GetVirtualSlotIdxByRealInvenIdxNType( int iType, int iInvenIdx )
{
	for( int j = 0; j < INVENTORY_PAGE_SIZE; ++j )
	{
		if( m_VirtualInventory[iType][j].GetInvenIdx() == iInvenIdx )
			return j;

	}
	return -1;
}
POINT	CInventoryDLG::GetVirtualSlotIdxByRealInvenIdx(int iInvenIdx)
{
	POINT ptRet = {-1,-1};
	for( int i = 0; i < MAX_INV_TYPE; ++i )
	{
		for( int j = 0; j < INVENTORY_PAGE_SIZE; ++j )
		{
			if( m_VirtualInventory[i][j].GetInvenIdx() == iInvenIdx )
			{
				ptRet.x = i;
				ptRet.y = j;
			}
		}
	}
	return ptRet;
}

int CInventoryDLG::GetVirtualSlotIdxByPoint( POINT pt )
{
	for( int i = 0; i < INVENTORY_PAGE_SIZE; ++i )
	{
		if( m_VirtualInventory[m_nInvType][i].IsInside( pt ))
			return i;
	}
	return -1;	
}
*/
void CInventoryDLG::UpdateSlotPosition( int iType )
{
	for( int j = 0; j < INVENTORY_PAGE_SIZE; ++j )
		m_ItemSlots[iType][j].MoveWindow( m_sPosition );
}

void CInventoryDLG::MoveWindow( POINT ptMouse )
{
	CTDialog::MoveWindow( ptMouse );
	UpdateSlotPosition( m_nInvType );
}

///*
//void CInventoryDLG::UpdateInventory()
//{
//	if( m_pInventory == NULL )
//		return;
//
//	int iTypeIndex			= 0;
//	int iRealInvenIndex     = 0;
//	int iRealSlotIndex		= 0;
//	int iVirtualSlotIndex	= 0;
//	int iEmptySlotIdx		= 0;
//	tagITEM Item;
//	CIconInvenItem InvenIcon;
//	POINT	ptDraw;
///*
//	///실제 인벤토리있는 아이템중 가상 인벤토리에 없는 아이템을 추가한다.
//	for( iTypeIndex = 0; iTypeIndex < MAX_INV_TYPE; ++iTypeIndex )
//	{
//		for( iRealSlotIndex = 0; iRealSlotIndex < INVENTORY_PAGE_SIZE; ++iRealSlotIndex )
//		{
//			iRealInvenIndex   = MAX_EQUIP_IDX + ( iTypeIndex * INVENTORY_PAGE_SIZE ) + iRealSlotIndex ;
//
//			Item = m_pInventory->m_ItemPAGE[ iTypeIndex ][ iRealSlotIndex ];
//
//			if( !Item.IsEmpty() )
//			{
//				for( iVirtualSlotIndex = 0; iVirtualSlotIndex < INVENTORY_PAGE_SIZE; ++iVirtualSlotIndex )
//				{
//					InvenIcon = m_VirtualInventory[ iTypeIndex ][ iVirtualSlotIndex ];
//					if( !InvenIcon.IsEmpty() )
//					{
//						if( InvenIcon.GetInvenIdx() == iRealInvenIndex )
//							break;
//					}
//				}
//
//				if( iVirtualSlotIndex == INVENTORY_PAGE_SIZE )///찾지 못한경우
//				{
//					iEmptySlotIdx = GetVirtualEmptySlot( iTypeIndex );
//					if( iEmptySlotIdx >= 0 && iEmptySlotIdx < INVENTORY_PAGE_SIZE )
//					{
//						m_VirtualInventory[iTypeIndex][iEmptySlotIdx].SetInvenIdx( iRealInvenIndex );
//						ptDraw.x = m_sPosition.x + ITEM_START_POS_X + ( (iEmptySlotIdx%10) * ITEM_WIDTH );
//						ptDraw.y = m_sPosition.y + ITEM_START_POS_Y + ( (iEmptySlotIdx/10) * ITEM_HEIGHT );
//						m_VirtualInventory[iTypeIndex][iEmptySlotIdx].SetPosition( ptDraw );
//					}
//					else
//					{
//						LogString( LOG_NORMAL, "Error Add Item to Virtual Inventory: Not Found Empty Slot" );
//					}
//				}
//			}
//		}
//	}
//	
//	/// 가상 인벤토리에서 실제 인벤토리에 없는 아이템을 뺀다.
//	/// Update시에 실제 인벤토리에 없다면 Clear가 되지만 한번더 빼준다.
//	for( iTypeIndex = 0; iTypeIndex < MAX_INV_TYPE; ++iTypeIndex )
//	{
//		for( iVirtualSlotIndex = 0; iVirtualSlotIndex < INVENTORY_PAGE_SIZE; ++iVirtualSlotIndex )
//		{
//			InvenIcon = m_VirtualInventory[ iTypeIndex ][ iVirtualSlotIndex ];
//			if( !InvenIcon.IsEmpty() )
//			{
//				iRealInvenIndex = InvenIcon.GetInvenIdx();
//				if( m_pInventory->m_ItemLIST[ iRealInvenIndex ].IsEmpty() )
//					m_VirtualInventory[ iTypeIndex ][ iVirtualSlotIndex ].Clear();
//			}
//		}
//	}
//*/
/////}
/*
//int CInventoryDLG::GetVirtualEmptySlot( int iInvenType )
//{
//
//	for( int j = 0; j < INVENTORY_PAGE_SIZE; ++j )
//	{
//		if( m_VirtualInventory[iInvenType][j].IsEmpty() )
//			return j;
//	}
//	return -1;
//}
//
//void CInventoryDLG::SaveVirtualInventoryTable()
//{
//	g_ClientStorage.ClearVirtualInvenTable();
//
//	for( int i = 0; i < MAX_INV_TYPE; ++i )
//	{
//		for( int j = 0; j < INVENTORY_PAGE_SIZE; ++j )
//		{
//			CIconItem& refIcon = m_VirtualInventory[i][j];
//			if( refIcon.GetInvenIdx() >= 0 )
//			{
//				g_ClientStorage.SetVirutalInven( i, j , refIcon.GetInvenIdx() );
//			}
//		}
//	}
//}
//*/
void CInventoryDLG::Hide()
{
	CTDialog::Hide();
//	SaveVirtualInventoryTable();
}

int	CInventoryDLG::FindItem( int iItemType , int iItemNo )
{
///*	CIconItem* pItemIcon	= NULL;
//	tagITEM*   pItem		= NULL;
//	for( int i = 0; i < MAX_INV_TYPE; ++i )
//	{
//		for( int j = 0; j < INVENTORY_PAGE_SIZE; ++j )
//		{
//			pItemIcon = &(m_VirtualInventory[i][j]);
//			if( !pItemIcon->IsEmpty() )
//			{
//				pItem = &(g_pAVATAR->m_Inventory.m_ItemLIST[ pItemIcon->GetInvenIdx() ]);
//				if( !pItem->IsEmpty() )
//				{
//					if( iItemType == pItem->GetTYPE() && iItemNo == pItem->GetItemNO() )
//						return pItemIcon->GetInvenIdx();
//				}
//			}
//		}
//	}*/
	return -1;
}

int CInventoryDLG::FindItem( int iClass )
{
	///*
	//CIconItem* pItemIcon	= NULL;
	//tagITEM*   pItem		= NULL;
	//for( int i = 0; i < MAX_INV_TYPE; ++i )
	//{
	//	for( int j = 0; j < INVENTORY_PAGE_SIZE; ++j )
	//	{
	//		pItemIcon = &(m_VirtualInventory[i][j]);
	//		if( !pItemIcon->IsEmpty() )
	//		{
	//			pItem = &(g_pAVATAR->m_Inventory.m_ItemLIST[ pItemIcon->GetInvenIdx() ]);
	//			if( !pItem->IsEmpty() )
	//			{
	//				if( ITEM_TYPE( pItem->GetTYPE(), pItem->GetItemNO() ) == iClass )
	//					return pItemIcon->GetInvenIdx();
	//			}
	//		}
	//	}
	//}
	//*/
	return -1;
}
//
//void CInventoryDLG::Update( CObservable* pObservable, CTObject* pObj )
//{
//	assert( pObservable );
//	
//	if( !pObservable->HasChanged() )
//		return;
//	
//	if( pObj && strcmp(pObj->toString(), "CTEventItem") == 0)
//	{
//		CTEventItem* pEvent = (CTEventItem*)pObj;
//		int iIndex = pEvent->GetIndex();
//
//		if( iIndex >= INVENTORY_ITEM_INDEX_0 && iIndex < INVENTORY_ITEM_INDEX_L
//			AST )
//		{
//			switch( pEvent->GetID() )
//			{
//			case CTEventItem::EID_ADD_ITEM:
//				{
//					int iType = ( iIndex - INVENTORY_ITEM_INDEX_0 ) / INVENTORY_PAGE_SIZE;
//					int iSlot = ( iIndex - INVENTORY_ITEM_INDEX_0 ) % INVENTORY_PAGE_SIZE;
//					CItem* pItem = pEvent->GetItem();
//					assert( pItem && m_ItemSlots[iType][iSlot].GetIcon() == NULL );
//					if( pItem )
//						m_ItemSlots[iType][iSlot].AttachIcon( pItem->CreateItemIcon() );
//
//					break;
//				}
//			case CTEventItem::EID_DEL_ITEM:
//				{
//					int iType = ( iIndex - INVENTORY_ITEM_INDEX_0 ) / INVENTORY_PAGE_SIZE;
//					int iSlot = ( iIndex - INVENTORY_ITEM_INDEX_0 ) % INVENTORY_PAGE_SIZE;
//
//					assert( m_ItemSlots[iType][iSlot].GetIcon() );
//					m_ItemSlots[iType][iSlot].DetachIcon();
//					break;
//				}
//			default:
//				break;
//			}
//		}
//	}	
//	else
//	{
//		assert( 0 && "CTEvent is NULL or Invalid Type @CInventoryDlg::Update" );
//	}
//}
/// 선택된 아이템을 얻어온다. 실제 아이템..
/// IDX_GetITEM(, , ) 파라미터 세개짜리는 내부적으로 두개짜리 호출..실제 아이템을 얻어온다.
/*
short CInventoryDLG::GetMouse_PointITEM(tagITEM& sITEM, POINT ptPoint )
{
	CIconInvenItem* pIcon = NULL;
	
	for(short i=0;i<INVENTORY_PAGE_SIZE;i++) 
	{
		pIcon = &(m_VirtualInventory[ m_nInvType ][ i ]);
		if( pIcon->IsInside( ptPoint ) )
		{
			if( pIcon->IsEmpty() )
				return -1;

			sITEM = pIcon->GetItem();
			return pIcon->GetInvenIdx();
		}
	}
	return -1;
}
*/
void CInventoryDLG::Update( POINT pt )
{
	if( !IsVision() ) return;
	CTDialog::Update( pt );
	
	for( int iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE ; ++iSlot )
		m_ItemSlots[ m_nInvType ][iSlot].Update( pt );
	
}

void CInventoryDLG::Show()
{
	CTDialog::Show();
	UpdateSlotPosition( m_nInvType );
}