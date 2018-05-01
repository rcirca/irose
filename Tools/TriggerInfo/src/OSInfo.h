// OSInfo.h: interface for the COSInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OSINFO_H__ADEC01E8_482B_4736_B5CB_11A50963C868__INCLUDED_)
#define AFX_OSINFO_H__ADEC01E8_482B_4736_B5CB_11A50963C868__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>


enum EOS_TYPE {
	EOS_WINDOWS				= 0x00000000,
		EOS_WINDOWS95		= 0x00000000,
		EOS_WINDOWS95OSR2	= 0x00000001,
		EOS_WINDOWS98		= 0x00000010,
		EOS_WINDOWS98SE		= 0x00000011,
		EOS_WINDOWSME		= 0x00000100,
	EOS_WINDOWS_NT			= 0x10000000,
		EOS_WINDOWS_NT3		= 0x10000000,
		EOS_WINDOWS_NT4		= 0x10001000,
		EOS_WINDOWS_2000	= 0x10010000,
		 EOS_WINDOWS_2000P	= 0x10010000,
		 EOS_WINDOWS_2000S	= 0x10010001,
		 EOS_WINDOWS_2000A	= 0x10010010,
		 EOS_WINDOWS_2000D	= 0x10010011,
		EOS_WINDOWS_XP		= 0x10100000,
		 EOS_WINDOWS_XPP	= 0x10110000,
		 EOS_WINDOWS_XPS	= 0x10110001,
		 EOS_WINDOWS_XPA	= 0x10110010,
};



class COSInfo  
{
public:
	COSInfo();
	virtual ~COSInfo();

public:
	void Init();
	LPCTSTR GetOSPlatform();
	LPCTSTR GetOSSuite();
	LPCTSTR GetVersion();

public:
	OSVERSIONINFOEX	m_os;
	std::string		m_strOSPlatform;
	std::string		m_strOSSuites;
	std::string		m_strServicePack;
	std::string		m_strVersion;
	EOS_TYPE		m_eos;
};

inline LPCTSTR COSInfo::GetOSPlatform()
{
	return m_strOSPlatform.c_str();
}

inline LPCTSTR COSInfo::GetOSSuite()
{
	return m_strOSSuites.c_str();
}

inline LPCTSTR COSInfo::GetVersion()
{
	return m_strVersion.c_str();
}

#endif // !defined(AFX_OSINFO_H__ADEC01E8_482B_4736_B5CB_11A50963C868__INCLUDED_)
