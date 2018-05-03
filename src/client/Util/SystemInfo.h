#ifndef _SYSTEM_INFO_
#define _SYSTEM_INFO_



enum enumWINDOWS_VERSION
{
	WINDOWS_98 = 0,
	WINDOWS_NT,
	WINDOWS_ME,
	WINDOWS_2000,
};


//----------------------------------------------------------------------------------------------------
/// class CSystemInfo
/// 유즈 시스템의 정보 관리
//----------------------------------------------------------------------------------------------------
class CSystemInfo
{
private:
	
	char					m_szLanguageBuffer[64];
	char					m_szWindowsFolder[1024];
	OSVERSIONINFO			m_OSVerInfo;
	int						m_iWindowsVersion;

public:
	CSystemInfo(void);
	~CSystemInfo(void);

	void					CollectingSystemInfo();	

	int						GetWindowsVersion(){ return m_iWindowsVersion; }
	const char*				GetLanguage(){ return m_szLanguageBuffer; }
	const char*				GetWindowsFolder(){ return m_szWindowsFolder; }
};

extern class CSystemInfo g_SystemInfo;

#endif //_SYSTEM_INFO_