#include "stdafx.h"
#include ".\hoticonslot.h"

#include "../../Object.h"
#include "../ICon/CIconQuick.h"
#include "../ICon/CIconItem.h"
#include "../ICon/CIconSkill.h"
#include "../ICon/CIconDialog.h"
#include "../ICon/CIconSkillClan.h"
#include "../it_Mgr.h"
#include "../gamecommon/item.h"


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Constructor
//----------------------------------------------------------------------------------------------------

CHotIconSlot::CHotIconSlot(void)
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Destructor
//----------------------------------------------------------------------------------------------------

CHotIconSlot::~CHotIconSlot(void)
{
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CHotIconSlot::SetHotIcons( CHotICONS* pHotIcon )
{
	if( pHotIcon )
	{
		::CopyMemory ( &g_pAVATAR->m_HotICONS, pHotIcon, sizeof(CHotICONS) );		

		g_itMGR.UpdateQuickSlot();
	}
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------

void CHotIconSlot::AddQuickItem( int iSlot, int iType, int iIndex )
{	
	g_pAVATAR->m_HotICONS.m_IconLIST[ iSlot ].m_nSlotNo		= iIndex;
	g_pAVATAR->m_HotICONS.m_IconLIST[ iSlot ].m_cType		= iType;

	if( iType && iIndex && g_pSoundLIST )
		g_pSoundLIST->IDX_PlaySound( SID_REGIST_QUICKSLOT );

	g_itMGR.UpdateQuickSlot();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 해당 슬롯의 정보를 기반으로 적절한( Skill type인지 아이템 타입인지 ) CIcon객체를 생성한다.
//----------------------------------------------------------------------------------------------------

CIcon* CHotIconSlot::GetHotIcon( int iSlot )
{
	CIcon* pIcon = NULL;
	CIconQuick* pQuickIcon = NULL;
	switch( g_pAVATAR->m_HotICONS.m_IconLIST[ iSlot ].m_cType )
	{
		case 0:
			break;
		case INV_ICON:
			{
				CItemSlot* pItemSlot	= g_pAVATAR->GetItemSlot();				
				CItem* pItem			= pItemSlot->GetItem( g_pAVATAR->m_HotICONS.m_IconLIST[ iSlot ].m_nSlotNo );

				if( pItem )
					pQuickIcon	= new CIconQuick( pItem->CreateItemIcon()  );
			}
			break;
		case SKILL_ICON:
			{
				int iSkillSlot	= g_pAVATAR->m_HotICONS.m_IconLIST[ iSlot ].m_nSlotNo; 
				pQuickIcon		= new CIconQuick( new CIconSkill( iSkillSlot ) );
			}
			break;
		case DIALOG_ICON:
			{
				int iDialogType = g_pAVATAR->m_HotICONS.m_IconLIST[ iSlot ].m_nSlotNo;
				if( iDialogType > 0 && iDialogType < DLG_TYPE_MAX )
				{
					if( CIconDialog* p = CIconDialog::CreateIcon( iDialogType , false ) )
						pQuickIcon = new CIconQuick( p );
				}
				break;
			}
		case CLANSKILL_ICON:
			{
				int iSkillSlot = g_pAVATAR->m_HotICONS.m_IconLIST[ iSlot ].m_nSlotNo;
				pQuickIcon = new CIconQuick( new CIconSkillClan( iSkillSlot ) );
				break;
			}
		default:
			assert( 0 && "GetHotIcon gets invalid icon type" );
			break;
	}

	if( pQuickIcon )
		pQuickIcon->SetQuickBarSlotIndex( iSlot );

	return pQuickIcon;
}
//----------------------------------------------------------------------------------------------------
/// @param
/// @brief
//----------------------------------------------------------------------------------------------------
tagHotICON CHotIconSlot::GetHotItem( int iSlot )
{
	assert( iSlot >= 0 && iSlot < MAX_HOT_ICONS );
	return g_pAVATAR->m_HotICONS.m_IconLIST[ iSlot ];
}