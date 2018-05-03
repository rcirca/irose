/** 
 * @file zz_profiler.h
 * @brief profiler class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-feb-2002
 * 
 * $Header: /engine/include/zz_profiler.h 6     04-07-07 3:29a Zho $
 * $History: zz_profiler.h $
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-07-07   Time: 3:29a
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-01-03   Time: 8:50p
 * Updated in $/engine/include
 * check_limit_msec in .cpp
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-03   Time: 8:11p
 * Updated in $/engine/include
 * implementation codes in .cpp
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-29   Time: 2:38p
 * Updated in $/engine/include
 * too deep bug fixed
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/include
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef	__ZZ_PROFILER_H__
#define __ZZ_PROFILER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __ZZ_MEM_H__
#include "zz_mem.h"
#endif

#ifndef __ZZ_OS_H__
#include "zz_os.h"
#endif

#ifndef __ZZ_LOG_H__
#include "zz_log.h"
#endif

#ifndef __ZZ_HASH_TABLE_H__
#include "zz_hash_table.h"
#endif

struct zz_profiler_item {
	int count; // number of occurence
	char name[ZZ_MAX_STRING]; // function name
	double total; // total timing in [ms]

	zz_profiler_item(const char * name_in = NULL) : count(0), total(0) {
		if (name_in) {
			strcpy(name, name_in);
		}
		else {
			name[0] = '\0';
		}
	}
	~zz_profiler_item() {}
};

// for profiling
class zz_profiler {
	int _count;
	int _cycle;
	const char * _msg;
	uint64 _start;
	uint64 _end;
	uint64 _sum_of_intervals;
	uint64 _ticks_per_second;
	static uint64 _total_profiled; // totally profiled sum, starting from reset()
	static int _depth; // starting from 0. represent overlapping depth

public:
	typedef zz_hash_table<zz_profiler_item> zz_items;

private:
	zz_items::iterator _item_it;
	static zz_items _items;
	
public:
	zz_profiler(const char * msg, int cycle = 1);
	~zz_profiler()
	{
		end();
	}
	
	void begin();
	void end();

	void reset ();

	// in milliseconds
	double get_total_profiled ();

	static void dump ();
	
	static void statistics (bool by_time);

	//long interval () { return long(_sum_of_intervals/_count); }
	//long interval () { return long(_end - _start); }
};

// macro design
#ifndef __ZZ_PROFILER_DISABLE__
#define ZZ_PROFILER_BEGIN(pname) static zz_profiler pname(#pname); pname.begin()
#define ZZ_PROFILER_BEGIN_CYCLE(pname, cycle) static zz_profiler pname(#pname, cycle); pname.begin()
#define ZZ_PROFILER_END(pname) pname.end()
#define ZZ_PROFILER_INSTALL(pname) zz_profiler pname(#pname); pname.begin()
#else // -----------------------
#define ZZ_PROFILER_BEGIN(pname)
#define ZZ_PROFILER_BEGIN_CYCLE(pname, cycle)
#define ZZ_PROFILER_END(pname)
#define ZZ_PROFILER_INSTALL(pname)
#endif // __ZZ_PROFILER_DISABLE__

void zz_profiler_begin ();
void zz_profiler_end (const char * profile_name);

// usage :
/*
#include "zz_profiler.h"

void foo (void)
{
	int a(10);
	int i;

	for (i = 0; i < 100; i++) {
		a = a+i;
	}
}

void main ()
{
	ZZ_PROFILER_BEGIN( mypro );
	
	foo();
	
	ZZ_PROFILER_END( mypro );
}
*/

#endif // __ZZ_PROFILER_H__