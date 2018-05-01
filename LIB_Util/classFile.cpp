#include <windows.h>
#include <stdio.h>
#include "classFILE.h"

//-------------------------------------------------------------------------------------------------
classLogFILE::classLogFILE ()
: m_FP( NULL ) , classTHREAD( true ), m_pEvent( NULL )
{
    ::InitializeCriticalSection( &m_csLOCK );
	m_pEvent = new classEVENT(NULL, false, false, NULL);
}

classLogFILE::~classLogFILE ()
{
	this->Terminate();
	m_pEvent->SetEvent();

	if( IsOpened() )
		fclose( m_FP );

	m_FP = NULL;
	if( m_LogLIST.GetNodeCount() > 0 ) // 여기서도 남아 있다는 얘기는 ....
		ClearAllLogLIST();

	::DeleteCriticalSection( &m_csLOCK );
	delete m_pEvent; m_pEvent = NULL;
}

//-------------------------------------------------------------------------------------------------
bool classLogFILE::Open( const char * szPrefix /*, short nBufferSIZE*/ )
{
	Lock();

	//	m_StrBUFF = new char[ nBufferSIZE ];
	
	SYSTEMTIME st;
	GetLocalTime( &st );
	char szFileNAME[ 256 ];
	sprintf( szFileNAME, "%s_ChatLog_%4d%02d%02d.txt", szPrefix, st.wYear, st.wMonth, st.wDay);

	if( _access( szFileNAME, 0 ) != 0 )
		m_FP = fopen( szFileNAME, "w+" );
	else
		m_FP = fopen( szFileNAME, "a+" );
	if( !m_FP )
	{
		UnLock();
		return false;
	}

	UnLock();
	return true;
}

//-------------------------------------------------------------------------------------------------
/// 여기는 반드시 Lock 걸린 상태에서 들어 와야 한다.
void classLogFILE::ClearAllLogLIST( void )
{
	classDLLNODE<LogMSG *> * pMsgNODE = m_LogLIST.GetHeadNode();
	while( pMsgNODE )
	{
		delete[] pMsgNODE->DATA;
		m_LogLIST.DeleteNFree( pMsgNODE );
		pMsgNODE = m_LogLIST.GetHeadNode(  );
	}
}

//-------------------------------------------------------------------------------------------------
/// 여기는 반드시 Lock 걸린 상태에서 들어 와야 한다.
bool classLogFILE::WriteAllString( void )
{
	if( !IsOpened() )
		return false;

	classDLLNODE<LogMSG *> * pMsgNODE = m_LogLIST.GetHeadNode();
	while( pMsgNODE )
	{
		fwrite( pMsgNODE->DATA->m_szMSG, sizeof(char), strlen( pMsgNODE->DATA->m_szMSG ), m_FP );
		pMsgNODE = m_LogLIST.GetNextNode( pMsgNODE );
	}
	
	return true;
}

//-------------------------------------------------------------------------------------------------
void classLogFILE::Execute( void )
{
	return;

	while( !this->Terminated ) {
		m_pEvent->WaitFor( 0xffffffff );

		if( this->Terminated )
			break;

		Lock ();
		
		classDLLNODE<LogMSG *> * pMsgNODE = m_AddList.GetHeadNode();
		while( pMsgNODE )
		{
			m_LogLIST.AllocNAppend( pMsgNODE->DATA );
			m_AddList.DeleteNFree( pMsgNODE );
			pMsgNODE = m_AddList.GetHeadNode();
		}
		UnLock ();
		m_pEvent->ResetEvent();

		WriteAllString() ;

		ClearAllLogLIST();
	}
}

//-------------------------------------------------------------------------------------------------
bool classLogFILE::QueueString( const char *fmt, ...)
{
	if( this->Terminated )
		return true;

	if( !IsOpened() )
		return false;

	Lock();

	char StrBUFF[ 1024 ];
    try {
        va_list argptr;
        va_start(argptr, fmt);
        vsprintf(StrBUFF, fmt, argptr);
        va_end(argptr);
    }
    catch( ... ) {
		UnLock();
        return false;
    }

	LogMSG * pNewMSG = new LogMSG;
	m_AddList.AllocNAppend( pNewMSG );
	strncpy( pNewMSG->m_szMSG, StrBUFF, FILELOG_MAX_LENGTH );

	UnLock();

	m_pEvent->SetEvent();

	return true;
}

//-------------------------------------------------------------------------------------------------
void classLogFILE::Close( void )
{
	Lock();
	if( IsOpened() )
		fclose( m_FP );
	m_FP = NULL;
	UnLock();
}