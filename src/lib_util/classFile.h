#ifndef __CLASSFILE_H__
#define __CLASSFILE_H__

#include <stdio.h>
#include <io.h>
#include "classTHREAD.h"
#include "DLList.h"
#include "classSYNCOBJ.h"

#define		FILELOG_MAX_LENGTH	255

struct LogMSG
{
	char m_szMSG[ FILELOG_MAX_LENGTH ];
};

class classLogFILE : public classTHREAD 
{
private:
	CRITICAL_SECTION	m_csLOCK;
	FILE *				m_FP;
	// char *				m_StrBUFF;

	classDLLIST< LogMSG * >    m_LogLIST;
	classDLLIST< LogMSG * >    m_AddList;
	classEVENT			* m_pEvent;

	void Lock( void )
	{
		::EnterCriticalSection( &m_csLOCK );
	}

	void UnLock( void )
	{
		::LeaveCriticalSection( &m_csLOCK );
	}
	bool WriteAllString( void );
	void ClearAllLogLIST( void );

public:
	classLogFILE();
	~classLogFILE();

	bool Open( const char * szPrefix /*, short nBufferSIZE = 1024*/ );
	void Close( void );

	bool QueueString( const char * fmt , ... );

	virtual void Execute ( void );

	bool IsOpened( void )
	{
		return (m_FP != NULL);
	}

};

#endif
