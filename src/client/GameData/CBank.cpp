#include "stdafx.h"
#include ".\cbank.h"
//#include "../gamecommon/citemequip.h"
//#include "../gamecommon/citemuse.h"
//#include "../gamecommon/citemetc.h"
//#include "../gamecommon/citempat.h"
#include "../gamecommon/item.h"
#include "../common/cuserdata.h"



CBank::CBank(void)
{
	m_Money = 0;
}

CBank::~CBank(void)
{

}

CBank& CBank::GetInstance()
{
	static CBank s_Instance;
	return s_Instance;
}

void CBank::SetItem( int iIndex, tagITEM& Item )
{
	std::map< int, CItem* >::iterator iter;
	
	CItem* pItem = NULL;
	iter = m_Items.find( iIndex );
	if( iter != m_Items.end() )
	{
		pItem = iter->second;

		tagITEM TempItem;
		TempItem.Clear();
		pItem->SetItem( TempItem );

		SetChanged();
		NotifyObservers( pItem );

		m_Items.erase( iter );
		delete pItem;
	}
	
	if( !Item.IsEmpty() )
	{
		if( pItem = new CItem )
		{
			pItem->SetIndex( iIndex );
			pItem->SetItem( Item );
			m_Items.insert( std::map< int, CItem* >::value_type( iIndex, pItem ) );
			SetChanged();
			NotifyObservers( pItem );
		}
	}
}

void   CBank::ClearItems()
{
	std::map< int, CItem* >::iterator iter;
	
	tagITEM DelItem;
	DelItem.Clear();
	CItem* pItem = NULL;
	for( iter = m_Items.begin(); iter != m_Items.end();)
	{
		pItem = iter->second;
		pItem->SetItem( DelItem );

		SetChanged();
		NotifyObservers( pItem );
		
		iter = m_Items.erase( iter );
		delete pItem;
	}
}

//CItem* CBank::CreateItem( tagITEM& Item )
//{
//	CItem* pItem = NULL;
//	switch( Item.GetTYPE() )
//	{
//	case ITEM_TYPE_FACE_ITEM:
//	case ITEM_TYPE_HELMET:
//	case ITEM_TYPE_ARMOR:
//	case ITEM_TYPE_GAUNTLET:
//	case ITEM_TYPE_BOOTS:
//	case ITEM_TYPE_KNAPSACK:
//	case ITEM_TYPE_JEWEL:
//	case ITEM_TYPE_WEAPON:
//	case ITEM_TYPE_SUBWPN:
//		pItem = new CItemEquip;
//		break;
//	case ITEM_TYPE_USE:
//		pItem = new CItemUse;
//		break;
//	case ITEM_TYPE_ETC:
//	case ITEM_TYPE_NATURAL:
//		pItem = new CItemEtc;
//		break;
//	case ITEM_TYPE_RIDE_PART:
//		pItem = new CItemPat;
//		break;
//	default:
//		assert( 0 && "Invalid Item Type @CBank::CreatItem" );
//		break;
//	}
//
//	return pItem;	
//}

void CBank::SetNpcClientIndex( int iIndex )
{
	m_iNpcClientIndex = iIndex;
}

int	CBank::GetNpcClientIndex()
{
	return m_iNpcClientIndex;
}

bool CBank::HasEmptySlot(  bool bPlatinum )
{
	///나중에 정확하게 처리하자.
	//if( bPlatinum )
	//{

	//}
	//else
	//{

	//}
	//if( m_Items.size() < BANKSLOT_TOTAL_SIZE /*g_iSlotCountPerPage * g_iPageCount*/ )
	//	return true;


	return true;
}

void CBank::SetMoney( __int64 money )
{
	m_Money = money;
}
__int64 CBank::GetMoney()
{
	return m_Money;
}
