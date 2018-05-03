/** 
 * @file zz_thread.cpp
 * @brief thread class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    13-dec-2003
 *
 * $Header: /engine/src/zz_thread.cpp 7     04-04-25 1:19p Zho $
 * $History: zz_thread.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-04-25   Time: 1:19p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-03-10   Time: 10:42a
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-17   Time: 2:09p
 * Updated in $/engine/src
 * Removed calling virtual function in constructor because of pure virtual
 * function call problem.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-15   Time: 8:56p
 * Updated in $/engine/src
 * converted zz_vfs_reader to zz_vfs_thread
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-14   Time: 1:58p
 * Updated in $/engine/src
 * added zz_thread and zz_event class and refined zz_vfs_thread class.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-13   Time: 3:54p
 * Created in $/engine/src
 * oknernel and thread class added
 */

#include "zz_tier0.h"

#include "zz_thread.h"

#include <assert.h>

//#define ZZ_MULTI_THREAD

zz_thread::zz_thread ()
{
}

unsigned _stdcall zz_thread::call_thread_proc (void * thread_pointer)
{
	zz_thread * thread = static_cast<zz_thread *>(thread_pointer);
	
	if (!thread) return 0;

	return thread->thread_proc();
}

bool zz_thread::thread_start ()
{
#ifdef ZZ_MULTI_THREAD
	if (is_valid_handle(handle_)) return true;

	handle_ = (HANDLE)_beginthreadex(NULL, 0, call_thread_proc , (void*)this, 0, (unsigned int*)&thread_id_);
	assert(is_valid_handle(handle_));
	return is_valid_handle(handle_);
#else
	return false;
#endif
}

bool zz_thread::thread_suspend ()
{
#ifdef ZZ_MULTI_THREAD
	if (!is_valid_handle(handle_)) return false; // not started
	return (::SuspendThread(handle_) != ((DWORD)-1));
#else
	return false;
#endif
}

bool zz_thread::thread_resume () {
#ifdef ZZ_MULTI_THREAD
	if (!is_valid_handle(handle_)) return false; // not started
	return (::ResumeThread(handle_) != ((DWORD)-1));
#else
	return false;
#endif
}

bool zz_thread::set_thread_priority (zz_priority priority)
{
#ifdef ZZ_MULTI_THREAD
	return (::SetThreadPriority(handle_, static_cast<int>(priority)) == TRUE);
#else
	return false;
#endif
}

zz_thread::zz_priority zz_thread::get_thread_priority ()
{
#ifdef ZZ_MULTI_THREAD
	return static_cast<zz_priority>(::GetThreadPriority(handle_));
#else
	return NORMAL;
#endif
}

