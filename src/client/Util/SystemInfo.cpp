#include "stdafx.h"
#include ".\systeminfo.h"


class CSystemInfo g_SystemInfo;

CSystemInfo::CSystemInfo(void)
{
	m_OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
}

CSystemInfo::~CSystemInfo(void)
{
}

void CSystemInfo::CollectingSystemInfo()
{
	// Get system language
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SENGLANGUAGE, 
					m_szLanguageBuffer, sizeof(m_szLanguageBuffer));

	GetVersionEx( &m_OSVerInfo );
	if (m_OSVerInfo.dwMajorVersion == 4)
	{
		if (m_OSVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
			m_iWindowsVersion = WINDOWS_98;
		else
			m_iWindowsVersion = WINDOWS_NT;
	}
	else if (m_OSVerInfo.dwMajorVersion == 5)
	{
		if (m_OSVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
			m_iWindowsVersion = WINDOWS_98;
		else
			m_iWindowsVersion = WINDOWS_2000;
	}

	GetWindowsDirectory( m_szWindowsFolder, sizeof(m_szWindowsFolder));
}