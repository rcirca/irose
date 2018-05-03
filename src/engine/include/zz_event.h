/** 
 * @file zz_event.h
 * @brief Event class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-dec-2003
 *
 * $Header: /engine/include/zz_event.h 2     03-12-15 8:56p Zho $
 * $History: zz_event.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-15   Time: 8:56p
 * Updated in $/engine/include
 * converted zz_vfs_reader to zz_vfs_thread
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-14   Time: 1:58p
 * Created in $/engine/include
 * added zz_thread and zz_event class and refined zz_vfs_thread class.
 */

#ifndef __ZZ_EVENT_H__
#define __ZZ_EVENT_H__

#ifndef __ZZ_OSKERNEL_H__
#include "zz_oskernel.h"
#endif

class zz_event : public zz_oskernel {
public:
	zz_event ();
	bool set ();
	bool reset ();
};

#endif // __ZZ_EVENT_H__