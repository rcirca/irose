/** 
 * @file zz_db.cpp
 * @brief database class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-jan-2004
 *
 * $Header: /engine/src/zz_db.cpp 2     04-02-11 2:05p Zho $
 * $History: zz_db.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-22   Time: 4:39p
 * Created in $/engine/src
 * Created zz_db database class for test.
 */

#define ZZ_IGNORE_DB

#ifndef ZZ_IGNORE_DB

#include "zz_tier0.h"
#include "zz_log.h"
#include "zz_db.h"
#include "zz_profiler.h"

#pragma comment(lib, "sqlite.lib")

zz_db::zz_db (const char * db_path)
{
	path.set(db_path);

	char * errmsg = NULL;
	hdb = sqlite_open(db_path, 0 /* mode: not supported*/, &errmsg);
	if (errmsg) {
		ZZ_LOG("db: open(%s) failed.\n", db_path);
		ZZ_LOG("\t%s\n", errmsg);
		::free(errmsg);
		hdb = NULL;
	}
}

zz_db::~zz_db ()
{
	sqlite_close(hdb);
}

int zz_db::exec (char * sql, zz_db_callback callback, void *user_data)
{
	ZZ_PROFILER_INSTALL(Pdb_exec);

	char * errmsg = NULL;
	int ret = sqlite_exec(hdb, sql, callback, user_data, &errmsg);
	if (errmsg) {
		ZZ_LOG("db: exec(%s) failed.\n", sql);
		ZZ_LOG("\t%s\n", errmsg);
		::free(errmsg);
	}
	return ret;
}

#endif // ZZ_IGNORE_DB