/** 
 * @file zz_timer.cpp
 * @brief timer class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    20-jun-2002
 *
 * $Header: /engine/src/zz_timer.cpp 8     04-09-18 4:45p Zho $
 */

#include <assert.h>

#include "zz_tier0.h"
#include "zz_timer.h"
#include "zz_profiler.h"
#include "zz_system.h"

zz_timer::zz_timer(void) : running(false)
{
}

zz_timer::~zz_timer(void)
{
}

void zz_timer::start (void)
{
	zz_os::get_ticks(last_ticks);
	last_time = 0;
	compare_time = 0;
	compare_safetime = 0;
	last_ticks_excess = 0;

	running = true;
	zz_os::get_ticks_per_second(ticks_per_second);
	initial_ticks_per_second = ticks_per_second;
	
	zz_os::get_system_time(start_time_filetime);

	//ZZ_LOG("timer: timer_started. start_time(%ld)\n", start_time);
}

void zz_timer::stop (void)
{
	running = false;
}

void zz_timer::reset (void)
{
	start();
}

zz_time zz_timer::get_time ()
{
	if (!running) return 0;

	zz_os::get_ticks(new_ticks);

	// caution: engine ticks is not same as timer ticks
	const int ENGINE_TICKS_PER_SEC = ZZ_TICK_PER_SEC; // this is for engine ticks

	assert(ticks_per_second > 0);

	zz_time new_time = last_time + zz_time((new_ticks - last_ticks)*ENGINE_TICKS_PER_SEC/ticks_per_second);

	last_ticks_excess = new_ticks - last_ticks;
	uint64 time_diff_ticks = ticks_per_second*uint64(new_time - last_time)/ENGINE_TICKS_PER_SEC;
	
	assert(last_ticks_excess >= time_diff_ticks);
	last_ticks_excess -= time_diff_ticks;

	assert(new_time >= last_time);

	assert(new_ticks >= last_ticks_excess);
	last_ticks = new_ticks - last_ticks_excess;
	last_time = new_time;

	// time check
	if ((new_time - compare_time) > ENGINE_TICKS_PER_SEC) { // elapsed 1 sec
		sync_to_safe_time(new_time);
	}

	return new_time;
}

void zz_timer::sync_to_safe_time (const zz_time& new_time)
{
	zz_time new_safetime = get_safe_time();

	if (new_safetime == compare_safetime) return; // no need to sync

	const double ZZ_SCALE_TICKS_PER_SECOND = .1;
	uint64 max_ticks_per_second = 4*initial_ticks_per_second;
	uint64 min_ticks_per_second = initial_ticks_per_second/4;

	const zz_time ZZ_ERROR_TIME = ZZ_TICK_PER_SEC / 2; // .5 sec error

	if (new_safetime < ZZ_ERROR_TIME) return; // skip

	if (new_time > (new_safetime + ZZ_ERROR_TIME)) { // too fast
		//ZZ_LOG("timer: too fast = %ld\n", new_time - new_safetime); // zhotest
		ticks_per_second += 
			uint64(initial_ticks_per_second * ZZ_SCALE_TICKS_PER_SECOND *
			(1.0 - ((new_safetime - compare_safetime) / (new_time - compare_safetime))));
	}
	else if (new_time < (new_safetime - ZZ_ERROR_TIME)) { // too slow
		//ZZ_LOG("timer: too slow = %ld\n", new_safetime - new_time); // zhotest
		ticks_per_second -= 
			uint64(initial_ticks_per_second * ZZ_SCALE_TICKS_PER_SECOND*
			(1.0 - ((new_time - compare_time) / (new_safetime - compare_safetime))));
	}

	if (ticks_per_second > max_ticks_per_second) {
		//znzin->set_diff_time(new_time - new_safetime);
		ticks_per_second = max_ticks_per_second;
	}
	else if (ticks_per_second < min_ticks_per_second) {
		//znzin->set_diff_time(new_safetime - new_time);
		ticks_per_second = min_ticks_per_second;
	}

	compare_time = new_time;
	compare_safetime = new_safetime;
}

zz_time zz_timer::get_safe_time ()
{
	if (!running) return 0;
	uint64 finish_time_in_filetime;
	zz_os::get_system_time(finish_time_in_filetime);

	/*
	1 sec = 1,000,000,000 nano-sec
	1 sec =      10,000,000 hundred-nano-sec
	1 msec =          10,000 hundred-nano-sec
	*/
	uint64 diff = finish_time_in_filetime - start_time_filetime; // in 100-nanosecs
	const uint64 HUNDREDNANOSEC_PER_SEC = 10000000;
	zz_time t = zz_time(diff * ZZ_TICK_PER_SEC / HUNDREDNANOSEC_PER_SEC);

	return t;
}
