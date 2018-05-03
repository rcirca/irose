/** 
 * @file zz_oskernal.h
 * @brief OS kernel object class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    13-dec-2002
 *
 * $Header: /engine/include/zz_oskernel.h 3     03-12-15 8:56p Zho $
 * $History: zz_oskernel.h $
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

#ifndef __OS_KERNAL_H__
#define __OS_KERNAL_H__

#include "zz_type.h"

#ifdef WIN32
//--------------------------------------------------------------------------------
// thread object base class
// from book "Win32 multithreaded programming" by Aaron Michael Cohen, Mike Woodling, O'reilly
//--------------------------------------------------------------------------------
class zz_oskernel {
protected:
	HANDLE handle_;

	zz_oskernel () : handle_(NULL) {}

public:
	virtual ~zz_oskernel () = 0
	{
		if (is_valid_handle(handle_)) {
			::CloseHandle(handle_);
			handle_ = NULL;
		}
	}

	bool is_valid_handle (HANDLE handle) const
	{
		return (handle && (handle != INVALID_HANDLE_VALUE));
	}

	// returns false if it is timed out or abanded.
	bool wait ( dword milliseconds = INFINITE)
	{
		return (::WaitForSingleObject(handle_, milliseconds) == WAIT_OBJECT_0);
	}

	HANDLE get_handle () const
	{
		if (this == NULL)
			return NULL;
		return handle_;
	}

	operator HANDLE() const
	{
		return get_handle();
	}

	// wait for this object and other object
	bool wait_for_two ( zz_oskernel * other, dword milliseconds )
	{
		HANDLE handles[2];
		handles[0] = handle_;
		handles[1] = other->get_handle();

		DWORD ret = ::WaitForMultipleObjects(2, handles, true /* waitall */, milliseconds);

		return ((WAIT_OBJECT_0 <= ret) && (ret <= WAIT_OBJECT_0 + 1));
	}
};

#endif // WIN32

#endif // __OS_KERNAL_H__