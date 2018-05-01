#ifndef _CSqliteDB_
#define _CSqliteDB_
#include "sqlite.h"

class CSqliteDB
{
	CSqliteDB(void);
	~CSqliteDB(void);
public:
	static CSqliteDB& GetInstance();
	int		Open( const char* pszName );
	int		Close();
	int		Exec( const char* sql ,sqlite_callback callback);
//	int		ExecPrintf( const char* sqlFormat, sqlite_callback callback, ... );
	int		AddMemo( const char* pszName, const char* pszSender, const char* pszContent, DWORD time );
	int		LoadMemo( const char* pszName, sqlite_callback callback );
	int		DeleteMemo( int iRowid );

private:
	sqlite* m_db;
};
#endif