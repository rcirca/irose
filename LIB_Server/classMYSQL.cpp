
#include <tchar.h>
#include <crtdbg.h>
#include <stdio.h>
#include "classMYSQL.h"
#include "classSTR.h"

#ifdef	_DEBUG
    #pragma comment (lib, "d_libmysql.lib")
#else
    #pragma comment (lib, "r_libmysql.lib")
#endif

/*
	서버와의 접속이 끊겼을 경우...
	Query 실행시 항상 재접속 된다. !!..
*/
//-------------------------------------------------------------------------------------------------
classMYSQL::classMYSQL (short nBindParamCNT, WORD wQueryBufferLEN)
{
	m_pMySQL = NULL;
	m_pQueryResultSET = NULL;
	m_iRecordCNT = 0;
	m_iColumnCNT = 0;

	m_nBindParamCNT = nBindParamCNT;
	_ASSERT( m_nBindParamCNT > 0 );

	m_pBindPARAM = new tagBINDPARAM [ nBindParamCNT ];

	m_dwMaxQueryBuffSize = wQueryBufferLEN;
	m_pQueryBuff = new char[ wQueryBufferLEN ];
}
//-------------------------------------------------------------------------------------------------
classMYSQL::~classMYSQL ()
{
	if ( m_pBindPARAM ) {
		delete[] m_pBindPARAM;
		m_pBindPARAM = NULL;
	}
	if ( m_pQueryBuff ) {
		delete[] m_pQueryBuff;
		m_pQueryBuff = NULL;
	}

	this->Disconnect ();
}

//-------------------------------------------------------------------------------------------------
const char *classMYSQL::GetERROR(void)
{
	return ::mysql_error(m_pMySQL);
}

//-------------------------------------------------------------------------------------------------
bool classMYSQL::Connect (char *szServerIP, char *szUser, char *szPassword)//, short nPortNO)
{
	short nPortNO = MYSQL_PORT;

    this->Disconnect ();

	m_pMySQL = ::mysql_init( NULL );
	if ( m_pMySQL ) {
		/*
			host == NULL : localhost
			MYSQL *mysql_real_connect(	MYSQL *mysql, 
										const char *host,
										const char *user, 
										const char *passwd, 
										const char *db,
										unsigned int port, 
										const char *unix_socket, 
										unsigned int client_flag);
		*/
		if ( ::mysql_real_connect( m_pMySQL, szServerIP, szUser, szPassword, NULL, nPortNO, NULL, 0 ) ) 
			return true;
    }

	m_iErrCODE = ::mysql_errno (m_pMySQL);

	sprintf (m_pQueryBuff, "MYSQL:: Connect Error[ ErrCODE: %d, %s ]\n", m_iErrCODE, this->GetERROR() );
	::OutputDebugString( m_pQueryBuff );

	this->Disconnect ();
	return false;
}

//-------------------------------------------------------------------------------------------------
void classMYSQL::Disconnect ()
{
	this->FreeResult ();
    
	if ( m_pMySQL ) {
		::mysql_close( m_pMySQL );
		m_pMySQL = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
bool classMYSQL::SelectDB (char *szDB)
{
	m_iErrCODE = ::mysql_select_db(m_pMySQL, szDB);
	return ( 0 == m_iErrCODE );
}

//-------------------------------------------------------------------------------------------------
void classMYSQL::StoreResult ()
{
	this->FreeResult ();
	m_pQueryResultSET = ::mysql_store_result( m_pMySQL );
}

//-------------------------------------------------------------------------------------------------
void classMYSQL::FreeResult ()
{
	if ( m_pQueryResultSET ) {
		::mysql_free_result( m_pQueryResultSET );
		m_pQueryResultSET = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
int classMYSQL::ExecMySQL (char *szQuery)
{
	// int mysql_query(MYSQL *mysql, const char *query) 
	unsigned long ulLength = strlen( szQuery );
	if ( ulLength > 0 ) {
		return this->ExecMySQL(szQuery, ulLength);
	}
	m_iRecordCNT = 0;
	return 0;
}
int classMYSQL::ExecMySQL (char *szQuery, unsigned long ulLength)
{
	m_iErrCODE = ::mysql_real_query(m_pMySQL, szQuery, ulLength);
	if ( 0 == m_iErrCODE ) {
		// 영향 받은 레크드 수 !!!
		m_iRecordCNT = (int) ::mysql_affected_rows( m_pMySQL );
	} else
		m_iRecordCNT = -1;

	return m_iRecordCNT;
}

//-------------------------------------------------------------------------------------------------
//bool classMYSQL::QuerySP  (char *szFormat, ...)
//{
//    va_list ArgPtr;
//
//    va_start(ArgPtr, szFormat);
//    vsprintf(m_pQueryBuff, szFormat, ArgPtr);
//    va_end(ArgPtr);
//
//	return this->QueryString( m_pQueryBuff, strlen(m_pQueryBuff) );
//}

bool classMYSQL::QuerySQL (char *szFormat, ...)
{
    va_list ArgPtr;

    va_start(ArgPtr, szFormat);
    vsprintf(m_pQueryBuff, szFormat, ArgPtr);
    va_end(ArgPtr);

	return this->QueryString( m_pQueryBuff, strlen(m_pQueryBuff) );
}
int	 classMYSQL::ExecSQL  (char *szFormat, ...)
{
    va_list ArgPtr;

    va_start(ArgPtr, szFormat);
    vsprintf(m_pQueryBuff, szFormat, ArgPtr);
    va_end(ArgPtr);

	return this->ExecMySQL( m_pQueryBuff, strlen(m_pQueryBuff) );
}

//-------------------------------------------------------------------------------------------------
bool classMYSQL::QueryString (char *szQuery)
{
	// int mysql_query(MYSQL *mysql, const char *query) 
	unsigned long ulLength = strlen( szQuery );
	if ( ulLength > 0 ) {
		return this->QueryString(szQuery, ulLength);
	}
	m_iRecordCNT = 0;
	return false;
}
//-------------------------------------------------------------------------------------------------
bool classMYSQL::QueryString (char *szQuery, unsigned long ulLength)
{
	m_iErrCODE = ::mysql_real_query(m_pMySQL, szQuery, ulLength);
	if ( 0 == m_iErrCODE ) {
		this->StoreResult ();
		m_iRecordCNT = (int) ::mysql_num_rows( m_pQueryResultSET );
		m_iColumnCNT = 0;
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
bool classMYSQL::Fetch ()
{
	if ( m_ResultRecord = ::mysql_fetch_row( m_pQueryResultSET ) ) {
		m_iColumnCNT = ::mysql_num_fields( m_pQueryResultSET );
		return true;
	}

	m_iColumnCNT = 0;
	return false;
}

//-------------------------------------------------------------------------------------------------
BYTE *classMYSQL::GetDataPTR (WORD wColumnIDX)
{
	if ( wColumnIDX >= m_iColumnCNT ) {
		_ASSERT( wColumnIDX>=0 && wColumnIDX < m_iColumnCNT );
		return NULL;
	}

	return (BYTE*)m_ResultRecord[ wColumnIDX ];
}
char *classMYSQL::GetStrPTR (WORD wColumnIDX, bool bSetSpaceToNULL)
{
	if ( wColumnIDX >= m_iColumnCNT ) {
		_ASSERT( wColumnIDX>=0 && wColumnIDX < m_iColumnCNT );
		return NULL;
	}

	return m_ResultRecord[ wColumnIDX ];
}
int	classMYSQL::GetStrLen (WORD wColumnIDX)
{
	if ( wColumnIDX >= m_iColumnCNT ) {
		_ASSERT( wColumnIDX>=0 && wColumnIDX < m_iColumnCNT );
		return NULL;
	}

	return ( m_ResultRecord[ wColumnIDX ] ) ? strlen( m_ResultRecord[ wColumnIDX ] ) : 0;
}

int classMYSQL::GetInteger (WORD wColumnIDX)
{
	if ( wColumnIDX >= m_iColumnCNT ) {
		_ASSERT( wColumnIDX>=0 && wColumnIDX < m_iColumnCNT );
		return 0;
	}

	return atoi( m_ResultRecord[ wColumnIDX ] );
}

short classMYSQL::GetInteger16 (WORD wColumnIDX)
{
	if ( wColumnIDX >= m_iColumnCNT ) {
		_ASSERT( wColumnIDX>=0 && wColumnIDX < m_iColumnCNT );
		return 0;
	}

	return (short)atoi( m_ResultRecord[ wColumnIDX ] );
}

__int64 classMYSQL::GetInt64 (WORD wColumnIDX)
{
	if ( wColumnIDX >= m_iColumnCNT ) {
		_ASSERT( wColumnIDX>=0 && wColumnIDX < m_iColumnCNT );
		return 0;
	}

	return _atoi64( m_ResultRecord[ wColumnIDX ] );
}


//-------------------------------------------------------------------------------------------------
bool classMYSQL::BindPARAM (short nParamIDX, BYTE *pData, unsigned int uiDataSize)
{
	_ASSERT( nParamIDX > 0 && nParamIDX < m_nBindParamCNT );
	if ( nParamIDX < 1 || nParamIDX > m_nBindParamCNT )
		return false;

	m_pBindPARAM[ nParamIDX ].m_pDATA = pData;
	m_pBindPARAM[ nParamIDX ].m_uiDataSize = uiDataSize;

	return true;
}

//-------------------------------------------------------------------------------------------------
DWORD classMYSQL::MakeQuery (char *szCommand, ...)
{
	va_list va;
	va_start( va, szCommand );

	int iStrLen;
	iStrLen = strlen( szCommand );

	::CopyMemory( m_pQueryBuff, szCommand, iStrLen );
	char *pBuff = m_pQueryBuff + iStrLen;

	MQ_PARAM_TYPE  type;
	while ((type = va_arg(va, MQ_PARAM_TYPE)) != MQ_PARAM_END) {
		switch( type ) {
			case MQ_PARAM_ADDSTR:
			{
				char *pStr = va_arg(va, char*);
				iStrLen = strlen(pStr);

				*pBuff ++ = ' ';	// 공백을 앞에 넣는다.
				::CopyMemory (pBuff, pStr, iStrLen);
				pBuff += iStrLen;
				*pBuff ++ = ' ';	// 공백을 뒤에 넣는다.
				break;
			}
			case MQ_PARAM_INT	:
			{
				int iValue = va_arg(va, int);
				sprintf (m_szTemp, "%d", iValue );

				iStrLen = strlen(m_szTemp);
				::CopyMemory (pBuff, m_szTemp, iStrLen);
				pBuff += iStrLen;
				break;
			}

			case MQ_PARAM_INT16	:
			{
				short nValue = va_arg(va, short);
				sprintf (m_szTemp, "%d", nValue );

				iStrLen = strlen(m_szTemp);
				::CopyMemory (pBuff, m_szTemp, iStrLen);
				pBuff += iStrLen;
				break;
			}

			case MQ_PARAM_INT64 :
			{
				__int64 i64Value = va_arg(va, __int64);
				// char *_i64toa( __int64 value, char *string, int radix );

				_i64toa( i64Value, m_szTemp, 10 );

				iStrLen = strlen(m_szTemp);
				::CopyMemory (pBuff, m_szTemp, iStrLen);
				pBuff += iStrLen;
				break;
			}
			case MQ_PARAM_FLOAT	:
			{
				double fValue = va_arg(va, double);
				sprintf (m_szTemp, "%f", fValue );

				iStrLen = strlen(m_szTemp);
				::CopyMemory (pBuff, m_szTemp, iStrLen);
				pBuff += iStrLen;
				break;
			}
			case MQ_PARAM_STR	: 
			{
				char *pStr = va_arg(va, char*);

				*pBuff++ = '\'';

				iStrLen = strlen(pStr);
				::CopyMemory (pBuff, pStr, iStrLen);
				pBuff += iStrLen;

				*pBuff++ = '\'';
				break;
			}
			case MQ_PARAM_BINDIDX	:
			{
				int iParamIDX = va_arg(va, int);
				// unsigned long ::mysql_real_escape_string (MYSQL *mysql, char *to, const char *from, unsigned long length) 
				*pBuff++= '\'';
				 pBuff += ::mysql_real_escape_string( m_pMySQL, pBuff, (const char*)m_pBindPARAM[ iParamIDX ].m_pDATA, m_pBindPARAM[ iParamIDX ].m_uiDataSize);
				*pBuff++= '\'';
				break;
			}
		}
	}
	va_end(va);

	*pBuff = 0;
//	OutputDebugString ( m_pQueryBuff );
//	OutputDebugString ( "\n\n");

	m_dwQueryBuffSize = pBuff - m_pQueryBuff;

	_ASSERT( m_dwMaxQueryBuffSize > m_dwQueryBuffSize );

	return m_dwQueryBuffSize;
}

//-------------------------------------------------------------------------------------------------
