/*
	$Header: /7HeartsOnline/Server/SHO_WS/Sho_ws_lib/WS_SocketLOG.h 4     04-05-27 9:45p Icarus $
	$History: WS_SocketLOG.h $
 * 
 * *****************  Version 4  *****************
 * User: Icarus       Date: 04-05-27   Time: 9:45p
 * Updated in $/7HeartsOnline/Server/SHO_WS/Sho_ws_lib
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 04-05-19   Time: 11:11a
 * Updated in $/7HeartsOnline/Server/SHO_WS/Sho_ws_lib
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-04-17   Time: 11:46a
 * Updated in $/7HeartsOnline/Server/SHO_WS/Sho_ws_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:20a
 * Created in $/7HeartsOnline/Server/Sho_ws/Sho_ws_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:47a
 * Created in $/SevenHearts/Server/Sho_ws/Sho_ws_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:35a
 * Created in $/SHO/Server/Sho_ws/Sho_ws_lib
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:26a
 * Created in $/SevenHearts/Server/SHO_WS_LIB/Server/Sho_ws/Sho_ws_lib
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-03-23   Time: 6:50p
 * Updated in $/SevenHearts/Server/SHO_WS_LIB
*/
#ifndef	__WS_SOCKETLOG_H
#define	__WS_SOCKETLOG_H
#include "SHO_WS_LIB.h"
//-------------------------------------------------------------------------------------------------

// #define	__CDB_SOCKET

#ifdef	__CDB_SOCKET
	#include "CDB_Socket.h"
#endif


class CWS_Client;

class WS_logSOCKET 
#ifdef	__CDB_SOCKET
	: public CDB_Socket
#endif
{
private:
//	CStrVAR	m_TmpSTR;

	char *GetServerNAME ()
	{
		return SHO_WS::GetInstance()->GetServerName();
	}

public :
	CStrVAR		m_QuerySTR;

#ifndef	__CDB_SOCKET
	CCriticalSection	m_csSQL;
	SYSTEMTIME			m_locTime;
	char				m_szDateTime[ 30 ];

	char	   *GetCurDateTimeSTR()
	{
		::GetLocalTime( &m_locTime );

		// date & time format = "0000-00-00 00:00:00"
		sprintf( m_szDateTime, "%04d-%02d-%02d %02d:%02d:%02d", m_locTime.wYear, m_locTime.wMonth, m_locTime.wDay, m_locTime.wHour, m_locTime.wMinute, m_locTime.wSecond);
		return m_szDateTime;
	}
	bool Connect ()		{	return true;	}
	void Disconnect ()	{	;				}

	void Send_wsv_LOG_SQL();
#endif

	WS_logSOCKET (bool bUseMySQL);
	~WS_logSOCKET ();

	bool Insert_WorldVAR ();
	bool Update_WorldVAR ();

	bool Insert_NpcVAR ();
	bool Update_NpcVAR ();

	bool When_CreateCHAR ( CWS_Client *pSourAVT, char *szCharNAME );
	bool When_DeleteCHAR ( CWS_Client *pSourAVT, char *szCharNAME );
} ;
extern WS_logSOCKET	*g_pSockLOG;

//-------------------------------------------------------------------------------------------------
#endif
