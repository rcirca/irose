#ifndef __CACCOUNT_H
#define __CACCOUNT_H
#include "classHASH.h"
#include "DLLIST.h"
//-------------------------------------------------------------------------------------------------

class CAccount {
private :
public  :
    String                  m_stAccount;
//  unsigned long           m_ulHashKey;
    // join server group id
    classDLLNODE<CAccount*>*m_pListNODE;
} ;


class CAccountLIST {
private :
    classDLLIST<CAccount*>  m_AccountLIST;
    int                     m_iMaxLoginCNT;

    bool      Add (char *szAccount);
    void      Del (char *szAccount);
public  :
    CAccountLIST ()         {   m_iMaxLoginCNT=0;   }
           
    classHASH<CAccount*>    m_HashTABLE;

                            //         m_AccountLIST.GetNodeCount();
    int  GetCount ()        {   return m_HashTABLE.GetCount (); }

    short LogIN (char *szAccount, char *szPassword);
    void  LogOUT(char *szAccount);
    
    CAccount *FindACCOUNT(char *szAccount);
} ;
extern CAccountLIST g_AccountLIST;

//-------------------------------------------------------------------------------------------------
#endif

 