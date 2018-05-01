/*
	$Header: /7HeartsOnline/Server/SHO_GS/Sho_gs_lib/srv_COMMON/CDB_Socket.h 4     04-04-19 5:39p Icarus $
	$History: CDB_Socket.h $
 * 
 * *****************  Version 4  *****************
 * User: Icarus       Date: 04-04-19   Time: 5:39p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib/srv_COMMON
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 04-04-14   Time: 5:57p
 * Updated in $/7HeartsOnline/Server/SHO_GS/Sho_gs_lib/srv_COMMON
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-04-13   Time: 6:36p
 * Updated in $/7HeartsOnline/Server/Common
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:20a
 * Created in $/7HeartsOnline/Server/Common
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:47a
 * Created in $/SevenHearts/Server/Common
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:35a
 * Created in $/SHO/Server/Common
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:26a
 * Created in $/SevenHearts/Server/SHO_GS_LIB/Server/Common
*/
#ifndef	__CDB_SOCKET_H
#define	__CDB_SOCKET_H
#include "LIB_Util.h"

#include <windows.h>
#include <wchar.h>
#include <crtdbg.h>
#include <math.h>
#include <stdio.h>

#include "classSYNCOBJ.h"
#include "classMYSQL.h"
#include "classODBC.h"
#include "CClientSOCKET.h"
#include "classSTR.h"
/*
#include "CVector.h"
#include "DataTYPE.h"
#include "NET_Prototype.h"

#include "classLOG.h"
#include "classTIME.h"
#include "classPACKET.h"
#include "ioDataPOOL.h"
*/

//-------------------------------------------------------------------------------------------------
struct t_PACKET;
class CDB_Socket : public CClientSOCKET {
public :
	bool WndPROC ( WPARAM wParam, LPARAM lParam );

protected :
    CCriticalSection m_csSQL;
	classSQL		*m_pSQL;

	char		*m_pQueryBUFF;
	DWORD		 m_dwBuffLEN;

	SYSTEMTIME	 m_locTime;
	char		 m_szDateTime[ 30 ];
	CTimer		*m_pReconnectTimer;

	t_PACKET	*m_pRecvPket;

	HWND		 m_hMainWND;
	UINT		 m_uiSocketMSG;
	CStrVAR		 m_LogServerIP;
	int			 m_LogServerPORT;

	CStrVAR		 m_LogDBName;
	CStrVAR		 m_LogDBAccount;
	CStrVAR		 m_LogDBPassword;

	bool		 Send_gsv_LOG_DB_INFO ();
	void		 Send_gsv_LOG_SQL();

	virtual char *GetServerNAME ()=0;

public :
	CDB_Socket (bool bUseMySQL);
	virtual ~CDB_Socket ();

	void Init (HWND hWND, char *szLogServerIP, int iLogServerPort, UINT uiSocketMSG, char *szDBName, char *szDBAccount, char *szDBPassword,
				UINT_PTR uiTimerEventID, UINT uiTimerElapse, TIMERPROC lpTimerFunc);

	bool Connect ();
	void Disconnect ();

	char *GetCurDateTimeSTR()
	{
		::GetLocalTime( &m_locTime );

		// date & time format = "0000-00-00 00:00:00"
		sprintf( m_szDateTime, "%04d-%02d-%02d %02d:%02d:%02d", m_locTime.wYear, m_locTime.wMonth, m_locTime.wDay, m_locTime.wHour, m_locTime.wMinute, m_locTime.wSecond);
		return m_szDateTime;
	}
} ;

//-------------------------------------------------------------------------------------------------
#endif