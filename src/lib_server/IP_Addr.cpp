
#include <windows.h>
#include <winsock.h>
#include <tchar.h>
#include <crtdbg.h>
#include "classUTIL.h"
#include "classLOG.h"
#include "classSTR.h"
#include "IP_Addr.h"

//-------------------------------------------------------------------------------------------------
CIPv4Addr::~CIPv4Addr ()
{
	classDLLNODE< tagIP4ADDR >	*pNODE;
	for (int iC=0; iC<IPv4_LIST_SIZE; iC++) {
		while( pNODE = m_LIST[ iC ].GetHeadNode () )
			m_LIST[ iC ].DeleteNFree( pNODE );
	}
}

bool CIPv4Addr::Add (char *szIPFrom, char *szIPTo)
{
	in_addr iaFrom, iaTo;
	in_addr iaFromORI, iaToORI;

	iaFromORI.S_un.S_addr = ::inet_addr( szIPFrom );
	iaToORI.S_un.S_addr   = ::inet_addr( szIPTo	);

	iaFrom.S_un.S_un_b.s_b1 = iaFromORI.S_un.S_un_b.s_b4;
	iaFrom.S_un.S_un_b.s_b2 = iaFromORI.S_un.S_un_b.s_b3;
	iaFrom.S_un.S_un_b.s_b3 = iaFromORI.S_un.S_un_b.s_b2;
	iaFrom.S_un.S_un_b.s_b4 = iaFromORI.S_un.S_un_b.s_b1;
							  
	iaTo.S_un.S_un_b.s_b1	= iaToORI.S_un.S_un_b.s_b4;
	iaTo.S_un.S_un_b.s_b2	= iaToORI.S_un.S_un_b.s_b3;
	iaTo.S_un.S_un_b.s_b3	= iaToORI.S_un.S_un_b.s_b2;
	iaTo.S_un.S_un_b.s_b4	= iaToORI.S_un.S_un_b.s_b1;

	classDLLNODE< tagIP4ADDR >	*pNODE;

	pNODE = m_LIST[ iaFromORI.S_un.S_un_b.s_b1 ].AllocNAppend ();
	pNODE->DATA.m_ulMIN = iaFrom.S_un.S_addr;
	pNODE->DATA.m_ulMAX = iaTo.S_un.S_addr;

	//g_LOG.CS_ODS( 0xfff, "ADD IP( %s ~ %s ) => ( %lu ~ %lu : %lu ), ( %d.%d.%d.%d ~ %d.%d.%d.%d ) \n",
	//	szIPFrom, szIPTo,

	//	iaFrom.S_un.S_addr, iaTo.S_un.S_addr, iaTo.S_un.S_addr - iaFrom.S_un.S_addr, 

	//	iaFrom.S_un.S_un_b.s_b1, 
	//	iaFrom.S_un.S_un_b.s_b2, 
	//	iaFrom.S_un.S_un_b.s_b3, 
	//	iaFrom.S_un.S_un_b.s_b4, 

	//	iaTo.S_un.S_un_b.s_b1, 
	//	iaTo.S_un.S_un_b.s_b2, 
	//	iaTo.S_un.S_un_b.s_b3, 
	//	iaTo.S_un.S_un_b.s_b4 );

	g_LOG.CS_ODS( 0xfff, "ADD IP( %d.%d.%d.%d ~ %d.%d.%d.%d ) => ( %lu ) \n",
		iaFrom.S_un.S_un_b.s_b4, 
		iaFrom.S_un.S_un_b.s_b3, 
		iaFrom.S_un.S_un_b.s_b2, 
		iaFrom.S_un.S_un_b.s_b1, 

		iaTo.S_un.S_un_b.s_b4, 
		iaTo.S_un.S_un_b.s_b3, 
		iaTo.S_un.S_un_b.s_b2, 
		iaTo.S_un.S_un_b.s_b1,

		iaTo.S_un.S_addr - iaFrom.S_un.S_addr );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool CIPv4Addr::Find (in_addr &inADDR)
{
	in_addr inREVS;

	inREVS.S_un.S_un_b.s_b1 = inADDR.S_un.S_un_b.s_b4;
	inREVS.S_un.S_un_b.s_b2 = inADDR.S_un.S_un_b.s_b3;
	inREVS.S_un.S_un_b.s_b3 = inADDR.S_un.S_un_b.s_b2;
	inREVS.S_un.S_un_b.s_b4 = inADDR.S_un.S_un_b.s_b1;

	classDLLNODE< tagIP4ADDR >	*pNODE;
	pNODE = m_LIST[ inADDR.S_un.S_un_b.s_b1 ].GetHeadNode ();
	while( pNODE ) {
		if ( inREVS.S_un.S_addr >= pNODE->DATA.m_ulMIN &&
			 inREVS.S_un.S_addr <= pNODE->DATA.m_ulMAX )
			return true;

		pNODE = pNODE->m_pNEXT;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool CIPv4Addr::Find (char *szIPAddr)
{
	in_addr inADDR;

	inADDR.S_un.S_addr = ::inet_addr( szIPAddr );
	
	return this->Find( inADDR );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
