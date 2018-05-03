/** 
 * @file zz_critical_section.h
 * @brief critical section wrapper class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    07-dec-2003
 *
 * $Header: /engine/include/zz_critical_section.h 4     04-02-27 4:51p Zho $
 * $History: zz_critical_section.h $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-27   Time: 4:51p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-31   Time: 1:15p
 * Updated in $/engine/include
 * Let the member functions inline.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/include
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-08   Time: 10:21a
 * Created in $/engine/include
 **/

#ifndef __ZZ_CRITICAL_SECTION_H__
#define __ZZ_CRITICAL_SECTION_H__

#ifdef WIN32
#include <windows.h>
#endif

class zz_critical_section {
private:
#ifdef WIN32
	CRITICAL_SECTION csection_; // critical section object
#endif

public:
	zz_critical_section();
	~zz_critical_section();

	void enter ();
	void leave ();
};

#ifdef WIN32
inline zz_critical_section::zz_critical_section()
{
	InitializeCriticalSection(&csection_);
}

inline zz_critical_section::~zz_critical_section()
{
	DeleteCriticalSection(&csection_);
}

inline void zz_critical_section::enter ()
{
	EnterCriticalSection(&csection_);
}

inline void zz_critical_section::leave ()
{
	LeaveCriticalSection(&csection_);
}
#endif // WIN32

#endif //  __ZZ_CRITICAL_SECTION_H__