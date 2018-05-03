/** 
 * @file zz_vfs_thread.cpp
 * @brief file pre loader class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-dec-2003
 *
 * $Header: /engine/src/zz_vfs_thread.cpp 34    04-03-10 10:42a Zho $
 * $History: zz_vfs_thread.cpp $
 * 
 * *****************  Version 34  *****************
 * User: Zho          Date: 04-03-10   Time: 10:42a
 * Updated in $/engine/src
 * 
 * *****************  Version 33  *****************
 * User: Zho          Date: 04-03-08   Time: 7:52p
 * Updated in $/engine/src
 * To use vfsgetdata()
 * 
 * *****************  Version 32  *****************
 * User: Zho          Date: 04-03-06   Time: 7:11p
 * Updated in $/engine/src
 * 
 * *****************  Version 31  *****************
 * User: Zho          Date: 04-03-05   Time: 6:06p
 * Updated in $/engine/src
 * 
 * *****************  Version 30  *****************
 * User: Zho          Date: 04-03-05   Time: 2:08p
 * Updated in $/engine/src
 * 
 * *****************  Version 29  *****************
 * User: Zho          Date: 04-03-05   Time: 1:40p
 * Updated in $/engine/src
 * Redesigned vfs_thread.
 * 
 * *****************  Version 28  *****************
 * User: Zho          Date: 04-02-29   Time: 11:28p
 * Updated in $/engine/src
 * 
 * *****************  Version 27  *****************
 * User: Zho          Date: 04-02-27   Time: 6:16p
 * Updated in $/engine/src
 * Separated worker thread's TriggerVFS handle and main thread's
 * TriggerVFS handle because of file reading bug.
 * 
 * *****************  Version 26  *****************
 * User: Zho          Date: 04-02-24   Time: 7:08p
 * Updated in $/engine/src
 * 
 * *****************  Version 25  *****************
 * User: Zho          Date: 04-02-23   Time: 3:51p
 * Updated in $/engine/src
 * Moved create_buffer_and_read_file() from public: to protected:.
 * 
 * *****************  Version 24  *****************
 * User: Zho          Date: 04-02-22   Time: 6:44p
 * Updated in $/engine/src
 * 
 * *****************  Version 23  *****************
 * User: Zho          Date: 04-02-21   Time: 6:46p
 * Updated in $/engine/src
 * 
 * *****************  Version 22  *****************
 * User: Zho          Date: 04-02-20   Time: 4:42p
 * Updated in $/engine/src
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 04-02-20   Time: 4:20p
 * Updated in $/engine/src
 * Some assertion added.
 * 
 * *****************  Version 20  *****************
 * User: Zho          Date: 04-02-20   Time: 12:24p
 * Updated in $/engine/src
 * Added statistics in debug mode.
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 04-02-12   Time: 7:08p
 * Updated in $/engine/src
 * Removed get_size().
 * 
 * *****************  Version 18  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 17  *****************
 * User: Zho          Date: 04-02-10   Time: 9:50p
 * Updated in $/engine/src
 * 
 * *****************  Version 16  *****************
 * User: Zho          Date: 04-02-10   Time: 9:40p
 * Updated in $/engine/src
 * impreved vfs_pkg read performance.
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-02-10   Time: 6:22p
 * Updated in $/engine/src
 * items_ holds only pointer of zz_item.
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-02-03   Time: 12:12p
 * Updated in $/engine/src
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-02-01   Time: 5:53p
 * Updated in $/engine/src
 * A Bug fixed. In push_back(zz_item), item's filename(zz_string) may be
 * reset. Unfortunately, if other thread is working with the filename
 * pointer had taken before string was reset, the filename is not a valid
 * name. It could cause a hang.
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-01-29   Time: 5:32p
 * Updated in $/engine/src
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-01-29   Time: 3:10p
 * Updated in $/engine/src
 * To use single critical section for all items.
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-01-27   Time: 11:31a
 * Updated in $/engine/src
 * Added macro as USE_VFS_THREAD_TEXTURE_LOADING.
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-01-17   Time: 3:39p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-01-17   Time: 2:12p
 * Updated in $/engine/src
 * More bug-free thread code.
 * Not limited by the number of items.
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-01-16   Time: 11:38p
 * Updated in $/engine/src
 * before converting to use std:vector with zz_item items_[MAX_ITEMS]
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-27   Time: 2:11p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-27   Time: 12:49p
 * Updated in $/engine/src
 * code clean up
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-26   Time: 6:03p
 * Updated in $/engine/src
 * in DEST, not to clear filename bug was fixed.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-24   Time: 5:40p
 * Updated in $/engine/src
 * added item's reference count
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-22   Time: 11:12a
 * Updated in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-15   Time: 8:56p
 * Created in $/engine/src
 * converted zz_vfs_reader to zz_vfs_thread
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-12-14   Time: 1:58p
 * Updated in $/engine/src
 * added zz_thread and zz_event class and refined zz_vfs_thread class.
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-12-13   Time: 2:28p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-10   Time: 11:48p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-10   Time: 9:01p
 * Updated in $/engine/src
 * open() bug fixed. did not returned openned index. new index was
 * returned.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-10   Time: 5:19p
 * Updated in $/engine/src
 * forced terminate worker thread and clear buffer
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-10   Time: 4:18p
 * Updated in $/engine/src
 * priority adjusted to "below normal"
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/src
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-07   Time: 5:32p
 * Created in $/engine/src
 * */

#ifndef ZZ_IGNORE_TRIGGERVFS

#include "zz_tier0.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <io.h>
#include <algorithm> // for get_snapshot
#include "zz_mem.h"
#include "zz_vfs_thread.h"
#include "zz_vfs_pkg.h"
#include "zz_autolock.h"
#include "zz_log.h"
#include "zz_type.h"
#include "zz_path.h"
#include "zz_system.h"
#include "zz_profiler.h"

// for debugging purpose
char state_string[3][5] = { "FILL", "READ", "DEST" };

unsigned zz_vfs_thread::thread_proc ()
{
	zz_item * item;
	int size;
	bool current_not_end = false;
	while (1)
	{
		{
			zz_autolock<zz_critical_section> locked(cs_);
			size = items_.size();
			current_ = items_.begin();
			current_not_end = (current_ != items_.end());
		}
		if (size == 0) {
			// We have nothing to do, take a rest until new job is arrived.
			event_.wait();  // Wait for event object.
			event_.reset(); // Not to skip in next time wait.
		}
		while (current_not_end)
		{
			{ // begining of the locking block
				zz_autolock<zz_critical_section> locked(cs_);

				if (current_ != items_.end()) { // Double check
					item = (*current_);
					
					switch (item->state)
					{
					case STATE_FILL:
						// Fill the buffer
						item->buffer = create_buffer_and_read_file_(item->filename.get(), &item->filesize);
						item->state = STATE_READ;
						break;
					case STATE_READ:
						// Do nothing
						break;
					}
				}
				++current_;
				current_not_end = (current_ != items_.end());
			} // end of the locking block
			::Sleep(200); // Return the context to the main thread.
		}
		if (terminate_worker_ != 0) { // If it's time to exit.
			return 0;
		}
	}
	return 1;
}

zz_vfs_thread::zz_vfs_thread () :
	worker_pkg_system_(NULL), terminate_worker_(0)
{
}

zz_vfs_thread::~zz_vfs_thread ()
{	
	lockset_(terminate_worker_, static_cast<LOCKTYPE>(true));

	event_.set(); // to wake the worker thread when it is waiting event.

	wait(); // wait until the worker thread is dead.
	// assure that the worker thread is terminated.
	clear_all_(); // clean up all remaining process
}

void zz_vfs_thread::clear_all_ ()
{
	zz_item_it it = items_.begin();
	while (it != items_.end())
	{
		// delete the buffer
		if ((*it)->buffer) {
			zz_delete [] (*it)->buffer;
			(*it)->buffer = NULL;
		}
		items_.erase(it);
		it = items_.begin();
	}
}

// Open file by filename, and returns the file handle.
zz_vfs_thread::zz_item_it zz_vfs_thread::open (const char * filename_in)
{
	zz_autolock<zz_critical_section> locked(cs_);

	// Check if we have already the same name.	
	zz_item_it it = items_.find(filename_in);

	if (it != items_.end()) { // If it was already opened,
		// Increase reference count
		(*it)->refcount++;
		// In this case, current empty handle is not modified. So, finding empty handle is not required.
		// And count-increasing and event-setting is not also required.
		return it;
	}

	// Lock and fill the content.
	it = items_.insert(filename_in, zz_new zz_item(filename_in));

	assert( it != items_.end() );
	event_.set();

	return it;
}

void * zz_vfs_thread::read (zz_item_it handle, uint32 * psize, bool forcing)
{
	if (!is_valid_handle(handle)) {
		return NULL;
	}

	zz_autolock<zz_critical_section> locked(cs_);

	zz_item * item = (*handle);

	if ((item->state != STATE_READ) && (forcing)) {
		// similar work as worker thread::STATE_FILL
		item->buffer = create_buffer_and_read_file_(item->filename.get(), &item->filesize);
		item->state = STATE_READ;
	}
	
	if (psize) {
		*psize = item->filesize;
	}
	return item->buffer;
}

// request the worker thread to free the buffer
bool zz_vfs_thread::close (zz_item_it handle)
{
	if (!is_valid_handle(handle)) {
		return false;
	}

	zz_autolock<zz_critical_section> locked(cs_);

	zz_item * item = (*handle);

	if (item->refcount > 0) { // Another one has this.
		item->refcount--;
		// do nothing
	}
	else { // This is the last one.
		// Destroy the buffer
		//item->state = STATE_DEST;
		zz_delete [] item->buffer;
		item->buffer = NULL;
		current_ = items_.erase(handle);
		zz_delete item;
	}
	return true;
}

// This call should be in locked state.
void * zz_vfs_thread::create_buffer_and_read_file_ (const char * filename, uint32 * psize)
{
	// The main thread uses main thread's pkg_system that is obtained by zz_system::get_pkg_system().
	// The worker thread uses its own pkg_system(also stored in zz_system)  in zz_vfs_thread.
	//ZZ_PROFILER_INSTALL(Pcreate_buffer_and_read_file);

	zz_vfs_pkg fs(worker_pkg_system_);
	
	*psize = fs.get_size(filename);

	//ZZ_LOG("vfs_thread: create_buffer_and_read_file(%s)=>%d\n", filename, *psize);
	
	if (!fs.open(filename)) {
		return NULL;
	}
	void * buf = zz_new char[*psize]; // The allocated memory must be deallocated in the caller.
	assert(buf);
	
	fs.read(reinterpret_cast<char*>(buf), *psize);

	fs.close();
	
	return buf;
}


void zz_vfs_thread::get_num_items (int& num_fill, int& num_total)
{
	zz_autolock<zz_critical_section> locked(cs_);
	num_fill = 0;
	num_total = 0;
	for (zz_item_it it = items_.begin(); it != items_.end(); ++it) {
		if ((*it)->state == STATE_FILL) {
			num_fill++;
		}
		num_total++;
	}
}

#endif // ZZ_IGNORE_TRIGGER_VFS