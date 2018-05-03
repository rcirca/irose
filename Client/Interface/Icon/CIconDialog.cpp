#include "stdafx.h"
#include ".\cicondialog.h"
#include "../it_mgr.h"
#include "../CToolTipMgr.h"
#include "../CDragNDropMgr.h"
#include "../CDragItem.h"
#include "../IO_ImageRes.h"
#include "../command/uicommand.h"
#include "../TypeResource.h"

#include "tgamectrl/resourcemgr.h"
CIconDialog::CIconDialog( )
{
	m_pDragItem = NULL;
	m_bClicked  = false;;
}

CIconDialog::~CIconDialog(void)
{
	SAFE_DELETE( m_pDragItem );
}

void CIconDialog::SetDragItem( CDragItem* pDragItem )
{
	m_pDragItem = pDragItem;
}

CIconDialog* CIconDialog::CreateIcon( int iDialogType ,bool bCreateDragItem )
{
	int iGraphicID= 0;
	CInfo Info;
	Info.Clear();
	switch( iDialogType )
	{
	case DLG_TYPE_CLAN:
		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_GUILD" );
		Info.AddString( STR_CLAN );
		break;
	case DLG_TYPE_ITEM:
		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_ITEM" );
		Info.AddString( STR_ITEM);
		break;
	case DLG_TYPE_CHAR:
		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DLG_AVATAR" );
		Info.AddString( STR_AVATARINFO);
		break;
	case DLG_TYPE_SKILL:
		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_SKILL" );
		Info.AddString( STR_SKILL );
		break;
	case DLG_TYPE_QUEST:
		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_QUEST" );
		Info.AddString( STR_QUEST );
		break;
	case DLG_TYPE_COMMUNITY:
		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_COMMUNITY" );
		Info.AddString( STR_COMMUNITY );
		break;
//	case DLG_TYPE_GUILD:
//		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_GUILD" );
//		break;
	case DLG_TYPE_HELP:
		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_HELP" );
		Info.AddString( STR_HELP );
		break;
	case DLG_TYPE_OPTION:
		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_OPTION" );
		Info.AddString( STR_OPTION );
		break;
	case DLG_TYPE_PARTY:
		iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_PARTY" );
		Info.AddString( STR_PARTY );
		break;
	case DLG_TYPE_SYSTEM:
		//iGraphicID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI, "UI21_ICON_DIALOG_SYSTEM" );
		//break;
	default:
		iGraphicID = 0;
		break;
	}


//UI21_ICON_DIALOG_INFO

	if( iGraphicID )
	{
		CIconDialog* pIcon = new CIconDialog;
		pIcon->SetDialogType( iDialogType );
		pIcon->SetGraphicModuleID( IMAGE_RES_UI );
		pIcon->SetGraphicID( iGraphicID );
		pIcon->SetToolTip( Info );
		if( bCreateDragItem )
		{
			CDragItem* pDragItem = new CDragItem;
			CTCommand* pCmd = new CTCmdMoveDialogIcon2Ground;
			pDragItem->AddTarget( CDragItem::TARGET_GROUND, pCmd );

			if( iDialogType != DLG_TYPE_PARTY )
			{
				pCmd = new CTCmdRegistDialogIcon2QuickBar;
				pDragItem->AddTarget( DLG_TYPE_QUICKBAR, pCmd );
			}

			pIcon->SetDragItem( pDragItem );
			pDragItem->SetIcon( pIcon );
		}
		return pIcon;
	}

	return NULL;
}

void CIconDialog::SetDialogType( int iDialogType )
{
	m_iDialogType = iDialogType;
}
int  CIconDialog::GetDialogType()
{
	return m_iDialogType;
}

void CIconDialog::Update( POINT ptMouse )
{
	CIcon::Update( ptMouse );

	if( g_itMGR.IsMouseOnInterface() )
	{
		m_bClicked = false;	
		return ;
	}

	if( m_bClicked )
	{
		if( abs(m_ptClicked.x - ptMouse.x ) >= g_pCApp->GetWIDTH() / 200 ||
			abs(m_ptClicked.y - ptMouse.y ) >= g_pCApp->GetHEIGHT() / 200 )
		{
			m_pDragItem->SetIcon( this );
			CDragNDropMgr::GetInstance().DragStart( m_pDragItem );
			m_bClicked = false;
			return;
		}
	}

	if( !IsInside( ptMouse ) ) return ;

	ptMouse.x += 20;
	m_ToolTip.SetPosition( ptMouse );

	if( !m_ToolTip.IsEmpty() )
		CToolTipMgr::GetInstance().RegistInfo( m_ToolTip );

}

unsigned CIconDialog::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	POINT ptMouse = { LOWORD( lParam ), HIWORD( lParam ) };
	if( !IsInside( ptMouse ) ) return 0;

	switch( uiMsg )
	{
	case WM_LBUTTONUP:
		{
			if( m_bClicked )
			{
				m_bClicked = false;
				return uiMsg;
			}
			break;
		}
	case WM_LBUTTONDBLCLK:
		{
			ExecuteCommand();
			CTCmdRemoveDialogIcon* pCmd = new CTCmdRemoveDialogIcon( m_iDialogType );
			g_itMGR.AddTCommand( DLG_TYPE_MAX, pCmd );
			return uiMsg;
		}
	case WM_LBUTTONDOWN:
		{
			m_ptClicked.x = LOWORD( lParam );
			m_ptClicked.y = HIWORD( lParam );
			m_bClicked = true;
			return uiMsg;
		}
	}
	return 0;
}

void CIconDialog::ExecuteCommand()
{
	g_itMGR.OpenDialog( m_iDialogType );
}

CIcon*	CIconDialog::Clone()
{
	CIconDialog* pIcon = new CIconDialog;
	*pIcon = *this;
	pIcon->SetDragItem( NULL );
	return pIcon;
}

void CIconDialog::GetToolTip( CInfo& ToolTip,  DWORD dwDialogType, DWORD dwType )
{
	ToolTip = m_ToolTip;
}

void CIconDialog::SetPosition( POINT pt )
{
	if( pt.x < 0 ) pt.x = 0;
	if( pt.y < 0 ) pt.y = 0;

	if( pt.x + GetWidth() >= g_pCApp->GetWIDTH() )
		pt.x = g_pCApp->GetWIDTH() - GetWidth();

	if( pt.y + GetHeight() >= g_pCApp->GetHEIGHT() )
		pt.y = g_pCApp->GetHEIGHT() - GetHeight();

	CIcon::SetPosition( pt );
}

CDragItem*	CIconDialog::GetDragItem()
{
	return m_pDragItem;
}

int CIconDialog::GetIndex()
{
	return GetDialogType();
}