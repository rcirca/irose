#ifndef __SYSINFO_H__
#define __SYSINFO_H__

#include <map>
#include <string>
#include "dispinfo.h"
#include "systeminfo.h"
using namespace std;

static LPSTINFO m_system;
static DPINFO m_display1;

class CSysInfo
{
private:
	map<string, string> m_mapCPU;
	map<string, string> m_mapOS;
	map<string, string> m_mapVideo;
	map<string, string> m_mapDisc;
	map<string, string> m_mapMem;
	vector<DisplayInfo*> m_DisplayInfo;
	LPSTINFO  m_SystemInfo;

	// HWND m_hWnd;
private:
	void GetOSInfo (void);
	void GetCpuInfo (void);
	void GetVideoInfo (void);
	void GetDiskInfo (void); // 
	void GetMemInfo (void); // 
	
	
public:
	CSysInfo ();
	~CSysInfo ();
	std::string GetAllInfo (void);
	std::string MakeWebString (void);
    bool GetDXInfo();

};

#endif