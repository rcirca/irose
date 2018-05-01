#include "stdafx.h"
#include ".\iteminfoview.h"
#include <algorithm>
#include <functional>

#include "../CObjGROUND.h"
#include "../GameCommon/Item.h"


void DeleteDropItemInfo( CDropItemInfo* pInfo )
{
	if( pInfo )
	{
		delete pInfo;
		pInfo = NULL;
	}
}



//----------------------------------------------------------------------------------------------------
/// class CDropItemInfo
/// 드랍아이템의 정보를 출력할 심플 컨터롤
//----------------------------------------------------------------------------------------------------
CDropItemInfo::CDropItemInfo()
{
}

CDropItemInfo::~CDropItemInfo()
{
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 클래스 데이터 맴버 초기화
//----------------------------------------------------------------------------------------------------

void CDropItemInfo::Reset()
{
	m_pDropItem = NULL;
	m_Info.Clear();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 아이템 정보 세팅
//----------------------------------------------------------------------------------------------------

void CDropItemInfo::SetDropItemInfo( CObjITEM* pItem )
{
	Reset();

	m_pDropItem = pItem;

	m_Info.AddString( m_pDropItem->Get_NAME(), CItem::GetItemNameColor( pItem->m_ITEM.GetTYPE(), pItem->m_ITEM.GetItemNO()  ) );

	D3DVECTOR PosSCR;
	m_pDropItem->GetScreenPOS( PosSCR );
	POINT pt = { PosSCR.x, PosSCR.y };
	m_Info.SetPosition( pt );
}

void CDropItemInfo::Draw()
{
	m_Info.Draw();
}







CItemInfoView::CItemInfoView(void)
{	
}

CItemInfoView::~CItemInfoView(void)
{
}

void CItemInfoView::ReserverDropItemPool( int iPoolSize )
{
	for( int i = 0; i < iPoolSize ; i++ )
	{
		CDropItemInfo* pInfo = new CDropItemInfo;
		m_DropItemInfoPool.push_back( pInfo );
	}
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 모든 디스플레이 리스트 클리어. 그리고 풀도 클리어
//----------------------------------------------------------------------------------------------------

void CItemInfoView::Clear()
{
	std::for_each( m_DropItemInfoPool.begin(), m_DropItemInfoPool.end(), DeleteDropItemInfo );	
	m_DropItemInfoPool.clear();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 뷰리스트 정리
//----------------------------------------------------------------------------------------------------

void CItemInfoView::Reset()
{
	m_DropItemInfoPool.insert( m_DropItemInfoPool.begin(), m_ViewedItemList.begin(), m_ViewedItemList.end() );
	m_ViewedItemList.clear();
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 새로운 아이템 정보를 추가한다.
//----------------------------------------------------------------------------------------------------

bool CItemInfoView::PushDropItemInfo( CObjITEM* pItem )
{
	if( pItem == NULL )
		return false;

	CDropItemInfo* pInfo = NULL;
	if( m_DropItemInfoPool.empty() )
	{
		pInfo = new CDropItemInfo;
	}else
	{
		pInfo = m_DropItemInfoPool.back();
		m_DropItemInfoPool.pop_back();
	}
	
	pInfo->SetDropItemInfo( pItem );
	m_ViewedItemList.push_back( pInfo );

	return true;
}

//----------------------------------------------------------------------------------------------------
/// @param
/// @brief 모든 디스플레이 리스트 랜더링
//----------------------------------------------------------------------------------------------------

void CItemInfoView::Draw()
{
	int i = m_ViewedItemList.size();

	std::for_each( m_ViewedItemList.begin(), m_ViewedItemList.end(), 
		std::mem_fun< void, CDropItemInfo >( &CDropItemInfo::Draw ) );
}
