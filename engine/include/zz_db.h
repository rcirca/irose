/** 
 * @file zz_db.h
 * @brief database class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-jan-2004
 *
 * $Header: /engine/include/zz_db.h 1     04-01-22 4:39p Zho $
 * $History: zz_db.h $
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-22   Time: 4:39p
 * Created in $/engine/include
 * Created zz_db database class for test.
 */

#ifndef __ZZ_DB_H__
#define __ZZ_DB_H__

#ifndef __ZZ_STRING_H__
#include "zz_string.h"
#endif

#include "sqlite.h"

typedef sqlite * zz_hdb;

// callback sample:
// int db_callback (void * user_data, int argc, char **argv, char ** column_names) { return 0; }
typedef int (*zz_db_callback)(void*,int,char**,char**);

//--------------------------------------------------------------------------------
class zz_db {
//--------------------------------------------------------------------------------
private:
	zz_string path; // database filename
	sqlite * hdb; // database handle

public:
	zz_db (const char * db_path);
	~zz_db ();
	
	// execute sql query and do user callback routine.
	int exec (char * sql, zz_db_callback callback, void *user_data);
};

/* TESTCODE
#include "zz_db.h"

 int db_callback (void * user_data, int argc, char **argv, char ** column_names)
 {
	 for (int i = 0; i < argc; i++) {
		 std::cout << argv[i] << "|";
	 }
	 std::cout << std::endl;
	 return 0;
 }

void main ()
{
	zz_db mydb("test.db");
	
	mydb.exec("delete from test;", db_callback, 0);

	int i;
	char sql[256];
	for (i = 0; i < 1000; i++) {
		sprintf(sql, "insert into test values (%d, 'name%04d', %f);", i, i, i+.05f);
		mydb.exec(sql, NULL, 0);
	}
	
	mydb.exec("select * from test;", db_callback, 0);
}
*/

#endif // __ZZ_DB_H__