#ifndef iocpSOCKETH
#define iocpSOCKETH
#include <ScktComp.hpp>
#include "DLLIST.h"
#include "CPacket.h"
#include "classSTR.h"
//---------------------------------------------------------------------------

typedef enum {
	ioREAD,
	ioWRITE
//    ClientIoWrite,
//    ClientQoS
} IO_MODE, IO_OPERATION;


struct tagIO_DATA {
	OVERLAPPED					m_Overlapped;
	IO_MODE						m_IOmode;
	DWORD						m_dwIOBytes;
	CPacket 				   *m_pCPacket;
/*
	int							m_iPoolIndex;
	classDLLNODE<tagIO_DATA>   *m_pPoolNode;
*/
	classDLLNODE<tagIO_DATA>   *m_pNODE;
} ;


class iocpSOCKET
{
private:
    void Recv_Continue (tagIO_DATA *pRecvDATA);
    bool Send_Continue (tagIO_DATA *pSendDATA);
    bool Recv_Done (tagIO_DATA *pRecvDATA);

	CRITICAL_SECTION			m_csSELF;
	CRITICAL_SECTION			m_csSendList;

protected:
	classDLLIST<tagIO_DATA>		m_SendList;			// Client에게 보낼 데이타 리스트.
	bool						m_bSending;			// WriteFile에 보내고 있냐 ?
/*
	CRITICAL_SECTION			m_csRecvList;
	classDLLIST<tagIO_DATA>		m_RecvList;			// Client에서 받은 데이타 리스트.
*/
public :
    SOCKET  m_Socket;
    CStrVAR m_IP;

    iocpSOCKET ();
    ~iocpSOCKET ();

    void Lock ()            {   ::EnterCriticalSection( &m_csSELF );    }
    void Unlock ()          {   ::LeaveCriticalSection( &m_csSELF );    }

    void Clear_LIST (void);
    void CloseSocket (void);

    bool Recv_Start (void);
    void Recv_Complete (tagIO_DATA *pRecvDATA);

    void Send_Start (CPacket *pCPacket);
    void Send_Complete (tagIO_DATA *pSendDATA);

    virtual bool HandlePACKET ( t_PACKET *pPacket )=0;

    static classDLLNODE<tagIO_DATA> *Alloc_RecvIODATA (void);
    static                      void  Free_RecvIODATA (tagIO_DATA *pRecvDATA);

    static classDLLNODE<tagIO_DATA> *Alloc_SendIODATA (CPacket *pCPacket);
    static                      void  Free_SendIODATA (tagIO_DATA *pSendDATA);
} ;

//---------------------------------------------------------------------------
#endif
