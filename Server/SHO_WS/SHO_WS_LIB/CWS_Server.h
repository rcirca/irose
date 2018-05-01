#ifndef	__CWS_SERVER_H
#define	__CWS_SERVER_H
//-------------------------------------------------------------------------------------------------

class CWS_Client;
class CWS_Party;

class CWS_Server : public iocpSOCKET
{
private:
	CStrVAR	m_TmpStr;

	int							   m_iUserBuffSIZE;
	CWS_Client					 **m_ppChannelUSER;
//  classDLLIST< CLS_Account* >    m_AccountLIST;
	CCriticalSection	  m_csListNODE;

	bool Recv_zws_SERVER_INFO (t_PACKET *pPacket);

	bool Send_wls_CONFIRM_ACCOUNT_REPLY (BYTE btResult, DWORD dwWSID, DWORD dwGSID, CWS_Client *pCLIENT=NULL);//DWORD dwLoginTIME, DWORD dwRight=0, char *szAccount=NULL, char *szChar=NULL, bool bFirstZONE=false, DWORD dwLSID=0 );
	bool Recv_zws_CONFIRM_ACCOUNT_REQ( t_PACKET *pPacket );

	bool Recv_zws_SUB_ACCOUNT( t_PACKET *pPacket );

	bool Recv_gsv_CHANGE_CHAR( t_PACKET *pPacket );

	bool Recv_gsv_CHEAT_REQ( t_PACKET *pPacket );

	bool Recv_gsv_SET_WORLD_VAR( t_PACKET *pPacket );
	bool Recv_srv_ACTIVE_MODE( t_PACKET* pPacket );
	bool Recv_zws_ACCOUNT_LIST( t_PACKET* pPacket );
	bool Recv_srv_USER_LIMIT( t_PACKET *pPacket );
	bool Recv_zws_CREATE_CLAN( t_PACKET *pPacket );
	bool Recv_gsv_ADJ_CLAN_VAR( t_PACKET *pPacket );

public :
/*
	bool Recv_gsv_WARP_USER( t_PACKET *pPacket );
*/
	bool Send_wsv_CHEAT_REQ( t_PACKET *pPacket, DWORD dwReqGSID, DWORD dwReplyGSID, char *szCode );

public :
	void				 *m_pSrvListITEM;
	void LockLI ()		{	m_csListNODE.Lock();	}
	void UnlockLI ()	{	m_csListNODE.Unlock();	}

//  classDLLNODE< CWS_Server* > m_ListNODE;
	CDLList< CWS_Server* >::tagNODE	m_listNODE;

    CStrVAR m_ServerNAME;
    CStrVAR m_ServerIP;
    WORD    m_wListenPORT;
    DWORD   m_dwRandomSEED;

	BYTE	m_btChannelNO;
	BYTE	m_btLowAGE;
	BYTE	m_btHighAGE;

    DWORD   m_dwCheckSEC;
    DWORD   m_dwAcceptRIGHT;

	short	m_nBeforePERCENT;
	short	m_nUserPERCENT;
    int     m_iCurUserCNT;
    int     m_iMaxUserCNT;
	DWORD	m_dwLimitUserCNT;

	bool	m_bActive;

	CWS_Server ();
	~CWS_Server ();

	void Init ();
	void Free ();

	void AddChannelCLIENT( DWORD dwGSClientIDX, CWS_Client *pClient );
	void SubChannelCLIENT( CWS_Client *pClient );
	CWS_Client *GetChannelCLIENT( WORD wGSClientIDX )					
	{	
		return m_ppChannelUSER[ wGSClientIDX ];	
	}

	inline bool SendPacket( classPACKET *pCPacket )		{   return this->Send_Start( pCPacket );	}
	bool Send_str_PACKET (WORD wType, char *szString);

private :
	//----------------------- virtual function inherit from iocpSOCKET
    bool HandlePACKET( t_PACKETHEADER *pPacket );
} ;


//-------------------------------------------------------------------------------------------------
class CWS_ListSERVER : public IOCPSocketSERVER, public CDataPOOL< CWS_Server >
{
private :
	CCriticalSection			m_csLIST;
//	CDLList< CWS_Server* >		m_listSERVER;
	void	LockLIST()		{	m_csLIST.Lock();	}
	void	UnlockLIST()	{	m_csLIST.Unlock();	}

	short	m_nChannelCNT;
	CWS_Server	*m_ppChannelSERVER[ MAX_CHANNEL_SERVER ];

public :
	// Worker Thread 갯수 = CPU * 1 + 1
	CWS_ListSERVER( UINT uiInitDataCNT, UINT uiIncDataCNT );
	~CWS_ListSERVER();

	void Send_ToWORLD( classPACKET *pCPacket );
    bool Add_ServerList2Packet( classPACKET *pCPacket, DWORD dwRight );
	void Send_gsv_ANNOUNCE_CHAT( char *szMessage, char *szAccount );
    void Send_lsv_CHECK_ALIVE (void);
	void Make_wls_CHANNEL_LIST( classPACKET *pCPacket );

	void Set_ChannelACTIVE( int iChannelNo, bool bActive, bool bToggle=false );
	bool Set_ChannelSERVER( BYTE btChannelNO, CWS_Server *pServer );
	
	void Make_srv_SERVER_LIST_REPLY( classPACKET *pCPacket );

	bool Send_zws_SET_USER_CLAN( CWS_Client *pClient );
	bool Send_zws_DEL_USER_CLAN( CWS_Client *pClient );

	short Get_ChannelCOUNT ()		{	return m_nChannelCNT;	}

	CWS_Server*Get_ChannelSERVER ( BYTE btChannelNO )
	{
		if ( btChannelNO >= MAX_CHANNEL_SERVER )
			return NULL;

		this->LockLIST();
			CWS_Server *pServer = m_ppChannelSERVER[ btChannelNO ];
		this->UnlockLIST ();
		return pServer;
	}
	void Del_ChannelSERVER (int iChannelNo);

	//-------------------------------------------------------------------------------------------------
	/// Inherited from IOCPSocketSERVER
	iocpSOCKET*	AllocClientSOCKET()
	{
		// 메모리할당
		CWS_Server *pSOCKET = this->Pool_Alloc ();
		if ( pSOCKET ) {
			pSOCKET->Init ();
		/*
			this->LockLIST ();
				m_listSERVER.AppendNode( &pSOCKET->m_listNODE );
			this->UnlockLIST ();
		*/
		}
		return (iocpSOCKET*)pSOCKET;
	}
	void FreeClientSOCKET( iocpSOCKET *pSOCKET )
	{
		/*
		this->LockLIST ();
			m_listSERVER.DeleteNode( &((CWS_Server*)pSOCKET)->m_listNODE );
		this->UnlockLIST ();
		*/
		this->Set_ChannelSERVER( ((CWS_Server*)pSOCKET)->m_btChannelNO, NULL );
 
		g_LOG.CS_ODS (0xffff, ">>> Delete World SERVER : %s, %s  ( %s UsedCnt: %d ) \n", 
						((CWS_Server*)pSOCKET)->m_ServerNAME.Get(), 
						pSOCKET->m_IP.Get(), 
						this->GetPoolNAME(), 
						this->GetUsedCNT()-1 );

		// 검증없이 메모리 해제
		((CWS_Server*)pSOCKET)->Free ();
		this->Pool_Free( (CWS_Server*)pSOCKET );
	}
	void ClosedClientSOCKET( iocpSOCKET *pSOCKET )
	{
		this->FreeClientSOCKET( pSOCKET );
	}
} ;
extern CWS_ListSERVER   *g_pListSERVER;


//-------------------------------------------------------------------------------------------------
#endif
