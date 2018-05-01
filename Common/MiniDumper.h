
#ifndef __MINIDUMPER_H__
#define __MINIDUMPER_H__

#include <windows.h>

class MiniDumper
{
public:
	enum DumpLevel
	{
		DUMP_LEVEL_0,
		DUMP_LEVEL_1,
		DUMP_LEVEL_2
	};


private:
	static DumpLevel s_DumpLevel;            
	static bool s_bAddTimeStamp;
	static TCHAR s_szAppName[_MAX_PATH];
	static TCHAR s_szFaultReason[2048];


public:
	MiniDumper();
	MiniDumper(DumpLevel DL, bool bAddTimestamp=true);

	~MiniDumper();

	void BreakMe();


private:
	static LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtrs);
	static LPCTSTR GetFaultReason(struct _EXCEPTION_POINTERS* pExPtrs);
};

#endif //__MINIDUMPER_H__