#include "stdafx.h"
#include ".\callgm.h"
#include "../System/SystemProcScript.h"
#include "../Country.h"

CCallGM::CCallGM(void)
{
}

CCallGM::~CCallGM(void)
{
}

void CCallGM::CallGM( std::string& strServerName, std::string& strChannelName, std::string& strMsg  )
{
	std::string strSendedString = strServerName + "_" + strChannelName + "_" + strMsg;

	DWORD dwCountryCode = CCountry::GetSingleton().GetCountryCode();

	CSystemProcScript::GetSingleton().CallLuaFunction( "SendGMCall", 
												ZZ_PARAM_INT,
												dwCountryCode,
												ZZ_PARAM_STRING,
												strSendedString,
												ZZ_PARAM_END );	
}