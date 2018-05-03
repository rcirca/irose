#ifndef	__CLASSODBC_H
#define	__CLASSODBC_H
//-------------------------------------------------------------------------------------------------
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <odbcss.h>
#include "classSQL.h"
#include "classSTR.h"
//-------------------------------------------------------------------------------------------------


#define	MAX_ODBC_COLUMN_CNT			64
#define	MAX_ODBC_COLUMN_LEN			4096
#define	MAX_ODBC_COLUMN_NAME		64

#define MAX_ODBC_BIND_PARAMETER		64
#define	MAX_ODBC_QUERY_BUFFER		8192

struct tagODBCCOL {
	SQLCHAR	    m_Name[ MAX_ODBC_COLUMN_NAME ];
	SQLSMALLINT	m_NameLen;
	SQLSMALLINT	m_SqlCType;
	SQLUINTEGER	m_ColumnSize;
	SQLINTEGER	m_StrLen_or_IndPtr;
	union {
		char				m_String[ MAX_ODBC_COLUMN_LEN+1 ];
		BYTE				m_Data  [ MAX_ODBC_COLUMN_LEN+1 ];
		TIMESTAMP_STRUCT	m_SQL_TIMESTAMP;
		SQLINTEGER			m_SQL_CHAR;
		SQLINTEGER			m_SQL_INTEGER;
		SQLINTEGER			m_SQL_SMALLINT;
		SQLINTEGER			m_SQL_REAL;  
		SQLINTEGER			m_SQL_DOUBLE;
		__int64 			m_SQL_BIGINT;
	} ;
} ;

struct tagODBCDATA {
	union {
		BYTE	   *m_pDATA;
		bool		m_bVal;
		char		m_chVal;
		short		m_nVal;
		long		m_lVal;
		float		m_fVal;
		double		m_dblVal;
	} ;
	SQLUINTEGER	m_uiDataSize;
	SQLINTEGER	m_StrLen_or_Ind;
} ;

class classODBC : public classSQL {
// protected :
public :
	CStrVAR		m_ErrMSG;
	short		m_nBindParamCNT;
	void		AllocParam (short nCntToAlloc);
	tagODBCDATA*m_pBindPARAM;

	short		m_nAllocColCnt;
	void		AllocColumn (short nCntToAlloc);

	tagODBCCOL *m_pColumn;

	SQLHENV		m_hENV;
	SQLHDBC		m_hDBC1;
	SQLHSTMT	m_hSTMT1;

	SQLSMALLINT m_nResultColCnt;
	SQLINTEGER  m_iResultRowCnt;		// Update, Delete, Insert명령시 영향받은 레코드 갯수를 구해온다.

public  :
	RETCODE	m_RetCode;

	classODBC (short nBindParamCNT=MAX_ODBC_BIND_PARAMETER, WORD wQueryBufferLEN=MAX_ODBC_QUERY_BUFFER);
	~classODBC ();

//	bool		QuerySP  (char *szFormat, ...);
	bool		QuerySQL (char *szFormat, ...);
	int			ExecSQL  (char *szFormat, ...);

	void		Clear (void);

	SQLSMALLINT	GetColDataType(BYTE btColumn)	{	return m_pColumn[ btColumn ].m_SqlCType;	}

	bool		FirstDSN (char *pszDSN, int lMax);
	bool		NextDSN  (char *pszDSN, int lMax);
	bool		RegisterDSN (char *szDSN, char *szDBName, char *szServerIP, char *szUser);
	bool		ReigsterDsnIfNone (char *szDSN, char *szDBName, char *szServerIP, char *szUser);

	// inherited from classSQL
	bool		Connect (char *szDSN, char *szUserName, char *szPassword);
	void		Disconnect (void);
	bool		SelectDB (char *szDBName)		{	return true;	}

	const char* GetERROR();

	int			GetRecordCNT ()					{	return (int)m_iResultRowCnt;				}
	int			GetColumnCNT ()					{	return m_nResultColCnt;						}

	bool		Fetch ();
	bool		BindPARAM (short nParamIDX, BYTE *pData, unsigned int uiDataSize);
	DWORD		MakeQuery (char *szCommand, ...);
	int			ExecSQLBuffer ();
	int			ExecSQLPacket (char *szQuery, ULONG ulLength)	{	return 0;	}

	int			ExecSQLBuffer (char *szQuery);
	int			ExecSQLBuffer (char *szQuery, unsigned long ulLength);

	bool		QueryString (char *szQuery, unsigned long ulLength);
	bool		QueryString (char *szQuery);

	bool		QuerySQLBuffer ( /* bool bBindResult=true */ );

	BYTE*		GetDataPTR	(WORD wColumn);
	char	   *GetStrPTR	(WORD wColumn, bool bSetSpaceToNULL=true);
	int			GetStrLen	(WORD wColumn);
	int			GetInteger	(WORD wColumn);
	short		GetInteger16 (WORD wColumnIDX);	// __KCHS_BATTLECART__
	__int64		GetInt64	(WORD wColumn);
	bool		GetTimestamp( WORD wColumnIDX, sqlTIMESTAMP *pTimeStamp );

	bool BindRESULT ();
	bool GetMoreRESULT ()						{	return ( ::SQLMoreResults( this->m_hSTMT1) != SQL_NO_DATA );	}

	bool SetParameter		(short nParamIDX, BYTE *pData, UINT uiDataSize, short nIOType, short nValueType, short nParamType );
	bool SetParam_string	( short nParamIDX, char *szStr );
	bool SetParam_wstring	( short nParamIDX, char *szStr );
	bool SetParam_long		( short nParamIDX, long &lOutResult, long &cbLen );
} ;


/*
	// Example ...
	classODBC COdbc;

	if ( COdbc.Connect ("TriggerNET", "icarus", "xxxx") )
		Log_String ("ODBC Connected ...");
	else
		Log_String ("ODBC Connect failed");

	if ( COdbc.Query ("select * from tblUSER") ) {
		while ( COdbc.Fetch () != SQL_NO_DATA ) {
			short nI, nLen;
			char  aa[ 10 ];

			for (nI=0; nI<COdbc.GetColCnt(); nI++) {
				nLen = COdbc.GetStrLen (nI);
				
				OutputDebugString ( itoa(nLen, aa, 10) );
				OutputDebugString ( ":");

				if ( COdbc.GetStrPtr(nI) != NULL ) 
					OutputDebugString ( COdbc.GetStrPtr(nI) );
				else 
					OutputDebugString ( "NULL" );

				OutputDebugString ("   ");
			}
			OutputDebugString("\n");
		}
	}
*/

//-------------------------------------------------------------------------------------------------
#endif