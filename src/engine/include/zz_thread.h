/** 
 * @file zz_thread.h
 * @brief thread class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    13-dec-2003
 *
 * $Header: /engine/include/zz_thread.h 4     04-01-17 2:09p Zho $
 * $History: zz_thread.h $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-17   Time: 2:09p
 * Updated in $/engine/include
 * Removed calling virtual function in constructor because of pure virtual
 * function call problem.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-15   Time: 8:56p
 * Updated in $/engine/include
 * converted zz_vfs_reader to zz_vfs_thread
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-14   Time: 1:58p
 * Updated in $/engine/include
 * added zz_thread and zz_event class and refined zz_vfs_thread class.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-13   Time: 3:54p
 * Created in $/engine/include
 * oknernel and thread class added
 */

#ifndef __ZZ_THREAD_H__
#define __ZZ_THREAD_H__

// this class is currently windows-specific.
#ifdef WIN32
#include <windows.h>
#endif

#include <process.h>
#include "zz_oskernel.h"

class zz_thread : public zz_oskernel {
private:
	static unsigned _stdcall call_thread_proc (void * thread_pointer);

protected:
	unsigned int thread_id_;

public:
#ifdef WIN32
	enum zz_priority {
		ABOVE_NORMAL = THREAD_PRIORITY_ABOVE_NORMAL, // defined in winbase.h and winnt.h
		BELOW_NORMAL = THREAD_PRIORITY_BELOW_NORMAL,
		HIGHEST = THREAD_PRIORITY_HIGHEST,
		IDLE = THREAD_PRIORITY_IDLE,
		LOWEST = THREAD_PRIORITY_LOWEST,
		NORMAL = THREAD_PRIORITY_NORMAL,
		TIME_CRITICAL = THREAD_PRIORITY_TIME_CRITICAL,
	};
#endif

	zz_thread ();
	virtual ~zz_thread () {}

	bool thread_start ();
	bool thread_suspend ();
	bool thread_resume ();
	// bool terminate(); // not implemented yet for the safety issue
	bool set_thread_priority (zz_priority priority);
	zz_priority get_thread_priority ();
	virtual unsigned thread_proc (void) = 0;
};

#endif // __ZZ_THREAD_H__