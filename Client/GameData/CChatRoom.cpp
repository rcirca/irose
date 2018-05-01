#include "stdafx.h"
#include ".\cchatroom.h"
#include "../Object.h"
#include "../Network/CNetwork.h"

#include "../System/CGame.h"
#include "../Country.h"
#include "../Interface/it_MGR.h"

CChatMember::CChatMember(WORD wServerIdx, const char* pszName )
{
	m_wServerIdx = wServerIdx;
	assert( pszName );
	if( pszName )
		m_strName = pszName;
}
void CChatMember::SetServerIdx( WORD wServerIdx )
{
	m_wServerIdx = wServerIdx;
}
void CChatMember::SetName( const char* pszName )
{
	assert( pszName );
	if( pszName )
		m_strName = pszName;
}
WORD CChatMember::GetServerIdx()
{
	return m_wServerIdx;
}
const char* CChatMember::GetName()
{
	return m_strName.c_str();
}
//*--------------------------------------------------------------------------//
//*--------------------------------------------------------------------------//
//*--------------------------------------------------------------------------//
CChatRoom::CChatRoom(void)
{
	m_iState = STATE_DEACTIVATED;
}

CChatRoom::~CChatRoom(void)
{

}

CChatRoom& CChatRoom::GetInstance()
{
	static CChatRoom s_Instance;
	return s_Instance;
}

int	CChatRoom::GetState()
{
	return m_iState;
}
void CChatRoom::SetState( int iState )
{
	m_iState = iState;
}
//*----------------------------------------------------------------------//
/// 새로운 유저가 들어온다.
//*----------------------------------------------------------------------//
void CChatRoom::JoinMember( WORD wServerIdx, const char* pszName )
{
	CChatMember NewMember;
	NewMember.SetServerIdx( wServerIdx );
	NewMember.SetName( pszName );
	m_Members.push_back( NewMember );


	m_Event.SetID( CTEventChatRoom::EID_JOIN_MEMBER );
	m_Event.SetName( pszName );
	m_Event.SetServerIdx( wServerIdx );

	SetChanged();
	NotifyObservers( &m_Event );
}
//*----------------------------------------------------------------------//
/// 기존 유저가 나간다.
//*----------------------------------------------------------------------//
void CChatRoom::LeaveMember( WORD wServerIdx )
{
	std::list< CChatMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->GetServerIdx() == wServerIdx )
		{
			m_Event.SetID( CTEventChatRoom::EID_LEAVE_MEMBER );
			m_Event.SetServerIdx( wServerIdx );

			SetChanged();
			NotifyObservers( &m_Event );

			m_Members.erase( iter );
			break;
		}
	}
}

void CChatRoom::LeaveMember( const char* pszName )
{
	assert( pszName );
	if( pszName == NULL ) return;

	std::list< CChatMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( strcmpi( iter->GetName(), pszName ) == 0 )
		{
			m_Event.SetID( CTEventChatRoom::EID_LEAVE_MEMBER );
			m_Event.SetServerIdx( iter->GetServerIdx() );

			SetChanged();
			NotifyObservers( &m_Event );

			m_Members.erase( iter );
			break;
		}
	}
}
//*----------------------------------------------------------------------//
/// 내가 기존 채팅방에 조인
//*----------------------------------------------------------------------//
void CChatRoom::Join()
{
	///UI상태는 나는 일반 채팅 멤버
	m_iState = STATE_ACTIVATED;
	m_Event.SetID( CTEventChatRoom::EID_STATE_ACTIVATED );
	SetChanged();
	NotifyObservers( &m_Event );
}
//*----------------------------------------------------------------------//
/// 내가 새로운 채팅방을 만듬
//*----------------------------------------------------------------------//
void CChatRoom::Made( WORD wMyID )
{
	JoinMember( wMyID,
		g_pAVATAR->Get_NAME() );

	m_iState = STATE_ACTIVATED;

	m_Event.SetID( CTEventChatRoom::EID_STATE_ACTIVATED );
	SetChanged();
	NotifyObservers( &m_Event );
	///UI상태는 내가 방장
}
//*----------------------------------------------------------------------//
/// 내가 채팅방을 나간다.
//*----------------------------------------------------------------------//
void CChatRoom::Leave()
{
	std::list< CChatMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end();)
	{
		m_Event.SetID( CTEventChatRoom::EID_LEAVE_MEMBER );
		m_Event.SetServerIdx( iter->GetServerIdx() );

		SetChanged();
		NotifyObservers( &m_Event );

		iter = m_Members.erase( iter );
	}

	m_iState = STATE_DEACTIVATED;
	m_Event.SetID( CTEventChatRoom::EID_STATE_DEACTIVATED );
	SetChanged();
	NotifyObservers( &m_Event );
	
	g_pNet->Send_tag_CHAT_HEADER( CHAT_REQ_LEFT );

	m_strTitle.clear();
}

void CChatRoom::SendReqMakeRoom( BYTE btRoomType, BYTE btMaxUser, char* pszTitle, char* pszPwd )
{

	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_CHATROOM );
			return;
		}        
	}

	assert( m_iState == STATE_DEACTIVATED );	
	if( m_iState == STATE_DEACTIVATED )
	{
		g_pNet->Send_cli_CHAT_ROOM_MAKE( btRoomType, btMaxUser, pszTitle, pszPwd );
		SetTitle( pszTitle );
		m_iState = STATE_WAIT_MAKE;
	}
}

void CChatRoom::SendReqJoinRoom( BYTE btRoomType, WORD wRoomID, char* pszPwd )
{

	if(CCountry::GetSingleton().IsJapan())
	{
		if( !(CGame::GetInstance().GetPayType() & PLAY_FLAG_COMMUNITY) )
		{
			g_itMGR.OpenMsgBox( STR_JP_BILL_CANT_USE_CHATROOM );
			return;
		}        
	}	

	assert( m_iState == STATE_DEACTIVATED );
	if( m_iState == STATE_DEACTIVATED )
	{
		g_pNet->Send_cli_CHAT_ROOM_JOIN( btRoomType, wRoomID, pszPwd );
		m_iState = STATE_WAIT_JOIN;
	}
}

void CChatRoom::SetMaster( WORD wServerIdx )
{
	m_Event.SetID( CTEventChatRoom::EID_SET_MASTER );
	m_Event.SetServerIdx( wServerIdx );
	SetChanged();
	NotifyObservers( &m_Event );
}
void CChatRoom::SetTitle( const char* pszTitle )
{
	assert( pszTitle );
	if( pszTitle )
		m_strTitle = pszTitle;
}
const char* CChatRoom::GetTitle()
{
	return m_strTitle.c_str();
}