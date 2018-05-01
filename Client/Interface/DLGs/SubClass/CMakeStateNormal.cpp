#include "stdafx.h"
#include ".\cmakestatenormal.h"
#include "../../Icon/CIconItem.h"
#include "../MakeDLG.h"
#include "../../it_mgr.h"
#include "../../CToolTipMgr.h"

#include "../../Command/UICommand.h"

#include "../../../Common/IO_Skill.h"
#include "../../../System/CGame.h"
#include "../../../Network/CNetwork.h"

#include "../../../System/CGame.h"

#include "../../../../TGameCtrl/JComboBox.h"
CMakeStateNormal::CMakeStateNormal( CMakeDLG* pParent)
{ 
	m_pParent = pParent;
	//m_iSelectedItemIdx = -1;
}

void CMakeStateNormal::Draw()
{

}

void CMakeStateNormal::Update( POINT ptMouse )
{
	/*int iRet = CManufacture::GetInstance().IsValidSendMakeItemReq();
	std::string	strToolTipMsg;
	if( iRet )
	{
		m_pParent->SetEnableChild( CMakeDLG::IID_BTN_START, false );
		switch( iRet )
		{
		case 1:
			strToolTipMsg = "NULL";
			break;
		case 2:
			strToolTipMsg = STR_NOT_ENOUGH_INVENTORY_SPACE;
			break;
		case 3:
			strToolTipMsg = STR_NOT_ENOUGH_MANA;
			break;
		case 4:
			strToolTipMsg = STR_NOT_EXIST_MATERIAL;
			break;
		case 5:
			strToolTipMsg = STR_NOT_ENOUGH_MATERIAL;
			break;
		default:
			break;
		}
	}
	else
	{
		m_pParent->SetEnableChild( CMakeDLG::IID_BTN_START, true );		
		strToolTipMsg = STR_START_MAKE_ITEM;
	}

	CWinCtrl* pCtrl = m_pParent->Find( CMakeDLG::IID_BTN_START);
	
	if( CWinCtrl::IsProcessMouseOver() )
		if( CWinCtrl::GetProcessMouseOverCtrl() == pCtrl )
			CToolTipMgr::GetInstance().RegToolTip( (short)ptMouse.x, (short)ptMouse.y , strToolTipMsg.c_str() );
*/
}

void CMakeStateNormal::Show()
{

}

void CMakeStateNormal::Hide()
{

}

unsigned int CMakeStateNormal::Process(UINT uiMsg,WPARAM wParam,LPARAM lParam )
{
	unsigned iProcID;
	if( iProcID = m_pParent->CTDialog::Process( uiMsg, wParam, lParam) )
	{
		for( int i = 0; i < g_iMaxCountMaterial; ++i )
			if( m_pParent->m_listMaterialSlot[i].Process( uiMsg, wParam, lParam ) )
				return uiMsg;

		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			OnLButtonUp( iProcID, wParam, lParam );
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown( iProcID, wParam, lParam );
			break;
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CMakeStateNormal::OnLButtonDown( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case CMakeDLG::IID_COMBOBOX_ITEM:
		{
			///Item을 바꾸어 주자
			CWinCtrl* pCtrl = m_pParent->Find( CMakeDLG::IID_COMBOBOX_ITEM );
			if( pCtrl )
			{
				CJComboBox* pComboBox = ( CJComboBox* ) pCtrl;
				const CTObject* pObj = pComboBox->GetSelectedItem();
				if( pObj )
				{
					CMakeComboItem* pComboItem = (CMakeComboItem*)pObj;
					CManufacture::GetInstance().SetMakeItem( pComboItem->GetItem() );
				}
			}
			break;
		}
	case CMakeDLG::IID_COMBOBOX_CLASS:
		{
			CWinCtrl* pCtrl = m_pParent->Find( CMakeDLG::IID_COMBOBOX_CLASS );
			if( pCtrl )
			{
				CJComboBox* pComboBox = ( CJComboBox* ) pCtrl;
				const CTObject* pObj = pComboBox->GetSelectedItem();
				if( pObj )
				{
					CMakeComboClass* pComboClass = (CMakeComboClass*)pObj;
					CManufacture::GetInstance().SetMakeClass( pComboClass->GetClass() );
				}
			}
			break;
		}
	default:
		break;
	}
}

void CMakeStateNormal::OnLButtonUp( unsigned iProcID, WPARAM wParam, LPARAM lParam )
{
	switch( iProcID )
	{
	case CMakeDLG::IID_BTN_CLOSE:
		m_pParent->Hide();
		return;
	case CMakeDLG::IID_BTN_START:
		{
			if( int iRet = CManufacture::GetInstance().IsValidSendMakeItemReq() )
			{
				if( iRet )
				{
					switch( iRet )
					{
					case 1:
						g_itMGR.OpenMsgBox( STR_NOTSELECT_MAKEITEM  );
						break;
					case 2:
						g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_INVENTORY_SPACE );
						break;
					case 3:
						g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MANA );
						break;
					case 4:
						g_itMGR.OpenMsgBox( STR_NOT_EXIST_MATERIAL );
						break;
					case 5:
						g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MATERIAL );
						break;
					default:
						break;
					}
				}
				break;
			}

			short nUseItemINV[g_iMaxCountMaterial];
			ZeroMemory( nUseItemINV, sizeof( short ) * g_iMaxCountMaterial );
			CIcon* pIcon = NULL;
			CIconItem* pItemIcon = NULL;
			
			for( int i = 0 ; i < g_iMaxCountMaterial; ++i )
			{
				if( pIcon  = m_pParent->m_listMaterialSlot[i].GetIcon() )
				{
					pItemIcon = ( CIconItem* ) pIcon;
					nUseItemINV[i] = pItemIcon->GetIndex();
				}
				else
				{
					nUseItemINV[i] = 0;
				}
			}

			m_pParent->ChangeState( CMakeDLG::STATE_WAIT );
			pIcon = m_pParent->m_MakeItemSlot.GetIcon();
			if( pIcon )
			{
				pItemIcon = (CIconItem*)pIcon;
				tagITEM& Item = pItemIcon->GetItem();
				if( !Item.IsEmpty() )
				{
					g_pNet->Send_cli_CREATE_ITEM_REQ(
						(BYTE)CManufacture::GetInstance().GetSkillSlotIndex(),
						(char)Item.GetTYPE(),
						(short)Item.GetItemNO(),
						nUseItemINV
					);
					g_pAVATAR->Skill_UseAbilityValue( CManufacture::GetInstance().GetSkillIndex() );
				}
			}
		}			
		return;
	default:
		break;
	}
}

void CMakeStateNormal::Init()
{
	m_pParent->SetEnableChild( CMakeDLG::IID_BTN_CLOSE, true );
}

bool CMakeStateNormal::IsModal()
{
	return m_pParent->CTDialog::IsModal();
}