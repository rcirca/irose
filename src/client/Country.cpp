#include "stdafx.h"
#include ".\country.h"


unsigned long ReadRegistryKeyValue (HKEY hRootKey , const char * Path, const char * KeyName )
{
	HKEY hKey;
	LPBYTE lpbVALUE ;
	DWORD buflen, ret;
	char szZeroEndedPath[ 1024 ];

	sprintf ( szZeroEndedPath, "%s", Path );

    if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, szZeroEndedPath, 0, KEY_READ, &hKey)) 
	{
        lpbVALUE = 0;
        buflen = sizeof( lpbVALUE );
        ret = RegQueryValueEx(hKey, KeyName, NULL, NULL, (LPBYTE) &lpbVALUE, &buflen);
		
		RegCloseKey ( hKey );
		if (ret == ERROR_SUCCESS) 
			return (unsigned long)lpbVALUE;
    }

	return 0;
}

CCountry _country;

CCountry::CCountry(void)
{
	m_dwCountryCode = 0;
}

CCountry::~CCountry(void)
{
}

bool CCountry::CheckCountry()
{
	char * Path =  "SOFTWARE\\TriggerSoft\\Rose Online";
	unsigned long dwValue = ReadRegistryKeyValue ( 	HKEY_LOCAL_MACHINE, Path, "Language" );


	memset( m_bIsThatCountry, 0, sizeof( m_bIsThatCountry ) );	
	m_dwCountryCode = 0;

	if( dwValue < COUNTRY_MAX )
	{
		m_dwCountryCode				= dwValue;
		m_bIsThatCountry[ dwValue ] = true;
		return true;
	}

	return false;
}
//*-----------------------------------------------------------------------------------------/
/// 새로운 시스템을 적용시켜야 하는가?
//*-----------------------------------------------------------------------------------------/
bool CCountry::IsApplyNewVersion()
{
//	if( this->IsTAIWAN() || this->IsPhilipplines() || this->IsUSA() || this->IsEU() ) 
//		return true; 

	if( this->IsKorea() || this->IsJapan() )
	{
		return false;
	}

	return false;
	//return true;
}

//*-----------------------------------------------------------------------------------------/
///UseItem Delay를 새롭게 적용될 국가체크 
//*-----------------------------------------------------------------------------------------/
bool CCountry::IsUseItemDelayNewVersion()
{
	return IsApplyNewVersion();
}

DWORD CCountry::GetCountryCode()
{ 
	return m_dwCountryCode; 
}
bool CCountry::IsTAIWAN()
{ 
	return m_bIsThatCountry[ COUNTRY_CHA_TW ]; 
}
bool CCountry::IsROSE() 
{ 
	return m_bIsThatCountry[ COUNTRY_IROSE ]; 
}
bool CCountry::IsPhilipplines()
{ 
	return m_bIsThatCountry[ COUNTRY_PH ]; 
}
bool CCountry::IsJapan()
{
	return m_bIsThatCountry[ COUNTRY_JPN ];
}
bool CCountry::IsUSA()
{
	return m_bIsThatCountry[ COUNTRY_USA ];
}
bool CCountry::IsKorea()
{
	return m_bIsThatCountry[ COUNTRY_KOR ];
}
bool CCountry::IsEU()
{
	return m_bIsThatCountry[ COUNTRY_EU ];
}

//---------------------------------------------------------------------------------------
/// @brief 클라이언트 실행시 로드된 레지스트리에서 인자로 넘어온 국가코드가 같은지 판단한다.
//---------------------------------------------------------------------------------------
bool CCountry::IsCountry( int country )
{
	/// 배열 범위 체크
	assert( country >= 0 && country < COUNTRY_MAX );
	if( country < 0 && country >= COUNTRY_MAX ) return false;

	return m_bIsThatCountry[ country ];
}

//--------------------------------------------------------------------------------------
bool IsTAIWAN()
{	
	return true;//( CCountry::GetSingleton().IsTAIWAN() || CCountry::GetSingleton().IsPhilipplines() || CCountry::GetSingleton().IsUSA() );
}


