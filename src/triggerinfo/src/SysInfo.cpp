#include "stdafx.h"


//#include "CpuInfo.h"
#include "__d3d9.h"
#include <direct.h>
//#include "getdxver.h"
#include "dispinfo.h"
#include "SysInfo.h"
#define BUFSIZE 255




CSysInfo::CSysInfo ()
{

	CD3D9 D3dInfo;

	if(D3dInfo.Init())
	{
	D3dInfo.GetDisplayInfo (&m_mapVideo); // map 형
	D3dInfo.GetDisplayInfo(m_DisplayInfo); // vector 형 - dxdiag 기반.
	D3dInfo.GetSystemInfo(&m_SystemInfo);
	D3dInfo.Release (); 
	}

	m_system = m_SystemInfo;	// 시스템 정보
    m_display1 = *(m_DisplayInfo[0]); // 1번 디스플레이
}

CSysInfo::~CSysInfo ()
{
}

void CSysInfo::GetOSInfo (void)
{
	m_mapOS.clear ();

	m_mapOS[ "Language" ] = (string)m_system->m_szLanguagesEnglish; // 현재 언어.

	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return;
	}

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:

		// Test for the specific product family.
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			m_mapOS[ "Name" ] = "Microsoft Windows Server 2003 family";
		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			m_mapOS[ "Name" ] = "Microsoft Windows XP ";
		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			m_mapOS[ "Name" ] = "Microsoft Windows 2000 ";
		else if ( osvi.dwMajorVersion <= 4 )
			m_mapOS[ "Name" ] = "Microsoft Windows NT ";

		// Test for specific product on Windows NT 4.0 SP6 and later.
		if( bOsVersionInfoEx )
		{
			// Test for the workstation type.
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				if( osvi.dwMajorVersion == 4 )
					m_mapOS[ "Edition" ] = "Workstation 4.0 ";
				else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
					m_mapOS[ "Edition" ] = "Home Edition ";
				else
					m_mapOS[ "Edition" ] = "Professional ";
			}
			// Test for the server type.
			else if ( osvi.wProductType == VER_NT_SERVER )
			{
				if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						m_mapOS[ "Edition" ] = "Datacenter Edition " ;
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						m_mapOS[ "Edition" ] = "Enterprise Edition " ;
					else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
						m_mapOS[ "Edition" ] = "Web Edition " ;
					else
						m_mapOS[ "Edition" ] = "Standard Edition " ;
				}
				else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						m_mapOS[ "Edition" ] = "Datacenter Server " ;
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						m_mapOS[ "Edition" ] = "Advanced Server " ;
					else
						m_mapOS[ "Edition" ] = "Server " ;
				}
				else  // Windows NT 4.0 
				{
					if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						m_mapOS[ "Edition" ] = "Server 4.0, Enterprise Edition ";
					else
						m_mapOS[ "Edition" ] = "Server 4.0 ";
				}
			}
		}
		else  // Test for specific product on Windows NT 4.0 SP5 and earlier
		{
			HKEY hKey;
			char szProductType[BUFSIZE];
			DWORD dwBufLen=BUFSIZE;
			LONG lRet;

			lRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE
							, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions"
							, 0
							, KEY_QUERY_VALUE
							, &hKey );
			if( lRet == ERROR_SUCCESS )
			{

				lRet = RegQueryValueEx ( hKey
							, "ProductType"
							, NULL
							, NULL
							, (LPBYTE) szProductType
							, &dwBufLen);
				if( (lRet == ERROR_SUCCESS) && (dwBufLen <= BUFSIZE) )
				{
					RegCloseKey( hKey );

					string strEdition;
					if ( lstrcmpi( "WINNT", szProductType) == 0 )
						strEdition = "Workstation ";
					if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
						strEdition = "Server ";
					if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
						strEdition = "Advanced Server ";

					char buff[ 32 ];
					sprintf (buff, "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
					m_mapOS[ "Edition" ] = strEdition + buff;
				}
			}
		}

		// Display service pack (if any) and build number.
		char szSrvPack[ 255 ];
		if( osvi.dwMajorVersion == 4 && 
			lstrcmpi( osvi.szCSDVersion, "Service Pack 6" ) == 0 )
		{
			HKEY hKey;
			LONG lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE
								, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"
								, 0
								, KEY_QUERY_VALUE
								, &hKey );
			if( lRet == ERROR_SUCCESS )
				sprintf (szSrvPack, "Service Pack 6a (Build %d)", osvi.dwBuildNumber & 0xFFFF );         
			else // Windows NT 4.0 prior to SP6a
			{
				sprintf (szSrvPack, "%s (Build %d)"
						, osvi.szCSDVersion
						, osvi.dwBuildNumber & 0xFFFF);
			}

			RegCloseKey( hKey );

			m_mapOS[ "Service Pack" ] = szSrvPack;
		}
		else // Windows NT 3.51 and earlier or Windows 2000 and later
		{
			sprintf (szSrvPack, "%s (Build %d)", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
			m_mapOS[ "Service Pack" ] = szSrvPack;
		}


		break;

		// Test for the Windows 95 product family.
	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			m_mapOS[ "Name" ] = "Microsoft Windows 95 ";
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				m_mapOS[ "Name" ] += "OSR2 ";
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			m_mapOS[ "Name" ] = "Microsoft Windows 98 ";
			if ( osvi.szCSDVersion[1] == 'A' )
				m_mapOS[ "Name" ] += "SE ";
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			m_mapOS[ "Name" ] = "Microsoft Windows Millennium Edition";
		} 
		break;

	case VER_PLATFORM_WIN32s:

		m_mapOS[ "Name" ] = "Microsoft Win32s";
		break;
	}
}

void CSysInfo::GetCpuInfo (void)
{
//=======================================================================================================	
// 2005. 06. 15 kjhpower    dxdiag에서 정보를 얻어오는 관계로 주석 처리.	
//=======================================================================================================
	/*****************************************
	 * CPU Speed
	 */
	//unsigned long lCpuSpeed = 0;
	//
	//lCpuSpeed = GetCpuNormSpeed (0);
	//if(lCpuSpeed == 0)
	//	lCpuSpeed = ProcSpeedCalc ();

	//char szCpuSpeed[ 16 ];
	//sprintf (szCpuSpeed, "%u", lCpuSpeed);
	//m_mapCPU[ "speed" ] = szCpuSpeed;


	//CPUInfo CpuInfo;
	//m_mapCPU[ "vendor" ] = CpuInfo.GetVendorString ();
	
	m_mapCPU[ "CPUInfo" ] = m_system->m_szProcessorEnglish; // cpu 정보.
}

void CSysInfo::GetVideoInfo (void)
{
//=======================================================================================================	
// 2005. 06. 15 kjhpower    dxdiag에서 정보를 얻어오는 관계로 주석 처리.	
//=======================================================================================================
//	std::string strDxVersion = GetDxVersion ();
//	string directxversion = (string)m_system->m_szDirectXVersionEnglish;
//	m_mapVideo[ "DirectX Version" ] = strDxVersion + "(" + directxversion + ")";

	m_mapVideo[ "GraphicCardMemory" ] = (string)m_display1.m_szDisplayMemoryLocalized; // 그래픽카드 메모리 정보.
	m_mapVideo[ "DisplayMode" ] = (string)m_display1.m_szDisplayModeLocalized; // 현재 디스플레이 정보.
	m_mapVideo[ "DirectX Version" ] = (string)m_system->m_szDirectXVersionLongEnglish; // 다이렉트 X 버젼 정보.
}

bool CSysInfo::GetDXInfo (void)
{
//	std::string strDxVersion = GetDxVersion (); // <-- 2005.06.15 kjhpower 수정 이전에 주석처리되있었음.

//=======================================================================================================	
// 2005. 06. 15 kjhpower    dxdiag에서 정보를 얻어오는 관계로 주석 처리.	
//=======================================================================================================
/*	
	DWORD version_n;

    version_n=GetDxVersion2();
	
	if(version_n<0x00090002)
    return FALSE;
	else 
*/
    return TRUE;

}

bool IsRightDisk (const char * szName)
{
	UINT type = GetDriveType (szName); // "C:"

	switch(type){
		case DRIVE_UNKNOWN :     // The drive type cannot be determined.
			break;
		case DRIVE_NO_ROOT_DIR : // The root directory does not exist.
			break;
		case DRIVE_REMOVABLE :   // The disk can be removed from the drive.
			break;
		case DRIVE_FIXED :       // The disk cannot be removed from the drive.
			break;
		case DRIVE_REMOTE :      // The drive is a remote (network) drive.
			break;
		case DRIVE_CDROM :       // The drive is a CD-ROM drive.
			break;
		case DRIVE_RAMDISK :     // The drive is a RAM disk.
			break;
	}

	return false;
}


void CSysInfo::GetDiskInfo (void)
{
	// 드라이브명을 가지고 옴
	int		curdrive;
	char	path[ _MAX_PATH ];
	
	int iDriveCNT = 0;

	/* Save current drive. */
	curdrive = _getdrive ();

	char cInstallDrv = curdrive + 'A' - 1;

	_chdrive (curdrive);
	path[ 0 ] = '\0';
	_getdcwd (curdrive, path, _MAX_PATH);

	m_mapDisc[ "Current Folder" ] = path;

	typedef BOOL (*FN_GetDiskFreeSpaceEx) (LPSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
	FN_GetDiskFreeSpaceEx pGetDiskFreeSpaceEx = (FN_GetDiskFreeSpaceEx)GetProcAddress( GetModuleHandle("kernel32.dll"),
                         "GetDiskFreeSpaceExA");

	ULARGE_INTEGER i64FreeBytesToCaller;
	ULARGE_INTEGER i64TotalBytes;
	ULARGE_INTEGER i64FreeBytes;
	DWORD dwMB = 1024 * 1024;
	
	int iFreeMSize;
	int iTotalSize;
	
	iFreeMSize = 0;
	iTotalSize = 0;

	char szDrive[ 8 ];
	sprintf ( szDrive, "%c:", cInstallDrv);
	if (pGetDiskFreeSpaceEx)
	{
		BOOL fResult = GetDiskFreeSpaceEx (szDrive,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,
			(PULARGE_INTEGER)&i64FreeBytes					
			);
		if(fResult)
		{
			iFreeMSize = (int)(i64FreeBytesToCaller.QuadPart / (1024 * 1024));
			iTotalSize = (int)(i64TotalBytes.QuadPart / (1024 * 1024));
		}
	}
	else 
	{
		DWORD dwSectPerClust;
		DWORD dwBytesPerSect;
		DWORD dwFreeClusters;
		DWORD dwTotalClusters;

		BOOL fResult = GetDiskFreeSpace (szDrive, 
			&dwSectPerClust, 
			&dwBytesPerSect,
			&dwFreeClusters, 
			&dwTotalClusters);

		if(fResult)
		{
			iFreeMSize = (int)(( dwFreeClusters * dwSectPerClust * dwBytesPerSect ) / (1024 * 1024));
			iTotalSize = (int)(( dwTotalClusters * dwSectPerClust * dwBytesPerSect ) / (1024 * 1024));
		}
	}

	char szSize[ 16 ];
	char szKey[ 32 ]; 
	sprintf (szSize, "%dMB", iFreeMSize);
	sprintf (szKey, "Free Space of %c:", cInstallDrv);
	m_mapDisc[ szKey ] = szSize;
	sprintf (szSize, "%dMB", iTotalSize);
	sprintf (szKey, "Total Space of %c:", cInstallDrv);
	m_mapDisc[ szKey ] = szSize;	
}

#define DIV			(1024 * 1024)
#define WIDTH		7
void CSysInfo::GetMemInfo (void)
{
	char *divisor = "M";

	char buff[ 255 ];
	MEMORYSTATUS stat;

	stat.dwLength = sizeof (stat);

	GlobalMemoryStatus (&stat);

	// 물리 메모리
	sprintf (buff, "%ld MB"
			, stat.dwTotalPhys / DIV
			);
	m_mapMem[ "Total Phys Memory" ] = buff;

	sprintf (buff, "%ld MB"
			, stat.dwAvailPhys / DIV
			);
	m_mapMem[ "Free Phys Memory" ] = buff;


	// 페이지 파일
	sprintf (buff, "%ld MB"
			, stat.dwTotalPageFile / DIV
			);
	m_mapMem[ "Total Page File" ] = buff;

	sprintf (buff, "%ld MB"
			, stat.dwAvailPageFile / DIV
			);
	m_mapMem[ "Free Page File" ] = buff;


	// 가상 메모리
	sprintf (buff, "%ld MB"
			, stat.dwTotalVirtual / DIV
			);
	m_mapMem[ "Total Virtual Memory" ] = buff;

	sprintf (buff, "%ld MB"
			, stat.dwAvailVirtual / DIV
			);
	m_mapMem[ "Free Virtual Memory" ] = buff;
}


std::string CSysInfo::GetAllInfo (void)
{
	struct sInfo 
	{
		map<string, string> *	pMap;
		string					Part;
	};

	sInfo Infos[ ] = {	{&m_mapCPU	, "CPU"		}, 
						{&m_mapOS	, "OS"		},
						{&m_mapVideo, "Video"	},
						{&m_mapDisc	, "Disk"	},
						{&m_mapMem	, "Memory"} };
	std::map<string, string>::iterator im;

	GetCpuInfo ();
	GetDiskInfo ();
	GetOSInfo ();
	GetVideoInfo ();
	GetMemInfo ();

	string strInfo;
	for(int i = 0; i < sizeof (Infos) / sizeof (sInfo); i++)
	{
		strInfo += "[ ";
		strInfo += Infos[ i ].Part + " ]\n";
		for(im = Infos[ i ].pMap->begin (); im != Infos[ i ].pMap->end (); im++)
		{
			strInfo += im->first;
			strInfo += " : ";
			strInfo += im->second;
			strInfo += "\n";
		}
	}

	return strInfo;
}

std::string CSysInfo::MakeWebString (void)
{
	struct sInfo 
	{
		map<string, string> *	pMap;
		string					Part;
	};

	sInfo Infos[ ] = {	{&m_mapCPU	, "CPU"		}, 
						{&m_mapOS	, "OS"		},
						{&m_mapVideo, "Video"	},
						{&m_mapDisc	, "Disk"	},
						{&m_mapMem	, "Memory"} };
	std::map<string, string>::iterator im;

	string strInfo;
	for(int i = 0; i < sizeof (Infos) / sizeof (sInfo); i++)
	{
		strInfo += "&";
		strInfo += Infos[ i ].Part + "=";
		for(im = Infos[ i ].pMap->begin (); im != Infos[ i ].pMap->end (); im++)
		{
			strInfo += im->first;
			strInfo += " : ";
			strInfo += im->second;
			strInfo += ",";
		}
	}

	return strInfo;
}

