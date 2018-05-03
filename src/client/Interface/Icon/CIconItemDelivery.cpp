#include "stdafx.h"
#include ".\ciconitemdelivery.h"
#include "../../Common/CItem.h"
#include "../../GameCommon/Item.h"
#include "../io_imageres.h"
#include "../CTDrawImpl.h"
#include "../TypeResource.h"
#include "tgamectrl/resourcemgr.h"

int CIconItemDelivery::s_selected_icon = 0;

CIconItemDelivery::CIconItemDelivery(void)
{
}

CIconItemDelivery::CIconItemDelivery( tagITEM* pItem , const char* pszFrom ) :CIconItem( pItem )
{
	m_border_grahicid = CResourceMgr::GetInstance()->GetImageNID(IMAGE_RES_UI ,"UI13_BORDER_SELECTED_ITEM" );
	if( pszFrom  )
		m_from = pszFrom;
	else
		m_from.clear();

//	if( m_pItem )
//		m_pItem->SetEnable();
}

CIconItemDelivery::~CIconItemDelivery(void)
{
	if( s_selected_icon == GetCItem()->GetIndex() )
		s_selected_icon = 0;
}

unsigned CIconItemDelivery::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	POINT pt = { LOWORD( lParam ), HIWORD( lParam ) };
	if( uiMsg == WM_LBUTTONDOWN && IsInside( pt ) )
		s_selected_icon = GetCItem()->GetIndex();

	return 0;
}

/// 선택된 아이콘에 대해서 테두리 이미지를 더 그려준다.
void CIconItemDelivery::Draw()
{
	CIconItem::Draw();
	if( s_selected_icon == GetCItem()->GetIndex())
		g_DrawImpl.Draw( (int)m_ptPosition.x, (int)m_ptPosition.y, IMAGE_RES_UI  , m_border_grahicid );
}

void CIconItemDelivery::GetToolTip( CInfo& strToolTip,  DWORD dwDialogType, DWORD dwType )
{
	CIconItem::GetToolTip( strToolTip, dwDialogType, dwType );
	if( !m_from.empty() )
		strToolTip.AddString( CStr::Printf("%s:%s",STR_SENDER, m_from.c_str() ) , g_dwGREEN );
		
}
