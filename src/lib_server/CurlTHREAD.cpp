
#include <windows.h>
#include "classUTIL.h"
#include "classLOG.h"
#include "CurlTHREAD.h"

//-------------------------------------------------------------------------------------------------
CurlTHREAD::CurlTHREAD () : classTHREAD( false )
{
	m_pEVENT = new classEVENT(NULL, false, false, NULL);

	// THREAD_PRIORITY_BELOW_NORMAL :: Indicates 1 point below normal priority for the priority class
	// THREAD_PRIORITY_LOWEST :: Indicates 2 points below normal priority for the priority class.
	this->SetPriority( THREAD_PRIORITY_BELOW_NORMAL );
}
CurlTHREAD::~CurlTHREAD ()
{
	SAFE_DELETE( m_pEVENT );
}

//-------------------------------------------------------------------------------------------------
void CurlTHREAD::Execute ()
{
	g_LOG.CS_ODS( 0xffff, ">>>> CurlTHREAD::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );

    m_HTTP.Open ();


    while( TRUE  ) {
		if ( m_pEVENT->WaitFor( INFINITE ) != WAIT_OBJECT_0 )
			break;
		if ( this->Terminated )
			break;

        m_HTTP.OpenURL( m_szURL );
	}

    m_HTTP.Close ();

	g_LOG.CS_ODS( 0xffff, "<<<< CurlTHREAD::Execute() ThreadID: %d(0x%x)\n", this->ThreadID, this->ThreadID );
}

//-------------------------------------------------------------------------------------------------
void CurlTHREAD::Free ()
{
    this->Terminate ();
	do {
		this->m_pEVENT->SetEvent ();
	} while ( !IsFinished() ) ;
}

//-------------------------------------------------------------------------------------------------
void CurlTHREAD::SendURL (const char *szURL)
{
    // m_URL.printf( "http://211.218.152.151/gamestatic/game_conn.asp?gameid=RHAKMU&servername=%s&conncount=%d", szServerName, iUserCnt);
	strcpy (m_szURL, szURL);

    m_pEVENT->SetEvent ();
}

//-------------------------------------------------------------------------------------------------
