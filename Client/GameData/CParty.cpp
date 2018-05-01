#include "stdafx.h"
#include ".\cparty.h"
#include "Object.h"
#include "../interface/interfacetype.h"
#include "../interface/it_mgr.h"
#include "../interface/dlgs/CMsgBox.h"
#include "../interface/Command/CTCmdHotExec.h"
#include "common/Calculation.h"
#include "../Network/CNetwork.h"
//*-------------------------------------------------------------------------------------
/// 문제 - 존서버별로 Object Server Index가 겹치는 경우
/// 1. UserTag로 Object Server Index를 바꾸었을경우, 다른 Zone Server에 있는 아바타들끼리의 index가 겹칠수 있다.
//*-------------------------------------------------------------------------------------
const int c_iAutoExpDecreaseTimeSpace = 10000;
CParty::CParty(void)
{
	m_bHasParty			= false;
	m_iLevel			= 1;
	m_iCurrExp			= 0;
	m_wLeaderObjSvrIdx	= 0;
}

CParty::~CParty(void)
{
}

CParty& CParty::GetInstance()
{
	static CParty s_Instance;
	return s_Instance;
}
//*----------------------------------------------------------------------
/// @param	memberinfo 서버와 Network에서 사용되는 struct
/// @param  pszName	아바타 이름
/// @return 추가에 성공시 true
/// @brief 파티에 파티원이 들어올때 처리하는 Method
//*----------------------------------------------------------------------
bool CParty::JoinMember( tag_PARTY_MEMBER& memberinfo, const char* pszName )
{
	assert( pszName );
	if( pszName == NULL )
		return false;

	//if( IsPartyMember( memberinfo.m_wObjectIDX ) )
	//{
	//	assert( 0 && "멤버리스트에 있는 아바타입니다.@CParty::JoinMember" );
	//	return false;
	//}

	if( memberinfo.m_wObjectIDX != g_pObjMGR->Get_ServerObjectIndex(g_pAVATAR->Get_INDEX() ))
	{
		CObjAVT* pObjAVT = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( memberinfo.m_wObjectIDX), false );

		if( pObjAVT )
		{
			
			pObjAVT->m_EndurancePack.DeleteEnduranceState( memberinfo.m_dwStatusFALG );
			//increase_maxhp = pObjAVT->m_EndurancePack.GetStateValue( ING_INC_MAX_HP );
			pObjAVT->Set_HP( memberinfo.m_nHP );
			pObjAVT->Set_MaxHP( memberinfo.m_nMaxHP );
			pObjAVT->Set_CON( memberinfo.m_nCON );
			pObjAVT->Set_AddRecoverHP( memberinfo.m_btRecoverHP );
			pObjAVT->Set_AddRecoverMP( memberinfo.m_btRecoverMP );
			pObjAVT->SetStamina( memberinfo.m_nSTAMINA );
		}
	}

	PartyMember member;

	member.m_strName = pszName;
	member.m_Info = memberinfo;

	m_Members.push_back( member );

	m_Event.SetID( CTEventParty::EID_JOIN_MEMBER );
	m_Event.SetObjSvrIdx( member.m_Info.m_wObjectIDX );
	m_Event.SetObjectTag( member.m_Info.m_dwUserTAG );
	m_Event.SetName( pszName );
	SetChanged();
	NotifyObservers( &m_Event );
	return true;
}
//*----------------------------------------------------------------------
/// @param	wObjSvrIdx 파티를 나가는 아바타의 Object Server Index;
/// @return 파티원리스트에서 찾지 못하면 false;
/// @brief 파티에 파티원이 나갈때 Method
//*----------------------------------------------------------------------
bool CParty::LeaveMember(  DWORD dwObjectTAG )
{
	std::list< PartyMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->m_Info.m_dwUserTAG == dwObjectTAG )
		{
			m_Event.SetID( CTEventParty::EID_LEAVE_MEMBER );
			m_Event.SetObjSvrIdx( iter->m_Info.m_wObjectIDX );
			SetChanged();
			NotifyObservers( &m_Event );
			m_Members.erase( iter );
			return true;
		}
	}
	assert( 0 && "Not Found Party Member @CParty::LeaveMember" );
	return false;
}

bool CParty::BanMember( DWORD dwObjectTag )
{
	///내가 Ban당했을경우
	if( dwObjectTag == g_pAVATAR->GetUniqueTag() )
	{
		g_itMGR.AppendChatMsg(STR_PARTY_BANNED, IT_MGR::CHAT_TYPE_SYSTEM );
		Leave();
		return true;
	}

	std::list< PartyMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->m_Info.m_dwUserTAG == dwObjectTag )
		{
			m_Event.SetID( CTEventParty::EID_BAN_MEMBER );
			m_Event.SetObjectTag( dwObjectTag );
			SetChanged();
			NotifyObservers( &m_Event );
			m_Members.erase( iter );
			return true;
		}
	}
	assert( 0 && "Not Found Party Member @CParty::BanMember" );
	return false;
}
//*----------------------------------------------------------------------
/// @param			wObjSvrIdx	정보를 원하는 아바타의 Object Server Index;
/// @param [out]	member		아바타의 정보를 담을 Data Reference
/// @return			bool		파티원리스트에서 찾지 못하면 false;
///
/// @brief	파티에 파티원이 나갈때 Method
//*----------------------------------------------------------------------
bool CParty::GetMemberInfoByObjSvrIdx(WORD wObjSvrIdx, PartyMember& member )
{
	std::list< PartyMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->m_Info.m_wObjectIDX == wObjSvrIdx )
		{
			member = *iter;
			return true;
		}
	}
	return false;
}	
bool CParty::GetMemberInfoByTag(DWORD dwObjectTAG, PartyMember& member )
{
	std::list< PartyMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->m_Info.m_dwUserTAG == dwObjectTAG )
		{
			member = *iter;
			return true;
		}
	}
	return false;
}
//*----------------------------------------------------------------------
/// @brief 내가 파티중인가?
//*----------------------------------------------------------------------
bool CParty::HasParty()
{
	return m_bHasParty;
}
//*----------------------------------------------------------------------
/// @brief 파티멤버가 풀인가?
//*----------------------------------------------------------------------
bool CParty::IsPartyFull()
{
	return false;
}
//*----------------------------------------------------------------------
/// @brief 현재 파티인원수 
//*----------------------------------------------------------------------
int CParty::GetMemberCount()
{
	return m_Members.size();
}
//*----------------------------------------------------------------------
/// @brief 내가 파티의 리더인가?
//*----------------------------------------------------------------------
bool CParty::IsPartyLeader()				
{
	if( g_pAVATAR->GetUniqueTag() == m_dwLeaderObjectTAG )//g_pObjMGR->Get_ServerObjectIndex(g_pAVATAR->Get_INDEX()) == m_wLeaderObjSvrIdx )
		return true;

	return false;
}
//*----------------------------------------------------------------------
/// @param wObjSvrIdx 아바타의 서버 오브젝트 인덱스
/// @brief 파티 멤버인가?
//*----------------------------------------------------------------------
bool CParty::IsPartyMember(WORD wObjSvrIdx)
{
	std::list< PartyMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->m_Info.m_wObjectIDX == wObjSvrIdx )
			return true;
	}
	return false;
}
//*----------------------------------------------------------------------
/// @param iLevel 파티 레벨
/// @brief 다음레벨업을 위해서 필요한 총 Exp
//*----------------------------------------------------------------------
int	 CParty::GetNeedExp4Levelup ( int iLevel )
{
	// return ( ( iLevel+3) * ( iLevel+5)*5 ) + 40;
	return CCal::Get_PartyLevelUpNeedEXP( iLevel );	// 2004. 7. 22 서버와 공통으로 사용하는 함수로 교체
}
//*----------------------------------------------------------------------
/// @brief 파티레벨을 구한다.
//*----------------------------------------------------------------------
int CParty::GetLevel()
{
	return m_iLevel;
}
//*----------------------------------------------------------------------
/// @brief 현재 레벨에서의 경험치를 구한다
//*----------------------------------------------------------------------
int CParty::GetCurrExp()
{
	return m_iCurrExp;
}
//*----------------------------------------------------------------------
/// @brief 현재 레벨에서의 최대경험치를 구한다( 다음레벨업을 위한 필요경험치이기도 하다 )
//*----------------------------------------------------------------------
int CParty::GetMaxExp()
{
	return GetNeedExp4Levelup( m_iLevel );
}
//*----------------------------------------------------------------------
/// @brief 내가 파티에 조인할때 처리할 Method
//*----------------------------------------------------------------------
void CParty::Join()
{
	m_bHasParty = true;
	m_Event.SetID( CTEventParty::EID_INIT );
	SetChanged();
	NotifyObservers( &m_Event );
	///자기 자신을 리스트에 추가.
	tag_PARTY_MEMBER memberinfo;
	memberinfo.m_wObjectIDX = g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() );
	memberinfo.m_dwUserTAG	= g_pAVATAR->GetUniqueTag();
	JoinMember( memberinfo, g_pAVATAR->Get_NAME() );
}

void CParty::Make()///내가 파티를 만들었을경우
{
	m_bHasParty = true;
	m_Event.SetID( CTEventParty::EID_INIT );
	SetChanged();
	NotifyObservers( &m_Event );

	SetExp( 0 );
	SetLevel( 1 );
	tag_PARTY_MEMBER memberinfo;
	memberinfo.m_wObjectIDX = g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() );
	memberinfo.m_dwUserTAG	= g_pAVATAR->GetUniqueTag();
	JoinMember( memberinfo, g_pAVATAR->Get_NAME() );
	ChangeLeaderByObjSvrIdx( g_pObjMGR->Get_ServerObjectIndex( g_pAVATAR->Get_INDEX() ) );
}
//*----------------------------------------------------------------------
/// @brief 내가 파티를 떠날때 처리할 Method
//*----------------------------------------------------------------------
void CParty::Leave()
{
	m_bHasParty = false;
	Destory();
}
//*----------------------------------------------------------------------
/// @brief 파티가 어떠한 이유로 해산되었다.
//*----------------------------------------------------------------------
void CParty::Destory()
{
	m_bHasParty			= false;
	m_iLevel			= 1;
	m_iCurrExp			= 0;
	m_wLeaderObjSvrIdx	= 0;
	m_dwLeaderObjectTAG = 0;

	std::list< PartyMember >::iterator iter;
	for( iter =	m_Members.begin(); iter != m_Members.end(); )
	{
		m_Event.SetID( CTEventParty::EID_CLEAR );
		SetChanged();
		NotifyObservers( &m_Event );


		iter = m_Members.erase( iter );
	}
}
//*----------------------------------------------------------------------
/// @brief 파티리더 변경
//*----------------------------------------------------------------------

bool CParty::ChangeLeaderByObjSvrIdx( WORD wObjSvrIdx )
{
	PartyMember member;
	if( GetMemberInfoByObjSvrIdx( wObjSvrIdx , member ) )
	{
		m_wLeaderObjSvrIdx = wObjSvrIdx;
		m_dwLeaderObjectTAG = member.m_Info.m_dwUserTAG;

		m_Event.SetID( CTEventParty::EID_CHANGE_LEADER );
		m_Event.SetObjSvrIdx( wObjSvrIdx );
		SetChanged();
		NotifyObservers( &m_Event );
		return true;
	}
	assert( 0 && "Not Found Party Member @CParty::ChangeLeader" );
	return false;
}
bool CParty::ChangeLeaderByTag( DWORD dwObjectTag )
{
	PartyMember member;
	if( GetMemberInfoByTag( dwObjectTag, member ) )
	{
		m_wLeaderObjSvrIdx	= member.m_Info.m_wObjectIDX;
		m_dwLeaderObjectTAG = dwObjectTag;

		m_Event.SetID( CTEventParty::EID_CHANGE_LEADER );
		m_Event.SetObjSvrIdx( member.m_Info.m_wObjectIDX );

		SetChanged();
		NotifyObservers( &m_Event );

		return true;
	}
	assert( 0 && "Not Found Party Member @CParty::ChangeLeader" );
	return false;
}



//*----------------------------------------------------------------------
/// @brief MemberInfo.m_dwUserTag 로 Member를 찾아서 해당 정보를 바꾸어 준다.( 존서버간 이동시 사용 )
//*----------------------------------------------------------------------
bool CParty::ChangeMemberInfoByUserTag( tag_PARTY_MEMBER& MemberInfo )
{
	std::list< PartyMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->m_Info.m_dwUserTAG == MemberInfo.m_dwUserTAG )
		{
			m_Event.SetID( CTEventParty::EID_CHANGE_OBJECT_SERVERINDEX );
			m_Event.SetObjSvrIdx( MemberInfo.m_wObjectIDX );
			m_Event.SetObjectTag( MemberInfo.m_dwUserTAG );

			iter->m_Info	= MemberInfo;

			SetChanged();
			NotifyObservers( &m_Event );
			return true;
		}
	}
	assert( 0 && "Not Found Party Member @CParty::ChangeMemberInfoByUserTag" );
	return false;
}
void CParty::SetLevel( int iLevel )
{
	m_iLevel = iLevel;
}
//*----------------------------------------------------------------------
/// @brief 서버에서 받은 경험치는 저장하고 나서 Update 시간을 현재 시간으로 set한다.
//*----------------------------------------------------------------------
void CParty::SetExp( int iExp )
{
	m_iCurrExp = iExp;
	m_dwExpLastUpdateTime = g_GameDATA.GetGameTime();
}

//*----------------------------------------------------------------------
/// @brief 파티가 레벨업이 되었을때 처리할 Method
/// @warn	1. 내자신도 리스트에 포함되어있다.MP는 별도로 나만 올려준다. ( 다른 멤버는 알수도 없다. )
///			2. 죽었을경우에는 올리지 않는다.
//*----------------------------------------------------------------------
void CParty::LevelUp()
{
	g_pAVATAR->Set_MP( g_pAVATAR->Get_MaxMP() );

//	g_pAVATAR->Set_HP( g_pAVATAR->Get_MaxHP() );

	CObjAVT* pObjAVT;
	std::list< PartyMember >::iterator iter;
	for( iter = m_Members.begin(); iter != 	m_Members.end(); ++iter )
	{
		pObjAVT = g_pObjMGR->Get_CharAVT( g_pObjMGR->Get_ClientObjectIndex( iter->m_Info.m_wObjectIDX ), true );
		if( pObjAVT )
			pObjAVT->Set_HP( pObjAVT->Get_MaxHP() );
	}
}
//*-----------------------------------------------------------------------------------//
/// @breif 파티중일경우 매프레임 Update 하는 Method
//*-----------------------------------------------------------------------------------//
void CParty::Update()
{
	if( !m_bHasParty ) return;

	DWORD dwCurrTime  = g_GameDATA.GetTime();
	DWORD dwTimeSpace =  dwCurrTime - m_dwExpLastUpdateTime;

	if( dwTimeSpace && (dwTimeSpace > c_iAutoExpDecreaseTimeSpace) )
		SetExp( m_iCurrExp -  ( GetAutoDecreaseExp( m_iLevel, GetMemberCount() ) * dwTimeSpace ) / c_iAutoExpDecreaseTimeSpace );

	/// 이하 내가 파티장일경우에만 처리하는 부분
	if( !IsPartyLeader() ) 	return;
	std::list< PartyMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->m_Info.m_wObjectIDX == 0 )
		{
			if( dwCurrTime - iter->m_dwDisconnectedTime > 5 * 60 * 1000 )
			{
				///서버에게 Ban하도록 요청한다.
				if( iter->m_Info.m_dwUserTAG == g_pAVATAR->GetUniqueTag() )
					continue;

				CTCmdSendPacketPartyReq Command( PARTY_REQ_BAN, iter->m_Info.m_dwUserTAG );
				Command.Exec( NULL );
			}
		}
	}
}
//*-----------------------------------------------------------------------------------//
/// @breif 일정시간마다 감소하는 파티 경험치를 구하는 Method
//*-----------------------------------------------------------------------------------//
int CParty::GetAutoDecreaseExp( int iPartyLv, int iCountPartyMember )
{
	if( iCountPartyMember <= 0 )
		return 0;

	return iPartyLv / 2 + iCountPartyMember;
}
//*-----------------------------------------------------------------------------------//
/// @breif 현재 내가 파티에 들어갈수 있는가?
//*-----------------------------------------------------------------------------------//
bool CParty::IsValidJoinParty()
{
	if( g_pAVATAR && g_pAVATAR->Get_HP() <= 0 )
		return false;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DEAL ) )
		return false;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_EXCHANGE ) )
		return false;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_DIALOG ))
		return false;

	if( g_itMGR.IsDlgOpened( DLG_TYPE_MAKE ))
		return false;

	if( HasParty() )
		return false;

	if( g_itMGR.FindMsgBox( CMsgBox::MSGTYPE_RECV_PARTY_REQ ) )
		return false;

	return true;
}

WORD CParty::GetLeaderSvrIndex()
{
	return 	m_wLeaderObjSvrIdx;
}

DWORD CParty::GetLeaderObjectTAG()
{
	return m_dwLeaderObjectTAG;
}

void CParty::SetLeaderObjSvrIdx( WORD wObjSvrIdx )
{
	m_wLeaderObjSvrIdx = wObjSvrIdx;
}

void CParty::SetLeaderObjectTag( DWORD dwObjectTag )
{
	m_dwLeaderObjectTAG = dwObjectTag;
}

//*-----------------------------------------------------------------------------------//
/// @breif 파티원이 비정상 종료되었다
//*-----------------------------------------------------------------------------------//
void CParty::MemberDisconnected( DWORD dwUserTag )
{
	std::list< PartyMember >::iterator iter;
	for( iter = m_Members.begin(); iter != m_Members.end(); ++iter )
	{
		if( iter->m_Info.m_dwUserTAG == dwUserTag )
		{
			iter->m_Info.m_wObjectIDX = 0;
			iter->m_dwDisconnectedTime = g_GameDATA.GetTime();
			m_Event.SetID( CTEventParty::EID_MEMBER_DISCONNECTED );
			m_Event.SetObjectTag( dwUserTag );
			m_Event.SetObjSvrIdx(0);

			SetChanged();
			NotifyObservers( &m_Event );
			return;
		}
	}
	assert( 0 && "Not Found Party Member @CParty::MemberDisconnected" );
}

void CParty::SendChangePartyRule( BYTE btPartyRule )
{	
	if( m_btPartyRule != btPartyRule )
	{
		m_btPartyRule = btPartyRule;
		g_pNet->Send_cli_PARTY_RULE( btPartyRule );
	}
}

void CParty::RecvPartyRule( BYTE btPartyRule )
{
	m_btPartyRule = btPartyRule;

	m_Event.SetID( CTEventParty::EID_CHANGE_RULE );
	m_Event.SetRule( btPartyRule );

	SetChanged();
	NotifyObservers( &m_Event );

	///UI들에게 알린다.
}