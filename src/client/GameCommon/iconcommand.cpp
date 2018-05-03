#include "stdafx.h"
#include "iconcommand.h"
#include "item.h"
#include "../gamedata/cdealdata.h"
#include "../interface/icon/ciconitem.h"
//bool CTCmdItemIconInBuyList::Exec( CTObject* pObj )
//{
//	if( pObj == NULL )
//	{
//		assert( pObj && "pObj is NULL @CTCmdIconInBuyList::Exec" );
//		return true;
//	}
//	CItem* pItem = ( CItem* ) pObj;
//	CDealData::GetInstance().RemoveItemFromBuyList( pItem->GetIndex() );
//	return true;
//}
//
//bool CTCmdItemIconInSellList::Exec( CTObject* pObj )
//{
//	if( pObj == NULL )
//	{
//		assert( pObj && "pObj is NULL @CTCmdIconInSellList::Exec" );
//		return true;
//	}
//	CItem* pItem = ( CItem*)pObj;
//	CDealData::GetInstance().RemoveItemFromSellList( pItem->GetIndex() );
//	return true;
//}

bool CTCmdRemoveItemInBuyList::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdRemoveItemInBuyList" );
		return true;
	}

	if( strcmp( pObj->toString(), "CIcon" ) == 0 )
		CDealData::GetInstance().RemoveItemFromBuyList( ((CIconItem*)pObj)->GetCItem() );
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
		CDealData::GetInstance().RemoveItemFromBuyList( (CItem*)pObj );
	else
		assert( 0 && "Invalid CTObject Type @CTCmdRemoveItemInBuyList" );

	return true;
}

bool CTCmdRemoveItemInSellList::Exec( CTObject* pObj )
{
	if( pObj == NULL )
	{
		assert( pObj && "pObj is NULL @CTCmdRemoveItemInSellList" );
		return true;
	}

	if( strcmp( pObj->toString(), "CIcon" ) == 0 )
		CDealData::GetInstance().RemoveItemFromSellList( ((CIconItem*)pObj)->GetCItem() );
	else if( strcmp( pObj->toString(), "CItem" ) == 0 )
		CDealData::GetInstance().RemoveItemFromSellList( (CItem*)pObj );
	else
		assert( 0 && "Invalid CTObject Type @CTCmdRemoveItemInSellList" );

	return true;
}

