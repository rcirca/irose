/** 
 * @file zz_mem.h
 * @brief memory class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-feb-2002
 *
 * $Header: /engine/include/zz_mem.h 9     04-02-27 5:48p Zho $
 * $History: zz_mem.h $
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-02-27   Time: 5:48p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-02-24   Time: 2:17p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-02-21   Time: 6:45p
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-20   Time: 12:18p
 * Updated in $/engine/include
 * Mem offed in debug mode.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-17   Time: 2:05p
 * Updated in $/engine/include
 * make thread-safe.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-15   Time: 8:34p
 * Updated in $/engine/include
 * Readded memory check.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-13   Time: 6:56p
 * Updated in $/engine/include
 * Not to use mem manager
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
 * User: Zho          Date: 03-11-30   Time: 7:53p
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

/*
MSDN Library: C++ Language Reference:

Initializing Static Objects:
=================

Global static objects are initialized in the order they occur in the source. They are destroyed in the 
reverse order. Across translation units, however, the order of initialization is dependent on how the
object files are arranged by the linker; the order of destruction still takes place in the reverse of 
that in which objects were constructed.

Local static objects are initialized when they are first encountered in the program flow, 
and they are destroyed in the reverse order at program termination. Destruction of local static objects 
occurs only if the object was encountered and initialized in the program flow.
*/

#ifndef	__ZZ_MEM_H__
#define __ZZ_MEM_H__

#define ZZ_MEM_USE_MULTITHREAD

#ifdef ZZ_MEM_ON

#include <stdlib.h>

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#ifndef __ZZ_TREE_H__
#include "zz_tree.h"
#endif

#ifndef __ZZ_CRITICAL_SECTION_H__
#include "zz_critical_section.h"
#endif

// memory allocation & freeing
// memory status logging
// memory leak detection
//--------------------------------------------------------------------------------
struct zz_mem {
private:
	static bool _started;

	static zz_critical_section * _cs;

	// total memory (physical?)
	static size_t _total;

	// maximum usage log
	static size_t _max;

	// used memory ( remained = _total - _used )
	static size_t _used;
	
	static int _count;
	struct _mem_info {
		int index;
		size_t size;
	};

	// contain memory size information
	// pointer(const void *), pointer_size(long)
	typedef zz_tree<const void *, _mem_info> zz_mem_map;
	static zz_mem_map _mem_map;

protected:
	// show memory allocation status
	static void status();
	
	// reset total memory size
	static void reset_total (size_t total) { _total = total; }

	static void confirm ();

	// start memory managing
	static void begin ();

	// stop memory managing
	static void end ();

public:
	zz_mem ();
	~zz_mem ();

	// memory allocation
	static void * malloc (size_t size);
	
	// memory free
	static void free (void * p);	
};

#ifdef ZZ_MEM_REDEFINE_OFF
#define zz_new new
#define zz_delete delete
#else

#ifdef new
#undef new
#endif // new

#ifdef delete
#undef delete
#endif // delete

// use global operator new and delete by "::"
#define zz_new ::new(__FILE__, __LINE__)
// the delete operator cannot have parameters
#define zz_delete ::delete

static inline void * operator new (size_t size)
{
	//ZZ_LOG("mem: new\n");
	return zz_mem::malloc(size);
}

static inline void *operator new[] (size_t size)
{
	//ZZ_LOG("mem: new\n");
	return zz_mem::malloc(size);
}

static inline void * operator new (size_t size, const char * file, int line)
{
	//ZZ_LOG("mem: new at %s(file), %d(line)\n", file, line);
	return zz_mem::malloc(size);
}

static inline void * operator new[] (size_t size, const char * file, int line)
{
	//ZZ_LOG("mem: new[] at %s(file), %d(line)\n", file, line);
	return zz_mem::malloc(size);
}

static inline void operator delete (void * p)
{
	//ZZ_LOG("mem: delete\n");
	zz_mem::free(p);
	//zz_mem::status();
}

static inline void operator delete[] (void *p)
{
	//ZZ_LOG("mem: delete[]\n");
	zz_mem::free(p);
	//zz_mem::status();
}

// to disable "Compiler Warning (level 1) C4291"
static inline void operator delete (void * p, const char * file, int line)
{
	//ZZ_LOG("mem: delete\n");
	zz_mem::free(p);
	//zz_mem::status();
}

// to disable "Compiler Warning (level 1) C4291"
static inline void operator delete[] (void *p, const char * file, int line)
{
	//ZZ_LOG("mem: delete[]\n");
	zz_mem::free(p);
	//zz_mem::status();
}

#endif // ZZ_MEM_REDEFINE_OFF

// usage :
/*
#ifndef __ZZ_MEM_H__
#include "zz_mem.h"
#endif


void main ()
{
	zz_mem::reset_total(10);
	{
		char * haha = zz_new char[100];
		zz_delete[] haha;
	}
}
*/

#else
#define zz_new new
#define zz_delete delete
#endif // ZZ_MEM_ON

#endif // __ZZ_MEM_H__
