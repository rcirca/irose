/*
	$Header: /7HeartsOnline/LIB_Server/ipLIST.cpp 2     04-07-19 8:02p Icarus $
*/

#include <windows.h>
#include "ipLIST.h"


//-------------------------------------------------------------------------------------------------
IP_LIST::IP_LIST ()
{
	m_pHashTable = new classHASH< classDLLNODE<tagConnIP>* >;
}

IP_LIST::~IP_LIST ()
{
}

//-------------------------------------------------------------------------------------------------
classDLLNODE<tagConnIP> * IP_LIST::Search( t_HASHKEY HashKEY, char *szIP )
{
	tagHASH< classDLLNODE<tagConnIP>* > *pHashNode;

	classDLLNODE<tagConnIP> *pConnIP;

	pHashNode = m_pHashTable->Search( HashKEY );
	pConnIP = pHashNode ? pHashNode->m_DATA : NULL;
	while( pConnIP ) {
		if ( 0 == strcmpi( pConnIP->DATA.m_IP.Get(), szIP ) ) {
			pConnIP->DATA.m_dwConnCOUNT ++;
			return pConnIP;
		}

		pHashNode = m_pHashTable->SearchContinue( pHashNode, HashKEY );
		pConnIP = pHashNode ? pHashNode->m_DATA : NULL;
	}

	return NULL;
}

//-------------------------------------------------------------------------------------------------
bool IP_LIST::Insert( t_HASHKEY HashKEY, SOCKET hSocket, DWORD dwCurTIME, char *szIP )
{	
	classDLLNODE<tagConnIP> *pConnIP;
/*
	classDLLNODE<tagConnIP> *pDelIP;

	pConnIP = m_ListIP.GetHeadNode ();
	while( pConnIP ) {
		// 15초 이상 된것들 모두 삭제..
		if ( dwCurTIME - pConnIP->DATA.m_dwConnSECOND >= 15 ) {
			pDelIP  = pConnIP;
			pConnIP = m_ListIP.GetNextNode( pConnIP );

			m_pHashTable->Delete( pConnIP->DATA.m_HashKEY, pConnIP );
			m_ListIP.DeleteNFree( pConnIP );
			continue;
		}

		pConnIP = m_ListIP.GetNextNode( pConnIP );
	}
*/
	pConnIP = new classDLLNODE<tagConnIP>;

	pConnIP->DATA.m_IP.Set( szIP );
	pConnIP->DATA.m_dwConnSECOND = dwCurTIME;
	pConnIP->DATA.m_dwConnCOUNT  = 1;
	pConnIP->DATA.m_HashKEY		 = HashKEY;
	pConnIP->DATA.m_hSocket		 = hSocket;

	m_ListIP.AppendNode( pConnIP );

	m_pHashTable->Insert( HashKEY, pConnIP );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool IP_LIST::Delete( t_HASHKEY HashKEY, SOCKET hSocket )
{
	tagHASH< classDLLNODE<tagConnIP>* > *pHashNode;
	classDLLNODE<tagConnIP> *pConnIP;

	pHashNode = m_pHashTable->Search( HashKEY );
	pConnIP = pHashNode ? pHashNode->m_DATA : NULL;
	while( pConnIP ) {
		if ( hSocket == pConnIP->DATA.m_hSocket ) {
			pConnIP->DATA.m_dwConnCOUNT ++;
			return true;
		}

		pHashNode = m_pHashTable->SearchContinue( pHashNode, HashKEY );
		pConnIP = pHashNode ? pHashNode->m_DATA : NULL;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
