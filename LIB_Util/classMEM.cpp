/*
*/
#include <windows.h>
#include "classMEM.h"
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/** 
 * @file zz_mem.cpp
 * @brief memory class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-feb-2002
 *
 * $Header: /znzin11/engine/source/zz_mem.cpp 6     03-06-11 9:11a Zho $
 */

//-----------------------------------//
// macros setting

#define ZZ_USE_NEWDELETE
//#define ZZ_WIN32_CRTDBG
//-----------------------------------//

#define ZZ_MEM_OFF // force only this zz_mem.cpp

//#ifdef ZZ_WIN32_CRTDBG
//#define CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#endif

#include <stdlib.h>


size_t zz_mem::_total = 100000000;//ZZ_MAX_MEMORY; // 100 mega
size_t zz_mem::_used = 0;
size_t zz_mem::_max = 0;
int zz_mem::_count = 0;

// zz_mem::zz_mem_map zz_mem::_mem_map;

zz_mem::zz_mem() 
{
	LogString(LOG_NORMAL, "mem: created. total %d byte available.\n", _total);

#ifdef ZZ_WIN32_CRTDBG
	//// crt debug flag setting
	//_CrtSetDbgFlag(
	//	_CRTDBG_ALLOC_MEM_DF
	//	| _CRTDBG_LEAK_CHECK_DF
	//	// | _CRTDBG_CHECK_ALWAYS_DF
	//	// | _CRTDBG_CHECK_EVERY_16_DF
	//	);
#endif
}

zz_mem::~zz_mem() 
{
	status();

#ifdef ZZ_WIN32_CRTDBG
	if (_CrtDumpMemoryLeaks() == TRUE) {
		LogString(LOG_NORMAL, "mem: memory leaked!!!\n");
	}
#endif

	if (_used != 0) {
		LogString(LOG_NORMAL, "mem: destroyed. Memory leaked!!!\n");
		/*
		for (zz_mem_map::iterator it = _mem_map.begin(); it != _mem_map.end(); it++) {
			LogString(LOG_NORMAL, "%d ", it.value()->index);
		}
		*/
		LogString(LOG_NORMAL, " not freed.\n");
	}
	else {
		LogString(LOG_NORMAL, "mem: destroyed. OK\n");
	}
}

void zz_mem::status() 
{
	LogString(LOG_NORMAL, "mem: status(): total(%d) = used(%d) + remained(%d). highest(%d)\n", 
		_total, _used, _total - _used, _max);
}

void * zz_mem::malloc (size_t size) 
{ 
	void * p;

	if (_used + size > _total) {
		LogString(LOG_NORMAL, "mem: cannot allocate memory(size:%d bytes out of total %d bytes)!\n", size, _total);
		return NULL;
	}

	_used += size;

	//_max = (_used > _max) ? _used : _max;
	if (_used > _max) {
		_max = _used;
		//LogString(LOG_NORMAL, "mem::malloc() _max = %d\n", _max);
	}
	
#ifdef ZZ_USE_NEWDELETE
	p = new char[size];
#else	
	p = ::malloc(size);
#endif
	
	//LogString(LOG_NORMAL, "mem: allocated %d bytes (0x%x)\n", size, p);
	//status();

	// CAUTION: break on here to catch memory leak by index ( _count == XXX )
	_mem_info info;
	info.index = _count++;
	info.size = size;

//	_mem_map.insert(p, info);
	//_mem_map.insert(zz_mem_map::value_type(p, size));
	return p;
}

void zz_mem::free (void * p) 
{ 
	int size;
	/*
	zz_mem_map::iterator the_iterator;

    the_iterator = _mem_map.find(p);
    if (the_iterator == _mem_map.end()) {
        return;
    }
    */
#ifdef ZZ_USE_NEWDELETE
	delete [] p;
#else
	::free(p);
#endif
	
	p = NULL;

//	size = int(the_iterator.value()->size);
	//size = (*the_iterator).second;

	//LogString(LOG_NORMAL, "mem: freed %d bytes\n", size);
	//LogString(LOG_NORMAL, "mem: freed %d[index = %d] bytes\n", size, the_iterator.value()->index);
	_used -= size;

//	_mem_map.erase(the_iterator);
	
	//status();
}

#ifndef ZZ_MEM_MANAGER_OFF
static zz_mem zzmem; // just one zz_mem instance
#endif // ZZ_MEM_MANAGER_OFF