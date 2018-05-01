/** 
 * @file zz_timer.h
 * @brief timer class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    20-jun-2002
 *
 * $Header: /engine/include/zz_timer.h 2     04-09-06 3:09p Zho $
 * $History: zz_timer.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-09-06   Time: 3:09p
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef	__ZZ_TIMER_H__
#define __ZZ_TIMER_H__

#include <time.h>
#include <zz_type.h>

// 1 second = ZZ_TICK_PER_SEC timer unit (ZZ_TICK_PER_SEC is defined in zz_type.h)
// timer class is used only in zz_system class
class zz_system;
class zz_timer {
private:
	uint64 ticks_per_second; // in engine ticks
	uint64 initial_ticks_per_second;

	// real private members
	uint64 start_time_filetime; // safe systemtimer in filetime unit(100-nanosecs)
	uint64 last_ticks; // last timer ticks. this is updated in every get_time()
	uint64 new_ticks; // new time ticks
	uint64 last_ticks_excess;
	zz_time last_time; // last engine ticks. this is updated in every get_time()
	zz_time compare_time; // time to compare the difference with system timer in 1 sec
	zz_time compare_safetime; // filetime for compare
	bool running; // whether time is running or not

	zz_time get_safe_time (); // get safe system time in engine ticks
	void sync_to_safe_time (const zz_time& new_time); // synchronize to system safe timer by changing the ticks_per_second


public:
	zz_timer(void);
	~zz_timer(void);
	void start (void);
	void stop (void);
	void reset (void);

	bool is_running ()
	{
		return running;
	}

	zz_time get_time (void);
};

#endif // __ZZ_TIMER_H__