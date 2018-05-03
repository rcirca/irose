#include "stdafx.h"
#include ".\livecheck.h"
#include "../Network/CNetwork.h"


/// for singleton
CLiveCheck	_liveCheck;


CLiveCheck::CLiveCheck(void)
{
	m_dwElapsedTime				= 0;
	m_dwLastRecoveryUpdateTime	= g_GameDATA.GetGameTime();
}

CLiveCheck::~CLiveCheck(void)
{
}

//----------------------------------------------------------------------------------------------
/// 2분에 한번씩 서버에 패킷 송신
//----------------------------------------------------------------------------------------------
void CLiveCheck::Check()
{
	const int iCheckTime = 270000;;
	//--------------------------------------------------------------------------------
	/// Update check frame
	//--------------------------------------------------------------------------------
	DWORD dwCurrentTime = g_GameDATA.GetGameTime();
	DWORD dwFrameElapsedTime = dwCurrentTime - m_dwLastRecoveryUpdateTime;		/// 이전프레임에서 현재 프레임 사이에 흐른시간을 더해준다.
	m_dwLastRecoveryUpdateTime = dwCurrentTime;

	m_dwElapsedTime += dwFrameElapsedTime;

	if( m_dwElapsedTime > iCheckTime )
	{
		g_pNet->Send_cli_HEADER( CLI_ALIVE ,false );
		LogString( LOG_NORMAL, "Send Live Packet[ %d ]", dwCurrentTime / 1000 );

		m_dwElapsedTime -= iCheckTime;
	}
}

void CLiveCheck::ResetTime()
{
	m_dwElapsedTime				= 0;
	m_dwLastRecoveryUpdateTime	= g_GameDATA.GetGameTime();
}