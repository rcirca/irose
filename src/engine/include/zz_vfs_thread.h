/** 
 * @file zz_vfs_thread.h
 * @brief file pre loader class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-dec-2003
 *
 * $Header: /engine/include/zz_vfs_thread.h 22    04-03-10 10:42a Zho $
 * $History: zz_vfs_thread.h $
 * 
 * *****************  Version 22  *****************
 * User: Zho          Date: 04-03-10   Time: 10:42a
 * Updated in $/engine/include
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 04-03-05   Time: 6:06p
 * Updated in $/engine/include
 * 
 * *****************  Version 20  *****************
 * User: Zho          Date: 04-03-05   Time: 2:08p
 * Updated in $/engine/include
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 04-03-05   Time: 1:40p
 * Updated in $/engine/include
 * Redesigned vfs_thread.
 * 
 * *****************  Version 18  *****************
 * User: Zho          Date: 04-02-29   Time: 11:28p
 * Updated in $/engine/include
 * 
 * *****************  Version 17  *****************
 * User: Zho          Date: 04-02-27   Time: 6:16p
 * Updated in $/engine/include
 * Separated worker thread's TriggerVFS handle and main thread's
 * TriggerVFS handle because of file reading bug.
 * 
 * *****************  Version 16  *****************
 * User: Zho          Date: 04-02-23   Time: 3:51p
 * Updated in $/engine/include
 * Moved create_buffer_and_read_file() from public: to protected:.
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-02-22   Time: 6:44p
 * Updated in $/engine/include
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-02-21   Time: 6:46p
 * Updated in $/engine/include
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-02-20   Time: 4:42p
 * Updated in $/engine/include
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-02-20   Time: 12:24p
 * Updated in $/engine/include
 * Added statistics in debug mode.
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-02-12   Time: 7:08p
 * Updated in $/engine/include
 * Removed get_size().
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/include
 * Added zz_tier0.h
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-02-10   Time: 6:22p
 * Updated in $/engine/include
 * items_ holds only pointer of zz_item.
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-02-01   Time: 5:53p
 * Updated in $/engine/include
 * A Bug fixed. In push_back(zz_item), item's filename(zz_string) may be
 * reset. Unfortunately, if other thread is working with the filename
 * pointer had taken before string was reset, the filename is not a valid
 * name. It could cause a hang.
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-01-29   Time: 3:10p
 * Updated in $/engine/include
 * To use single critical section for all items.
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-01-17   Time: 2:12p
 * Updated in $/engine/include
 * More bug-free thread code.
 * Not limited by the number of items.
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-01-16   Time: 11:38p
 * Updated in $/engine/include
 * before converting to use std:vector with zz_item items_[MAX_ITEMS]
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-27   Time: 12:49p
 * Updated in $/engine/include
 * code clean up
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-24   Time: 5:40p
 * Updated in $/engine/include
 * added item's reference count
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-22   Time: 11:08a
 * Updated in $/engine/include
 * Some comments added.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-15   Time: 8:56p
 * Created in $/engine/include
 * converted zz_vfs_reader to zz_vfs_thread
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-14   Time: 1:58p
 * Updated in $/engine/include
 * added zz_thread and zz_event class and refined zz_vfs_thread class.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-13   Time: 2:28p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-10   Time: 5:19p
 * Updated in $/engine/include
 * forced terminate worker thread and clear buffer
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/include
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-07   Time: 5:32p
 * Created in $/engine/include
 * */

#ifndef __ZZ_VFS_THREAD_H__
#define __ZZ_VFS_THREAD_H__

#ifndef ZZ_IGNORE_TRIGGERVFS

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#ifndef __ZZ_HASH_TABLE_H__
#include "zz_hash_table.h"
#endif

#ifndef __ZZ_CRITICAL_SECTION_H__
#include "zz_critical_section.h"
#endif

#ifndef __ZZ_EVENT_H__
#include "zz_event.h"
#endif

#ifndef __ZZ_VFS_H__
#include "zz_vfs.h"
#endif

#ifndef __ZZ_THREAD_H__
#include "zz_thread.h"
#endif

#ifndef __ZZ_STRING_H__
#include "zz_string.h"
#endif

#ifndef __ZZ_AUTOLOCK_H__
#include "zz_autolock.h"
#endif

#ifndef __ZZ_PATH_H__
#include "zz_path.h"
#endif

#include <windows.h>
#include <process.h>

#include <vector> // for get_snapshot
#include <string> // for get_snapshot

class zz_vfs_pkg_system;

class zz_vfs_thread : public zz_thread {
private:
	zz_critical_section cs_; // Critical section object to be syncronized with this item
	zz_vfs_pkg_system * worker_pkg_system_; // worker thread's file system

	typedef LONG LOCKTYPE;
	LOCKTYPE terminate_worker_; // If set, worker thread can exit
	
	zz_event event_; // Event

	enum zz_state {
		STATE_FILL = 0, // To fill the buffer
		STATE_READ = 1, // To read the buffer
		STATE_DEST = 2, // To destroy the buffer
	};

public:
	//--------------------------------------------------------------------------------
	// zz_item :
	//
	// The minimum file item to be read independantly.
	//--------------------------------------------------------------------------------
	struct zz_item {
		zz_state state; // Current state
		zz_string filename; // File name
		void * buffer; // Data buffer pointer to store file content. be allocated and freed in thread function
		uint32 filesize; // Size in byte of this file
		unsigned int refcount; // Reference count maintained by open/close()
		
		zz_item (const char * filename_in = NULL) : state(STATE_FILL), buffer(NULL), filesize(0), refcount(0), filename(filename_in)
		{
		}

		~zz_item ()
		{
			assert(!buffer);
		}
	};

	// Predefined item iterator.
	typedef zz_hash_table<zz_item * >::iterator zz_item_it;

private:
	zz_hash_table <zz_item *> items_; // Item container
	zz_item_it current_; // Current item iterator.

	// Set interlocked object.
	void lockset_ (LOCKTYPE& tolock, LOCKTYPE value);

	// Clear all items.
	void clear_all_ ();

	/// Create buffer and read file into data_out, and returns the number of byte read
	/// You should free the buffer pointer after using by yourself.
	/// You do not need to close file.
	/// @filename filename to open
	/// @psize size to get
	/// @return buffer pointer created
	void * create_buffer_and_read_file_  (const char * filename, uint32 * psize);

public:
	zz_vfs_thread ();
	~zz_vfs_thread ();

	// Thread procedure
	unsigned thread_proc (void);

	/// Check handle
	/// @return true if it is valid
	bool is_valid_handle (zz_item_it handle);

	/// Request to read the file with filename.
	/// @filename filananme to open.
	/// @return request-handle. If open fails, the return value is -1.
	zz_item_it open (const char * filename);
	
	/// Get the file handle from filename.
	/// @filename filename to find.
	/// @return file handle. If no matching was found, the return value is -1(invalid handle).
	zz_item_it find (const char * filename);

	/// Checks if the requested reading has been finished, and then returns the buffer pointer.
	/// @forcing if true, read() is always succeeded.
	/// @psize read size
	/// @return new allocated pointer. If the method fails, the return value is NULL.
	void * read (zz_item_it handle, uint32 * psize = NULL, bool forcing = false);

	/// Release buffer pointer.
	/// @handle file handle to close
	/// @return true if suceeded.
	bool close (zz_item_it handle);

	/// Suspend worker thread
	void lock ();

	/// Resume worker thread
	void unlock ();

	/// Set worker thead file system.
	/// This filesystem is only for worker thread.
	/// This will be set by znzin
	bool set_worker_pkg_system (zz_vfs_pkg_system * worker_pkg_system_in);

	/// For statistics
	void get_num_items (int& num_fill, int& num_total);
};

inline bool zz_vfs_thread::set_worker_pkg_system (zz_vfs_pkg_system * worker_pkg_system_in)
{
	if (worker_pkg_system_) return false;
	worker_pkg_system_ = worker_pkg_system_in;
	return true;
}

inline bool zz_vfs_thread::is_valid_handle (zz_item_it handle)
{
	zz_autolock<zz_critical_section> locked(cs_);
	return (handle != items_.end());
}

inline void zz_vfs_thread::lockset_ (LOCKTYPE& tolock, LOCKTYPE value)
{
	InterlockedExchange(&tolock, value);
}

inline zz_vfs_thread::zz_item_it zz_vfs_thread::find (const char * filename_in)
{
	zz_autolock<zz_critical_section> locked(cs_);

	if (!filename_in) return items_.end();

	return items_.find(filename_in);
}

inline void zz_vfs_thread::lock ()
{
	//zz_autolock<zz_critical_section> locked(cs_);
	thread_suspend();
}

inline void zz_vfs_thread::unlock ()
{
	//zz_autolock<zz_critical_section> locked(cs_);
	thread_resume();
}

#endif // ZZ_IGNORE_TRIGGERVFS
#endif // __ZZ_VFS_THREAD_H__
