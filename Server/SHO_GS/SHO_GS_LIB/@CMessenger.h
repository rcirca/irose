#ifndef	__CMESSENGER_H
#define	__CMESSENGER_H
#include "CDLList.h"

#if !defined( __SHO_WS ) && !defined( __SHO_GS )
	#error >>> ERROR :: must defined __SHO_WS or __SHO_GS
#endif

#ifdef	__SHO_WS
	#include "CWS_Client.h"
	typedef	CWS_Client	classUSER;
#endif

#ifdef	__SHO_GS
	#include "GS_ListUSER.h"
#endif

//-------------------------------------------------------------------------------------------------

// 내가 등록한 친구 리스트
struct tagMyFriend {
	int			m_iSocketIDX;
	BYTE		m_btSTATUS;
	t_HASHKEY	m_HashNAME;
	CStrVAR		m_Name;
	tagMyFriend ()
	{
		m_iSocketIDX = 0;
		m_btSTATUS = 0;
		m_HashNAME = 0;
	}
} ;


class CMessenger {
private:
	CCriticalSection		m_csFRIEND;
	bool					m_bUpdate;

public :
	CDLList< tagMyFriend >	m_ListFRIEND;

	bool IsUPDATE ()		{	return m_bUpdate;	}	// db 갱신해야 하냐?

	void When_LogIN (int iCount, BYTE *pLIST);
	void When_LogOUT ();
	void When_APPEND (BYTE btStatus, int iSocketIDX, char *szName, t_HASHKEY HashNAME=0);
	void When_DELETE (t_HASHKEY HashName, char *szName);
} ;

//-------------------------------------------------------------------------------------------------
#endif
