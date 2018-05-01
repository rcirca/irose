
#include "stdAFX.h"

#include "CMessenger.h"

//-------------------------------------------------------------------------------------------------
/*
1. 로그온시
	. 내가 등록시킨 케릭이 접속되어 있는가 ???
		. YES
			. 나를 친구로 등록하고 있는가 ?
				. YES
					. 친구를 online모드로
				. NO
					. 넘이 나를 친구에서 삭제했다.
					. 친구를 online모드로 ??
					. 나의 친구리스트에서 넘을 삭제 ??
					. 친구 정보를 전송 안한다고 통보 ??
		. NO
			. 친구는 offline모드다...


	. 나를 등록 시킨 케릭이 접속되어 있는가 ??? 
		. 접속되어 있으면 =>
			. 나를 친구로 등록하고 있는가 ?
				. YES
					. 내가 online모드로 된것을 통보.
				. NO
					. 친구넘이 나를 친구 목록에서 제외했다.
					  => 나를 등록시킨 리스트에서 친구넘을 삭제
*/

/*
	classPACKET *pCPacket = Packet_AllocNLock ();
	if ( !pCPacket )
		return;

	pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
	pCPacket->m_HEADER.m_nSize = sizeof( wsv_MESSENGER );
	pCPacket->m_wsv_MESSENGER.m_btCMD = MSGR_CHANGE_STATUS;
	pCPacket->m_wsv_MESSENGER.m_wServerID = xxx;

	SendPacketToSocketIDX( iSocketIDX, pCPacket );

	Packet_ReleaseNUnlock( pCPacket );
*/
void CMessenger::When_LogIN (int iCount, BYTE *pLIST)
{
	// 1. 내가 등록한 친구의 접속 상태 얻음...
	// 2. 내가 등록한 친구가 나를 친구로 갖고 있냐 ?
	//		YES: 자신에게 친구 상태 전송, 친구에게 나의 상태 전송
	//		NO : 나의 목록에서 친구 삭제? 나한테만 알려? 나한테도 안알려?
	char *pName;
	int iNameLEN;
	classUSER *pClient;

	for (int iC=0; iC<iCount; iC++) {
		pName = (char*)pLIST;
		iNameLEN = strlen( pName );

#ifdef	__SHO_GS
		pClient = g_pUserLIST->Find_CHAR( pName );
#endif
#ifdef	__SHO_WS
		pClient = g_pListCLIENT->Find_CHAR( pName );
#endif

		if ( pClient ) {
			// online 접속상태...
			this->When_APPEND (0, pClient->m_iSocketIDX, pName, pClient->m_HashCHAR);
		} else {
			// offline 
			this->When_APPEND (0, 0, pName);
		}

		pName += ( iNameLEN+1 );
	}
}


//-------------------------------------------------------------------------------------------------
/*
2. 로그아웃시
	. 나를 등록시키고 있는 친구리스트 조회..
		. 나의 상태를 offline으로...
*/
void CMessenger::When_LogOUT ()
{
	CDLList< tagMyFriend >::tagNODE *pNODE;

	m_csFRIEND.Lock ();
	{
		pNODE = m_ListFRIEND.GetHeadNode();
		while( pNODE ) {
			if ( pNODE->m_VALUE.m_iSocketIDX ) {

			}

			pNODE = pNODE->GetNext ();
		}
	}
	m_csFRIEND.Unlock ();
}


//-------------------------------------------------------------------------------------------------
/*
3. 친구등록시 : 친구는 반드시 온라인 상태여야 한다.
	. 나의 친구목록에 추가
	. 친구자신을 등록시킨 케릭 리스트에 나를 추가.
*/
void CMessenger::When_APPEND (BYTE btStatus, int iSocketIDX, char *szName, t_HASHKEY HashName)
{
	CDLList< tagMyFriend >::tagNODE *pNODE;

	this->m_bUpdate = true;

	m_csFRIEND.Lock ();
	{
		pNODE = m_ListFRIEND.AllocNAppend();
		pNODE->m_VALUE.m_btSTATUS = btStatus;
		pNODE->m_VALUE.m_iSocketIDX = iSocketIDX;
		pNODE->m_VALUE.m_Name.Set( szName );
		pNODE->m_VALUE.m_HashNAME = ( 0 == HashName ) ? CStr::GetHASH( szName ) : HashName;
	}
	m_csFRIEND.Unlock ();
}


//-------------------------------------------------------------------------------------------------
/*
4. 친구삭제시 : 
	. 나의 친구목록에서 삭제
	. 삭제 대상이 온라인이면
		. YES
			. 삭제대상의 자신을 등록한 목록 리스트에서 나를 삭제.
		. NO
			. 무시? or DB에서 읽어서 업데이트?
*/
void CMessenger::When_DELETE (t_HASHKEY HashName, char *szName)
{
	CDLList< tagMyFriend >::tagNODE *pNODE;

	this->m_bUpdate = true;

	m_csFRIEND.Lock ();
	{
		pNODE = m_ListFRIEND.GetHeadNode();
		while( pNODE ) {
			if ( pNODE->m_VALUE.m_HashNAME == HashName ) {
				if ( !strcmpi( pNODE->m_VALUE.m_Name.Get(), szName ) ) {
					m_ListFRIEND.DeleteNFree( pNODE );
					break;
				}
			}

			pNODE = pNODE->GetNext ();
		}
	}
	m_csFRIEND.Unlock ();
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
