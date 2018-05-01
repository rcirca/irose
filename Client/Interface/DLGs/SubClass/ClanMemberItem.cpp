#include "stdafx.h"
#include ".\clanmemberitem.h"
#include "ZListBox.h"
#include "../../../Game.h"
#include "../../../GameCommon/StringManager.h"
#include "../../../GameData/CClan.h"
#include "../../CTDrawImpl.h"
#include "../../IO_ImageRes.h"
#include "../../it_mgr.h"
#include "../../CToolTipMgr.h"
#include "../CClanDlg.h"
#include "../../TypeResource.h"
#include "ResourceMgr.h"

CClanMemberItem::CClanMemberItem( const char* pszName, int iClass ,int iClanPoint, int iChannelNo, short nLevel, short nJob )
{
	assert( pszName );
	if( pszName  )
	{
		m_strName	= pszName;
		m_iClass	= iClass;
		m_strClass = CStringManager::GetSingleton().GetClanClass( m_iClass );
		m_iClanPoint = iClanPoint;
		m_iChannelNo		= iChannelNo;
		if( iChannelNo != 0xff )
		{
			m_strChannel = CStr::Printf( "Channel-%d", iChannelNo );
			SetLevel( nLevel );
			SetJob( nJob );
		}
	}
	SetWidth( 200 );
	SetHeight( 18 );

	m_iOnlineMarkImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"CLAN01_MARK_ONLINE" );
	m_iOfflineMarkImageID = CResourceMgr::GetInstance()->GetImageNID( IMAGE_RES_UI,"CLAN01_MARK_OFFLINE" );

}

CClanMemberItem::~CClanMemberItem(void)
{

}

unsigned CClanMemberItem::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsInside( LOWORD(lParam), HIWORD( lParam )) ) return 0;
	switch( uiMsg )
	{
	case WM_LBUTTONDOWN:
		{
			CTDialog* pDlg = g_itMGR.FindDlg( DLG_TYPE_CLAN );
			if( pDlg )
			{
				CClanDlg* pClanDlg = (CClanDlg*)pDlg;
				pClanDlg->SetSelectedMember( m_iClanPoint );
			}
			SetSelected();
		}
		return GetControlID();
	default:
		break;
	}
	return 0;
}

void CClanMemberItem::SetSelected()
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

void CClanMemberItem::SetConnectStatus( int iChannelNo )
{
	m_iChannelNo		= iChannelNo;
	if( iChannelNo == 0xff )
	{
		m_strChannel.clear();
		g_itMGR.AppendChatMsg( CStr::Printf( FSTR_FRIEND_LOGOUT, m_strName.c_str() ), IT_MGR::CHAT_TYPE_CLAN );
		//님이 접속종료 하였습니다
	}
	else
	{
		m_strChannel = CStr::Printf( "Channel-%d", iChannelNo );
		g_itMGR.AppendChatMsg( CStr::Printf( FSTR_FRIEND_LOGIN, m_strName.c_str() ), IT_MGR::CHAT_TYPE_CLAN );
		//님이 접속하였습니다
	}
}

void CClanMemberItem::Draw()
{
	DWORD dwColor = g_dwBLACK;


	if( m_iClass <= CClan::CLAN_PENALTY )
		dwColor = g_dwRED;

	if( IsSelected() )
		dwColor = g_dwYELLOW;

	if( m_strChannel.empty() )
		g_DrawImpl.Draw( m_sPosition.x, m_sPosition.y, IMAGE_RES_UI, m_iOfflineMarkImageID );
	else
		g_DrawImpl.Draw( m_sPosition.x, m_sPosition.y, IMAGE_RES_UI, m_iOnlineMarkImageID );

	::drawFontf( g_GameDATA.m_hFONT[ FONT_SMALL ], true, 20, 2, dwColor, "%s(%s)",m_strName.c_str(), m_strClass.c_str() );
}

const char* CClanMemberItem::GetName()
{
	return m_strName.c_str();
}

void CClanMemberItem::SetClass( int iClass )
{
	m_iClass = iClass;
	m_strClass = CStringManager::GetSingleton().GetClanClass( m_iClass );
}

int CClanMemberItem::GetClass()
{
	return m_iClass;
}

int CClanMemberItem::GetClanPoint()
{
	return m_iClanPoint;
}

int CClanMemberItem::GetChannelNo()
{
	return m_iChannelNo;
}

void CClanMemberItem::Update( POINT ptMouse )
{
	CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog();
	if( pDlg && pDlg->GetDialogType() != DLG_TYPE_CLAN )
		return;

	if( IsInside( ptMouse.x, ptMouse.y) && m_iChannelNo != 0xFF)
	{
		m_Info.Clear();
		
		POINT ptDraw = m_sPosition;
		ptDraw.y	-= 50;
		ptDraw.x	+= 25;

		m_Info.SetPosition( ptDraw );
		m_Info.AddString( CStr::Printf("%s: %d", CStringManager::GetSingleton().GetAbility( AT_LEVEL ), m_nLevel ) );
		m_Info.AddString( CStr::Printf("%s: %s", CStringManager::GetSingleton().GetAbility( AT_CLASS ), m_strJob.c_str() ) );
		m_Info.AddString( CStr::Printf("%s: %d","Channel", m_iChannelNo ) );

		CToolTipMgr::GetInstance().RegistInfo( m_Info );
	}

}

void CClanMemberItem::SetLevel( short nLevel )
{
	m_nLevel = nLevel;
}

void CClanMemberItem::SetJob( short nJob )
{
	m_nJob = nJob;
	m_strJob = CStringManager::GetSingleton().GetJobName( nJob );
}

short CClanMemberItem::GetJob()
{
	return m_nJob;
}

short CClanMemberItem::GetLevel()
{
	return m_nLevel;
}