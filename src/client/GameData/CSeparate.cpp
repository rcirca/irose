#include "stdafx.h"
#include ".\cseparate.h"
#include "../GameCommon/Item.h"
#include "../interface/it_mgr.h"
#include "../Common/CItem.h"
#include "../Common/IO_STB.h"
#include "../Network/CNetwork.h"
#include "event/CTEventItem.h"
CSeparate::CSeparate(void)
{
	m_pMaterialItem = NULL;///분리 하고자 하는 아이템
	m_iRequireMp    = 0;
	m_wNpcSvrIdx    = 0;
	m_iSkillSlotIdx = 0;
	m_bWaitServerReply = false;
}

CSeparate::~CSeparate(void)
{

}

CSeparate& CSeparate::GetInstance()
{
	static CSeparate s_Instance;
	return s_Instance;
}

void CSeparate::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );

	if( m_pMaterialItem && pObj && strcmp( pObj->toString(), "CTEventItem") == 0  )
	{
		CTEventItem* pEvent = (CTEventItem*)pObj;
		int iIndex = pEvent->GetIndex();
		if( pEvent->GetID() == CTEventItem::EID_DEL_ITEM || 
			pEvent->GetID() == CTEventItem::EID_CHANGE_ITEM )
		{
			if( iIndex == m_pMaterialItem->GetIndex() )
			{
				m_bWaitServerReply = false;

				m_Event.SetID( CTEventSeparate::EID_REMOVE_MATERIAL_ITEM );
				SetChanged();
				NotifyObservers( &m_Event );
				delete m_pMaterialItem;
				m_pMaterialItem = NULL;

				std::vector< CItem* >::iterator iter;
				int iIndex = 0;
				for( iter = m_pItems.begin(); iter != m_pItems.end(); ++iIndex )
				{
					m_Event.SetID( CTEventSeparate::EID_REMOVE_OUTPUT_ITEM );
					m_Event.SetIndex( iIndex );
					SetChanged();
					NotifyObservers( &m_Event );

					delete *iter;
					iter = m_pItems.erase( iter );
				}
			}
		}
	}
}

void CSeparate::RemoveItem()
{
//	assert( m_pMaterialItem );
	if( m_pMaterialItem )
	{
		m_Event.SetID( CTEventSeparate::EID_REMOVE_MATERIAL_ITEM );
		SetChanged();
		NotifyObservers( &m_Event );
		delete m_pMaterialItem;
		m_pMaterialItem = NULL;
	}

	std::vector< CItem* >::iterator iter;
	int iIndex = 0;
	for( iter = m_pItems.begin(); iter != m_pItems.end(); ++iIndex )
	{
		m_Event.SetID( CTEventSeparate::EID_REMOVE_OUTPUT_ITEM );
		m_Event.SetIndex( iIndex );
		SetChanged();
		NotifyObservers( &m_Event );

		delete *iter;
		iter = m_pItems.erase( iter );
	}

	m_iRequireMoney = 0;
}

void CSeparate::SetItem( CItem* pItem )
{
	assert( pItem );
	tagITEM& Item = pItem->GetItem();

	m_iType = TYPE_NONE;

	if( !Item.IsEnableSeparate() )
	{
		g_itMGR.OpenMsgBox(STR_ITEM_CANT_SEPARATED);
		return;
	}

	if( Item.HasSocket() && Item.GetGemNO() > 300 )
		m_iType = TYPE_SEPARATE;
	else if( ITEM_PRODUCT_IDX( Item.GetTYPE(), Item.GetItemNO() ) )
		m_iType = TYPE_DECOMPOSITION;
	else
	{
		assert( 0 && "Invalid Separate Type" );
		return;
	}

	if( m_pMaterialItem )
		RemoveItem();

	m_pMaterialItem = new CItemFragment( pItem );	
	
	if( Item.IsEnableDupCNT() )
		m_pMaterialItem->SetQuantity( 1 );

	m_Event.SetID( CTEventSeparate::EID_SET_MATERIAL_ITEM );
	m_Event.SetItem( m_pMaterialItem );
	SetChanged();
	NotifyObservers( &m_Event );

	m_iRequireMoney = Item.GetSeparateCost();

	switch( m_iType )
	{
	case TYPE_SEPARATE:
		{
			tagITEM MainMaterialItem;		
			MainMaterialItem.Init( pItem->GetType() * 1000 + pItem->GetItemNo() );
			CItem* pMainMaterialItem = new CItem;
			pMainMaterialItem->SetItem( MainMaterialItem );
			m_pItems.push_back( pMainMaterialItem );

			m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
			m_Event.SetIndex( 0 );
			m_Event.SetItem( pMainMaterialItem );
			SetChanged(); 
			NotifyObservers( &m_Event );


			tagITEM GemItem;
			GemItem.Init( ITEM_TYPE_GEM * 1000 + Item.GetGemNO() );
			CItem* pGemItem	 = new CItem;
			pGemItem->SetItem( GemItem );
			m_pItems.push_back( pGemItem );

			m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
			m_Event.SetIndex( 1 );
			m_Event.SetItem( pGemItem );
			SetChanged();
			NotifyObservers( &m_Event);

			m_iRequireMp = ITEM_QUALITY( MainMaterialItem.GetTYPE(), MainMaterialItem.GetItemNO() ) / 2 +
				ITEM_QUALITY( GemItem.GetTYPE(), GemItem.GetItemNO() );

			break;
		}
	case TYPE_DECOMPOSITION:
		{
			tagITEM MaterialItem;
			CItem*  pMaterialItem = NULL;
			int iProductIdx = ITEM_PRODUCT_IDX( Item.GetTYPE(), Item.GetItemNO() );
			if( int iRawMaterial = PRODUCT_RAW_MATERIAL( iProductIdx ) )
			{
				int iOriQual = ITEM_QUALITY( Item.GetTYPE(), Item.GetItemNO() );
	
				if( iRawMaterial >= 1000 )
				{
					MaterialItem.Init( iRawMaterial );
				}
				else
				{
					int iTemp    = ( iOriQual - 20 ) / 12;

					if( iTemp < 1 ) 	iTemp = 1;
					if( iTemp > 10 )	iTemp = 10;

					int iItemNo = ( iRawMaterial - 421 ) * 10 + iTemp;

					assert( iItemNo >= 1 && iItemNo <= g_TblNATUAL.m_nDataCnt );
					if( iItemNo < 0 || iItemNo > g_TblNATUAL.m_nDataCnt )
						return;

					MaterialItem.Init( ITEM_TYPE_NATURAL * 1000 + iItemNo );
				}

				if( MaterialItem.IsEnableDupCNT() )
					MaterialItem.m_uiQuantity = GetQuantity( Item, PRODUCT_NEED_ITEM_CNT( iProductIdx, 0 ) );


				pMaterialItem = new CItem;
				pMaterialItem->SetItem( MaterialItem );

				m_pItems.push_back( pMaterialItem );
				m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
				m_Event.SetIndex( 0 );
				m_Event.SetItem( pMaterialItem );
				SetChanged(); 
				NotifyObservers( &m_Event );

				int iMaterialIdx  = 0 ;
				int iClass = 0;
				int iIndex = 0;
				for( int i = 1 ; i < 4; ++i )
				{
					if( iMaterialIdx = PRODUCT_NEED_ITEM_NO( iProductIdx, i ) )
					{
						//iClass = ITEM_TYPE( iMaterialIdx / 1000, iMaterialIdx % 1000 );
						//if( iClass == 427 || iClass == 428 )///연금재료와 , 화학품은 없어진다.
						//	continue;

						MaterialItem.Init( iMaterialIdx );
						if( MaterialItem.IsEnableDupCNT() )
							MaterialItem.m_uiQuantity = GetQuantity( Item, PRODUCT_NEED_ITEM_CNT( iProductIdx, i) );

						pMaterialItem = new CItem;
						pMaterialItem->SetItem( MaterialItem );
						m_pItems.push_back( pMaterialItem );						
						++iIndex;
						
						m_Event.SetID( CTEventSeparate::EID_SET_OUTPUT_ITEM );
						m_Event.SetIndex( iIndex );
						m_Event.SetItem( pMaterialItem );
						SetChanged(); 
						NotifyObservers( &m_Event );
					}
				}

				m_iRequireMp = iOriQual + 30;
			}
			///		
			break;
		}
	default:
		break;
	}
}
int CSeparate::GetRequireMp()
{
	return m_iRequireMp;
}

int	CSeparate::GetQuantity( tagITEM& Item, int iRequireQuantity )
{
	int iDuration = 0;
	if( Item.HasDurability() )
		iDuration = Item.GetDurability();

	int iLife = 1000;
	if( Item.HasLife() )
		iLife = Item.GetLife();

	return iRequireQuantity * ( 130 ) * ( iDuration / 2 + iLife / 10 + 100 ) / 55000;
}

void CSeparate::Send_CRAFT_BREAKUP_REQ()
{
	if( m_bWaitServerReply ) return;

	assert( m_pMaterialItem );
	if( m_pMaterialItem )
	{
		if( m_iSkillSlotIdx && m_wNpcSvrIdx == 0 )
		{
			g_pNet->Send_cli_CRAFT_BREAKUP_REQ( 
				CRAFT_BREAKUP_USE_SKILL, 
				m_iSkillSlotIdx, 
				m_pMaterialItem->GetIndex() 
			);
			m_bWaitServerReply = true;
		}
		else if( m_wNpcSvrIdx && m_iSkillSlotIdx == 0)
		{
			g_pNet->Send_cli_CRAFT_BREAKUP_REQ( 
				CRAFT_BREAKUP_FROM_NPC, 
				m_wNpcSvrIdx, 
				m_pMaterialItem->GetIndex() 
			);
			m_bWaitServerReply = true;
		}
		else
		{
			assert( 0 && "Invalid BreakUp Req Type" );
		}
	}
}

void CSeparate::SetSkillSlotIdx( int iSkillSlotIdx )
{
	m_iSkillSlotIdx = iSkillSlotIdx;
	m_wNpcSvrIdx    = 0;
}

void CSeparate::SetNpcSvrIdx( WORD wNpcSvrIdx )
{
	m_wNpcSvrIdx    = wNpcSvrIdx;
	m_iSkillSlotIdx = 0;
}

WORD CSeparate::GetNpcSvrIdx()
{
	return m_wNpcSvrIdx;
}

int CSeparate::GetType()
{
	if( m_iSkillSlotIdx && m_wNpcSvrIdx == 0 )
		return TYPE_SKILL;
	else if( m_wNpcSvrIdx && m_iSkillSlotIdx == 0)
		return TYPE_NPC;

	return TYPE_NONE;
}
int CSeparate::GetRequireMoney()
{
	return m_iRequireMoney;
}