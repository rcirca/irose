#ifndef __CCONNECTION_H
#define __CCONNECTION_H
#include <vcl.h>
#include <ScktComp.hpp>
#include <mmsystem.h>
#include "classLOG.h"
#include "DLLIST.h"
#include "NET_Prototype.h"
#include "classPACKET.h"
//---------------------------------------------------------------------------

class CConnector {
private  :
    int SendBuf     (void *pBuf, int iCount);
    int ReceiveBuf  (void *pBuf, int iCount);

protected:
    short                           m_nRecvByte;            // 현재까지 소켓에서 읽은 바이트수...
    t_PACKET                       *m_pRecvPket;
    short                           m_nSendByte;

    virtual void PacketHandle (void);

public   :
    TCustomWinSocket               *m_pSocket;
    CRITICAL_SECTION                m_csSENDq;
    
    #ifdef  __CRITICAL_PACKET
        inline void LockSENDq()     {   ::EnterCriticalSection( &m_csSENDq ); }
        inline void UnlockSENDq()   {   ::LeaveCriticalSection( &m_csSENDq ); }
    #else
        inline void LockSENDq()     {   ;   }
        inline void UnlockSENDq()   {   ;   }
    #endif

    bool                            m_bIsWritable;          // 소켓이 쓰기 가능한 상태인가 ??
    DWORD                           m_dwLastActionTime;
    bool                            m_bCloseSocket;

    classDLLIST <classPACKET*>      m_SendPketQ;

    CConnector ();
    ~CConnector ();

    bool _Init (TCustomWinSocket *pSocket);
    void _Free (void);

    void ON_Read (void);
    void ON_Write (void)                {   m_bIsWritable = true;   }

    virtual bool AppendSendPacket (classPACKET *pCPacket);
    bool SendPacket (void);
} ;


//---------------------------------------------------------------------------
template <class dType>
class CConnectorLIST {
protected:
    classDLLIST <dType>   m_LIST;

public :
    CRITICAL_SECTION            m_csLIST;
    #ifdef  __THREAD_CLIENTMANAGER
        inline void LockLIST()       {   ::EnterCriticalSection( &m_csLIST ); }
        inline void UnlockLIST()     {   ::LeaveCriticalSection( &m_csLIST ); }
    #else
        inline void LockLIST()       {   ;   }
        inline void UnlockLIST()     {   ;   }
    #endif

    CConnectorLIST ()    {};
    ~CConnectorLIST ()   {};

    int  GetCount () {  return m_LIST.GetNodeCount();   }
    void FreeLIST ();

    virtual bool Connect (TCustomWinSocket *Socket);
    virtual void Disconnect (classDLLNODE <dType> *pConnNODE);
    
    void ProcSEND (void);
} ;


//-------------------------------------------------------------------------------------------------
template <class dType>
void CConnectorLIST<dType>::FreeLIST ()
{
    Log_String(LOG_DEBUG, ">>>>>> CConnectorLIST<dType>->FreeLIST");

    classDLLNODE <dType> *pNode;

    this->LockLIST ();	// classCLIENTMANAGER::_Free
        pNode = m_LIST.GetHeadNode ();
        while ( pNode != NULL ) {
            if ( pNode->DATA.m_pSocket->Connected ) {
                Log_String (LOG_DEBUG, "Close client socket ....\n");
                pNode->DATA.m_pSocket->Close ();
            }

            // Event FD_COLSOE 발생에 의해 유저 노드가 삭제될것이다.

            pNode = m_LIST.GetHeadNode ();
        }
    this->UnlockLIST ();

    Log_String(LOG_DEBUG, "<<<<<< CConnectorLIST<dType>->FreeLIST");
}

template <class dType>
bool CConnectorLIST<dType>::Connect (TCustomWinSocket *Socket)
{
    classDLLNODE <dType> *pNewNode;

    pNewNode = new classDLLNODE <dType>;
    if ( pNewNode == NULL ) {
        ;
        // Error Message ...
        // FormLogMODE->Log_String (LOG_NORMAL, "ERROR[%s:%d] : New Connection Memory Allocation Failed ...\n", __FILE__, __LINE__);
        ;
        return false;
    }

    pNewNode->DATA._Init (Socket);
    Socket->Data = pNewNode;

    this->LockLIST ();	// classCLIENTMANAGER::Connect
        m_LIST.AppendNode (pNewNode);
    this->UnlockLIST ();

    return true;
}

template <class dType>
void CConnectorLIST<dType>::Disconnect (classDLLNODE <dType> *pConnNODE)
{
    this->LockLIST ();
        m_LIST.DeleteNode (pConnNODE);
    this->UnlockLIST ();

	pConnNODE->DATA._Free ();
	delete pConnNODE;
}

template <class dType>
void CConnectorLIST<dType>::ProcSEND (void)
{
    static classDLLNODE <dType> *pConnNODE;
    static DWORD dwCurrentTime;

    dwCurrentTime = timeGetTime ();

//    this->m_bReadyToSEND = false;
    this->LockLIST ();

    pConnNODE = m_LIST.GetHeadNode ();
    while ( pConnNODE ) {
        if ( !pConnNODE->DATA.SendPacket () ) {
            ;
        }

        pConnNODE = m_LIST.GetNextNode (pConnNODE);
    }
    this->UnlockLIST ();
}


//---------------------------------------------------------------------------
#endif

