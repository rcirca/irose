#ifndef	__CLS_SQLTHREAD_H
#define	__CLS_SQLTHREAD_H
//-------------------------------------------------------------------------------------------------
#include "CLS_Account.h"
#include "CSqlTHREAD.h"
//-------------------------------------------------------------------------------------------------

class CLS_SqlTHREAD : public CSqlTHREAD
{
private:
	CCriticalSection			m_csUserLIST;
	classDLLIST< CLS_Account* >	m_AddUserLIST;
	classDLLIST< CLS_Account* >	m_RunUserLIST;

	// inherit virtual function from CSqlTHREAD...
	bool Run_SqlPACKET( tagQueryDATA *pSqlPACKET );

	bool Proc_cli_LOGIN_REQ ( tagQueryDATA *pSqlPACKET );

	void Execute ();

	CLS_SqlTHREAD ();
	~CLS_SqlTHREAD ();
    
	char	m_szTmpMD5[33];

public :
    bool    m_bCheckLogIN;
    DWORD   m_dwCheckRIGHT;

	void Set_EVENT ()		{	m_pEVENT->SetEvent ();		}
	bool Add_SqlPACKET  (DWORD dwSocketID, char *szAccount, t_PACKET *pPacket);
	bool Add_LogOutUSER (CLS_Account *pCAccount);

	DECLARE_INSTANCE( CLS_SqlTHREAD )
} ;
extern CLS_SqlTHREAD *g_pThreadSQL;

//-------------------------------------------------------------------------------------------------
#endif