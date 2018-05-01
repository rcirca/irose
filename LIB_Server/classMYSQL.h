/*
	$History: classMYSQL.h $
 * 
 * *****************  Version 9  *****************
 * User: Netggun      Date: 05-07-25   Time: 1:08a
 * Updated in $/7HeartsOnline/LIB_Server
 * 
 * *****************  Version 8  *****************
 * User: Icarus       Date: 05-04-04   Time: 2:31p
 * Updated in $/7HeartsOnline/LIB_Server
 * 
 * *****************  Version 7  *****************
 * User: Icarus       Date: 04-11-04   Time: 10:48p
 * Updated in $/7HeartsOnline/LIB_Server
 * 
 * *****************  Version 6  *****************
 * User: Icarus       Date: 04-10-26   Time: 5:48p
 * Updated in $/7HeartsOnline/LIB_Server
 * 
 * *****************  Version 5  *****************
 * User: Icarus       Date: 04-10-26   Time: 1:57p
 * Updated in $/7HeartsOnline/LIB_Server
 * 
 * *****************  Version 4  *****************
 * User: Icarus       Date: 04-07-27   Time: 7:25p
 * Updated in $/7HeartsOnline/LIB_Server
 * 
 * *****************  Version 3  *****************
 * User: Icarus       Date: 04-07-06   Time: 7:38p
 * Updated in $/7HeartsOnline/LIB_Server
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-05-27   Time: 8:32p
 * Updated in $/7HeartsOnline/LIB_Server
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:20a
 * Created in $/7HeartsOnline/LIB_Server
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:09a
 * Created in $/SevenHearts/LIB_Server
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:47a
 * Created in $/SevenHearts/LIB_Server
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:35a
 * Created in $/SHO/LIB_Server
*/
#ifndef	__CLASS_MYSQL_H
#define	__CLASS_MYSQL_H
#include <windows.h>
#include <winsock.h>
#include <mysql.h>
#include "classSQL.h"
//-------------------------------------------------------------------------------------------------

#define	MAX_MYSQL_BIND_PARAMETER	64
#define	MAX_MYSQL_QUERY_BUFFER		8192

class classMYSQL : public classSQL {
private:
	bool		 m_bIsConnected;
	int			 m_iErrCODE;
	MYSQL		*m_pMySQL;
	MYSQL_RES	*m_pQueryResultSET;

	int			 m_iColumnCNT;
	int			 m_iRecordCNT;
	MYSQL_ROW	 m_ResultRecord;	// MYSQL_ROW = char **

/*
	char		 m_szTemp[ 256 ];
	char	    *m_pQueryBuff;
	DWORD		 m_dwMaxQueryBuffSize;
	DWORD		 m_dwQueryBuffSize;
*/

	short		 m_nBindParamCNT;
	struct tagBINDPARAM {
		void		*m_pDATA;
		unsigned int m_uiDataSize;
	} *m_pBindPARAM;

	void StoreResult ();
	void FreeResult ();

public :
	classMYSQL (short nBindParamCNT=MAX_MYSQL_BIND_PARAMETER, WORD wQueryBufferLEN=MAX_MYSQL_QUERY_BUFFER);
	~classMYSQL ();

	void InitOnly ()			{	m_pMySQL = ::mysql_init( NULL );	}

	int		ExecMySQL (char *szQuery);
	int		ExecMySQL (char *szQuery, unsigned long ulLength);

	// inherited from classSQL
	bool	Connect (char *szServerIP, char *szUser, char *szPassword);//, short nPortNO=MYSQL_PORT);
	void	Disconnect ();
	bool	SelectDB (char *szDBName);

	const char*GetERROR();

	int		GetRecordCNT ()		{	return m_iRecordCNT;	}
	int		GetColumnCNT ()		{	return m_iColumnCNT;	}

	bool	Fetch ();
	bool	BindPARAM (short nParamIDX, BYTE *pData, unsigned int uiDataSize);
	DWORD	MakeQuery (char *szCommand, ...);
	int		ExecSQLBuffer ()	{	return ExecMySQL (m_pQueryBuff, m_dwQueryBuffSize);	}
	int		ExecSQLPacket (char *szQuery, ULONG ulLength)	{	return ExecMySQL(szQuery,ulLength);	}

	int		ExecSQLBuffer (char *szQuery)							{	return this->ExecMySQL(szQuery);			}
	int		ExecSQLBuffer (char *szQuery, unsigned long ulLength)	{	return this->ExecMySQL(szQuery,ulLength);	}

//	bool	QuerySP	 (char *szFormat, ...);
	bool	QuerySQL (char *szFormat, ...);
	int		ExecSQL  (char *szFormat, ...);

	bool	QueryString (char *szQuery, unsigned long ulLength);
	bool	QueryString (char *szQuery);

	bool	QuerySQLBuffer ( /* bool bBindResult */ )				
	{	
		return QueryString(m_pQueryBuff, m_dwQueryBuffSize);
	}

	BYTE*	GetDataPTR	(WORD wColumn);
	char*	GetStrPTR	(WORD wColumn, bool bSetSpaceToNULL=true);
	int		GetStrLen   (WORD wColumn);
	int		GetInteger	(WORD wColumn);
	short	GetInteger16 (WORD wColumnIDX);	// __KCHS_BATTLECART__
	__int64 GetInt64	(WORD wColumn);
	bool	GetTimestamp( WORD wColumnIDX, sqlTIMESTAMP *pTimeStamp ) 
	{
		return false;
	}
} ;


//-------------------------------------------------------------------------------------------------
#endif
