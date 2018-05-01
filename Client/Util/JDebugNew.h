#ifndef _DEBUGNEW_
#define _DEBUGNEW_

#include <stdio.h>
#include <list>

typedef struct 
{
	DWORD	address;
	DWORD	size;
	char	file[64];
	DWORD	line;
	char    AllocType;			// 0 : normal, 1: array
} ALLOC_INFO;


typedef std::list<ALLOC_INFO*> AllocList;


void AddTrack(DWORD addr,  DWORD asize,  const char *fname, DWORD lnum, char cAllocType = 0 );
void RemoveTrack(DWORD addr, char cAllocType = 0 );
void DumpUnfreedToOutputString();
void DumpUnfreedToFile();

#if ( 0 )

	#ifdef _DEBUG
	#define DEBUG_NEW new(__FILE__, __LINE__)
	#else
	#define DEBUG_NEW new
	#endif

	#ifdef _DEBUG
	void * __cdecl operator new(unsigned int size, const char *file, int line);
	void __cdecl operator delete(void *p );

	void * __cdecl operator new[](unsigned int size, const char *file, int line);
	void __cdecl operator delete[](void *p );
	#endif

#else
	#define DEBUG_NEW new
#endif


#define new DEBUG_NEW

#endif //_DEBUGNEW_
