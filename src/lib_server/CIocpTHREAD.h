#ifndef	__CIOCPTHREAD_H
#define	__CIOCPTHREAD_H
#include "classTHREAD.h"
//-------------------------------------------------------------------------------------------------

class CIocpTHREAD : public classTHREAD {
private:
protected :
	HANDLE       m_hIOCP;
    int          m_iThreadNO;

	virtual void Execute ();

public :
	CIocpTHREAD (bool bCreateSuspended);
	virtual ~CIocpTHREAD ()	{	/* nop */	}

	bool Init (HANDLE hIOCP, int iThreadNO);
	void Free (void);

    virtual void STATUS_ReturnTRUE  (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey, DWORD dwBytesIO)=0;
    virtual void STATUS_ReturnFALSE (LPOVERLAPPED lpOverlapped, DWORD dwCompletionKey)=0;
} ;

//-------------------------------------------------------------------------------------------------
#endif
