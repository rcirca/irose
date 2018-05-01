
#include "stdAFX.h"

#include "CMessenger.h"

#if !defined( __SHO_WS ) && !defined( __SHO_GS )
	#error >>> ERROR :: must defined __SHO_WS or __SHO_GS
#endif

#ifdef	__SHO_WS
	#include "CWS_Client.h"
	typedef	CWS_Client	classUSER;
#endif

#ifdef	__SHO_GS
	#include "GS_ListUSER.h"
#endif


//-------------------------------------------------------------------------------------------------
void CMessenger::MSGR_LogIN (int iCount, BYTE *pLIST)
{
/*
	1. 내가 등록한 친구리스트에서...
		친구가 접속해 있냐?
		y: 친구가 나를 리스트에 갖고 있냐?
		   y: 상태전송
		   n: 통과
		n: 오프라인 상태
*/
	classUSER *pClient;

	classPACKET *pToFR = Packet_AllocNLock ();
	classPACKET *pToME = Packet_AllocNLock ();

	pToFR->m_HEADER.m_wType = WSV_MESSENGER;
	pToFR->m_HEADER.m_nSize = sizeof( wsv_MESSENGER );
	pToFR->m_wsv_MESSENGER.m_btCMD = MSGR_CMD_CHANGE_STATUS;

	wsv_MCMD_STATUS_REPLY sFrStatus;

	sFrStatus.m_dwUserTAG = this->Get_DBID ();
	sFrStatus.m_btStatus  = FRIEND_STATUS_ONLINE;
	pToFR->AppendData( &sFrStatus, sizeof( wsv_MCMD_STATUS_REPLY ) );


	pToME->m_HEADER.m_wType = WSV_MESSENGER;
	pToME->m_HEADER.m_nSize = sizeof( wsv_MESSENGER ) + sizeof(BYTE);	// friend count
	pToME->m_wsv_MESSENGER.m_btCMD = MSGR_CMD_LIST;
	
	char *pName;
	int   iNameLEN;
	tagFriend_H *pFrH;

	CDLList< tagMyFriend >::tagNODE *pNODE;
	m_csFRIEND.Lock ();
	{
		for (int iC=0; iC<iCount; iC++) {
			pFrH = (tagFriend_H*)pLIST;
			pName = (char*)( pFrH+sizeof(tagFriend_H) );
			iNameLEN = strlen( pName );
			pLIST += ( sizeof(tagFriend_H)+iNameLEN+1 );

			pNODE = m_ListFRIEND.AllocNAppend();

			pNODE->m_VALUE.m_dwDBID   = pFrH->m_dwDBID;
			pNODE->m_VALUE.m_btSTATUS = pFrH->m_btSTATUS;
			pNODE->m_VALUE.m_Name.Set( pName );

			if ( !(FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS) ) {
				#ifdef	__SHO_GS
				pClient = g_pUserLIST->Find_CHAR( pName );
				#endif
				#ifdef	__SHO_WS
				pClient = g_pListCLIENT->Find_CHAR( pName );
				#endif
				if ( pClient ) {
					// 친구의 목록에 내가 로긴된걸로 설정 나에대한 상태를 리턴한다.
					pNODE->m_VALUE.m_pUSER    = pClient;
					pNODE->m_VALUE.m_btSTATUS = pClient->MSGR_OnOffLine( pToFR, this, this->Get_DBID(), FRIEND_STATUS_ONLINE );
				} else {
					pNODE->m_VALUE.m_btSTATUS = FRIEND_STATUS_OFFLINE;
				}
			} /* else {
				// 나를 삭제하거나 내가 차단한넘이기 땜에 내 상태 통보할 필요없다.
				// 나도 이넘을 로그아웃 상태로 보이게함...
			}
			*/

			sFrStatus.m_dwUserTAG = pNODE->m_VALUE.m_dwDBID;
			sFrStatus.m_btStatus  = pNODE->m_VALUE.m_btSTATUS;

			pToME->AppendData( &sFrStatus, sizeof( wsv_MCMD_STATUS_REPLY ) );
			pToME->AppendString( pName );
		}
	}
	m_csFRIEND.Unlock ();

	this->SendPacket( pToME );

	Packet_ReleaseNUnlock( pToME );
	Packet_ReleaseNUnlock( pToFR );
}


//-------------------------------------------------------------------------------------------------
/*
2. 로그아웃시
	. 나가 등록한 친구리스트 조회..
		NODE->m_VALUE.m_pUSER != NULL 이면 로그아웃 전송.
*/
void CMessenger::MSGR_LogOUT ()
{
	/*
	if ( m_bUpdate ) {
		;
		;
		;
	}
	*/
	classPACKET *pCPacket = Packet_AllocNLock ();

	pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_MESSENGER );
	pCPacket->m_wsv_MESSENGER.m_btCMD = MSGR_CMD_CHANGE_STATUS;

	wsv_MCMD_STATUS_REPLY sMyStatus;

	sMyStatus.m_dwUserTAG = this->Get_DBID ();
	sMyStatus.m_btStatus  = FRIEND_STATUS_OFFLINE;
	pCPacket->AppendData( &sMyStatus, sizeof( wsv_MCMD_STATUS_REPLY ) );

	CDLList< tagMyFriend >::tagNODE *pNODE;

	m_csFRIEND.Lock ();
	pNODE = m_ListFRIEND.GetHeadNode();
	while( pNODE ) {
		if ( !(FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS) ) {
			if ( pNODE->m_VALUE.m_pUSER ) {
				pNODE->m_VALUE.m_pUSER->MSGR_OnOffLine( pCPacket, NULL, this->Get_DBID(), FRIEND_STATUS_OFFLINE );
			}
		}

		m_ListFRIEND.DeleteNFree( pNODE );
		pNODE = m_ListFRIEND.GetHeadNode();
	}
	m_csFRIEND.Unlock ();

	Packet_ReleaseNUnlock( pCPacket );
}


//-------------------------------------------------------------------------------------------------
/*
3. 친구등록시 : 친구는 반드시 온라인 상태여야 한다.
	. 대상의 리스트에 이미 내가 친구로 되어 있냐?
	. 나의 친구목록에 추가
	. 친구자신을 등록시킨 케릭 리스트에 나를 추가.
*/
bool CMessenger::MSGR_Add( CWS_Client *pFriend )
{
	bool bResult;
	CDLList< tagMyFriend >::tagNODE *pNODE;

	m_csFRIEND.Lock ();
	{
		if ( m_ListFRIEND.GetNodeCount() < MAX_FRIEND_COUNT ) {
			bResult = true;
			this->m_bFriendUPDATE = true;

			pNODE = m_ListFRIEND.AllocNAppend();
			pNODE->m_VALUE.m_pUSER  = pFriend;
			pNODE->m_VALUE.m_dwDBID = pFriend->m_iDBID;
			pNODE->m_VALUE.m_Name.Set( pFriend->Get_NAME() );
		} else
			bResult = false;
	}
	m_csFRIEND.Unlock ();

	return bResult;
}

void CMessenger::MSGR_Del( DWORD dwDBID )
{
	CDLList< tagMyFriend >::tagNODE *pNODE;
	CWS_Client *pDestUSER=NULL;

	m_csFRIEND.Lock ();
	{
		pNODE = m_ListFRIEND.GetHeadNode ();
		while( pNODE ) {
			if ( dwDBID == pNODE->m_VALUE.m_dwDBID ) {
				// 이넘 삭제
				pDestUSER = pNODE->m_VALUE.m_pUSER;
				m_ListFRIEND.DeleteNFree( pNODE );
				this->m_bFriendUPDATE = true;
				break;
			}
			pNODE = pNODE->GetNext ();
		}
	}
	m_csFRIEND.Unlock ();

	if ( pDestUSER ) {
		pDestUSER->MSGR_Status( this->Get_DBID(), FRIEND_STATUS_DELETED );
	}
}

void CMessenger::MSGR_Ban( DWORD dwDBID )
{
	CDLList< tagMyFriend >::tagNODE *pNODE;
	CWS_Client *pDestUSER=NULL;

	m_csFRIEND.Lock ();
	{
		pNODE = m_ListFRIEND.GetHeadNode ();
		while( pNODE ) {
			if ( dwDBID == pNODE->m_VALUE.m_dwDBID ) {
				// 이넘의 메세지 차단..
				pNODE->m_VALUE.m_btSTATUS = FRIEND_STATUS_REFUSED;
				break;
			}
			pNODE = pNODE->GetNext ();
		}
	}
	m_csFRIEND.Unlock ();
/*
	if ( pDestUSER ) {
		pDestUSER->MSGR_Status( this->Get_DBID(), FRIEND_STATUS_REFUSED );
	}
*/
}

void CMessenger::MSGR_Status( DWORD dwDBID, BYTE btNewStatus )
{
	CDLList< tagMyFriend >::tagNODE *pNODE;

	m_csFRIEND.Lock ();
	{
		pNODE = m_ListFRIEND.GetHeadNode ();
		while( pNODE ) {
			if ( dwDBID == pNODE->m_VALUE.m_dwDBID ) {
				this->m_bFriendUPDATE = ( FRIEND_STATUS_REFUSED & ( pNODE->m_VALUE.m_btSTATUS | btNewStatus ) );

				pNODE->m_VALUE.m_btSTATUS = btNewStatus;

				if ( pNODE->m_VALUE.m_pUSER ) {
					classPACKET *pCPacket = Packet_AllocNLock ();

					pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
					pCPacket->m_HEADER.m_nSize = sizeof( wsv_MESSENGER );
					pCPacket->m_wsv_MESSENGER.m_btCMD = MSGR_CMD_CHANGE_STATUS;

					wsv_MCMD_STATUS_REPLY sStatus;

					sMyStatus.m_dwUserTAG = dwDBID;
					sMyStatus.m_btStatus  = btNewStatus;
					pCPacket->AppendData( &sMyStatus, sizeof( wsv_MCMD_STATUS_REPLY ) );

					pNODE->m_VALUE.m_pUSER->SendPacket( pCPacket );

					Packet_ReleaseNUnlock( pCPacket );
				}
				break;
			}
			pNODE = pNODE->GetNext ();
		}
	}
	m_csFRIEND.Unlock ();
}

//-------------------------------------------------------------------------------------------------
BYTE CMessenger::MSGR_OnOffLine ( classPACKET *pCPacket, CMessenger *pFriend, DWORD dwDBID, BYTE btNewStatus )
{
	CDLList< tagMyFriend >::tagNODE *pNODE;
	BYTE btStatus = FRIEND_STATUS_DELETED;

	m_csFRIEND.Lock ();
	{
		pNODE = m_ListFRIEND.GetHeadNode ();
		while( pNODE ) {
			if ( dwDBID == pNODE->m_VALUE.m_dwDBID ) {
				btStatus = pNODE->m_VALUE.m_btSTATUS;
				pNODE->m_VALUE.m_pUSER = pFriend;

				if ( !( FRIEND_STATUS_REFUSED & btStatus ) ) {
					pNODE->m_VALUE.m_btSTATUS = btNewStatus;
					this->SendPacket( pCPacket );
				}
				break;
			}
			pNODE = pNODE->GetNext ();
		}
	}
	m_csFRIEND.Unlock ();

	return btStatus;
}

//-------------------------------------------------------------------------------------------------
