#include "stdafx.h"
#include ".\cchatroomlistitem.h"
#include "../../../Game.h"
#include "../../it_mgr.h"
#include "../../../GameData/CChatRoom.h"
#include "tgamectrl/zlistbox.h"

CChatRoomListItem::CChatRoomListItem( WORD wID, BYTE btRoomType, BYTE btUserCount, const char* pszTitle )
{
	SetHeight( 20 );
	SetWidth( 174 );
	m_wID = wID;
	m_btRoomType = btRoomType;
	m_btUserCount = btUserCount;
	assert( pszTitle );
	if( pszTitle )
		m_strTitle = pszTitle;
}

CChatRoomListItem::~CChatRoomListItem(void)
{

}

void CChatRoomListItem::Draw()
{
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, (float)m_sPosition.x, (float)m_sPosition.y,0.0f);
	::setTransformSprite( mat );

	::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL_BOLD ], 
		true, 
		0, 0, 
		(IsSelected())?g_dwYELLOW:g_dwBLACK, 
		m_strTitle.c_str() );
}

unsigned CChatRoomListItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsInside( LOWORD(lParam), HIWORD( lParam )) ) return 0;

	switch( uiMsg )
	{
	case WM_LBUTTONDOWN:
		SetSelected();
		return GetControlID();
	case WM_LBUTTONDBLCLK:
		if( !g_itMGR.IsDlgOpened( DLG_TYPE_CHATROOM ) )
			CChatRoom::GetInstance().SendReqJoinRoom( m_btRoomType, m_wID, NULL );
		break;
	default:
		break;
	}
	
	return 0;
}
void CChatRoomListItem::SetSelected()
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
