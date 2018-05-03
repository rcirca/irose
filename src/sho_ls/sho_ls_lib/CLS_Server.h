#ifndef	__CLS_SERVER_H
#define	__CLS_SERVER_H

#include "CLS_Account.h"
#include "CDLList.h"
#include "SHO_LS_LIB.h"
//-------------------------------------------------------------------------------------------------

struct tagChannelSRV {
public :
	BYTE	m_btChannelNO;
	BYTE	m_btLowAGE;
	BYTE	m_btHighAGE;
	short	m_nUserPERCENT;
	CStrVAR	m_ChannelNAME;
	void Init ()
	{
		m_btChannelNO = 0;
		m_nUserPERCENT = 0;
		m_ChannelNAME.Set( NULL );
	}
} ;

class CLS_Server : public iocpSOCKET
{
private:
    classDLLIST< CLS_Account* >    m_AccountLIST;

    bool    Recv_gsv_CHECK_ALIVE (t_PACKET *pPacket);

    bool    Recv_zws_SERVER_INFO (t_PACKET *pPacket);

    bool    Recv_zws_ADD_ACCOUNT (t_PACKET *pPacket);
    bool    Recv_zws_SUB_ACCOUNT (t_PACKET *pPacket);

    bool    Recv_zws_CONFIRM_ACCOUNT_REQ (t_PACKET *pPacket);
    bool    Send_wls_CONFIRM_ACCOUNT_REPLY (BYTE btResult, DWORD dwLSID, DWORD dwGSID, DWORD dwLoginTIME, DWORD dwRight=0, DWORD dwPayFLAG=0, BYTE btChannelNO=0, char *szAccount=NULL);

	bool	Recv_wls_CHANNEL_LIST (t_PACKET *pPacket);
	bool	Recv_srv_ACTIVE_MODE (t_PACKET *pPacket);

	bool	Recv_wls_ACCOUNT_LIST (t_PACKET *pPacket);
	

	void	ClearChannels ();

	void	Inc_UserCNT ();
	void	Dec_UserCNT ();

public :
    bool    Send_str_PACKET( WORD wType, char *szString );
	void	Make_lsv_CHANNEL_LIST_REPLY( classPACKET *pCPacket );

public :
//    TListItem   *m_pListITEM;
    classDLLNODE< CLS_Server* > m_ListNODE;

	CCriticalSection			m_csLIST;
	CDLList< tagChannelSRV* >	m_ListCHANNEL;
	tagChannelSRV			    m_CHANNEL[ MAX_CHANNEL_SERVER ];

    CStrVAR m_ServerNAME;
    CStrVAR m_ServerIP;
    WORD    m_wListenPORT;
    DWORD   m_dwRandomSEED;
	bool	m_bActive;

    DWORD   m_dwCheckSEC;
    DWORD   m_dwAcceptRIGHT;

    int     m_iCurUserCNT;
    int     m_iMaxUserCNT;

	void   *m_pSrvListITEM;

	CLS_Server ();
	~CLS_Server ();

	void Init ()
	{
		m_bActive	 = true;
	    m_Socket     = INVALID_SOCKET;
		m_iSocketIDX = 0;					// CUserLIST::InitData

		m_ListNODE.DATA = this;
		m_pSrvListITEM = NULL;
	}
	void Free ();

private :
	//----------------------- virtual function inherit from iocpSOCKET
    bool HandlePACKET( t_PACKETHEADER *pPacket );
} ;


//-------------------------------------------------------------------------------------------------
class CLS_ListSERVER : public IOCPSocketSERVER, public CDataPOOL< CLS_Server >
{
private :
	CCriticalSection			m_csLIST;
	classDLLIST< CLS_Server* >	m_LIST;

	void	LockLIST()		{	m_csLIST.Lock();	}
	void	UnlockLIST()	{	m_csLIST.Unlock();	}

public :
	int		m_iCurUserCNT;
	int		m_iMaxUserCNT;
	char	m_szStatusSTR[ 64 ];

	// Worker Thread 갯수 = CPU * 1 + 1
	CLS_ListSERVER( UINT uiInitDataCNT, UINT uiIncDataCNT )
								: IOCPSocketSERVER( "CLS_ServerSOCKET", 1, 1, false ),
								  CDataPOOL< CLS_Server > ("CServerPOOL", uiInitDataCNT, uiIncDataCNT ),
								  m_csLIST( 4000 )
	{
		m_iCurUserCNT = m_iMaxUserCNT = 0;
	}
	~CLS_ListSERVER()
	{
		;
	}

    bool Add_ServerList2Packet( classPACKET *pCPacket, DWORD dwRight );
    void Send_lsv_ANNOUNCE_CHAT (void *pServer, char *szChatMSG);
    void Send_lsv_CHECK_ALIVE (void);

	short Get_ServerCOUNT()		{	return m_LIST.GetNodeCount();		}
	void  Make_srv_SERVER_LIST_REPLY (classPACKET *pCPacket);

	//-------------------------------------------------------------------------------------------------
	/// Inherited from IOCPSocketSERVER
	iocpSOCKET*	AllocClientSOCKET()
	{
		// 메모리할당
		CLS_Server *pSOCKET = this->Pool_Alloc ();
		if ( pSOCKET ) {
			pSOCKET->Init ();

			this->LockLIST ();
				m_LIST.AppendNode( &pSOCKET->m_ListNODE );
			this->UnlockLIST ();
		}
		return (iocpSOCKET*)pSOCKET;
	}
	void FreeClientSOCKET( iocpSOCKET *pSOCKET )
	{
		CLS_Server *pServer = (CLS_Server*)pSOCKET;

		pServer->LockSOCKET ();
		if ( pServer->m_pSrvListITEM ) {
			SHO_LS::ExeAPI()->DelWorldITEM( pServer->m_pSrvListITEM );
			pServer->m_pSrvListITEM = NULL;
		}
		pServer->UnlockSOCKET ();

		this->LockLIST ();
			m_LIST.DeleteNode( &pServer->m_ListNODE );
		this->UnlockLIST ();

		g_LOG.CS_ODS (0xffff, "%%% Delete World SERVER : %s, %s  ( %s UsedCnt: %d ) \n", 
						pServer->m_ServerNAME.Get(), 
						pSOCKET->m_IP.Get(), 
						this->GetPoolNAME(), 
						this->GetUsedCNT()-1 );

		// 검증없이 메모리 해제
		((CLS_Server*)pSOCKET)->Free ();
		this->Pool_Free( pServer );
	}
	void ClosedClientSOCKET( iocpSOCKET *pSOCKET )
	{
		this->FreeClientSOCKET( pSOCKET );
	}
} ;

extern CLS_ListSERVER   *g_pListSERVER;


//-------------------------------------------------------------------------------------------------
#endif
