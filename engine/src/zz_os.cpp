/** 
 * @file zz_os.cpp
 * @brief operation system related
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/src/zz_os.cpp 8     06-10-17 10:57a Choo0219 $
 * $History: zz_os.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Choo0219     Date: 06-10-17   Time: 10:57a
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Choo0219     Date: 05-02-02   Time: 4:12p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-09-06   Time: 3:09p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-07-07   Time: 12:51a
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-28   Time: 5:44p
 * Updated in $/engine/src
 * riva tnt2 enabled.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-28   Time: 2:15p
 * Updated in $/engine/src
 * more bug-findable version.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_os.h"
#include "zz_log.h"

#ifdef WIN32	
#include <windows.h>
#include <winbase.h>

zz_os_counter_type g_counter_type = TIMER_RDTSC;

bool g_counter_initialized = false;

void (*g_get_ticks_ptr) (uint64& clock_time) = NULL;

uint64 g_ticks_per_second = 1000; // default

#pragma comment (lib, "Winmm.lib") // for timeGetTime()

void zz_os::initialize ()
{
	if (g_counter_initialized) return; // do only once

	char * counter_string = NULL;

	if (false){ //init_rdtsc()) { // try rdtsc and initialize
		g_counter_type = TIMER_RDTSC;
		g_get_ticks_ptr = zz_os::get_ticks_rdtsc;
		counter_string = "rdtsc";
	}
	else if (false/*init_qpc()*/) { // try qpc and initialize
		g_counter_type = TIMER_QPC;
		g_get_ticks_ptr = zz_os::get_ticks_qpc;
		counter_string = "qpc";
	}
	else { // use timeGetTime   //AMD 듀얼 코어 문제로 인한 시간 변경
		// none of fast methods are available.
		g_counter_type = TIMER_TIMEGETTIME;
		g_get_ticks_ptr = zz_os::get_ticks_tgt;
		g_ticks_per_second = 1000; // timeGetTime() uses millisecond
		counter_string = "tgt";
	}
	g_counter_initialized = true;
	ZZ_LOG("os: %s supported.\n", counter_string);
}

void zz_os::get_system_time (uint64& systime)
{
	SYSTEMTIME st;
	FILETIME ft;

	GetSystemTime(&st); // gets current time
	SystemTimeToFileTime(&st, &ft); // convert to file format

	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;

	systime = static_cast<uint64>(ui.QuadPart);
}

// or faster method, but only available in MSVC
// I foudn i got better performance like this as a 64-bit is held in edx:eax anyway. 
// However this might be an MSVC only thing ...
// [REF] http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-RDTSCTimer&forum=cotd&id=-1
/*
__declspec(naked) __int64 GetCycleCount() 
{ 
    __asm 
    {
        rdstc
        ret
    }
}
*/

#endif // end of WIN32