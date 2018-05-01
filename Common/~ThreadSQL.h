//---------------------------------------------------------------------------

#ifndef ThreadSQLH
#define ThreadSQLH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Syncobjs.hpp>
#include "classMYSQL.h"
#include "classSTR.h"
#include "DLLIST.h"

class classUSER;
struct t_PACKET;

struct tagQUERYBUFF {
    classUSER   *m_pUSER;
    t_PACKET    *m_pPacket;
    
    char        *m_pBUFF;
    DWORD        m_dwLEN;
} ;

//---------------------------------------------------------------------------
class CThreadSQL : public TThread
{
private:
    TEvent          *m_pEvent;
    classMYSQL      *m_pSQL;
    CRITICAL_SECTION m_csSQL;
    classDLLIST< tagQUERYBUFF >  m_SqlLIST;

    void LockSQL()      {   ::EnterCriticalSection( &m_csSQL );     }
    void UnlockSQL()    {   ::LeaveCriticalSection( &m_csSQL );     }

protected:
    void __fastcall Execute();

public:
    __fastcall CThreadSQL(bool CreateSuspended);
    __fastcall ~CThreadSQL();

    bool Connect (char *szServerIP, char *szUser, char *szPassword, char *szDBName);
    void Free ();
    bool Add_SQL( char *szSqlBuff, DWORD dwBuffSize );
    bool Add_PACKET( classUSER *pUser, t_PACKET *pPacket );
};
//---------------------------------------------------------------------------
#endif
