#include "stdafx.h"
#include ".\sqlitedb.h"


#pragma comment(lib, "sqlite.lib")

CSqliteDB::CSqliteDB(void)
{
	m_db = NULL;
}

CSqliteDB::~CSqliteDB(void)
{

}

CSqliteDB& CSqliteDB::GetInstance()
{
	static CSqliteDB s_Instance;
	return s_Instance;
}

int	CSqliteDB::Open( const char* pszName )
{
	assert( pszName );
	assert( m_db == NULL );
	if( pszName == NULL || m_db != NULL )
		return 1;

	char* zErrMsg;
	m_db = sqlite_open( pszName, 0, &zErrMsg);
	if( m_db == 0 )
	{
		assert( m_db );
		sqlite_freemem( zErrMsg );
 		return 1;
	}
	return 0;
}

int	CSqliteDB::Close()
{
	assert( m_db );
	if( m_db == NULL )
		return 1;

	sqlite_close( m_db);
	return 0;
}

int CSqliteDB::Exec( const char* sql ,sqlite_callback callback )
{
	assert( m_db );
	assert( sql );
	if( m_db == NULL )
		return 1;
	if( sql == NULL )
		return 1;
	
	char* zErrMsg;
	int rc = sqlite_exec( m_db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
//		g_itMGR.OpenMsgBox( zErrMsg );
		sqlite_freemem( zErrMsg );
		return 1;
	}
	return 0;
}
//
//int CSqliteDB::ExecPrintf( const char* sqlFormat, sqlite_callback callback, ... )
//{
//	assert( m_db );
//	assert( sqlFormat );
//	if( m_db == NULL )
//		return 1;
//	if( sqlFormat == NULL )
//		return 1;
//
//	char* zErrMsg;
//	va_list ap;
//	va_start(ap, zErrMsg);
//	int rc = sqlite_exec_printf( m_db, sqlFormat, callback, 0, &zErrMsg, ap );
//	va_end(ap);
//	if( rc != SQLITE_OK )
//	{
////		g_itMGR.OpenMsgBox( zErrMsg );
//		sqlite_freemem( zErrMsg );
//		return 1;
//	}
//	return 0;
//}

int CSqliteDB::AddMemo( const char* pszName, const char* pszSender, const char* pszContent, DWORD time )
{
	assert( m_db );
	if( m_db == NULL )
		return 1;

	const char* sqlFormat = "insert into memo values ( '%q', '%q' ,'%q', %d )";
	char* zErrMsg;
	int rc = sqlite_exec_printf( m_db, sqlFormat, NULL, 0, &zErrMsg, pszName, pszSender, pszContent, time );
	if( rc != SQLITE_OK )
	{
		sqlite_freemem( zErrMsg );
		return 1;
	}
	return 0;
}

int CSqliteDB::LoadMemo( const char* pszName, sqlite_callback callback )
{
	assert( m_db );
	if( m_db == NULL )
		return 1;

	const char* sqlFormat = "Select ROWID, sender, content, time from memo where name = '%q'";
	char* zErrMsg;
	int rc = sqlite_exec_printf( m_db, sqlFormat, callback, 0, &zErrMsg, pszName );
	if( rc != SQLITE_OK )
	{
		sqlite_freemem( zErrMsg );
		return 1;
	}
	return 0;
}

int CSqliteDB::DeleteMemo( int iRowid )
{
	assert( m_db );
	if( m_db == NULL )
		return 1;

	const char* sqlFormat = "delete from memo where ROWID = '%d'";
	char* zErrMsg;
	int rc = sqlite_exec_printf( m_db, sqlFormat, NULL, 0, &zErrMsg, iRowid );
	if( rc != SQLITE_OK )
	{
		sqlite_freemem( zErrMsg );
		return 1;
	}
	return 0;
}