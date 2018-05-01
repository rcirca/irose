#ifndef	__CLASSSQL_H
#define	__CLASSSQL_H
#include <windows.h>
//-------------------------------------------------------------------------------------------------

enum MQ_PARAM_TYPE {
	MQ_PARAM_NONE = 0,
	MQ_PARAM_INT,
	MQ_PARAM_INT64,
	MQ_PARAM_FLOAT,
	MQ_PARAM_STR,
	MQ_PARAM_BINDIDX,
	MQ_PARAM_ADDSTR,
	MQ_PARAM_INT16,
	MQ_PARAM_END = 0xffffffff
};


#define MAX_SQL_BIND_PARAMETER		48
#define	MAX_SQL_QUERY_BUFFER		8192

struct sqlTIMESTAMP {
	WORD	m_wYear;
	BYTE	m_btMon;
	BYTE	m_btDay;
	BYTE	m_btHour;
	BYTE	m_btMin;
	BYTE	m_btSec;
} ;

// sql interface class 
class classSQL 
{
protected:
	char		m_szTemp[ 256 ];
	char	   *m_pQueryBuff;
	DWORD		m_dwMaxQueryBuffSize;
	DWORD		m_dwQueryBuffSize;


public   :
	classSQL()					{	m_pQueryBuff=NULL, m_dwMaxQueryBuffSize=0;	}
	virtual ~classSQL()			{	if( m_pQueryBuff ) delete[] m_pQueryBuff;	}
	bool	GetNextRECORD ()	{	return this->Fetch();						}
	char*	GetQueryBuffPTR ()	{	return m_pQueryBuff;						}

	virtual bool	Connect (char *szDSNorIP, char *szUserName, char *szPassword)=0;
	virtual void	Disconnect ()=0;
	virtual bool	SelectDB (char *szDBName)=0;

	virtual const char*GetERROR()=0;

	virtual int		GetRecordCNT()=0;
	virtual int		GetColumnCNT()=0;

	virtual bool	Fetch ()=0;
	virtual bool	BindPARAM (short nParamIDX, BYTE *pData, unsigned int uiDataSize)=0;
	virtual DWORD	MakeQuery (char *szCommand, ...)=0;
	virtual int		ExecSQLBuffer ()=0;		// 영향 받은 레코드 카운트 리턴... :: 오류시 -1리턴.
	virtual int		ExecSQLPacket (char *szQuery, ULONG ulLength)=0;

	virtual int		ExecSQLBuffer (char *szQuery)=0;
	virtual int		ExecSQLBuffer (char *szQuery, unsigned long ulLength)=0;

//	virtual bool	QuerySP  (char *szFormat, ...)=0;
	virtual	bool	QuerySQL (char *szFormat, ...)=0;
	virtual int		ExecSQL  (char *szFormat, ...)=0;

	virtual bool	QueryString (char *szQuery, unsigned long ulLength)=0;
	virtual bool	QueryString (char *szQuery)=0;

	virtual bool	QuerySQLBuffer( /* bool bBindResult */ )=0;

	virtual BYTE*	GetDataPTR (WORD wColumn)=0;
	virtual char*	GetStrPTR  (WORD wColumn, bool bSetSpaceToNULL=true)=0;
	virtual int		GetStrLen  (WORD wColumn)=0;
	virtual int		GetInteger (WORD wColumn)=0;
	virtual short	GetInteger16 (WORD wColumnIDX)=0;	// __KCHS_BATTLECART__
	virtual __int64 GetInt64   (WORD wColumn)=0;
	virtual bool	GetTimestamp( WORD wColumnIDX, sqlTIMESTAMP *pTimeStamp ) = 0;

	virtual bool	BindRESULT ()			{	return false;	}
	virtual	bool	GetMoreRESULT ()		{	return false;	}

	virtual bool	SetParameter		( short nParamIDX, BYTE *pData, UINT uiDataSize, short nIOType, short nValueType, short nParamType )	{	return false;	}
	virtual bool	SetParam_string		( short nParamIDX, char *szStr )							{	return false;	};
	virtual bool	SetParam_wstring	( short nParamIDX, char *szStr )							{	return false;	};
	virtual bool	SetParam_long		( short nParamIDX, long &lOutResult, long &cbLen )			{	return false;	}
} ;

//-------------------------------------------------------------------------------------------------
#endif
