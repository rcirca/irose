#include "stdafx.h"
#include ".\cdragndropmgr.h"
#include "CDragItem.h"
#include "Icon/CIcon.h"
#include "icon/ciconitem.h"
#include "../gamecommon/item.h"
#include "../gamecommon/CObservable.h"
#include "../GameData/Event/CTEventItem.h"
#include "../GameData/Event/CTEventAvatarStore.h"

#include "tgamectrl/tcommand.h"

CDragNDropMgr::CDragNDropMgr(void)
{
}

CDragNDropMgr::~CDragNDropMgr(void)
{
}

CDragNDropMgr& CDragNDropMgr::GetInstance()
{
	static CDragNDropMgr s_Instance;
	return s_Instance;
}

void CDragNDropMgr::Draw( POINT ptMouse )
{
	if( m_pItem )
	{
		CIcon* pIcon = m_pItem->GetIcon();
		if( pIcon )
		{
			ptMouse.x -= pIcon->GetWidth() / 2;
			ptMouse.y -= pIcon->GetHeight() / 2;
			pIcon->SetPosition( ptMouse );		
			pIcon->Draw();
		}
	}
}

void CDragNDropMgr::DragStart( CDragItem* pItem )
{
	assert( pItem );
	assert( pItem->GetIcon() );
	m_pItem = pItem;
}

void CDragNDropMgr::DragEnd( int iTargetType )
{
	if( m_pItem )
	{
		const std::map< int , CTCommand* >& Targets = m_pItem->GetTargets();
		std::map< int , CTCommand* >::const_iterator iter;
		CTCommand* pCmd = NULL;
		iter = Targets.find( iTargetType );
		
		if( iter != Targets.end() )
		{
			pCmd = iter->second;
			if( pCmd )
				pCmd->Exec( m_pItem->GetIcon() );
		}
		else
		{
			iter = Targets.find( CDragItem::TARGET_ALL );
			if( iter != Targets.end() )
			{
				pCmd = iter->second;
				if( pCmd )
					pCmd->Exec( m_pItem->GetIcon() );
				else
					m_pItem->GetIcon()->ExecuteCommand();
			}
		}
		m_pItem->SetIcon( NULL );
		m_pItem = NULL;
	}
}
bool CDragNDropMgr::IsDraging()
{
	if( m_pItem )
		return true;
	return false;
}



/**
*
* ÇöÀç Observalble - CItemSlot, CAvatarStoreDlg
*
*/
void CDragNDropMgr::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	
	if( !pObservable->HasChanged() )
		return;
	
	if( m_pItem == NULL )
		return;

	if( m_pItem->GetIcon() == NULL )
		return;


	if( pObj )
	{
		if( strcmp( pObj->toString(), "CTEventItem" ) == 0 )
		{
			CTEventItem* pEvent = (CTEventItem*)pObj;
			int iIndex = pEvent->GetIndex();
			if( pEvent->GetID() == CTEventItem::EID_DEL_ITEM )
			{
				if( iIndex == ((CIconItem*)m_pItem->GetIcon())->GetIndex() )
				{
					m_pItem->SetIcon( NULL );
					m_pItem = NULL;
				}
			}
		}
		else if( strcmp( pObj->toString(), "AvatarStore" ) == 0 )
		{
			CTEventAvatarStore* pEvent = (CTEventAvatarStore*)pObj;
			int iIndex = pEvent->GetIndex();
			if( pEvent->GetID() == CTEventAvatarStore::EID_DELETE_ITEM_SELLLIST )
			{
				if( iIndex == ((CIconItem*)m_pItem->GetIcon())->GetIndex() )
				{
					m_pItem->SetIcon( NULL );
					m_pItem = NULL;
				}
			}
		}
	}
	else
	{
		assert( 0 && "CTEvent is NULL or Invalid Type@CDragNDropMgr::Update" );
	}
}
///*--------------------------------------------------------------------------------*/