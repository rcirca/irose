#include "stdafx.h"
#include ".\cquestlistitem.h"
#include "../../../Game.h"
#include "../../../common/io_quest.h"
#include "../../CToolTipMgr.h"
#include "../../IO_ImageRes.h"
#include "../../CTDrawImpl.h"


#include "ZListBox.h"
CQuestListItem::CQuestListItem( int iQuestIndex )
{
	m_iQuestIndex = iQuestIndex;
	SetHeight( 24 );
	SetWidth( 174 );

	const RECT rect = { 27, 5, 170, 24 };
	m_title.set_string( QUEST_NAME( m_iQuestIndex ), rect,FONT_NORMAL);
}

CQuestListItem::~CQuestListItem(void)
{

}

void CQuestListItem::Draw()
{
	CWinCtrl::Draw();

	///이름 출력
	///일단 아이콘이 어떤것이 그려지는지 모르니까..Trasform하고 제목만 출력하자.

	if( QUEST_ICON( m_iQuestIndex ) )
		g_DrawImpl.Draw( m_sPosition.x + 3, m_sPosition.y + 1, IMAGE_RES_STATE_ICON, QUEST_ICON( m_iQuestIndex ) );


	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x , (float)m_sPosition.y ,0.0f);
	::setTransformSprite( mat );

	if( IsSelected() )
		m_title.set_color( g_dwYELLOW );
	else
		m_title.set_color( D3DCOLOR_RGBA(255,255,255,255) );

	m_title.draw();
}

unsigned CQuestListItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsInside( LOWORD(lParam), HIWORD( lParam )) ) return 0;

	if( uiMsg == WM_LBUTTONDOWN )
	{
		SetSelected();
		return GetControlID();
	}
	
	return 0;
}

void CQuestListItem::Update( POINT ptMouse )
{
	if( IsInside( ptMouse.x, ptMouse.y ) )
	{
		if( m_title.is_ellipsis() )
			CToolTipMgr::GetInstance().RegToolTip( m_sPosition.x + 10, m_sPosition.y - 18, m_title.get_string() );
	}
}

/// 부모가 있다면 일단 ZListBox라고 가정한다
/// 추후 EventHandler, Listener, Action등을 구현해야 한다.
void CQuestListItem::SetSelected()
{
	if( m_pParent )
	{
		if( m_pParent->GetControlType() == CTRL_ZLISTBOX )
		{
			CZListBox* pList = ( CZListBox*)m_pParent;
			pList->DeselectAll();
		}
	}
	CWinCtrl::SetSelected();
}