#ifndef	__CTHREAD_MSGR_H
#define	__CTHREAD_MSGR_H
#if defined(__SHO_WS) || defined(__INC_WORLD)
//-------------------------------------------------------------------------------------------------

#define	MAX_FRIEND_COUNT		35	// < ( max_packet_size / max_char_name_len )

class CMessenger;

#ifdef	__INC_WORLD
	class classUSER;
	typedef	classUSER	CWS_Client;
#endif
	
#ifdef __SHO_WS
	class CWS_Client;
#endif

#include "CSqlTHREAD.h"

//-------------------------------------------------------------------------------------------------
#pragma pack (push, 1)
struct tagFriend_H {
	DWORD		m_dwDBID;
	BYTE		m_btSTATUS;
} ;
#pragma pack (pop)

// 내가 등록한 친구 리스트
struct tagMyFriend : public tagFriend_H {
	CMessenger *m_pMSGR;
	CStrVAR		m_Name;
	tagMyFriend ()
	{
		m_pMSGR = NULL;
	}
} ;

//-------------------------------------------------------------------------------------------------
class CMessenger // : public CS
{
private:
	CDLList< tagMyFriend >	m_ListFRIEND;
	bool					m_bFriendUPDATE;

public :
	CStrVAR		m_Name;
	BYTE		m_btMsgrSTATUS;
	DWORD		m_dwDBID;
	int			m_iOwnerSocetIDX;

	void Init (char *szName, DWORD dwDBID, int iSocketIdx)
	{
		m_Name.Set( szName );
		m_btMsgrSTATUS	 = FRIEND_STATUS_OFFLINE;
		m_dwDBID		 = dwDBID;
		m_iOwnerSocetIDX = iSocketIdx;
		m_bFriendUPDATE  = false;
	}

	bool SendPacket (int iClientSocketIDX, DWORD dwDBID, classPACKET *pCPacket);

	int   Get_FriendCNT()					{	return m_ListFRIEND.GetNodeCount();		}
	DWORD Get_DBID ()						{	return m_dwDBID;						}

	bool MSGR_IsUPDATE ()					{	return m_bFriendUPDATE;	}	// db 갱신해야 하냐?

	bool MSGR_Add( CMessenger *pFriend );	// 친구 추가
	
	void MSGR_Del( DWORD dwDBID );			// 친구 삭제
	void MSGR_Ban( DWORD dwDBID );			// 채팅 차단
	void MSGR_Status2ONE( DWORD dwDBID, BYTE btNewStatus, CMessenger *pMessenger );
	void MSGR_Status2ALL( BYTE btNewStatus );
	void MSGR_Msg( t_PACKET *pPacket );

	void MSGR_LogIN (int iCount, BYTE *pLIST);
	int  MSGR_LogOUT (BYTE *pOutBUFF);

	BYTE MSGR_OnOffLine ( classPACKET *pCPacket, CMessenger *pFriend, DWORD dwDBID, BYTE btStatus );
} ;


//-------------------------------------------------------------------------------------------------
struct tagMSGR_CMD {
	int			 m_iSocketIDX;
	DWORD		 m_dwDBID;
	BYTE		 m_btCMD;
	t_PACKET	*m_pPacket;
	CStrVAR		 m_Name;
} ;

class CThreadMSGR : public CSqlTHREAD
{
private:
	CDataPOOL< CMessenger >  m_Pools;
	classHASH< CMessenger* > m_HashMSGR;

	CMessenger *AllocMEM ()					
	{	
	//	return this->m_Pools.Pool_Alloc ();
		CMessenger *pMSGR = new CMessenger;
		return pMSGR;
	}
	void FreeMEM (CMessenger *pMSGR)		
	{	
	//	m_Pools.Pool_Free( pMSGR );
		delete pMSGR;
	}

	void Check_FRIENDS ();
	void Check_ItemCNT (int iItemType, int iItemNo);
	void Execute ();

	CDLList< tagMSGR_CMD >	m_ProcCMD;
	CDLList< tagMSGR_CMD >	m_WaitCMD;
	BYTE *m_pListBUFF;

	// inherit virtual function from CSqlTHREAD...
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET )	{	return true;	}

	bool Run_MessengerPACKET( tagMSGR_CMD *pMsgCMD );
	bool LogIN  (tagMSGR_CMD *pCMD);
	void LogOUT (CMessenger *pMSGR);

public :
	CThreadMSGR( UINT uiInitDataCNT, UINT uiIncDataCNT );
	virtual ~CThreadMSGR ();

	void Set_EVENT ()		{	m_pEVENT->SetEvent ();	}
	void Add_MessengerCMD (char *szCharName, BYTE btCMD, t_PACKET *pPacket, int iSocketIDX, DWORD dwDBID=0);
	CMessenger *SearchMSGR (char *szCharName);
} ;
extern CThreadMSGR	*g_pThreadMSGR;
//-------------------------------------------------------------------------------------------------
#endif
#endif