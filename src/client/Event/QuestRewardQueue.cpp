#include "stdafx.h"
#include ".\questrewardqueue.h"
#include <algorithm>
#include "../Object.h"
#include "../Interface/it_MGR.h"


CQuestRewardQueue g_QuestRewardQueue;


CQuestRewardQueue::CQuestRewardQueue(void)
{
	Clear();
}

CQuestRewardQueue::~CQuestRewardQueue(void)
{
	Clear();
}

void CQuestRewardQueue::Clear()
{
//	m_InvItemList.clear();
	m_SetAbilityList.clear();
	m_AddAbilityList.clear();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 모든 리스트를 적용..
//----------------------------------------------------------------------------------------------------

void CQuestRewardQueue::ApplyReward()
{
	//----------------------------------------------------------------------------------------------------
	/// 아이템 보상
	//----------------------------------------------------------------------------------------------------	
	//std::for_each( m_InvItemList.begin(), m_InvItemList.end(), CQuestRewardQueue::RewardItem );
	
	//----------------------------------------------------------------------------------------------------
	/// Set ability
	//----------------------------------------------------------------------------------------------------	
	std::for_each( m_SetAbilityList.begin(), m_SetAbilityList.end(), CQuestRewardQueue::SetAbility );

	//----------------------------------------------------------------------------------------------------
	/// Add alility
	//----------------------------------------------------------------------------------------------------	
	std::for_each( m_AddAbilityList.begin(), m_AddAbilityList.end(), CQuestRewardQueue::AddAbility );


	/// Reset
	Clear();
}

//void CQuestRewardQueue::PushSetInvItem( tag_SET_INVITEM& sSetInvItem )
//{
//	m_InvItemList.push_back( sSetInvItem );
//}

void CQuestRewardQueue::PushSetAbility( gsv_SET_ABILITY& sSetAbility )
{
	m_SetAbilityList.push_back( sSetAbility );
}

void CQuestRewardQueue::PushAddAbility( gsv_SET_ABILITY& sAddAbility )
{
	m_AddAbilityList.push_back( sAddAbility );
}


//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 아이템 보상함수 for std::algorithm
//----------------------------------------------------------------------------------------------------

//void CQuestRewardQueue::RewardItem( tag_SET_INVITEM sInvITEM )
//{
//	char *szMsg = NULL;
//
//	tagITEM *pInvITEM = NULL;
//	tagITEM *pSrvITEM = NULL;
//
//	pSrvITEM = &sInvITEM.m_ITEM;
//	if ( pSrvITEM->IsEnableDupCNT() ) 
//	{
//		pInvITEM = &g_pAVATAR->m_Inventory.m_ItemLIST[ sInvITEM.m_btInvIDX ];
//		szMsg = CStr::Printf("%s %d개를 보상 받았습니다.", ITEM_NAME( pSrvITEM->GetTYPE(), pSrvITEM->GetItemNO() ), pSrvITEM->GetQuantity() - pInvITEM->GetQuantity() );
//	} else {
//		szMsg = CStr::Printf("%s을 보상 받았습니다.", ITEM_NAME( pSrvITEM->GetTYPE(), pSrvITEM->GetItemNO() ) );
//	}
//	g_itMGR.AppendChatMsg( szMsg, g_dwWHITE );
//}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 아바타 능력치 세팅
//----------------------------------------------------------------------------------------------------

void CQuestRewardQueue::SetAbility( gsv_SET_ABILITY sSetAbility )
{
	CStr::Printf( F_STR_SET_ABILITY, 
		CStringManager::GetSingleton().GetAbility( sSetAbility.m_wAbilityTYPE ),
			sSetAbility.m_iValue, g_dwWHITE );

	g_pAVATAR->Set_AbilityValue( sSetAbility.m_wAbilityTYPE, sSetAbility.m_iValue );
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief Add Avatar ability
//----------------------------------------------------------------------------------------------------

void CQuestRewardQueue::AddAbility( gsv_SET_ABILITY sSetAbility )
{
	CStr::Printf( F_STR_INCREASE_ABILITY, 
			CStringManager::GetSingleton().GetAbility( sSetAbility.m_wAbilityTYPE ),
			sSetAbility.m_iValue, g_dwWHITE );

	g_pAVATAR->Add_AbilityValue( sSetAbility.m_wAbilityTYPE, sSetAbility.m_iValue );
}