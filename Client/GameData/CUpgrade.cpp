#include "stdafx.h"
#include ".\cupgrade.h"
#include "CRequireMaterial.h"
#include "../gamecommon/item.h"
#include "../interface/it_mgr.h"
#include "../interface/typeresource.h"
#include "event/CTEventItem.h"
#include "../Network/CNetwork.h"
#include "../common/CEconomy.h"

CUpgrade::CUpgrade(void)
{
	m_pTargetItem = NULL;
	m_iNpcSvrIdx	= 0 ;
	m_iSkillSlotIdx = 0;
	m_iUseItemInvenIdx = 0;
	m_iRequireMp	= 0;
	m_iSuccessProb  = 0;

	for( int i = 0 ; i < 3; ++i )
	{
		m_pMaterialItems[i] = NULL;
		m_pRequireMaterialItems[i] = NULL;/// 제련에 요구되는 아이템리스트
	}

}

CUpgrade::~CUpgrade(void)
{
	SAFE_DELETE( m_pTargetItem );

	for( int i = 0 ; i < 3; ++i )
	{
		SAFE_DELETE( m_pMaterialItems[i]);
		SAFE_DELETE( m_pRequireMaterialItems[i]);/// 제련에 요구되는 아이템리스트
	}

}

CUpgrade& CUpgrade::GetInstance()
{
	static CUpgrade s_Instance;
	return s_Instance;
}


void CUpgrade::SetTargetItem( CItem* pItem )
{
	tagITEM& Item = pItem->GetItem();
	m_pTargetItem = new CItemFragment( pItem );
	m_Event.SetID( CTEventUpgrade::EID_SET_TARGETITEM );
	m_Event.SetItem( m_pTargetItem );
	SetChanged();
	NotifyObservers( &m_Event );
	
	m_iRequireMoney = Item.GetUpgradeCost();

	int iMaterialLineNo = 0;
	if( Item.GetTYPE() == ITEM_TYPE_WEAPON )
		iMaterialLineNo = Item.GetGrade() + 1;
	else
		iMaterialLineNo = Item.GetGrade() + 11;
	
	assert( iMaterialLineNo >= 0 && iMaterialLineNo <= g_TblPRODUCT.m_nDataCnt );
	if( iMaterialLineNo >= 0 && iMaterialLineNo <= g_TblPRODUCT.m_nDataCnt )
	{
		CRequireMaterial* pRequireItem = NULL;
		if( int iClass = PRODUCT_RAW_MATERIAL( iMaterialLineNo ) )
		{
			pRequireItem = new CRequireMaterial;
			pRequireItem->SetItemClass( iClass );
			pRequireItem->SetRequireCount( PRODUCT_NEED_ITEM_CNT( iMaterialLineNo, 0 ) );
			pRequireItem->SetName( CStringManager::GetSingleton().GetItemType( iClass ) );
			m_pRequireMaterialItems[0] =  pRequireItem;
		}

		int iItemType = 0;
		int iItemNo   = 0;

		for( int iCol = 1 ; iCol < 3; ++iCol )
		{
			if( int iItem = PRODUCT_NEED_ITEM_NO( iMaterialLineNo, iCol ) )
			{
				assert( iItem > 1000 );
				if( iItem > 1000 )
				{
					iItemType = iItem / 1000;
					iItemNo   = iItem % 1000;
					assert( iItemNo >= 0 && iItemNo <= g_pTblSTBs[ iItemType ]->m_nDataCnt );
					if( iItemNo >= 0 && iItemNo <= g_pTblSTBs[ iItemType ]->m_nDataCnt )
					{
						pRequireItem = new CRequireMaterial;
						pRequireItem->SetItemType( iItemType );
						pRequireItem->SetItemNo( iItemNo );
						pRequireItem->SetRequireCount( PRODUCT_NEED_ITEM_CNT( iMaterialLineNo, iCol ) );
						if( ITEM_NAME(iItemType, iItemNo) )
							pRequireItem->SetName( ITEM_NAME(iItemType, iItemNo) );
						else
							pRequireItem->SetName( STR_NONAME );

						m_pRequireMaterialItems[iCol] = pRequireItem;
					}
				}
			}
		}
	}
}

void CUpgrade::SetMaterialItem( CItem* pItem )
{
	assert( pItem );
	if( pItem == NULL ) return;

	//1. 첫번째 재료 타입인가를 체크한다.
	CRequireMaterial* pRequireMaterial = m_pRequireMaterialItems[0];
	assert( pRequireMaterial );
	if( pRequireMaterial )
	{
		if( m_pMaterialItems[0] == NULL &&
			ITEM_TYPE( pItem->GetType(),pItem->GetItemNo() ) == pRequireMaterial->GetItemClass() && 
			pItem->GetQuantity() >= pRequireMaterial->GetRequireCount() )
		{
			CItemFragment* pFragmentItem = new CItemFragment( pItem );
			m_pMaterialItems[0]			 = pFragmentItem;

			pFragmentItem->SetQuantity( pRequireMaterial->GetRequireCount() );
			m_Event.SetID( CTEventUpgrade::EID_SET_MATERIALiTEM );
			m_Event.SetIndex( 0 );
			m_Event.SetItem( pFragmentItem );
			SetChanged();
			NotifyObservers( &m_Event );

			m_iRequireMp    = ComputeRequireMp();
			m_iSuccessProb = ComputeSuccessProb();
			return;
		}
	}

	for( int iIndex = 1; iIndex < 3; ++iIndex )
	{
		pRequireMaterial = m_pRequireMaterialItems[iIndex];
		if( pRequireMaterial )
		{
			if( pItem->GetType() == pRequireMaterial->GetItemType() &&
				pItem->GetItemNo() == pRequireMaterial->GetItemNo() &&
				m_pMaterialItems[iIndex] == NULL &&
				pItem->GetQuantity() >= pRequireMaterial->GetRequireCount() )
			{
				CItemFragment* pFragmentItem = new CItemFragment( pItem );
				m_pMaterialItems[iIndex]	 = pFragmentItem;
				pFragmentItem->SetQuantity( pRequireMaterial->GetRequireCount() );

				m_Event.SetIndex( iIndex );
				m_Event.SetID( CTEventUpgrade::EID_SET_MATERIALiTEM );
				m_Event.SetItem( pFragmentItem );
				SetChanged();
				NotifyObservers( &m_Event );

				m_iRequireMp   = ComputeRequireMp();
				m_iSuccessProb = ComputeSuccessProb();
				return;
			}
		}
	}
}

//*-----------------------------------------------------------/
/// 제련할 대상아이템이나 제련에 필요한 재료 아이템을 추가한다.
//*-----------------------------------------------------------/
bool CUpgrade::SetItem( CItem* pItem )
{
	assert( pItem );
	if( pItem == NULL ) return false;

	if( m_pTargetItem == NULL )
	{
		tagITEM& Item = pItem->GetItem();
		if( !Item.IsEnableUpgrade() )
		{
			g_itMGR.OpenMsgBox( STR_ITEM_CANT_UPGRADED );
			return false;
		}

		SetTargetItem( pItem );
	}
	else
	{
		SetMaterialItem( pItem );			
	}
	return true;
}

void CUpgrade::RemoveTargetItem()
{
	if( m_pTargetItem == NULL ) return;
	
	m_Event.SetID( CTEventUpgrade::EID_REMOVE_TARGETITEM );
	SetChanged();
	NotifyObservers( &m_Event );
	SAFE_DELETE( m_pTargetItem );

	m_iRequireMoney = 0;

	int iIndex = 0;

	for( iIndex = 0 ; iIndex < 3; ++iIndex )
	{
		if( m_pMaterialItems[iIndex] )
		{
			m_Event.SetID( CTEventUpgrade::EID_REMOVE_MATERIALITEM );
			m_Event.SetIndex( iIndex );
			SetChanged();
			NotifyObservers( &m_Event );
			SAFE_DELETE( m_pMaterialItems[iIndex] );
		}
		SAFE_DELETE( m_pRequireMaterialItems[iIndex ] );
	}

	m_iRequireMp	= 0;
	m_iSuccessProb  = 0;
}

//*-----------------------------------------------------------/
/// 제련할 대상아이템이나 제련에 필요한 재료 아이템을 뺀다.
//*-----------------------------------------------------------/
void CUpgrade::RemoveMaterialItem( CItem* pItem )
{
	assert( pItem );
	if( pItem == NULL ) return;

	for( int iIndex = 0 ; iIndex < 3; ++iIndex )
	{
		if( m_pMaterialItems[iIndex] && m_pMaterialItems[iIndex ]->GetIndex() == pItem->GetIndex() )
		{
			m_Event.SetID( CTEventUpgrade::EID_REMOVE_MATERIALITEM );
			m_Event.SetIndex( iIndex );
			SetChanged();
			NotifyObservers( &m_Event );

			SAFE_DELETE( m_pMaterialItems[iIndex] )

			m_iRequireMp	= 0;
			m_iSuccessProb  = 0;
			return;
		}
	}
}

void CUpgrade::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );
	if( pObj == NULL || strcmp( pObj->toString(),"CTEventItem" ) )
	{
		assert( 0 && "Invalid Event Type or NULL" );
		return;
	}

	CTEventItem* pEvent = (CTEventItem*)pObj;
	if( m_pTargetItem && 
		pEvent->GetID() == CTEventItem::EID_DEL_ITEM && 
		pEvent->GetIndex() == m_pTargetItem->GetIndex() )
	{
		RemoveTargetItem();
	}
}

void CUpgrade::SetNpcSvrIdx( int iNpcSvrIdx )
{
	m_iNpcSvrIdx	= iNpcSvrIdx ;
	m_iSkillSlotIdx = 0;
	m_iUseItemInvenIdx = 0;

}
void CUpgrade::SetSkillSlotIdx( int iSkillSlotIdx )
{
	m_iNpcSvrIdx	= 0 ;
	m_iSkillSlotIdx = iSkillSlotIdx;
	m_iUseItemInvenIdx = 0;
}

void CUpgrade::SetUseItemInvenIdx( int iUseItemInvenIdx )
{
	m_iNpcSvrIdx	= 0 ;
	m_iSkillSlotIdx = 0;
	m_iUseItemInvenIdx = iUseItemInvenIdx;
}

bool CUpgrade::Send_CRAFT_UPGRADE_REQ()
{
	assert( m_pTargetItem );
	if( m_pTargetItem == NULL ) return false;

	BYTE btUseItemINV[ UPGRADE_ITEM_STEP ];
	for( int i = 0; i < UPGRADE_ITEM_STEP; ++i )
	{
		if( m_pRequireMaterialItems[i] )
		{
			if( m_pMaterialItems[i] == NULL )
				return false;
			else
				btUseItemINV[i] = m_pMaterialItems[i]->GetIndex();
		}
		else
		{
			btUseItemINV[i] = 0;
		}
	}

	if( m_iNpcSvrIdx && m_iSkillSlotIdx == 0 && m_iUseItemInvenIdx == 0 )
	{
		g_pNet->Send_cli_CRAFT_UPGRADE_REQ( CRAFT_UPGRADE_FROM_NPC,
			m_iNpcSvrIdx, m_pTargetItem->GetIndex(), btUseItemINV );
	}
	else if( m_iSkillSlotIdx && m_iNpcSvrIdx == 0 && m_iUseItemInvenIdx == 0 )
	{
		g_pNet->Send_cli_CRAFT_UPGRADE_REQ( CRAFT_UPGRADE_USE_SKILL,
			m_iSkillSlotIdx, m_pTargetItem->GetIndex(), btUseItemINV );
	}
	else if( m_iUseItemInvenIdx && m_iSkillSlotIdx == 0 && m_iNpcSvrIdx == 0 )
	{
#pragma message("아이템 사용으로 제련하는 경우?")
	}
	return true;
}

int CUpgrade::GetRequireMp()
{
	return m_iRequireMp;
}

int CUpgrade::GetSuccessProb()
{
	return 	m_iSuccessProb;
}

int CUpgrade::ComputeSuccessProb()
{
	assert( m_pTargetItem );
	if( m_pMaterialItems[0] == NULL )
		return 0;

	if( m_pTargetItem == NULL )
		return 0;

	tagITEM& Item			= m_pTargetItem->GetItem();
	tagITEM& MaterialItem	= m_pMaterialItems[0]->GetItem();

	float fDuration		= Item.GetDurability();
	float fItemQuality	= (float)ITEM_QUALITY( Item.GetTYPE(), Item.GetItemNO() );
	float fGrade		= Item.GetGrade();
	float fMatQuality   = (float)ITEM_QUALITY( MaterialItem.GetTYPE(), MaterialItem.GetItemNO() );
	float fWorldProd	= Get_WorldPROD();

//	float fDividend		= (fGrade + 1) * (fGrade + 2) * (fGrade * 5 + fItemQuality + 260) * 37200 * 0.18f ;
//	float fDivisor		= fMatQuality * ( fDuration + 200 ) * ( fWorldProd + 10 );

	float fDividend		= 800 / 320 * ( fMatQuality * (fDuration + 180 ) * ( fWorldProd + 10 ) );
	float fDivisor		= ( fGrade + 2 ) * ( fGrade + 3 ) * (fGrade * 5 + fItemQuality * 3 + 250 );
	
#pragma message( "추후 바뀌어야 할 재련예상확률(2005/2/22)" )
	//float fDivisor		= ( fGrade + 2 ) * ( fGrade + 3 ) * (fGrade + 5 + fItemQuality * 3 + 250 );


	assert( fDivisor > 0  );
	if( fDivisor <= 0 )
		return 0;

	int iRet =  fDividend / fDivisor - 60;

	if(iRet <= 0 ) iRet = 1;
	if(iRet >= 100 ) iRet = 99;
	
	return iRet;
}

int CUpgrade::ComputeRequireMp()
{
	assert( m_pTargetItem );
	if( m_pMaterialItems[0] == NULL )
		return 0;

	if( m_pTargetItem == NULL )
		return 0;

	tagITEM& Item			= m_pTargetItem->GetItem();
	float fItemQuality	= (float)ITEM_QUALITY( Item.GetTYPE(), Item.GetItemNO() );
	float fGrade		= Item.GetGrade();

//	return (int)(( fGrade + 2 ) + (fItemQuality + 100 ) * 0.16 );
	return (int)(( fGrade + 4 ) * (fItemQuality + 20 ) * 0.9 );

}
const char*	CUpgrade::GetRequireMaterialName( int iIndex )
{
	assert( iIndex >= 0 && iIndex < 3 );
	if(  iIndex >= 0 && iIndex < 3 && m_pRequireMaterialItems[iIndex] )
		return m_pRequireMaterialItems[iIndex]->GetName();

	return NULL;
}
int CUpgrade::GetRequireMaterialCount( int iIndex )
{
	assert( iIndex >= 0 && iIndex < 3 );
	if(  iIndex >= 0 && iIndex < 3 && m_pRequireMaterialItems[iIndex] )
		return m_pRequireMaterialItems[iIndex]->GetRequireCount();

	return 0;
}

const char* CUpgrade::GetTargetItemName()
{
	if( m_pTargetItem )
		return m_pTargetItem->GetName();

	return NULL;
}

CItemFragment* CUpgrade::GetTargetItem()
{
	return m_pTargetItem;
}

int CUpgrade::IsValidUpgradeCondition()
{
	if( m_pTargetItem == NULL ) return INVALID_TARGETITEM;
	if( !m_pTargetItem->GetItem().IsEnableUpgrade() ) return INVALID_TARGETITEM;
	switch( GetType() )
	{
	case TYPE_SKILL:
		if( g_pAVATAR->Get_MP() < m_iRequireMp ) 
			return NOTENOUGH_MP;
		break;
	case TYPE_NPC:
		if( g_pAVATAR->Get_MONEY() < m_iRequireMoney )
			return NOTENOUGH_MONEY;
		break;
	default:
		return INVALID_TYPE;
		break;
	}


	for( int iIndex = 0; iIndex < 3; ++iIndex )
	{
		if( m_pRequireMaterialItems[iIndex] )
		{
			if(m_pMaterialItems[iIndex] == NULL )
				return INVALID_MATERIAL;

			if( m_pRequireMaterialItems[iIndex]->GetRequireCount() > m_pMaterialItems[iIndex]->GetQuantity() )
				return NOTENOUGH_MATERIAL;
		}
	}
	return ALL_OK;


}

bool CUpgrade::SendPacketUpgrade()
{
	assert( m_pTargetItem );
	if( m_pTargetItem == NULL ) return false;

	if( int iError = IsValidUpgradeCondition() )
	{
		switch( iError )
		{
		case NOTENOUGH_MP:
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MANA );
			break;
		case NOTENOUGH_MATERIAL: /// 재료아이템의 개수 부족.
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MATERIAL );
			break;
		case INVALID_MATERIAL:   /// 필요한 재료아이템이 없다.
			g_itMGR.OpenMsgBox( STR_NOT_EXIST_MATERIAL );
			break;
		case INVALID_TARGETITEM: /// 제련할 아이템이 없거나 잘못된 아이템이다.
			break;
		case NOTENOUGH_MONEY:	 /// npc로 제련시 돈이 부족합니다.
			g_itMGR.OpenMsgBox( STR_NOT_ENOUGH_MONEY );
			break;
		default:
			break;
		}
		return false;
	}
	

	BYTE btUseITEMInv[3];
	for( int i = 0; i < 3; ++i )
	{
		if( m_pMaterialItems[i] )
			btUseITEMInv[i] = m_pMaterialItems[i]->GetIndex();
		else
			btUseITEMInv[i] = 0;
	}
	if( m_iNpcSvrIdx && m_iSkillSlotIdx == 0 && m_iUseItemInvenIdx == 0 )
	{
		g_pNet->Send_cli_CRAFT_UPGRADE_REQ( CRAFT_UPGRADE_FROM_NPC, m_iNpcSvrIdx, m_pTargetItem->GetIndex(), btUseITEMInv );
	}
	else if( m_iNpcSvrIdx == 0 && m_iSkillSlotIdx && m_iUseItemInvenIdx == 0 )
	{
		g_pNet->Send_cli_CRAFT_UPGRADE_REQ( CRAFT_UPGRADE_USE_SKILL, m_iSkillSlotIdx,  m_pTargetItem->GetIndex(), btUseITEMInv );
	}
	else if( m_iNpcSvrIdx == 0  &&  m_iSkillSlotIdx == 0 && m_iUseItemInvenIdx  )
	{
		return false;
	}
	else
	{
		assert( 0 && "Invalid Upgrade Type" );
		return false;
	}
	return true;
}

void CUpgrade::ClearResultItemSet()
{
	m_ResultItemSet.clear();
}

void CUpgrade::AddResultItemSet( tag_SET_INVITEM& Item )
{
	m_ResultItemSet.push_back( Item );
}

//*----------------------------------------------------------------------//
/// 모든 결과가 Setting된 후에 호출되어야 한다.
//*----------------------------------------------------------------------//
void CUpgrade::SetResult( BYTE btResult )
{
	m_btResult = btResult;
	m_Event.SetID( CTEventUpgrade::EID_RECEIVE_RESULT );
	SetChanged();
	NotifyObservers( &m_Event );
}

void CUpgrade::SetResultSuccessProb( int iSuccessProb )
{
	m_iResultSuccessProb = iSuccessProb;
}

int	 CUpgrade::GetResultSuccessProb()
{
	return m_iResultSuccessProb;
}

BYTE CUpgrade::GetResult()
{
	return m_btResult;
}

void CUpgrade::ApplyResultItemSet()
{
	std::list<tag_SET_INVITEM>::iterator iter;
	for( iter = m_ResultItemSet.begin(); iter != m_ResultItemSet.end(); ++iter )
		g_pAVATAR->Set_ITEM( iter->m_btInvIDX, iter->m_ITEM );
}

WORD CUpgrade::GetNpcSvrIdx()
{
	return m_iNpcSvrIdx;
}

int CUpgrade::GetType()
{
	if( m_iNpcSvrIdx && m_iSkillSlotIdx == 0 && m_iUseItemInvenIdx == 0 )
		return TYPE_NPC;		
	else if( m_iNpcSvrIdx == 0 && m_iSkillSlotIdx && m_iUseItemInvenIdx == 0 )
		return TYPE_SKILL;

	return TYPE_NONE;
}

int CUpgrade::GetRequireMoney()
{
	return m_iRequireMoney;
}