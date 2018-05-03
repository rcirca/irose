//---------------------------------------------------------------------------

#include "stdAFX.h"
#pragma hdrstop

#include "ThreadSQL.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Unit1::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall CThreadSQL::CThreadSQL(bool CreateSuspended) : TThread(CreateSuspended)
{
    Priority = tpNormal;

    m_pEvent = new TEvent(NULL, false, false, "Event_SQL" );
    m_pSQL = NULL;

    ::InitializeCriticalSection( &m_csSQL );
}

__fastcall CThreadSQL::~CThreadSQL()
{
    SAFE_DELETE( m_pSQL );
    SAFE_DELETE( m_pEvent );

    ::DeleteCriticalSection( &m_csSQL );
}

//---------------------------------------------------------------------------
void __fastcall CThreadSQL::Execute()
{
    classDLLNODE< tagQUERYBUFF > *pSqlNODE, *pDelNODE;

    while( !Terminated ) {
        if ( m_pEvent->WaitFor( INFINITE ) != wrSignaled )
            break;
        m_pEvent->ResetEvent();

        pSqlNODE = m_SqlLIST.GetHeadNode ();
        while( pSqlNODE ) {
            if ( m_pSQL->ExecSQL (pSqlNODE->DATA.m_pBUFF, pSqlNODE->DATA.m_dwLEN) ) {
                // 성공...
                SAFE_DELETE_ARRAY( pSqlNODE->DATA.m_pBUFF );

                pDelNODE = pSqlNODE;
                this->LockSQL ();
                pSqlNODE = m_SqlLIST.GetNextNode( pSqlNODE );
                m_SqlLIST.DeleteNFree( pDelNODE );
                this->UnlockSQL ();
            } else {
                // 실패...
                this->LockSQL ();
                pSqlNODE = m_SqlLIST.GetNextNode( pSqlNODE );
                this->UnlockSQL ();
            }
        }
    }
}

//---------------------------------------------------------------------------
bool CThreadSQL::Connect (char *szServerIP, char *szUser, char *szPassword, char *szDBName)
{
    m_pSQL = new classMYSQL;
    if ( !m_pSQL->Connect( szServerIP, szUser, szPassword ) ) {
        SAFE_DELETE( m_pSQL );
        return false;
    }
    if ( !m_pSQL->SelectDB( szDBName ) ) {
        m_pSQL->Disconnect ();
        SAFE_DELETE( m_pSQL );
        return false;
    }
    return true;
}

void CThreadSQL::Free ()
{
    this->Terminate ();
    this->m_pEvent->SetEvent ();
    if ( this->Suspended ) {
        this->Resume ();
    }
}

//---------------------------------------------------------------------------
bool CThreadSQL::Add_SQL( char *szSqlBuff, DWORD dwBuffSize )
{
    if ( !szSqlBuff || !dwBuffSize )
        return false;

    classDLLNODE< tagQUERYBUFF > *pSqlNODE;

    this->LockSQL ();
    pSqlNODE = m_SqlLIST.AllocNAppend ();
    pSqlNODE->DATA.m_pBUFF = new char[ dwBuffSize ];
    pSqlNODE->DATA.m_dwLEN = dwBuffSize;
    ::CopyMemory( pSqlNODE->DATA.m_pBUFF, szSqlBuff, dwBuffSize);
    this->UnlockSQL ();

    this->m_pEvent->SetEvent ();
    return true;
}

//---------------------------------------------------------------------------
bool CThreadSQL::Add_PACKET( classUSER *pUser, t_PACKET *pPacket )
{


    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

