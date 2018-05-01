#ifndef	__CWS_ACCOUNT_H
#define	__CWS_ACCOUNT_H
//-------------------------------------------------------------------------------------------------

#define	BIT_LOGIN_WS	0x01
#define	BIT_LOGIN_GS	0x02

// #define	__SHOW_USER_LISTVIEW

class CWS_Client;

class CWS_Account {
private:
public :
	DWORD		m_dwLSID;
	DWORD		m_dwWSID;
	DWORD		m_dwGSID;

	DWORD		m_dwLoginTIME;
	BYTE		m_btChannelNO;

	BYTE		m_btLoginBIT;

	CStrVAR		m_Account;
	CWS_Client *m_pCLIENT;

#ifdef	__SHOW_USER_LISTVIEW
	void	   *m_pListITEM;
#endif

	CWS_Account ()
	{
		m_btLoginBIT = 0;
		m_pCLIENT = NULL;
#ifdef	__SHOW_USER_LISTVIEW
		m_pListITEM = NULL;
#endif
	}
	~CWS_Account ();

	void Add_WSBit ( CWS_Client *pClient, char *szAccount, DWORD dwLSID, DWORD dwWSID, DWORD dwLoginTime )
	{
		m_pCLIENT = pClient;
		m_Account.Set( szAccount );
		m_dwLSID = dwLSID;
		m_dwWSID = dwWSID;
		m_dwLoginTIME = dwLoginTime;

		m_btLoginBIT |= BIT_LOGIN_WS;
	}
	void Add_GSBit (DWORD dwGSID )
	{
		// GS에서 인증 요청오면 ...
		m_dwGSID = dwGSID;
		m_btLoginBIT |= BIT_LOGIN_GS;
	}
	BYTE Sub_LoginBIT ( BYTE btBIT )
	{
		m_btLoginBIT &= ~btBIT;
		return m_btLoginBIT;
	}


	char *Get_ACCOUNT()		{	return m_Account.Get();		}
} ;
/*
class CWS_Server;
class CWS_AccountLIST
{
private:
	static CDataPOOL< CWS_Account >	*m_pAccountPOOL;

	CStrVAR							 m_Name;
    classHASH< CWS_Account* >		*m_pHashACCOUNT;
    CCriticalSection	m_csListACCOUNT;

public :
	CWS_AccountLIST (char *szLisNAME, int iHashTableSIZE);
	~CWS_AccountLIST ();

	CWS_Account *Mem_NEW ()						
	{	
		CWS_Account *pCAccount = m_pAccountPOOL->Pool_Alloc();
		if ( pCAccount )
			pCAccount->Init ();
		return pCAccount;
	}
	void	Mem_DEL( CWS_Account *pCAccount )	
	{	
		// LogString (LOG_NORMAL, "   $$$ [%s] deleted in %s UsedCnt: %d \n", pCAccount->m_Account.Get(), m_pAccountPOOL->GetPoolNAME(), m_pAccountPOOL->GetUsedCNT()-1 );
		m_pAccountPOOL->Pool_Free( pCAccount );	
	}

    int		GetCount ()     {   return m_pHashACCOUNT->GetCount();    }

    bool	Insert_ACCOUNT (char *szAccount, CWS_Server *pWorldServer);
	void	Delete_ACCOUNT (char *szAccount);

    CWS_Account*Search_ACCOUNT (char *szAccount);
} ;
*/

//-------------------------------------------------------------------------------------------------
#endif