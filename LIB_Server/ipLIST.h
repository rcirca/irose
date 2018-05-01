/*
	$Header: /7HeartsOnline/LIB_Server/ipLIST.h 1     04-03-25 11:20a Icarus $
*/
#ifndef	__IP_LIST_H
#define	__IP_LIST_H
#include "DLLIST.h"
#include "classSTR.h"
#include "classHASH.h"
//-------------------------------------------------------------------------------------------------

struct tagConnIP {
	DWORD		m_dwConnSECOND;
	DWORD		m_dwConnCOUNT;
	t_HASHKEY	m_HashKEY;
	SOCKET		m_hSocket;
	CStrVAR		m_IP;
} ;

class IP_LIST {
private:
	classDLLIST< tagConnIP >	 m_ListIP;
	classHASH  < classDLLNODE<tagConnIP>* >	*m_pHashTable;

public :
	IP_LIST ();
	~IP_LIST ();

	classDLLNODE<tagConnIP> *Search( t_HASHKEY HashKEY, char *szIP );
	bool Insert( t_HASHKEY HashKEY, SOCKET hSocket, DWORD dwCurTIME, char *szIP );
	bool Delete( t_HASHKEY HashKEY, SOCKET hSocket );
} ;

//-------------------------------------------------------------------------------------------------
#endif