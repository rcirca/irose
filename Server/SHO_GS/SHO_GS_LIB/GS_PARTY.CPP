
#include <stdAFX.h>
#include "GS_PARTY.h"
#include "GS_ThreadZONE.h"
#include "GS_ListUSER.h"
#include "GS_ThreadLOG.h"

//-------------------------------------------------------------------------------------------------
/*
CPartyBUFF::CPartyBUFF (UINT uiInitDataCNT, UINT uiIncDataCNT) : CDataPOOL< CParty > ( "CPartyBUFF", uiInitDataCNT, uiIncDataCNT )
{
}
*/
CPartyBUFF::CPartyBUFF( WORD wMaxPartyCNT ) : CIndexARRAY< CParty* > ( "CPartyBUFF", wMaxPartyCNT ), m_csPartyBUFF( 4000 )
{
	m_pPartyBUFF = new CParty[ wMaxPartyCNT ];
	for (WORD wI=0; wI<wMaxPartyCNT; wI++) 
		m_pPartyBUFF[ wI ].m_wPartyWSID = wI;
}
CPartyBUFF::~CPartyBUFF ()
{
	SAFE_DELETE_ARRAY( m_pPartyBUFF );
}

//-------------------------------------------------------------------------------------------------
bool CPartyBUFF::CreatePARTY ( classUSER *pOwner )
{
	CParty *pCParty = this->Pool_Alloc ();
	if ( pCParty ) {
		pCParty->InitPARTY();
		// pCParty->m_nMinUserLEV = pCParty->m_nMaxUserLEV = pOwner->Get_LEVEL();

		pOwner->m_pPartyBUFF = pCParty;

		tagPartyUSER *pPU;

#ifdef	__USE_ARRAY_PARTY_USER
		pOwner->m_nPartyPOS = 0;

		pPU = new tagPartyUSER;
		pCParty->m_pPartyUSERs[ 0 ] = pPU;
#else
		x
#endif

		pPU->m_dwDBID = pOwner->m_dwDBID;
		pPU->m_pUSER  = pOwner;
		pPU->m_nLevel = pOwner->Get_LEVEL();
		pPU->m_NameCHAR.Set( pOwner->Get_NAME() );

		pCParty->m_iAverageLEV = pCParty->m_iTotalLEV = pOwner->Get_LEVEL();
		pCParty->m_dwLastKillTIME = pOwner->GetZONE()->GetCurrentTIME();

		return true;
	}
	return false;
}
void CPartyBUFF::DeletePARTY (CParty *pCParty)
{
	// 동시에 DeletePARTY호출됮 못하도록 :: 2004. 10. 05 :: 5-3 섭 다운 원인... 0x8d050
	this->m_csPartyBUFF.Lock ();

	if ( this->m_pINDEX[ pCParty->m_wPartyWSID ] ) {
		pCParty->Lock ();
		{
	#ifdef	__USE_ARRAY_PARTY_USER
			for (short nI=0; nI<pCParty->GetMemberCNT(); nI++) {
				if ( NULL == pCParty->m_pPartyUSERs[ nI ] ) {
					g_pThreadLOG->When_SysERR( __FILE__, nI, "DeletePARTY:: NULL == pCParty->m_pPartyUSERs[ nI ]" );
					continue;
				}
				
				if ( pCParty->m_pPartyUSERs[ nI ]->m_pUSER ) {
					pCParty->m_pPartyUSERs[ nI ]->m_pUSER->m_pPartyBUFF = NULL;
					pCParty->m_pPartyUSERs[ nI ]->m_pUSER->Send_gsv_PARTY_REPLY( 0, PARTY_REPLY_DESTROY );
				}

				SAFE_DELETE( pCParty->m_pPartyUSERs[ nI ] );
			}
	#else
			x
	#endif
		}
		pCParty->Unlock ();

		this->Pool_Free( pCParty );
	}
	this->m_csPartyBUFF.Unlock ();
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define	MAX_PARTY_WAIT_TIME		( 5 * 60 * 1000 )	// 5 min
void CParty::OnConnect (classUSER *pUSER)
{
	this->Lock ();	
	{
#ifdef	__USE_ARRAY_PARTY_USER
		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] ) {
				g_pThreadLOG->When_SysERR( __FILE__, nI, "OnConnect:: NULL == this->m_pPartyUSERs[ nI ]" );
				continue;
			}

			if ( this->m_pPartyUSERs[ nI ]->m_dwDBID == pUSER->m_dwDBID ) {
				// 접속 끊긴후 아직 파티에 남아 있다.
				pUSER->m_pPartyBUFF = this;
				pUSER->m_nPartyPOS = nI;

				this->m_pPartyUSERs[ nI ]->m_pUSER = pUSER;
				this->m_pPartyUSERs[ nI ]->m_dwDisconnectTIME = 0;

				// 참석자에게 기존 사용자 정보 전송.
				pUSER->Send_gsv_PARTY_REPLY( 0, PARTY_REPLY_REJOIN );
				this->Send_PartyUserLIST( pUSER, pUSER->m_dwDBID );
				break;
			}
		}
#else
		x
#endif
	}
	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
void CParty::OnDisconnect (classUSER *pUSER)
{
	// !!!! SQL 쓰래드에서 this->Lock 걸려 들어옴
	// 모두 디스난 상태면 파티 뽀갬.
	tagPartyUSER *pPU;
#ifdef	__USE_ARRAY_PARTY_USER
	pPU = this->m_pPartyUSERs[ pUSER->m_nPartyPOS ];
#else
	x
#endif
	pPU->m_pUSER  = NULL;
	pPU->m_dwDisconnectTIME = classTIME::GetCurrentAbsSecond();

	// 파짱이면 이후 사용자에게 파짱 물림.
#ifdef	__USE_ARRAY_PARTY_USER
	if ( 0 == pUSER->m_nPartyPOS ) {	// 파티 오너가 짤렸네...
		for (short nI=1; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] ) {
				g_pThreadLOG->When_SysERR( __FILE__, nI, "OnDisconnect:: NULL == this->m_pPartyUSERs[ nI ]" );
				continue;
			}

			if ( this->m_pPartyUSERs[ nI ]->m_pUSER ) {
				this->Change_PartyOWNER( this->m_pPartyUSERs[ nI ]->m_pUSER, PARTY_REPLY_CHANGE_OWNERnDISCONN );
				return;
			}
		}
		// 파짱이 아니라는건 파짱이 살아있다는뜻(파짱이 짤리면 살은넘한테 파짱을 넘기기땜에..)
		g_pPartyBUFF->DeletePARTY( this );
		return;
	}

	// 파티원이 짤렸다.
	this->Send_gsv_PARTY_REPLY2Members( PARTY_REPLY_DISCONNECT, pPU->m_dwDBID );
	return;
#else
	x
#endif
}

//-------------------------------------------------------------------------------------------------
void CParty::Member_LevelUP (short nPartyPOS, short nLevelDIFF)
{
	this->Lock ();
	{
		this->m_pPartyUSERs[ nPartyPOS ]->m_nLevel += nLevelDIFF;
		this->m_iTotalLEV += nLevelDIFF;
		this->m_iAverageLEV = this->m_iTotalLEV / m_nMemberCNT;
	}
	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool CParty::Kick_MEMBER( DWORD dwDBID )
{
	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		for (short nI=1; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] ) {
				g_pThreadLOG->When_SysERR( __FILE__, nI, "Kick_MEMBER:: NULL == this->m_pPartyUSERs[ nI ]" );
				continue;
			}

			if ( m_pPartyUSERs[ nI ]->m_dwDBID != dwDBID )
				continue;
			
			if ( m_pPartyUSERs[ nI ]->m_pUSER )
				m_pPartyUSERs[ nI ]->m_pUSER->Send_gsv_PARTY_REPLY( dwDBID, PARTY_REPLY_BAN );

			this->Sub_PartyUSER( nI );	// Sub_PartyUSER
			break;
		}
#else
		x
#endif
	}
	this->Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CParty::SendWhisperToPartyMembers( char *szMessage )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_WHISPER;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_WHISPER );
	pCPacket->AppendString( "파티옵션" );
	pCPacket->AppendString( szMessage );

	this->SendToPartyMembers( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CParty::SendToPartyMembers( classPACKET *pCPacket )
{
	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] ) {
				g_pThreadLOG->When_SysERR( __FILE__, nI, "SendToPartyMembers:: NULL == this->m_pPartyUSERs[ nI ]" );
				continue;
			}

			if ( this->m_pPartyUSERs[ nI ]->m_pUSER ) {
				this->m_pPartyUSERs[ nI ]->m_pUSER->SendPacket( pCPacket );
			}
		}
#else
		x
#endif
	}
	this->Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
void CParty::LevelUP ()
{
	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] ) {
				g_pThreadLOG->When_SysERR( __FILE__, nI, "LevelUP:: NULL == this->m_pPartyUSERs[ nI ]" );
				continue;
			}

			if ( m_pPartyUSERs[ nI ]->m_pUSER && m_pPartyUSERs[ nI ]->m_pUSER->Get_HP() > 0 ) {
				m_pPartyUSERs[ nI ]->m_pUSER->Set_HP( m_pPartyUSERs[ nI ]->m_pUSER->Get_MaxHP() );
				m_pPartyUSERs[ nI ]->m_pUSER->Set_MP( m_pPartyUSERs[ nI ]->m_pUSER->Get_MaxMP() );
			}
		}
#else
		x
#endif
	}
	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
/*
//bool CParty::SendToPartyMembersExecptNEAR( classUSER *pSender, classPACKET *pCPacket )
//{
//	this->Lock ();
//	{
//#ifdef	__USE_ARRAY_PARTY_USER
//		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
//			if ( m_pPartyUSERs[ nI ] && m_pPartyUSERs[ nI ]->m_pUSER && 
//				pSender->GetZONE() != m_pPartyUSERs[ nI ]->m_pUSER->GetZONE() ||
//				abs( pSender->m_PosSECTOR.x - pUserNODE->m_VALUE.m_pUSER->m_PosSECTOR.x ) > 1 ||
//				abs( pSender->m_PosSECTOR.y - pUserNODE->m_VALUE.m_pUSER->m_PosSECTOR.y ) > 1 ) {
//					pUserNODE->m_VALUE.m_pUSER->Send_Start( pCPacket );
//			}
//		}
//#else
//		CDLList< tagPartyUSER >::tagNODE *pUserNODE;
//		pUserNODE = m_PartyUSER.GetHeadNode();
//		while( pUserNODE ) {
//			if ( pUserNODE->m_VALUE.m_pUSER && 
//				pSender->GetZONE() != pUserNODE->m_VALUE.m_pUSER->GetZONE() ||
//				abs( pSender->m_PosSECTOR.x - pUserNODE->m_VALUE.m_pUSER->m_PosSECTOR.x ) > 1 ||
//				abs( pSender->m_PosSECTOR.y - pUserNODE->m_VALUE.m_pUSER->m_PosSECTOR.y ) > 1 ) {
//					pUserNODE->m_VALUE.m_pUSER->Send_Start( pCPacket );
//			}
//
//			pUserNODE = pUserNODE->GetNext ();
//		}
//#endif
//	}
//	this->Unlock ();
//
//	return true;
//}
*/
//-------------------------------------------------------------------------------------------------
bool CParty::SendToPartyMembersExceptME( classUSER *pSender, classPACKET *pCPacket )
{
	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
			if ( nI == pSender->m_nPartyPOS )
				continue;

			if ( this->m_pPartyUSERs[ nI ] && this->m_pPartyUSERs[ nI ]->m_pUSER )
				this->m_pPartyUSERs[ nI ]->m_pUSER->SendPacket( pCPacket );
		}
#else
		x
#endif
	}
	this->Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CParty::SendPartyLEVnEXP (classUSER *pUSER, BYTE bitLevelUP)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_PARTY_LEVnEXP;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_LEVnEXP );
	pCPacket->m_gsv_PARTY_LEVnEXP.m_btLEVEL = this->m_btPartyLEV;
	pCPacket->m_gsv_PARTY_LEVnEXP.m_iEXP = this->m_iPartyEXP;
	pCPacket->m_gsv_PARTY_LEVnEXP.m_bitLevelUP = bitLevelUP;

	if ( NULL == pUSER )
		this->SendToPartyMembers( pCPacket );
	else
		pUSER->SendPacket( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
#define	PARTY_EXP_UPDATE_TIME	( 10 * 1000 )	// 5 sec
#define	PARTY_EXP_SHARED_RANGE	5000

bool CParty::AddEXP( CObjMOB *pMobCHAR, int iTotalEXP, int iPartyExpA, int iPartyExpB )
{
	classUSER *pNearUSER[ MAX_PARTY_MEMBERS ];
	int iD, iNearUSER=0;
	bool bFindOwner=false;

	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] ) {
				g_pThreadLOG->When_SysERR( __FILE__, nI, "AddEXP:: NULL == this->m_pPartyUSERs[ nI ]" );
				continue;
			}

			if ( m_pPartyUSERs[ nI ]->m_pUSER && pMobCHAR->GetZONE() == m_pPartyUSERs[ nI ]->m_pUSER->GetZONE() ) 
			{
				iD = pMobCHAR->Get_DISTANCE( m_pPartyUSERs[ nI ]->m_pUSER->m_PosCUR );
				if ( iD < PARTY_EXP_SHARED_RANGE ) {
					pNearUSER[ iNearUSER++ ] = m_pPartyUSERs[ nI ]->m_pUSER;
				}
			}
		}
#else
		x
#endif
		if ( iNearUSER > 1 ) {	// 파티로 솔로링하는건 안돼~~
			int iEXP;
			if ( this->m_btPartyRULE & BIT_PARTY_RULE_EXP_PER_PLAYER ) {
				// 개인 레벨에 따라 분배...
				for (iD=0; iD<iNearUSER; iD++) {
					iEXP = (int)( ( iTotalEXP * ( m_btPartyLEV+101 ) ) * ( pNearUSER[ iD ]->Get_LEVEL()+35-m_iAverageLEV ) / (iNearUSER*4+1) / 700 );
					if ( iEXP > 0 ) {
						pNearUSER[ iD ]->Add_EXP( iEXP, true, pMobCHAR->Get_INDEX() );
					}
					if ( pNearUSER[ iD ] == this->GetPartyOWNER() )
						bFindOwner = true;
				}
			} else {
				// 파티원에게 균등 분배...
				iEXP = (int)( ( iTotalEXP * ( this->m_btPartyLEV+101 ) ) / (iNearUSER*4+1) / 20 );
				if ( iEXP > 0 ) {
					for (iD=0; iD<iNearUSER; iD++) {
						pNearUSER[ iD ]->Add_EXP( iEXP, true, pMobCHAR->Get_INDEX() );
						if ( pNearUSER[ iD ] == this->GetPartyOWNER() )
							bFindOwner = true;
					}
				}
			}
		}
	}
	this->Unlock ();


	// 파티장이 같은 로컬 존에 있냐?
	if ( iNearUSER > 1 && bFindOwner && m_btPartyLEV < MAX_PARTY_LEVEL ) {
		int iCalEXP, iNewEXP;
		DWORD dwCurTIME = pMobCHAR->GetZONE()->GetCurrentTIME();
		BYTE btLevelUP=0;

		// 일정 간격 감소되는 경험치.
		iCalEXP = ( m_btPartyLEV/2 + m_nMemberCNT ) * ( dwCurTIME - this->m_dwLastKillTIME ) / PARTY_EXP_UPDATE_TIME;
		this->m_dwLastKillTIME = dwCurTIME;

		iNewEXP = ( iCalEXP > m_iPartyEXP ) ? 0 : m_iPartyEXP-iCalEXP;

		// 얻는 경험치
		int iDiffLEV = pMobCHAR->Get_LEVEL() - m_iAverageLEV;
		if ( iDiffLEV >= 0 ) {
			// 얻는 파티 경험치 = (몹레벨-파티평균 레벨 + A) * (몹레벨-파티평균 레벨 + A + 1) * B/10 + 2
			// iCalEXP = ( iDiffLEV + 2 ) * ( iDiffLEV + 1 ) + 2;
			iCalEXP = ( iDiffLEV + iPartyExpA ) * ( iDiffLEV + iPartyExpA + 1 ) * iPartyExpB/10 + 2;

			iNewEXP += iCalEXP;
			iCalEXP = CCal::Get_PartyLevelUpNeedEXP( m_btPartyLEV );
			if ( iNewEXP >= iCalEXP ) {
				iNewEXP -= iCalEXP;
				m_btPartyLEV ++;
				// 모든 파티원 HP/MP 꽉~~
				this->LevelUP ();
				btLevelUP = 1;
			}
		}

		if ( iNewEXP != m_iPartyEXP ) {
			m_iPartyEXP = iNewEXP;
			this->SendPartyLEVnEXP( NULL, btLevelUP );
		}
	}
	return ( iNearUSER > 1 );
}

//-------------------------------------------------------------------------------------------------
bool CParty::Change_ObjectIDX (classUSER *pUSER)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_CHANGE_OBJIDX;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHANGE_OBJIDX );
	pCPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_dwUserTAG	  = pUSER->m_dwDBID;
	pCPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_wObjectIDX   = pUSER->Get_INDEX();
	pCPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_nMaxHP		  = pUSER->GetOri_MaxHP();	// Get_MaxHP();
	pCPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_nHP		  = pUSER->Get_HP ();
	pCPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_dwStatusFALG = pUSER->m_IngSTATUS.GetFLAGs();

	pCPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_nCON		  = pUSER->GetCur_CON ();
	pCPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_btRecoverHP  = pUSER->m_btRecoverHP;
	pCPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_btRecoverMP  = pUSER->m_btRecoverMP;
	pCPacket->m_gsv_CHANGE_OBJIDX.m_Member.m_nSTAMINA	  = pUSER->GetCur_STAMINA();

	this->SendToPartyMembersExceptME( pUSER, pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CParty::Change_PartyOWNER (classUSER *pNewOWNER, BYTE btReply)
{
	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		tagPartyUSER *pPU = this->m_pPartyUSERs[ 0 ];

		this->m_pPartyUSERs[ 0 ] = this->m_pPartyUSERs[ pNewOWNER->m_nPartyPOS ];
		this->m_pPartyUSERs[ pNewOWNER->m_nPartyPOS ] = pPU;
		if ( pPU && pPU->m_pUSER ) {
			pPU->m_pUSER->m_nPartyPOS = pNewOWNER->m_nPartyPOS;
		}
		pNewOWNER->m_nPartyPOS = 0;
#else
		x
#endif
	}
	this->Unlock ();


	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_PARTY_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_REPLY );
	pCPacket->m_gsv_PARTY_REPLY.m_btREPLY = btReply;
	pCPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG = pNewOWNER->Get_INDEX();

	this->SendToPartyMembers( pCPacket );

	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
//void CParty::Check_UserLEV ()
//{
//	m_nMinUserLEV = 300;
//	m_nMaxUserLEV = 0;
//
//	for (short nI=0; nI<this->m_nMemberCNT; nI++) {
//		if ( NULL == this->m_pPartyUSERs[ nI ] )
//			continue;
//		if ( m_pPartyUSERs[ nI ]->m_nLevel > m_nMaxUserLEV )
//			m_nMaxUserLEV = m_pPartyUSERs[ nI ]->m_nLevel;
//		else
//		if ( m_pPartyUSERs[ nI ]->m_nLevel < m_nMinUserLEV )
//			m_nMinUserLEV = m_pPartyUSERs[ nI ]->m_nLevel;
//	}
//
//	if ( this->m_nMaxUserLEV - this->m_nMinUserLEV >= 10 ) {
//		if ( 0 == ( this->m_btPartyRULE & BIT_PARTY_RULE_EXP_PER_PLAYER ) ) {
//			// 개인 레벨에 따라 분배로 변경...
//			this->Set_PartyRULE( this->m_btPartyRULE );
//		}
//	}
//}

//-------------------------------------------------------------------------------------------------
bool CParty::Send_PartyUserLIST( classUSER *pUSER, DWORD dwUserTAG )
{
	this->SendPartyLEVnEXP( pUSER );

    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return false;

	// pUSER에게 기존 유저들 정보 전송...
	pCPacket->m_HEADER.m_wType = GSV_PARTY_MEMBER;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_MEMBER );
	pCPacket->m_gsv_PARTY_MEMBER.m_btPartyRULE = this->m_btPartyRULE;
	pCPacket->m_gsv_PARTY_MEMBER.m_cUserCNT = 0;

	tag_PARTY_MEMBER sMember, sDisMEM = { 0, };

	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] ) {
				// 이런 경우는 뭐냐 ???
				g_pThreadLOG->When_SysERR( __FILE__, nI, "Send_PartyUserLIST:: NULL == pCParty->m_pPartyUSERs[ nI ]" );
				continue;	
			}

			if ( dwUserTAG != m_pPartyUSERs[ nI ]->m_dwDBID ) {
				// 비정상 종료후 접속시 자신에게 자신의 정보 전송하지 않도록...
				if ( m_pPartyUSERs[ nI ]->m_pUSER ) {
					sMember.m_dwUserTAG		= m_pPartyUSERs[ nI ]->m_pUSER->m_dwDBID;
					sMember.m_wObjectIDX	= m_pPartyUSERs[ nI ]->m_pUSER->Get_INDEX();
					sMember.m_nHP			= m_pPartyUSERs[ nI ]->m_pUSER->Get_HP();
					sMember.m_nMaxHP		= m_pPartyUSERs[ nI ]->m_pUSER->GetOri_MaxHP();	// Get_MaxHP ();
					sMember.m_dwStatusFALG	= m_pPartyUSERs[ nI ]->m_pUSER->m_IngSTATUS.GetFLAGs();
					sMember.m_nCON		    = m_pPartyUSERs[ nI ]->m_pUSER->GetCur_CON ();
					sMember.m_btRecoverHP	= m_pPartyUSERs[ nI ]->m_pUSER->m_btRecoverHP;
					sMember.m_btRecoverMP	= m_pPartyUSERs[ nI ]->m_pUSER->m_btRecoverMP;
					sMember.m_nSTAMINA		= m_pPartyUSERs[ nI ]->m_pUSER->GetCur_STAMINA ();

					pCPacket->AppendData( &sMember, sizeof( tag_PARTY_MEMBER ) );
				} else {
					sDisMEM.m_dwUserTAG = m_pPartyUSERs[ nI ]->m_dwDBID;
					pCPacket->AppendData( &sDisMEM, sizeof( tag_PARTY_MEMBER ) );
				}
				pCPacket->AppendString( m_pPartyUSERs[ nI ]->m_NameCHAR.Get() );

				pCPacket->m_gsv_PARTY_MEMBER.m_cUserCNT  ++;
			}
		}
#else
		x
#endif
	}
	this->Unlock ();

	pUSER->SendPacket( pCPacket );
    Packet_ReleaseNUnlock( pCPacket );

	return true;
}


extern bool IsTAIWAN();
BYTE CParty::Add_PartyUSER (classUSER *pUSER)
{
	// 05.05.25 대만 오베이후는 파티원은 무조건 5명 !!!
	short nMaxUser = IsTAIWAN() ? 6 : this->m_btPartyLEV / 5 + 4;

	// 기존 유저에게 새로운 유저 들왔다고 전송...
    classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return 0;

	this->Lock ();
	{
		// 정원 초과 ?
		if ( this->m_nMemberCNT >= nMaxUser || this->m_nMemberCNT >= MAX_PARTY_MEMBERS ) {
			this->Unlock ();
			return PARTY_REPLY_FULL_MEMBERS;
		}

		this->Send_PartyUserLIST( pUSER, 0 );

		pCPacket->m_HEADER.m_wType = GSV_PARTY_MEMBER;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_MEMBER );
		pCPacket->m_gsv_PARTY_MEMBER.m_btPartyRULE = this->m_btPartyRULE;
		pCPacket->m_gsv_PARTY_MEMBER.m_cUserCNT = 1;

		tag_PARTY_MEMBER sMember;

		sMember.m_dwUserTAG		= pUSER->m_dwDBID;
		sMember.m_wObjectIDX	= pUSER->Get_INDEX();
		sMember.m_nHP			= pUSER->Get_HP ();
		sMember.m_nMaxHP		= pUSER->GetOri_MaxHP();	// Get_MaxHP ();
		sMember.m_dwStatusFALG	= pUSER->m_IngSTATUS.GetFLAGs();

		sMember.m_nCON		    = pUSER->GetCur_CON ();
		sMember.m_btRecoverHP	= pUSER->m_btRecoverHP;
		sMember.m_btRecoverMP	= pUSER->m_btRecoverMP;
		sMember.m_nSTAMINA		= pUSER->GetCur_STAMINA ();

		pCPacket->AppendData( &sMember, sizeof( tag_PARTY_MEMBER ) );
		pCPacket->AppendString( pUSER->Get_NAME() );

		this->SendToPartyMembers( pCPacket );
		Packet_ReleaseNUnlock( pCPacket );

#ifdef	__USE_ARRAY_PARTY_USER
		tagPartyUSER *pPU = new tagPartyUSER;

		this->m_pPartyUSERs[ m_nMemberCNT ] = pPU;
		pUSER->m_nPartyPOS = m_nMemberCNT;

		pPU->m_dwDBID = pUSER->m_dwDBID;
		pPU->m_pUSER  = pUSER;
		pPU->m_nLevel = pUSER->Get_LEVEL();
		pPU->m_NameCHAR.Set( pUSER->Get_NAME() );

		m_nMemberCNT ++;
#else
		x
#endif
		pUSER->m_pPartyBUFF = this;
		this->m_iTotalLEV += pUSER->Get_LEVEL();
		this->m_iAverageLEV = this->m_iTotalLEV / m_nMemberCNT;

		this->Set_PartyRULE( this->m_btPartyRULE );
	}
	this->Unlock ();

	return 0;
}

//-------------------------------------------------------------------------------------------------
bool CParty::Sub_PartyUSER( short nPartyPOS )
{
	DWORD dwLeftUserID;
	DWORD dwOwnerID;

	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		if ( NULL == this->m_pPartyUSERs[ nPartyPOS ] ) {
			this->Unlock ();
			return true;
		}

		dwLeftUserID = this->m_pPartyUSERs[ nPartyPOS ]->m_dwDBID;

		if ( this->m_pPartyUSERs[ nPartyPOS ]->m_pUSER )
			this->m_pPartyUSERs[ nPartyPOS ]->m_pUSER->m_pPartyBUFF = NULL;

		this->m_iTotalLEV -= this->m_pPartyUSERs[ nPartyPOS ]->m_nLevel;

		SAFE_DELETE( this->m_pPartyUSERs[ nPartyPOS ] );
		m_nMemberCNT --;

		short nI;
		for (nI=nPartyPOS; nI<m_nMemberCNT; nI++) {
			this->m_pPartyUSERs[ nI ] = this->m_pPartyUSERs[ nI+1 ];
			if ( this->m_pPartyUSERs[ nI ]->m_pUSER )
				this->m_pPartyUSERs[ nI ]->m_pUSER->m_nPartyPOS = nI;
		}
		m_pPartyUSERs[ m_nMemberCNT ] = NULL;

		if ( m_nMemberCNT > 0 ) {
			this->m_iAverageLEV = this->m_iTotalLEV / m_nMemberCNT;

			for (nI=0; nI<MAX_INV_TYPE; nI++) {
				if ( nPartyPOS > 0 && this->m_btItemORDERs[ nI ] >= nPartyPOS )
					this->m_btItemORDERs[ nI ] --;
			}
			//if ( nPartyPOS <= this->m_btItemORDER ) {
			//	if ( 0 == nPartyPOS )
			//		this->m_btItemORDER = m_nMemberCNT-1;
			//}
		}
		else {
			this->m_iAverageLEV = 1;
		}

		dwOwnerID = m_pPartyUSERs[ 0 ]->m_dwDBID;
#else
		x
#endif
	}
	this->Unlock ();

	if ( m_nMemberCNT <= 1 ) {
		// 1명 남았으면 파티 뽀갠다.
		g_pPartyBUFF->DeletePARTY( this );
	} else {
		// 남은 유저에게 유저 탈퇴 통보...
		classPACKET *pCPacket;
		pCPacket = Packet_AllocNLock ();
		if ( !pCPacket ) 
			return false;

		pCPacket->m_HEADER.m_wType = GSV_PARTY_MEMBER;
		pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_MEMBER ) + sizeof(DWORD) * 2;
		pCPacket->m_gsv_PARTY_MEMBER.m_btPartyRULE = this->m_btPartyRULE;
		pCPacket->m_gsv_PARTY_MEMBER.m_cUserCNT = -1;
		pCPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[ 0 ] = dwLeftUserID;
		pCPacket->m_gsv_PARTY_MEMBER.m_dwObjectTAG[ 1 ] = dwOwnerID;//pOwnerNODE->m_VALUE.m_pUSER->m_dwDBID;

		this->SendToPartyMembers( pCPacket );
		Packet_ReleaseNUnlock( pCPacket );
	}

	return true;
}


//-------------------------------------------------------------------------------------------------
void CParty::Share_MONEY( int iMoney )
{
	int iAdd = iMoney / this->m_nMemberCNT + 1;

	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] ) {
				// 이런 경우는 뭐냐 ???
				g_pThreadLOG->When_SysERR( __FILE__, nI, "Share_MONEY:: NULL == pCParty->m_pPartyUSERs[ nI ]" );
				continue;	
			}
			if ( this->m_pPartyUSERs[ nI ]->m_pUSER ) {		// 0x0008bce5 오류... NULL == this->m_pPartyUSERs[ nI ] ???
				this->m_pPartyUSERs[ nI ]->m_pUSER->Add_MoneyNSend( iAdd, GSV_SET_MONEY_ONLY );
			}
		}
#else
		x
#endif
	}
	this->Unlock ();
}

//-------------------------------------------------------------------------------------------------
bool CParty::Send_gsv_PARTY_ITEM (classUSER *pPicker, CObjITEM *pItemOBJ)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_PARTY_ITEM;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_ITEM);

	pCPacket->m_gsv_PARTY_ITEM.m_wObjectIDX = pPicker->Get_INDEX();
	pCPacket->m_gsv_PARTY_ITEM.m_ITEM		= pItemOBJ->m_ITEM;

	this->SendToPartyMembersExceptME( pPicker, pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

bool CParty::Give_Item2ORDER (classUSER *pTryOBJ, CObjITEM *pItemOBJ)
{
	this->Lock ();
	{
		t_InvTYPE InvPage = CInventory::m_InvTYPE[ pItemOBJ->m_ITEM.GetTYPE() ];

		BYTE btOrder = m_btItemORDERs[ InvPage ];
		for (short nI=0; nI<m_nMemberCNT; nI++) {
			btOrder = (1+btOrder) % m_nMemberCNT;
			// 같은 존이면 순서자에게...
			if ( m_pPartyUSERs[ btOrder ] && m_pPartyUSERs[ btOrder ]->m_pUSER &&
				m_pPartyUSERs[ btOrder ]->m_pUSER->GetZONE() == pTryOBJ->GetZONE() ) {

				// 파티원들한테 아이템 습득 메세지...
				this->Send_gsv_PARTY_ITEM( m_pPartyUSERs[ btOrder ]->m_pUSER, pItemOBJ );
				m_pPartyUSERs[ btOrder ]->m_pUSER->Pick_ITEM( pItemOBJ );

				m_btItemORDERs[ InvPage ] = btOrder;
				this->Unlock ();
				return true;
			}
		}
	}
	this->Unlock ();

	// 멀리 있거나 접속 끊긴 넘이면 줍은넘 먹기...
	return pTryOBJ->Pick_ITEM( pItemOBJ );
}

//-------------------------------------------------------------------------------------------------
bool CParty::Send_gsv_PARTY_REPLY2Members (	BYTE btReply, DWORD dwFromIDXorTAG )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		false;
	
	pCPacket->m_HEADER.m_wType = GSV_PARTY_REPLY;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_REPLY );
	pCPacket->m_gsv_PARTY_REPLY.m_btREPLY = btReply;
	pCPacket->m_gsv_PARTY_REPLY.m_dwFromIDXorTAG = dwFromIDXorTAG;

	this->SendToPartyMembers( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CParty::Send_gsv_CHECK_NPC_EVENT( classUSER *pTryOBJ, short nNpcIDX )
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return false;

	pCPacket->m_HEADER.m_wType = GSV_CHECK_NPC_EVENT;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_CHECK_NPC_EVENT );
	pCPacket->m_gsv_CHECK_NPC_EVENT.m_nNpcIDX = nNpcIDX;

	int iDist;
	this->Lock ();
	{
#ifdef	__USE_ARRAY_PARTY_USER
		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] ) {
				g_pThreadLOG->When_SysERR( __FILE__, nI, "SendToPartyMembers:: NULL == this->m_pPartyUSERs[ nI ]" );
				continue;
			}

			if ( this->m_pPartyUSERs[ nI ]->m_pUSER &&
				 this->m_pPartyUSERs[ nI ]->m_pUSER->GetZONE() == pTryOBJ->GetZONE() ) {
				iDist = pTryOBJ->Get_DISTANCE( this->m_pPartyUSERs[ nI ]->m_pUSER->m_PosCUR );
				if ( iDist < PARTY_EXP_SHARED_RANGE ) {
					this->m_pPartyUSERs[ nI ]->m_pUSER->m_iLastEventNpcIDX = nNpcIDX;
					this->m_pPartyUSERs[ nI ]->m_pUSER->SendPacket( pCPacket );
				}
			}
		}
#else
		x
#endif
	}
	this->Unlock ();

	Packet_ReleaseNUnlock( pCPacket );
	return true;
}

//-------------------------------------------------------------------------------------------------
void CParty::Set_PartyRULE( BYTE btRULE )
{
	short nMinUserLEV = 300;
	short nMaxUserLEV = 0;

	this->Lock ();
	{
		for (short nI=0; nI<this->m_nMemberCNT; nI++) {
			if ( NULL == this->m_pPartyUSERs[ nI ] )
				continue;
			if ( m_pPartyUSERs[ nI ]->m_nLevel > nMaxUserLEV )
				nMaxUserLEV = m_pPartyUSERs[ nI ]->m_nLevel;
			if ( m_pPartyUSERs[ nI ]->m_nLevel < nMinUserLEV )
				nMinUserLEV = m_pPartyUSERs[ nI ]->m_nLevel;
		}
	}
	this->Unlock ();

	if ( nMaxUserLEV - nMinUserLEV >= 10 ) {
		if ( 0 == ( btRULE & BIT_PARTY_RULE_EXP_PER_PLAYER ) ) {
			// 개인 레벨에 따라 강제로 설정...
			btRULE |= BIT_PARTY_RULE_EXP_PER_PLAYER;
		}
	}
	if ( this->m_btPartyRULE == btRULE )
		return;

	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket ) 
		return;
	
	this->m_btPartyRULE = btRULE;

	pCPacket->m_HEADER.m_wType = GSV_PARTY_RULE;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_PARTY_RULE );
	pCPacket->m_gsv_PARTY_RULE.m_btPartyRULE = btRULE;

	this->SendToPartyMembers( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
void CParty::Warp_USERS(int iZoneNO, tPOINTF &PosGOTO)
{
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = GSV_RELAY_REQ;
	pCPacket->m_HEADER.m_nSize = sizeof( gsv_RELAY_REQ );

	pCPacket->m_gsv_RELAY_REQ.m_wRelayTYPE  = RELAY_TYPE_RECALL;
	pCPacket->m_gsv_RELAY_REQ.m_nCallZoneNO = iZoneNO;
	pCPacket->m_gsv_RELAY_REQ.m_PosCALL		= PosGOTO;

	this->SendToPartyMembers( pCPacket );
	Packet_ReleaseNUnlock( pCPacket );
}

//-------------------------------------------------------------------------------------------------
