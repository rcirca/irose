#include "stdafx.h"
#include ".\cgoodsdlg.h"
#include "../Icon/CIconItem.h"
#include "TEditBox.h"
#include "../../GameData/CPrivateStore.h"
#include "../../common/io_stb.h"
CGoodsDlg::CGoodsDlg( int iDlgType)
{
	SetDialogType( iDlgType );
	m_Slot.SetParent( iDlgType );
	m_Slot.SetOffset( 17, 35 );
}

CGoodsDlg::~CGoodsDlg(void)
{
	m_Slot.DetachIcon();
}

void CGoodsDlg::SetIcon( CIcon* pIcon )
{
	m_Slot.DetachIcon();
	m_Slot.AttachIcon( pIcon->Clone() );
}

void CGoodsDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
	m_Slot.Draw();

	CIcon* pIcon = m_Slot.GetIcon();
	if( pIcon )
		drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, 60,5, g_dwWHITE,pIcon->GetName() );
}

void CGoodsDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	m_Slot.MoveWindow( m_sPosition );
}

void CGoodsDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;
	CTDialog::Update( ptMouse );
	m_Slot.Update( ptMouse );
}

unsigned CGoodsDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam, lParam ))
	{
		if( uiMsg == WM_LBUTTONUP )
		{
			switch( uiProcID )
			{
			case IID_BTN_CONFIRM:
				{
					char* pszBuf;
					int iUnitPrice = 0, iQuantity = 0;
					CIcon*	pIcon  = m_Slot.GetIcon();
					CWinCtrl* pCtrl = Find( IID_EDIT_PRICE );
					if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
					{
						CTEditBox* pEdit = (CTEditBox*)pCtrl;
						pszBuf = pEdit->get_text();
						if( pszBuf != NULL )
							iUnitPrice = atoi( pszBuf );
					}
					
					pCtrl = Find( IID_EDIT_QUANTITY );
					if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
					{
						CTEditBox* pEdit = (CTEditBox*)pCtrl;
						pszBuf = pEdit->get_text();
						if( pszBuf != NULL )
							iQuantity = atoi( pszBuf );
					}
					if( iUnitPrice > 0 && iQuantity > 0 && pIcon )
					{
						CIconItem* pItemIcon = (CIconItem*)pIcon;
						
						switch( m_iType )
						{
						case ADD_SELLLIST:
							{
								tagITEM Item = pItemIcon->GetItem();
								if( Item.IsEnableDupCNT() )
								{
									if( iQuantity < 0 )
										iQuantity = 1;
									if( iQuantity > Item.GetQuantity() )
										iQuantity = Item.GetQuantity();
								}
								else
								{
									iQuantity = 1;
								}
								
								CPrivateStore::GetInstance().AddItemSellList( pItemIcon->GetCItem(), iQuantity, iUnitPrice );
								break;
							}
						case ADD_BUYLIST:
							CPrivateStore::GetInstance().AddItemBuyList( pItemIcon->GetIndex(), iUnitPrice, iQuantity );						
							break;
						default:
							break;
						}
					}
					Hide();
					break;
				}
			case IID_BTN_CLOSE:
				Hide();
				break;
			default:
				break;
			}
		}
		return uiMsg;
	}
	return 0;
}

void CGoodsDlg::Show()
{
	CTDialog::Show();
	CWinCtrl* pCtrl = Find( IID_EDIT_PRICE );
	if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
	{
		CTEditBox* pEdit = (CTEditBox*)pCtrl;
		pEdit->clear_text();

		if( CIcon* pIcon = m_Slot.GetIcon() )
		{
			CIconItem* pItemIcon = (CIconItem*)pIcon;
			tagITEM& Item = pItemIcon->GetItem();
			
			int iPrice  = 1;
			if( m_iType	== ADD_SELLLIST )
				iPrice = ITEM_BASE_PRICE( Item.GetTYPE(), Item.GetItemNO() ) * 0.6;
			else
				iPrice = ITEM_BASE_PRICE( Item.GetTYPE(), Item.GetItemNO() ) * 0.7;

			pEdit->AppendText(CStr::Printf("%d", iPrice ));
		}
		else
		{
			pEdit->AppendText("1");
		}
	}
	
	pCtrl = Find( IID_EDIT_QUANTITY );
	if( pCtrl && pCtrl->GetControlType() == CTRL_EDITBOX )
	{
		CTEditBox* pEdit = (CTEditBox*)pCtrl;
		pEdit->clear_text();

		CIcon* pIcon = m_Slot.GetIcon();

		if( pIcon != NULL && m_iType == ADD_SELLLIST )
		{
			CIconItem* pItemIcon = (CIconItem*)pIcon;
			tagITEM& Item = pItemIcon->GetItem();
			pEdit->AppendText(CStr::Printf("%d", Item.GetQuantity() ));
		}
		else
		{
			pEdit->AppendText("1");
		}
	}

	switch( m_iType )
	{
	case ADD_SELLLIST:
		ShowChild( IID_TEXT_SELL );
		HideChild( IID_TEXT_BUY );
		break;
	case ADD_BUYLIST:
		ShowChild( IID_TEXT_BUY );
		HideChild( IID_TEXT_SELL );
		break;
	default:
		break;
	}
	
}
void CGoodsDlg::SetType( int iType )
{
	m_iType = iType;
}