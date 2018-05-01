#include "stdafx.h"
#include ".\cpatdlg.h"
//#include "../it_mgr.h"
//#include "../CDragItem.h"
//#include "../Icon/CIconItem.h"
//#include "../Command/UICommand.h"
//#include "../Command/dragcommand.h"
//#include "../Command/ctcmdnumberinput.h"
//#include "../../Object.h"
//#include "../../Common/IO_Pat.h"
//#include "../../gamecommon/CObservable.h"
//#include "../../gamecommon/item.h"
//#include "../../GameData/Event/CTEventItem.h"

CPatDlg::CPatDlg()
{
	//SetDialogType( iType );

	//m_pCmdInvenIcon = new CTCmdAssembleRideItem;
	//m_pCmdEquipIcon = new CTCmdDisAssembleRideItem;


	//CTCommand* pCmd = NULL;

	//m_pEquipDragItem = new CDragItem;
	//pCmd = new CTCmdDragItemFromPatEquip;
	//m_pEquipDragItem->AddTarget( iType, pCmd );


	//m_pInvenDragItem = new CDragItem;
	//pCmd = new CTCmdDragItemFromPatInven;
	//m_pInvenDragItem->AddTarget( iType, pCmd );


	/////매매창으로의 이동
	//CTCmdNumberInput* pNumberCmd = new CTCmdAddItem2DealFromInventory;
	//CTCmdOpenNumberInputDlg* pOpenCmd = new CTCmdOpenNumberInputDlg;
	//pOpenCmd->SetCommand( pNumberCmd );

	//m_pInvenDragItem->AddTarget( DLG_TYPE_DEAL, pOpenCmd );

	/////상점창으로 이동
	//pNumberCmd = new CTCmdSellItem;
	//pOpenCmd = new CTCmdOpenNumberInputDlg;
	//pOpenCmd->SetCommand( pNumberCmd );
	//m_pInvenDragItem->AddTarget( DLG_TYPE_STORE, pOpenCmd );

	/////땅으로 드랍
	//pNumberCmd	= new CTCmdDropItem;
	//pOpenCmd	= new CTCmdOpenNumberInputDlg;
	//pOpenCmd->SetCommand( pNumberCmd );
	//m_pInvenDragItem->AddTarget( CDragItem::TARGET_GROUND, pOpenCmd );

	/////Bank로 이동
	//pNumberCmd	= new CTCmdMoveItemInv2Bank;
	//pOpenCmd	= new CTCmdOpenNumberInputDlg;
	//pOpenCmd->SetCommand( pNumberCmd );
	//m_pInvenDragItem->AddTarget( DLG_TYPE_BANK, pOpenCmd );


	/////교환창으로 이동
	//pNumberCmd  = new CTCmdAddMyItem2Exchange;
	//pOpenCmd	= new CTCmdOpenNumberInputDlg;
	//pOpenCmd->SetCommand( pNumberCmd );
	//m_pInvenDragItem->AddTarget( DLG_TYPE_EXCHANGE, pOpenCmd );









	//CSlot Slot;
	//CSlotPatEquip EquipSlot;
	//POINT ptPos = { 77, 45 };
	//for( int i = 0; i < MAX_RIDING_PART; ++i )
	//{
	//	EquipSlot.SetOffset( ptPos );
	//	EquipSlot.SetDragAvailable();
	//	EquipSlot.SetDragItem( m_pEquipDragItem );
	//	EquipSlot.SetParent( iType );
	//	m_listEquipSlot.push_back( EquipSlot );
	//	ptPos.y += 37;
	//}


	//ptPos.x = 11;
	//ptPos.y = 201;

	//for( int i = 0; i < 4; ++i )
	//{
	//	for( int j = 0 ; j < 6; ++j )
	//	{
	//		Slot.SetOffset( ptPos );
	//		Slot.SetDragAvailable();
	//		Slot.SetDragItem( m_pInvenDragItem );
	//		Slot.SetParent( iType );
	//		m_listInvenSlot.push_back( Slot );
	//		ptPos.x += 33;
	//	}
	//	ptPos.x = 11;
	//	ptPos.y += 33;
	//}
}

CPatDlg::~CPatDlg(void)
{
	//SAFE_DELETE( m_pEquipDragItem );
	//SAFE_DELETE( m_pInvenDragItem );
}

//void	CPatDlg::MoveWindow( POINT pt )
//{
//	//CTDialog::MoveWindow( pt );
//	//for_each( m_listEquipSlot.begin(), m_listEquipSlot.end(), bind2nd( mem_fun_ref(&CSlot::MoveWindow), pt ));
//	//for_each( m_listInvenSlot.begin(), m_listInvenSlot.end(), bind2nd( mem_fun_ref(&CSlot::MoveWindow), pt ));
//}
//
//void	CPatDlg::Draw()
//{
//	if( !IsVision() ) return;
//	CTDialog::Draw();
//	for_each( m_listEquipSlot.begin(), m_listEquipSlot.end(), mem_fun_ref(&CSlot::Draw) );
//	for_each( m_listInvenSlot.begin(), m_listInvenSlot.end(), mem_fun_ref(&CSlot::Draw) );
//}
//
//
//void	CPatDlg::Update( POINT ptMouse )
//{
//	if( !IsVision() ) return;
//	CTDialog::Update( ptMouse );
//
//	for_each( m_listEquipSlot.begin(), m_listEquipSlot.end(), bind2nd( mem_fun_ref(&CSlot::Update), ptMouse ));
//	for_each( m_listInvenSlot.begin(), m_listInvenSlot.end(), bind2nd( mem_fun_ref(&CSlot::Update), ptMouse ));
//}
//
//unsigned	CPatDlg::Process( UINT uiMsg, WPARAM wParam, LPARAM lParam )
//{
//	if( !IsVision() ) return 0;
//	unsigned iProcID = 0;
//	if( iProcID = CTDialog::Process( uiMsg, wParam, lParam ))
//	{
//	/*	std::vector< CSlotPatEquip >::iterator iterEquipSlot;
//		for( iterEquipSlot = m_listEquipSlot.begin(); iterEquipSlot != m_listEquipSlot.end(); ++iterEquipSlot )
//		{
//			if( iterEquipSlot->Process( uiMsg, wParam, lParam ) )
//				return uiMsg;
//		}
//
//		std::vector< CSlot >::iterator iterInven;
//		for( iterInven = m_listInvenSlot.begin(); iterInven != m_listInvenSlot.end(); ++iterInven )
//		{
//			if( iterInven->Process( uiMsg, wParam, lParam ) )
//				return uiMsg;
//		}*/
//
//		switch(uiMsg)
//		{
//		case WM_LBUTTONUP:
//			OnLButtonUp( iProcID, wParam, lParam );
//			break;
//		default:
//			break;
//		}
//		return uiMsg;
//	}
//	return 0;
//}
//
//
//void CPatDlg::OnLButtonUp( unsigned iProcID, WPARAM wParam, LPARAM lParam )
//{
//	switch( iProcID )
//	{
//	case IID_BTN_CLOSE:
//		Hide();
//		break;
//	default:
//		break;
//	}
//}
//
//bool	CPatDlg::IsInsideInvenSlots( POINT ptMouse )
//{
//	std::vector< CSlot >::iterator	iter;
//	/*for( iter = m_listInvenSlot.begin(); iter != m_listInvenSlot.end(); ++iter )
//	{
//		if( iter->IsInside( ptMouse.x, ptMouse.y ) )
//			return true;
//	}*/
//	return false;
//}
//
//bool	CPatDlg::IsInsideEquipSlots( POINT ptMouse )
//{
//	/*std::vector< CSlotPatEquip >::iterator	iter;
//	for( iter = m_listEquipSlot.begin(); iter != m_listEquipSlot.end(); ++iter )
//	{
//		if( iter->IsInside( ptMouse.x, ptMouse.y ) )
//			return true;
//	}*/
//	return false;
//}
