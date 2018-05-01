#ifndef	__CACCECTTHREAD_H
#define	__CACCECTTHREAD_H
#include "classTHREAD.h"
//-------------------------------------------------------------------------------------------------

class CAcceptTHREAD : public classTHREAD {
private:
protected :
	SOCKET	m_ListenSocket;

    void  SocketERROR ();
	virtual void Execute ();

public :
	CAcceptTHREAD (bool bCreateSuspended);
	virtual ~CAcceptTHREAD ()	{	/* nop */	}

	bool Init (int iTCPPort, int iKeepAliveSec);
	void Free (void);

    virtual bool AcceptSOCKET (SOCKET hSocket, sockaddr_in &SockADDR)=0;
} ;

//-------------------------------------------------------------------------------------------------
#endif
