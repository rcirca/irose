#include "stdafx.h"
#include "cslot.h"
#include "../CGoodsDlg.h"
#include "../../Icon/CIconItem.h"
#include "../../CDragItem.h"
#include "../../CDragNDropMgr.h"
#include "../../IO_ImageRes.h"
#include "../../../Common/IO_STB.h"
#include "../../CInfo.h"
#include "../../it_mgr.h"
#include "../../CToolTipMgr.h"
#include "../../../GameData/CPrivateStore.h"
#include "../../../System/CGame.h"
#include "../../../Network/CNetwork.h"
#include "../../../GameCommon/Item.h"


#include "tgamectrl/tdialog.h"
#include "tgamectrl/tcommand.h"
#include "tgamectrl/actionevent.h"
#include "tgamectrl/iactionlistener.h"

CSlot::CSlot(void)
{
	m_bClicked		 = false;
	m_pIcon			 = NULL;
	m_pDragItem		 = NULL;
	m_pCommand		 = NULL;
	m_bDragAvailable = false;
	m_iWidth		= 40;
	m_iHeight		= 40;
	m_dwTooltipType = 0;
	SetControlType( CTRL_SLOT );

}

CSlot::~CSlot(void)
{
	DetachIcon();
}

void CSlot::DetachIcon()
{
	if( m_pIcon )
	{
		m_pIcon->SetSlot( NULL );
		delete m_pIcon;
		m_pIcon = NULL;
	}
}

bool CSlot::AttachIcon( CIcon* pIcon )
{
	if( pIcon == NULL )
		return false;

	if( m_pIcon )
		return false;

	m_pIcon = pIcon;

	SetIconPosition( m_sPosition );
	m_pIcon->SetSlot( this );
	return true;
}

CIcon * CSlot::MoveIcon()
{
	CIcon * pIcon = m_pIcon;
	m_pIcon = NULL;

    return pIcon;
}

void CSlot::Draw()
{
	if( m_pIcon )
		m_pIcon->Draw();
}

unsigned int CSlot::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	POINT pt = { LOWORD(lParam), HIWORD(lParam) };
	
	unsigned uiRet = 0;	
	
	switch( uiMsg )
	{
	case WM_LBUTTONUP:
		m_bClicked = false;
		break;
	case WM_LBUTTONDOWN:
		{
			if( CWinCtrl::IsInside( pt.x, pt.y ) )
			{
				if( m_pIcon )
				{
					if( m_pIcon->Process( uiMsg, wParam ,lParam ) == 0 )
					{
						if( m_bDragAvailable && m_pDragItem && m_pIcon->IsEnable() )
						{
							m_bClicked = true;
							m_ptClicked.x = LOWORD( lParam );
							m_ptClicked.y = HIWORD( lParam );
						}
					}
				}
				uiRet = uiMsg;
			}
			break;
		}
	case WM_LBUTTONDBLCLK:
		{
			///영역안이라도 커맨드 실행이 안된 빈슬롯은 처리안된것으로 한다.
			if( CWinCtrl::IsInside( pt.x, pt.y ) )
			{
				if( m_pIcon )
				{
					m_pIcon->ExecuteCommand();
					uiRet = uiMsg;
				}
				else if( m_pCommand )
				{
					m_pCommand->Exec( NULL );
					uiRet = uiMsg;
				}
			}
			break;
		}
	default:
		break;
	}

	if( !m_ActionListenerList.IsEmpty() && IsInside( pt.x, pt.y ) )
	{
		std::list< IActionListener* >& list =  m_ActionListenerList.GetListenerList( );
		std::list< IActionListener* >::iterator iter;
		CActionEvent Event( this, uiMsg , wParam, lParam );

		for( iter = list.begin(); iter != list.end(); ++iter )
			uiRet |= (*iter)->ActionPerformed( &Event );
	}
	return uiRet;
}

void CSlot::Update( POINT ptMouse )
{
	if( m_pIcon )
	{
		m_pIcon->Update( ptMouse );

		CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
		if( pDlg && pDlg->GetDialogType() != m_iParentID )
		{
			m_bClicked = false;		
			return ;
		}

		if( m_bClicked )
		{
			if( abs(m_ptClicked.x - ptMouse.x ) >= g_pCApp->GetWIDTH() / 200 ||
				abs(m_ptClicked.y - ptMouse.y ) >= g_pCApp->GetHEIGHT() / 200 )
			{

				m_pDragItem->SetIcon( m_pIcon );
				CDragNDropMgr::GetInstance().DragStart( m_pDragItem );
				m_bClicked = false;
				return;
			}
		}

		if( CWinCtrl::IsInside( ptMouse.x, ptMouse.y) )
		{
			CInfo	ToolTip;
			ToolTip.Clear();

			m_pIcon->GetToolTip( ToolTip ,m_iParentID, m_dwTooltipType );

			ptMouse.x += 20;
			ToolTip.SetPosition( ptMouse );

			if( !ToolTip.IsEmpty() )
				CToolTipMgr::GetInstance().RegistInfo( ToolTip );
		}
	}
}


void CSlot::SetPosition( POINT pt )
{
	m_sPosition.x = pt.x;
	m_sPosition.y = pt.y;

	SetIconPosition( pt );
}

//POINT CSlot::GetPosition()
//{
//	POINT pt = { m_rcThis.left, m_rcThis.top };
//	return pt;
//}

void CSlot::SetIconPosition( POINT pt )
{
	if( m_pIcon )
		m_pIcon->SetPosition( pt );
}
//
//void CSlot::SetOffset( POINT pt )
//{
//	m_ptOffset = pt;
//}

void CSlot::MoveWindow( POINT pt )
{
	POINT ptNew;
	ptNew.x = pt.x + m_ptOffset.x;
	ptNew.y = pt.y + m_ptOffset.y;
	SetPosition( ptNew );
}

//void CSlot::SetWidth( int iWidth )
//{
//	m_rcThis.right = m_rcThis.left + iWidth;
//}
//
//void CSlot::SetHeight( int iHeight )
//{
//	m_rcThis.bottom = m_rcThis.top + iHeight;
//}

void CSlot::SetParent( int iID )
{
	m_iParentID = iID;
}

void CSlot::SetDragAvailable()
{
	m_bDragAvailable = true;
}

void CSlot::SetDragDisable()
{
	m_bDragAvailable = false;
}

void CSlot::SetDragItem( CDragItem* pDragItem )
{
	assert( pDragItem );
	m_pDragItem = pDragItem;
}
CDragItem* CSlot::GetDragItem()
{
	return m_pDragItem;
}

int CSlot::GetParent()
{
	return m_iParentID;
}

void CSlot::ResetClicked()
{
	m_bClicked = false;
}

void CSlot::SetTooltipType( DWORD dwType )
{
	m_dwTooltipType = dwType;
}
/////*------------------------------------------------------------------------------------------
//#include "../../../object.h"
//unsigned int CSlotPatEquip::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
//{
//	if( g_pAVATAR->GetPetMode() >= 0 )
//		return 0;
//	return CSlot::Process( uiMsg, wParam ,lParam );
//}
//void CSlotPatEquip::Draw()
//{
//	CSlot::Draw();
//	CIcon* pIcon = GetIcon();
//	CIconItem* pItemIcon = NULL;
//	if( pIcon )
//	{
//		pItemIcon = (CIconItem*)pIcon;
//		RECT rcDraw = { 40, 8, 125, 26 };
//		drawFontf( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rcDraw ,D3DCOLOR_ARGB( 255,255,255,255 ), DT_VCENTER | DT_RIGHT ,ITEM_NAME( ITEM_TYPE_RIDE_PART, pItemIcon->GetItemNo() ) );
//	}
//}


///*------------------------------------------------------------------------------------------
//*----------------------------------------------------------------------------
CSlotBuyPrivateStore* CSlotBuyPrivateStore::s_pSelectedSlot = NULL;
CSlotBuyPrivateStore::CSlotBuyPrivateStore(void)
{
	m_bExhibition = false;
}
void CSlotBuyPrivateStore::SetSelected()
{
	assert( GetIcon() );
	s_pSelectedSlot = this;
}
bool CSlotBuyPrivateStore::IsSelected()
{
	assert( GetIcon() );
	if( s_pSelectedSlot == this )
		return true;
	return false;
}
void CSlotBuyPrivateStore::Draw()
{
	CSlot::Draw();
	//if( GetIcon() && ( m_bExhibition || s_pSelectedSlot == this) )
	//	g_DrawImpl.Draw( m_sPosition.x, m_sPosition.y, IMAGE_RES_ITEM, 0 );
}

void CSlotBuyPrivateStore::DetachIcon()
{
	CSlot::DetachIcon();
	if( s_pSelectedSlot == this )
		s_pSelectedSlot = NULL;

	SetExhibition( false );
}

void CSlotBuyPrivateStore::SetExhibition( bool b )
{
	m_bExhibition = b;
}

unsigned int CSlotBuyPrivateStore::Process( UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	POINT pt = { LOWORD( lParam ), HIWORD( lParam ) };
	if( !IsInside( pt.x, pt.y )) return 0;	

	CIcon* pIcon = GetIcon();
	if( pIcon && uiMsg == WM_LBUTTONDOWN && GetAsyncKeyState( VK_SHIFT ) < 0 )
	{
		CIconItem* pItemIcon = (CIconItem*)pIcon;
		if( m_bExhibition )
		{
			CPrivateStore::GetInstance().RemoveItemBuyList( pItemIcon->GetIndex() );
		}
		else
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_GOODS );
			if( pDlg )
			{
				CGoodsDlg* pGoodsDlg = (CGoodsDlg*)pDlg;
				pGoodsDlg->SetIcon( pItemIcon );
				pGoodsDlg->SetType( CGoodsDlg::ADD_BUYLIST );
				g_itMGR.OpenDialog( DLG_TYPE_GOODS );
			}
		}
		return uiMsg;
	}

	if( m_bExhibition )
	{
		if( uiMsg == WM_LBUTTONDOWN )
			return uiMsg;
		return 0;
	}
		
	return CSlot::Process( uiMsg, wParam, lParam );

}
bool CSlotBuyPrivateStore::IsExhibition()
{
	return m_bExhibition;
}
