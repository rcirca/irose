/*
	$Header: /7HeartsOnline/LIB_Server/ioDataPOOL.cpp 1     04-03-25 11:20a Icarus $
*/
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "ioDataPOOL.h"


//-------------------------------------------------------------------------------------------------
IMPLEMENT_INSTANCE2( CPoolPACKET )

CPoolPACKET::CPoolPACKET( UINT uiInitDataCNT, UINT uiIncDataCNT ) 
#ifdef	__USE_PACKET_POOL
							: CDataPOOL< classPACKET > ( "Packet", uiInitDataCNT, uiIncDataCNT )
#endif
{
}


//-------------------------------------------------------------------------------------------------
IMPLEMENT_INSTANCE2( CPoolRECVIO )

CPoolRECVIO::CPoolRECVIO( UINT uiInitDataCNT, UINT uiIncDataCNT ) 
#ifdef	__USE_RECV_IODATA_POOL
							: CDataPOOL< IODATANODE > ( "RecvIO", uiInitDataCNT, uiIncDataCNT )
#endif
{
}


//-------------------------------------------------------------------------------------------------
IMPLEMENT_INSTANCE2( CPoolSENDIO )

CPoolSENDIO::CPoolSENDIO( UINT uiInitDataCNT, UINT uiIncDataCNT ) 
#ifdef	__USE_SEND_IODATA_POOL
							: CDataPOOL< IODATANODE > ( "SendIO", uiInitDataCNT, uiIncDataCNT )
#endif
{
}

//-------------------------------------------------------------------------------------------------
