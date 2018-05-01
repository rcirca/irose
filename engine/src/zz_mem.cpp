/** 
 * @file zz_mem.cpp
 * @brief memory class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-feb-2002
 *
 * $Header: /engine/src/zz_mem.cpp 5     04-02-27 5:48p Zho $
 * $History: zz_mem.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-27   Time: 5:48p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-21   Time: 6:45p
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-17   Time: 2:06p
 * Updated in $/engine/src
 * Make it thread-safe.
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
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#ifdef ZZ_MEM_ON

#define ZZ_MEM_REDEFINE_OFF // force only this zz_mem.cpp

#define ZZ_MEM_NEWDELETE_ON
//#define ZZ_WIN32_CRTDBG
//-----------------------------------//

//#ifdef ZZ_WIN32_CRTDBG
//#define CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#endif

#include "zz_tier0.h"
#include "zz_log.h"

#include <stdlib.h>

#ifdef ZZ_MEM_USE_MULTITHREAD
#include "zz_autolock.h"
#endif

//--------------------------------------------------------------------------------
size_t zz_mem::_total = ZZ_MAX_MEMORY; // 100 mega
size_t zz_mem::_used = 0;
size_t zz_mem::_max = 0;
int zz_mem::_count = 0;
bool zz_mem::_started = false;

zz_mem::zz_mem_map zz_mem::_mem_map;

#ifdef ZZ_MEM_USE_MULTITHREAD
zz_critical_section * zz_mem::_cs = NULL;
#endif // ZZ_MEM_USE_MULTITHREAD

zz_log g_log; // log first
zz_mem g_mem; // end second
//--------------------------------------------------------------------------------

zz_mem::zz_mem() 
{
	begin();
}

zz_mem::~zz_mem() 
{
	end();
}

void zz_mem::confirm ()
{
#ifdef ZZ_WIN32_CRTDBG
	if (_CrtDumpMemoryLeaks() == TRUE) {
		ZZ_LOG("mem: memory leaked!!!\n");
	}
#endif // ZZ_WIN32_CRTDBG

	if (_used != 0) {
		ZZ_LOG("mem: end failed. memory leak\n");
		for (zz_mem_map::iterator it = _mem_map.begin(); it != _mem_map.end(); it++) {
			ZZ_LOG("%d ", it.value()->index);
		}
		ZZ_LOG(" not freed.\n");
	}
	else {
		ZZ_LOG("mem: end.\n");
	}
}

void zz_mem::status() 
{
	ZZ_LOG("mem: status(): total(%dM:%d) = used(%dM:%d) + free(%dM:%d). max(%dM:%d)\n", 
		_total/1000000, _total,
		_used/1000000, _used,
		(_total - _used)/1000000, _total - _used,
		_max/1000000, _max);
}

void * zz_mem::malloc (size_t size) 
{ 
	if (!_started) {
		void * p = ::malloc(size);
		ZZ_LOG("mem: mem not started. malloc(%x:%d) done.\n", p, size);
		return p;
	}

#ifdef ZZ_MEM_USE_MULTITHREAD
	zz_autolock<zz_critical_section> locker(_cs);
#endif // ZZ_MEM_USE_MULTITHREAD

	void * p;

	if (_used + size > _total) {
		ZZ_LOG("mem: cannot allocate memory(size:%d bytes out of total %d bytes)!\n", size, _total);
		assert(!"out of memory");
		return NULL;
	}

	_used += size;

	//_max = (_used > _max) ? _used : _max;
	if (_used > _max) {
		_max = _used;
		//ZZ_LOG("mem::malloc() _max = %d\n", _max);
	}
	
#ifdef ZZ_MEM_NEWDELETE_ON
	p = new char[size];
#else // ZZ_MEM_NEWDELETE_ON

	p = ::malloc(size);
#endif
	
	//ZZ_LOG("mem: allocated %d bytes (0x%x)\n", size, p);
	//status();

	// CAUTION: break on here to catch memory leak by index ( _count == XXX )
	_mem_info info;
	info.index = _count++;
	info.size = size;

	if (_mem_map.insert(p, info) == _mem_map.end()) {
		assert(!"mem:malloc() failed.");
		ZZ_LOG("mem:malloc() failed.\n");
		status();
		return NULL;
	}
	//_mem_map.insert(zz_mem_map::value_type(p, size));
	return p;
}

void zz_mem::free (void * p) 
{ 
	if (!_started) {
		ZZ_LOG("mem: mem not started. free(%x) done.\n", p);
		return ::free(p);
	}

#ifdef ZZ_MEM_USE_MULTITHREAD
	zz_autolock<zz_critical_section> locker(_cs);
#endif // ZZ_MEM_USE_MULTITHREAD

	int size;
	zz_mem_map::iterator the_iterator;

    the_iterator = _mem_map.find(p);
    if (the_iterator == _mem_map.end()) {
		assert(!"mem:free() failed.");
		ZZ_LOG("mem:free() failed.\n");
        return;
    }
    
#ifdef ZZ_MEM_NEWDELETE_ON
	delete [] p;
#else
	::free(p);
#endif // ZZ_MEM_NEWDELETE_ON
	
	p = NULL;

	size = int(the_iterator.value()->size);
	//size = (*the_iterator).second;

	//ZZ_LOG("mem: freed %d bytes\n", size);
	//ZZ_LOG("mem: freed %d[index = %d] bytes\n", size, the_iterator.value()->index);
	_used -= size;

	_mem_map.erase(the_iterator);
	
	//status();
}

void zz_mem::begin ()
{
	if (_started) return;

#ifdef ZZ_MEM_USE_MULTITHREAD
	_cs = new zz_critical_section;
#endif // ZZ_MEM_USE_MULTITHREAD

	_started = true;

	ZZ_LOG("mem: begin. total %d byte available.\n", _total);

#ifdef ZZ_WIN32_CRTDBG
	//// crt debug flag setting
	//_CrtSetDbgFlag(
	//	_CRTDBG_ALLOC_MEM_DF
	//	| _CRTDBG_LEAK_CHECK_DF
	//	// | _CRTDBG_CHECK_ALWAYS_DF
	//	// | _CRTDBG_CHECK_EVERY_16_DF
	//	);
#endif // ZZ_WIN32_CRTDBG
}

void zz_mem::end ()
{
	if (!_started) return;
	
	zz_mem::status();
	zz_mem::confirm();

	_started = false;

#ifdef ZZ_MEM_USE_MULTITHREAD
	if (_cs) {
		delete _cs;
		_cs = NULL;
	}
#endif // ZZ_MEM_USE_MULTITHREAD
}

#endif // ZZ_MEM_ON
