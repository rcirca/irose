/** 
 * @file zz_event.cpp
 * @brief Event class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-dec-2003
 *
 * $Header: /engine/src/zz_event.cpp 3     04-02-11 2:05p Zho $
 * $History: zz_event.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-15   Time: 8:56p
 * Updated in $/engine/src
 * converted zz_vfs_reader to zz_vfs_thread
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-14   Time: 1:58p
 * Created in $/engine/src
 * added zz_thread and zz_event class and refined zz_vfs_thread class.
 */

#include "zz_tier0.h"
#include "zz_event.h"

#ifdef WIN32
zz_event::zz_event ()
{
	handle_ = ::CreateEvent(NULL, TRUE /* manualreset */, FALSE /* initial state */, NULL /* name */);
}

bool zz_event::set ()
{
	return (::SetEvent(handle_) == TRUE);
}

bool zz_event::reset ()
{
	return (::ResetEvent(handle_) == TRUE);
}
#endif // WIN32
