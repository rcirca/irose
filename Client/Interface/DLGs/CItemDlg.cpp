#include "stdafx.h"
#include ".\citemdlg.h"
#include "../../GameData/Event/CTEventItem.h"
#include "../../GameCommon/Item.h"
#include "../CDragItem.h"
#include "../it_mgr.h"
#include "../../Object.h"

#include "../Icon/CIconItem.h"
#include "../Command/CTCmdNumberInput.h"
#include "../Command/CTCmdOpenDlg.h"
#include "../Command/CTCmdHotExec.h"
#include "../Command/UICommand.h"
#include "../Command/DragCommand.h"
#include "../../GameData/CExchange.h"
#include "../../System/CGame.h"
#include "../../Network/CNetwork.h"
#include "../../misc/gameutil.h"
#include "../../Country.h"

#include "tgamectrl/actionevent.h"
#include "tgamectrl/tpane.h"
const	int	MAX_DROP_MONEY	= 100000;///돈을 바닥에 버릴경우 최대 10만까지

const POINT c_ptEquipedSlotOffsets[] = {	
	{  19, 67 },	///Face
	{  69, 67 },	///Helmet
	{  69,113 },	///Armor
	{ 119, 67 },	///Knapsack
	{  19,159 },	///Gauntlet
	{  69,159 },	///boots
	{  19,113 },	///Weapon_R
	{ 119,113 },	///Weapon_L
	{  69,205 },	///Necklace
	{  19,205 },	///Ring
	{ 119,205 }		///Earring
};

const POINT c_ptBulletEquipSlots[]=
{
	{ 169,67 },
	{ 169,113},
	{ 169,159}
};


///배틀 카트 시스템 적용으로 파츠가 1개 추가
#if defined(_GBC)
	const POINT c_ptPatEquipSlots[] = 
	{
		{ 19,  79},
		{ 19, 125},
		{ 19, 171},
		{119,  79},
		{119, 125}
	};
#else
	const POINT c_ptPatEquipSlots[] = 
	{
		{ 19,  68},
		{ 19, 114},
		{ 19, 160},
		{ 19, 206}
	};
#endif


CItemDlg::CItemDlg(int iType)
{
	SetDialogType( iType );
	CTCommand* pCmd = NULL;

	m_iEquipTab		= 0;
	m_iInventoryTab = 0;
	m_bMinimize		= false;
	m_pEquipDragItem = new CDragItem;
	pCmd = new CTCmdDragItemFromEquipInItemDlg;
	m_pEquipDragItem->AddTarget( iType, pCmd );
	m_pEquipDragItem->AddTarget( CDragItem::TARGET_ALL, NULL );///
	

	int iSlot = 0;
	for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
	{
		m_AvatarEquipSlots[iSlot].SetParent( iType );
		m_AvatarEquipSlots[iSlot].SetOffset( c_ptEquipedSlotOffsets[iSlot] );
		m_AvatarEquipSlots[iSlot].SetDragAvailable();
		m_AvatarEquipSlots[iSlot].SetDragItem( m_pEquipDragItem );
	}

	for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
	{
		m_BulletEquipSlots[iSlot].SetParent( iType );
		m_BulletEquipSlots[iSlot].SetOffset( c_ptBulletEquipSlots[iSlot] );
		m_BulletEquipSlots[iSlot].SetDragAvailable();
		m_BulletEquipSlots[iSlot].SetDragItem( m_pEquipDragItem );
	}


	for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
	{
		m_PatEquipSlots[iSlot].SetParent( iType );
		m_PatEquipSlots[iSlot].SetOffset( c_ptPatEquipSlots[iSlot] );
		m_PatEquipSlots[iSlot].SetDragAvailable();
		m_PatEquipSlots[iSlot].SetDragItem( m_pEquipDragItem );
	}


	m_pInvenDragItem = new CDragItem;

	pCmd = new CTCmdDragTakeinItem2UpgradeDlg;
	m_pInvenDragItem->AddTarget( DLG_TYPE_UPGRADE, pCmd );


	///분리/분해창으로 이동
	pCmd = new CTCmdDragItem2SeparateDlg;
	m_pInvenDragItem->AddTarget( DLG_TYPE_SEPARATE, pCmd );


	///퀵바로의 이동
	pCmd = new CTCmdDragInven2QuickBar;
	m_pInvenDragItem->AddTarget( DLG_TYPE_QUICKBAR, pCmd );
#ifdef _NEW_UI // 2nd SkillBar
	///EX퀵바로의 이동
	pCmd = new CTCmdDragInven2QuickBar( DLG_TYPE_QUICKBAR_EXT );
	m_pInvenDragItem->AddTarget( DLG_TYPE_QUICKBAR_EXT, pCmd );
#endif

	///매매창으로의 이동
	CTCmdNumberInput* pNumberCmd = new CTCmdAddItem2DealFromInventory;
	CTCmdOpenNumberInputDlg* pOpenCmd = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_DEAL, pOpenCmd );

	///교환창으로 이동
	pNumberCmd  = new CTCmdAddMyItem2Exchange;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_EXCHANGE, pOpenCmd );


	///상점창으로 이동
	pNumberCmd = new CTCmdSellItem;
	pOpenCmd   = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_STORE, pOpenCmd );

	///땅 혹은 채팅창으로 드랍
	///채팅창은 보이지는 않지만 일정영역을 가지고 있다.. 
	///채팅창에 걸쳐서 버려도 땅에 버리는 것을 간주한다
	pNumberCmd	= new CTCmdDropItem;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( CDragItem::TARGET_GROUND, pOpenCmd );



	pNumberCmd	= new CTCmdDropItem;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_CHAT, pOpenCmd );
	
	///Bank로 이동
	pNumberCmd	= new CTCmdMoveItemInv2Bank;
	pOpenCmd	= new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pInvenDragItem->AddTarget( DLG_TYPE_BANK, pOpenCmd );

	///제조창으로 이동
	pCmd = new CTCmdTakeInItem2MakeDlg;
	m_pInvenDragItem->AddTarget( DLG_TYPE_MAKE, pCmd );


	///자신이 개설한 개인상점창으로
	pCmd = new CTCmdDragItem2PrivateStoreDlg;
	m_pInvenDragItem->AddTarget( DLG_TYPE_PRIVATESTORE, pCmd );

	///다른 아바타의 개인상점을 이용시
	CTCmdDragItem2AvatarStoreDlg* pOpenNumberInputDlg = new CTCmdDragItem2AvatarStoreDlg;
	CTCmdNumberInput* pCmd2 = new CTCmdSellItem2AvatarStore;
	pOpenNumberInputDlg->SetCommand( pCmd2 );
	m_pInvenDragItem->AddTarget( DLG_TYPE_AVATARSTORE, pOpenNumberInputDlg );

	///장착슬롯으로 이동시에는 장착 혹은 재밍등....
	pCmd = new CTCmdDragItemFromInvenInItemDlg;
	m_pInvenDragItem->AddTarget( iType, pCmd );
	

	POINT ptOffset;
	for( int iItemType  = 0 ; iItemType < MAX_INV_TYPE; ++iItemType )
	{
		for( iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
			ptOffset.x = 13  +  (iSlot%5) * ( m_ItemSlots[iItemType][iSlot].GetWidth() + 1);
			ptOffset.y = 284 +  (iSlot/5) * ( m_ItemSlots[iItemType][iSlot].GetHeight() + 1);
			m_ItemSlots[iItemType][iSlot].SetParent( iType );
			m_ItemSlots[iItemType][iSlot].SetOffset( ptOffset );
			m_ItemSlots[iItemType][iSlot].SetDragAvailable();
			m_ItemSlots[iItemType][iSlot].SetDragItem( m_pInvenDragItem );
		}
	}


	m_pCmdDropMoney				= new CTCmdDropMoney;
	m_pCmdAddMyMoney2Exchange	= new CTCmdAddMyMoney2Exchange;
}

CItemDlg::~CItemDlg(void)
{
	SAFE_DELETE( m_pInvenDragItem );
	SAFE_DELETE( m_pEquipDragItem );
	SAFE_DELETE( m_pCmdDropMoney );
	SAFE_DELETE( m_pCmdAddMyMoney2Exchange );

}
void CItemDlg::Draw()
{
	if( !IsVision() ) return;

	CTDialog::Draw();

	int iSlot = 0;
	if( m_iEquipTab == 1 )
	{
		if( !m_bMinimize)
			for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
				m_PatEquipSlots[iSlot].Draw();

		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[MAX_INV_TYPE-1][iSlot].Draw();
	}
	else
	{
		if( !m_bMinimize)
		{
			for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
				m_AvatarEquipSlots[iSlot].Draw();
			
			for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
				m_BulletEquipSlots[iSlot].Draw();
		}


		if( m_iInventoryTab == 1 )
		{
			::endSprite();
			::beginSprite( D3DXSPRITE_ALPHABLEND ); 
		}


		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].Draw();
	}
	
	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x , (float)m_sPosition.y + m_iHeight - 23,0.0f);
	::setTransformSprite( mat );

	///돈
	const int money_buffer_size = 64;
	char money_buffer[ money_buffer_size ];
	CGameUtil::ConvertMoney2String( g_pAVATAR->Get_MONEY(), money_buffer, money_buffer_size );

	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ],true,  40,0,D3DCOLOR_XRGB( 255, 240, 0 ), "%s", money_buffer );
	///무게
	::drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ],true, 145,0,D3DCOLOR_XRGB( 0, 210, 255 ),  "%d/%d", g_pAVATAR->GetCur_WEIGHT(), g_pAVATAR->GetCur_MaxWEIGHT() );
}

void CItemDlg::OnLButtonUp( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_MAXIMIZE:
		Maximize();
		break;
	case IID_BTN_MINIMIZE:
		Minimize();
		break;
	case IID_BTN_ICONIZE:
		g_itMGR.AddDialogIcon( 	GetDialogType() );
		break;
	case IID_BTN_CLOSE:
		Hide();
		break;
	case IID_BTN_MONEY:
		{
			int iMaxDropMoney = 0;		

			if( g_itMGR.IsDlgOpened( DLG_TYPE_BANK ) )
			{
				//g_itMGR.OpenMsgBox( STR_CANT_STORE_MONEY2BANK );
			}
			else if( g_itMGR.IsDlgOpened( DLG_TYPE_EXCHANGE ) )
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

			break;
		}
	default:
		break;
	}
}
void CItemDlg::OnLButtonDown( unsigned uiProcID, WPARAM wParam, LPARAM lParam )
{
	switch( uiProcID )
	{
	case IID_BTN_EQUIP_PAT:
		{
			m_iEquipTab = 1;
		
			CWinCtrl* pCtrl;
			if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_PAT ) )
				pCtrl->Show();

			if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_ITEM ) )
				pCtrl->Hide();

			for( int iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
				m_PatEquipSlots[iSlot].MoveWindow( m_sPosition );

			for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
				m_ItemSlots[3][iSlot].MoveWindow( m_sPosition );

			break;
		}
	case IID_BTN_EQUIP_AVATAR:
		{
			m_iEquipTab = 0;
			CWinCtrl* pCtrl;
			if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_PAT ) )
				pCtrl->Hide();

			if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_ITEM ) )
				pCtrl->Show();

			int iSlot = 0;
			for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
				m_AvatarEquipSlots[iSlot].MoveWindow( m_sPosition );

			for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
				m_BulletEquipSlots[iSlot].MoveWindow( m_sPosition );

			for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
				m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );

			break;
		}
	case IID_BTN_INVEN_EQUIP:
		m_iInventoryTab = 0;
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );

		break;
	case IID_BTN_INVEN_USE:
		m_iInventoryTab = 1;
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );

		break;
	case IID_BTN_INVEN_ETC:
		m_iInventoryTab = 2;
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );

		break;

	default:
		break;
	}
}
unsigned CItemDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;

	if( unsigned iProcID = CTDialog::Process( uiMsg, wParam, lParam ) )
	{
		if( ProcessSlots( uiMsg, wParam, lParam ) )
			return uiMsg;

		switch( uiMsg )
		{
		case WM_LBUTTONDOWN:
			OnLButtonDown( iProcID, wParam , lParam );
			break;
		case WM_LBUTTONUP:
			OnLButtonUp( iProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}
void CItemDlg::Show()
{
	CTDialog::Show();

	CWinCtrl* pCtrl = NULL;

	if( m_iEquipTab == 0 )
	{
		if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_PAT ) )
			pCtrl->Hide();

		if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_ITEM ) )
			pCtrl->Show();
	}
	else
	{
		if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_PAT ) )
			pCtrl->Show();

		if( pCtrl = FindChildInPane( IID_PANE_INVEN, IID_TABBEDPANE_INVEN_ITEM ) )
			pCtrl->Hide();
	}


	if( m_bMinimize )
	{
		if( pCtrl = FindChildInPane( IID_PANE_EQUIP, IID_BTN_MINIMIZE ) )
			pCtrl->Hide();
	}
	else
	{
		if( pCtrl = FindChildInPane( IID_PANE_EQUIP, IID_BTN_MAXIMIZE ) )
			pCtrl->Hide();
	}

}

void CItemDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj == NULL || strcmp( pObj->toString(), "CTEventItem") )
	{
		assert( 0 && "CTEvent is NULL or Invalid" );	
		return;
	}

	CTEventItem* pEvent = (CTEventItem*)pObj;
	int iIndex = pEvent->GetIndex();
	switch( pEvent->GetID() )
	{
	case CTEventItem::EID_ADD_ITEM:
		{
			CItem* pItem = pEvent->GetItem();

			if( iIndex >= 1	&& iIndex < MAX_EQUIP_IDX )
				m_AvatarEquipSlots[ iIndex - 1].AttachIcon( pItem->CreateItemIcon() );
			else if(  iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )
				m_BulletEquipSlots[ iIndex - INVENTORY_SHOT_ITEM0 ].AttachIcon( pItem->CreateItemIcon() );
			else if( iIndex >= INVENTORY_RIDE_ITEM0 && iIndex < INVENTORY_RIDE_ITEM0 + MAX_RIDING_PART )
				m_PatEquipSlots[ iIndex - INVENTORY_RIDE_ITEM0 ].AttachIcon( pItem->CreateItemIcon() );
			else if( iIndex >= INVENTORY_ITEM_INDEX_0 && iIndex <= INVENTORY_ITEM_INDEX_LAST )
			{
				int iInvenSlotIndex = iIndex - INVENTORY_ITEM_INDEX_0;
				int iType = iInvenSlotIndex / INVENTORY_PAGE_SIZE;
				for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
				{
					if( m_ItemSlots[iType][iSlot].GetIcon() == NULL )
					{
						m_ItemSlots[iType][iSlot].AttachIcon( pItem->CreateItemIcon() );
						break;
					}
				}
			}
			break;
		}
	case CTEventItem::EID_DEL_ITEM:
		{
			if( iIndex >= 1	&& iIndex < MAX_EQUIP_IDX )
				m_AvatarEquipSlots[ iIndex - 1].DetachIcon();
			else if(  iIndex >= INVENTORY_SHOT_ITEM0 && iIndex < INVENTORY_SHOT_ITEM0 + MAX_SHOT_TYPE )
				m_BulletEquipSlots[ iIndex - INVENTORY_SHOT_ITEM0 ].DetachIcon();
			else if( iIndex >= INVENTORY_RIDE_ITEM0 && iIndex < INVENTORY_RIDE_ITEM0 + MAX_RIDING_PART )
				m_PatEquipSlots[ iIndex - INVENTORY_RIDE_ITEM0 ].DetachIcon();
			else if( iIndex >= INVENTORY_ITEM_INDEX_0 && iIndex <= INVENTORY_ITEM_INDEX_LAST )
			{
				int iInvenSlotIndex = iIndex - INVENTORY_ITEM_INDEX_0;
				int iType = iInvenSlotIndex / INVENTORY_PAGE_SIZE;
				CIcon* pIcon = NULL;
				for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
				{
					if( pIcon = m_ItemSlots[ iType ][ iSlot ].GetIcon() )
					{
						if( pIcon->GetIndex() == iIndex )
						{
							m_ItemSlots[ iType ][ iSlot ].DetachIcon();				
							break;
						}
					}
				}
			}
			break;
		}
	default:
		break;
	}

	
}
void CItemDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	int iSlot;


	if( m_iEquipTab == 1 )
	{
		for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
			m_PatEquipSlots[iSlot].MoveWindow( m_sPosition );

		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[MAX_INV_TYPE-1][iSlot].MoveWindow( m_sPosition );
	}
	else
	{
		for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
			m_AvatarEquipSlots[iSlot].MoveWindow( m_sPosition );

		for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
			m_BulletEquipSlots[iSlot].MoveWindow( m_sPosition );

		for( iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].MoveWindow( m_sPosition );
	}

}
void CItemDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );

	int iSlot = 0;
	if( m_iEquipTab == 1 )
	{
		if( !m_bMinimize )
			for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
				m_PatEquipSlots[iSlot].Update( ptMouse );

		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[MAX_INV_TYPE-1][iSlot].Update(ptMouse );
	}
	else
	{
		if( !m_bMinimize )
		{
			for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
				m_AvatarEquipSlots[iSlot].Update( ptMouse );
			
			for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
				m_BulletEquipSlots[iSlot].Update( ptMouse );
		}

		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[m_iInventoryTab][iSlot].Update( ptMouse );
	}

}

bool CItemDlg::ProcessSlots( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	int iSlot;
	if( m_iEquipTab == 1 )
	{
		if( !m_bMinimize )
			for( iSlot = 0 ; iSlot < MAX_RIDING_PART; ++iSlot )
				if( m_PatEquipSlots[iSlot].Process( uiMsg, wParam , lParam ) )
					return true;

		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			if( m_ItemSlots[MAX_INV_TYPE-1][iSlot].Process( uiMsg, wParam, lParam ) )
				return true;
	}
	else
	{
		if( !m_bMinimize )
		{
			for( iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
				if( m_AvatarEquipSlots[iSlot].Process( uiMsg, wParam, lParam) )
					return true;
			
			for( iSlot = 0 ; iSlot < MAX_SHOT_TYPE; ++iSlot )
				if( m_BulletEquipSlots[iSlot].Process( uiMsg, wParam, lParam) )
					return true;
		}
		
		for( iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			if( m_ItemSlots[m_iInventoryTab][iSlot].Process( uiMsg, wParam,lParam) )
				return true;
	}
	return false;
}


bool CItemDlg::IsInsideInven( POINT pt )
{
	RECT rc = { 10, 290, 220, 530 };
	POINT ptTemp = { pt.x - m_sPosition.x, pt.y - m_sPosition.y };

	return PtInRect( &rc, ptTemp )?true:false;
}

bool CItemDlg::IsInsideEquip( POINT pt )
{
	if( m_bMinimize )
		return false;

	RECT rc = { 10, 60, 220, 250 };
	POINT ptTemp = { pt.x - m_sPosition.x, pt.y - m_sPosition.y };
	return PtInRect( &rc, ptTemp )?true:false;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 현재마우스 위치로 장비 장착창 슬롯 번호를 구한다.
/// @return < 0 이라면 적당한 장비장착창 위가 아님..
//----------------------------------------------------------------------------------------------------

int CItemDlg::GetEquipSlot( POINT pt )
{
	if( m_bMinimize )
		return -1;

	RECT rt;
	POINT ptTemp = { pt.x - m_sPosition.x, pt.y - m_sPosition.y };

	for( int iSlot = 0 ; iSlot < MAX_EQUIP_IDX-1; ++iSlot )
	{
		SetRect( &rt, c_ptEquipedSlotOffsets[ iSlot ].x, c_ptEquipedSlotOffsets[ iSlot ].y,
						c_ptEquipedSlotOffsets[ iSlot ].x + 40, c_ptEquipedSlotOffsets[ iSlot ].y + 40 );

		if( PtInRect( &rt, ptTemp ) )
		{
			return iSlot + 1;
		}
	}

	return -1;
}

CSlot*	CItemDlg::GetInvenSlot( POINT pt )
{
	int iInvenType = 0;

	if( m_iEquipTab == 1 )
		iInvenType = MAX_INV_TYPE-1;
	else
		iInvenType = m_iInventoryTab;

	for( int iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		if( m_ItemSlots[iInvenType][iSlot].IsInside( pt.x, pt.y ) )
			return &(m_ItemSlots[iInvenType][iSlot]);
	
	return NULL;
}
CWinCtrl* CItemDlg::FindChildInPane( unsigned uiPaneID, unsigned uiChildID )
{
	CWinCtrl* pCtrl = Find( uiPaneID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_PANE )
	{
		CTPane* pPane = (CTPane*)pCtrl;
		return pPane->FindChild( uiChildID );
	}
	return NULL;
}

void CItemDlg::Minimize()
{
	CWinCtrl* pCtrl = NULL;
	m_bMinimize = true;

	if( pCtrl = FindChildInPane( IID_PANE_EQUIP, IID_BTN_MINIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = FindChildInPane( IID_PANE_EQUIP, IID_BTN_MAXIMIZE ) )
		pCtrl->Show();

	if( pCtrl = Find( IID_PANE_INVEN ) )
	{
		pCtrl->SetOffset( 0, 54 );
		SetHeight( pCtrl->GetHeight() + 54 );
	}

	POINT ptOffset;
	for( int iItemType  = 0 ; iItemType < MAX_INV_TYPE; ++iItemType )
	{
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
			ptOffset.x = 13  +  (iSlot%5) * ( m_ItemSlots[iItemType][iSlot].GetWidth() + 1);
			ptOffset.y = 84  +  (iSlot/5) * ( m_ItemSlots[iItemType][iSlot].GetHeight() + 1);
			m_ItemSlots[iItemType][iSlot].SetOffset( ptOffset );
		}
	}


	MoveWindow( m_sPosition );
}

void CItemDlg::Maximize()
{
	CWinCtrl* pCtrl = NULL;
	m_bMinimize = false;

	if( pCtrl = FindChildInPane( IID_PANE_EQUIP, IID_BTN_MINIMIZE ) )
		pCtrl->Show();

	if( pCtrl = FindChildInPane( IID_PANE_EQUIP, IID_BTN_MAXIMIZE ) )
		pCtrl->Hide();

	if( pCtrl = Find( IID_PANE_INVEN ) )
	{
		pCtrl->SetOffset( 0, 254 );
		SetHeight( pCtrl->GetHeight() + 254 );
	}


	POINT ptOffset;
	for( int iItemType  = 0 ; iItemType < MAX_INV_TYPE; ++iItemType )
	{
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
			ptOffset.x = 13  +  (iSlot%5) * ( m_ItemSlots[iItemType][iSlot].GetWidth() + 1);
			ptOffset.y = 284 +  (iSlot/5) * ( m_ItemSlots[iItemType][iSlot].GetHeight() + 1);

			m_ItemSlots[iItemType][iSlot].SetOffset( ptOffset );
		}
	}

	MoveWindow( m_sPosition );
}

void CItemDlg::SwitchIcon( int iReal, int iVirtual )
{
	if( iReal == 0 ) return;

	//int iRealType = iReal / INVENTORY_PAGE_SIZE;
	//int iRealSlot = iReal % INVENTORY_PAGE_SIZE;

	int iVirtualType = iVirtual / INVENTORY_PAGE_SIZE;
	int iVirtualSlot = iVirtual % INVENTORY_PAGE_SIZE;

	//assert( iRealType == iVirtualType );
	//if( iRealType != iVirtualType )
	//	return;

	CSlot* pSlot = GetInvenSlotByRealIndex( iReal );

	if( pSlot && pSlot->GetIcon() )
	{
		CSlot* pTargetSlot = &(m_ItemSlots[iVirtualType][iVirtualSlot]);
		if( pTargetSlot != pSlot )
		{
			CIconItem* pItemIcon = (CIconItem*)pSlot->GetIcon();
			if( CIcon* pTargetIcon = pTargetSlot->GetIcon() )
			{
				CItem* pTempItem = ((CIconItem*)pTargetIcon)->GetCItem();
				assert( pTempItem );
				pTargetSlot->DetachIcon();

				CItem* pSourceItem = pItemIcon->GetCItem();
				assert( pSourceItem );
				pSlot->DetachIcon();

				pTargetSlot->AttachIcon( pSourceItem->CreateItemIcon() );
				pSlot->AttachIcon( pTempItem->CreateItemIcon() );
			}
			else
			{
				CItem* pItem = pItemIcon->GetCItem();
				pSlot->DetachIcon();
				pTargetSlot->AttachIcon( pItem->CreateItemIcon() );
			}
		}
	}
}
//*-----------------------------------------------------------------------
/// @brief 실제 인벤토리인덱스를 가진 아이템 아이콘을 가진 Slot을 찾는다.
//*-----------------------------------------------------------------------
CSlot*	CItemDlg::GetInvenSlotByRealIndex( int iIndex )
{
	CIcon* pIcon = NULL;
	for( int iType = 0 ; iType < MAX_INV_TYPE ; ++iType )
	{
		for( int iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
			if( pIcon = m_ItemSlots[iType][iSlot].GetIcon() )
			{
				if( pIcon->GetIndex() == iIndex )
					return &(m_ItemSlots[iType][iSlot]);
			}
		}
	}
	return NULL;
}

void CItemDlg::ApplySavedVirtualInventory( std::list<S_InventoryData>& list )
{
	std::list<S_InventoryData>::iterator iter;
	for( iter = list.begin(); iter != list.end(); ++iter )
		SwitchIcon( iter->lRealIndex, iter->lVirtualIndex );
}
void CItemDlg::GetVirtualInventory( std::list<S_InventoryData>& list )
{
	list.clear();
	S_InventoryData Data;
	CIcon* pIcon;
	for( int iType  = 0;  iType < MAX_INV_TYPE; ++iType )
	{
		for( int iSlot = 0 ; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
		{
			if( pIcon = m_ItemSlots[iType][iSlot].GetIcon() )
			{
				Data.lRealIndex    = pIcon->GetIndex();
				Data.lVirtualIndex = iType * INVENTORY_PAGE_SIZE + iSlot;
				list.push_back( Data );
			}
		}
	}
}

unsigned	 CItemDlg::ActionPerformed( CActionEvent* e )
{
	assert( e );
	if( e == NULL ) return 0;
	switch( e->GetID() )
	{
	case WM_LBUTTONDOWN:
		{
			CWinCtrl* pSource = e->GetSource();
			assert( pSource );
			if( pSource == NULL ) break;

			unsigned wLParam = e->GetLParam();
			POINT ptMouse = { LOWORD( wLParam ), HIWORD( wLParam ) };
			if( !pSource->IsInside( ptMouse.x, ptMouse.y ) )
				break;

			switch( pSource->GetControlType() )
			{
			case CTRL_SLOT:
				{
					CSlot* pSlot = (CSlot*)pSource;
					if( CIcon* pIcon = pSlot->GetIcon() )
					{
						switch( g_itMGR.GetState() )
						{
						case IT_MGR::STATE_REPAIR:
							{
								CGame& refGame = CGame::GetInstance();
								if( refGame.GetRepairMode() != CGame::REPAIR_NONE  )///수리모드일경우
								{
									pSlot->ResetClicked();
									if( IsAvailableRepair( pIcon ) )
									{
										CIconItem* pItemIcon =( CIconItem*) pIcon;
										switch( refGame.GetRepairMode() )
										{
										case CGame::REPAIR_ITEM:
											{
												g_pNet->Send_cli_USE_ITEM_TO_REPAIR( refGame.GetUsingRepairItemInvenIdx(), pItemIcon->GetIndex() );
//												CTCommand* pCmd = new CTCmdEndRepair;
//												g_itMGR.AddTCommand( DLG_TYPE_MAX, pCmd );
												return e->GetID();
												break;
											}
										case CGame::REPAIR_NPC:
											{
												g_pNet->Send_cli_REPAIR_FROM_NPC( refGame.GetRepairNpcSvrIdx(), pItemIcon->GetIndex());
//												CTCommand* pCmd = new CTCmdEndRepair;
//												g_itMGR.AddTCommand( DLG_TYPE_MAX, pCmd );
												return e->GetID();
												break;
											}
										default:
											break;
										}
									}
								}
								break;
							}
						case IT_MGR::STATE_APPRAISAL:
							{
								pSlot->ResetClicked();
								CIconItem* pItemIcon =( CIconItem*) pIcon;
								tagITEM& Item = pItemIcon->GetItem();
								if( Item.IsEnableAppraisal() )
								{
									__int64 i64Price = (ITEM_BASE_PRICE( Item.GetTYPE(), Item.GetItemNO() ) + 10000 ) * ( Item.GetDurability() + 50 ) / 10000;
									if( i64Price <= g_pAVATAR->Get_MONEY() )
									{
										CGame::GetInstance().SetAppraisalCost( i64Price );
										CTCommand* pCmd = new CTCmdSendAppraisalReq( pItemIcon->GetIndex() );
										g_itMGR.OpenMsgBox(CStr::Printf( STR_MSG_IDENTIFY_COST, pItemIcon->GetName(), i64Price ),  
											CMsgBox::BT_OK | CMsgBox::BT_CANCEL, 
											true,
											0,
											pCmd );
									}
									else
									{
										g_itMGR.OpenMsgBox( STR_NOTENOUGH_IDENTIFYCOST );
									}
								}
								break;
							}
						default:
							break;
						}
					}
					break;
				}
			default:
				break;
			}
			break;
		}
	default:
		break;
	}
	return 0;
}

bool CItemDlg::IsAvailableRepair( CIcon* pIcon )
{
	assert( pIcon );
	if( pIcon == NULL ) return false;

	CIconItem* pItemIcon = (CIconItem*)pIcon;
	/*const*/ tagITEM& Item = pItemIcon->GetItem();

	if( Item.GetTYPE() != ITEM_TYPE_RIDE_PART && !Item.IsEquipITEM() )
		return false;

	if( Item.GetLife() >= 1000 )
		return false;
	
	if( ITEM_TYPE( Item.GetTYPE(), Item.GetItemNO() ) == TUNING_PART_ENGINE_CART ||
		ITEM_TYPE( Item.GetTYPE(), Item.GetItemNO() ) == TUNING_PART_ENGINE_CASTLEGEAR )
		return false;

	if( Item.GetDurability() <= 0 )
	{
		g_itMGR.AppendChatMsg(STR_CANT_REPAIR_DURABILITY_IS_ZERO, IT_MGR::CHAT_TYPE_SYSTEM );	
		return false;
	}
	
	return true;
}

void CItemDlg::AddActionEventListener2Slots( )
{
	int iIndex = 0;

	for( iIndex = 0 ; iIndex < MAX_EQUIP_IDX-1 ; ++iIndex )
		m_AvatarEquipSlots[iIndex].AddActionListener( this );

	for( int iIndex = 0; iIndex  < MAX_RIDING_PART; ++iIndex )
		m_PatEquipSlots[iIndex].AddActionListener( this );

	for( int iType = 0; iType < MAX_INV_TYPE; ++iType )
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[iType][iSlot].AddActionListener( this );



}
void CItemDlg::RemoveActionEventListener2Slots( )
{
	int iIndex = 0;
	for( iIndex = 0 ; iIndex < MAX_EQUIP_IDX-1 ; ++iIndex )
		m_AvatarEquipSlots[iIndex].RemoveActionListener( this );

	for( int iIndex = 0; iIndex  < MAX_RIDING_PART; ++iIndex )
		m_PatEquipSlots[iIndex].RemoveActionListener( this );

	for( int iType = 0; iType < MAX_INV_TYPE; ++iType )
		for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE; ++iSlot )
			m_ItemSlots[iType][iSlot].RemoveActionListener( this );
}
