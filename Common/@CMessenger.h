#ifndef	__CMESSENGER_H
#define	__CMESSENGER_H
#include "CDLList.h"

/*
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
*/

class CWS_Client;
class CMessenger;

//-------------------------------------------------------------------------------------------------

#define	MAX_FRIEND_COUNT		40	// < ( max_packet_size / max_char_name_len )

struct tagFriend_H {
	DWORD		m_dwDBID;
	BYTE		m_btSTATUS;
} ;

// 내가 등록한 친구 리스트
struct tagMyFriend : public tagFriend_H {
	CMessenger *m_pUSER;
	CStrVAR		m_Name;
	tagMyFriend ()
	{
		m_pUSER = NULL;
	}
} ;


class CMessenger {
private:
	CDLList< tagMyFriend >	m_ListFRIEND;
	CCriticalSection		m_csFRIEND;
	bool					m_bFriendUPDATE;

public :
	BYTE					m_btMsgrSTATUS;

	CMessenger ()
	{
		m_bFriendUPDATE = false;
	}

	bool MSGR_IsUPDATE ()					{	return m_bFriendUPDATE;	}	// db 갱신해야 하냐?

	bool MSGR_Add( CWS_Client *pFriend );	// 친구 추가
	
	void MSGR_Del( DWORD dwDBID );			// 친구 삭제
	void MSGR_Ban( DWORD dwDBID );			// 채팅 차단
	void MSGR_Status( DWORD dwDBID, BYTE btNewStatus );

	void MSGR_LogIN (int iCount, BYTE *pLIST);
	void MSGR_LogOUT ();

	BYTE MSGR_OnOffLine ( classPACKET *pCPacket, CMessenger *pFriend, DWORD dwDBID, BYTE btStatus );

	virtual DWORD Get_DBID ()=0;
    virtual bool SendPacket( classPACKET *pCPacket )=0;
} ;

//-------------------------------------------------------------------------------------------------
#endif
