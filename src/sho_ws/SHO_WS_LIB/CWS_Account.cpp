
#include "stdAFX.h"
#include "SHO_WS_LIB.h"
#include "WS_SocketLSV.h"
#include "CWS_Account.h"
#include "CWS_Client.h"

//-------------------------------------------------------------------------------------------------
CWS_Account::~CWS_Account ()
{
	if( 0 == m_btLoginBIT )
	{
		char * szAccount = this->m_Account.Get() ? this->m_Account.Get() : "unknown user";
		
		LogString( LOG_DEBUG, "Invalid logout process found for [ %s ]\n", szAccount );
	}

	// assert( 0 == m_btLoginBIT );

	//  월드서버에서 채널서버에 접속하기 전에 접속 종료할때는 m_pCLIENT를 NULL로 만들수 없네~~
//	assert( NULL == m_pCLIENT );
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
