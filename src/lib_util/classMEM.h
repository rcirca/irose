/** 
 * @file zz_mem.h
 * @brief memory class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-feb-2002
 *
 * $Header: /LIB_Util/classMEM.h 1     03-07-03 11:38a Icarus $
 */

#ifndef	__ZZ_MEM_H__
#define __ZZ_MEM_H__

#include <stdlib.h>
#ifndef __ZZ_LOG__
#include "..\client\util\classLOG.h"
#endif

// memory allocation & freeing
// memory status logging
// memory leak detection
struct zz_mem {
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

	/*
	// contain memory size information
	// pointer(const void *), pointer_size(long)
	typedef zz_tree<const void *, _mem_info> zz_mem_map;
	static zz_mem_map _mem_map;
	*/
//public:
	zz_mem();
	~zz_mem();
	
	// memory allocation
	static void * malloc (size_t size);
	
	// memory free
	static void free (void * p);
	
	// show memory allocation status
	static void status();
	
	// reset total memory size
	static void reset_total (size_t total) { _total = total; }
};

// __ZZ_MEM_NOCUSTOM__ used only zz_mem.cpp
// because zz_mem.cpp does not use custom memory operator
// All codes which include zz_mem.h use this custom memory operator(new, delete)
// If you do log [file name] and [line number] in which the operator is executed,
// use the zz_new and zz_delete operator.


//// In Release mode, we do not use memory manager!!!
#ifndef _DEBUG
#define ZZ_MEM_OFF
#define ZZ_MEM_MANAGER_OFF
#endif //_DEBUG

#ifndef ZZ_MEM_OFF

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
	return zz_mem::malloc(size);
}

static inline void *operator new[] (size_t size)
{
	return zz_mem::malloc(size);
}

static inline void * operator new (size_t size, const char * file, int line)
{
	LogString (LOG_NORMAL, " %s / %d \n", file, line );

	return zz_mem::malloc(size);
}

static inline void * operator new[] (size_t size, const char * file, int line)
{
	LogString (LOG_NORMAL, " %s / %d \n", file, line );

	return zz_mem::malloc(size);
}

static inline void operator delete (void * p)
{
	zz_mem::free(p);
}

static inline void operator delete[] (void *p)
{
	zz_mem::free(p);
}

// to disable "Compiler Warning (level 1) C4291"
static inline void operator delete (void * p, const char * file, int line)
{
	zz_mem::free(p);
}

// to disable "Compiler Warning (level 1) C4291"
static inline void operator delete[] (void *p, const char * file, int line)
{
	zz_mem::free(p);
}

#else 
#define zz_new new
#define zz_delete delete
#endif // ZZ_MEM_OFF

// usage :
/*
#ifndef __ZZ_mem__
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

#endif // __ZZ_MEM_H__






/*
void * operator new( unsigned int cb )
{
    void *res = _nh_malloc( cb, 1 );

    return res;
}

*/