#include "stdafx.h"
#include "CStoreDLG.h"
#include "DealDlg.h"
#include "../it_mgr.h"
#include "../CTDrawImpl.h"
#include "../IO_ImageRes.h"
#include "../TypeResource.h"
#include "../CTooltipMgr.h"

#include "../Command/CTCmdNumberInput.h"
#include "../Command/uicommand.h"
#include "../Command/dragcommand.h"

#include "../../Network/CNetwork.h"
#include "../../Common/IO_STB.h"
#include "../../System/CGame.h"
#include "../../gamecommon/item.h"
#include "../../gamedata/Cstore.h"
#include "../cdragitem.h"
#include "../icon/ciconitem.h"

#include "tgamectrl/tradiobutton.h"
#include "tgamectrl/tradiobox.h"


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



CStoreDLG::CStoreDLG( int iType )
{	
	m_nInvType   = 0;

	POINT ptOffset;

	m_pDragItem = new CDragItem;
	///매매창으로의 이동
	CTCmdNumberInput* pNumberCmd = new CTCmdAddItem2DealFromStore;
	CTCmdOpenNumberInputDlg* pOpenCmd = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pDragItem->AddTarget( DLG_TYPE_DEAL, pOpenCmd );
	///인벤토리로 이동
	pNumberCmd = new CTCmdBuyItem;
	pOpenCmd = new CTCmdOpenNumberInputDlg;
	pOpenCmd->SetCommand( pNumberCmd );
	m_pDragItem->AddTarget( DLG_TYPE_ITEM, pOpenCmd );

	///개인상점으로의 이동 = Wish List에 추가
	CTCommand* pCmd = new CTCmdDragItem2WishList;
	m_pDragItem->AddTarget( DLG_TYPE_PRIVATESTORE, pCmd );

	for( int i = 0; i < MAX_INV_TYPE; ++i )
	{
		for( int j = 0; j < c_iSlotCountPerTab; ++j )
		{
			ptOffset.x = 11 + ( (j%8) * (m_Slots[i][j].GetWidth() + 1) );
			ptOffset.y = 51 + ( (j/8) * (m_Slots[i][j].GetHeight() + 1) );
			m_Slots[i][j].SetOffset( ptOffset );
			m_Slots[i][j].SetParent( iType );
			m_Slots[i][j].SetDragAvailable();
			m_Slots[i][j].SetDragItem( m_pDragItem );
		}
	}

	SetDialogType( iType );


	m_pCmdNumberInput		= new CTCmdAddItem2DealFromStore;
	m_pCmdOpenNumberInput	= new CTCmdOpenNumberInputDlg;
	m_pCmdOpenNumberInput->SetCommand( m_pCmdNumberInput );
}

CStoreDLG::~CStoreDLG()
{
	SAFE_DELETE( m_pDragItem );
	SAFE_DELETE( m_pCmdNumberInput );
	SAFE_DELETE( m_pCmdOpenNumberInput );
}


void CStoreDLG::Draw()
{
	if(!IsVision()) return ;
	CTDialog::Draw();

	for( int iSlot = 0; iSlot < c_iSlotCountPerTab; ++iSlot )
		m_Slots[ m_nInvType ][iSlot].Draw();
}

unsigned int CStoreDLG::Process( UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsVision()) return 0;
	
	unsigned iProcID = 0;
	if( iProcID = CTDialog::Process(uiMsg,wParam,lParam)) 
	{
		for( int iSlot = 0; iSlot < c_iSlotCountPerTab; ++iSlot )
		{
			if( m_Slots[ m_nInvType ][iSlot].Process( uiMsg, wParam, lParam ) )
				return uiMsg;
		}
		
		switch(uiMsg)
		{
		case WM_LBUTTONUP:
			On_LButtonUP( iProcID, wParam, lParam );
			break;
		default:
			break;

		}
		return uiMsg;
	}
	return 0;
}
bool CStoreDLG::On_LButtonUP( unsigned iProcID, WPARAM wParam , LPARAM lParam )
{
	switch( iProcID )
	{
	case IID_BTN_CLOSE:
		g_itMGR.Close_store();
		break;
	case IID_BTN_TAB1:
		m_nInvType = 0;
		UpdateSlotPosition( m_nInvType );
		break;
	case IID_BTN_TAB2:
		m_nInvType = 1;
		UpdateSlotPosition( m_nInvType );
		break;
	case IID_BTN_TAB3:
		m_nInvType = 2;
		UpdateSlotPosition( m_nInvType );
		break;
	case IID_BTN_TAB4:
		m_nInvType = 3;
		UpdateSlotPosition( m_nInvType );
	default:
		break;
	}
	return false;
}


void CStoreDLG::SetEnableTabButton( int iInvType, bool bEnable )
{
	if( iInvType < 0 || iInvType >= MAX_INV_TYPE )
		return;

	unsigned int iID = 0;
	switch( iInvType )
	{
	case 0: iID = IID_BTN_TAB1; break;
	case 1: iID = IID_BTN_TAB2; break;
	case 2: iID = IID_BTN_TAB3; break;
	case 3: iID = IID_BTN_TAB4; break;
	default:
		return;
	}
	SetEnableChild( iID, bEnable );

}

void CStoreDLG::SetTabButtonText( short iIndex, const char* szText )
{
	unsigned int iID = 0;
	
	switch( iIndex )
	{
	case 0: iID = IID_BTN_TAB1; break;
	case 1: iID = IID_BTN_TAB2; break;
	case 2: iID = IID_BTN_TAB3; break;
	case 3: iID = IID_BTN_TAB4; break;
	default:
		return;
	}

	CWinCtrl* pCtrl = Find( iID );
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBUTTON )
	{
		CTRadioButton* pBtn = (CTRadioButton*)pCtrl;
		pBtn->SetText( (char*)szText );
	}
}

void CStoreDLG::Show()
{
	CTDialog::Show();
	/////Tab1 활성화, 나머지 False;
	CWinCtrl* pCtrl = Find( IID_RADIOBOX );

	m_nInvType = 0;	
	if( pCtrl && pCtrl->GetControlType() == CTRL_RADIOBOX )
	{
		CTRadioBox* pRadioBox = (CTRadioBox*)pCtrl;
		pRadioBox->SetPressedButton( IID_BTN_TAB1 );
	}

	CStore& StoreModel = CStore::GetInstance();

	for( int i = 0 ; i < 4; ++i )
	{
		if( StoreModel.GetTabName(i).empty() )
		{
			SetEnableTabButton( i, false );	
			SetTabButtonText(i, NULL );
		}
		else
		{
			SetEnableTabButton( i, true );	
			SetTabButtonText(i, StoreModel.GetTabName(i).c_str());
		}
	}
	

	
	UpdateSlotPosition( m_nInvType );
}


void CStoreDLG::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	UpdateSlotPosition( m_nInvType );
}

void CStoreDLG::UpdateSlotPosition( int iType )
{
	for( int iSlot = 0; iSlot < c_iSlotCountPerTab; ++iSlot )
		m_Slots[ iType ][ iSlot ].MoveWindow( m_sPosition );
}
void CStoreDLG::Update( POINT ptMouse )
{
	if(!IsVision() ) return;
	CTDialog::Update( ptMouse );
		
	for( int iSlot = 0; iSlot < c_iSlotCountPerTab; ++iSlot )
		m_Slots[ m_nInvType ][iSlot].Update( ptMouse );



	if( CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog() )
	{
		if( pDlg == (CTDialog*)this )
		{
			CTRadioButton* pBtn = NULL;	
			CWinCtrl* pCtrl = NULL;
			if( pCtrl = Find( IID_BTN_TAB1 ) )
			{
				pBtn = (CTRadioButton*)pCtrl;
				CSinglelineString& text = pBtn->GetText();
				if( text.is_ellipsis() && pBtn->IsInside( ptMouse.x, ptMouse.y ) )
					CToolTipMgr::GetInstance().RegToolTip( pBtn->GetPosition().x - 10, pBtn->GetPosition().y - 18, text.get_string() );
			}
			
			if( pCtrl = Find( IID_BTN_TAB2 ) )
			{
				pBtn = (CTRadioButton*)pCtrl;
				CSinglelineString& text = pBtn->GetText();
				if( text.is_ellipsis() && pBtn->IsInside( ptMouse.x, ptMouse.y ) )
					CToolTipMgr::GetInstance().RegToolTip( pBtn->GetPosition().x - 10, pBtn->GetPosition().y - 18, text.get_string() );
			}
			
			if( pCtrl = Find( IID_BTN_TAB3 ) )
			{
				pBtn = (CTRadioButton*)pCtrl;
				CSinglelineString& text = pBtn->GetText();
				if( text.is_ellipsis() && pBtn->IsInside( ptMouse.x, ptMouse.y ) )
					CToolTipMgr::GetInstance().RegToolTip( pBtn->GetPosition().x - 10, pBtn->GetPosition().y - 18, text.get_string() );
			}
			
			if( pCtrl = Find( IID_BTN_TAB4 ) )
			{
				pBtn = (CTRadioButton*)pCtrl;
				CSinglelineString& text = pBtn->GetText();
				if( text.is_ellipsis() && pBtn->IsInside( ptMouse.x, ptMouse.y ) )
					CToolTipMgr::GetInstance().RegToolTip( pBtn->GetPosition().x - 10, pBtn->GetPosition().y - 18, text.get_string() );
			}
		}
	}
	



	CObjCHAR* pObjChar = g_pObjMGR->Get_CharOBJ( CStore::GetInstance().GetNpcObjIndex(), false );

	///Object가 없거나 거리가 일정이상이면 닫아버려라.
	if( pObjChar == NULL )
		g_itMGR.Close_store();
	else if( g_pAVATAR->Get_DISTANCE( pObjChar ) >= 1000 )
		g_itMGR.Close_store();
}

int	 CStoreDLG::IsInValidShow()
{
	if( g_pAVATAR && g_pAVATAR->Get_HP() <= 0 )
		return 99;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DEAL ) )
		return DLG_TYPE_DEAL;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_EXCHANGE ) )
		return DLG_TYPE_EXCHANGE;

//	if( g_itMGR.IsDlgOpened( DLG_TYPE_QUERY )) // 상점은 Query창에서 불려진다.
//		return DLG_TYPE_QUERY;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_MAKE ))
		return DLG_TYPE_MAKE;

	return 0;
}

void CStoreDLG::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	if( pObj == NULL )
	{
		assert( pObj && "Invalide Update Argument @CStoreDLG::Update()" );
		return;
	}

	CItem* pItem = (CItem*)pObj;

	int iTab  = pItem->GetIndex() / c_iSlotCountPerTab;
	int iSlot = pItem->GetIndex() % c_iSlotCountPerTab;
	CIcon* pIcon = m_Slots[iTab][iSlot].GetIcon();

	if( pItem->IsEmpty() ) //삭제, 같은 Index를 찾아서 지워라
	{
		if( pIcon )
		{
			CIconItem* pItemIcon = (CIconItem*)pIcon;
			if( pItemIcon->GetIndex() == pItem->GetIndex() )
			{
				m_Slots[iTab][iSlot].DetachIcon();
			}
			else
			{
				assert( 0 && "해당슬롯에 다른 인덱스를 가진 아이콘이 있다 @CStore::Update");
			}
		}
		else
		{
			assert( pIcon && "없는 아이템을 지우려고 한다. @CStoreDLG::Update" );
		}
		
	}
	else///추가
	{
		if( pIcon )
		{
			assert( 0 && "추가할 슬롯에 이미 아이콘이 있다 @CStore::Update") ;
		}
		else
		{
			pIcon = pItem->CreateItemIcon();

			pIcon->SetCommand( m_pCmdOpenNumberInput );

			m_Slots[iTab][iSlot].AttachIcon( pIcon );
		}
	}
}