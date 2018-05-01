#ifndef	__CMESSENGER_H
#define	__CMESSENGER_H
#include "CDLList.h"
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
public :
	CDLList< tagMyFriend >	m_ListFRIEND;

	void When_LogIN (int iCount, BYTE *pLIST);
	void When_LogOUT ();
	void When_APPEND (BYTE btStatus, int iSocketIDX, char *szName, t_HASHKEY HashNAME=0);
	void When_DELETE (t_HASHKEY HashName, char *szName);
} ;

//-------------------------------------------------------------------------------------------------
#endif
