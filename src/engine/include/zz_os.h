/** 
* @file zz_os.h
* @brief OS(operation system) related functions
* @author Jiho Choi (zho@korea.com)
* @version 1.0
* @date    16-feb-2002
*
* $Header: /engine/include/zz_os.h 4     04-09-06 3:09p Zho $
*/

#ifndef	__ZZ_OS_H__
#define __ZZ_OS_H__

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif


enum zz_os_counter_type {
	TIMER_RDTSC, // rdtsc
	TIMER_QPC, // QueryPerformanceCounter
	TIMER_TIMEGETTIME, // timeGetTime
	TIMER_SYSTEM, // GetLocalTime
};

extern zz_os_counter_type g_counter_type;

extern bool g_counter_initialized;

extern void (*g_get_ticks_ptr) (uint64& clock_time);

extern uint64 g_ticks_per_second; // default

// each implementation file name is
// zz_os_win32.cpp : windows
// zz_os_linux.cpp : linux + Xwindows
namespace zz_os {

	// initialize thing
	void initialize ();

	// checks if support rdtsc
	inline bool check_rdtsc ()
	{
		__try {
			__asm {
				rdtsc
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			return false;
		}
		return true;
	}


	// checks if support qpc
	inline bool check_qpc ()
	{
		LARGE_INTEGER qwTicksPerSec;
		return QueryPerformanceFrequency(&qwTicksPerSec) ? true : false;
	}

	// get fast cpu clock ticks (using rdtsc)
	inline void get_ticks_rdtsc (uint64& ticks)
	{
		uint32 _low, _high;
		__asm {
			rdtsc
				mov _low, eax
				mov _high, edx
		}
		ticks = ((unsigned __int64)_high << 32) | (unsigned __int64)_low;
		// ticks = static_cast<long>(GetTickCount()); // slow version
	}

	// get ticks per second (using QueryPerformanceCounter)
	inline void get_ticks_qpc (uint64& ticks)
	{
		LARGE_INTEGER qwTime;
		QueryPerformanceCounter(&qwTime);
		ticks = static_cast<uint64>(qwTime.QuadPart);
	}

	// get timeGetTime
	inline void get_ticks_tgt (uint64& ticks)
	{
		ticks = static_cast<uint64>(timeGetTime());
	}

	// check and initialize rdtsc
	inline bool init_rdtsc ()
	{
		if (!check_rdtsc()) return false;

		uint64 start, end;
		get_ticks_rdtsc(start);
		::Sleep(1000); // to measure 1 second.
		get_ticks_rdtsc(end);
		g_ticks_per_second = end - start;
		return true;
	}

	// check and initialize qpc
	inline bool init_qpc ()
	{
		if (!check_qpc()) return false;
		LARGE_INTEGER qwTicksPerSec;

		QueryPerformanceFrequency(&qwTicksPerSec);
		g_ticks_per_second = qwTicksPerSec.QuadPart;
		return true;
	}

	// get ticks per second
	inline void get_ticks_per_second (uint64& ticks_per_second_in)
	{
		ticks_per_second_in = g_ticks_per_second;
	}

	// get ticks
	inline void get_ticks (uint64& ticks)
	{
		(*g_get_ticks_ptr)(ticks);
	}

	void get_system_time (uint64& systime); // get system time in 100-nanosecs since 1601/1/1

	//void win_init (int width, int height, int bpp, void bool fullscreen);
} // end of namespace

// usage :
// zz_os::get_ticks();

#endif // __ZZ_OS_H__
