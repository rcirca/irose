
#include "stdAFX.h"
#include <windows.h>
#include <stdio.h>
#include "classLOG.h"
#include "ListACCOUNT.h"


extern void UpdateAccountCounter (String stMSG);

//-------------------------------------------------------------------------------------------------
CAccount *CAccountLIST::FindACCOUNT (char *szAccount)
{
    unsigned long ulHashKEY = StrToHashKey(szAccount);

    CAccount *pCAccount = m_HashTABLE.Search( ulHashKEY );
    while( pCAccount ) {
        if ( 0 == strcmpi (pCAccount->m_stAccount.c_str(), szAccount ) )
            return pCAccount;

        pCAccount = m_HashTABLE.SearchContinue( ulHashKEY );
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------
bool CAccountLIST::Add (char *szAccount)
{
    CAccount *pCAccount = this->FindACCOUNT(szAccount);
    if ( pCAccount ) {
        LogString( LOG_DEBUG, "Already logged in account: [%s],[%s]... ",
            szAccount, pCAccount->m_stAccount.c_str() );

        return false;
    }

    unsigned long ulHashKEY = StrToHashKey(szAccount);

    pCAccount = new CAccount;
    pCAccount->m_stAccount = szAccount;

    m_HashTABLE.Insert ( ulHashKEY, pCAccount );

    pCAccount->m_pListNODE = m_AccountLIST.AllocNAppend ();
    pCAccount->m_pListNODE->DATA = pCAccount;

    if ( this->GetCount () > m_iMaxLoginCNT ) m_iMaxLoginCNT = this->GetCount ();
    
    String stCNT;
    stCNT.printf ( "Logged in: %d/%d", this->GetCount (), m_iMaxLoginCNT );
    UpdateAccountCounter( stCNT );

    return true;
}

//-------------------------------------------------------------------------------------------------
void CAccountLIST::Del (char *szAccount)
{
    CAccount *pCAccount = this->FindACCOUNT(szAccount);
    if ( pCAccount ) {
        unsigned long ulHashKEY = StrToHashKey(szAccount);

        CAccount *pCAccount = m_HashTABLE.Delete ( ulHashKEY );
        m_AccountLIST.DeleteNFree ( pCAccount->m_pListNODE );

        delete pCAccount;
    } else
        LogString( LOG_DEBUG, "ERROR:: delete from logged in account: [%s]... ", szAccount);
        
    String stCNT;
    stCNT.printf ( "Logged in: %d/%d", this->GetCount (), m_iMaxLoginCNT );
    UpdateAccountCounter( stCNT );
}

//-------------------------------------------------------------------------------------------------
short CAccountLIST::LogIN (char *szAccount, char *szPassword)
{
/*
#define RESULT_LOGIN_REPLY_OK                   0x00
#define RESULT_LOGIN_REPLY_FAILED               0x01    // 오류
#define RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT    0x02    // 계정 없다.
#define RESULT_LOGIN_REPLY_INVALID_PASSWORD     0x03    // 비번 오류
#define RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN     0x04    // 이미 로그인 중이다
#define RESULT_LOGIN_REPLY_REFUSED_ACCOUNT      0x05    // 서버에서 거부된 계정이다.
#define RESULT_LOGIN_REPLY_NEED_CHARGE          0x06    // 충전이 필요한다
*/
    // check account & password from DB
    if ( szAccount == NULL ) {
        return RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT;
    }
    if ( szPassword == NULL ) {
        return RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT;
    }

    if ( !this->Add (szAccount) ) {
        return RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN;
    }

    return RESULT_LOGIN_REPLY_OK;
}
void CAccountLIST::LogOUT (char *szAccount)
{
    this->Del (szAccount);
}

//-------------------------------------------------------------------------------------------------


