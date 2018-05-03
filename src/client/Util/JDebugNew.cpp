#include "stdafx.h"
#include "JDebugNew.h"

//------------------------------------------------------------------------
// AddTrack 내부에서 new 를 사용하는것은 제외해야한다..
#undef new

AllocList *allocList;

void AddTrack(DWORD addr,  DWORD asize,  const char *fname, DWORD lnum, char cAllocType )
{
	ALLOC_INFO *info;

	if(!allocList) {
		allocList = new(AllocList);
	}

	info = new(ALLOC_INFO);
	info->address = addr;
	strncpy(info->file, fname, 63);
	info->line = lnum;
	info->size = asize;
	info->AllocType = cAllocType;
	allocList->insert(allocList->begin(), info);
};

void RemoveTrack(DWORD addr, char cAllocType )
{
	AllocList::iterator i;
	ALLOC_INFO	*info;

	if(!allocList)
		return;
	for(i = allocList->begin(); i != allocList->end(); i++)
	{
		if((*i)->address == addr)
		{
			info = *i;
			if( info->AllocType == cAllocType )
			{
				allocList->remove((*i));
			}

			break;
		}
	}
};

void DumpUnfreedToOutputString()
{
	AllocList::iterator i;
	DWORD totalSize = 0;
	char buf[1024];

	if(!allocList)
		return;

	for(i = allocList->begin(); i != allocList->end(); i++) {
		sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n",
			(*i)->file, (*i)->line, (*i)->address, (*i)->size);
		OutputDebugString(buf);
		totalSize += (*i)->size;
	}
	sprintf(buf, "-----------------------------------------------------------\n");
	OutputDebugString(buf);
	sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
	OutputDebugString(buf);
};

void DumpUnfreedToFile()
{
	AllocList::iterator i;
	DWORD totalSize = 0;
	char buf[1024];

	if(!allocList)
		return;


	FILE*	fp = fopen( "MemLeak.txt", "wt" );

	if( fp == NULL )
		return;

	for(i = allocList->begin(); i != allocList->end(); i++) 
	{
		fprintf( fp, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n",
			(*i)->file, (*i)->line, (*i)->address, (*i)->size );				
		totalSize += (*i)->size;
	}

	fprintf( fp, "-----------------------------------------------------------\n" );	
	fprintf( fp, "Total Unfreed: %d bytes\n", totalSize );

	fclose( fp );	
};


#if ( 0 )
#ifdef _DEBUG
void * __cdecl operator new(unsigned int size, const char *file, int line)
{
	void *ptr = (void *)malloc(size);
	AddTrack((DWORD)ptr, size, file, line);
	return(ptr);
};
void __cdecl operator delete(void *p )
{
	RemoveTrack((DWORD)p);
	free(p);
};

void * __cdecl operator new[](unsigned int size, const char *file, int line)
{
	void *ptr = (void *)malloc(size);
	AddTrack((DWORD)ptr, size, file, line, 1 );
	return(ptr);
};
void __cdecl operator delete[](void *p )
{
	RemoveTrack((DWORD)p, 1 );
	free(p);
};
#endif
#endif