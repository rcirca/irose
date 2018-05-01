/** 
 * @file zz_profiler.cpp
 * @brief profiler functions. stack version.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    10-oct-2003
 *
 * $Header: /engine/src/zz_profiler.cpp 7     04-07-07 3:30a Zho $
 * $History: zz_profiler.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-07-07   Time: 3:30a
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-02-28   Time: 5:44p
 * Updated in $/engine/src
 * riva tnt2 enabled.
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-03   Time: 8:50p
 * Updated in $/engine/src
 * check_limit_msec in .cpp
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-03   Time: 8:11p
 * Updated in $/engine/src
 * implementation codes in .cpp
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/src
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-11-07   Time: 6:43p
 * Updated in $/znzin11/engine/source
 * - 노드간 충돌체크 수정(레벨별로 나뉘도록)
 * - 파일시스템 파일존재체크 약간 빠르게
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-11-06   Time: 6:04p
 * Updated in $/znzin11/engine/source
 * - 프로파일러에 통계내는 기능 추가. destZnzin() 에 사용예.
 * - worldToScreen() 에 z 축도 포함시킴.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-10-10   Time: 2:38a
 * Created in $/znzin11/engine/source
 */

#include "zz_tier0.h"
#include <assert.h>
#include <stack>
#include <map>
#include <algorithm>

#include "zz_algebra.h"
#include "zz_log.h"
#include "zz_os.h"
#include "zz_type.h"
#include "zz_profiler.h"

//#define LOG_ONLY_ONE_LEVEL
#define PROFILER_CHECK_LIMIT_MSEC (0.01)

using namespace std;

static stack<uint64> time_stack;
static int stack_depth = 0;

uint64 zz_profiler::_total_profiled = 0;
int zz_profiler::_depth = 0;
zz_hash_table<zz_profiler_item> zz_profiler::_items;

void zz_profiler_begin ()
{
	uint64 start_time;
	zz_os::get_ticks(start_time); 
	
	time_stack.push(start_time);
	++stack_depth;
}

void zz_profiler_end (const char * profile_name)
{
	uint64 ticks_per_second;
	zz_os::get_ticks_per_second(ticks_per_second);
	uint64 start_time, end_time;
	zz_os::get_ticks(end_time); 
	
	start_time = time_stack.top();
	time_stack.pop();
	--stack_depth;
	uint64 sum_of_intervals = end_time - start_time;

	// make depth spaces
	char spaces[256];
	int i;
	for (i = 0; i < stack_depth; i++) {
		spaces[i] = '\t';
	}
	spaces[i] = '\0';

	double elapsed = double(sum_of_intervals)/ticks_per_second*1000.0;
	if (elapsed > PROFILER_CHECK_LIMIT_MSEC) {
		// print out profiler message
		ZZ_LOG("%sprofiler: % 15.6lf msec. in [%s]\n", spaces, elapsed, profile_name);
	}
}

template<class Type>
struct profiler_greater_time : public binary_function <Type, Type, bool> 
{
	bool operator() (const Type &a, const Type &b) const
	{
		return a->total > b->total;
	}
};


template<class Type>
struct profiler_greater_count : public binary_function <Type, Type, bool> 
{
	bool operator() (const Type &a, const Type &b) const
	{
		return a->count > b->count;
	}
};

//=================================
void zz_profiler::statistics (bool by_time)
{
	std::map<zz_profiler_item * const, double, profiler_greater_time<zz_profiler_item *> > map_item_time;
	std::map<zz_profiler_item * const, double, profiler_greater_count<zz_profiler_item *> > map_item_count;
	std::map<zz_profiler_item * const, double, profiler_greater_time<zz_profiler_item *> >::iterator it_map_time;
	std::map<zz_profiler_item * const, double, profiler_greater_count<zz_profiler_item *> >::iterator it_map_count;			

	if (_items.size() <= 0) return;

	// insert to map
	zz_profiler::zz_items::iterator it;
	for (it = zz_profiler::_items.begin(); it != zz_profiler::_items.end(); it++) {
		if (by_time) {
			map_item_time[&(*it)] = (*it).total;
		}
		else {
			map_item_count[&(*it)] = (*it).total;
		}
	}

	if (by_time) {
		ZZ_LOG("profiler: statistics(by_time) started.\n");
		for (it_map_time = map_item_time.begin(); it_map_time != map_item_time.end(); it_map_time++) {
			zz_profiler_item * item = (*it_map_time).first;
			ZZ_LOG("total(% 15.6lf), count(%10d), average(% 15.6lf), [%s]\n",
				item->total, item->count, (item->total / item->count), item->name);
		}
		ZZ_LOG("profiler: statistics(by_time) ended.\n");
	}
	else {
		ZZ_LOG("profiler: statistics(by_count) started.\n");
		for (it_map_count = map_item_count.begin(); it_map_count != map_item_count.end(); it_map_count++) {
			zz_profiler_item * item = (*it_map_count).first;
			ZZ_LOG("count(%10d), total(% 15.6lf), average(% 15.6lf), [%s]\n",
				item->count, item->total, (item->total / item->count), item->name);
		}
		ZZ_LOG("profiler: statistics(by_count) ended.\n");
	}
}


zz_profiler::zz_profiler(const char * msg, int cycle)
{ 
	_msg = msg;
	_count = 0;
	_sum_of_intervals = 0;
	_start = _end = 0;
	_cycle = cycle;
	_ticks_per_second = 1000;
	_item_it = _items.find(msg);
	if (_item_it == _items.end()) {
		zz_profiler_item item(msg);
		_items.insert(msg, item);
		_item_it = _items.find(msg);
	}
}
	
void zz_profiler::begin() 
{ 
	zz_os::get_ticks(_start); 
	zz_os::get_ticks_per_second(_ticks_per_second);
	_depth++;
}

void zz_profiler::end() 
{
	if (_start == 0) return; // not started yet

	zz_os::get_ticks(_end);
	_count++;
	_sum_of_intervals += (_end - _start);
	_end = 0;
	_depth--;
	assert(_depth >= 0);

	if (_count == _cycle) {
		char spaces[256];
		int i;
		if (_depth > 255) { // not to overspace 256
			_depth = 255;
		}
		for (i = 0; i < _depth; i++) {
			spaces[i] = '\t';
		}
		spaces[i] = '\0';
		double timing = double(_sum_of_intervals/_count)/_ticks_per_second*1000.0;

		if (timing > PROFILER_CHECK_LIMIT_MSEC)
#ifdef LOG_ONLY_ONE_LEVEL
			if (_depth == 0)
#endif
				ZZ_LOG("%sprofiler: % 15.6lf msec. in [%s]\n", spaces, timing, _msg);

		// item related
		(*_item_it).count += _count;
		(*_item_it).total += timing;

		_total_profiled += _sum_of_intervals;
		_sum_of_intervals = 0;
		_count = 0;
		_start = 0;
	}
}

void zz_profiler::dump ()
{
	zz_hash_table<zz_profiler_item>::iterator it;
	ZZ_LOG("profiler: dump() started...\n");
	for (it = _items.begin(); it != _items.end(); it++) {
		ZZ_LOG("[%s] = count(%d), total(%f)\n", (*it).name, (*it).count, (*it).total);
	}
	ZZ_LOG("profiler: dump() ended...\n");
}

void zz_profiler::reset ()
{
	_total_profiled = 0;
}

double zz_profiler::get_total_profiled ()
{
	return double(_total_profiled)/_ticks_per_second*1000.0;
}
